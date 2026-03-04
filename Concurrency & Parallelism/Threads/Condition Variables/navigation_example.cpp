#include <iostream>
#include <mutex>
#include <thread>
#include <string>
#include <chrono>
#include <condition_variable>

bool have_i_arrived = false;
int total_distance = 10;
int covered_distance = 0;

std::condition_variable cv;
std::mutex mut;

void keep_moving() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        covered_distance++;

        // Notify the waiting threads if the event occurs
        if(covered_distance == total_distance) cv.notify_one();
    }
}

void ask_driver_to_wake_up_time_t() {
    std::unique_lock<std::mutex> unique_lck(mut);
    cv.wait(unique_lck, [] {return covered_distance == total_distance;});
    std::cout << "Reached. Distance covered = " << covered_distance << std::endl;
}

void run_communication() {
    std::thread driver_thread(keep_moving);
    std::thread passenger_thread(ask_driver_to_wake_up_time_t);
    passenger_thread.join();
    driver_thread.join();
}

int main() {
    run_communication();
}