#include "logger.h"
#include <ctime>
#include <sstream>
#include <iomanip>

Logger::Logger(int cap) : capacity(cap), head(0), tail(0), count(0) {
    buffer.resize(capacity);
}

void Logger::log(const std::string& message) {
    std::time_t now = std::time(nullptr);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " - " << message;
    buffer[tail] = ss.str();
    tail = (tail + 1) % capacity;
    if (count == capacity) {
        head = (head + 1) % capacity;
    } else {
        ++count;
    }
}

std::vector<std::string> Logger::getLogs() const {
    std::vector<std::string> result;
    for (int i = 0; i < count; ++i) {
        int idx = (head + i) % capacity;
        result.push_back(buffer[idx]);
    }
    return result;
}