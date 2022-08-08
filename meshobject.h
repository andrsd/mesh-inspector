#pragma once

class vtkExtractBlock;
class vtkCompositeDataGeometryFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;

class MeshObject {
public:
    MeshObject(vtkExtractBlock * eb);
    virtual ~MeshObject();

    bool visible();
    vtkActor * getActor();
    vtkProperty * getProperty();

    void setVisible(bool visible);

protected:
    vtkCompositeDataGeometryFilter * geometry;
    vtkPolyDataMapper * mapper;
    vtkActor * actor;
};
