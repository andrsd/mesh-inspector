#pragma once

#include "vtkVector.h"
#include "vtkBoundingBox.h"

class vtkDataObject;
class vtkAlgorithmOutput;
class vtkPolyDataAlgorithm;
class vtkMapper;
class vtkActor;
class vtkProperty;
class vtkVector3d;

class MeshObject {
public:
    explicit MeshObject(vtkAlgorithmOutput * alg_output);
    virtual ~MeshObject();

    bool visible();
    vtkMapper * getMapper() const;
    vtkActor * getActor();
    vtkProperty * getProperty();
    double * getBounds() const;
    vtkVector3d getCenterOfBounds() const;
    int getNumCells() const;
    int getNumPoints() const;

    virtual void modified();
    virtual void update();
    void setVisible(bool visible);
    void setPosition(double x, double y, double z);

protected:
    vtkVector3d computeCenterOfBounds();

    vtkDataObject * data_object;
    vtkPolyDataAlgorithm * geometry;
    vtkMapper * mapper;
    vtkActor * actor;
    vtkBoundingBox bounding_box;
    vtkVector3d center_of_bounds;
};
