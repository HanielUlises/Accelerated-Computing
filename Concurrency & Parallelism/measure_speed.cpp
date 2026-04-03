#include <future>
#include <chrono>
#include <iostream>

using ull = unsigned long long;
using ui = unsigned int;

ull sequential_sum(ui low, ui high) {
    ull sum = 0;
    for(auto i = low; i < high; i++)
        sum += i;
    return sum;
}

ull parallel_sum(ui low, ui high, int depth = 0) {
    if(depth > 3) {
        ull sum = 0;
        for(int i = low; i < high; i++)
            sum += i;
        return sum;
    } else {
        int mid = low + (high - low) / 2;
        int left = parallel_sum(low, mid, depth++);
        int right = parallel_sum(mid, high, depth++);
        return left + right;
    }   
}

int main() {
    const int NUM_EVAL_RUNS = 10;
    const int SUM_VALUE = 100'000;

    std::cout << "Evaluating sequential implementation...\n";
    std::chrono::duration<double> sequential_time(0);
    auto sequential_result = sequential_sum(0, SUM_VALUE);
    
    for(int i = 0; i < NUM_EVAL_RUNS; i++) {
        auto start_time = std::chrono::high_resolution_clock::now();
        sequential_sum(0, SUM_VALUE);
        sequential_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    sequential_time /= NUM_EVAL_RUNS;

    std::cout << "Evaluating parallel implementation...\n";
    std::chrono::duration<double> parallel_time(0);
    auto parallel_result = parallel_sum(0, SUM_VALUE);
    
    for(int i = 0; i < NUM_EVAL_RUNS; i++) {
        auto start_time = std::chrono::high_resolution_clock::now();
        parallel_sum(0, SUM_VALUE);
        parallel_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    parallel_time /= NUM_EVAL_RUNS;

    if(sequential_result != parallel_result) {
        std::cout << "Error: Result mismatch.\n";
        std::cout << "Sequential result: " << sequential_result << " Paralle result: " << parallel_result << '\n';
    }

    std::cout << "======================================================";
    std::cout << "Average sequential time: " << sequential_time.count() * 1000 << '\n';
    std::cout << "Average parallel time: " << parallel_time.count() * 1000 << '\n';
    std::cout << "Speedup: " << sequential_time / parallel_time << '\n';
    std::cout << "Efficiency: " << 100 * (sequential_time/parallel_time)/std::thread::hardware_concurrency() << '%';

}
