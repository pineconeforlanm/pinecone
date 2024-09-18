//
// Created by 28192 on 2024/9/13.
//

#ifndef UTIL_H
#define UTIL_H

#include <cxxabi.h>
#include <pthread.h>

#include <string>

namespace pinecone::util {
auto GetThreadId() -> pid_t;

auto GetHostName() -> std::string_view;

template <class T>
auto TypeToName() -> std::string_view {
  static const auto s_name = std::string_view{
      abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr)};
  return s_name;
}
}  // namespace pinecone::util

#endif  // UTIL_H
