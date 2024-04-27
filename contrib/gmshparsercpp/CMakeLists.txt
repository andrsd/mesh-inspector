cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gmshparsercpp
    VERSION 1.3
    LANGUAGES CXX
)

set(GMSHPARSERCPP_LIBRARY_TYPE "STATIC")

find_package(fmt 0.8 REQUIRED)

add_subdirectory(src)
