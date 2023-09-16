#pragma once

#include "ointeractorinterface.h"
#include "vtkInteractorStyleImage.h"

class OInteractorStyle2D : public vtkInteractorStyleImage, public OInteractorInterface {
public:
    OInteractorStyle2D(MainWindow * widget);

protected:
    void OnLeftButtonDown() override;
    void OnLeftButtonUp() override;
    void OnMiddleButtonDown() override;
    void OnMiddleButtonUp() override;
    void OnRightButtonDown() override;
    void OnRightButtonUp() override;
    void OnMouseMove() override;
};
