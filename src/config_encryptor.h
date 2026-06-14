#ifndef CONFIG_ENCRYPTOR_H
#define CONFIG_ENCRYPTOR_H
#include <string>

class ConfigEncryptor {
public:
    // 加密明文配置并写入文件
    static bool saveEncrypted(const std::string& filepath,
                              const std::string& username,
                              const std::string& password,
                              const std::string& loginUrl);
    // 从文件读取并解密
    static bool loadEncrypted(const std::string& filepath,
                              std::string& username,
                              std::string& password,
                              std::string& loginUrl);
    // 检查加密配置文件是否存在
    static bool configExists(const std::string& filepath);
};
#endif