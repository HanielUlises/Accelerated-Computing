#include <iostream>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <thread>
#include <queue>

template <typename T>
class thread_safe_queue {
        std::mutex mut;
        std::condition_variable cv;
        std::queue<std::shared_ptr<T>> queue;

    public:
        thread_safe_queue() {
            
        }

        thread_safe_queue(thread_safe_queue const&);

        void push(T &);
        bool empty ();
        std::shared_ptr<T> pop();
        std::shared_ptr<T> wait_pop();
};