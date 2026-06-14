#ifndef TIMER_H
#define TIMER_H
#include <vector>
#include <functional>
#include <chrono>

class Timer {
public:
    struct TimerNode {
        std::chrono::steady_clock::time_point expireTime;
        std::function<void()> callback;
        bool operator>(const TimerNode& other) const {
            return expireTime > other.expireTime;
        }
    };
private:
    std::vector<TimerNode> heap;
    void heapifyUp(int idx);
    void heapifyDown(int idx);
public:
    void addTimer(int delayMs, std::function<void()> callback);
    void tick();
    bool empty() const { return heap.empty(); }
};
#endif