#include <thread>
#include <iostream>

void assigned_job() {
    std::cout << "Task: [" << std::this_thread::get_id() << "]\n";
}

int main() {
    std::jthread thread1(assigned_job);
    return 0;
}