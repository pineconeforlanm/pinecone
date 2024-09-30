//
// Created by pinec on 2024/9/24.
//
#include <pinecone/threadpool.hpp>
#include <ylt/easylog.hpp>
#include <thread>
#include <chrono>

auto main() -> int {
  auto pool = pinecone::ThreadPool::GetInstance();

  const auto live_thread = pool->GetIdleThreadCount();
  ELOGI << "live thread number : " << live_thread;

  pool->Commit([pool](){
    ELOGI << "live thread number : " << pool->GetIdleThreadCount();
  });
  pool->Commit([pool](){
    ELOGI << "live thread number : " << pool->GetIdleThreadCount();
  });
  pool->Commit([pool](){
    ELOGI << "live thread number : " << pool->GetIdleThreadCount();
  });
  std::this_thread::sleep_for(std::chrono::microseconds(5));
  ELOGI << "live thread number : " << pool->GetIdleThreadCount();

  return 0;
}