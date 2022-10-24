#pragma once

#include <stdexcept>

namespace fe {

struct QuadPt2D {
    /// x and y-coordinate
    double x, y;
    /// weight
    double w;

    QuadPt2D() {}
    QuadPt2D(double x, double y, double w)
    {
        this->x = x;
        this->y = y;
        this->w = w;
    }

    double
    operator[](int idx) const
    {
        if (idx == 0)
            return this->x;
        else if (idx == 1)
            return this->y;
        else
            throw std::invalid_argument("Index out of bounds");
    }
};

class Quadrature2D {
public:
    Quadrature2D();

    /// Get maximum integration order
    ///
    /// @return Maximum integration order
    int get_max_order() const { return max_order; }

    /// Get points
    ///
    /// @param order Integration order
    /// @return Array of 2D quad points for order `order`
    QuadPt2D *get_points(int order) const { return this->tables[order]; }

    /// Get number of quad points
    ///
    /// @param order Integration order
    /// @return Number of integration points for order `order`
    inline int get_num_points(int order) const { return this->np[order]; };

protected:
    /// maximum order for integration
    int max_order;
    /// number of integration points
    /// indexing: [order]
    int *np;
    /// tables with integration points
    /// indexing: [order][point no.]
    QuadPt2D **tables;
};

/// Standard quadrature on triangles
///
class QuadStdTri : public Quadrature2D {
public:
    QuadStdTri();
};

/// Standard quadrature on quadrilaterals
///
class QuadStdQuad : public Quadrature2D {
public:
    QuadStdQuad();
};

}
