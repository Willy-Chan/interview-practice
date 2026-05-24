// std::condition_variable: wake me up when I get signalled
// cv.wait(lock, [] { return wakeWhenThisConditionIsTrue })
// cv.wait(lock, [] { return !wakeWhenThisConditionIsFalse })
//                  VVVVVV
// while (!wakeWhenThisConditionIsTrue) {
//     cv.wait(lock); // Release lock and go to sleep
// }

// consumer does cv.wait(lock, [] { return wakeWhenThisConditionIsTrue })
// - give up the lock
// - THIS THREAD IS NOW ASLEEP
// - upon signal: (1) get the lock back and (2) check predicate to see if we can continue!

// producer does cv.notify_one() or cv.notify_all(): 
// As soon as Mr.Producer pushes data, it calls this to wake up one (or all) sleeping threads.

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

// Shared resources between the threads
std::string delivery_box = "";
std::mutex box_mutex;
std::condition_variable cv;

void consumer() {
    std::unique_lock<std::mutex> lock(box_mutex);

    cv.wait(lock, [] { return !delivery_box.empty(); });            // WAIT until delivery_box.empty() is FALSE
    
    std::cout << "Consumer processed: " << delivery_box << std::endl;       // we awoke and got the lock back!
}

void producer() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); 

    {
        std::unique_lock<std::mutex> lock(box_mutex);
        delivery_box = "Order #12457 - Apple Stock Buy";                // producer messes with the shared variable
        std::cout << "Producer placed the order!" << std::endl;

        // LOCK GET RELEASED HERE
    }

    cv.notify_one();                    // signal that consumer() can consume
}

int main() {
    // jthread automatically joins when main ends
    std::thread t1(consumer);
    std::thread t2(producer);

    t1.join();
    t2.join();
    return 0;
}