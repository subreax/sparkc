cmake_minimum_required(VERSION 3.16)
project(sparkc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)

add_subdirectory(src)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests 
  tests/asm/test_rv32base.cpp
  tests/alloc/test_linearalloc.cpp
  tests/rva/RvaMovTest.cpp
  src/spark/backend/rv/asm/Rv32Base.cpp
  src/spark/common/Constant.cpp
  src/spark/skr/value/SkrValue.cpp
)
target_include_directories(tests PUBLIC src/spark/)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)