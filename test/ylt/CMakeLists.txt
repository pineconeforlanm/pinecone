file(GLOB ALL_YLT_TEST_FILES "./*.cc")

set(EXECUTABLE_PREFIX "test_ylt_base")

set(ALL_YLT_TEST_LIBS glog stdc++exp)

foreach (TEST_EXECUTABLE ${ALL_YLT_TEST_FILES})
    get_filename_component(FILENAME_WITH_EXT ${TEST_EXECUTABLE} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" FILENAME_WITHOUT_EXT ${FILENAME_WITH_EXT})
    message("add test name : ${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT}")
    add_executable(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} ${TEST_EXECUTABLE})
    target_include_directories(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    target_include_directories(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} PRIVATE "${CMAKE_SOURCE_DIR}/third_party/yalantinglibs/include")
    target_include_directories(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} PRIVATE "${CMAKE_SOURCE_DIR}/third_party/yalantinglibs/include/ylt/standalone")
    target_include_directories(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} PRIVATE "${CMAKE_SOURCE_DIR}/third_party/yalantinglibs/include/ylt/thirdparty")
    target_link_libraries(${EXECUTABLE_PREFIX}_${FILENAME_WITHOUT_EXT} ${ALL_YLT_TEST_LIBS})
endforeach ()