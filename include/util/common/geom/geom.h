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

    template < typename G >
    inline G rotate
    (
        const G & geometry,
        double radians,
        point2d_t at = {}
    );

    template < typename G >
    inline G scale
    (
        const G & geometry,
        double n,
        point2d_t at = {}
    );

    template < typename G >
    inline G move
    (
        const G & geometry,
        point2d_t value
    );

    template < typename G >
    inline point2d_t center
    (
        const G & geometry
    );

    template < typename G1, typename G2 >
    inline bool intersects
    (
        const G1 & geometry1,
        const G2 & geometry2
    );

    template < typename G1, typename G2 >
    inline bool contains
    (
        const G1 & geometry1,
        const G2 & geometry2
    );
}
