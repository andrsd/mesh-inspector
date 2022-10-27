#include "shapeset1d.h"

namespace fe {

Shapeset1D::Shapeset1D() : Shapeset(), vertex_indices(nullptr)
{
    for (int i = 0; i < RealFunction1D::VALUE_TYPES; i++)
        this->shape_table[i] = nullptr;
}

} // namespace fe
