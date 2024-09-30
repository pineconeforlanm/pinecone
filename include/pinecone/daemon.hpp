//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_DAEMON_H
#define PINECONE_DAEMON_H

#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <pinecone/config.hpp>
#include <pinecone/singleton.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <ylt/easylog.hpp>

namespace pinecone {

inline auto g_daemon_restart_interval =
    Config::Lookup("daemon.restart_interval", static_cast<uint32_t>(5), "daemon restart interval");

class ProcessInfo : public BaseSingleton<ProcessInfo> {
 public:
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

  ProcessInfo() = default;

  ~ProcessInfo() = default;

  auto SetParentId(const pid_t parent_id) -> void { parent_id_ = parent_id; }

  [[nodiscard]] auto GetParentId() const -> pid_t { return parent_id_; }

  auto SetMainId(const pid_t main_id) -> void { main_id_ = main_id; }

  [[nodiscard]] auto GetMainId() const -> pid_t { return main_id_; }

  auto SetParentStartTime(const TimePoint& start_time) -> void { parent_start_time_ = start_time; }

  [[nodiscard]] auto GetParentStartTime() const -> TimePoint { return parent_start_time_; }

  auto SetMainStartTime(const TimePoint& start_time) -> void { main_start_time_ = start_time; }

  [[nodiscard]] auto GetMainStartTime() const -> TimePoint { return main_start_time_; }

  [[nodiscard]] auto GetRestartCount() const -> uint64_t { return restart_count_.load(); }

  auto ReloadRestartCount() -> void { restart_count_.fetch_add(1); }

  [[nodiscard]] auto ToString(const std::string_view& time_fmt = "%Y-%m-%d %H:%M:%S") const -> std::string {
    std::stringstream sstr;
    const auto pst = std::chrono::system_clock::to_time_t(parent_start_time_);
    const auto mst = std::chrono::system_clock::to_time_t(main_start_time_);
    sstr << "ProcessInfo str : {"
         << "\n\t"
         << "parent id : " << parent_id_ << "\n\t"
         << "main id : " << main_id_ << "\n\t"
         << "parent start time : " << std::put_time(std::localtime(&pst), time_fmt.data()) << "\n\t"
         << "main start time : " << std::put_time(std::localtime(&mst), time_fmt.data()) << "\n\t"
         << "restart_count : " << restart_count_ << "\n}\n";
    return sstr.str();
  }

 private:
  pid_t parent_id_;
  pid_t main_id_;
  TimePoint parent_start_time_;
  TimePoint main_start_time_;
  std::atomic_uint64_t restart_count_;
};

inline auto RealStart(int argc, char** argv, const std::function<int(int, char**)>& main_cb) -> int {
  const auto process_mgr = ProcessInfo::GetInstance();
  process_mgr->SetMainId(getpid());
  process_mgr->SetMainStartTime(std::chrono::system_clock::now());
  return main_cb(argc, argv);
}

inline auto RealDaemon(int argc, char** argv, const std::function<int(int, char**)>& main_cb) -> int {
  const auto process_mgr = ProcessInfo::GetInstance();
  process_mgr->SetParentId(getpid());
  process_mgr->SetParentStartTime(std::chrono::system_clock::now());
  while (true) {
    if (auto pid = fork(); pid == 0) {
      process_mgr->SetMainId(getpid());
      process_mgr->SetMainStartTime(std::chrono::system_clock::now());
      ELOGI << "process start pid = " << getpid();
      return RealStart(argc, argv, main_cb);
    } else if (pid < 0) {
      ELOGE << "fork fail return = " << pid << " errno = " << errno << ", error str =" << strerror(errno);
      return -1;
    } else {
      int status = 0;
      waitpid(pid, &status, 0);
      if (static_cast<bool>(status)) {
        if (status == 9) {
          ELOGI << "killed";
          break;
        } else {
          ELOGE << "child crash pid=" << pid << " status=" << status;
        }
      } else {
        ELOGI << "child finished pid=" << pid;
        break;
      }
      process_mgr->ReloadRestartCount();
      sleep(g_daemon_restart_interval->GetValue());
    }
  }
  return 0;
}

inline auto StartDaemon(int argc, char** argv, const std::function<int(int, char**)>& main_cb, const bool is_daemon)
    -> int {
  if (!is_daemon) {
    const auto process_mgr = ProcessInfo::GetInstance();
    process_mgr->SetParentId(getpid());
    process_mgr->SetParentStartTime(std::chrono::system_clock::now());
    return RealStart(argc, argv, main_cb);
  }
  return RealDaemon(argc, argv, main_cb);
}
}  // namespace pinecone

#endif  // PINECONE_DAEMON_H
