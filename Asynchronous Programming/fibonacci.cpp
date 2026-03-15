#include <future>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

unsigned long long int fibonacci(long long int n) {
    std::vector<int> v (n);
    v[0] = 1;
    v[1] = 1;
    for(int i = 2; i < n; i++)
        v[i] = v[i-1] + v[i-2];
    return v[n-1];
}

int main() {
    int n;
    std::cin >> n;
    std::cout << "Calling fibonacci (" << n << ")" << std::endl;

    std::future<unsigned long long int> result = std::async(fibonacci, n);
    bool finished = false;

    using namespace std::literals;
    while(result.wait_for(1s) != std::future_status::ready) 
        std::cout << "Waiting for the result...\n";

    std::cout << result.get() << std::endl;
}