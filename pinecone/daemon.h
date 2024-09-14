//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_DAEMON_H
#define PINECONE_DAEMON_H

#include <pinecone/singletion.h>

#include <ylt/struct_yaml/yaml_reader.h>
#include <ylt/struct_yaml/yaml_writer.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <unistd.h>


namespace pinecone {
    using namespace std::chrono;

    class ProcessInfo {
    public:
        using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

        ProcessInfo() = default;

        ~ProcessInfo() = default;

        auto set_parent_id(pid_t parent_id) -> void { parent_id_ = parent_id; }

        [[nodiscard]] auto get_parent_id() const -> pid_t { return parent_id_; }

        auto set_main_id(pid_t main_id) -> void { main_id_ = main_id; }

        [[nodiscard]] auto get_main_id() const -> pid_t { return main_id_; }

        auto set_parent_start_time(const TimePoint &start_time) -> void { parent_start_time_ = start_time; }

        [[nodiscard]] auto get_parent_start_time() const -> TimePoint { return parent_start_time_; }

        auto set_main_start_time(const TimePoint &start_time) -> void { main_start_time_ = start_time; }

        [[nodiscard]] auto get_main_start_time() const -> TimePoint { return main_start_time_; }

        [[nodiscard]] auto get_restart_count() const -> uint64_t { return restart_count_.load(); }

        auto reload_restart_count() -> void { restart_count_.fetch_add(1); }

        [[nodiscard]] auto to_string(std::string_view time_fmt = "%Y-%m-%d %H:%M:%S") const -> std::string;

    private:
        pid_t parent_id_;
        pid_t main_id_;
        TimePoint parent_start_time_;
        TimePoint main_start_time_;
        std::atomic_uint64_t restart_count_;
    };

    using ProcessInfoMgr = pinecone::util::Singleton<ProcessInfo>;

    auto StartDaemon(int argc, char **argv,
                     const std::function<int(int argc, char **argv)>& main_cb,
                     bool is_daemon) -> int;
}

#endif //PINECONE_DAEMON_H
