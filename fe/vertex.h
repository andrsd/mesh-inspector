#pragma once

namespace fe {

class Vertex1D {
public:
    static const int NUM_COORDS = 2;

    Vertex1D() : x(0.) {}
    Vertex1D(double _x) : x(_x) {}

    /// x-coordinate
    double x;
};

class Vertex2D {
public:
    static const int NUM_COORDS = 2;

    Vertex2D() : x(0.), y(0.) {}
    Vertex2D(double _x, double _y) : x(_x), y(_y) {}

    /// x-, y-coordinates
    double x, y;
};

class Vertex3D {
public:
    static const int NUM_COORDS = 3;

    Vertex3D() : x(0.), y(0.), z(0.) {}
    Vertex3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

    /// x-, y-, z-coordinates
    double x, y, z;
};

} // namespace fe
