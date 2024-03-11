cmake_minimum_required(VERSION 3.16)

project(mesh-inspector
    VERSION 1.6
    LANGUAGES C CXX)

set(MESH_INSPECTOR_COPYRIGHT
    "Copyright © 2022-present, David Andrš, All Rights Reserved"
)
mark_as_advanced(FORCE MESH_INSPECTOR_COPYRIGHT)

math(EXPR MESH_INSPECTOR_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
math(EXPR MESH_INSPECTOR_MINOR_VERSION ${PROJECT_VERSION_MINOR})

include(CheckIncludeFiles)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4
    COMPONENTS
        Core
        Gui
        Widgets
        Network
        Svg
)
if(Qt6_FOUND)
    message(STATUS "Found Qt6: ${Qt6_VERSION_MAJOR}.${Qt6_VERSION_MINOR}.${Qt6_VERSION_PATCH} (found in: ${Qt6_DIR})")
else()
    message(FATAL_ERROR "Unable to find the Qt6.")
endif()

find_package(VTK 9.3.0...<9.4.0
    COMPONENTS
        CommonCore
        FiltersExtraction
        FiltersHybrid
        FiltersVerdict
        GUISupportQt
        InteractionStyle
        RenderingAnnotation
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
        RenderingQt
        IOExodus
        IOGeometry
        IOImage
        IOLegacy
        IOXML
)
if(VTK_FOUND)
    message(STATUS "Found VTK: ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION} (found in: ${VTK_DIR})")
else()
    message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()
if(NOT(TARGET VTK::GUISupportQt))
    message(FATAL_ERROR "VTK not built with Qt support.")
endif()

CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)

add_subdirectory(contrib/gmshparsercpp)
add_subdirectory(common)
add_subdirectory(src)
