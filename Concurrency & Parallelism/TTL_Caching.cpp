/* To compile and run:
g++ exercise_2_starter.cpp -pthread -o exercise_2_solution
./exercise_2_solution

Expected behavior:
- Multiple threads can store and retrieve cached values
- Items automatically expire after their TTL period
- Background cleanup thread removes expired items
- Threads can wait for specific keys to become available

YOUR TASKS:
1. Declare mutex and condition_variable for thread coordination
2. Add locking to put() method and implement thread notification
3. Add locking to get() method for thread-safe access
4. Implement wait_for_key() with unique_lock and predicate-based waiting
5. Add while loop and locking to cleanup_expired() method
6. Add locking to size() method for thread-safe access
*/

#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <vector>

class TTLCache {
private:
    struct CacheItem {
        int value;
        std::chrono::steady_clock::time_point expiry_time;
        
        CacheItem(int val, std::chrono::seconds ttl) 
            : value(val), expiry_time(std::chrono::steady_clock::now() + ttl) {}
    };
    
    std::unordered_map<std::string, CacheItem> cache;
    // TODO: Declare std::mutex variable named: cache_mutex
    // TODO: Declare std::condition_variable variable named: key_available
    std::mutex cache_mutex;
    std::condition_variable key_available;
    bool cleanup_running;
    
 public:
    TTLCache() : cleanup_running(true) {}
    
    // Store a value in the cache with TTL
    void put(const std::string& key, int value, std::chrono::seconds ttl) {
        // TODO: Create a lock for cache_mutex using std::lock_guard
        std::lock_guard<std::mutex> cache_lock(cache_mutex);
        cache.emplace(key, CacheItem(value, ttl));
        // TODO: Notify all threads waiting for key_avaialble
        key_available.notify_all();
        std::cout << "Stored key '" << key << "' with value " << value << std::endl;
    }
    
    // Get a value from cache (returns -1 if not found or expired)
    int get(const std::string& key) {
        // TODO: Lock cache_mutex using std::lock_guard
        std::lock_guard<std::mutex> cache_lock(cache_mutex);
        auto it = cache.find(key);
        
        if (it == cache.end()) {
            return -1; // Key not found
        }
        
        // Check if item has expired
        if (std::chrono::steady_clock::now() > it->second.expiry_time) {
            cache.erase(it);
            return -1; // Expired
        }
        
        return it->second.value;
    }
    
    // Wait for a key to become available (with timeout)
    bool wait_for_key(const std::string& key, std::chrono::seconds timeout) {
        // TODO: Create std::unique_lock<std::mutex> using cache_mutex, store in variable named: lock
        std::unique_lock<std::mutex> lock(cache_mutex);
                 auto deadline = std::chrono::steady_clock::now() + timeout;
         
         // TODO: Implement key_available.wait_until() with lock, deadline, and lambda predicate
         // Lambda should capture this and key by reference [this, &key]
         // Lambda should find key in cache and check if current time <= expiry_time
         // Return false if key not found, true if key exists and not expired
        return key_available.wait_until(lock, deadline, [this, &key](){
            auto it = cache.find(key);
            if(it == cache.end()) return false;

            return std::chrono::steady_clock::now() <= it -> second.expiry_time;});
         // TODO: Replace with actual wait_until implementation
    }
    
    // Background cleanup of expired items
    void cleanup_expired() {
        // TODO: Create while loop using cleanup_running condition
        while (cleanup_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // TODO: Create std::lock_guard<std::mutex> using cache_mutex
            std::lock_guard<std::mutex> cache_lock(cache_mutex);
            auto now = std::chrono::steady_clock::now();
            
            for (auto it = cache.begin(); it != cache.end();) {
                if (now > it->second.expiry_time) {
                    std::cout << "Cleaning up expired key: " << it->first << std::endl;
                    it = cache.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    
    void stop_cleanup() {
        cleanup_running = false;
    }
    
    // Get current cache size for demonstration
    size_t size() {
        // Lock the cache_mutex using std::lock_guard 
        return cache.size();
    }
};

// Thread function that stores values in cache
void producer_thread(TTLCache& cache, int thread_id) {
    for (int i = 0; i < 3; ++i) {
        std::string key = "key_" + std::to_string(thread_id) + "_" + std::to_string(i);
        int value = thread_id * 100 + i;
        
        // Store with different TTL values
        std::chrono::seconds ttl(2 + i); // 2-4 seconds TTL
        cache.put(key, value, ttl);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Thread function that retrieves values from cache
void consumer_thread(TTLCache& cache, int thread_id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Let producer start
    
    for (int i = 0; i < 2; ++i) {
        std::string key = "key_1_" + std::to_string(i); // Look for keys from thread 1
        
        // Try to get the value
        int value = cache.get(key);
        if (value != -1) {
            std::cout << "Consumer " << thread_id << " found " << key << " = " << value << std::endl;
        } else {
            std::cout << "Consumer " << thread_id << " waiting for " << key << std::endl;
            
            // Wait for key to become available
            if (cache.wait_for_key(key, std::chrono::seconds(3))) {
                value = cache.get(key);
                std::cout << "Consumer " << thread_id << " got " << key << " = " << value << " after waiting" << std::endl;
            } else {
                std::cout << "Consumer " << thread_id << " timed out waiting for " << key << std::endl;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    TTLCache cache;
    
    std::cout << "Starting TTL Cache demonstration..." << std::endl;
    std::cout << "Items will expire after their TTL period" << std::endl;
    std::cout << std::endl;
    
    // Start cleanup thread
    std::thread cleanup_thread(&TTLCache::cleanup_expired, &cache);
    
    // Start producer threads
    std::vector<std::thread> producers;
    for (int i = 1; i <= 2; ++i) {
        producers.emplace_back(producer_thread, std::ref(cache), i);
    }
    
    // Start consumer threads
    std::vector<std::thread> consumers;
    for (int i = 1; i <= 2; ++i) {
        consumers.emplace_back(consumer_thread, std::ref(cache), i);
    }
    
    // Wait for producers and consumers
    for (auto& t : producers) {
        t.join();
    }
    for (auto& t : consumers) {
        t.join();
    }
    
    std::cout << std::endl;
    std::cout << "Cache size before final cleanup: " << cache.size() << std::endl;
    
    // Wait a bit more to see cleanup in action
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    std::cout << "Cache size after cleanup: " << cache.size() << std::endl;
    
    // Stop cleanup thread
    cache.stop_cleanup();
    cleanup_thread.join();
    
    std::cout << "Exercise completed!" << std::endl;
    
    return 0;
} 
