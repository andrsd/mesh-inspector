#pragma once

#include <stdexcept>

namespace fe {

struct QuadPt3D {
    /// x, y and z-coordinate
    double x, y, z;
    /// weight
    double w;

    QuadPt3D() {}
    QuadPt3D(double x, double y, double z, double w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    double
    operator[](int idx) const
    {
        if (idx == 0)
            return this->x;
        else if (idx == 1)
            return this->y;
        else if (idx == 2)
            return this->z;
        else
            throw std::invalid_argument("Index out of bounds");
    }
};

class Quadrature3D {
public:
    Quadrature3D();

protected:
    /// maximum order for integration
    int max_order;
    /// number of integration points
    /// indexing: [order]
    int *np;
    /// tables with integration points
    /// indexing: [order][point no.]
    QuadPt3D **tables;
};

/// Standard quadrature on 3D hexahedron
///
class QuadStdHex : public Quadrature3D {
public:
    QuadStdHex();
};

/// Standard quadrature on 3D tetrahedron
///
class QuadStdTetra : public Quadrature3D {
public:
    QuadStdTetra();
};

}
