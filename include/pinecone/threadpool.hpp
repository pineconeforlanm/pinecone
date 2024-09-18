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
#include <iostream>
#include <mutex>
#include <pinecone/noncopyable.hpp>
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

  explicit ThreadPool(uint32_t num = std::thread::hardware_concurrency())
      : stop_(false), thread_num_(num < 1 ? 1 : num) {
    Start();
  }

  ~ThreadPool() { Stop(); }

  template <class F, class... Args>
  auto Commit(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
    using RetType = decltype(f(args...));
    if (stop_.load()) {
      return std::future<RetType>{};
    }
    auto task = std::make_shared<std::packaged_task<RetType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<RetType> ret = task->get_future();
    {
      std::lock_guard<std::mutex> cv_mt(cv_mt_);
      tasks_.emplace([task] { (*task)(); });
    }
    cv_lock_.notify_one();
    return ret;
  }

  auto GetIdleThreadCount() -> uint32_t { return thread_num_.load(); }

 private:
  void Start() {
    for (int i = 0; i < thread_num_; ++i) {
      pools_.emplace_back(pinecone::util::GetThreadId(), [i, this]() {
        pools_[i].first = pinecone::util::GetThreadId();
        while (!this->stop_.load()) {
          Task task;
          {
            std::unique_lock<std::mutex> cv_mt(cv_mt_);
            this->cv_lock_.wait(cv_mt, [this] {
              return this->stop_.load() || !this->tasks_.empty();
            });
            if (this->tasks_.empty()) {
              return;
            }

            task = std::move(this->tasks_.front());
            this->tasks_.pop();
          }
          this->thread_num_--;
          task();
          this->thread_num_++;
        }
      });
    }
  }

  void Stop() {
    stop_.store(true);
    cv_lock_.notify_all();
    for (auto &thread : pools_) {
      if (thread.second.joinable()) {
        //        ELOGW << "join thread id is [" << thread.first << "]";
        thread.second.join();
      }
    }
    //    ELOGW << "thread pool will exit";
  }

  std::mutex cv_mt_;
  std::condition_variable cv_lock_;
  std::atomic_bool stop_;
  std::atomic_uint32_t thread_num_;
  std::queue<Task> tasks_;
  std::vector<std::pair<pid_t, std::thread>> pools_;
};

}  // namespace pinecone::base

#endif  // PINECONE_THREADPOOL_H
