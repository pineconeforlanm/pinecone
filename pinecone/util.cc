//
// Created by 28192 on 2024/9/13.
//

#include <pinecone/util.h>
#include <ylt/easylog.hpp>
#include <array>
#include <unistd.h>

namespace pinecone::util {
    auto get_thread_id() -> pid_t {
        static thread_local auto const s_pid = static_cast<pid_t>(syscall(SYS_gettid));
        return s_pid;
    }

    auto get_host_name() -> std::string_view {
        static auto s_host_name = std::array<char, 1024>{0};
        static std::once_flag s_once_flag;
        std::call_once(s_once_flag, []() {
            if (const auto status = gethostname(s_host_name.data(), s_host_name.size()); status < 0) {
                ELOGE << "gethostname() failed";
            }
        });
        return s_host_name.data();
    }
}
