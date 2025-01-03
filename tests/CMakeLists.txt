cmake_minimum_required(VERSION 3.10)

project(mesh-inspector-tests)

qt_standard_project_setup()

find_package(Qt6 COMPONENTS Core Gui Widgets Test REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

macro(add_qt_test NAME SOURCE)
    qt_add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} PUBLIC mesh-inspector-lib Qt6::Test)
    target_include_directories(${NAME}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/src
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
endmacro()

add_qt_test(color-profile-test ColorProfile_test.cpp)
