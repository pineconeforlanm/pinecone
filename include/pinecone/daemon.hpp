//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_DAEMON_H
#define PINECONE_DAEMON_H

#include <unistd.h>
#include <ylt/struct_yaml/yaml_reader.h>
#include <ylt/struct_yaml/yaml_writer.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <pinecone/singleton.hpp>
#include <string>

namespace pinecone {
class ProcessInfo {
 public:
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

  ProcessInfo() = default;

  ~ProcessInfo() = default;

  auto SetParentId(pid_t parent_id) -> void { parent_id_ = parent_id; }

  [[nodiscard]] auto GetParentId() const -> pid_t { return parent_id_; }

  auto SetMainId(pid_t main_id) -> void { main_id_ = main_id; }

  [[nodiscard]] auto GetMainId() const -> pid_t { return main_id_; }

  auto SetParentStartTime(const TimePoint &start_time) -> void { parent_start_time_ = start_time; }

  [[nodiscard]] auto GetParentStartTime() const -> TimePoint { return parent_start_time_; }

  auto SetMainStartTime(const TimePoint &start_time) -> void { main_start_time_ = start_time; }

  [[nodiscard]] auto GetMainStartTime() const -> TimePoint { return main_start_time_; }

  [[nodiscard]] auto GetRestartCount() const -> uint64_t { return restart_count_.load(); }

  auto ReloadRestartCount() -> void { restart_count_.fetch_add(1); }

  [[nodiscard]] auto ToString(std::string_view time_fmt = "%Y-%m-%d %H:%M:%S") const -> std::string;

 private:
  pid_t parent_id_;
  pid_t main_id_;
  TimePoint parent_start_time_;
  TimePoint main_start_time_;
  std::atomic_uint64_t restart_count_;
};

using ProcessInfoMgr = pinecone::util::Singleton<ProcessInfo>;

auto StartDaemon(int argc, char **argv, const std::function<int(int, char **)> &main_cb, bool is_daemon) -> int;
}  // namespace pinecone

#endif  // PINECONE_DAEMON_H
