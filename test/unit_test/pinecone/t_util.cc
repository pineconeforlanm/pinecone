//
// Created by 28192 on 2024/9/13.
//
#include <gtest/gtest.h>
#include <pinecone/util.h>

namespace pinecone::test {
    class UtilTest : public ::testing::Test {
    public:
        static constexpr auto kDefaultConfigRootPath{std::string_view{"./config/"}};

        auto SetUp() -> void override {
        }

        auto TearDown() -> void override {
        }
    };

    TEST_F(UtilTest, base) {
        static auto s_host_name = std::array<char, 1024>{0};
        const auto status = gethostname(s_host_name.data(), s_host_name.size());
        EXPECT_EQ(std::string_view{s_host_name.data()}, pinecone::util::get_host_name());
        EXPECT_EQ(std::string_view{s_host_name.data()}, pinecone::util::get_host_name());

        EXPECT_EQ("int", pinecone::util::type_to_name<int>());
        EXPECT_EQ("long", pinecone::util::type_to_name<long>());
        EXPECT_EQ("long long", pinecone::util::type_to_name<long long>());

        EXPECT_EQ(static_cast<pid_t>(syscall(SYS_gettid)), pinecone::util::get_thread_id());
        EXPECT_EQ(static_cast<pid_t>(syscall(SYS_gettid)), pinecone::util::get_thread_id());
    }
}