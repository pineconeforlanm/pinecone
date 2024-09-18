//
// Created by 28192 on 2024/9/13.
//
#include <gtest/gtest.h>

#include <boost/version.hpp>
#include <pinecone/version.hpp>
#include <ylt/version.hpp>

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
      PROJECT_VERSION "." BUILD_TIME "-" GIT_VERSION "-" GIT_DATE "-" GIT_HASH};
  static constexpr std::string_view kThirdPartyBoostVersion{BOOST_LIB_VERSION};

  static constexpr auto kThirdPartyYltVersion{YLT_VERSION};
};

TEST_F(VersionTest, Base) {
  EXPECT_EQ(kBuildTime, pinecone::info::GetBuildTime());
  EXPECT_EQ(kMiniVersion, pinecone::info::GetMiniVersion());
  EXPECT_EQ(kDetailVersion, pinecone::info::GetDetailVersion());
  EXPECT_EQ(kGitData, pinecone::info::GetGitDate());
  EXPECT_EQ(kGitHash, pinecone::info::GetGitHash());
  EXPECT_EQ(kGitVersion, pinecone::info::GetGitVersion());
}

TEST_F(VersionTest, ThirdParty) {
  EXPECT_EQ(kThirdPartyBoostVersion,
            pinecone::info::third_party::GetBoostVersion());
  EXPECT_EQ(kThirdPartyYltVersion,
            pinecone::info::third_party::GetYltVersion());
}

}  // namespace pinecone::test