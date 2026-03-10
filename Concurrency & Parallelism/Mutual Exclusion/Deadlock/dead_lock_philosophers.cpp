#include <thread>
#include <iostream>

// #define NO_DEADLOCK

int sushi_count = 1000;

void philosopher(std::mutex &first_chopstick, std::mutex &second_chosptick) {
    while(sushi_count > 0) {
        first_chopstick.lock();
        second_chosptick.lock();
        
        if(sushi_count) sushi_count--;

        second_chosptick.unlock();
        first_chopstick.unlock();
    }
}

int main() {
    #ifdef NO_DEADLOCK
    // Chopstick A is the highest priority chopstick
    // Agents should take the highest priority chopstick first
    std::mutex chopstick_a, chopstick_b;
    std::thread agent_1(philosopher, std::ref(chopstick_a), std::ref(chopstick_b));
    std::thread agent_2(philosopher, std::ref(chopstick_a), std::ref(chopstick_b));
    #endif

    std::mutex chopstick_a, chopstick_b;
    std::thread agent_1(philosopher, std::ref(chopstick_a), std::ref(chopstick_b));
    std::thread agent_2(philosopher, std::ref(chopstick_b), std::ref(chopstick_a));

    agent_1.join();
    agent_2.join();

    std::cout << "Philosophers are done eating" << std::endl;
}