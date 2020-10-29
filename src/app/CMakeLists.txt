cmake_minimum_required(VERSION 3.10)

include_directories(../lib)
link_directories(../lib)

project(brandy0)

file(GLOB all_SRC "*.cpp")

add_executable(brandy0 ${all_SRC})
