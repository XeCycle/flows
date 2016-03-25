cmake_minimum_required(VERSION 2.8)
project(Flows)

set(Flows_CXXSTD_FLAG "-std=c++14" CACHE
  STRING "Compiler flag to specify language standard; must be >=c++11")

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} ${Flows_CXXSTD_FLAG} -Wall -Wpedantic -Wextra -Wconversion -Werror")

include_directories(include)

enable_testing()
add_subdirectory(test)
