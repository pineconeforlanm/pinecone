//
// Created by 28192 on 2024/9/14.
//
#include <gtest/gtest.h>
#include <pinecone/threadpool.h>

namespace pinecone {
    namespace {
        constexpr auto kNumber{3};
    }

class ThreadPoolTest : public ::testing::Test {
public:
    auto SetUp() -> void override {
    }

    auto TearDown() -> void override {
    }
};

    TEST_F(ThreadPoolTest, base) {
        auto& pool = pinecone::base::ThreadPool::instance();
        [[maybe_unused]] auto count = pool.get_idle_thread_count();
        auto result = pool.commit([](){
            return kNumber;
        });
        EXPECT_EQ(kNumber, result.get());
    }

}