# Build icon.icns from a single PNG file
#
# @param FILE Path to the PNG file
# @param SIZE Size of the icon in pixels
macro(make_icon_icns)
    cmake_parse_arguments(ARG "" "FILE;SIZE" "" ${ARGN})

    set_source_files_properties(
        icon.icns
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )

    add_custom_target(make-icon-iconset-dir ALL
        COMMAND
            ${CMAKE_COMMAND} -E make_directory icon.iconset
    )

    add_custom_command(
        OUTPUT
            icon.iconset/icon_${ARG_SIZE}x${ARG_SIZE}.png
        COMMAND
            sips -z ${ARG_SIZE} ${ARG_SIZE} ${ARG_FILE} --out icon.iconset/icon_${ARG_SIZE}x${ARG_SIZE}.png
        DEPENDS
            make-icon-iconset-dir
            ${ARG_FILE}
    )

    add_custom_command(
        OUTPUT icon.icns
        COMMAND iconutil -c icns icon.iconset
        DEPENDS icon.iconset/icon_${ARG_SIZE}x${ARG_SIZE}.png
    )
endmacro()
