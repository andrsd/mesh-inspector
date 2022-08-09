#pragma once

#include "BoundingBox.h"
#include "vtkVector.h"

class vtkExtractBlock;
class vtkCompositeDataGeometryFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkVector3d;

class MeshObject {
public:
    MeshObject(vtkExtractBlock * eb);
    virtual ~MeshObject();

    bool visible();
    vtkActor * getActor();
    vtkProperty * getProperty();
    vtkVector3d getCenterOfBounds();

    void setVisible(bool visible);

protected:
    vtkVector3d computeCenterOfBounds();

    vtkCompositeDataGeometryFilter * geometry;
    vtkPolyDataMapper * mapper;
    vtkActor * actor;
    BoundingBox bounds;
    vtkVector3d center_of_bounds;
};
