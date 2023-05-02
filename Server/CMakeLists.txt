cmake_minimum_required(VERSION 3.11)

project(server CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.74.0 REQUIRED)

#if(Boost_FOUND)
#  include_directories(${Boost_INCLUDE_DIRS})
#endif()

set(CMAKE_CPP_COMPILER clang)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(server src/main.cpp)

target_link_libraries(server PRIVATE Threads::Threads)
