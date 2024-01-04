cmake_minimum_required(VERSION 3.28.1)
project(mgbda)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(lib)
add_subdirectory(testing)
add_subdirectory(x86_64)