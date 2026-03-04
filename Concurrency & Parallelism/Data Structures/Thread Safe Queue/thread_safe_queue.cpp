#include "thread_safe_queue.h"

template <typename T>
thread_safe_queue<T>::thread_safe_queue(thread_safe_queue const &other) {
    this -> mut = other.mut;
    this -> cv = other.cv;
    this -> queue = other.queue;
}

template <typename T>
void thread_safe_queue<T>::push(T &value) {
    std::lock_guard<std::mutex> lock_grd(mut);
    queue.push(std::make_shared<T> (value));
    cv.notify_one();
}

template <typename T>
bool thread_safe_queue<T>::empty() {
    std::lock_guard<std::mutex> lock_grd(mut);
    return queue.empty();
}

template <typename T>
std::shared_ptr<T> thread_safe_queue<T>::pop(){
    std::lock_guard<std::mutex> lock_grd(mut);

    if(queue.empty()) {
        return std::shared_ptr<T>();
    } else {
        std::shared_ptr<T> ref(queue.front());
        queue.pop();
        return ref;
    }
}

template <typename T>
std::shared_ptr<T> thread_safe_queue<T>::wait_pop() {
    std::unique_lock<std::mutex> lock_guard(mut);
    cv.wait(lock_guard, [this] {
        return !queue.empty();
    });
    std::shared_ptr<T> ref = queue.front();
    queue.pop();
    return ref;
}