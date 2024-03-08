// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "blockobject.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyDataSilhouette.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDoubleArray.h"
#include "vtkAlgorithmOutput.h"
#include "mainwindow.h"
#include "meshqualitytool.h"

BlockObject::BlockObject(vtkAlgorithmOutput * alg_output, vtkCamera * camera) :
    MeshObject(alg_output),
    grid(nullptr),
    silhouette(nullptr),
    silhouette_mapper(nullptr),
    silhouette_actor(nullptr),
    opacity(1.)
{
    auto do_class = std::string(this->data_object->GetClassName());
    if (do_class == "vtkMultiBlockDataSet") {
        auto mb_ds = dynamic_cast<vtkMultiBlockDataSet *>(this->data_object);
        auto n_blocks = mb_ds->GetNumberOfBlocks();
        for (unsigned int i = 0; i < n_blocks; i++) {
            auto blk = mb_ds->GetBlock(i);

            auto unstr_grid = dynamic_cast<vtkUnstructuredGrid *>(blk);
            if (unstr_grid)
                setUpCellQuality(unstr_grid);
            else
                std::cerr << "Unknown block type" << std::endl;
        }
        modified();
        update();
    }
    else if (do_class == "vtkUnstructuredGrid") {
        auto unstr_grid = dynamic_cast<vtkUnstructuredGrid *>(this->data_object);
        setUpCellQuality(unstr_grid);
        modified();
        update();
    }

    this->actor->VisibilityOn();

    this->color = QColor::fromRgbF(1., 1., 1.);

    this->setUpSilhouette(camera);
}

BlockObject::~BlockObject()
{
    this->silhouette->Delete();
    this->silhouette_mapper->Delete();
    this->silhouette_actor->Delete();
}

void
BlockObject::setUpCellQuality(vtkUnstructuredGrid * unstr_grid)
{
    this->grid = unstr_grid;
    auto n_cells = unstr_grid->GetNumberOfCells();

    auto cell_quality = vtkDoubleArray::New();
    cell_quality->SetNumberOfTuples(n_cells);
    for (vtkIdType i = 0; i < n_cells; i++)
        cell_quality->SetValue(i, (double) i);
    cell_quality->SetName(MeshQualityTool::MESH_QUALITY_FIELD_NAME);

    auto cell_data = unstr_grid->GetCellData();
    cell_data->AddArray(cell_quality);
}

void
BlockObject::modified()
{
    MeshObject::modified();
    if (this->grid)
        this->grid->Modified();
}

void
BlockObject::update()
{
    MeshObject::update();
}

vtkCellData *
BlockObject::getCellData() const
{
    auto do_class = std::string(this->data_object->GetClassName());
    if (do_class == "vtkMultiBlockDataSet") {
        auto mb_ds = dynamic_cast<vtkMultiBlockDataSet *>(this->data_object);
        auto n_blocks = mb_ds->GetNumberOfBlocks();
        if (n_blocks == 1) {
            auto unstr_grid = dynamic_cast<vtkUnstructuredGrid *>(mb_ds->GetBlock(0));
            return unstr_grid->GetCellData();
        }
        else
            return nullptr;
    }
    else if (do_class == "vtkUnstructuredGrid") {
        auto unstr_grid = dynamic_cast<vtkUnstructuredGrid *>(this->data_object);
        return unstr_grid->GetCellData();
    }
    else
        return nullptr;
}

vtkUnstructuredGrid *
BlockObject::getUnstructuredGrid() const
{
    return this->grid;
}

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
    property->SetLineWidth(1.5);
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

double
BlockObject::getOpacity() const
{
    return this->opacity;
}

void
BlockObject::setColor(const QColor & clr)
{
    this->color = clr;
}

void
BlockObject::setOpacity(double value)
{
    this->opacity = value;
}

void
BlockObject::setSilhouetteVisible(bool visible)
{
    if (visible)
        this->silhouette_actor->VisibilityOn();
    else
        this->silhouette_actor->VisibilityOff();
}
