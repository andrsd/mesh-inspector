#pragma once

namespace gmshparsercpp {

/// See [1] for details
///
/// [1] https://gmsh.info/doc/texinfo/gmsh.html#MSH-file-format
enum ElementType {
    NONE = -1,
    LINE2 = 1,
    TRI3 = 2,
    QUAD4 = 3,
    TET4 = 4,
    HEX8 = 5,
    PRISM6 = 6,
    PYRAMID5 = 7,
    LINE3 = 8,
    TRI6 = 9,
    QUAD9 = 10,
    TET10 = 11,
    HEX27 = 12,
    PRISM18 = 13,
    PYRAMID14 = 14,
    POINT = 15,
    QUAD8 = 16,
    HEX20 = 17,
    PRISM15 = 18,
    PYRAMID13 = 19,
    ITRI9 = 20,
    TRI10 = 21,
    ITRI12 = 22,
    TRI15 = 23,
    ITRI15 = 24,
    TRI21 = 25,
    LINE4 = 26,
    LINE5 = 27,
    LINE6 = 28,
    TET20 = 29,
    TET35 = 30,
    TET56 = 31,
    HEX64 = 92,
    HEX125 = 93
};

} // namespace gmshparsercpp
