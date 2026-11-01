cmake_minimum_required(VERSION 4.2)

project(PFM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(run 
src/main.cpp
)

target_include_directories(run PRIVATE include)