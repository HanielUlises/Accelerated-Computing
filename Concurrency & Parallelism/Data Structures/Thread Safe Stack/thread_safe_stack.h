#include <iostream>
#include <mutex>
#include <stack>
#include <thread>

template <typename T>
class trivial_thread_safe_stack {
        std::stack<T> _stack;
        std::mutex mut;

    public:
        void push(T element);
        void pop();
        T& top();
        bool empty();
        size_t size();
};