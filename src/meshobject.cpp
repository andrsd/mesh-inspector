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
#include "vtkPlaneCutter.h"
#include "vtkPlane.h"

MeshObject::MeshObject(vtkAlgorithmOutput * alg_output) :
    clipping(false),
    clipper(vtkSmartPointer<vtkPolyDataPlaneClipper>::New()),
    clip_plane(vtkSmartPointer<vtkPlane>::New()),
    clipped_actor(vtkSmartPointer<vtkActor>::New())
{
    auto * algoritm = alg_output->GetProducer();
    this->data_object = algoritm->GetOutputDataObject(0);

    if (dynamic_cast<vtkMultiBlockDataSet *>(this->data_object)) {
        this->geometry = vtkSmartPointer<vtkCompositeDataGeometryFilter>::New();
        this->mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        this->clipped_away_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        this->cut_away_geometry = vtkSmartPointer<vtkCompositeDataGeometryFilter>::New();
    }
    else {
        this->geometry = vtkSmartPointer<vtkGeometryFilter>::New();
        this->mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        this->clipped_away_mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        this->cut_away_geometry = vtkSmartPointer<vtkGeometryFilter>::New();
    }

    this->geometry->SetInputConnection(0, alg_output);
    this->geometry->Update();

    this->mapper->SetInputConnection(this->geometry->GetOutputPort());
    this->mapper->SetScalarModeToUsePointFieldData();
    this->mapper->InterpolateScalarsBeforeMappingOn();

    this->actor = vtkSmartPointer<vtkActor>::New();
    this->actor->SetMapper(this->mapper);
    this->actor->VisibilityOff();

    this->bounding_box = vtkBoundingBox(this->actor->GetBounds());
    double center[3];
    this->bounding_box.GetCenter(center);
    this->center_of_bounds = vtkVector3d(center[0], center[1], center[2]);

    this->cutter = vtkSmartPointer<vtkPlaneCutter>::New();
    this->cutter->SetInputData(this->data_object);
}

MeshObject::~MeshObject() {}

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
        this->clipper->SetCapping(false);
        this->clipper->SetClippingLoops(false);
        this->clipper->Update();

        this->mapper->SetInputConnection(this->clipper->GetOutputPort());
        this->mapper->Update();

        // set up surface from the cut
        this->cutter->SetPlane(this->clip_plane);
        this->cutter->Update();

        this->cut_away_geometry->SetInputConnection(this->cutter->GetOutputPort());
        this->clipped_away_mapper->SetInputConnection(this->cut_away_geometry->GetOutputPort());

        this->clipped_actor->SetMapper(this->clipped_away_mapper);
        this->clipped_actor->SetVisibility(this->actor->GetVisibility());

        // copy some actor properties into the clipped-away actor
        auto prop = this->actor->GetProperty();
        auto clip_prop = this->clipped_actor->GetProperty();
        clip_prop->SetAmbient(prop->GetAmbient());
        clip_prop->SetDiffuse(prop->GetDiffuse());
        clip_prop->SetColor(prop->GetColor());
        clip_prop->SetSpecular(prop->GetSpecular());
        clip_prop->SetOpacity(prop->GetOpacity());
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
