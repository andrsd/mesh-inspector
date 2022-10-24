#pragma once

#include "common.h"
#include "quadrature2d.h"
#include "function2d.h"
#include "element.h"

#define CHECK_PARAMS
#define CHECK_TABLE(n, msg)

namespace fe {

class Element;

template <typename TYPE>
class Function2D {
public:
    Function2D();
    virtual ~Function2D();

    /// @return The polynomial degree of the function currently being represented by the class.
    order2_t
    get_fn_order() const
    {
        return order;
    }

    /// @return The number of vector components of the function being represented by the class.
    int
    get_num_components() const
    {
        return num_components;
    }

    /// @param component [in] The component of the function (0-2).
    /// @return The values of the function at all points of the current integration rule.
    TYPE *
    get_fn_values(int component = 0)
    {
        CHECK_PARAMS;
        CHECK_TABLE(FN, "Function values");
        return cur_node->values[component][FN];
    }

    /// @param component [in] The component of the function (0-2).
    /// @return The x partial derivative of the function at all points of the current integration
    /// rule.
    TYPE *
    get_dx_values(int component = 0)
    {
        CHECK_PARAMS;
        CHECK_TABLE(DX, "DX values");
        return cur_node->values[component][DX];
    }

    /// @param component [in] The component of the function (0-2).
    /// @return The y partial derivative of the function at all points of the current integration
    /// rule.
    TYPE *
    get_dy_values(int component = 0)
    {
        CHECK_PARAMS;
        CHECK_TABLE(DY, "DY values");
        return cur_node->values[component][DY];
    }

    /// This function provides the both often-used dx and dy values in one call.
    /// @param dx [out] Variable which receives the pointer to the first partial derivatives by x
    /// @param dy [out] Variable which receives the pointer to the first partial derivatives by y
    /// @param component [in] The component of the function (0 or 1).
    void
    get_dx_dy_values(TYPE *& dx, TYPE *& dy, int component = 0)
    {
        CHECK_PARAMS;
        CHECK_TABLE(DX, "DX values");
        CHECK_TABLE(DY, "DY values");

        dx = cur_node->values[component][DX];
        dy = cur_node->values[component][DY];
    }

    /// For internal use.
    TYPE *
    get_values(int component, int b)
    {
        CHECK_PARAMS;
        return cur_node->values[component][b];
    }

    /// Frees all precalculated tables.
    virtual void free() = 0;

    /// precalculates the current function at the current integration points.
    virtual void precalculate(const int np, const QuadPt2D * pt, int mask) = 0;

public:
    static const int COMPONENTS = 2;
    static const int VALUE_TYPES = 3;

protected:
    static const int QUAD_COUNT = 8;

    /// Active element
    Element * elem;
    /// current function polynomial order
    order2_t order;
    /// number of vector components
    int num_components;

    struct Node {
        /// a combination of FN_XXX: specifies which tables are present
        int mask;
        /// size in bytes of this struct (for maintaining total_mem)
        int size;
        /// pointers to 'data'
        TYPE * values[COMPONENTS][VALUE_TYPES];
        /// value tables
        TYPE data[0];
    };

    Node * cur_node;

    /// list of available quadratures
    Quadrature2D * quads[QUAD_COUNT];
    /// active quadrature (index into 'quads')
    int cur_quad;

    /// searches 'quads' for the given quadrature
    int find_quad(Quadrature2D * quad);

    ///
    int register_quad(Quadrature2D * quad);

    /// allocates a new Node structure
    virtual Node * new_node(int mask, int num_points);

    void
    free_cur_node()
    {
        if (cur_node != NULL) {
            ::free(cur_node);
            cur_node = NULL;
        }
    }

    void
    replace_cur_node(Node * node)
    {
        free_cur_node();
        cur_node = node;
    }

    /// index to mask table (3 dimensions)
    static int idx2mask[][COMPONENTS];
};

// the order of items must match values of EValueType
template<typename TYPE>
int Function2D<TYPE>::idx2mask[][COMPONENTS] = {
    { FN_VAL_0, FN_VAL_1 },
    { FN_DX_0,  FN_DX_1  },
    { FN_DY_0,  FN_DY_1  }
};


template <typename TYPE>
Function2D<TYPE>::Function2D() :
    elem(nullptr),
    order(0),
    cur_node(nullptr),
    cur_quad(0)
{
    memset(this->quads, 0, sizeof(this->quads));
}

template <typename TYPE>
Function2D<TYPE>::~Function2D()
{
}


template <typename TYPE>
typename Function2D<TYPE>::Node *
Function2D<TYPE>::new_node(int mask, int num_points)
{
    // get the number of tables
    int nt = 0, m = mask;
    if (num_components < 3)
        m &= FN_VAL_0 | FN_DX_0 | FN_DY_0;
    while (m) {
        nt += m & 1;
        m >>= 1;
    }

    // allocate a node including its data part, init table pointers
    int size = sizeof(Node) + sizeof(TYPE) * num_points * nt;
    Node *node = (Node *) malloc(size);
    node->mask = mask;
    node->size = size;
    memset(node->values, 0, sizeof(node->values));
    TYPE *data = node->data;
    for (int j = 0; j < this->num_components; j++) {
        for (int i = 0; i < VALUE_TYPES; i++)
            if (mask & this->idx2mask[i][j]) {
                node->values[j][i] = data;
                data += num_points;
            }
    }

    return node;
}

typedef Function2D<double> RealFunction2D;

} // namespace fe
