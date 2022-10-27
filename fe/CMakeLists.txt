cmake_minimum_required(VERSION 3.16)

project(fe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    ${PROJECT_NAME}
    SHARED
        element.cpp
        quadrature1d.cpp
        quadrature2d.cpp
        quadrature3d.cpp
        refdomain.cpp
        refmap1d.cpp
        refmap2d.cpp
        refmap3d.cpp
        refmapss.cpp
        shapefunction1d.cpp
        shapefunction2d.cpp
        shapefunction3d.cpp
        shapeset1d.cpp
        shapeset2d.cpp
        shapeset3d.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        ${VTK_LIBRARIES}
)
