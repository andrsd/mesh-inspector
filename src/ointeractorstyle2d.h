// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "ointeractorinterface.h"
#include "vtkInteractorStyleImage.h"

class OInteractorStyle2D : public vtkInteractorStyleImage, public OInteractorInterface {
public:
    explicit OInteractorStyle2D(MainWindow * widget);

protected:
    void OnLeftButtonDown() override;
    void OnLeftButtonUp() override;
    void OnMiddleButtonDown() override;
    void OnMiddleButtonUp() override;
    void OnRightButtonDown() override;
    void OnRightButtonUp() override;
    void OnMouseMove() override;
};
