#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <vector>

class Logger {
private:
    std::vector<std::string> buffer;
    int head;
    int tail;
    int count;
    int capacity;
public:
    Logger(int cap = 100);
    void log(const std::string& message);
    std::vector<std::string> getLogs() const;
};
#endif