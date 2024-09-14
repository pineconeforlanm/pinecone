file(GLOB ALL_PCSG_TEST_FILES "./*.cc")

set(EXECUTABLE_PREFIX "test_pinecone_base")

set(ALL_PCSG_TEST_LIBS pinecone)

foreach (TEST_EXECUTABLE ${ALL_PCSG_TEST_FILES})
    get_filename_component(FILENAME_WITH_EXT ${TEST_EXECUTABLE} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" FILENAME_WITHOUT_EXT ${FILENAME_WITH_EXT})
    message("add test name : ${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT}")
    add_executable(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} ${TEST_EXECUTABLE})
    target_include_directories(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} PRIVATE "${CMAKE_SOURCE_DIR}")
    target_link_libraries(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} ${ALL_PCSG_TEST_LIBS})
endforeach ()