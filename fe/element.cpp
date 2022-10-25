#include "element.h"
#include "vtkPoints.h"

namespace fe {

Element::Element(vtkCell * cell) : vtk_cell(cell) {}

int
Element::get_dim()
{
    return this->vtk_cell->GetCellDimension();
}

int
Element::get_type()
{
    return this->vtk_cell->GetCellType();
}

int
Element::get_num_vertices()
{
    return this->vtk_cell->GetNumberOfPoints();
}

double *
Element::get_vertex_coords(int vtx)
{
    vtkPoints * pts = this->vtk_cell->GetPoints();
    return pts->GetPoint(vtx);
}

} // namespace fe
