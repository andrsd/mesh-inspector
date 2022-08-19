#pragma once

#include "meshobject.h"

class NodeSetObject : public MeshObject {
public:
    NodeSetObject(vtkAlgorithmOutput * alg_output);
    virtual ~NodeSetObject();
};
