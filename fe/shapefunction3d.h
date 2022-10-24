#pragma once

#include "function3d.h"
#include "shapeset3d.h"

namespace fe {

class Element;

/// Represents a shape function on a ref. domain
///
class ShapeFunction3D : public RealFunction3D {
public:
    /// Constructs a standard precalculated shapeset class.
    /// @param shapeset [in] Pointer to the shapeset to be precalculated.
    ShapeFunction3D(Shapeset3D * shapeset);
    ShapeFunction3D();

    virtual ~ShapeFunction3D();

    void free();

    /// Ensures subsequent calls to get_active_element() will be returning 'e'.
    /// Switches the class to the appropriate mode (triangle, quad).
    virtual void set_active_element(Element * e);

    /// Activates a shape function given by its index. The values of the shape function
    /// can then be obtained by setting the required integration rule order by calling
    /// set_quad_order() and after that calling get_values(), get_dx_values(), etc.
    /// @param index [in] Shape index.
    void set_active_shape(int index);

    /// @return Index of the active shape (can be negative if the shape is constrained).
    int
    get_active_shape() const
    {
        return index;
    };

    /// @return Pointer to the shapeset which is being precalculated.
    Shapeset3D *
    get_shapeset() const
    {
        return shapeset;
    }

    void set_shapeset(Shapeset3D * ss);

    virtual void precalculate(const int np, const QuadPt3D * pt, int mask);

protected:
    ///
    Shapeset3D * shapeset;
    /// index of active shape function
    int index;
};

} // namespace fe
