#include "shapefunction3d.h"
#include "element.h"

namespace fe {

ShapeFunction3D::ShapeFunction3D() : RealFunction3D()
{
    this->shapeset = NULL;
    this->num_components = 0;
}

ShapeFunction3D::ShapeFunction3D(Shapeset3D * shapeset) : RealFunction3D()
{
    set_shapeset(shapeset);
}

ShapeFunction3D::~ShapeFunction3D()
{
    free();
}

void
ShapeFunction3D::set_active_shape(int index)
{
    free_cur_node();
    this->index = index;
    this->order = shapeset->get_order(index);
}

void
ShapeFunction3D::set_active_element(Element * e)
{
    free_cur_node();
    this->elem = e;
}

void
ShapeFunction3D::free()
{
    free_cur_node();
}

void
ShapeFunction3D::set_shapeset(Shapeset3D * ss)
{
    free_cur_node();
    this->shapeset = ss;
    this->num_components = ss->get_num_components();
    assert(this->num_components == 1 || this->num_components == 3);
}

void
ShapeFunction3D::precalculate(const int np, const QuadPt3D * pt, int mask)
{
    int oldmask = (cur_node != NULL) ? this->cur_node->mask : 0;
    int newmask = mask | oldmask;
    Node * node = new_node(newmask, np);

    // precalculate all required tables
    for (int ic = 0; ic < this->num_components; ic++) {
        for (int j = 0; j < RealFunction3D::VALUE_TYPES; j++) {
            if (newmask & idx2mask[j][ic])
                this->shapeset->get_values(j, this->index, np, pt, ic, node->values[ic][j]);
        }
    }

    // remove the old node and attach the new one to the Judy array
    replace_cur_node(node);
}

} // namespace fe
