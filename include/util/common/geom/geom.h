#pragma once

#include <type_traits>

namespace geom
{

    template < typename G >
    struct is_planar
        : std::integral_constant < bool, false >
    {
    };

}