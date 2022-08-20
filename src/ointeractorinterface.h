#pragma once

#include <QPoint>

class MainWindow;
class vtkObject;

class OInteractorInterface {
public:
    OInteractorInterface(MainWindow * widget);

protected:
    virtual void onLeftButtonPress(vtkObject * object, unsigned long event, void * ctx);
    virtual void onLeftButtonRelease(vtkObject * object, unsigned long event, void * ctx);
    virtual void onKeyPress(vtkObject * object, unsigned long event, void * ctx);
    virtual void onKeyRelease(vtkObject * object, unsigned long event, void * ctx);
    virtual void onChar(vtkObject * object, unsigned long event, void * ctx);
    virtual void onMouseMove(vtkObject * object, unsigned long event, void * ctx);

    MainWindow * widget;
    QPoint last_mouse_pos;
    bool left_button_down;
};
