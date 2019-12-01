cmake_minimum_required(VERSION 3.0)

project(MarkDownParser)
set(CMAKE_CXX_STANDARD 17)
include_directories(include)

aux_source_directory(src src_list)

add_executable(mdps ${src_list})