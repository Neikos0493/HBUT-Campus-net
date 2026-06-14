#include "state_machine.h"
#include "auth_client.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

StateMachine::StateMachine() : currentState(NetState::DISCONNECTED), logger(100), online(false), failCount(0) {
    if (!config.loadFromFile("data/config.ini")) {
        std::cerr << "警告：配置文件加载失败，请先完成首次设置。" << std::endl;
    }
}

bool StateMachine::reloadConfig(const std::string& configPath) {
    return config.loadFromFile(configPath);
}

void StateMachine::restart() {
    failCount = 0;
    currentState = NetState::DISCONNECTED;
    logger.log("状态机已重置，准备重新认证");
}

void StateMachine::stop() {
    logger.log("程序正常退出");
}

void StateMachine::checkOnlineStatus() {
    int ret = system("ping -n 1 www.baidu.com > nul 2>&1");
    online = (ret == 0);
    if (!online) {
        logger.log("心跳检测失败，可能掉线");
    }
}

void StateMachine::handleDisconnected() {
    logger.log("状态: 未连接WiFi");
    logger.log("WiFi已连接");
    currentState = NetState::CONNECTED_NO_AUTH;
}

void StateMachine::handleConnectedNoAuth() {
    logger.log("状态: 已连接WiFi，需要认证");
    currentState = NetState::AUTHENTICATING;
}

void StateMachine::handleAuthenticating() {
    logger.log("正在认证...");
    AuthClient auth;
    std::string url = config.get("login_url");
    std::string username = config.get("username");
    std::string password = config.get("password");
    bool success = auth.login(username, password, url);

    if (success) {
        failCount = 0;
        logger.log("认证成功，已上线");
        currentState = NetState::ONLINE;
        online = true;
        timer.addTimer(5000, [this]() { this->checkOnlineStatus(); });
    } else {
        logger.log("认证失败");
        currentState = NetState::FAILED;
    }
}

void StateMachine::handleOnline() {
    timer.tick();
    if (!online) {
        logger.log("心跳检测失败，需要重新认证");
        currentState = NetState::DISCONNECTED;
    } else {
        if (timer.empty()) {
            timer.addTimer(5000, [this]() { this->checkOnlineStatus(); });
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void StateMachine::handleFailed() {
    failCount++;
    logger.log("认证失败 (" + std::to_string(failCount) + "/" + std::to_string(MAX_FAIL_COUNT) + ")");

    if (failCount >= MAX_FAIL_COUNT) {
        logger.log("连续失败达到上限，暂停 10 分钟");
        std::this_thread::sleep_for(std::chrono::minutes(10));
        failCount = 0;
    } else {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    currentState = NetState::DISCONNECTED;
}

void StateMachine::run() {
    logger.log("校园网自动认证系统启动");
    while (true) {
        switch (currentState) {
            case NetState::DISCONNECTED:       handleDisconnected();      break;
            case NetState::CONNECTED_NO_AUTH:  handleConnectedNoAuth();   break;
            case NetState::AUTHENTICATING:     handleAuthenticating();    break;
            case NetState::ONLINE:             handleOnline();            break;
            case NetState::FAILED:             handleFailed();            break;
        }
    }
}