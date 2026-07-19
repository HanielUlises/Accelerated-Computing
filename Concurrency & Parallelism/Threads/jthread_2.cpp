#include <thread>
#include <iostream>
#include <stop_token>
#include <chrono>

using namespace::std::literals;

void assigned_job(std::stop_token token) {
    int counter{0};
    while(counter < 10) {
        if(token.stop_requested()) return;
        std::this_thread::sleep_for(0.2s);
        std::cout << "This is an interruptible thread: " << counter << "\n";
        ++counter;
    }
    std::cout << "Task: [" << std::this_thread::get_id() << "]\n";
}

int main() {
    std::jthread thread1(assigned_job);
    return 0;
}