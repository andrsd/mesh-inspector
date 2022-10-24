#pragma once

#include "shapeset.h"
#include "function3d.h"
#include "common.h"
#include "quadrature3d.h"
#include <stdexcept>

namespace fe {

// shape function in 3D
typedef double (*shape_fn_3d_t)(double, double, double);

// TODO: turn lambda fns into inline functions (so we get overloading, etc.)

// Affine coordinates 3D

#define lambda0(x, y, z) (((y) + 1) / 2)
#define lambda1(x, y, z) (-(1 + (x) + (y) + (z)) / 2)
#define lambda2(x, y, z) (((x) + 1) / 2)
#define lambda3(x, y, z) (((z) + 1) / 2)

// X derivatives of affine coordinates

#define lambda0dx(x, y, z) (0.0)
#define lambda1dx(x, y, z) (-1.0 / 2.0)
#define lambda2dx(x, y, z) (1.0 / 2.0)
#define lambda3dx(x, y, z) (0.0)

// Y derivatives of affine coordinates

#define lambda0dy(x, y, z) (1.0 / 2.0)
#define lambda1dy(x, y, z) (-1.0 / 2.0)
#define lambda2dy(x, y, z) (0.0)
#define lambda3dy(x, y, z) (0.0)

// Z derivatives of affine coordinates

#define lambda0dz(x, y, z) (0.0)
#define lambda1dz(x, y, z) (-1.0 / 2.0)
#define lambda2dz(x, y, z) (0.0)
#define lambda3dz(x, y, z) (1.0 / 2.0)

class Shapeset3D : public Shapeset {
public:
    Shapeset3D();

    /// @return index of a vertex shape function for a vertex
    /// @param [in] vertex - index of the vertex
    int get_vertex_index(int vertex) const
    {
        return this->vertex_indices[vertex];
    }

    ///
    order3_t get_order(int /*index*/) const
    {
        return order3_t(1);
    }

    /// Evaluate function in the set of points
    /// @param[in] n
    /// @param[in] index Index of the function being evaluate
    /// @param[in] np The number of points in array 'pt'
    /// @param[in] pt Points where the function is evaluated
    /// @param[in] component The number of component of the evaluated function
    /// @param[out] vals The array of values (caller is responsible for freeing this memory)
    void
    get_values(int n, int index, int np, const QuadPt3D * pt, int component, double * vals)
    {
        check_component(component);
        for (int k = 0; k < np; k++)
            vals[k] = this->shape_table[n][component][index](pt[k].x, pt[k].y, pt[k].z);
    }

    /// Evaluate function 'index' in points 'pt'
    double get_value(int n, int index, double x, double y, double z, int component)
    {
        check_component(component);
        return this->shape_table[n][component][index](x, y, z);
    }

    inline double
    get_fn_value(int index, double x, double y, double z, int component)
    {
        return get_value(FN, index, x, y, z, component);
    }

    inline double
    get_dx_value(int index, double x, double y, double z, int component)
    {
        return get_value(DX, index, x, y, z, component);
    }

    inline double
    get_dy_value(int index, double x, double y, double z, int component)
    {
        return get_value(DY, index, x, y, z, component);
    }

    inline double
    get_dz_value(int index, double x, double y, double z, int component)
    {
        return get_value(DZ, index, x, y, z, component);
    }

protected:
    shape_fn_3d_t ** shape_table[RealFunction3D::VALUE_TYPES];

    /// Indices of vertex shape functions on reference element, indexing: [vertex shape fn index]
    int * vertex_indices;
};

} // namespace fe
