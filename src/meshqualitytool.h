#pragma once

#include <QObject>

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
    void setColorProfile(ColorProfile * profile);
    void update();

public slots:
    void onMeshQuality();
    void onMetricChanged(int metric_id);
    void onClose();

protected:
    void setupLookupTable();
    void setupColorBar();
    void getCellQualityRange(double range[]);
    void setBlockMeshQualityProperties(BlockObject * block, double range[]);

    MainWindow * main_window;
    Model * & model;
    View * & view;
    MeshQualityWidget * mesh_quality;
    vtkLookupTable * lut;
    vtkScalarBarActor * color_bar;

public:
    static const char * MESH_QUALITY_FIELD_NAME;
};
