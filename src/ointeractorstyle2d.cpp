// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ointeractorstyle2d.h"
#include "vtkCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCallbackCommand.h"

OInteractorStyle2D::OInteractorStyle2D(MainWindow * widget) :
    vtkInteractorStyleImage(),
    OInteractorInterface(widget)
{
}

void
OInteractorStyle2D::OnLeftButtonDown()
{
    auto event_pos = this->Interactor->GetEventPosition();
    auto pt = QPoint(event_pos[0], event_pos[1]);
    OInteractorInterface::onLeftButtonPress(pt);
}

void
OInteractorStyle2D::OnLeftButtonUp()
{
    auto event_pos = this->Interactor->GetEventPosition();
    auto pt = QPoint(event_pos[0], event_pos[1]);
    OInteractorInterface::onLeftButtonRelease(pt);
}

void
OInteractorStyle2D::OnMiddleButtonDown()
{
    auto event_pos = this->Interactor->GetEventPosition();
    this->FindPokedRenderer(event_pos[0], event_pos[1]);
    if (this->CurrentRenderer == nullptr)
        return;

    this->GrabFocus(this->EventCallbackCommand);
    this->StartPan();
}

void
OInteractorStyle2D::OnMiddleButtonUp()
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
OInteractorStyle2D::OnRightButtonDown()
{
    auto event_pos = this->Interactor->GetEventPosition();
    this->FindPokedRenderer(event_pos[0], event_pos[1]);
    if (this->CurrentRenderer == nullptr)
        return;

    this->GrabFocus(this->EventCallbackCommand);
    this->StartPan();
}

void
OInteractorStyle2D::OnRightButtonUp()
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
OInteractorStyle2D::OnMouseMove()
{
    auto event_pos = this->Interactor->GetEventPosition();
    auto pt = QPoint(event_pos[0], event_pos[1]);
    OInteractorInterface::onMouseMove(pt);
    vtkInteractorStyleImage::OnMouseMove();
}
