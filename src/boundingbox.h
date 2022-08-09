#pragma once

#include "vtkVector.h"

class BoundingBox {
public:
    BoundingBox();
    explicit BoundingBox(double xmin,
                         double xmax,
                         double ymin,
                         double ymax,
                         double zmin,
                         double zmax);
    explicit BoundingBox(const vtkVector3d & min, const vtkVector3d & max);

    vtkVector3d center() const;
    double min(int i) const;
    double max(int i) const;

    void doUnion(const BoundingBox & a);

protected:
    vtkVector3d min_pt;
    vtkVector3d max_pt;
};
