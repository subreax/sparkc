cmake_minimum_required(VERSION 3.16)
project(spark-compiler)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/lyra)
add_subdirectory(lib/sparkc)

add_executable(spark-compiler
    src/main.cpp 
    src/printer/rva/RvaPrinter.cpp
)
target_include_directories(spark-compiler PRIVATE src/)
target_link_libraries(spark-compiler PRIVATE lyra sparkc)
