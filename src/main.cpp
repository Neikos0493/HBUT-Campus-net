#include <windows.h>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include "state_machine.h"
#include "tray_manager.h"
#include "startup_manager.h"

StateMachine* g_sm = nullptr;
TrayManager* g_tray = nullptr;
HWND g_hConsoleWnd = nullptr;

const std::string BUILTIN_URL = "http://172.16.54.18/eportal/InterFace.do?method=login";

static std::string readLine() {
    std::wstring input;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prevMode;
    GetConsoleMode(hIn, &prevMode);
    SetConsoleMode(hIn, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    wchar_t ch;
    DWORD read;
    while (true) {
        if (!ReadConsoleW(hIn, &ch, 1, &read, nullptr) || read == 0) break;
        if (ch == L'\r' || ch == L'\n') {
            if (ch == L'\r') {
                ReadConsoleW(hIn, &ch, 1, &read, nullptr);
            }
            break;
        }
        input.push_back(ch);
    }
    SetConsoleMode(hIn, prevMode);
    int len = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.size(), nullptr, 0, nullptr, nullptr);
    std::string result(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.size(), &result[0], len, nullptr, nullptr);
    return result;
}

static void printLine(const std::wstring& text) {
    DWORD written;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hOut, text.c_str(), text.length(), &written, nullptr);
}

static bool showAccountDialog(std::string& username, std::string& password) {
    bool needNewConsole = (GetConsoleWindow() == nullptr);
    if (needNewConsole) {
        AllocConsole();
        SetConsoleTitleW(L"校园网认证 - 账号设置");
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    printLine(L"========================================\n");
    printLine(L"  校园网自动认证系统 - 账号设置\n");
    printLine(L"========================================\n");
    printLine(L"请输入学号: ");
    username = readLine();
    printLine(L"请输入密码(加密密文): ");
    password = readLine();
    printLine(L"\n设置完成，窗口即将关闭...\n");
    Sleep(1000);

    if (needNewConsole) {
        FreeConsole();
    }
    return !username.empty() && !password.empty();
}

static bool saveConfig(const std::string& user, const std::string& pass, const std::string& url) {
    CreateDirectoryA("data", nullptr);
    std::ofstream file("data/config.ini");
    if (!file) return false;
    file << "username=" << user << "\n";
    file << "password=" << pass << "\n";
    file << "login_url=" << url << "\n";
    return true;
}

void onTrayMenu(int cmd) {
    switch (cmd) {
        case ID_TRAY_MENU_MODIFY: {
            std::string user, pass;
            if (showAccountDialog(user, pass)) {
                if (!saveConfig(user, pass, BUILTIN_URL)) {
                    std::cerr << "[错误] 配置保存失败" << std::endl;
                    break;
                }
                if (g_sm) {
                    g_sm->reloadConfig("data/config.ini");
                    g_sm->restart();
                }
            }
            break;
        }
        case ID_TRAY_MENU_SHOW: {
            if (g_hConsoleWnd) {
                ShowWindow(g_hConsoleWnd, SW_SHOW);
                SetForegroundWindow(g_hConsoleWnd);
            }
            break;
        }
        case ID_TRAY_MENU_EXIT: {
            PostQuitMessage(0);
            break;
        }
    }
}

static void setCurrentDirToExeDir() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring exePath(path);
    size_t pos = exePath.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        SetCurrentDirectoryW(exePath.substr(0, pos).c_str());
    }
}

int main() {
    setCurrentDirToExeDir();

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    SetConsoleTitleW(L"校园网自动认证");

    g_hConsoleWnd = GetConsoleWindow();

    std::cout << "========================================" << std::endl;
    std::cout << "  校园网自动认证系统 v2.0" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[提示] 关闭窗口将退出程序，可使用托盘菜单修改账号或退出。" << std::endl;

    // 首次配置
    {
        std::ifstream test("data/config.ini");
        if (!test.good()) {
            std::string user, pass;
            if (!showAccountDialog(user, pass)) {
                std::cerr << "未输入有效账号，程序退出。" << std::endl;
                return 1;
            }
            if (!saveConfig(user, pass, BUILTIN_URL)) {
                std::cerr << "配置保存失败！" << std::endl;
                return 1;
            }
            std::cout << "[系统] 配置已保存到 data/config.ini" << std::endl;
        }
    }

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    StartupManager::addToStartup(L"CampusNet", exePath);

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    TrayManager tray(hInstance, L"校园网认证 - 已连接");
    g_tray = &tray;
    tray.setMenuCallback(onTrayMenu);

    StateMachine sm;
    g_sm = &sm;

    if (!sm.reloadConfig("data/config.ini")) {
        std::cerr << "状态机加载配置失败，程序退出。" << std::endl;
        return 1;
    }

    std::thread authThread([&sm]() { sm.run(); });
    authThread.detach();

    tray.messageLoop();

    sm.stop();
    std::cout << "[系统] 程序已退出。" << std::endl;
    return 0;
}