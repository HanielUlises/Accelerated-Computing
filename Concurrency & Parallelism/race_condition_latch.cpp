#include <array>
#include <boost/thread/latch.hpp>
#include <iostream>
#include <mutex>
#include <thread>

unsigned int objects = 1;
std::mutex borrower;
boost::latch checkpoint(10);

void cpu_work(unsigned long work_units) {
    unsigned long x = 0;
    for (unsigned long i; i < work_units; i++)
        x++;
}

void multiplier_1() {
    cpu_work(1);
    checkpoint.wait();
    {
      std::scoped_lock<std::mutex> lock(borrower);
      objects *= 2;
    }
    
    
    std::cout << "multiplier_1 doubled the number of objects: " << objects
            << std::endl;
}

void adder_1() {
    cpu_work(1);
    {
      std::scoped_lock<std::mutex> lock(borrower);
      objects += 3;
    }
    
    std::cout << "adder_1 added 3 objects: " << objects << std::endl;
    checkpoint.count_down();
}

int main() {
    std::array<std::thread, 10> agents;

    for (int i = 0; i < 10; i += 2) {
        agents[i] = std::thread(multiplier_1);
        agents[i + 1] = std::thread(adder_1);
    }

    for (auto &a : agents)
        a.join();

    std::cout << objects << " are needed" << std::endl;
}
