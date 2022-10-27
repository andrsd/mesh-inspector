#pragma once

#include "common.h"
#include "shapefunction1d.h"
#include "vertex.h"

namespace fe {

class Element;
class QuadPt1D;

class RefMap1D {
public:
    RefMap1D();

    void set_element(Element *e);
    double1x1 * get_ref_map(const int np, const QuadPt1D * pt);
    double * get_jacobian(const int np, const QuadPt1D * pt, bool trans);

protected:
    /// Shape function
    ShapeFunction1D * pss;
    /// Number of coefficients and shape functions
    int n_coefs;
    /// Shape function indices
    /// 4 is the max number of vertices for 2D elements
    int indices[4];
    /// Coefficients
    Vertex1D * coefs;
    /// Element vertices
    Vertex1D vertex[4];
};

}
