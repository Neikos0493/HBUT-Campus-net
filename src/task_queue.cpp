#include "task_queue.h"
#include <stdexcept>

TaskQueue::TaskQueue() : front(nullptr), rear(nullptr), size(0) {}

TaskQueue::~TaskQueue() {
    while (!isEmpty()) dequeue();
}

void TaskQueue::enqueue(const Task& task) {
    Node* newNode = new Node(task);
    if (isEmpty()) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    ++size;
}

Task TaskQueue::dequeue() {
    if (isEmpty()) throw std::runtime_error("Queue empty");
    Node* temp = front;
    Task ret = temp->data;
    front = front->next;
    delete temp;
    --size;
    if (isEmpty()) rear = nullptr;
    return ret;
}

bool TaskQueue::isEmpty() const { return size == 0; }
int TaskQueue::getSize() const { return size; }