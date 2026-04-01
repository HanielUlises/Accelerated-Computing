#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <thread>
#include <boost/asio.hpp>
#include <iostream>

void task_execution(int task_id) {
    std::cout << "Thread [" << std::this_thread::get_id() << "] is executing task " << task_id << '\n';
}

int main() {
    boost::asio::thread_pool pool(4);
    
    for(int i = 0; i < 100; i++) {
        boost::asio::post(pool, [i]() {task_execution(i);});
    }

    pool.join();
}
