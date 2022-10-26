#pragma once

#include "common.h"
#include "shapefunction3d.h"
#include "vertex.h"

namespace fe {

class Element;
class QuadPt3D;

class RefMap3D {
public:
    RefMap3D();

    void set_element(Element * e);
    double3x3 * get_ref_map(const int np, const QuadPt3D * pt);
    double * get_jacobian(const int np, const QuadPt3D * pt, bool trans);

protected:
    /// Shape function
    ShapeFunction3D * pss;
    /// Number of coefficients and shape functions
    int n_coefs;
    /// Shape function indices
    /// 8 is the max number of vertices for 3D elements
    int indices[8];
    /// Coefficients
    Vertex3D * coefs;
    /// Element vertices
    Vertex3D vertex[8];
};

} // namespace fe
