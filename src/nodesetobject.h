#pragma once

#include "meshobject.h"

class vtkExtractBlock;

class NodeSetObject : public MeshObject {
public:
    NodeSetObject(vtkExtractBlock * eb);
    virtual ~NodeSetObject();
};
