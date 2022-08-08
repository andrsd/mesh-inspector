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
