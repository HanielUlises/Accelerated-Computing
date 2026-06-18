#include <iostream>
#include <future>
#include <numeric>
#include <vector>

constexpr int MIN_ELEMENT_COUNT = 1000;

template <typename iterator>
int parallel_accumulate(iterator begin, iterator end) {
    long length = std::distance(begin, end);

    // At least runs 1000 elements
    if(length <= MIN_ELEMENT_COUNT) {
        std::cout << std::this_thread::get_id() << std::endl;
        return std::accumulate(begin, end, 0);
    }

    iterator mid = begin;
    std::advance(mid, (length + 1) / 2);

    // Recursive  call to parallel accumulate
    std::future<int> f1 = std::async(std::launch::deferred | std::launch::async, parallel_accumulate<iterator>, mid, end);
    auto sum = parallel_accumulate(begin, mid);
    return sum + f1.get();
}

int main() {
    std::vector<int>v(1000,1);
    std::cout << "The sum is " << parallel_accumulate(v.begin(), v.end());
}