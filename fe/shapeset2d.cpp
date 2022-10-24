#include "shapeset2d.h"

namespace fe {

Shapeset2D::Shapeset2D() : vertex_indices(nullptr)
{
    for (int i = 0; i < RealFunction2D::VALUE_TYPES; i++)
        this->shape_table[i] = nullptr;
}

} // namespace fe
