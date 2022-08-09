#include "blockobject.h"
#include "vtkDataObject.h"
#include "vtkCompositeDataGeometryFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyDataSilhouette.h"
#include "vtkExtractBlock.h"

BlockObject::BlockObject(vtkExtractBlock * eb, vtkCamera * camera) :
    MeshObject(eb),
    silhouette(nullptr),
    silhouette_mapper(nullptr),
    silhouette_actor(nullptr)
{
    vtkDataObject * data_object = eb->GetOutput();

    this->actor->SetScale(0.99999);
    this->actor->VisibilityOn();

    this->color = QColor::fromRgbF(1., 1., 1.);

    this->setUpSilhouette(camera);
}

BlockObject::~BlockObject() {}

void
BlockObject::setUpSilhouette(vtkCamera * camera)
{
    this->silhouette = vtkPolyDataSilhouette::New();
    this->silhouette->SetInputData(this->mapper->GetInput());
    this->silhouette->SetCamera(camera);

    this->silhouette_mapper = vtkPolyDataMapper::New();
    this->silhouette_mapper->SetInputConnection(this->silhouette->GetOutputPort());

    this->silhouette_actor = vtkActor::New();
    this->silhouette_actor->SetMapper(this->silhouette_mapper);
    this->silhouette_actor->VisibilityOff();

    auto * property = this->silhouette_actor->GetProperty();
    property->SetColor(0, 0, 0);
    property->SetLineWidth(3);
}

vtkActor *
BlockObject::getSilhouetteActor()
{
    return this->silhouette_actor;
}

vtkProperty *
BlockObject::getSilhouetteProperty()
{
    return this->silhouette_actor->GetProperty();
}

const QColor &
BlockObject::getColor()
{
    return this->color;
}

void
BlockObject::setColor(const QColor & color)
{
    this->color = color;
}

void
BlockObject::setSilhouetteVisible(bool visible)
{
    if (visible)
        this->silhouette_actor->VisibilityOn();
    else
        this->silhouette_actor->VisibilityOff();
}