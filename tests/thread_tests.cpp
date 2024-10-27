#include <gtest/gtest.h>
#include "round_robin/round_robin.hpp"
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class AdvancedRoundRobinTest : public ::testing::Test {
protected:
    rr::RoundRobin<std::string> str_rr;
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;

    void SetUp() override {
        str_rr.add("A");
        str_rr.add("B");
        str_rr.add("C");
    }
};

// Multi-threaded access test with minimal locking
TEST_F(AdvancedRoundRobinTest, MultiThreadedAccess) {
    std::vector<std::string> results[3]; // Results for each thread
    std::thread threads[3];

    // Lambda function for each thread
    auto worker = [&](int id) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ return ready; }); // Wait for the green light

        for (int i = 0; i < 10; ++i) { // Run 10 iterations
            lock.unlock(); // Release lock before accessing RoundRobin
            results[id].push_back(str_rr.next());
            lock.lock(); // Reacquire lock for synchronization
        }
    };

    // Start threads and wait for them to finish
    for (int i = 0; i < 3; ++i) {
        threads[i] = std::thread(worker, i);
    }

    // Give threads the green light
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_all();

    // Wait for threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify that each thread saw all elements in the RoundRobin
    for (const auto& result : results) {
        std::set<std::string> uniqueElements(result.begin(), result.end());
        EXPECT_EQ(uniqueElements, (std::set<std::string>{"A", "B", "C"}));
    }
}

// Test removal during multi-threaded access
TEST_F(AdvancedRoundRobinTest, RemovalDuringMultiThreadedAccess) {
    std::queue<std::string> removalQueue;
    std::thread threads[2];

    // Lambda function for the remover thread
    auto remover = [&]() {
        for (int i = 0; i < 5; ++i) { // Remove 5 times
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate delay
            if (str_rr.empty()) {
                break;
            }
            std::string item = str_rr.next();
            {
                std::lock_guard<std::mutex> lock(mtx);
                removalQueue.push(item);
            }
            str_rr.remove_current();
        }
    };

    // Lambda function for the accessor thread
    auto accessor = [&]() {
        for (int i = 0; i < 10; ++i) { // Run 10 iterations
            std::string* item = str_rr.try_next();
            if (!item) { // Check if try_next returned nullptr (empty container)
                break; // Exit loop if empty
            }
            // Use *item to access the string
        }
    };

    threads[0] = std::thread(remover);
    threads[1] = std::thread(accessor);

    // Wait for threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify removals
    EXPECT_EQ(removalQueue.size(), 3);
    for (int i = 0; i < 3; ++i) {
        removalQueue.pop();
    }
}

// Stress test with many threads and iterations
TEST_F(AdvancedRoundRobinTest, StressTest) {
    const int numThreads = 10;
    const int numIterations = 100;

    std::thread threads[numThreads];
    std::vector<std::string> results[numThreads];

    // Lambda function for each thread
    auto worker = [&](int id) {
        for (int i = 0; i < numIterations; ++i) {
            results[id].push_back(str_rr.next());
        }
    };

    // Start threads
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = std::thread(worker, i);
    }

    // Wait for threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify that all threads collectively saw all elements
    std::set<std::string> uniqueElements;
    for (const auto& result : results) {
        uniqueElements.insert(result.begin(), result.end());
    }
    EXPECT_EQ(uniqueElements, (std::set<std::string>{"A", "B", "C"}));
}
