cmake_minimum_required(VERSION 3.10)

project(mesh-inspector-tests)

qt_standard_project_setup()

find_package(Qt6 COMPONENTS Core Gui Widgets Test REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(mesh-inspector-lib STATIC ${TEST_SOURCES})

target_include_directories(mesh-inspector-lib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/src
)

target_link_libraries(mesh-inspector-lib
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Svg
        Qt6::Test
        common
        ${VTK_LIBRARIES}
        gmshparsercpp
)

macro(add_qt_test NAME SOURCE)
    qt_add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} PUBLIC mesh-inspector-lib)
    target_include_directories(${NAME}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/src
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
endmacro()

add_qt_test(color-profile-test ColorProfile_test.cpp)
