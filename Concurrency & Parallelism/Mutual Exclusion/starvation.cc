#include <thread>
#include <mutex>
#include <iostream>
#include <array>

int sushi_count = 5000;

void philosopher(std::mutex &chopsticks) {
    int sushi_eaten = 0;
    while(sushi_count > 0) {
        std::scoped_lock lock(chopsticks);
        if(sushi_count){ sushi_count--; sushi_eaten++;}
    }
    std::cout << "Philosopher: [" << std::this_thread::get_id() << "] has eaten " << sushi_eaten << " sushis" << std::endl;
}

int main() {
    std::mutex chopsticks;
    std::array<std::thread, 200> philosophers;

    for(int i = 0; i < philosophers.size(); i++) 
        philosophers[i] = std::thread(philosopher, std::ref(chopsticks));

    for (auto &philosopher_ : philosophers)
        philosopher_.join();

    std::cout << "Philosophers are done eating" << std::endl;
}