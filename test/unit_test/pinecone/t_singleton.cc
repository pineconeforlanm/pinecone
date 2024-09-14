//
// Created by 28192 on 2024/9/14.
//
#include <gtest/gtest.h>
#include <pinecone/singletion.h>

namespace pinecone {
    namespace {
        constexpr auto kNumber{3};
        struct ConfigPerson {
            std::string name;
            int age;

            auto operator==(const ConfigPerson &other) const -> bool {
                return name == other.name && age == other.age;
            }
        };
    }

    class SingletonTest : public ::testing::Test {
    public:
        auto SetUp() -> void override {
        }

        auto TearDown() -> void override {
        }
    };

    TEST_F(SingletonTest, base) {
        auto person_1 = pinecone::util::Singleton<ConfigPerson>::get_instance();
        auto person_2 = pinecone::util::Singleton<ConfigPerson>::get_instance();
        EXPECT_EQ(person_1, person_2);
        EXPECT_EQ(*person_1, *person_2);
    }

}