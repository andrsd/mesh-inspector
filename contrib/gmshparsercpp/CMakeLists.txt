cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gmshparsercpp
    VERSION 1.3
    LANGUAGES CXX
)

set(GMSHPARSERCPP_LIBRARY_TYPE "STATIC")

if (GMSHPARSERCPP_WITH_FMT)
    find_package(fmt 0.8 REQUIRED)
endif()

add_subdirectory(src)
