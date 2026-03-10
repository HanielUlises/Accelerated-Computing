#include <thread>
#include <iostream>
#include <mutex>

// #define NO_LIVELOCK

int sushi_count = 1000;

void philosopher(std::mutex &first_chopstick, std::mutex &second_chosptick) {
    while(sushi_count > 0) {
        first_chopstick.lock();
        second_chosptick.lock();

        if(!second_chosptick.try_lock()){ 
            first_chopstick.unlock();
            std::this_thread::yield();
        }
        else {
            if(sushi_count) sushi_count--;

            second_chosptick.unlock();
            first_chopstick.unlock();
        }


    }
}

int main() {
    std::mutex chopstick_a, chopstick_b;
    std::thread agent_1(philosopher, std::ref(chopstick_a), std::ref(chopstick_b));
    std::thread agent_2(philosopher, std::ref(chopstick_b), std::ref(chopstick_a));
    std::thread agent_3(philosopher, std::ref(chopstick_a), std::ref(chopstick_b));
    std::thread agent_4(philosopher, std::ref(chopstick_b), std::ref(chopstick_a));

    agent_1.join();
    agent_2.join();
    agent_3.join();
    agent_4.join();

    std::cout << "Philosophers are done eating" << std::endl;
}