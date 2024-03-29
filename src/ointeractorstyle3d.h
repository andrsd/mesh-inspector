// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "ointeractorinterface.h"
#include "vtkInteractorStyleTrackballCamera.h"

class OInteractorStyle3D : public vtkInteractorStyleTrackballCamera, public OInteractorInterface {
public:
    explicit OInteractorStyle3D(MainWindow * widget);

protected:
    void OnLeftButtonDown() override;
    void OnLeftButtonUp() override;
    void OnMiddleButtonDown() override;
    void OnMiddleButtonUp() override;
    void OnRightButtonDown() override;
    void OnRightButtonUp() override;
    void OnMouseMove() override;
    void OnKeyPress() override;
    void OnKeyRelease() override;
    void OnChar() override;
};
