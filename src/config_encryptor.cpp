#include "config_encryptor.h"
#include <windows.h>
#include <dpapi.h>
#include <fstream>
#include <vector>
#include <cstring>
#pragma comment(lib, "crypt32.lib")

// 加密：不包含字符串末尾的 '\0'
static std::vector<BYTE> protectData(const std::string& plaintext) {
    DATA_BLOB dataIn, dataOut;
    dataIn.pbData = (BYTE*)plaintext.data();      // 不指向 c_str，避免包含 '\0'
    dataIn.cbData = (DWORD)plaintext.size();      // 不包含 '\0'
    CryptProtectData(&dataIn, L"CampusNet Config",
                     nullptr, nullptr, nullptr, 0, &dataOut);
    std::vector<BYTE> result(dataOut.pbData, dataOut.pbData + dataOut.cbData);
    LocalFree(dataOut.pbData);
    return result;
}

// 解密：同样不包含 '\0'
static std::string unprotectData(const BYTE* data, DWORD size) {
    DATA_BLOB dataIn, dataOut;
    dataIn.pbData = (BYTE*)data;
    dataIn.cbData = size;
    if (!CryptUnprotectData(&dataIn, nullptr, nullptr, nullptr, nullptr, 0, &dataOut)) {
        return "";
    }
    std::string result((char*)dataOut.pbData, dataOut.cbData);
    LocalFree(dataOut.pbData);
    return result;
}

bool ConfigEncryptor::saveEncrypted(const std::string& filepath,
                                    const std::string& username,
                                    const std::string& password,
                                    const std::string& loginUrl) {
    auto encUser = protectData(username);
    auto encPass = protectData(password);
    auto encUrl  = protectData(loginUrl);

    std::ofstream file(filepath, std::ios::binary);
    if (!file) return false;

    auto writeBlob = [&](const std::vector<BYTE>& blob) {
        DWORD len = (DWORD)blob.size();
        file.write((char*)&len, sizeof(len));
        file.write((char*)blob.data(), len);
    };
    writeBlob(encUser);
    writeBlob(encPass);
    writeBlob(encUrl);
    return true;
}

bool ConfigEncryptor::loadEncrypted(const std::string& filepath,
                                    std::string& username,
                                    std::string& password,
                                    std::string& loginUrl) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) return false;

    auto readBlob = [&]() -> std::vector<BYTE> {
        DWORD len = 0;
        file.read((char*)&len, sizeof(len));
        if (file.gcount() != sizeof(len)) return {};
        std::vector<BYTE> data(len);
        file.read((char*)data.data(), len);
        if (file.gcount() != len) return {};
        return data;
    };

    auto blobUser = readBlob();
    auto blobPass = readBlob();
    auto blobUrl  = readBlob();

    if (blobUser.empty() || blobPass.empty() || blobUrl.empty()) return false;

    username = unprotectData(blobUser.data(), (DWORD)blobUser.size());
    password = unprotectData(blobPass.data(), (DWORD)blobPass.size());
    loginUrl = unprotectData(blobUrl.data(), (DWORD)blobUrl.size());

    return !username.empty() && !password.empty() && !loginUrl.empty();
}

bool ConfigEncryptor::configExists(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    return file.good();
}