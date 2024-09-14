//
// Created by 28192 on 2024/9/13.
//
#include <gtest/gtest.h>
#include <pinecone/config.h>

#include <filesystem>

namespace pinecone::test {
    struct ConfigPerson {
        std::string name;
        int age;

        auto operator==(const ConfigPerson &other) const -> bool {
            return name == other.name && age == other.age;
        }
    };

    namespace fs = std::filesystem;

    namespace {
        constexpr int kIntType{8080};
        constexpr int kIntSetType{9090};
        constexpr auto kFloatType{123.456};
        constexpr auto kDoubleType{123.456};

        const auto g_person = ConfigPerson{.name = "bob", .age = 36};
        auto pc_int_type = pinecone::config::Config::lookup("system.int", kIntType, "system int");
        auto pc_float_type = pinecone::config::Config::lookup("system.float", kFloatType, "system float");
        auto pc_double_type = pinecone::config::Config::lookup("system.double", kDoubleType, "system double");
        auto pc_person_type = pinecone::config::Config::lookup("user.person", ConfigPerson{"bob", 36}, "user person");
    }

    class ConfigTest : public ::testing::Test {
    public:
        static constexpr auto kDefaultConfigRootPath{std::string_view{"./config/"}};

        auto SetUp() -> void override {
        }

        auto TearDown() -> void override {
        }
    };

    TEST_F(ConfigTest, GetConfigRootPath) {
        EXPECT_EQ(kDefaultConfigRootPath, pinecone::config::get_config_default_root_path());
    }

    TEST_F(ConfigTest, ConfigValue) {
        auto exists = pinecone::config::Config::lookup_base("system.int");
        EXPECT_TRUE(exists);
        EXPECT_EQ("int", pc_int_type->get_type_name());
        EXPECT_EQ(kIntType, pc_int_type->get_value());
        auto pc_int_get_type = pinecone::config::Config::lookup<int>("system.int");
        EXPECT_EQ(kIntType, pc_int_get_type->get_value());
        pc_int_type->set_value(kIntSetType);
        EXPECT_EQ(kIntSetType, pc_int_type->get_value());
        EXPECT_EQ(kFloatType, pc_float_type->get_value());
        EXPECT_EQ(kDoubleType, pc_double_type->get_value());
        EXPECT_EQ(g_person, pc_person_type->get_value());
        pc_int_type->set_value(kIntType);
        pc_int_type->set_value(kIntType);
    }

    TEST_F(ConfigTest, ConfigBadValue) {
        const auto k_delete_system_root_path = fs::path{kDefaultConfigRootPath};
        const auto k_delete_system_int_path = fs::path{"./config/system/int.yaml"};
        const auto k_delete_system_double_path = fs::path{"./config/system/double.yaml"};

        fs::remove_all(k_delete_system_int_path);
        fs::remove_all(k_delete_system_double_path);
        fs::create_directory(k_delete_system_double_path);
        try {
            auto pc_search_int_type = pinecone::config::Config::lookup("system.int", kIntType, "system int");
            auto pc_bad_int_type = pinecone::config::Config::lookup("*system.int", kIntType, "system int");
        } catch (const std::exception &e) {
            /// todo nothing
            [[maybe_unused]] const auto *exc = e.what();
        }
        auto exists = pinecone::config::Config::lookup_base("system.val");
        EXPECT_FALSE(exists);

        EXPECT_EQ("int", pc_int_type->get_type_name());

        auto pc_int_get_type = pinecone::config::Config::lookup<int>("system.int");
        EXPECT_EQ(kIntType, pc_int_get_type->get_value());
        auto pc_bad_int_get_type = pinecone::config::Config::lookup<int>("system.bad_int");
        EXPECT_FALSE(pc_bad_int_get_type);

        pc_int_type->set_value(kIntSetType);
        EXPECT_EQ(kIntSetType, pc_int_type->get_value());

        pinecone::config::Config::load_from_conf_dir();
        EXPECT_EQ(kIntSetType, pc_int_type->get_value());
        fs::remove_all(k_delete_system_root_path);
        pinecone::config::Config::load_from_conf_dir();
    }

    TEST_F(ConfigTest, ConfigValueListener) {
        auto on_change_cb = [](int old_val, int new_val) { };
        on_change_cb(3, 5);
        auto key = pc_int_type->add_listener(on_change_cb);
        [[maybe_unused]] auto g_cb = pc_int_type->get_listener(key);
        pc_int_type->del_listener(key);
        pc_int_type->add_listener(on_change_cb);
        pc_int_type->clear_listener();
    }

    TEST_F(ConfigTest, base) {
        EXPECT_EQ("system.int", pc_int_type->get_name());
        EXPECT_EQ("system int", pc_int_type->get_description());
    }
}
