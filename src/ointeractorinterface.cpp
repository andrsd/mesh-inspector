#include "ointeractorinterface.h"
#include "mainwindow.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include <QPoint>
#include <QCoreApplication>
#include <QKeyEvent>

OInteractorInterface::OInteractorInterface(MainWindow * widget) :
    widget(widget),
    last_mouse_pos(QPoint(-1, -1))
{
}

void
OInteractorInterface::onLeftButtonPress(const QPoint & pos)
{
    this->last_mouse_pos = pos;
}

void
OInteractorInterface::onLeftButtonRelease(const QPoint & pos)
{
    if (this->last_mouse_pos == pos)
        this->widget->onClicked(pos);
}

void
OInteractorInterface::onKeyPress(const QKeySequence & seq, const Qt::KeyboardModifiers & mods)
{
    if (seq.count() > 0) {
        auto * e = new QKeyEvent(QEvent::KeyPress, seq[0].toCombined(), mods);
        QCoreApplication::postEvent(this->widget, e);
    }
}

void
OInteractorInterface::onKeyRelease()
{
}

void
OInteractorInterface::onChar()
{
}

void
OInteractorInterface::onMouseMove(const QPoint & pos)
{
    this->widget->onMouseMove(pos);
}

Qt::KeyboardModifiers
OInteractorInterface::getKeyboardModifiers(vtkRenderWindowInteractor * interactor)
{
    Qt::KeyboardModifiers mods = Qt::NoModifier;
#if __APPLE__
    if (interactor->GetControlKey())
        mods |= Qt::MetaModifier;
#else
    if (interactor->GetControlKey())
        mods |= Qt::ControlModifier;
#endif
    if (interactor->GetShiftKey())
        mods |= Qt::ShiftModifier;
    if (interactor->GetAltKey())
        mods |= Qt::AltModifier;

    return mods;
}
