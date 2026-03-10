#include <thread>
#include <iostream>
#include <mutex>

// #define DEADLOCK

int sushi_count = 1000;

void philosopher(std::mutex &first_chopstick, std::mutex &second_chosptick) {
    while(sushi_count > 0) {
        std::scoped_lock<std::mutex> scp_lck1(first_chopstick);
        std::scoped_lock<std::mutex> scp_lck2(second_chosptick);
        
        if(sushi_count) sushi_count--;

        second_chosptick.unlock();
        first_chopstick.unlock();
    }
}

int main() {
    #ifdef DEADLOCK
    std::mutex chopstick_a, chopstick_b;
    std::thread agent_1(philosopher, std::ref(chopstick_a), std::ref(chopstick_b));
    std::thread agent_2(philosopher, std::ref(chopstick_b), std::ref(chopstick_a));
    #endif

    std::mutex chopstick_a, chopstick_b;
    std::thread agent_1(philosopher, std::ref(chopstick_a), std::ref(chopstick_b));
    std::thread agent_2(philosopher, std::ref(chopstick_b), std::ref(chopstick_a));

    agent_1.join();
    agent_2.join();

    std::cout << "Philosophers are done eating" << std::endl;
}