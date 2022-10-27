#pragma once

#include "boundingbox.h"
#include "vtkVector.h"

class vtkDataObject;
class vtkUnstructuredGrid;
class vtkAlgorithmOutput;
class vtkPolyData;
class vtkPolyDataAlgorithm;
class vtkMapper;
class vtkActor;
class vtkProperty;
class vtkVector3d;

class MeshObject {
public:
    MeshObject(vtkAlgorithmOutput * alg_output);
    virtual ~MeshObject();

    void init();
    bool visible();
    void update();
    vtkUnstructuredGrid * getGrid();
    vtkPolyData * getPolyData();
    vtkMapper * getMapper();
    vtkActor * getActor();
    vtkProperty * getProperty();
    const BoundingBox & getBounds() const;
    vtkVector3d getCenterOfBounds() const;
    int getNumCells() const;
    int getNumPoints() const;

    void setVisible(bool visible);
    void setPosition(double x, double y, double z);

protected:
    vtkVector3d computeCenterOfBounds();

    vtkAlgorithmOutput * alg_output;
    vtkDataObject * data_object;
    vtkUnstructuredGrid * grid;
    vtkPolyDataAlgorithm * geometry;
    vtkMapper * mapper;
    vtkActor * actor;
    BoundingBox bounds;
    vtkVector3d center_of_bounds;
};
