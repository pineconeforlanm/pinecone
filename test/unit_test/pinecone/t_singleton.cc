//
// Created by 28192 on 2024/9/14.
//
#include <gtest/gtest.h>

#include <pinecone/singleton.hpp>
#include <string>
namespace pinecone {
namespace {
struct ConfigPerson {
  std::string name;
  int age;

  auto operator==(const ConfigPerson &other) const -> bool {
    return name == other.name && age == other.age;
  }
};
}  // namespace

class SingletonTest : public testing::Test {
 public:
  auto SetUp() -> void override {}

  auto TearDown() -> void override {}
};

TEST_F(SingletonTest, base) {
  const auto *person_1 = Singleton<ConfigPerson>::GetInstance();
  const auto *person_2 = Singleton<ConfigPerson>::GetInstance();
  EXPECT_EQ(person_1, person_2);
  EXPECT_EQ(*person_1, *person_2);
}

}  // namespace pinecone