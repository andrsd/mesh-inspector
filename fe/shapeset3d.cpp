#include "shapeset3d.h"

namespace fe {

Shapeset3D::Shapeset3D() : Shapeset(), vertex_indices(nullptr)
{
    for (int i = 0; i < RealFunction3D::VALUE_TYPES; i++)
        this->shape_table[i] = nullptr;
}

} // namespace fe
