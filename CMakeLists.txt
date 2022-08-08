cmake_minimum_required(VERSION 3.16)

project(mesh-inspector
    VERSION 1.0
    LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED
    COMPONENTS
        Core
        Gui
        Widgets
)

find_package(VTK
    COMPONENTS
        CommonCore
        FiltersExtraction
        FiltersHybrid
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
)
if(NOT VTK_FOUND)
    message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()
if(NOT(TARGET VTK::GUISupportQt))
    message(FATAL_ERROR "VTK not built with Qt support.")
endif()

add_subdirectory(common)

add_executable(
    ${PROJECT_NAME}
        aboutdlg.cpp
        blockobject.cpp
        exodusiireader.cpp
        infowindow.cpp
        main.cpp
        mainwindow.cpp
        reader.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        common
        ${CMAKE_BUILD_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        common
        ${VTK_LIBRARIES}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Mesh Inspector"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
)

install(
    TARGETS
        ${PROJECT_NAME}
    BUNDLE DESTINATION
        .
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
)

configure_file(meshinspectorconfig.h.in meshinspectorconfig.h)
