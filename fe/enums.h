#pragma once

namespace fe {

// 1D element modes
enum EMode1D { MODE_LINE = 0 };

// 2D element modes
enum EMode2D { MODE_TRIANGLE = 0, MODE_QUAD = 1 };

// 3D element modes
enum EMode3D { MODE_TETRAHEDRON = 0, MODE_HEXAHEDRON = 1, MODE_PRISM = 2 };

}
