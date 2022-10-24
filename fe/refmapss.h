#pragma once

#include "shapeset1d.h"
#include "shapeset2d.h"
#include "shapeset3d.h"

namespace fe {

/// Special shapeset used in RefMap1D for edge
///
class RefMapShapesetEdge : public Shapeset1D {
public:
    RefMapShapesetEdge();
};

/// Special shapeset used in RefMap2D for triangles
///
class RefMapShapesetTri : public Shapeset2D {
public:
    RefMapShapesetTri();
};

/// Special shapeset used in RefMap2D for quadrilateral
///
class RefMapShapesetQuad : public Shapeset2D {
public:
    RefMapShapesetQuad();
};

/// Special shapeset used in RefMap3D for tetrahedra
///
class RefMapShapesetTetra : public Shapeset3D {
public:
    RefMapShapesetTetra();
};

/// Special shapeset used in RefMap3D for hexahedra
///
class RefMapShapesetHex : public Shapeset3D {
public:
    RefMapShapesetHex();
};

}
