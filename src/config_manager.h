#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include <string>
#include <unordered_map>

class ConfigManager {
private:
    std::unordered_map<std::string, std::string> configMap;
public:
    bool loadFromFile(const std::string& filepath);
    std::string get(const std::string& key) const;
    void set(const std::string& key, const std::string& value);
    bool contains(const std::string& key) const;
};
#endif