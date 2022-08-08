#pragma once

#include <QColor>

class vtkCamera;
class vtkExtractBlock;
class vtkCompositeDataGeometryFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkPolyDataSilhouette;

class BlockObject {
public:
    BlockObject(vtkExtractBlock * eb, vtkCamera * camera);
    virtual ~BlockObject();

    bool visible();
    vtkActor * getActor();
    vtkProperty * getProperty();
    vtkActor * getSilhouetteActor();
    vtkProperty * getSilhouetteProperty();
    const QColor & getColor();
    void setColor(const QColor & color);
    void setVisible(bool visible);
    void setSilhouetteVisible(bool visible);

protected:
    void setUpSilhouette(vtkCamera * camera);

    vtkCompositeDataGeometryFilter * geometry;
    vtkPolyDataMapper * mapper;
    vtkActor * actor;
    vtkPolyDataSilhouette * silhouette;
    vtkPolyDataMapper * silhouette_mapper;
    vtkActor * silhouette_actor;
    QColor color;
};
