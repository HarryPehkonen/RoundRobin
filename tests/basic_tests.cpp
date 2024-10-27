#include <gtest/gtest.h>
#include "round_robin/round_robin.hpp"
#include <string>
#include <vector>

class RoundRobinTest : public ::testing::Test {
protected:
    rr::RoundRobin<std::string> str_rr;
    rr::RoundRobin<std::vector<int>> vec_rr;
    
    void SetUp() override {
        // Add test strings
        str_rr.add("first");
        str_rr.add("second");
        str_rr.add("third");
        
        // Add test vectors
        std::vector<int> v1{11, 12, 13};
        std::vector<int> v2{24, 25, 26, 27};
        std::vector<int> v3{37, 38, 39};
        vec_rr.add(std::move(v1));
        vec_rr.add(std::move(v2));
        vec_rr.add(std::move(v3));
    }
};

// Doesn't work:  Coverage report shows add(const T& item) is not
// covered.  This is because the RoundRobin class is a template.
// Add copyable.  It's already done, but this is just for coverage.
TEST_F(RoundRobinTest, AddCopyable) {
    rr::RoundRobin<std::string> rr;
    rr.add("1");
    rr.add("2");
    rr.add("3");
    EXPECT_EQ(rr.size(), 3);
}

// Basic string cycling tests
TEST_F(RoundRobinTest, StringCycling) {
    EXPECT_EQ(str_rr.next(), "third");  // LIFO order
    EXPECT_EQ(str_rr.next(), "second");
    EXPECT_EQ(str_rr.next(), "first");
    
    // Second round - should reset and return same order
    EXPECT_EQ(str_rr.next(), "third");
    EXPECT_EQ(str_rr.next(), "second");
    EXPECT_EQ(str_rr.next(), "first");
}

TEST_F(RoundRobinTest, TryNextBehavior) {
    EXPECT_EQ(*str_rr.try_next(), "third");
    EXPECT_EQ(*str_rr.try_next(), "second");
    EXPECT_EQ(*str_rr.try_next(), "first");
    EXPECT_EQ(*str_rr.try_next(), "third");  // Cycles back
    EXPECT_EQ(*str_rr.try_next(), "second");
    EXPECT_EQ(*str_rr.try_next(), "first");
}

// Vector processing tests
TEST_F(RoundRobinTest, VectorProcessing) {
    std::vector<int> processed;
    
    while (!vec_rr.empty()) {
        auto& current_vec = vec_rr.next();
        if (!current_vec.empty()) {
            processed.push_back(current_vec.back());
            current_vec.pop_back();
            
            if (current_vec.empty()) {
                vec_rr.remove_current();
            }
        }
    }
    
    // Should have processed all numbers 1-9 in reverse order within each vector
    std::vector<int> expected{39, 27, 13, 38, 26, 12, 37, 25, 11, 24};
    EXPECT_EQ(processed, expected);
}

TEST_F(RoundRobinTest, EmptyVectorRemoval) {
    // Empty the first vector
    auto& first = vec_rr.next();
    while (!first.empty()) {
        first.pop_back();
    }
    vec_rr.remove_current();
    
    EXPECT_EQ(vec_rr.size(), 2);  // Two vectors remaining
    
    auto& next = vec_rr.next();
    EXPECT_EQ(next.size(), 4);  // Next vector should be intact
}

// Edge cases
TEST_F(RoundRobinTest, EmptyContainer) {
    rr::RoundRobin<std::string> empty_rr;
    EXPECT_THROW(empty_rr.next(), std::runtime_error);
    EXPECT_EQ(empty_rr.try_next(), nullptr);
}

TEST_F(RoundRobinTest, SingleItem) {
    rr::RoundRobin<std::string> single;
    single.add("only");
    
    // Should keep returning the same item
    EXPECT_EQ(single.next(), "only");
    EXPECT_EQ(single.next(), "only");
    EXPECT_EQ(single.next(), "only");
}

// won't compile
// TEST(RoundRobinTest, CopyConstructorIsDeleted) {
//     rr::RoundRobin<int> rr1;
//     ASSERT_DEATH(rr::RoundRobin<int> rr2(rr1), ".*");
// }

// won't compile
// TEST(RoundRobinTest, CopyAssignmentIsDeleted) {
//     rr::RoundRobin<int> rr1, rr2;
//     ASSERT_DEATH(rr2 = rr1, ".*");
// }

TEST_F(RoundRobinTest, MoveConstructor) {
    rr::RoundRobin<std::string> moved(std::move(str_rr));
    EXPECT_EQ(moved.size(), 3);
    EXPECT_TRUE(str_rr.empty());
}

TEST_F(RoundRobinTest, MoveConstructorVectors) {
    rr::RoundRobin<std::vector<int>> moved(std::move(vec_rr));
    EXPECT_EQ(moved.size(), 3);
    EXPECT_TRUE(vec_rr.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
