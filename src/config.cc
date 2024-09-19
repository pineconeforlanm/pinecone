//
// Created by 28192 on 2024/9/13.
//

#include <filesystem>
#include <pinecone/config.hpp>

namespace pinecone::config {
namespace fs = std::filesystem;

namespace {
constexpr auto kDefaultConfigRootPath{std::string_view{"./config/"}};
constexpr auto kDefaultConfigFileSuffix = std::string_view{".yaml"};

auto ParseConfigToPath(std::string_view config_string, std::string_view root_path = kDefaultConfigRootPath)
    -> fs::path {
  std::string output{root_path};
  output.reserve(root_path.size() + config_string.size() + kDefaultConfigFileSuffix.size());
  for (const auto& iter : config_string) {
    if (iter == '.') {
      output += '/';
    } else {
      output += iter;
    }
  }
  output.append(kDefaultConfigFileSuffix);
  return fs::path{output};
}

auto CheckConfigRootExist(std::string_view path) -> bool {
  if (!fs::exists(path) || !fs::is_directory(path)) {
    ELOGW << "Config root path" << path << " directory does not exist";
    fs::create_directory(path);
    for (const auto& [name, value] : pinecone::config::Config::GetDatas()) {
      auto var_path = ParseConfigToPath(name, path);
      if (var_path.empty()) {
        ELOGW << var_path << " config var path is empty";
        continue;
      }
      fs::create_directories(var_path.parent_path());
      std::ofstream out{var_path};
      out << value->ToYaml();
    }
    return true;
  }
  return false;
}
}  // namespace

auto GetConfigDefaultRootPath() -> std::string_view { return kDefaultConfigRootPath; }

ConfigVarBase::ConfigVarBase(std::string_view name, std::string_view description) noexcept
    : name_(name), description_(description) {
  std::ranges::transform(name_, name_.begin(), ::tolower);
}

auto Config::LoadFromConfDir(std::string_view path) -> void {
  if (CheckConfigRootExist(path)) {
    return;
  }

  for (const auto& [name, value] : pinecone::config::Config::GetDatas()) {
    auto var_path = ParseConfigToPath(name, path);
    if (var_path.empty()) {
      ELOGW << var_path << " config var path is empty or does not exist";
      continue;
    }

    if (fs::is_directory(var_path) || !fs::exists(var_path)) {
      ELOGW << var_path << "is directory or does not exist";
      fs::remove_all(var_path);
      fs::create_directories(var_path.parent_path());
      std::ofstream out{var_path};
      out << value->ToYaml();
    } else if (fs::is_regular_file(var_path)) {
      std::ifstream const ifs{var_path};
      std::stringstream buffer;
      buffer << ifs.rdbuf();
      value->FromYaml(buffer.str());
    }
  }
}

auto Config::LookupBase(std::string_view name) -> ConfigVarBase::ptr {
  const auto& iter = GetDatas().find(name.data());
  return iter == GetDatas().end() ? nullptr : iter->second;
}
}  // namespace pinecone::config