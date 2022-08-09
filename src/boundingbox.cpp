#include "boundingbox.h"

BoundingBox::BoundingBox() : min_pt(0, 0, 0), max_pt(0, 0, 0) {}

BoundingBox::BoundingBox(double xmin,
                         double xmax,
                         double ymin,
                         double ymax,
                         double zmin,
                         double zmax) :
    min_pt(xmin, ymin, zmin),
    max_pt(xmax, ymax, zmax)
{
}

BoundingBox::BoundingBox(const vtkVector3d & min, const vtkVector3d & max) :
    min_pt(min),
    max_pt(max)
{
}

vtkVector3d
BoundingBox::center() const
{
    vtkVector3d ctr;
    for (int i = 0; i < 3; i++)
        ctr[i] = (this->min_pt[i] + this->max_pt[i]) / 2;
    return ctr;
}

double
BoundingBox::min(int i) const
{
    return this->min_pt[i];
}

double
BoundingBox::max(int i) const
{
    return this->max_pt[i];
}

void
BoundingBox::doUnion(const BoundingBox & a)
{
    for (int i = 0; i < 3; i++)
        this->min_pt[i] = std::min(this->min_pt[i], a.min(i));
    for (int i = 0; i < 3; i++)
        this->max_pt[i] = std::max(this->max_pt[i], a.max(i));
}
