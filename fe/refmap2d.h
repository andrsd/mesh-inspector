#pragma once

#include "common.h"
#include "shapefunction2d.h"
#include "vertex.h"

namespace fe {

class Element;
class QuadPt2D;

class RefMap2D {
public:
    RefMap2D();

    void set_element(Element *e);
    double2x2 * get_ref_map(const int np, const QuadPt2D * pt);
    double * get_jacobian(const int np, const QuadPt2D * pt, bool trans);

protected:
    /// Shape function
    ShapeFunction2D * pss;
    /// Number of coefficients and shape functions
    int n_coefs;
    /// Shape function indices
    /// 4 is the max number of vertices for 2D elements
    int indices[4];
    /// Coefficients
    Vertex2D * coefs;
    /// Element vertices
    Vertex2D vertex[4];
};

}
