#pragma once

#include "shapeset.h"
#include "function2d.h"
#include "common.h"
#include "quadrature2d.h"
#include <stdexcept>

namespace fe {

// shape function in 2D
typedef double (*shape_fn_2d_t)(double, double);

// TODO: turn lambda fns into inline functions (so we get overloading, etc.)

// Affine coordinates 2D

#define lambda0_2d(x, y) (((y) + 1) / 2)
#define lambda1_2d(x, y) (-((x) + (y)) / 2)
#define lambda2_2d(x, y) (((x) + 1) / 2)

// x derivatives of affine coordinates
#define lambda0x_2d(x, y) (0.0)
#define lambda1x_2d(x, y) (-1.0 / 2.0)
#define lambda2x_2d(x, y) (1.0 / 2.0)

// y derivatives of affine coordinates
#define lambda0y_2d(x, y) (1.0 / 2.0)
#define lambda1y_2d(x, y) (-1.0 / 2.0)
#define lambda2y_2d(x, y) (0.0)

class Shapeset2D : public Shapeset {
public:
    Shapeset2D();

    /// @return index of a vertex shape function for a vertex
    /// @param [in] vertex - index of the vertex
    int get_vertex_index(int vertex) const
    {
        return this->vertex_indices[vertex];
    }

    order2_t
    get_order(int /*index*/) const
    {
        return order2_t(1);
    }

    /// Evaluate function in the set of points
    /// @param[in] n
    /// @param[in] index Index of the function being evaluate
    /// @param[in] np The number of points in array 'pt'
    /// @param[in] pt Points where the function is evaluated
    /// @param[in] component The number of component of the evaluated function
    /// @param[out] vals The array of values (caller is responsible for freeing this memory)
    void
    get_values(int n, int index, int np, const QuadPt2D * pt, int component, double * vals)
    {
        check_component(component);
        for (int k = 0; k < np; k++)
            vals[k] = this->shape_table[n][component][index](pt[k].x, pt[k].y);
    }

    double
    get_value(int n, int index, double x, double y, int component)
    {
        check_component(component);
        return this->shape_table[n][component][index](x, y);
    }

    inline double
    get_fn_value(int index, double x, double y, int component)
    {
        return get_value(FN, index, x, y, component);
    }

    inline double
    get_dx_value(int index, double x, double y, int component)
    {
        return get_value(DX, index, x, y, component);
    }

    inline double
    get_dy_value(int index, double x, double y, int component)
    {
        return get_value(DY, index, x, y, component);
    }

protected:
    shape_fn_2d_t ** shape_table[RealFunction2D::VALUE_TYPES];

    /// Indices of vertex shape functions on reference element, indexing: [vertex shape fn index]
    int * vertex_indices;
};

} // namespace fe
