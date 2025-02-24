cmake_minimum_required(VERSION 3.30)
project(tomlin)
add_subdirectory(traypp EXCLUDE_FROM_ALL)
link_libraries(tray)
set(CMAKE_CXX_STANDARD 20)

add_executable(tomlin main.cpp)
