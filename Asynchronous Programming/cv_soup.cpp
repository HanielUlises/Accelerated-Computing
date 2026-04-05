#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <array>

int soup_servings = 10;
std::mutex slow_cooker_lid;
std::condition_variable soup_taken;

void hungry_person(int id) {
    int put_lid_back = 0;
    while (soup_servings > 0) {
        std::unique_lock<std::mutex> lid_lock(slow_cooker_lid);
        while((id != soup_servings % 2) && (soup_servings > 0)) {
            put_lid_back++;
            soup_taken.wait(lid_lock);
        }        

        if(soup_servings > 0) {
            soup_servings--;
            lid_lock.unlock();
            soup_taken.notify_one();
        }
    }
    std::cout << "Person " << id << " put the lid back " << put_lid_back << " times" << '\n';
}

int main() {
    std::array<std::thread, 5> hungry_threads;

    for(int i = 0; i < 5; i++) 
        hungry_threads[i] = std::thread(hungry_person, i);
    

    for(auto &ht : hungry_threads) 
        ht.join();

    return;
}