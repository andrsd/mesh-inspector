cmake_minimum_required(VERSION 3.16)

project(mesh-inspector
    VERSION 1.8
    LANGUAGES C CXX)

set(MESH_INSPECTOR_COPYRIGHT
    "Copyright © 2022-present, David Andrš, All Rights Reserved"
)
mark_as_advanced(FORCE MESH_INSPECTOR_COPYRIGHT)

option(MESH_INSPECTOR_BUILD_TESTS "Build tests" OFF)

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
        FiltersGeneric
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
        IOExportGL2PS
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

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(APP_ICON_MACOSX icon.icns)
endif()

add_subdirectory(contrib/fmt)
add_subdirectory(contrib/gmshparsercpp)
add_subdirectory(common)
add_subdirectory(src)

# main executable

add_executable(mesh-inspector
    main.cpp
    ${APP_ICON_MACOSX}
)

target_include_directories(
    mesh-inspector
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
)

target_link_libraries(
    mesh-inspector
    PRIVATE
        mesh-inspector-lib
)

set_target_properties(
    mesh-inspector
    PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Mesh Inspector"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_COPYRIGHT ${MESH_INSPECTOR_COPYRIGHT}
        MACOSX_BUNDLE_ICON_FILE icon.icns
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
)

set_source_files_properties(
    icon.icns
    ../resources/render-mode.svg
    PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
)

add_custom_target(make-icon-iconset-dir ALL
    COMMAND
        ${CMAKE_COMMAND} -E make_directory icon.iconset
)

add_custom_command(
    OUTPUT
        icon.iconset/icon_512x512.png
    COMMAND
        sips -z 512 512 ${CMAKE_SOURCE_DIR}/resources/app-icon.png --out icon.iconset/icon_512x512.png
    DEPENDS
        make-icon-iconset-dir
        ${CMAKE_SOURCE_DIR}/resources/app-icon.png
)

add_custom_command(
    OUTPUT icon.icns
    COMMAND iconutil -c icns icon.iconset
    DEPENDS icon.iconset/icon_512x512.png
)

# install

install(
    TARGETS
        mesh-inspector
        mesh-inspector-lib
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# test
if (MESH_INSPECTOR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
