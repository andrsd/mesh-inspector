cmake_minimum_required(VERSION 3.16)

project(common
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    ${PROJECT_NAME}
    OBJECT
        clickablelabel.cpp
        colorpicker.cpp
        doubleslider.cpp
        expandablewidget.cpp
        horzline.cpp
        infowidget.cpp
        loadfileevent.cpp
        notificationwidget.cpp
        otreeview.cpp
        sliderwithedit.cpp
)

generate_export_header(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_BINARY_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
)
