cmake_minimum_required(VERSION 3.16)
project(raycasting_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics REQUIRED)

add_executable(raycasting main.cpp)
target_link_libraries(raycasting SFML::Graphics)
