#include "ointeractorstyle3d.h"
#include "vtkCommand.h"

OInteractorStyle3D::OInteractorStyle3D(MainWindow * widget) :
    vtkInteractorStyleTrackballCamera(),
    OInteractorInterface(widget)
{
    AddObserver(vtkCommand::LeftButtonPressEvent, this, &OInteractorStyle3D::onLeftButtonPress);
    AddObserver(vtkCommand::LeftButtonReleaseEvent, this, &OInteractorStyle3D::onLeftButtonRelease);
    AddObserver(vtkCommand::KeyPressEvent, this, &OInteractorStyle3D::onKeyPress);
    AddObserver(vtkCommand::KeyReleaseEvent, this, &OInteractorStyle3D::onKeyRelease);
    AddObserver(vtkCommand::CharEvent, this, &OInteractorStyle3D::onChar);
}

void
OInteractorStyle3D::onLeftButtonPress(vtkObject * object, unsigned long event, void * ctx)
{
    OInteractorInterface::onLeftButtonPress(object, event, ctx);
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void
OInteractorStyle3D::onLeftButtonRelease(vtkObject * object, unsigned long event, void * ctx)
{
    OInteractorInterface::onLeftButtonRelease(object, event, ctx);
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}
