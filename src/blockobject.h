// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include "meshobject.h"

class vtkCamera;
class vtkExtractBlock;
class vtkCompositeDataGeometryFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkPolyDataSilhouette;
class vtkCellData;
class vtkUnstructuredGrid;

class BlockObject : public MeshObject {
public:
    BlockObject(vtkAlgorithmOutput * alg_output, vtkCamera * camera);
    ~BlockObject() override;
    void modified() override;
    void update() override;

    vtkActor * getSilhouetteActor();
    vtkProperty * getSilhouetteProperty();
    const QColor & getColor();
    double getOpacity() const;
    void setColor(const QColor & clr);
    void setOpacity(double value);
    void setSilhouetteVisible(bool visible);
    vtkCellData * getCellData() const;
    vtkUnstructuredGrid * getUnstructuredGrid() const;

protected:
    void setUpCellQuality(vtkUnstructuredGrid * unstr_grid);
    void setUpSilhouette(vtkCamera * camera);

    vtkUnstructuredGrid * grid;
    vtkSmartPointer<vtkPolyDataSilhouette> silhouette;
    vtkSmartPointer<vtkPolyDataMapper> silhouette_mapper;
    vtkSmartPointer<vtkActor> silhouette_actor;
    QColor color;
    double opacity;
};
