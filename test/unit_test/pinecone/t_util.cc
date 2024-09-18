//
// Created by 28192 on 2024/9/13.
//
#include <gtest/gtest.h>

#include <pinecone/util.hpp>

namespace pinecone::test {
class UtilTest : public ::testing::Test {
 public:
  static constexpr auto kDefaultConfigRootPath{std::string_view{"./config/"}};

  auto SetUp() -> void override {}

  auto TearDown() -> void override {}
};

TEST_F(UtilTest, base) {
  static auto s_host_name = std::array<char, 1024>{0};
  const auto status = gethostname(s_host_name.data(), s_host_name.size());
  EXPECT_EQ(std::string_view{s_host_name.data()},
            pinecone::util::GetHostName());
  EXPECT_EQ(std::string_view{s_host_name.data()},
            pinecone::util::GetHostName());

  EXPECT_EQ("int", pinecone::util::TypeToName<int>());
  EXPECT_EQ("long", pinecone::util::TypeToName<long>());
  EXPECT_EQ("long long", pinecone::util::TypeToName<long long>());

  EXPECT_EQ(static_cast<pid_t>(syscall(SYS_gettid)),
            pinecone::util::GetThreadId());
  EXPECT_EQ(static_cast<pid_t>(syscall(SYS_gettid)),
            pinecone::util::GetThreadId());
}
}  // namespace pinecone::test