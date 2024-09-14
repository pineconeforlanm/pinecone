//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_THREADPOOL_H
#define PINECONE_THREADPOOL_H

#include <pinecone/noncopyable.h>
#include <pinecone/util.h>
#include <ylt/easylog.hpp>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace pinecone::base {
    class ThreadPool {
    public:
        static auto instance() -> ThreadPool & {
            static ThreadPool ins;
            return ins;
        }

        using Task = std::packaged_task<void()>;

        explicit ThreadPool(uint32_t num = std::thread::hardware_concurrency())
                : stop_(false), thread_num_(num < 1 ? 1 : num) {
            start();
        }

        ~ThreadPool() {
            stop();
        }

        template<class F, class... Args>
        auto commit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
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

        auto get_idle_thread_count() -> uint32_t {
            return thread_num_.load();
        }

    private:
        void start() {
            for (int i = 0; i < thread_num_; ++i) {
                pools_.emplace_back(pinecone::util::get_thread_id(), [i, this]() {
                    pools_[i].first = pinecone::util::get_thread_id();
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

        void stop() {
            stop_.store(true);
            cv_lock_.notify_all();
            for (auto &thread: pools_) {
                if (thread.second.joinable()) {
//                    ELOGW << "join thread id is [" << thread.first << "]";
                    thread.second.join();
                }
            }
//            ELOGW << "thread pool will exit";
        }

        std::mutex cv_mt_;
        std::condition_variable cv_lock_;
        std::atomic_bool stop_;
        std::atomic_uint32_t thread_num_;
        std::queue<Task> tasks_;
        std::vector<std::pair<pid_t, std::thread>> pools_;
    };

}  // namespace pinecone::base::thread

#endif //PINECONE_THREADPOOL_H
