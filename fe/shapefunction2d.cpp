#include "shapefunction2d.h"
#include "element.h"

namespace fe {

ShapeFunction2D::ShapeFunction2D() : RealFunction2D()
{
    this->shapeset = NULL;
    this->num_components = 0;
}

ShapeFunction2D::ShapeFunction2D(Shapeset2D * shapeset) : RealFunction2D()
{
    set_shapeset(shapeset);
}

ShapeFunction2D::~ShapeFunction2D()
{
    free();
}

void
ShapeFunction2D::set_active_shape(int index)
{
    free_cur_node();
    this->index = index;
    this->order = shapeset->get_order(index);
}

void
ShapeFunction2D::set_active_element(Element * e)
{
    free_cur_node();
    this->elem = e;
}

void
ShapeFunction2D::free()
{
    free_cur_node();
}

void
ShapeFunction2D::set_shapeset(Shapeset2D * ss)
{
    free_cur_node();
    this->shapeset = ss;
    this->num_components = ss->get_num_components();
    assert(this->num_components == 1 || this->num_components == 3);
}

void
ShapeFunction2D::precalculate(const int np, const QuadPt2D * pt, int mask)
{
    int oldmask = (cur_node != NULL) ? this->cur_node->mask : 0;
    int newmask = mask | oldmask;
    Node * node = new_node(newmask, np);

    // precalculate all required tables
    for (int ic = 0; ic < this->num_components; ic++) {
        for (int j = 0; j < RealFunction2D::VALUE_TYPES; j++) {
            if (newmask & this->idx2mask[j][ic])
                this->shapeset->get_values(j, this->index, np, pt, ic, node->values[ic][j]);
        }
    }

    // remove the old node and attach the new one to the Judy array
    replace_cur_node(node);
}

} // namespace fe
