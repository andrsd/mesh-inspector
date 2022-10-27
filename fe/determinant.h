#pragma once

#include "common.h"

namespace fe {

inline double
det(const double1x1 & m)
{
    return m[0][0];
}

inline double
det(const double2x2 & m)
{
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

inline double
det(const double3x3 & m)
{
    return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
           m[2][0] * m[1][1] * m[0][2] - m[2][1] * m[1][2] * m[0][0] - m[2][2] * m[1][0] * m[0][1];
}

} // namespace fe
