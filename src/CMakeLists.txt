add_executable(
    ${PROJECT_NAME}
        aboutdlg.cpp
        blockobject.cpp
        boundingbox.cpp
        exodusiireader.cpp
        explodewidget.cpp
        filechangednotificationwidget.cpp
        infowindow.cpp
        main.cpp
        mainwindow.cpp
        meshobject.cpp
        nodesetobject.cpp
        reader.cpp
        sidesetobject.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}
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
