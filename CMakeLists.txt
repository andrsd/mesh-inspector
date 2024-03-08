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

find_package(Qt6 REQUIRED
    COMPONENTS
        Core
        Gui
        Widgets
        Network
        Svg
)

find_package(VTK
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
if(NOT VTK_FOUND)
    message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()
if(NOT(TARGET VTK::GUISupportQt))
    message(FATAL_ERROR "VTK not built with Qt support.")
endif()

CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)

add_subdirectory(contrib/gmshparsercpp)
add_subdirectory(common)
add_subdirectory(src)
