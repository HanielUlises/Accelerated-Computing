#include <thread>
#include <mutex>
#include <iostream>

int sushi_count = 5000;

void philosopher(std::mutex &chopsticks) {
    while(sushi_count > 0) {
        chopsticks.lock();
        
        if(sushi_count) sushi_count--;
        if(sushi_count == 10) {
            std::cout << "This philosopher has had enough" << std::endl;
            break;
        }
        chopsticks.unlock();
    }
}

int main() {
    std::mutex chopsticks;
    std::thread agent_1(philosopher, std::ref(chopsticks));
    std::thread agent_2(philosopher, std::ref(chopsticks));

    agent_1.join(); agent_2.join();

    std::cout << "Philosophers are done eating" << std::endl;

}