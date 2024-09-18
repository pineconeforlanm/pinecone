include(${CMAKE_SOURCE_DIR}/config/pinecone.cmake)

set(UNIT_TEST_EXECUTABLE_NAME run_all_pinecone_unit_test)

add_executable(${UNIT_TEST_EXECUTABLE_NAME})

file(GLOB_RECURSE ALL_PCSG_TEST_SOURCE_FILES
        run_all_unit_test.cc
        "pinecone/*.cc"
)

set(PCSG_BASE_LINK_LIBS pinecone gtest gtest_main)

target_sources(${UNIT_TEST_EXECUTABLE_NAME} PRIVATE ${ALL_PCSG_TEST_SOURCE_FILES})
target_include_directories(${UNIT_TEST_EXECUTABLE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${UNIT_TEST_EXECUTABLE_NAME} PRIVATE ${PCSG_BASE_LINK_LIBS})
