cmake_minimum_required(VERSION 3.16)
project(spark-compiler)

add_subdirectory(lib/sparkc)

add_executable(spark-compiler
    src/main.cpp 
    src/printer/skr/SkrPrinter.cpp
    src/printer/rva/RvaPrinter.cpp
)
target_link_libraries(spark-compiler PRIVATE sparkc)
