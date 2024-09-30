//
// Created by pinec on 2024/9/24.
//
#include <chrono>
#include <iostream>
#include <pinecone/daemon.hpp>
#include <stdexcept>
#include <sstream>
#include <thread>
#include <ylt/easylog.hpp>

auto main(int argc, char** argv) -> int {
  easylog::set_async(false);
  easylog::set_console(true);
  pinecone::StartDaemon(argc, argv, []([[maybe_unused]] int m_argc, [[maybe_unused]] char** m_argv) -> int {
        const auto process_mgr = pinecone::ProcessInfo::GetInstance();
    std::cout << process_mgr->ToString();
    while (true) {
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
          ELOGI << "this main daemon times is " << i << ":" << j;
          std::this_thread::sleep_for(std::chrono::seconds(1));
          if (i == 2) {
            std::stringstream ss;
            ss << "times i = " << i;
            throw std::runtime_error(ss.str());
          }
        }
      }
    }
    return 0;
  },true);

  return 0;
}