#pragma once

namespace fe {

// points
struct Point1D {
    double x;
};

struct Point2D {
    double x, y;
};

struct Point3D {
    double x, y, z;
};

typedef double double2[2];
typedef double double3[3];
typedef double double4[4];
typedef double double1x1[1][1];
typedef double double2x2[2][2];
typedef double double3x3[3][3];
typedef int int2[2];

typedef int order1_t;
typedef int order2_t;
typedef int order3_t;

//static const int VALUE_TYPES_2D = 3;

enum EValueType { FN = 0, DX = 1, DY = 2, DZ = 3 };

// Precalculation masks
enum {
    FN_VAL_0  = 0x00000001, /// Function values of the 1st component required

    FN_DX_0   = 0x00000002, /// First derivative in x of the 1st component required
    FN_DY_0   = 0x00000004, /// First derivative in y of the 1st component required
    FN_DZ_0   = 0x00000008, /// First derivative in z of the 1st component required

    FN_VAL_1  = 0x00000400, /// Function values of the 2nd component required
    FN_DX_1   = 0x00000800, /// First derivative in x of the 2nd component required
    FN_DY_1   = 0x00001000, /// First derivative in y of the 2nd component required
    FN_DZ_1   = 0x00002000, /// First derivative in y of the 2nd component required

    FN_VAL_2  = 0x00100000, /// Function values of the 3rd component required
    FN_DX_2   = 0x00200000, /// First derivative in x of the 3rd component required
    FN_DY_2   = 0x00400000, /// First derivative in y of the 3rd component required
    FN_DZ_2   = 0x00800000, /// First derivative in y of the 3rd component required
};

const int FN_VAL_1D = FN_VAL_0;
const int FN_DX_1D  = FN_DX_0;

const int FN_VAL_2D = FN_VAL_0 | FN_VAL_1;
const int FN_DX_2D  = FN_DX_0  | FN_DX_1;
const int FN_DY_2D  = FN_DY_0  | FN_DY_1;

const int FN_VAL_3D = FN_VAL_0 | FN_VAL_1 | FN_VAL_2;
const int FN_DX_3D  = FN_DX_0  | FN_DX_1  | FN_DX_2;
const int FN_DY_3D  = FN_DY_0  | FN_DY_1  | FN_DY_2;
const int FN_DZ_3D  = FN_DZ_0  | FN_DZ_1  | FN_DZ_2;

#define MEM_CHECK(var)

} // namespace fe
