//
// Created by 28192 on 2024/9/13.
//
#include <ylt/struct_yaml/yaml_reader.h>
#include <ylt/struct_yaml/yaml_writer.h>

#include <pinecone/config.hpp>
#include <sstream>
#include <ylt/easylog.hpp>

struct ConfigPerson {
  std::string name;
  int age;
  auto operator==(const ConfigPerson& rhs) const -> bool { return name == rhs.name && age == rhs.age; }
};
YLT_REFL(ConfigPerson, name, age);

class Person {
 public:
  std::string name;
  int age = 0;
  bool sex = false;
  Person() = default;
  auto toString() const -> std::string {
    std::stringstream ss;
    ss << "[Person name=" << name << " age=" << age << " sex=" << sex << "]";
    return ss.str();
  }

  bool operator==(const Person& oth) const { return name == oth.name && age == oth.age && sex == oth.sex; }
};
YLT_REFL(Person, name, age, sex);

class Pinecone {
 public:
  Pinecone() = default;

  ~Pinecone() = default;

  Pinecone(const std::string_view& name, const int age) : name_(name), age_(age){}

  friend auto operator<<(std::ostream& os, const Pinecone& pinecone) -> std::ostream& {
    os << pinecone.name_ << " " << pinecone.age_;
    return os;
  }
  YLT_REFL(Pinecone, name_, age_);

  auto operator==(const Pinecone& rhs) const -> bool { return name_ == rhs.name_ && age_ == rhs.age_; }

 private:
  std::string name_;
  int age_{};
};

PINECONE_CONFIG_LEXICAL_CAST_YAML(ConfigPerson);
PINECONE_CONFIG_LEXICAL_CAST_YAML(Pinecone);
PINECONE_CONFIG_LEXICAL_CAST_YAML(Person);

namespace {
auto g_person_val_config = pinecone::Config::Lookup(
    "root.ref.family.person", ConfigPerson{.name = "bob", .age = 36}, "system ref family person");

auto g_pinecone_val_config =
    pinecone::Config::Lookup("root.ref.family.pinecone", Pinecone{"body", 366}, "system ref family pinecone");

auto g_no_int_config_var = pinecone::Config::Lookup("system.no_int", 8080, "system no_int");

auto g_no_double_config_var = pinecone::Config::Lookup("double", 6.3699, "double");

auto g_person = pinecone::Config::Lookup("class.person", Person(), "system person");

auto Test02() -> void {
  ELOGI << g_no_int_config_var->GetValue();
  ELOGI << g_no_double_config_var->GetValue();
  ELOGI << g_pinecone_val_config->GetValue();
  ELOGI << g_person_val_config->GetValue().name << ":" << g_person_val_config->GetValue().age;
  ELOGI << g_person->GetValue().toString();
  const auto root = YAML::LoadFile("./config/test_config.yaml");
  pinecone::Config::LoadFromYaml(root);
  ELOGI << g_no_int_config_var->GetValue();
  ELOGI << g_no_double_config_var->GetValue();
  ELOGI << g_pinecone_val_config->GetValue();
  ELOGI << g_person_val_config->GetValue().name << ":" << g_person_val_config->GetValue().age;
  ELOGI << g_person->GetValue().toString();
}

}  // namespace

auto main() -> int {
  easylog::set_async(false);
  easylog::set_console(true);
  Test02();
  return 0;
}
