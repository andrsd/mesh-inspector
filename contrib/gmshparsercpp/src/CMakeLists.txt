add_library(${PROJECT_NAME}
    ${GMSHPARSERCPP_LIBRARY_TYPE}
        Exception.cpp
        MshFile.cpp
        MshLexer.cpp
)

if(CMAKE_PROJECT_NAME STREQUAL "gmshparsercpp")
    target_code_coverage(${PROJECT_NAME})
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        fmt::fmt
)
