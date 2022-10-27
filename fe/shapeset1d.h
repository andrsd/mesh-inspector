#pragma once

#include "shapeset.h"
#include "function1d.h"
#include "common.h"
#include "quadrature1d.h"
#include <stdexcept>

namespace fe {

// shape function in 1D
typedef double (*shape_fn_1d_t)(double);

class Shapeset1D : public Shapeset {
public:
    Shapeset1D();

    /// @return index of a vertex shape function for a vertex
    /// @param [in] vertex index of the vertex
    int
    get_vertex_index(int vertex) const
    {
        return this->vertex_indices[vertex];
    }

    ///
    order1_t
    get_order(int /*index*/) const
    {
        return order1_t(1);
    }

    /// Evaluate function in the set of points
    /// @param[in] n
    /// @param[in] index Index of the function being evaluate
    /// @param[in] np The number of points in array 'pt'
    /// @param[in] pt Points where the function is evaluated
    /// @param[in] component The number of component of the evaluated function
    /// @param[out] vals The array of values (caller is responsible for freeing this memory)
    void
    get_values(int n, int index, int np, const QuadPt1D * pt, int component, double * vals)
    {
        check_component(component);
        for (int k = 0; k < np; k++)
            vals[k] = this->shape_table[n][component][index](pt[k].x);
    }

    /// Evaluate function 'index' in points 'pt'
    virtual double
    get_value(int n, int index, double x, int component)
    {
        check_component(component);
        return this->shape_table[n][component][index](x);
    }

    inline double
    get_fn_value(int index, double x, int component)
    {
        return get_value(FN, index, x, component);
    }

    inline double
    get_dx_value(int index, double x, int component)
    {
        return get_value(DX, index, x, component);
    }

protected:
    shape_fn_1d_t ** shape_table[RealFunction1D::VALUE_TYPES];

    /// Indices of vertex shape functions on reference element, indexing: [vertex shape fn index]
    int * vertex_indices;
};

} // namespace fe
