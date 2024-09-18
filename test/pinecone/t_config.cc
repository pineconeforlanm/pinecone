//
// Created by 28192 on 2024/9/13.
//
#include <pinecone/config.hpp>

struct ConfigPerson {
  std::string name;
  int age;
};

class Pinecone {
 public:
  Pinecone() = default;

  ~Pinecone() = default;

  Pinecone(std::string_view name, int age) : name_(name), age_(age){};

  friend auto operator<<(std::ostream& os,
                         const Pinecone& pinecone) -> std::ostream& {
    os << pinecone.name_ << " " << pinecone.age_;
    return os;
  }
  YLT_REFL(Pinecone, name_, age_);

 private:
  std::string name_;
  int age_{};
};

auto g_person_val_config = pinecone::config::Config::lookup(
    "root.ref.family.person", ConfigPerson{"bob", 36},
    "system ref family person");

auto g_pinecone_val_config = pinecone::config::Config::lookup(
    "root.ref.family.pinecone", Pinecone{"body", 366},
    "system ref family pinecone");

auto g_no_int_config_var =
    pinecone::config::Config::lookup("system.no_int", 8080, "system no_int");

auto g_no_double_config_var =
    pinecone::config::Config::lookup("double", 6.3699, "double");

auto test_02() -> void {
  ELOGI << g_no_int_config_var->get_value();
  ELOGI << g_no_double_config_var->get_value();
  ELOGI << g_pinecone_val_config->get_value();
  ELOGI << g_person_val_config->get_value().name << ":"
        << g_person_val_config->get_value().age;
  pinecone::config::Config::load_from_conf_dir();
  ELOGI << g_no_int_config_var->get_value();
  ELOGI << g_no_double_config_var->get_value();
  ELOGI << g_pinecone_val_config->get_value();
  ELOGI << g_person_val_config->get_value().name << ":"
        << g_person_val_config->get_value().age;
}

auto main() -> int {
  easylog::set_async(false);
  easylog::set_console(true);
  test_02();
  return 0;
}
