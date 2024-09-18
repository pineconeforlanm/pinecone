include(${CMAKE_SOURCE_DIR}/config/pinecone.cmake)

set(PINECONE_LINK_LIB_LISTS Boost protobuf OpenSSL nlohmann_json)

foreach (PINECONE_LINK_LIB ${PINECONE_LINK_LIB_LISTS})
    find_package(${PINECONE_LINK_LIB} REQUIRED)
endforeach ()

set(PINECONE_BASE_LINK_LIBS rt dl pthread openssl::openssl protobuf::protobuf nlohmann_json::nlohmann_json)

add_library(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})

#file(GLOB_RECURSE ALL_PINECONE_LIB_HEADER_FILES "./*.hpp")

file(GLOB_RECURSE ALL_PINECONE_LIB_SOURCE_FILES "./*.cc")

include_directories("${CMAKE_SOURCE_DIR}/third_party/yalantinglibs/include")
include_directories("${CMAKE_SOURCE_DIR}/third_party/yalantinglibs/include/ylt/thirdparty")

target_sources(${PROJECT_NAME} PRIVATE ${ALL_PINECONE_LIB_HEADER_FILES} ${ALL_PINECONE_LIB_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/third_party/yalantinglibs/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/third_party/yalantinglibs/include/ylt/thirdparty")

target_link_libraries(${PROJECT_NAME} PRIVATE ${PINECONE_BASE_LINK_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_VERSION="${PROJECT_VERSION}")
target_compile_definitions(${PROJECT_NAME} PUBLIC GIT_VERSION="${GIT_VERSION}")
target_compile_definitions(${PROJECT_NAME} PUBLIC GIT_DATE="${GIT_DATE}")
target_compile_definitions(${PROJECT_NAME} PUBLIC GIT_HASH="${GIT_HASH}")
target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_TIME="${BUILD_TIME}")