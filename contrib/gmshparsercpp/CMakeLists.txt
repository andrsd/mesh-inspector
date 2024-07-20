cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gmshparsercpp
    VERSION 1.4
    LANGUAGES CXX
)

set(GMSHPARSERCPP_LIBRARY_TYPE "STATIC")
add_subdirectory(src)
