#include <iostream>
#include <thread>
#include <semaphore>
#include <chrono>

int main() {
    std::binary_semaphore sem{0};

    std::thread t([&sem] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Worker: task completed\n";
        sem.release();
    });

    std::cout << "Main: waiting...\n";
    sem.acquire();
    std::cout << "Main: signal received\n";

    t.join();
}