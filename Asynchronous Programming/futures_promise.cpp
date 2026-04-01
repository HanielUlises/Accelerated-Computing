#include <chrono>
#include <thread>
#include <future>
#include <iostream>
#include <vector>

long long int fibonacci(std::future<int> &n) {
    int N = n.get();
    std::vector<long long int> seq;
    seq.resize(N);
    seq[0] = 1;
    seq[1] = 1;

    for(int i = 2; i < N; i++) {
        seq[i] = seq[i - 1] + seq[i -2];
    }
    
    return seq[N-1];
}

int main() {
    std::promise<int> p;
    std::future<int> father = p.get_future();

    std::future<long long int> son = std::async(std::launch::async, fibonacci, std::ref(father));

    std::this_thread::sleep_for(std::chrono::seconds(4));
    p.set_value(8);

    auto result = son.get();
    std::cout << "th fibonacci number is: " << result << '\n'; 
    return 0;
}
