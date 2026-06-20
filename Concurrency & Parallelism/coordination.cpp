/* To compile and run:
g++ exercise_1_starter.cpp -o running_exercise -pthread && ./running_exercise

Expected behavior:
- Threads finish preparation at different times
- But announcements happen in predetermined order (1, 2, 3, 4)
- Later threads wait for earlier threads even if ready first

YOUR TASKS:
1. Declare mutex and condition_variable for coordination infrastructure
2. Initialize execution counter in constructor for coordination state tracking
3. Create unique_lock for RAII-based critical section protection
4. Implement predicate-based wait() for execution order control
5. Increment counter and notify waiting threads for sequential coordination
*/

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <chrono>

class OrderedPrinter {
private:
    // TODO: Declare std::mutex variable named: coordination_mutex
    // TODO: Declare std::condition_variable variable named: turn_signal
    // TODO: Declare int variable named: next_thread_to_execute
    std::mutex coordination_mutex;
    std::condition_variable turn_signal;
    int next_thread_to_execute;

    
public:
    // TODO: Initialize next_thread_to_execute to 1 in constructor
    OrderedPrinter() {
        // TODO: Constructor initialization
        next_thread_to_execute = 1;
    }
    
    void ordered_print(int thread_id, const std::string& location_name) {
        // TODO: Create std::unique_lock<std::mutex> using coordination_mutex
        // Store in variable named: lock
        std::unique_lock<std::mutex> lock(coordination_mutex);
        // TODO: Implement turn_signal.wait() with lambda predicate
        // Lambda should capture this and thread_id, return true when next_thread_to_execute == thread_id
        turn_signal.wait([this, int thread_id] () {return next_thread_to_execute == thread_id});
        
        // This thread's turn - execute the printing sequence
        std::cout << "=== " << location_name << " Restaurant Announcement ===" << std::endl;
        std::cout << "Thread " << thread_id << " (" << location_name << ") starting daily specials announcement..." << std::endl;
        
        // Simulate announcement time (while holding coordination lock)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::cout << "Thread " << thread_id << ": Today's special is Grilled Salmon with herbs!" << std::endl;
        std::cout << "Thread " << thread_id << ": Fresh ingredients sourced this morning!" << std::endl;
        std::cout << "Thread " << thread_id << ": Available until supplies last!" << std::endl;
        std::cout << "Thread " << thread_id << " (" << location_name << ") completed announcement." << std::endl;
        std::cout << std::endl;
        
        // TODO: Increment next_thread_to_execute counter
        // TODO: Call turn_signal.notify_all() to signal waiting threads
        next_thread_to_execute++;
        turn_signal.notify_all();
    }
};

// Restaurant thread function
void restaurant_announcement(OrderedPrinter& printer, int thread_id, const std::string& location) {
    // Simulate different preparation times - later threads finish prep first!
    int prep_time = 200 * (5 - thread_id); // Thread 4 finishes first, Thread 1 last
    std::this_thread::sleep_for(std::chrono::milliseconds(prep_time));
    
    std::cout << "[Background] Thread " << thread_id << " (" << location 
              << ") finished preparation, waiting for turn..." << std::endl;
    
    // Execute in coordinated order (not preparation order)
    printer.ordered_print(thread_id, location);
    
    std::cout << "[Background] Thread " << thread_id << " (" << location 
              << ") completed and ready for customers!" << std::endl;
}

int main() {
    OrderedPrinter printer;
    const int num_locations = 4;
    
    std::cout << "=== Carmen's Restaurant Chain Daily Specials Coordination ===" << std::endl;
    std::cout << "Multiple locations must announce specials in predetermined order" << std::endl;
    std::cout << "for brand consistency, regardless of preparation timing." << std::endl;
    std::cout << "Notice: Later threads will finish prep first but wait their turn!" << std::endl;
    std::cout << std::endl;
    
    // Restaurant locations
    std::vector<std::string> locations = {
        "Downtown", "Uptown", "Midtown", "Riverside"
    };
    
    std::vector<std::thread> restaurant_threads;
    
    // Create threads (they will finish preparation at different times)
    std::cout << "Starting all restaurant locations simultaneously..." << std::endl;
    for (int i = 1; i <= num_locations; ++i) {
        restaurant_threads.emplace_back(
            restaurant_announcement, 
            std::ref(printer), 
            i, 
            locations[i-1]
        );
    }
    
    // Wait for all restaurants to complete their coordinated announcements
    for (auto& t : restaurant_threads) {
        t.join();
    }
    
    std::cout << "=== All restaurant locations completed announcements in proper order! ===" << std::endl;
    std::cout << "Brand consistency maintained through mutex-based coordination." << std::endl;
    std::cout << std::endl;
    std::cout << "Key Points Demonstrated:" << std::endl;
    std::cout << "1. Threads executed in predetermined order (1->2->3->4)" << std::endl;
    std::cout << "2. Order enforced by mutex + condition variable coordination" << std::endl;
    std::cout << "3. Later threads waited even when ready first" << std::endl;
    std::cout << "4. Mutex controlled execution timing, not just data protection" << std::endl;
    
    return 0;
} 
