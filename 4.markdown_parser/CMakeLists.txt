cmake_minimum_required(VERSION 3.0)

project(MarkDownParser)

include_directories(include)

add_executable(mdps src/main.cc src/MarkdownTransform.cc)