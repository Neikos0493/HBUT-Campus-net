#ifndef TRAY_MANAGER_H
#define TRAY_MANAGER_H
#include <windows.h>
#include <string>
#include <functional>

#define ID_TRAY_MENU_MODIFY 1001
#define ID_TRAY_MENU_SHOW   1002
#define ID_TRAY_MENU_EXIT   1003

class TrayManager {
public:
    using MenuCallback = std::function<void(int)>;

    TrayManager(HINSTANCE hInstance, const std::wstring& tipText);
    ~TrayManager();
    void setMenuCallback(MenuCallback callback);
    void showBalloon(const std::wstring& title, const std::wstring& text);
    void setTip(const std::wstring& tip);
    void messageLoop();

private:
    HWND m_hWnd;
    NOTIFYICONDATAW m_nid;
    HMENU m_hMenu;
    MenuCallback m_menuCallback;
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
#endif