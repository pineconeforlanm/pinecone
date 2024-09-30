//
// Created by 28192 on 2024/9/13.
//

#ifndef UTIL_H
#define UTIL_H

#include <cxxabi.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <array>
#include <mutex>
#include <string_view>
#include <ylt/easylog.hpp>

namespace pinecone {
inline auto GetThreadId() -> pid_t {
  static thread_local auto const s_pid = static_cast<pid_t>(::syscall(SYS_gettid));
  return s_pid;
}

inline auto GetHostName() -> std::string_view {
  static auto s_host_name = std::array<char, 1024>{0};
  static std::once_flag s_once_flag;
  std::call_once(s_once_flag, []() {
    if (const auto status = gethostname(s_host_name.data(), s_host_name.size()); status < 0) {
      ELOGE << "gethostname() failed";
    }
  });
  return s_host_name.data();
}

template <class T>
auto TypeToName() -> std::string_view {
  static const auto s_name = std::string_view{abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr)};
  return s_name;
}
}  // namespace pinecone::util

#endif  // UTIL_H
