//
// Created by 28192 on 2024/9/13.
//

#ifndef UTIL_H
#define UTIL_H

#include <cxxabi.h>
#include <pthread.h>

#include <string>

namespace pinecone::util {
    auto get_thread_id() -> pid_t;
    auto get_host_name() -> std::string_view;

    template<class T>
    auto type_to_name() -> std::string_view {
        static const auto s_name =
                std::string_view{abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr)};
        return s_name;
    }
}

#endif //UTIL_H
