// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include "vtkSmartPointer.h"

class MainWindow;
class Model;
class View;
class MeshQualityWidget;
class vtkLookupTable;
class vtkScalarBarActor;
class ColorProfile;
class BlockObject;

class MeshQualityTool : public QObject {
public:
    explicit MeshQualityTool(MainWindow * main_wnd);
    ~MeshQualityTool() override;

    void setupWidgets();
    void setupVtk();
    void updateLocation();
    bool isVisible() const;
    void done();
    void update();

public slots:
    void onMeshQuality();
    void onMetricChanged(int metric_id);
    void onClose();
    void onColorProfileChanged(ColorProfile * profile);

protected:
    void setupLookupTable();
    void setupColorBar();
    void getCellQualityRange(double range[]);
    void setBlockMeshQualityProperties(std::shared_ptr<BlockObject> block, double range[]);

    MainWindow * main_window;
    Model *& model;
    View *& view;
    MeshQualityWidget * mesh_quality;
    vtkSmartPointer<vtkLookupTable> lut;
    vtkSmartPointer<vtkScalarBarActor> color_bar;

public:
    static const char * MESH_QUALITY_FIELD_NAME;
};
