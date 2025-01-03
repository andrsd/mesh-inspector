file(GLOB SRCS CONFIGURE_DEPENDS *.cpp)

add_library(
    mesh-inspector-lib
    SHARED
        ${SRCS}
        resources.qrc
)

set_target_properties(mesh-inspector-lib
    PROPERTIES
        OUTPUT mesh-inspector-lib
)

target_include_directories(
    mesh-inspector-lib
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/contrib/gmshparsercpp/include
)

target_link_libraries(
    mesh-inspector-lib
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Svg
        common
        ${VTK_LIBRARIES}
        gmshparsercpp
)

vtk_module_autoinit(
    TARGETS mesh-inspector-lib
    MODULES ${VTK_LIBRARIES}
)

configure_file(meshinspectorconfig.h.in meshinspectorconfig.h)
