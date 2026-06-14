#ifndef STARTUP_MANAGER_H
#define STARTUP_MANAGER_H
#include <string>

class StartupManager {
public:
    static bool addToStartup(const std::wstring& appName, const std::wstring& exePath);
    static bool removeFromStartup(const std::wstring& appName);
    static bool isInStartup(const std::wstring& appName);
};
#endif