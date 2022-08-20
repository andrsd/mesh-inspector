#include "ointeractorstyle2d.h"
#include "vtkCommand.h"

OInteractorStyle2D::OInteractorStyle2D(MainWindow * widget) :
    vtkInteractorStyleImage(),
    OInteractorInterface(widget)
{
    AddObserver(vtkCommand::LeftButtonPressEvent, this, &OInteractorStyle2D::onLeftButtonPress);
    AddObserver(vtkCommand::LeftButtonReleaseEvent, this, &OInteractorStyle2D::onLeftButtonRelease);
    AddObserver(vtkCommand::KeyPressEvent, this, &OInteractorStyle2D::onKeyPress);
    AddObserver(vtkCommand::KeyReleaseEvent, this, &OInteractorStyle2D::onKeyRelease);
    AddObserver(vtkCommand::CharEvent, this, &OInteractorStyle2D::onChar);
    AddObserver(vtkCommand::MouseMoveEvent, this, &OInteractorStyle2D::onMouseMove);
}

void
OInteractorStyle2D::onLeftButtonPress(vtkObject * object, unsigned long event, void * ctx)
{
    OInteractorInterface::onLeftButtonPress(object, event, ctx);
    vtkInteractorStyleImage::OnLeftButtonDown();
}

void
OInteractorStyle2D::onLeftButtonRelease(vtkObject * object, unsigned long event, void * ctx)
{
    OInteractorInterface::onLeftButtonRelease(object, event, ctx);
    vtkInteractorStyleImage::OnLeftButtonUp();
}

void
OInteractorStyle2D::onMouseMove(vtkObject * object, unsigned long event, void * ctx)
{
    OInteractorInterface::onMouseMove(object, event, ctx);
    vtkInteractorStyleImage::OnMouseMove();
}
