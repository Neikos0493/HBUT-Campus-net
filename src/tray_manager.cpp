#include "tray_manager.h"
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)

TrayManager::TrayManager(HINSTANCE hInstance, const std::wstring& tipText) : m_menuCallback(nullptr) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"CampusNetTrayClass";
    RegisterClassW(&wc);

    m_hWnd = CreateWindowExW(WS_EX_TOOLWINDOW,
                             L"CampusNetTrayClass", L"CampusNet",
                             WS_POPUP, 0, 0, 0, 0,
                             nullptr, nullptr, hInstance, nullptr);
    SetWindowLongPtrW(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(NOTIFYICONDATAW);
    m_nid.hWnd   = m_hWnd;
    m_nid.uID    = 1;
    m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    m_nid.uCallbackMessage = WM_TRAYICON;
    m_nid.hIcon  = LoadIcon(nullptr, IDI_APPLICATION);
    wcsncpy_s(m_nid.szTip, tipText.c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_ADD, &m_nid);

    m_hMenu = CreatePopupMenu();
    AppendMenuW(m_hMenu, MF_STRING, ID_TRAY_MENU_MODIFY, L"修改账号密码");
    AppendMenuW(m_hMenu, MF_STRING, ID_TRAY_MENU_SHOW,   L"打开主面板");
    AppendMenuW(m_hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m_hMenu, MF_STRING, ID_TRAY_MENU_EXIT,   L"退出程序");
}

TrayManager::~TrayManager() {
    Shell_NotifyIconW(NIM_DELETE, &m_nid);
    DestroyMenu(m_hMenu);
    DestroyWindow(m_hWnd);
}

void TrayManager::setMenuCallback(MenuCallback callback) {
    m_menuCallback = callback;
}

void TrayManager::showBalloon(const std::wstring& title, const std::wstring& text) {
    m_nid.uFlags |= NIF_INFO;
    wcsncpy_s(m_nid.szInfoTitle, title.c_str(), _TRUNCATE);
    wcsncpy_s(m_nid.szInfo, text.c_str(), _TRUNCATE);
    m_nid.dwInfoFlags = NIIF_INFO;
    Shell_NotifyIconW(NIM_MODIFY, &m_nid);
}

void TrayManager::setTip(const std::wstring& tip) {
    wcsncpy_s(m_nid.szTip, tip.c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_MODIFY, &m_nid);
}

void TrayManager::messageLoop() {
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

LRESULT CALLBACK TrayManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TrayManager* self = (TrayManager*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    switch (msg) {
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hWnd);
                TrackPopupMenu(self->m_hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
                PostMessageW(hWnd, WM_NULL, 0, 0);
            }
            break;
        case WM_COMMAND:
            if (self->m_menuCallback) {
                self->m_menuCallback(LOWORD(wParam));
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}