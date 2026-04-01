#include <future>
#include <iostream>
#include <thread>

int count_objects() {
    std::cout << "Agent 2 is counting objects\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 52;
}

int main() {
    std::cout << "Agent 1 asks Agent 2 the count of objects\n";
    std::future<int> result = std::async(std::launch::async, count_objects);
    std::cout << "Agent 1 does other things while waiting result\n";
    std::cout << "Agent 2 responded with: " << result.get() << '\n';
}
