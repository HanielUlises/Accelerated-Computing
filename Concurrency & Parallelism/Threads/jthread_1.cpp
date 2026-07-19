#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <stop_token>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

template<typename T>
class BlockingQueue {
public:
    void push(T value) {
        {
            std::lock_guard lock(mutex_);
            queue_.push(std::move(value));
        }
        cv_.notify_one();
    }

    std::optional<T> pop(std::stop_token st) {
        std::unique_lock lock(mutex_);

        cv_.wait(lock, st, [&] {
            return !queue_.empty();
        });

        if (st.stop_requested() && queue_.empty())
            return std::nullopt;

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    std::size_t size() const {
        std::lock_guard lock(mutex_);
        return queue_.size();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable_any cv_;
    std::queue<T> queue_;
};

struct Job {
    int id;
    std::vector<int> values;
};

class Scheduler {
public:
    Scheduler(std::size_t workerCount)
    {
        workers_.reserve(workerCount);

        for (std::size_t i = 0; i < workerCount; ++i) {
            workers_.emplace_back(
                [this, i](std::stop_token st) {
                    workerLoop(st, i);
                });
        }

        producer_ = std::jthread(
            [this](std::stop_token st) {
                producerLoop(st);
            });

        monitor_ = std::jthread(
            [this](std::stop_token st) {
                monitorLoop(st);
            });
    }

    ~Scheduler() = default;

private:
    void producerLoop(std::stop_token st)
    {
        std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<int> sizeDist(5000, 12000);
        std::uniform_int_distribution<int> valueDist(1, 1000);

        int id = 0;

        while (!st.stop_requested()) {

            Job job;
            job.id = id++;
            job.values.resize(sizeDist(rng));

            for (auto& v : job.values)
                v = valueDist(rng);

            queue_.push(std::move(job));

            std::this_thread::sleep_for(25ms);
        }
    }

    void workerLoop(std::stop_token st, std::size_t workerId)
    {
        while (auto job = queue_.pop(st)) {

            auto result = std::accumulate(
                job->values.begin(),
                job->values.end(),
                std::uint64_t{0},
                [](std::uint64_t acc, int v) {
                    return acc + static_cast<std::uint64_t>(v * v);
                });

            {
                std::lock_guard lock(resultMutex_);
                completed_++;
                checksum_ ^= result;
            }

            std::cout
                << "worker " << workerId
                << " processed #" << job->id
                << '\n';
        }
    }

    void monitorLoop(std::stop_token st)
    {
        while (!st.stop_requested()) {

            std::this_thread::sleep_for(1s);

            std::lock_guard lock(resultMutex_);

            std::cout
                << "[monitor] queue=" << queue_.size()
                << " completed=" << completed_
                << " checksum=" << checksum_
                << '\n';
        }
    }

    BlockingQueue<Job> queue_;

    std::vector<std::jthread> workers_;
    std::jthread producer_;
    std::jthread monitor_;

    std::mutex resultMutex_;

    std::uint64_t checksum_ = 0;
    std::size_t completed_ = 0;
};

int main()
{
    {
        Scheduler scheduler(std::thread::hardware_concurrency());

        std::this_thread::sleep_for(5s);
    }

    std::cout << "scheduler destroyed\n";
}