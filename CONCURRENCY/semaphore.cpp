// SEMAPHORE: it's just an {atomic unsigned int}. It's just a COUNTER! Where if the counter is 0, we wait!
// Binary semaphore == just a lock, same thing


// ATOMIC OPERATIONS:
// wait   (acquire)    == WAIT IF THE VALUE IS 0; the second it's positive decrement and return.
// signal (release)    == just increment and return.

// !!!!!!!!
// Semaphores basically apply whenever you have a situation where a producer and consumer must share data via an intermediary buffer!!!!!!
//
// 
//
// !!!!!!!!!


#include <iostream>
#include <string>
#include <semaphore>
#include <thread>
#include <chrono>

std::string delivery_box = "";                          // shared intermediary: can only hold 1 element

std::counting_semaphore<1> consumer_sem(0);             // Consumer_Sem = 0, num_items ready to be consumed (i.e. num ready elems I can eat)
std::counting_semaphore<1> producer_sem(1);             // Producer_Sem = 1, num_items ready to be produced (i.e. num empty spots I can write to)


// I CANNOT CONSUME IF THE SHARED delivery_box IS EMPTY
void consumer() {
    for (int i = 0; i < 3; ++i) {
        consumer_sem.acquire();             // Consumer_Sem: WAIT until there's an item ready to be consumed
        
        std::cout << "Consumer processed: " << delivery_box << std::endl;       // delivery box SHOULD have data in it now...
        
        producer_sem.release();             // Producer_Sem: increment, "you can produce one more now!"
    }
}


// I CANNOT CONSUME IF THE SHARED delivery_box IS FULL
void producer() {
    std::string orders[] = {"Buy AAPL", "Sell MSFT", "Buy GOOG"};

    for (const auto& order : orders) {
        producer_sem.acquire();              // Consumer_Sem: WAIT until there's an item ready to be produced
        
        delivery_box = order;
        std::cout << "Producer placed: " << order << std::endl;         // Safe to write: Consumer is guaranteed to be waiting for data
        
        consumer_sem.release();         // Signal to the consumer that data is ready
    }
}

int main() {
    // std::jthread automatically starts and joins when main exits
    std::jthread t1(consumer);
    std::jthread t2(producer);
    return 0;
}