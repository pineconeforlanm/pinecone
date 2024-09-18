//
// Created by 28192 on 2024/9/14.
//
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iomanip>
#include <pinecone/config.hpp>
#include <pinecone/daemon.hpp>
#include <sstream>

namespace {
auto g_daemon_restart_interval = pinecone::config::Config::Lookup(
    "daemon.restart_interval", static_cast<uint32_t>(5),
    "daemon restart interval");
}

namespace pinecone {

auto ProcessInfo::ToString(std::string_view time_fmt) const -> std::string {
  std::stringstream ss;
  auto parent_start_time =
      std::chrono::system_clock::to_time_t(parent_start_time_);
  auto main_start_time = std::chrono::system_clock::to_time_t(main_start_time_);
  ss << "ProcessInfo str : {" << "\n\t" << "parent id : " << parent_id_
     << "\n\t" << "main id : " << main_id_ << "\n\t" << "parent start time : "
     << std::put_time(std::localtime(&parent_start_time), time_fmt.data())
     << "\n\t" << "main start time : "
     << std::put_time(std::localtime(&main_start_time), time_fmt.data())
     << "\n\t" << "restart_count : " << restart_count_ << "\n}\n";
  return ss.str();
}

namespace {
auto RealStart(int argc, char **argv,
               const std::function<int(int argc, char **argv)> &main_cb)
    -> int {
  ProcessInfoMgr::GetInstance()->SetMainId(getpid());
  ProcessInfoMgr::GetInstance()->SetMainStartTime(
      std::chrono::system_clock::now());
  return main_cb(argc, argv);
}

auto RealDaemon(int argc, char **argv,
                const std::function<int(int argc, char **argv)> &main_cb)
    -> int {
  ProcessInfoMgr::GetInstance()->SetParentId(getpid());
  ProcessInfoMgr::GetInstance()->SetParentStartTime(
      std::chrono::system_clock::now());
  while (true) {
    pid_t pid = fork();
    if (pid == 0) {
      ProcessInfoMgr::GetInstance()->SetMainId(getpid());
      ProcessInfoMgr::GetInstance()->SetMainStartTime(
          std::chrono::system_clock::now());
      ELOGI << "process start pid = " << getpid();
      return RealStart(argc, argv, main_cb);
    } else if (pid < 0) {
      ELOGE << "fork fail return = " << pid << " errno = " << errno
            << ", error str =" << strerror(errno);
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
      ProcessInfoMgr::GetInstance()->ReloadRestartCount();
      sleep(g_daemon_restart_interval->GetValue());
    }
  }
  return 0;
}
}  // namespace

auto StartDaemon(int argc, char **argv,
                 const std::function<int(int argc, char **argv)> &main_cb,
                 bool is_daemon) -> int {
  if (!is_daemon) {
    ProcessInfoMgr::GetInstance()->SetParentId(getpid());
    ProcessInfoMgr::GetInstance()->SetParentStartTime(
        std::chrono::system_clock::now());
    return RealStart(argc, argv, main_cb);
  }
  return RealDaemon(argc, argv, main_cb);
}

}  // namespace pinecone