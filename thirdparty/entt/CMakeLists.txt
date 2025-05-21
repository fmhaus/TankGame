cmake_minimum_required(VERSION 3.1)
project(entt)

add_library(entt INTERFACE)
target_include_directories(entt INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_compile_features(entt INTERFACE cxx_std_17)
