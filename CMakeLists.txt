cmake_minimum_required(VERSION 3.16)
project(sparkc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)

add_subdirectory(src/sparkc)
add_subdirectory(src/standalone)
