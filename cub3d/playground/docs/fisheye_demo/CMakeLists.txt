cmake_minimum_required(VERSION 3.16)
project(fisheye_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics REQUIRED)

add_executable(fisheye_demo main.cpp)
target_link_libraries(fisheye_demo SFML::Graphics)
