#pragma once

#include "meshobject.h"

class SideSetObject : public MeshObject {
public:
    SideSetObject(vtkAlgorithmOutput * alg_output);
    virtual ~SideSetObject();
};
