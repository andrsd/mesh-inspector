#include "shapeset1d.h"

namespace fe {

Shapeset1D::Shapeset1D() : Shapeset(), vertex_indices(nullptr)
{
    /// FIXME: 2 == RealFucntion1D::VALUE_TYPES
    for (int i = 0; i < 2; i++)
        this->shape_table[i] = nullptr;
}

} // namespace fe
