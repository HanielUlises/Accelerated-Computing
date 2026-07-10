#include <iostream>
#include <thread>
#include <future>
#include <stdexcept>
#include <cmath>
#include <mutex>

void print_result(std::future<int> &fut) {
    if(fut.valid()) {
        std::cout << "[This is valid future]\n";
        std::cout << fut.get() << '\n';
    } else {
        std::cout << "[Invalid future]\n";
    }
}

void run_code() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    std::thread th1(print_result, std::ref(fut));
    std::thread th2(print_result, std::ref(fut));

    prom.set_value(5);

    th1.join();
    th2.join();
}