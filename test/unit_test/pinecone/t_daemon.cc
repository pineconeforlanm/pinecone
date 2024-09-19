//
// Created by 28192 on 2024/9/14.
//
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <pinecone/copyable.hpp>
#include <pinecone/daemon.hpp>

namespace pinecone {
namespace {
constexpr auto kNumber{3};

class ConfigPerson : public Copyable {
  std::string name_;
  int age_{};

 public:
  ConfigPerson(std::string_view name, int age) : name_(name), age_(age) {}

  auto operator==(const ConfigPerson& other) const -> bool { return name_ == other.name_ && age_ == other.age_; }
};

}  // namespace

class DaemonTest : public ::testing::Test {
 public:
  auto SetUp() -> void override {}

  auto TearDown() -> void override {}
};

TEST_F(DaemonTest, base) {}

}  // namespace pinecone