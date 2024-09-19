//
// Created by 28192 on 2024/9/13.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <ylt/struct_yaml/yaml_reader.h>
#include <ylt/struct_yaml/yaml_writer.h>

#include <memory>
#include <mutex>
#include <pinecone/util.hpp>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <ylt/easylog.hpp>

namespace pinecone::config {
constexpr auto kDefaultNameConvention{std::string_view{"abcdefghijklmnopqrstuvwxyz._0123456789"}};

[[maybe_unused]] auto GetConfigDefaultRootPath() -> std::string_view;

class ConfigVarBase {
 public:
  using ptr = std::shared_ptr<ConfigVarBase>;

  explicit ConfigVarBase(std::string_view name, std::string_view description = "") noexcept;

  virtual ~ConfigVarBase() = default;

  [[nodiscard]] auto GetName() const noexcept -> std::string_view { return name_; }

  [[nodiscard]] auto GetDescription() const noexcept -> std::string_view { return description_; }

  [[nodiscard]] virtual auto ToYaml() const -> std::string = 0;

  virtual auto FromYaml(std::string_view yaml_str) -> void = 0;

  [[nodiscard]] virtual auto GetTypeName() const noexcept -> std::string_view = 0;

 protected:
  std::string name_;
  std::string description_;
};

template <typename T>
class ConfigVar : public ConfigVarBase {
 public:
  using ptr = std::shared_ptr<ConfigVar>;
  using on_change_cb = std::function<void(const T &old_value, const T &new_value)>;

  ConfigVar() noexcept : ConfigVarBase(pinecone::util::TypeToName<T>(), "") {}

  ConfigVar(std::string_view name, const T &default_value, std::string_view description = "") noexcept
      : ConfigVarBase(name, description), value_(default_value) {}

  [[nodiscard]] auto ToYaml() const noexcept -> std::string override {
    try {
      std::string yaml_str;
      struct_yaml::to_yaml(*this, yaml_str);
      return yaml_str;
    } catch (std::exception &e) {
      ELOGE << "ConfigVar::toString exception " << e.what() << " convert: " << GetTypeName() << " to string"
            << " name = " << name_;
    }
    return {};
  }

  auto FromYaml(std::string_view yaml_str) -> void override {
    try {
      struct_yaml::from_yaml(*this, yaml_str);
    } catch (std::exception &e) {
      ELOGE << "ConfigVar::fromString exception " << e.what() << " convert: string to" << GetTypeName()
            << " name=" << name_ << " - " << yaml_str;
    }
  }

  auto SetValue(T value) -> void {
    if (value_ == value) {
      return;
    }

    for (auto &iter : cbs_) {
      iter.second(value_, value);
    }
    value_ = value;
  }

  auto GetValue() const noexcept -> T { return value_; }

  [[nodiscard]] auto GetTypeName() const noexcept -> std::string_view override {
    return pinecone::util::TypeToName<T>();
  }

  auto AddListener(on_change_cb change_cb) -> uint64_t {
    static uint64_t s_func_id = 0;
    ++s_func_id;
    cbs_[s_func_id] = change_cb;
    return s_func_id;
  }

  auto DelListener(uint64_t key) -> void { cbs_.erase(key); }

  auto GetListener(uint64_t key) const -> on_change_cb {
    auto iter = cbs_.find(key);
    return iter == cbs_.end() ? nullptr : iter->second;
  }

  auto ClearListener() -> void { cbs_.clear(); }

  YLT_REFL(ConfigVar, value_);

 private:
  T value_;
  std::unordered_map<uint64_t, on_change_cb> cbs_;
};

class Config {
 public:
  using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;

  template <class T>
  static auto Lookup(std::string_view name, const T &default_value, std::string_view description = "") ->
      typename ConfigVar<T>::ptr {
    auto iter = GetDatas().find(name.data());
    if (iter != GetDatas().end()) {
      if (auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(iter->second)) {
        ELOGI << "Lookup name=" << name << " exists";
        return tmp;
      }
      ELOGE << "Lookup name=" << name << " exists but type not " << pinecone::util::TypeToName<T>()
            << " real_type=" << iter->second->GetTypeName() << " " << iter->second->ToYaml();
      return nullptr;
    }

    if (name.find_first_not_of(kDefaultNameConvention) != std::string::npos) {
      ELOGE << "Lookup name invalid " << name;
      throw std::invalid_argument(name.data());
    }

    auto value = std::make_shared<ConfigVar<T> >(name, default_value, description);
    GetDatas()[name.data()] = value;
    return value;
  }

  template <class T>
  static auto Lookup(std::string_view name) -> typename ConfigVar<T>::ptr {
    const auto item = GetDatas().find(name.data());
    if (item == GetDatas().end()) {
      return nullptr;
    }

    return std::dynamic_pointer_cast<ConfigVar<T> >(item->second);
  }

  static auto LoadFromConfDir(std::string_view path = GetConfigDefaultRootPath()) -> void;

  static auto LookupBase(std::string_view name) -> ConfigVarBase::ptr;

  static auto GetDatas() -> ConfigVarMap & {
    static ConfigVarMap s_datas;
    return s_datas;
  }
};
}  // namespace pinecone::config

#endif  // CONFIG_H
