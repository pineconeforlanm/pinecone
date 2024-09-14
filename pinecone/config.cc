//
// Created by 28192 on 2024/9/13.
//

#include <pinecone/config.h>

#include <filesystem>

namespace pinecone::config {
    namespace fs = std::filesystem;
    namespace {
        constexpr auto kDefaultConfigRootPath{std::string_view{"./config/"}};
        constexpr auto kDefaultConfigFileSuffix = std::string_view{".yaml"};
        auto parse_config_to_path(std::string_view config_string, std::string_view root_path = kDefaultConfigRootPath) -> fs::path {
            std::string output{root_path};
            output.reserve(root_path.size() + config_string.size() + kDefaultConfigFileSuffix.size());
            for (char iter: config_string) {
                if (iter == '.') {
                    output += '/';
                } else {
                    output += iter;
                }
            }
            output += kDefaultConfigFileSuffix;
            return fs::path{output};
        }

        auto check_config_root_exist(std::string_view path) -> bool {
            if (!fs::exists(path) || !fs::is_directory(path)) {
                ELOGW << "Config root path" << path << " directory does not exist";
                fs::create_directory(path);
                for (auto const &config_str : pinecone::config::Config::get_datas()) {
                    auto var_path = parse_config_to_path(config_str.first, path);
                    if (var_path.empty()) {
                        ELOGW << var_path << " config var path is empty";
                        continue;
                    }
                    fs::create_directories(var_path.parent_path());
                    std::ofstream out{var_path};
                    out << config_str.second->to_yaml();
                }
                return true;
            }
            return false;
        }

    }

    auto get_config_default_root_path() -> std::string_view {
        return kDefaultConfigRootPath;
    }

    auto Config::load_from_conf_dir(std::string_view path) -> void {
        if (check_config_root_exist(path)) {
            return;
        }

        for (auto const& config_str: pinecone::config::Config::get_datas()) {
            auto var_path = parse_config_to_path(config_str.first, path);
            if (var_path.empty()) {
                ELOGW << var_path << " config var path is empty or does not exist";
                continue;
            }

            if (fs::is_directory(var_path) || !fs::exists(var_path)) {
                ELOGW << var_path << "is directory or does not exist";
                fs::remove_all(var_path);
                fs::create_directories(var_path.parent_path());
                std::ofstream out{var_path};
                out << config_str.second->to_yaml();
            } else if (fs::is_regular_file(var_path)) {
                std::ifstream const ifs{var_path};
                std::stringstream buffer;
                buffer << ifs.rdbuf();
                config_str.second->from_yaml(buffer.str());
            }
        }
    }

    auto Config::lookup_base(std::string_view name) -> ConfigVarBase::ptr {
        auto iter = get_datas().find(name.data());
        return iter == get_datas().end() ? nullptr : iter->second;
    }

}
