#include "ointeractorinterface.h"
#include "mainwindow.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include <QPoint>
#include <QCoreApplication>
#include <QKeyEvent>

OInteractorInterface::OInteractorInterface(MainWindow * widget) :
    widget(widget),
    last_mouse_pos(QPoint(-1, -1)),
    left_button_down(false)
{
}

void
OInteractorInterface::onLeftButtonPress(vtkObject * object, unsigned long event, void * ctx)
{
    auto * interactor_style = dynamic_cast<vtkInteractorStyle *>(object);
    if (interactor_style) {
        this->left_button_down = true;
        auto * interactor = interactor_style->GetInteractor();
        auto click_pos = interactor->GetEventPosition();
        auto pt = QPoint(click_pos[0], click_pos[1]);
        this->last_mouse_pos = pt;
    }
}

void
OInteractorInterface::onLeftButtonRelease(vtkObject * object, unsigned long event, void * ctx)
{
    auto * interactor_style = dynamic_cast<vtkInteractorStyle *>(object);
    if (interactor_style) {
        this->left_button_down = false;
        auto * interactor = interactor_style->GetInteractor();
        auto click_pos = interactor->GetEventPosition();
        auto pt = QPoint(click_pos[0], click_pos[1]);
        if (this->last_mouse_pos == pt)
            this->widget->onClicked(pt);
    }
}

void
OInteractorInterface::onKeyPress(vtkObject * object, unsigned long event, void * ctx)
{
    auto * interactor_style = dynamic_cast<vtkInteractorStyle *>(object);
    if (interactor_style) {
        auto * interactor = interactor_style->GetInteractor();
        auto key = interactor->GetKeyCode();
        auto seq = QKeySequence(key);
        // FIXME: get the modifiers from interactor
        auto mods = Qt::NoModifier;
        if (seq.count() > 0) {
            auto * e = new QKeyEvent(QEvent::KeyPress, seq[0].toCombined(), mods);
            QCoreApplication::postEvent(this->widget, e);
        }
    }
}

void
OInteractorInterface::onKeyRelease(vtkObject * object, unsigned long event, void * ctx)
{
}

void
OInteractorInterface::onChar(vtkObject * object, unsigned long event, void * ctx)
{
}

void
OInteractorInterface::onMouseMove(vtkObject * object, unsigned long event, void * ctx)
{
    auto * interactor_style = dynamic_cast<vtkInteractorStyle *>(object);
    if (interactor_style) {
        auto * interactor = interactor_style->GetInteractor();
        auto pos = interactor->GetEventPosition();
        auto pt = QPoint(pos[0], pos[1]);
        this->widget->onMouseMove(pt);
    }
}
