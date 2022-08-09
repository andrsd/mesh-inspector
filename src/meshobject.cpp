#include "meshobject.h"
#include "vtkExtractBlock.h"
#include "vtkCompositeDataGeometryFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"

MeshObject::MeshObject(vtkExtractBlock * eb)
{
    this->geometry = vtkCompositeDataGeometryFilter::New();
    this->geometry->SetInputConnection(0, eb->GetOutputPort(0));
    this->geometry->Update();

    this->mapper = vtkPolyDataMapper::New();
    this->mapper->SetInputConnection(this->geometry->GetOutputPort());
    this->mapper->SetScalarModeToUsePointFieldData();
    this->mapper->InterpolateScalarsBeforeMappingOn();

    this->actor = vtkActor::New();
    this->actor->SetMapper(this->mapper);
    this->actor->VisibilityOff();

    double * bnds = this->actor->GetBounds();
    this->bounds = BoundingBox(bnds[0], bnds[1], bnds[2], bnds[3], bnds[4], bnds[5]);
    this->center_of_bounds = this->bounds.center();
}

MeshObject::~MeshObject() {}

bool
MeshObject::visible()
{
    return this->actor->GetVisibility();
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
MeshObject::setVisible(bool visible)
{
    if (visible)
        this->actor->VisibilityOn();
    else
        this->actor->VisibilityOff();
}

void
MeshObject::setPosition(double x, double y, double z)
{
    this->actor->SetPosition(x, y, z);
}

const BoundingBox &
MeshObject::getBounds() const
{
    return this->bounds;
}

vtkVector3d
MeshObject::getCenterOfBounds() const
{
    return this->center_of_bounds;
}
