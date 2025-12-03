add_executable(spark-compiler
    src/main.cpp 
    src/printer/skr/SkrPrinter.cpp
    src/printer/rva/RvaPrinter.cpp
)

target_link_libraries(spark-compiler PRIVATE sparkc)
