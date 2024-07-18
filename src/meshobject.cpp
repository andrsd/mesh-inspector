// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "meshobject.h"
#include "vtkDataObject.h"
#include "vtkAlgorithmOutput.h"
#include "vtkCompositeDataGeometryFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyDataPlaneClipper.h"
#include "vtkPlane.h"
#include "vtkTriangleFilter.h"

MeshObject::MeshObject(vtkAlgorithmOutput * alg_output) :
    clipping(false),
    clipper(vtkPolyDataPlaneClipper::New()),
    clip_plane(vtkPlane::New()),
    clipped_actor(vtkActor::New())
{
    auto * algoritm = alg_output->GetProducer();
    this->data_object = algoritm->GetOutputDataObject(0);

    if (dynamic_cast<vtkMultiBlockDataSet *>(this->data_object)) {
        this->geometry = vtkCompositeDataGeometryFilter::New();
        this->mapper = vtkPolyDataMapper::New();
        this->clipped_away_mapper = vtkPolyDataMapper::New();
    }
    else {
        this->geometry = vtkGeometryFilter::New();
        this->mapper = vtkDataSetMapper::New();
        this->clipped_away_mapper = vtkDataSetMapper::New();
    }

    this->geometry->SetInputConnection(0, alg_output);
    this->geometry->Update();

    this->mapper->SetInputConnection(this->geometry->GetOutputPort());
    this->mapper->SetScalarModeToUsePointFieldData();
    this->mapper->InterpolateScalarsBeforeMappingOn();

    this->actor = vtkActor::New();
    this->actor->SetMapper(this->mapper);
    this->actor->VisibilityOff();

    this->bounding_box = vtkBoundingBox(this->actor->GetBounds());
    double center[3];
    this->bounding_box.GetCenter(center);
    this->center_of_bounds = vtkVector3d(center[0], center[1], center[2]);
}

MeshObject::~MeshObject()
{
    this->geometry->Delete();
    this->mapper->Delete();
    this->actor->Delete();
}

bool
MeshObject::visible()
{
    return this->actor->GetVisibility();
}

vtkMapper *
MeshObject::getMapper() const
{
    return this->mapper;
}

vtkActor *
MeshObject::getActor()
{
    return this->actor;
}

vtkProperty *
MeshObject::getProperty()
{
    return this->actor->GetProperty();
}

void
MeshObject::modified()
{
    this->data_object->Modified();
    this->geometry->Modified();
    this->mapper->Modified();
    if (this->clipping)
        this->clipped_away_mapper->Modified();
}

void
MeshObject::update()
{
    this->geometry->Update();
    this->mapper->Update();
    if (this->clipping)
        this->clipped_away_mapper->Update();
}

void
MeshObject::setVisible(bool visible)
{
    if (visible) {
        this->actor->VisibilityOn();
        this->clipped_actor->VisibilityOn();
    }
    else {
        this->actor->VisibilityOff();
        this->clipped_actor->VisibilityOff();
    }
}

void
MeshObject::setPosition(double x, double y, double z)
{
    this->actor->SetPosition(x, y, z);
}

double *
MeshObject::getBounds() const
{
    return this->actor->GetBounds();
}

vtkVector3d
MeshObject::getCenterOfBounds() const
{
    return this->center_of_bounds;
}

int
MeshObject::getNumCells() const
{
    return this->data_object->GetNumberOfElements(vtkDataSet::CELL);
}

int
MeshObject::getNumPoints() const
{
    return this->data_object->GetNumberOfElements(vtkDataSet::POINT);
}

void
MeshObject::setClip(bool state)
{
    if (state) {
        this->clipping = true;

        this->clipper->SetInputConnection(this->geometry->GetOutputPort());
        this->clipper->SetPlane(this->clip_plane);
        this->clipper->SetCapping(true);
        this->clipper->SetClippingLoops(false);
        this->clipper->Update();

        this->mapper->SetInputConnection(this->clipper->GetOutputPort(0));
        this->mapper->Update();

        this->clipped_away_mapper->SetInputConnection(this->clipper->GetOutputPort(1));
        this->clipped_away_mapper->SetScalarModeToUsePointFieldData();
        this->clipped_away_mapper->InterpolateScalarsBeforeMappingOn();
        this->clipped_away_mapper->Update();

        this->clipped_actor->SetMapper(this->clipped_away_mapper);
        this->clipped_actor->SetVisibility(this->actor->GetVisibility());

        this->clipped_actor->SetProperty(this->actor->GetProperty());
    }
    else {
        this->clipping = false;

        this->mapper->SetInputConnection(this->geometry->GetOutputPort());
        this->mapper->SetScalarModeToUsePointFieldData();
        this->mapper->InterpolateScalarsBeforeMappingOn();
        this->mapper->Update();

        this->clipped_away_mapper->RemoveAllInputs();
        this->clipped_actor->SetMapper(nullptr);
    }
}

void
MeshObject::setClipPlane(vtkPlane * plane)
{
    auto normal = plane->GetNormal();
    this->clip_plane->SetNormal(normal);
    auto origin = plane->GetOrigin();
    this->clip_plane->SetOrigin(origin);
}

vtkActor *
MeshObject::getClippedActor()
{
    return this->clipped_actor;
}

vtkProperty *
MeshObject::getClippedProperty()
{
    return this->clipped_actor->GetProperty();
}
