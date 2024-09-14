//
// Created by 28192 on 2024/8/30.
//

#include <boost/version.hpp>
#include <pinecone/version.h>

namespace pinecone::info {
    namespace {
        constexpr std::string_view kMiniVersion{PROJECT_VERSION};
        constexpr std::string_view kGitVersion{GIT_VERSION};
        constexpr std::string_view kGitData{GIT_DATE};
        constexpr std::string_view kGitHash{GIT_HASH};
        constexpr std::string_view kBuildTime{BUILD_TIME};
        constexpr std::string_view kDetailVersion{
            PROJECT_VERSION "." BUILD_TIME "-" GIT_VERSION "-" GIT_DATE "-" GIT_HASH
        };
    }

    auto get_build_time() -> std::string_view {
        return kBuildTime;
    }

    auto get_mini_version() -> std::string_view {
        return kMiniVersion;
    }

    auto get_detail_version() -> std::string_view {
        return kDetailVersion;
    }

    auto get_git_version() -> std::string_view {
        return kGitVersion;
    }

    auto get_git_date() -> std::string_view {
        return kGitData;
    }

    auto get_git_hash() -> std::string_view {
        return kGitHash;
    }

    namespace third_party {
        auto get_boost_version() -> std::string_view {
            return BOOST_LIB_VERSION;
        }
    }
}
