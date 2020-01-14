cmake_minimum_required(VERSION 3.5)

project(example)

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)

  set(CMAKE_BUILD_TYPE Release)

endif()

set(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
add_executable(example ${SOURCE_FILES})

set(tfsecured_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../tfsecured/cpp")
add_library(tfsecured INTERFACE)
target_include_directories( tfsecured INTERFACE
    $<BUILD_INTERFACE:${tfsecured_dir}>
)
include_directories(
    ${tfsecured_dir}
)

find_package(TensorflowCC REQUIRED)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(Protobuf REQUIRED)

target_link_libraries(
    example

    ${OPENSSL_LIBRARIES}
    ${Protobuf_LIBRARIES}
    TensorflowCC::TensorflowCC
    tfsecured
)