#include "timer.h"

void Timer::addTimer(int delayMs, std::function<void()> callback) {
    TimerNode node;
    node.expireTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
    node.callback = callback;
    heap.push_back(node);
    heapifyUp(heap.size() - 1);
}

void Timer::tick() {
    auto now = std::chrono::steady_clock::now();
    while (!heap.empty() && heap[0].expireTime <= now) {
        heap[0].callback();
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
    }
}

void Timer::heapifyUp(int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap[idx] > heap[parent]) break;
        std::swap(heap[idx], heap[parent]);
        idx = parent;
    }
}

void Timer::heapifyDown(int idx) {
    int size = heap.size();
    while (true) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;
        if (left < size && heap[left] > heap[smallest]) smallest = left;
        if (right < size && heap[right] > heap[smallest]) smallest = right;
        if (smallest == idx) break;
        std::swap(heap[idx], heap[smallest]);
        idx = smallest;
    }
}