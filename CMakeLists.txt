cmake_minimum_required(VERSION 3.10...3.30)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

project(pinecone VERSION 0.0.1 LANGUAGES C CXX)

add_subdirectory(pinecone)
add_subdirectory(test)