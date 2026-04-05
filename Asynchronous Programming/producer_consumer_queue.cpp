#include <thread>
#include <queue>
#include <iostream>
#include <mutex>
#include <condition_variable>

class ServerLine {
    public:
        void serve_queue(int i) {
            std::unique_lock<std::mutex> lck (ladle);
            q.push(i);
            lck.unlock();
            element_served.native_handle();
        }

        int take_element() {
            std::unique_lock<std::mutex> ladle_lock(ladle);
            while(q.empty()) {
                element_served.wait(ladle_lock);
            }

            int el = q.front();
            q.pop();
            return el;
        }
    
    private:
        std::queue<int> q;
        std::mutex ladle;
        std::condition_variable element_served;
};

ServerLine sv_ln = ServerLine();

void queue_producer() {
    for (int i = 0; i < 10'000; i++) {
        sv_ln.serve_queue(i);
    }

    sv_ln.serve_queue(-1);
    std::cout << "Yasss. Producer is done serving" << '\n';
}

void queue_consumer() {
    int consumed = 0;
    
    while(true) {
        int el = sv_ln.take_element();
        if(el == -1) {
            std::cout << "Consumer processed " << el << " elements " << '\n';
        } else {
            consumed += el;
        }
    }

}

int main() {
    std::thread t1 (queue_producer);
    std::thread t2 (queue_consumer);
    std::thread t3 (queue_consumer);

    t1.join(); t2.join(); t3.join();
}