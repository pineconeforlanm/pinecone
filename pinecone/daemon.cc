//
// Created by 28192 on 2024/9/14.
//
#include <pinecone/config.h>
#include <pinecone/daemon.h>

#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace {
    auto g_daemon_restart_interval =
            pinecone::config::Config::lookup("daemon.restart_interval", static_cast<uint32_t>(5),
                                             "daemon restart interval");
}

namespace pinecone {

    auto ProcessInfo::to_string([[maybe_unused]] std::string_view time_fmt) const -> std::string {
        std::stringstream ss;
        auto parent_start_time = std::chrono::system_clock::to_time_t(parent_start_time_);
        auto main_start_time = std::chrono::system_clock::to_time_t(main_start_time_);
        ss << "ProcessInfo str : {" << "\n\t"
           << "parent id : " << parent_id_ << "\n\t"
           << "main id : " << main_id_ << "\n\t"
           << "parent start time : " << std::put_time(std::localtime(&parent_start_time), time_fmt.data()) << "\n\t"
           << "main start time : " << std::put_time(std::localtime(&main_start_time), time_fmt.data()) << "\n\t"
           << "restart_count : " << restart_count_ << "\n}\n";
        return ss.str();
    }

    namespace {
        auto real_start(int argc, char **argv,
                        std::function<int(int argc, char **argv)> main_cb) -> int {
            ProcessInfoMgr::get_instance()->set_main_id(getpid());
            ProcessInfoMgr::get_instance()->set_main_start_time(
                    std::chrono::system_clock::now());
            return main_cb(argc, argv);
        }

        auto real_daemon(int argc, char **argv, std::function<int(int argc, char **argv)> main_cb) -> int {
            ProcessInfoMgr::get_instance()->set_parent_id(getpid());
            ProcessInfoMgr::get_instance()->set_parent_start_time(std::chrono::system_clock::now());
            while (true) {
                pid_t pid = fork();
                if (pid == 0) {
                    ProcessInfoMgr::get_instance()->set_main_id(getpid());
                    ProcessInfoMgr::get_instance()->set_main_start_time(std::chrono::system_clock::now());
                    ELOGI << "process start pid = " << getpid();
                    return real_start(argc, argv, main_cb);
                } else if (pid < 0) {
                    ELOGE << "fork fail return = " << pid << " errno = " << errno << ", error str =" << strerror(errno);
                    return -1;
                } else {
                    int status = 0;
                    waitpid(pid, &status, 0);
                    if (status) {
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
                    ProcessInfoMgr::get_instance()->reload_restart_count();
                    sleep(g_daemon_restart_interval->get_value());
                }
            }
            return 0;
        }
    }

    auto StartDaemon(int argc, char **argv,
                      const std::function<int(int argc, char **argv)> &main_cb,
                      bool is_daemon) -> int {
        if (!is_daemon) {
            ProcessInfoMgr::get_instance()->set_parent_id(getpid());
            ProcessInfoMgr::get_instance()->set_parent_start_time(std::chrono::system_clock::now());
            return real_start(argc, argv, main_cb);
        }
        return real_daemon(argc, argv, main_cb);
    }

}  // namespace pinecone