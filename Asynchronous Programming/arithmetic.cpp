#include <future>
#include <string>
#include <iostream>
#include <thread>

void printing() {
    std::cout << "[Priting thread: " << std::this_thread::get_id() << "]" << std::endl;
}

int addition(int x, int y) {
    std::cout << "[Thread: " << std::this_thread::get_id() << " performing addition]" << std::endl;
    return x+y;
}

int subtraction(int x, int y) {
    std::cout << "[Thread: " << std::this_thread::get_id() << " performing subtraction]" << std::endl;
    return x-y;
}

void run_code() {
    std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;
    int x = 100;
    int y = 50;

    std::future<void> f1 = std::async(std::launch::async, printing);
    std::future<int> f2 = std::async(std::launch::deferred, addition, x,y);
    std::future<int> f3 = std::async(std::launch::deferred, subtraction, x, y);

    f1.get();
    std::cout << "Value received using f2 future: " << f2.get() << " (addition)" << std::endl;
    std::cout << "Value received using f3 future: " << f3.get() << " (subtraction)" << std::endl;
      
}

int main() {
    run_code();
    return 0;
} 