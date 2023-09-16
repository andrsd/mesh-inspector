#pragma once

#include <QPoint>

class MainWindow;
class QKeySequence;
class vtkObject;
class vtkRenderWindowInteractor;

/// Common functionality for sending events into the Qt widgets
class OInteractorInterface {
public:
    OInteractorInterface(MainWindow * widget);

protected:
    void onLeftButtonPress(const QPoint & pos);
    void onLeftButtonRelease(const QPoint & pos);
    void onMouseMove(const QPoint & pos);

    void onKeyPress(const QKeySequence & seq, const Qt::KeyboardModifiers & mods);
    void onKeyRelease();
    void onChar();

    Qt::KeyboardModifiers getKeyboardModifiers(vtkRenderWindowInteractor * interactor);

private:
    MainWindow * widget;
    QPoint last_mouse_pos;
};
