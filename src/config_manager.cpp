#include "config_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Config] 无法打开配置文件: " << filepath << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            configMap[key] = value;
        }
    }
    return true;
}

std::string ConfigManager::get(const std::string& key) const {
    auto it = configMap.find(key);
    return (it != configMap.end()) ? it->second : "";
}

void ConfigManager::set(const std::string& key, const std::string& value) {
    configMap[key] = value;
}

bool ConfigManager::contains(const std::string& key) const {
    return configMap.find(key) != configMap.end();
}