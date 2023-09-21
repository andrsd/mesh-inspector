#pragma once

#include "meshobject.h"

class NodeSetObject : public MeshObject {
public:
    explicit NodeSetObject(vtkAlgorithmOutput * alg_output);
};
