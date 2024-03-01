cmake_minimum_required(VERSION 3.23)
project(MyCPPSTL LANGUAGES CXX)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(src)
include_directories(easyloggingpp)

add_subdirectory(lib)
include_directories(googletest/include googletest)

add_executable(test_type_traits test/test_type_traits.cpp)
add_executable(test_iterator test/test_iterator.cpp)
add_executable(test_construct test/test_construct.cpp)
add_executable(test_utils test/test_utils.cpp)
add_executable(test_algobase test/test_algobase.cpp)
add_executable(test_memory test/test_memory.cpp)

add_executable(test_functional test/test_functional.cpp)
target_link_libraries(test_functional gtest gtest_main)

add_executable(test_vector test/test_vector.cpp)
target_link_libraries(test_vector gtest gtest_main)

add_executable(test_uninitialized test/test_uninitialized.cpp)
target_link_libraries(test_uninitialized gtest gtest_main)

add_executable(test_operator_overload test/test_operator_overload.cpp)
target_link_libraries(test_operator_overload gtest gtest_main)

add_executable(test_deque test/test_deque.cpp)
target_link_libraries(test_deque gtest gtest_main)
