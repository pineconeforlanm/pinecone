//
// Created by 28192 on 2024/9/13.
//
#include <gtest/gtest.h>
#include <ylt/struct_yaml/yaml_reader.h>
#include <ylt/struct_yaml/yaml_writer.h>
#include <exception>
#include <pinecone/config.hpp>
#include <filesystem>
#include <string>

namespace pinecone::test {
struct ConfigPerson {
  std::string name;
  int age;

  auto operator==(const ConfigPerson &other) const -> bool { return name == other.name && age == other.age; }
};
YLT_REFL(ConfigPerson, name, age);
}

PINECONE_CONFIG_LEXICAL_CAST_YAML(pinecone::test::ConfigPerson);

namespace pinecone::test {

namespace fs = std::filesystem;

namespace {
constexpr int kIntType{8080};
constexpr int kIntSetType{9090};
constexpr auto kFloatType{123.456};
constexpr auto kDoubleType{123.456};

const auto g_person = ConfigPerson{.name = "bob", .age = 36};
auto pc_int_type = Config::Lookup("system.int", kIntType, "system int");
auto pc_float_type = Config::Lookup("system.float", kFloatType, "system float");
auto pc_double_type = Config::Lookup("system.double", kDoubleType, "system double");
auto pc_person_type = Config::Lookup("user.person", ConfigPerson{.name="bob", .age=36}, "user person");
}  // namespace

class ConfigTest : public testing::Test {
 public:
  static constexpr auto kDefaultConfigRootPath{std::string_view{"./config/"}};

  auto SetUp() -> void override {}

  auto TearDown() -> void override {}
};

TEST_F(ConfigTest, GetConfigRootPath) {
  EXPECT_EQ(kDefaultConfigRootPath, pinecone::kDefaultConfigRootPath);
}

TEST_F(ConfigTest, ConfigParse) {
  constexpr auto kIntTypeStr{std::string_view{"8080"}};
  const auto str = pc_int_type->ToString();
  EXPECT_EQ(kIntTypeStr, str);
  pc_int_type->FromString(str);
  EXPECT_EQ(kIntType, pc_int_type->GetValue());
}

TEST_F(ConfigTest, ConfigValue) {
  const auto exists = Config::LookupBase("system.int");
  EXPECT_TRUE(exists);
  EXPECT_EQ("int", pc_int_type->GetTypeName());
  EXPECT_EQ(kIntType, pc_int_type->GetValue());
  const auto pc_int_get_type = Config::Lookup<int>("system.int");
  EXPECT_EQ(kIntType, pc_int_get_type->GetValue());
  pc_int_type->SetValue(kIntSetType);
  EXPECT_EQ(kIntSetType, pc_int_type->GetValue());
  EXPECT_EQ(kFloatType, pc_float_type->GetValue());
  EXPECT_EQ(kDoubleType, pc_double_type->GetValue());
  EXPECT_EQ(g_person, pc_person_type->GetValue());
  pc_int_type->SetValue(kIntType);
  pc_int_type->SetValue(kIntType);
}

TEST_F(ConfigTest, ConfigBadValue) {
  try {
    const auto pc_search_int_type = Config::Lookup("system.int", kIntType, "system int");
    const auto pc_bad_int_type = Config::Lookup("*system.int", kIntType, "system int");
  } catch (const std::exception &e) {
    /// todo nothing
    [[maybe_unused]] const auto *exc = e.what();
  }
  const auto exists = Config::LookupBase("system.val");
  EXPECT_FALSE(exists);

  EXPECT_EQ("int", pc_int_type->GetTypeName());

  const auto pc_int_get_type = Config::Lookup<int>("system.int");
  EXPECT_EQ(kIntType, pc_int_get_type->GetValue());
  const auto pc_bad_int_get_type = Config::Lookup<int>("system.bad_int");
  EXPECT_FALSE(pc_bad_int_get_type);

  pc_int_type->SetValue(kIntSetType);
  EXPECT_EQ(kIntSetType, pc_int_type->GetValue());

  Config::LoadFromConfDir();
  EXPECT_EQ(kIntSetType, pc_int_type->GetValue());
  Config::LoadFromConfDir();
  Config::LoadFromConfFile();
  std::filesystem::remove(kDefaultConfigFilePath.data());
  Config::LoadFromConfFile();
}

TEST_F(ConfigTest, ConfigValueListener) {
  const auto on_change_cb = []([[maybe_unused]] int old_val, [[maybe_unused]] int new_val) {};
  on_change_cb(3, 5);
  const auto key = pc_int_type->AddListener(on_change_cb);
  [[maybe_unused]] auto g_cb = pc_int_type->GetListener(key);
  pc_int_type->SetValue(kIntType);
  pc_int_type->DelListener(key);
  pc_int_type->AddListener(on_change_cb);
  pc_int_type->ClearListener();
}

TEST_F(ConfigTest, base) {
  EXPECT_EQ("system.int", pc_int_type->GetName());
  EXPECT_EQ("system int", pc_int_type->GetDescription());
}
}  // namespace pinecone::test
