//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_SINGLETON_H
#define PINECONE_SINGLETON_H

#include <memory>
#include <mutex>
#include <pinecone/noncopyable.hpp>

namespace pinecone {
template <class T>
class Singleton {
 public:
  static auto GetInstance() -> T* {
    static T s_ptr;
    return &s_ptr;
  }
};

template <class T>
class SingletonPtr {
 public:
  static auto GetInstance() -> std::shared_ptr<T> {
    static auto s_ptr(std::make_shared<T>());
    return s_ptr;
  }
};

template <typename T>
class BaseSingleton : public Noncopyable {
 public:
  BaseSingleton() = default;

  static auto GetInstance() -> std::shared_ptr<T> {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() { instance = std::make_shared<T>(); });
    return instance;
  }

  ~BaseSingleton() = default;

 protected:
  static std::shared_ptr<T> instance;
};

template <typename T>
std::shared_ptr<T> BaseSingleton<T>::instance = nullptr;
}  // namespace pinecone::util

#endif  // PINECONE_SINGLETON_H
