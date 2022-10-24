#pragma once

#include <stdexcept>

namespace fe {

class Shapeset {
public:
    Shapeset() : num_components(-1) {}

    int
    get_num_components() const
    {
        return num_components;
    }

protected:
    int num_components;

    inline void
    check_component(int comp)
    {
        if (comp < 0 || comp >= this->num_components)
            throw std::invalid_argument("Invalid component");
    }
};

} // namespace fe
