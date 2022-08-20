#pragma once

#include "ointeractorinterface.h"
#include "vtkInteractorStyleImage.h"

class OInteractorStyle3D : public vtkInteractorStyleTrackballCamera, public OInteractorInterface {
public:
    OInteractorStyle3D(MainWindow * widget);

protected:
    virtual void onLeftButtonPress(vtkObject * object, unsigned long event, void * ctx);
    virtual void onLeftButtonRelease(vtkObject * object, unsigned long event, void * ctx);
    virtual void onMouseMove(vtkObject * object, unsigned long event, void * ctx);
};
