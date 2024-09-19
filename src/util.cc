//
// Created by 28192 on 2024/9/13.
//

#include <unistd.h>

#include <array>
#include <pinecone/util.hpp>
#include <ylt/easylog.hpp>

namespace pinecone::util {
auto GetThreadId() -> pid_t {
  static thread_local auto const s_pid = static_cast<pid_t>(syscall(SYS_gettid));
  return s_pid;
}

auto GetHostName() -> std::string_view {
  static auto s_host_name = std::array<char, 1024>{0};
  static std::once_flag s_once_flag;
  std::call_once(s_once_flag, []() {
    if (const auto status = gethostname(s_host_name.data(), s_host_name.size());
      status < 0) {
      ELOGE << "gethostname() failed";
    }
  });
  return s_host_name.data();
}
} // namespace pinecone::util
