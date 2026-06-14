#include "auth_client.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

static std::wstring utf8ToWide(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}

static std::string readResponse(HINTERNET hRequest) {
    std::string response;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    do {
        WinHttpQueryDataAvailable(hRequest, &dwSize);
        if (dwSize == 0) break;
        pszOutBuffer = new char[dwSize + 1];
        ZeroMemory(pszOutBuffer, dwSize + 1);
        WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded);
        response.append(pszOutBuffer);
        delete[] pszOutBuffer;
    } while (dwSize > 0);
    return response;
}

bool AuthClient::login(const std::string& username, const std::string& password, const std::string& url) {
    std::cout << "[Auth] 尝试登录: " << username << " @ " << url << std::endl;

    std::wstring wUrl = utf8ToWide(url);
    URL_COMPONENTS urlComp = {0};
    urlComp.dwStructSize = sizeof(urlComp);
    wchar_t szHostName[256] = L"";
    wchar_t szUrlPath[1024] = L"";
    urlComp.lpszHostName = szHostName;
    urlComp.dwHostNameLength = 256;
    urlComp.lpszUrlPath = szUrlPath;
    urlComp.dwUrlPathLength = 1024;
    if (!WinHttpCrackUrl(wUrl.c_str(), 0, 0, &urlComp)) {
        std::cerr << "[Auth] URL解析失败" << std::endl;
        return false;
    }
    std::wstring host = szHostName;
    std::wstring path = szUrlPath;

    HINTERNET hSession = WinHttpOpen(L"CampusNet/1.0",
                                      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME,
                                      WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "[Auth] WinHttpOpen failed" << std::endl;
        return false;
    }
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), urlComp.nPort, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        std::cerr << "[Auth] WinHttpConnect failed" << std::endl;
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(),
                                             NULL, WINHTTP_NO_REFERER,
                                             WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        std::cerr << "[Auth] WinHttpOpenRequest failed" << std::endl;
        return false;
    }

    std::string postData =
        "userId=" + username +
        "&password=" + password +
        "&service=default" +
        "&queryString=wlanuserip%3D10.9.234.175%26wlanacname%3Dlogic%26nasip%3D10.253.0.17%26wlanparameter%3D00-72-ee-68-65-f5%26url%3Dhttp%3A%2F%2Fwww.msftconnecttest.com%2Fredirect%26userlocation%3Dethtrunk%2F3%3A3940.0" +
        "&operatorPwd=&operatorUserId=&validcode=&passwordEncrypt=true";

    LPCWSTR headers = L"Content-Type: application/x-www-form-urlencoded\r\n";
    WinHttpAddRequestHeaders(hRequest, headers, -1, WINHTTP_ADDREQ_FLAG_ADD);
    BOOL bRet = WinHttpSendRequest(hRequest, headers, 0,
                                    (LPVOID)postData.c_str(), postData.length(),
                                    postData.length(), 0);
    if (!bRet) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        std::cerr << "[Auth] WinHttpSendRequest failed" << std::endl;
        return false;
    }

    bRet = WinHttpReceiveResponse(hRequest, NULL);
    if (!bRet) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        std::cerr << "[Auth] WinHttpReceiveResponse failed" << std::endl;
        return false;
    }
    std::string response = readResponse(hRequest);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    bool success = false;
    if (response.find("\"result\":\"success\"") != std::string::npos ||
        response.find("\"success\":true") != std::string::npos ||
        response.find("success") != std::string::npos) {
        success = true;
    }
    std::cout << "[Auth] 服务器响应: " << response.substr(0, 200) << "..." << std::endl;
    std::cout << "[Auth] 登录结果: " << (success ? "成功" : "失败") << std::endl;
    return success;
}

bool AuthClient::logout(const std::string& url) {
    std::cout << "[Auth] 注销功能暂未实现" << std::endl;
    return true;
}