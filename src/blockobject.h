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

class BlockObject : public MeshObject {
public:
    BlockObject(vtkAlgorithmOutput * alg_output, vtkCamera * camera);
    virtual ~BlockObject();

    vtkActor * getSilhouetteActor();
    vtkProperty * getSilhouetteProperty();
    const QColor & getColor();
    double getOpacity();
    void setColor(const QColor & color);
    void setOpacity(double opacity);
    void setSilhouetteVisible(bool visible);

protected:
    void setUpSilhouette(vtkCamera * camera);

    vtkPolyDataSilhouette * silhouette;
    vtkPolyDataMapper * silhouette_mapper;
    vtkActor * silhouette_actor;
    QColor color;
    double opacity;
};
