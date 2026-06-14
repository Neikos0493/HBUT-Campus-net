#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include <string>
#include <functional>
#include "logger.h"
#include "timer.h"
#include "config_manager.h"

enum class NetState {
    DISCONNECTED,
    CONNECTED_NO_AUTH,
    AUTHENTICATING,
    ONLINE,
    FAILED
};

class StateMachine {
public:
    using StatusCallback = std::function<void(const std::wstring&, const std::wstring&)>;

    StateMachine();
    void run();
    bool reloadConfig(const std::string& configPath);
    void restart();
    void stop();
    void setStatusCallback(StatusCallback cb) { statusCallback = cb; }

private:
    NetState currentState;
    Logger logger;
    Timer timer;
    ConfigManager config;
    bool online;
    int failCount;
    static const int MAX_FAIL_COUNT = 3;
    StatusCallback statusCallback;

    void handleDisconnected();
    void handleConnectedNoAuth();
    void handleAuthenticating();
    void handleOnline();
    void handleFailed();
    void checkOnlineStatus();
};
#endif