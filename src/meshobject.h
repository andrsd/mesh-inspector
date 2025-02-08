// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "vtkVector.h"
#include "vtkBoundingBox.h"
#include "vtkSmartPointer.h"

class vtkDataObject;
class vtkAlgorithmOutput;
class vtkPolyDataAlgorithm;
class vtkMapper;
class vtkActor;
class vtkProperty;
class vtkVector3d;
class vtkPlane;
class vtkPolyDataPlaneClipper;
class vtkPlaneCutter;

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

    void setClip(bool state);
    void setClipPlane(vtkPlane * plane);
    vtkActor * getClippedActor();
    vtkProperty * getClippedProperty();

protected:
    vtkVector3d computeCenterOfBounds();

    vtkDataObject * data_object;
    vtkSmartPointer<vtkPolyDataAlgorithm> geometry;
    vtkSmartPointer<vtkMapper> mapper;
    vtkSmartPointer<vtkActor> actor;
    vtkBoundingBox bounding_box;
    vtkVector3d center_of_bounds;

    bool clipping;
    vtkSmartPointer<vtkPolyDataPlaneClipper> clipper;
    vtkSmartPointer<vtkPlane> clip_plane;
    vtkSmartPointer<vtkMapper> clipped_away_mapper;
    vtkSmartPointer<vtkActor> clipped_actor;
    vtkSmartPointer<vtkPlaneCutter> cutter;
    vtkSmartPointer<vtkPolyDataAlgorithm> cut_away_geometry;
};
