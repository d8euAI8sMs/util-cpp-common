#pragma once

#include <type_traits>

namespace geom
{

    template < typename G >
    struct is_planar
        : std::integral_constant < bool, false >
    {
    };

    /* forward declaration */
    template < typename X, typename Y > struct point;

    using point2d_t = point < double, double > ;

}
