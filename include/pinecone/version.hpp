//
// Created by 28192 on 2024/8/30.
//

#ifndef PINECONE_PINECONE_VERSION_H
#define PINECONE_PINECONE_VERSION_H

#include <boost/version.hpp>
#include <string_view>
#include <ylt/version.hpp>

namespace pinecone::info {
[[maybe_unused]] auto GetBuildTime() -> std::string_view;

[[maybe_unused]] auto GetMiniVersion() -> std::string_view;

[[maybe_unused]] auto GetDetailVersion() -> std::string_view;

[[maybe_unused]] auto GetGitVersion() -> std::string_view;

[[maybe_unused]] auto GetGitDate() -> std::string_view;

[[maybe_unused]] auto GetGitHash() -> std::string_view;

namespace third_party {
[[maybe_unused]] auto GetBoostVersion() -> std::string_view;
[[maybe_unused]] auto GetYltVersion() -> decltype(YLT_VERSION);
}  // namespace third_party
}  // namespace pinecone::info

#endif  // PINECONE_PINECONE_VERSION_H
