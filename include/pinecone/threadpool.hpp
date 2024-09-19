//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_THREADPOOL_H
#define PINECONE_THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <mutex>
#include <pinecone/singleton.hpp>
#include <pinecone/util.hpp>
#include <queue>
#include <thread>
#include <vector>
#include <ylt/easylog.hpp>

namespace pinecone::base {
class ThreadPool : public pinecone::util::BaseSingleton<ThreadPool> {
 public:
  using Task = std::packaged_task<void()>;

  explicit ThreadPool(const uint32_t capacity = std::thread::hardware_concurrency())
      : stop_(false), thread_num_(capacity < 1 ? 1 : capacity) {
    Start();
  }

  ~ThreadPool() { Stop(); }

  template <class F, class... Args>
  auto Commit(F &&func, Args &&...args) -> std::future<decltype(func(args...))> {
    using RetType = decltype(func(args...));
    if (stop_.load()) {
      return std::future<RetType>{};
    }
    auto task =
        std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));

    std::future<RetType> ret = task->get_future();
    {
      std::lock_guard<std::mutex> cv_mt(mutex_);
      tasks_.emplace([task] { (*task)(); });
    }
    cv_.notify_one();
    return ret;
  }

  [[nodiscard]] auto GetIdleThreadCount() const -> uint32_t { return thread_num_.load(); }

 private:
  void Start() {
    for (int i = 0; i < thread_num_; ++i) {
      pools_.emplace_back(pinecone::util::GetThreadId(), [i, this]() {
        pools_[i].first = pinecone::util::GetThreadId();
        while (!stop_.load()) {
          Task task;
          {
            std::unique_lock<std::mutex> cv_mt(mutex_);
            cv_.wait(cv_mt, [this] { return stop_.load() || !tasks_.empty(); });
            if (tasks_.empty()) {
              return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
          }
          --thread_num_;
          task();
          ++thread_num_;
        }
      });
    }
  }

  void Stop() {
    stop_.store(true);
    cv_.notify_all();
    for (auto &[pid, thread] : pools_) {
      if (thread.joinable()) {
        //        ELOGW << "join thread id is [" << thread.first << "]";
        thread.join();
      }
    }
    //    ELOGW << "thread pool will exit";
  }

  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic_bool stop_;
  std::atomic_uint32_t thread_num_;
  std::queue<Task> tasks_;
  std::vector<std::pair<pid_t, std::thread>> pools_;
};
}  // namespace pinecone::base

#endif  // PINECONE_THREADPOOL_H
