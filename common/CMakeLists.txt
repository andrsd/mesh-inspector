cmake_minimum_required(VERSION 3.16)

project(common
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    ${PROJECT_NAME}
    SHARED
        clickablelabel.cpp
        colorpicker.cpp
        expandablewidget.cpp
        horzline.cpp
        infowidget.cpp
        loadfileevent.cpp
        notificationwidget.cpp
        otreeview.cpp
        sliderwithedit.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Qt6::Widgets
)

install(
    TARGETS
        ${PROJECT_NAME}
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
)
