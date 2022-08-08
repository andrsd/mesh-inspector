#pragma once

#include "meshobject.h"

class vtkExtractBlock;

class SideSetObject : public MeshObject {
public:
    SideSetObject(vtkExtractBlock * eb);
    virtual ~SideSetObject();
};
