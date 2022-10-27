#include "shapefunction1d.h"
#include "element.h"

namespace fe {

ShapeFunction1D::ShapeFunction1D() : RealFunction1D()
{
    this->shapeset = NULL;
    this->num_components = 0;
}

ShapeFunction1D::ShapeFunction1D(Shapeset1D * shapeset) : RealFunction1D()
{
    set_shapeset(shapeset);
}

ShapeFunction1D::~ShapeFunction1D()
{
    free();
}

void
ShapeFunction1D::set_active_shape(int index)
{
    free_cur_node();
    this->index = index;
    this->order = shapeset->get_order(index);
}

void
ShapeFunction1D::set_active_element(Element * e)
{
    free_cur_node();
    this->elem = e;
}

void
ShapeFunction1D::free()
{
    free_cur_node();
}

void
ShapeFunction1D::set_shapeset(Shapeset1D * ss)
{
    free_cur_node();
    this->shapeset = ss;
    this->num_components = ss->get_num_components();
    assert(this->num_components == 1);
}

void
ShapeFunction1D::precalculate(const int np, const QuadPt1D * pt, int mask)
{
    int oldmask = (cur_node != NULL) ? this->cur_node->mask : 0;
    int newmask = mask | oldmask;
    Node * node = new_node(newmask, np);

    // precalculate all required tables
    for (int ic = 0; ic < this->num_components; ic++) {
        for (int j = 0; j < RealFunction1D::VALUE_TYPES; j++) {
            if (newmask & this->idx2mask[j][ic])
                this->shapeset->get_values(j, this->index, np, pt, ic, node->values[ic][j]);
        }
    }

    // remove the old node and attach the new one to the Judy array
    replace_cur_node(node);
}

} // namespace fe
