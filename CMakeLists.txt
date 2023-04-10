cmake_minimum_required(VERSION 3.23)
project(MyCPPSTL LANGUAGES CXX)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(src)

add_executable(test_vector test/test_vector.cpp)
add_executable(test_type_traits test/test_type_traits.cpp)
