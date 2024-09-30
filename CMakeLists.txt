cmake_minimum_required(VERSION 3.10...3.30)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
    if (CMAKE_CXX_COMPILER_VERSION MATCHES "12.*")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
    endif()
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

project(pinecone VERSION 0.0.1 LANGUAGES C CXX)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(third_party)