cmake_minimum_required(VERSION 3.0)
project(rttilib)
include_directories(include)
aux_source_directory(src src_list)
add_executable(rttilib ${src_list})
