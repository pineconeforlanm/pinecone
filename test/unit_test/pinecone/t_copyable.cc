//
// Created by 28192 on 2024/9/14.
//
#include <gtest/gtest.h>

#include <pinecone/copyable.hpp>
#include <string>
#include <string_view>

namespace pinecone {
namespace {
class ConfigPerson : public Copyable {
  std::string name_;
  int age_{};

 public:
  ConfigPerson(std::string_view name, int age) : name_(name), age_(age) {}

  auto operator==(const ConfigPerson &other) const -> bool {
    return name_ == other.name_ && age_ == other.age_;
  }
};

}  // namespace

class CopyableTest : public testing::Test {
 public:
  auto SetUp() -> void override {}

  auto TearDown() -> void override {}
};

TEST_F(CopyableTest, base) {
  const auto person_1 = ConfigPerson("zhang_san", 18);
  const auto person_2 = person_1;
  EXPECT_EQ(person_1, person_2);
}

}  // namespace pinecone