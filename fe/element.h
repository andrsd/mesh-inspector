#pragma once

#include "vtkCell.h"
#include "vertex.h"

namespace fe {

class Element {
public:
    Element(vtkCell * cell);

    int get_dim();
    int get_type();
    int get_num_vertices();
    double * get_vertex_coords(int vtx);

protected:
    vtkCell * vtk_cell;
};

} // namespace fe
