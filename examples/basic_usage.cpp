#include <round_robin/round_robin.hpp>
#include <iostream>
#include <string>

int main() {
    // Create a RoundRobin container of strings
    rr::RoundRobin<std::string> tasks;
    
    // Add some tasks
    tasks.add("Process emails");
    tasks.add("Check logs");
    tasks.add("Update database");
    
    std::cout << "Processing tasks in round-robin fashion:\n";
    
    // Process tasks for a few rounds
    int round = 1;
    auto it = tasks.begin();
    for (int i = 0; i < 9; ++i) {  // 3 tasks * 3 rounds
        if (it == tasks.begin()) {
            std::cout << "\nRound " << round++ << ":\n";
        }
        
        std::cout << "Processing task: " << *it << '\n';
        ++it;
    }
    
    // Demonstrate adding and removing tasks
    std::cout << "\nAdding new task 'Backup files'\n";
    tasks.add("Backup files");
    
    std::cout << "Processing with dynamic task list:\n";
    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        std::cout << "Current task: " << *it << '\n';
        
        // Remove 'Check logs' task when we encounter it
        auto next = std::next(it);
        if (next != tasks.end() && *next == "Check logs") {
            std::cout << "Removing 'Check logs' task\n";
            it.remove_next();
        }
    }
    
    return 0;
}
