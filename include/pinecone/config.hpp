//
// Created by 28192 on 2024/9/13.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <yaml-cpp/yaml.h>
#include <ylt/struct_json/json_reader.h>
#include <ylt/struct_json/json_writer.h>

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <cctype>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <pinecone/noncopyable.hpp>
#include <pinecone/util.hpp>
#include <ranges>
#include <set>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <ylt/easylog.hpp>

#define PINECONE_CONFIG_LEXICAL_CAST_YAML(Type)              \
  template <>                                                \
  class pinecone::LexicalCast<std::string, Type> {   \
   public:                                                   \
    auto operator()(const std::string& str) const -> Type {  \
      Type type;                                             \
      struct_yaml::from_yaml(type, str);                     \
      return type;                                           \
    };                                                       \
  };                                                         \
                                                             \
  template <>                                                \
  class pinecone::LexicalCast<Type, std::string> {   \
   public:                                                   \
    auto operator()(const Type& type) const -> std::string { \
      std::string str;                                       \
      struct_yaml::to_yaml(type, str);                       \
      return str;                                            \
    };                                                       \
  };

namespace pinecone {
constexpr auto kDefaultNameConvention{std::string_view{"abcdefghijklmnopqrstuvwxyz._0123456789"}};
constexpr auto kDefaultConfigRootPath{std::string_view{"./config/"}};
constexpr auto kDefaultConfigFileSuffix{std::string_view{".yaml"}};
constexpr auto kDefaultConfigFilePath{std::string_view{"./config/pinecone.yaml"}};

[[maybe_unused]] inline auto GetConfigDefaultRootPath() -> std::string_view { return kDefaultConfigRootPath; }

class ConfigVarBase {
 public:
  using ptr = std::shared_ptr<ConfigVarBase>;

  explicit ConfigVarBase(const std::string_view& name, const std::string_view& description) noexcept
      : name_{name}, description_{description} {
    std::ranges::transform(name_, name_.begin(), ::tolower);
  };

  virtual ~ConfigVarBase() = default;

  [[nodiscard]] auto GetName() const noexcept -> std::string_view { return name_; }

  [[nodiscard]] auto GetDescription() const noexcept -> std::string_view { return description_; }

  [[nodiscard]] virtual auto ToString() const -> std::string = 0;

  virtual auto FromString(const std::string_view& yaml_str) -> void = 0;

  [[nodiscard]] virtual auto GetTypeName() const noexcept -> std::string_view = 0;

 protected:
  std::string name_;
  std::string description_;
};

template <class F, class T>
class LexicalCast {
 public:
  auto operator()(const F& value) -> T { return boost::lexical_cast<T>(value); }
};

template <class T>
class LexicalCast<std::string, std::vector<T>> {
 public:
  auto operator()(const std::string& value) -> std::vector<T> {
    const auto node = YAML::Load(value);
    std::vector<T> vec;
    std::stringstream ss;
    for (auto&& iter : node) {
      ss.str("");
      ss << iter;
      vec.push_back(LexicalCast<std::string, T>()(ss.str()));
    }
    return vec;
  }
};

template <class T>
class LexicalCast<std::vector<T>, std::string> {
 public:
  auto operator()(const std::vector<T>& value) -> std::string {
    const YAML::Node node(YAML::NodeType::Sequence);
    for (auto& iter : value) {
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(iter)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

template <class T>
class LexicalCast<std::string, std::list<T>> {
 public:
  auto operator()(const std::string& value) -> std::list<T> {
    const auto node = YAML::Load(value);
    std::list<T> vec;
    std::stringstream ss;
    for (auto&& iter : node) {
      ss.str("");
      ss << iter;
      vec.push_back(LexicalCast<std::string, T>()(ss.str()));
    }
    return vec;
  }
};

template <class T>
class LexicalCast<std::list<T>, std::string> {
 public:
  auto operator()(const std::list<T>& value) -> std::string {
    const YAML::Node node(YAML::NodeType::Sequence);
    for (auto& iter : value) {
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(iter)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

template <class T>
class LexicalCast<std::string, std::set<T>> {
 public:
  auto operator()(const std::string& value) -> std::set<T> {
    const auto node = YAML::Load(value);
    std::set<T> vec;
    std::stringstream ss;
    for (auto&& iter : node) {
      ss.str("");
      ss << iter;
      vec.insert(LexicalCast<std::string, T>()(ss.str()));
    }
    return vec;
  }
};

template <class T>
class LexicalCast<std::set<T>, std::string> {
 public:
  auto operator()(const std::set<T>& value) -> std::string {
    const YAML::Node node(YAML::NodeType::Sequence);
    for (auto& iter : value) {
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(iter)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

template <class T>
class LexicalCast<std::string, std::unordered_set<T>> {
 public:
  auto operator()(const std::string& str) -> std::unordered_set<T> {
    const auto node = YAML::Load(str);
    std::unordered_set<T> vec;
    std::stringstream ss;
    for (auto&& iter : node) {
      ss.str("");
      ss << iter;
      vec.insert(LexicalCast<std::string, T>()(ss.str()));
    }
    return vec;
  }
};

template <class T>
class LexicalCast<std::unordered_set<T>, std::string> {
 public:
  auto operator()(const std::unordered_set<T>& value) -> std::string {
    const YAML::Node node(YAML::NodeType::Sequence);
    for (auto& iter : value) {
      node.push_back(YAML::Load(LexicalCast<T, std::string>()(iter)));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

template <class T>
class LexicalCast<std::string, std::map<std::string, T>> {
 public:
  auto operator()(const std::string& str) -> std::map<std::string, T> {
    const auto node = YAML::Load(str);
    std::map<std::string, T> vec;
    std::stringstream ss;
    for (auto it = node.begin(); it != node.end(); ++it) {
      ss.str("");
      ss << it->second;
      vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
    }
    return vec;
  }
};

template <class T>
class LexicalCast<std::map<std::string, T>, std::string> {
 public:
  auto operator()(const std::map<std::string, T>& value) -> std::string {
    const YAML::Node node(YAML::NodeType::Map);
    for (auto& iter : value) {
      node[iter.first] = YAML::Load(LexicalCast<T, std::string>()(iter.second));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

template <class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>> {
 public:
  auto operator()(const std::string& str) -> std::unordered_map<std::string, T> {
    const auto node = YAML::Load(str);
    std::unordered_map<std::string, T> vec;
    std::stringstream ss;
    for (auto it = node.begin(); it != node.end(); ++it) {
      ss.str("");
      ss << it->second;
      vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
    }
    return vec;
  }
};

template <class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
 public:
  auto operator()(const std::unordered_map<std::string, T>& value) -> std::string {
    const YAML::Node node(YAML::NodeType::Map);
    for (auto& iter : value) {
      node[iter.first] = YAML::Load(LexicalCast<T, std::string>()(iter.second));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
  }
};

template <typename T, typename FromStr = LexicalCast<std::string, T>, typename ToStr = LexicalCast<T, std::string>>
class ConfigVar final : public ConfigVarBase {
 public:
  using ptr = std::shared_ptr<ConfigVar>;
  using on_change_cb = std::function<void(const T& old_value, const T& new_value)>;

  ConfigVar() noexcept : ConfigVarBase(TypeToName<T>(), "") {}

  ConfigVar(const std::string_view& name, const T& default_value, const std::string_view& description) noexcept
      : ConfigVarBase(name, description), value_(default_value) {}

  [[nodiscard]] auto ToString() const noexcept -> std::string override {
    try {
      std::shared_lock lock(shared_mutex_);
      return ToStr()(value_);
    } catch (std::exception& e) {
      ELOGE << "ConfigVar::toString exception " << e.what() << " convert: " << GetTypeName() << " to string"
            << " name = " << name_;
    }
    return {};
  }

  auto FromString(const std::string_view& yaml_str) -> void override {
    try {
      SetValue(FromStr()(yaml_str.data()));
    } catch (std::exception& e) {
      ELOGE << "ConfigVar::fromString exception " << e.what() << " convert: string to " << GetTypeName()
            << " name=" << name_ << " - " << yaml_str;
    }
  }

  auto SetValue(T value) -> void {
    {
      std::shared_lock lock(shared_mutex_);
      if (value_ == value) {
        return;
      }

      for (auto& iter : cbs_) {
        iter.second(value_, value);
      }
    }
    std::unique_lock lock(shared_mutex_);
    value_ = value;
  }

  auto GetValue() const noexcept -> T {
    std::shared_lock lock(shared_mutex_);
    return value_;
  }

  [[nodiscard]] auto GetTypeName() const noexcept -> std::string_view override { return TypeToName<T>(); }

  auto AddListener(on_change_cb change_cb) -> uint64_t {
    static uint64_t s_func_id = 0;
    std::unique_lock lock(shared_mutex_);
    ++s_func_id;
    cbs_[s_func_id] = change_cb;
    return s_func_id;
  }

  auto DelListener(uint64_t key) -> void {
    std::unique_lock lock(shared_mutex_);
    cbs_.erase(key);
  }

  auto GetListener(uint64_t key) const -> on_change_cb {
    std::shared_lock lock(shared_mutex_);
    auto iter = cbs_.find(key);
    return iter == cbs_.end() ? nullptr : iter->second;
  }

  auto ClearListener() -> void {
    std::unique_lock lock(shared_mutex_);
    cbs_.clear();
  }

 private:
  mutable std::shared_mutex shared_mutex_;
  T value_;
  std::unordered_map<uint64_t, on_change_cb> cbs_;
};

class Config : public Noncopyable {
 public:
  using ConfigVarMap = std::unordered_map<std::string, ConfigVarBase::ptr>;

  template <class T>
  static auto Lookup(const std::string_view& name, const T& default_value, const std::string_view& description) ->
      typename ConfigVar<T>::ptr {
    std::unique_lock const lock(GetMutex());
    if (const auto iter = GetDatas().find(name.data()); iter != GetDatas().end()) {
      auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(iter->second);
      if (tmp == nullptr) {
        ELOGE << "Lookup name=" << name << " exists but type not " << TypeToName<T>()
              << " real_type=" << iter->second->GetTypeName() << " " << iter->second->ToString();
        return nullptr;
      }
      ELOGI << "Lookup name=" << name << " exists";
      return tmp;
    }

    if (name.find_first_not_of(kDefaultNameConvention) != std::string::npos) {
      ELOGE << "Lookup name invalid " << name;
      throw std::invalid_argument("name");
    }
    auto value = std::make_shared<ConfigVar<T>>(name, default_value, description);
    GetDatas()[name.data()] = value;
    return value;
  }

  template <class T>
  static auto Lookup(const std::string_view& name) -> typename ConfigVar<T>::ptr {
    std::shared_lock const lock(GetMutex());
    const auto item = GetDatas().find(name.data());
    if (item == GetDatas().end()) {
      return nullptr;
    }

    return std::dynamic_pointer_cast<ConfigVar<T>>(item->second);
  }

  static auto LoadFromYaml(const YAML::Node& root) -> void {
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);

    for (auto& [key, node] : all_nodes) {
      if (key.empty()) {
        continue;
      }

      std::ranges::transform(key, key.begin(), ::tolower);

      if (const auto var = LookupBase(key)) {
        if (node.IsScalar()) {
          var->FromString(node.Scalar());
        } else {
          std::stringstream val_str;
          val_str << node;
          var->FromString(val_str.str());
        }
      }
    }
  }

  static auto LoadFromConfFile(const std::string_view& filename = kDefaultConfigFilePath) -> void {
    if (!std::filesystem::exists(filename)) {
      ELOGE << "Load config file " << filename << " does not exist";
      return;
    }

    try {
      const auto root = YAML::LoadFile(filename.data());
      LoadFromYaml(root);
      ELOGI << "Loaded config file " << filename << " OK";
    } catch (const std::exception& e) {
      ELOGE << "Failed to load config file " << filename << ", error message : " << e.what();
    }
  }

  static auto LoadFromConfDir(const std::string_view& path = kDefaultConfigRootPath) -> void {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
      if (entry.path().extension().string() != ".yaml" && entry.path().extension().string() != ".yml") {
        continue;
      }

      try {
        const auto root = YAML::LoadFile(entry.path());
        LoadFromYaml(root);
        ELOGI << "Loaded config file " << entry.path() << " OK";
      } catch (const std::exception& e) {
        ELOGE << "Failed to load config file " << entry.path() << ", error message : " << e.what();
      }
    }
  }

  static auto LookupBase(const std::string_view& name) -> ConfigVarBase::ptr {
    std::shared_lock const lock(GetMutex());
    const auto iter = GetDatas().find(name.data());
    return iter == GetDatas().end() ? nullptr : iter->second;
  }

  static auto Visit(const std::function<void(ConfigVarBase::ptr)>& callback) -> void {
    for (const auto& value : GetDatas() | std::ranges::views::values) {
      callback(value);
    }
  }

 private:
  static auto GetDatas() -> ConfigVarMap& {
    static ConfigVarMap s_datas;
    return s_datas;
  }
  static auto GetMutex() -> std::shared_mutex& {
    static std::shared_mutex s_mutex;
    return s_mutex;
  }
  static void ListAllMember(const std::string_view& prefix, const YAML::Node& node,
                            std::list<std::pair<std::string, const YAML::Node>>& output) {
    if (prefix.find_first_not_of(kDefaultNameConvention) != std::string::npos) {
      ELOGE << "Config invalid name: " << prefix << " : " << node;
      return;
    }
    output.emplace_back(prefix.data(), node);
    if (node.IsMap()) {
      for (auto it = node.begin(); it != node.end(); ++it) {
        ListAllMember(prefix.empty() ? it->first.Scalar() : std::string(prefix) + "." + it->first.Scalar(), it->second,
                      output);
      }
    }
  }
};
}  // namespace pinecone::config

#endif  // CONFIG_H
