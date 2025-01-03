cmake_minimum_required(VERSION 3.1...3.18)

project(FMT CXX)

# Define the fmt library, its includes and the needed defines.
set(FMT_HEADERS args.h chrono.h color.h compile.h core.h format.h
    format-inl.h os.h ostream.h printf.h ranges.h std.h
    xchar.h)
set(FMT_SOURCES src/format.cc src/os.cc)

add_library(fmt ${FMT_SOURCES})
add_library(fmt::fmt ALIAS fmt)

target_include_directories(fmt ${FMT_SYSTEM_HEADERS_ATTRIBUTE} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${FMT_INC_DIR}>)

set_target_properties(fmt PROPERTIES
    VERSION ${FMT_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR}
    PUBLIC_HEADER "${FMT_HEADERS}"
    DEBUG_POSTFIX "${FMT_DEBUG_POSTFIX}"
    POSITION_INDEPENDENT_CODE ON
)
