#include "startup_manager.h"
#include <windows.h>

bool StartupManager::addToStartup(const std::wstring& appName, const std::wstring& exePath) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
                                L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                                0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) return false;

    result = RegSetValueExW(hKey, appName.c_str(), 0, REG_SZ,
                            (const BYTE*)exePath.c_str(),
                            (DWORD)((exePath.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

bool StartupManager::removeFromStartup(const std::wstring& appName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
                                L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                                0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) return false;
    result = RegDeleteValueW(hKey, appName.c_str());
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

bool StartupManager::isInStartup(const std::wstring& appName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
                                L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                                0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) return false;
    DWORD type, size;
    result = RegQueryValueExW(hKey, appName.c_str(), nullptr, &type, nullptr, &size);
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}