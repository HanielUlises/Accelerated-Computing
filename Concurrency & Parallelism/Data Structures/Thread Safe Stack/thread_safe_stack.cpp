#include "thread_safe_stack.h"

template <typename T>
void trivial_thread_safe_stack<T>::push(T element) {
    std::lock_guard<std::mutex> lck_grd(mut);
    _stack.push(T);
}

template <typename T>
std::shared_ptr<T>  trivial_thread_safe_stack<T>::pop() {
    std::lock_guard<std::mutex> lck_grd(mut);
    if (_stack.empty()) throw std::runtime_error("Stack is empty\n");
    std::shared_ptr<T> res(_stack.top());
    _stack.pop();
    return res;
}

template <typename T>
T& trivial_thread_safe_stack<T>::top() {
    std::lock_guard<std::mutex> lck_grd(mut);
    _stack.top();
}

template <typename T>
bool trivial_thread_safe_stack<T>::empty() {
    
}

template <typename T>
size_t trivial_thread_safe_stack<T>::size() {

}