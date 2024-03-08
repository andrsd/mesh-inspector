// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ointeractorstyle3d.h"
#include "mainwindow.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include <QKeyEvent>
#include <QCoreApplication>

OInteractorStyle3D::OInteractorStyle3D(MainWindow * widget) :
    vtkInteractorStyleTrackballCamera(),
    OInteractorInterface(widget)
{
}

void
OInteractorStyle3D::OnLeftButtonDown()
{
    auto event_pos = this->Interactor->GetEventPosition();
    auto pt = QPoint(event_pos[0], event_pos[1]);
    OInteractorInterface::onLeftButtonPress(pt);
}

void
OInteractorStyle3D::OnLeftButtonUp()
{
    auto event_pos = this->Interactor->GetEventPosition();
    auto pt = QPoint(event_pos[0], event_pos[1]);
    OInteractorInterface::onLeftButtonRelease(pt);
}

void
OInteractorStyle3D::OnMiddleButtonDown()
{
    int * event_pos = this->Interactor->GetEventPosition();
    this->FindPokedRenderer(event_pos[0], event_pos[1]);
    if (this->CurrentRenderer == nullptr)
        return;

    this->GrabFocus(this->EventCallbackCommand);
    this->StartPan();
}

void
OInteractorStyle3D::OnMiddleButtonUp()
{
    switch (this->State) {
    case VTKIS_PAN:
        this->EndPan();
        if (this->Interactor)
            this->ReleaseFocus();
        break;
    }
}

void
OInteractorStyle3D::OnRightButtonDown()
{
    int * event_pos = this->Interactor->GetEventPosition();
    this->FindPokedRenderer(event_pos[0], event_pos[1]);
    if (this->CurrentRenderer == nullptr)
        return;

    this->GrabFocus(this->EventCallbackCommand);
    if (this->Interactor->GetControlKey())
        this->StartPan();
    else
        this->StartRotate();
}

void
OInteractorStyle3D::OnRightButtonUp()
{
    switch (this->State) {
    case VTKIS_ROTATE:
        this->EndRotate();
        break;

    case VTKIS_PAN:
        this->EndPan();
        break;
    }

    if (this->Interactor)
        this->ReleaseFocus();
}

void
OInteractorStyle3D::OnMouseMove()
{
    auto event_pos = this->Interactor->GetEventPosition();
    auto pt = QPoint(event_pos[0], event_pos[1]);
    OInteractorInterface::onMouseMove(pt);
    vtkInteractorStyleTrackballCamera::OnMouseMove();
}

void
OInteractorStyle3D::OnKeyPress()
{
    auto key = this->Interactor->GetKeyCode();
    auto seq = QKeySequence(key);
    auto mods = getKeyboardModifiers(this->Interactor);
    OInteractorInterface::onKeyPress(seq, mods);
}

void
OInteractorStyle3D::OnKeyRelease()
{
}

void
OInteractorStyle3D::OnChar()
{
}
