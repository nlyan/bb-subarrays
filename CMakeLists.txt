cmake_minimum_required(VERSION 3.4)

project (subarrays)

set (CXX_STANDARD 14)
set (CXX_STANDARD_REQUIRED ON)
set (CXX_EXTENSIONS OFF)

include_directories ("./range-v3/include")
add_executable (${PROJECT_NAME} "main.cpp")
