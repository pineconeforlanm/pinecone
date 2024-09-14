//
// Created by 28192 on 2024/9/13.
//
#include <boost/version.hpp>
#include <gtest/gtest.h>
#include <pinecone/version.h>

namespace pinecone::test {

    class VersionTest : public testing::Test {
    public:
        auto SetUp() -> void override {}
        auto TearDown() -> void override {}
        static constexpr std::string_view kMiniVersion{PROJECT_VERSION};
        static constexpr std::string_view kGitVersion{GIT_VERSION};
        static constexpr std::string_view kGitData{GIT_DATE};
        static constexpr std::string_view kGitHash{GIT_HASH};
        static constexpr std::string_view kBuildTime{BUILD_TIME};
        static constexpr std::string_view kDetailVersion{
            PROJECT_VERSION "." BUILD_TIME "-" GIT_VERSION "-" GIT_DATE "-" GIT_HASH
        };
        static constexpr std::string_view kThirdPartyBoostVersion{BOOST_LIB_VERSION};
    };

    TEST_F(VersionTest, Base) {
        EXPECT_EQ(kBuildTime, pinecone::info::get_build_time());
        EXPECT_EQ(kMiniVersion, pinecone::info::get_mini_version());
        EXPECT_EQ(kDetailVersion, pinecone::info::get_detail_version());
        EXPECT_EQ(kGitData, pinecone::info::get_git_date());
        EXPECT_EQ(kGitHash, pinecone::info::get_git_hash());
        EXPECT_EQ(kGitVersion, pinecone::info::get_git_version());
        EXPECT_EQ(kThirdPartyBoostVersion, pinecone::info::third_party::get_boost_version());
    }

}