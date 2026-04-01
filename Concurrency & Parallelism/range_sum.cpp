#include <iostream>
#include <future>

using ll = long long int;

ll recursive_sum(ll low, ll high, int depth = 0) {
    if (depth > 3) {
        ll sum = 0;
        for(auto i = low; i < high; i++)
            sum += i;
        return sum;
    } else {
        auto mid = (high + low)/2;
        auto left = std::async(std::launch::async, recursive_sum, low, mid, depth++);
        auto right = recursive_sum(mid, high, depth++);
        return left.get() + right;
    }
}

int main() {
    ll total = recursive_sum(1, 100);
    std::cout << "Total sum: " << total << '\n';

    return 0;
}
