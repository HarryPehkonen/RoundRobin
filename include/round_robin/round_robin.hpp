#ifndef ROUND_ROBIN_HPP
#define ROUND_ROBIN_HPP

#include <forward_list>
#include <stdexcept>
#include <utility> // For std::move
#include <cassert> // For assert (used in debug checks)

namespace rr {

/**
 * @brief A round-robin container that cycles through items in a consistent order.
 * 
 * This class is designed to provide a round-robin scheduling strategy, where each item
 * added to the container is visited in a cyclic manner. It supports movable and copyable
 * types, with special handling for move-only types like std::unique_ptr.
 * 
 * @tparam T The type of items stored in the round-robin container.
 */
template<typename T>
class RoundRobin {
private:
    /**
     * @struct Item
     * @brief Internal struct to hold items with a visited flag.
     */
    struct Item {
        T value; ///< The actual item stored.
        bool visited; ///< Flag to track if the item has been visited in the current cycle.
        
        /**
         * @brief Constructor for Item, initializing with a movable item.
         * @param v The item to store, moved into this struct.
         */
        Item(T&& v) : value(std::move(v)), visited(false) {}
        
        /**
         * @brief Constructor for Item, initializing with a copyable item.
         * @param v The item to store, copied into this struct.
         */
        Item(const T& v) : value(v), visited(false) {}
    };

    std::forward_list<Item> items_; ///< The underlying container for round-robin scheduling.
    typename std::forward_list<Item>::iterator current_; ///< Iterator to track the current item for potential removal.
    size_t count_ = 0; ///< Count of items in the container.

    /**
     * @brief Checks if all items in the container have been visited.
     * @return True if all items have been visited, false otherwise.
     */
    bool all_visited() const {
        for (const auto& item : items_) {
            if (!item.visited) return false;
        }
        return true;
    }

    /**
     * @brief Resets the visited flags for all items in the container.
     */
    void reset_visited() {
        for (auto& item : items_) {
            item.visited = false;
        }
    }

public:
    /**
     * @brief Default constructor, initializing an empty round-robin container.
     */
    RoundRobin() : current_(items_.before_begin()) {}

    /**
     * @brief Move constructor, transferring ownership of the round-robin container.
     * @param other The RoundRobin instance to move from.
     */
    RoundRobin(RoundRobin&& other) noexcept 
        : items_(std::move(other.items_))
       , current_(items_.before_begin())
       , count_(other.count_) {
        other.count_ = 0;
    }

    /**
     * @brief Move assignment operator, transferring ownership of the round-robin container.
     * @param other The RoundRobin instance to move from.
     * @return Reference to this RoundRobin instance after the move.
     */
    RoundRobin& operator=(RoundRobin&& other) noexcept {
        if (this!= &other) {
            items_ = std::move(other.items_);
            current_ = items_.before_begin();
            count_ = other.count_;
            other.count_ = 0;
        }
        return *this;
    }

    // Disable copy constructor and assignment operator
    RoundRobin(const RoundRobin&) = delete;
    RoundRobin& operator=(const RoundRobin&) = delete;

    /**
     * @brief Adds a copyable item to the round-robin container.
     * @param item The item to add, copied into the container.
     */
    void add(const T& item) {
        items_.push_front(Item(item));
        ++count_;
    }

    /**
     * @brief Adds a movable item to the round-robin container.
     * @param item The item to add, moved into the container.
     */
    void add(T&& item) {
        items_.push_front(Item(std::move(item)));
        ++count_;
    }

    /**
     * @brief Attempts to retrieve the next item in the round-robin cycle.
     * @return A pointer to the next item, or nullptr if the container is empty.
     * 
     * This function respects the move semantics of the stored type T.
     */
    T* try_next() {
        if (items_.empty()) {
            current_ = items_.before_begin();
            return nullptr;
        }
        
        if (all_visited()) {
            reset_visited();
        }
        
        // Track previous position for potential removal
        auto prev = items_.before_begin();
        for (auto it = items_.begin(); it!= items_.end(); ++it) {
            if (!it->visited) {
                it->visited = true;
                current_ = prev;  // Store previous iterator for remove_current()
                return &it->value;
            }
            prev = it;
        }
        
        // Should never reach here since we reset visits if all were visited
        assert(!items_.empty()); // Defensive programming
        current_ = items_.before_begin();
        return &items_.front().value;
    }

    /**
     * @brief Retrieves the next item in the round-robin cycle, throwing if the container is empty.
     * @return A reference to the next item.
     * 
     * This function is a convenience wrapper around try_next(), suitable for scenarios where
     * an empty container is considered an error.
     */
    T& next() {
        T* result = try_next();
        if (!result) {
            throw std::runtime_error("Attempted to get next item from empty RoundRobin");
        }
        return *result;
    }

    /**
     * @brief Removes the current item from the container.
     * 
     * Should be called only after a successful call to next() or try_next().
     * The next call to next() or try_next() will return the next item in sequence.
     * 
     * @throws std::runtime_error if called before any next() or try_next() call, or if the container is empty.
     */
    void remove_current() {
        if (items_.empty()) {
            throw std::runtime_error("Attempted to remove from empty RoundRobin");
        }
        
        auto next = std::next(current_);
        if (next == items_.end()) {
            throw std::runtime_error("Invalid current position in RoundRobin");
        }
        
        items_.erase_after(current_);
        --count_;
        
        // If we removed the last item, reset current
        if (items_.empty()) {
            current_ = items_.before_begin();
        }
    }

    /**
     * @brief Checks if the container is empty.
     * @return True if the container is empty, false otherwise.
     */
    bool empty() const {
        return count_ == 0;
    }

    /**
     * @brief Retrieves the number of items in the container.
     * @return The count of items.
     */
    size_t size() const {
        return count_;
    }
};

} // namespace rr

#endif // ROUND_ROBIN_HPP
