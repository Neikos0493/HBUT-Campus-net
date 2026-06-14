#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H
#include <string>

struct Task {
    int id;
    std::string type;
    int retryCount;
};

class TaskQueue {
private:
    struct Node {
        Task data;
        Node* next;
        Node(const Task& t) : data(t), next(nullptr) {}
    };
    Node* front;
    Node* rear;
    int size;
public:
    TaskQueue();
    ~TaskQueue();
    void enqueue(const Task& task);
    Task dequeue();
    bool isEmpty() const;
    int getSize() const;
};
#endif