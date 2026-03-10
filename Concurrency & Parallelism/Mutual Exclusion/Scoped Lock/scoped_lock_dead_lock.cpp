#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>

#define USE_SCOPED_LOCK

struct Resource {
    std::mutex m;
    int value = 0;
};

Resource database;
Resource cache;
Resource network;

void agent_a() {
#ifdef USE_SCOPED_LOCK
    std::scoped_lock lock(database.m,cache.m);
#else
    std::lock_guard<std::mutex> l1(database.m);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> l2(cache.m);
#endif

    database.value++;
    cache.value++;
    std::cout<<"agent_a updated database+cache\n";
}

void agent_b() {
#ifdef USE_SCOPED_LOCK
    std::scoped_lock lock(cache.m,network.m);
#else
    std::lock_guard<std::mutex> l1(cache.m);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> l2(network.m);
#endif

    cache.value++;
    network.value++;
    std::cout<<"agent_b updated cache+network\n";
}

void agent_c() {
#ifdef USE_SCOPED_LOCK
    std::scoped_lock lock(network.m,database.m);
#else
    std::lock_guard<std::mutex> l1(network.m);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> l2(database.m);
#endif

    network.value++;
    database.value++;
    std::cout<<"agent_c updated network+database\n";
}

void run_agent(void(*f)()) {
    for(int i=0;i<5;i++)
    {
        f();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    std::vector<std::thread> agents;

    agents.emplace_back(run_agent,agent_a);
    agents.emplace_back(run_agent,agent_b);
    agents.emplace_back(run_agent,agent_c);

    for(auto& t:agents)
        t.join();

    std::cout<<"database="<<database.value<<"\n";
    std::cout<<"cache="<<cache.value<<"\n";
    std::cout<<"network="<<network.value<<"\n";
}