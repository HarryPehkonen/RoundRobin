#include <round_robin/round_robin.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

// Simulated worker that processes tasks
class Worker {
    rr::RoundRobin<std::string>& tasks_;
    int id_;
    std::atomic<bool>& should_stop_;
    
public:
    Worker(rr::RoundRobin<std::string>& tasks, int id, std::atomic<bool>& stop_flag)
        : tasks_(tasks), id_(id), should_stop_(stop_flag) {}
    
    void operator()() {
        while (!should_stop_) {
            // Try to get and process a task
            if (!tasks_.empty()) {
                auto it = tasks_.begin();
                if (it != tasks_.end()) {
                    // Process the task
                    std::cout << "Worker " << id_ << " processing: " << *it << '\n';
                    
                    // Simulate work
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    
                    // Remove the completed task
                    it.remove_next();
                }
            } else {
                // No tasks available, wait a bit
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
};

int main() {
    // Create a shared task queue
    rr::RoundRobin<std::string> tasks;
    
    // Add initial tasks
    tasks.add("Task 1");
    tasks.add("Task 2");
    tasks.add("Task 3");
    tasks.add("Task 4");
    tasks.add("Task 5");
    
    // Create stop flag and workers
    std::atomic<bool> should_stop(false);
    constexpr int num_workers = 3;
    std::vector<std::thread> workers;
    
    // Start worker threads
    std::cout << "Starting " << num_workers << " workers...\n";
    for (int i = 0; i < num_workers; ++i) {
        workers.emplace_back(Worker(tasks, i + 1, should_stop));
    }
    
    // Periodically add new tasks
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        auto task_name = "Dynamic Task " + std::to_string(i + 1);
        std::cout << "Adding new task: " << task_name << '\n';
        tasks.add(std::move(task_name));
    }
    
    // Wait until most tasks are processed
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Stop workers and wait for them to finish
    should_stop = true;
    for (auto& worker : workers) {
        worker.join();
    }
    
    std::cout << "All workers finished.\n";
    return 0;
}
