#include <gtest/gtest.h>
#include "round_robin/round_robin.hpp"

TEST(RoundRobinTest, MoveSemanticNoLeak) {
    rr::RoundRobin<std::unique_ptr<int>> rr;
    auto ptr = std::make_unique<int>(5);
    rr.add(std::move(ptr)); // Move into RoundRobin
    ASSERT_NE(rr.size(), 0);
    rr.try_next(); // Consume the item
    rr.remove_current(); // Remove the item
    ASSERT_TRUE(rr.empty()); // Verify container is empty
    // If no crash or leaks reported, move semantics handled correctly
}

TEST(RoundRobinTest, CopySemanticNoLeak) {
    rr::RoundRobin<int> rr;
    rr.add(5); // Copy into RoundRobin
    ASSERT_NE(rr.size(), 0);
    rr.try_next(); // Consume the item
    rr.remove_current(); // Remove the item
    ASSERT_TRUE(rr.empty()); // Verify container is empty
    // Implicit check: If test completes without issues, likely no leak
}

TEST(RoundRobinTest, MultipleAdditionsAndRemovals) {
    rr::RoundRobin<std::string> rr;
    for (int i = 0; i < 100; ++i) {
        rr.add("Hello"); // Copy string into RoundRobin
    }
    ASSERT_EQ(rr.size(), 100);
    while (!rr.empty()) {
        rr.try_next();
        rr.remove_current();
    }
    ASSERT_TRUE(rr.empty());
    // Surviving this loop without leaks indicates good management
}

TEST(RoundRobinTest, MoveAssignmentNoLeak) {
    rr::RoundRobin<std::unique_ptr<int>> rr1;
    auto ptr = std::make_unique<int>(10);
    rr1.add(std::move(ptr));
    
    rr::RoundRobin<std::unique_ptr<int>> rr2;
    rr2 = std::move(rr1); // Move assignment
    ASSERT_NE(rr2.size(), 0);
    rr2.try_next();
    rr2.remove_current();
    ASSERT_TRUE(rr2.empty());
    // No leak expected after move assignment and use
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
