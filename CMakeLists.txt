cmake_minimum_required(VERSION 3.16)

project(Containerium VERSION 0.1.0 LANGUAGES CXX)

add_library(containerium INTERFACE)
add_library(containerium::containerium ALIAS containerium)

target_compile_features(containerium INTERFACE cxx_std_23)
target_include_directories(containerium INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
