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

void alternate_job() {
    int counter{0};
    while(counter < 10) {
        std::this_thread::sleep_for(0.2s);
        std::cerr << "This is non-interruptible thread: " << counter << '\n';
        ++counter;
    }
}

int main() {
    std::cout << '\n';
    std::jthread thread1(assigned_job);
    std::jthread thread2(alternate_job);

    std::this_thread::sleep_for(1.0s);
    thread1.request_stop();
    thread2.request_stop();

    return 0;
}