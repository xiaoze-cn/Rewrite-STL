cmake_minimum_required(VERSION 3.20)

project(ds VERSION 0.1.0 LANGUAGES CXX)

add_library(ds INTERFACE)
add_library(ds::ds ALIAS ds)

target_compile_features(ds INTERFACE cxx_std_23)
target_include_directories(ds INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
