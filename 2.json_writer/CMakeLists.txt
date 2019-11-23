cmake_minimum_required(VERSION 3.0)
project(JsonWriter)

include_directories(include)

add_executable(jsonwriter src/jsonWriter.cc src/test.cc)

