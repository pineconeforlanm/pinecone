//
// Created by pinec on 2024/9/30.
//
#define GLOG_USE_GLOG_EXPORT

#include <boost/stacktrace.hpp>
#include <ylt/easylog.hpp>
#include <glog/logging.h>

auto FuncTest02() -> void {
  ELOGW << "Hello World!";
  ELOGF << "easylog test02";
}

auto FuncTest01() -> void {
  FuncTest02();
}

auto main(int argc, char **argv) -> int {
  google::InitGoogleLogging(argv[0]);
  easylog::set_console(true);
  easylog::set_async(false);
  FuncTest01();
  google::ShutdownGoogleLogging();
  return 0;
}