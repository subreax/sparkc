@echo off
mkdir build
cd build
cmake .. -G Ninja -D CMAKE_BUILD_TYPE=Debug
cd ..
