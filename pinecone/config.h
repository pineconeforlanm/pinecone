//
// Created by 28192 on 2024/9/13.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <pinecone/util.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <ylt/easylog.hpp>
#include <ylt/struct_yaml/yaml_reader.h>
#include <ylt/struct_yaml/yaml_writer.h>

namespace pinecone::config {
    [[maybe_unused]] auto get_config_default_root_path() -> std::string_view;

    class ConfigVarBase {
    public:
        using ptr = std::shared_ptr<ConfigVarBase>;

        explicit ConfigVarBase(std::string_view name, std::string_view description = "") noexcept : name_(name),
            description_(description) {
            std::ranges::transform(name_, name_.begin(), ::tolower);
        }

        virtual ~ConfigVarBase() = default;

        [[nodiscard]] auto get_name() const noexcept -> std::string_view { return name_; }
        [[nodiscard]] auto get_description() const noexcept -> std::string_view { return description_; }

        [[nodiscard]] virtual auto to_yaml() const -> std::string = 0;

        virtual auto from_yaml(std::string_view yaml_str) -> void = 0;

        [[nodiscard]] virtual auto get_type_name() const noexcept -> std::string_view = 0;

    protected:
        std::string name_;
        std::string description_;
    };

    template<typename T>
    class ConfigVar : public ConfigVarBase {
    public:
        using ptr = std::shared_ptr<ConfigVar>;
        using on_change_cb =
        std::function<void(const T &old_value, const T &new_value)>;

        ConfigVar() noexcept : ConfigVarBase(pinecone::util::type_to_name<T>(), "") {}
        ConfigVar(std::string_view name, const T &default_value,
                  std::string_view description = "") noexcept : ConfigVarBase(name, description),
                                                                value_(default_value) {
        }

        [[nodiscard]] auto to_yaml() const noexcept -> std::string override {
            try {
                std::string yaml_str;
                struct_yaml::to_yaml(*this, yaml_str);
                return yaml_str;
            } catch (std::exception &e) {
                ELOGE << "ConfigVar::toString exception " << e.what() << " convert: " << get_type_name() << " to string" << " name = " << name_;
            }
            return {};
        }

        auto from_yaml(std::string_view yaml_str) -> void override {
            try {
                struct_yaml::from_yaml(*this, yaml_str);
            } catch (std::exception &e) {
                ELOGE << "ConfigVar::fromString exception " << e.what() << " convert: string to" << get_type_name() << " name=" << name_ << " - " << yaml_str;
            }
        }

        auto set_value(T value) -> void {
            if (value_ == value) {
                return;
            }

            for (auto &iter: cbs_) {
                iter.second(value_, value);
            }
            value_ = value;
        }

        auto get_value() const noexcept -> T {
            return value_;
        }

        [[nodiscard]] auto get_type_name() const noexcept -> std::string_view override {
            return pinecone::util::type_to_name<T>();
        }

        auto add_listener(on_change_cb change_cb) -> uint64_t {
            static uint64_t s_func_id = 0;
            ++s_func_id;
            cbs_[s_func_id] = change_cb;
            return s_func_id;
        }

        auto del_listener(uint64_t key) -> void {
            cbs_.erase(key);
        }

        auto get_listener(uint64_t key) const -> on_change_cb {
            auto iter = cbs_.find(key);
            return iter == cbs_.end() ? nullptr : iter->second;
        }

        auto clear_listener() -> void {
            cbs_.clear();
        }

        YLT_REFL(ConfigVar, value_);
    private:
        T value_;
        std::unordered_map<uint64_t, on_change_cb> cbs_;
    };

    class Config {
    public:
        using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;

        template<class T>
        static auto
        lookup(std::string_view name, const T &default_value,
               std::string_view description = "") -> typename ConfigVar<T>::ptr {
            auto iter = get_datas().find(name.data());
            if (iter != get_datas().end()) {
                if (auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(iter->second)) {
                    ELOGI << "Lookup name=" << name << " exists";
                    return tmp;
                }
                ELOGE << "Lookup name=" << name << " exists but type not " << pinecone::util::type_to_name<T>() << " real_type=" << iter->second->get_type_name() << " " << iter->second->to_yaml();
                return nullptr;
            }

            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
                ELOGE << "Lookup name invalid " << name;
                throw std::invalid_argument(name.data());
            }

            auto value = std::make_shared<ConfigVar<T> >(name, default_value, description);
            get_datas()[name.data()] = value;
            return value;
        }

        template<class T>
        static auto lookup(std::string_view name) -> typename ConfigVar<T>::ptr {
            const auto item = get_datas().find(name.data());
            if (item == get_datas().end()) {
                return nullptr;
            }

            return std::dynamic_pointer_cast<ConfigVar<T> >(item->second);
        }

        static auto load_from_conf_dir(std::string_view path = get_config_default_root_path())  -> void;

        static auto lookup_base(std::string_view name) -> ConfigVarBase::ptr;

        static auto get_datas() -> ConfigVarMap & {
            static ConfigVarMap s_datas;
            return s_datas;
        }
    };
}

#endif //CONFIG_H
