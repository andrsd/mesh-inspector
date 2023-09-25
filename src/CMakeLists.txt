if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(APP_ICON_MACOSX icon.icns)
endif()

add_executable(
    ${PROJECT_NAME}
        aboutdlg.cpp
        blockobject.cpp
        colorprofile.cpp
        exodusiireader.cpp
        explodetool.cpp
        explodewidget.cpp
        filechangednotificationwidget.cpp
        infowindow.cpp
        licensedlg.cpp
        main.cpp
        mainwindow.cpp
        meshmodel.cpp
        meshobject.cpp
        meshqualitywidget.cpp
        nodesetobject.cpp
        ointeractorinterface.cpp
        ointeractorstyle2d.cpp
        ointeractorstyle3d.cpp
        reader.cpp
        selection.cpp
        sidesetobject.cpp
        stlreader.cpp
        resources.qrc
        vtkreader.cpp
        ${APP_ICON_MACOSX}
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        common
        ${VTK_LIBRARIES}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Mesh Inspector"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_COPYRIGHT ${MESH_INSPECTOR_COPYRIGHT}
        MACOSX_BUNDLE_ICON_FILE icon.icns
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
)

set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

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
    OUTPUT
        icon.icns
    COMMAND
        iconutil -c icns icon.iconset
    DEPENDS
        icon.iconset/icon_512x512.png
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
