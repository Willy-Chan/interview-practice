#include <thread>
#include <vector>
#include <string>
#include <shared_mutex>
#include <mutex>
#include <cassert>


class ConcurrentHashTable {
private:
    struct Bucket {
        std::vector<std::pair<int, std::string>> pairs;         // [(k, v), (k, v), ...]
        std::mutex mtx;                                         // Protect ONLY this single bucket from access
    };

    std::vector<Bucket> table;                                  // table, each entry points to a bucket list

    size_t get_index(int key) {
        return std::hash<int>{}(key) % table.size();            // hash(key) -> some index in range [0, table.size()]
    }

public:
    explicit ConcurrentMap(size_t num_buckets = 100) : table(num_buckets) {}        // initialize number of hashbuckets in the table

    // --- READ ---
    std::string get(int key) {
        Bucket& bucket = table[get_index(key)];                 // REFERENCE to the bucket: do not make a COPY of "bucket" in table
        std::unique_lock<std::mutex> lock(bucket.mtx);          // Lock this bucket's mutex (read-lock)

        // std::shared_lock<std::shared_mutex>

        // Does NOT allow concurrent reads: would need shared_lock and shared_mutex instead
        // More overhead, but would be better if you're in a read-heavy environemnt.

        for (const auto& pair : bucket.pairs) {
            if (pair.first == key) return pair.second;          // get the value
        }
        return ""; 
    }

    // --- WRITE ---
    void put(int key, const std::string& value) {
        Bucket& bucket = table[get_index(key)];                     // get the bucket
        std::unique_lock<std::shared_mutex> lock(bucket.mtx);       // lock while we're writing

        for (auto& pair : bucket.pairs) {
            if (pair.first == key) {
                pair.second = value;                                // modify the value!
                return;
            }
        }
        bucket.pairs.push_back({key, value});                       // if not existing, add it to the end
    }

    // --- DELETE ---
    void erase(int key) {
        Bucket& bucket = table[get_index(key)];
        std::unique_lock<std::shared_mutex> lock(bucket.mtx);       // write lock

        auto& pairs = bucket.pairs;
        for (auto it = pairs.begin(); it != pairs.end(); ++it) {        // erase every element of the vector
            if (it->first == key) {
                pairs.erase(it);
                return;
            }
        }
    }
};

// RUNNING THREADS
// #include <thread>
// 
// void worker(int id) { /* ... */ }
//
// std::thread t(worker, 42);                       // start thread running worker(42)
// std::thread t([&] { map.put(1, "hello"); });     // lambda function (capture everything by reference, [=] for copy, [map] for map var only)
// t.join();                                        // wait for it to finish


void stress_test() {
    ConcurrentHashTable map(100);               // instantiate a hash table
    int num_threads = 8;
    int ops_per_thread = 10'000;

    std::vector<std::thread> threads;
    threads.reserve(num_threads);                   // allocate space for 8 threads

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t] {                           // put a function into the threads vector
            for (int i = 0; i < ops_per_thread; ++i) {
                int key = (t * ops_per_thread + i) % 1000;  // key collisions on purpose
                map.put(key, "v" + std::to_string(key));
                std::string val = map.get(key);
                assert(val == "v" + std::to_string(key));
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }
}