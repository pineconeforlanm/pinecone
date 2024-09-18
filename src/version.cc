//
// Created by 28192 on 2024/8/30.
//

#include <pinecone/version.hpp>

namespace pinecone::info {
namespace {
constexpr std::string_view kMiniVersion{PROJECT_VERSION};
constexpr std::string_view kGitVersion{GIT_VERSION};
constexpr std::string_view kGitData{GIT_DATE};
constexpr std::string_view kGitHash{GIT_HASH};
constexpr std::string_view kBuildTime{BUILD_TIME};
constexpr std::string_view kDetailVersion{
    PROJECT_VERSION "." BUILD_TIME "-" GIT_VERSION "-" GIT_DATE "-" GIT_HASH};
}  // namespace

auto GetBuildTime() -> std::string_view { return kBuildTime; }

auto GetMiniVersion() -> std::string_view { return kMiniVersion; }

auto GetDetailVersion() -> std::string_view { return kDetailVersion; }

auto GetGitVersion() -> std::string_view { return kGitVersion; }

auto GetGitDate() -> std::string_view { return kGitData; }

auto GetGitHash() -> std::string_view { return kGitHash; }

namespace third_party {
auto GetBoostVersion() -> std::string_view { return BOOST_LIB_VERSION; }
[[maybe_unused]] auto GetYltVersion() -> decltype(YLT_VERSION) {
  return YLT_VERSION;
}
}  // namespace third_party
}  // namespace pinecone::info
