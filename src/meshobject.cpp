#include "meshobject.h"
#include "vtkDataObject.h"
#include "vtkUnstructuredGrid.h"
#include "vtkAlgorithmOutput.h"
#include "vtkCompositeDataGeometryFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkDoubleArray.h"
#include "vtkCellArrayIterator.h"
#include "vtkCellData.h"
#include <iostream>

MeshObject::MeshObject(vtkAlgorithmOutput * alg_output) : alg_output(alg_output)
{
    auto * algorithm = alg_output->GetProducer();
    this->data_object = algorithm->GetOutputDataObject(0);
    if (dynamic_cast<vtkMultiBlockDataSet *>(this->data_object)) {
        auto * multi_blk = dynamic_cast<vtkMultiBlockDataSet *>(this->data_object);
        this->grid = dynamic_cast<vtkUnstructuredGrid *>(multi_blk->GetBlock(0));
        this->geometry = vtkCompositeDataGeometryFilter::New();
        this->mapper = vtkPolyDataMapper::New();
    }
    else {
        this->grid = dynamic_cast<vtkUnstructuredGrid *>(this->data_object);
        this->geometry = vtkGeometryFilter::New();
        this->mapper = vtkDataSetMapper::New();
    }
    this->actor = vtkActor::New();
}

MeshObject::~MeshObject()
{
    this->geometry->Delete();
    this->mapper->Delete();
    this->actor->Delete();
}

void
MeshObject::init()
{
    this->geometry->SetInputConnection(0, this->alg_output);
    this->geometry->Update();

    this->mapper->SetInputConnection(this->geometry->GetOutputPort());
    this->mapper->SetScalarModeToUsePointFieldData();
    this->mapper->InterpolateScalarsBeforeMappingOn();

    this->actor->SetMapper(this->mapper);
    this->actor->VisibilityOff();

    double * bnds = this->actor->GetBounds();
    this->bounds = BoundingBox(bnds[0], bnds[1], bnds[2], bnds[3], bnds[4], bnds[5]);
    this->center_of_bounds = this->bounds.center();
}

bool
MeshObject::visible()
{
    return this->actor->GetVisibility();
}

void
MeshObject::update()
{
    this->alg_output->Modified();
//    this->actor->Modified();
//    this->geometry->Update();
//    this->mapper->Update();
//    this->geometry->Modified();
}

vtkUnstructuredGrid *
MeshObject::getGrid()
{
    return this->grid;
}

vtkPolyData *
MeshObject::getPolyData()
{
    return this->geometry->GetOutput();
}

vtkMapper *
MeshObject::getMapper()
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
