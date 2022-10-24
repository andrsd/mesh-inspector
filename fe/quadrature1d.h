#pragma once

#include <stdexcept>

namespace fe {

struct QuadPt1D {
    /// x-coordinate
    double x;
    /// weight
    double w;

    QuadPt1D() {}
    QuadPt1D(double x, double w)
    {
        this->x = x;
        this->w = w;
    }

    double
    operator[](int idx) const
    {
        if (idx == 0)
            return this->x;
        else
            throw std::invalid_argument("Index out of bounds");
    }
};

/// Standard quadrature on 1D edge
///
class Quadrature1D {
public:
    Quadrature1D();
    virtual ~Quadrature1D();

    QuadPt1D *
    get_points(int order) const
    {
        return tables[order];
    }

    int
    get_num_points(int order) const
    {
        return np[order];
    };

    int
    get_max_order() const
    {
        return max_order;
    }

protected:
    /// maximal order for integration
    int max_order;

    /// number of integration points
    /// indexing: [order]
    int * np;

    /// tables with integration points
    /// indexing: [order][point no.]
    QuadPt1D ** tables;
};

class QuadStd1D : public Quadrature1D {
public:
    QuadStd1D();
    virtual ~QuadStd1D();
};

}
