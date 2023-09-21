#pragma once

#include "meshobject.h"

class SideSetObject : public MeshObject {
public:
    explicit SideSetObject(vtkAlgorithmOutput * alg_output);
};
