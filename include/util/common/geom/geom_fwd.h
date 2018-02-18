#pragma once

#include <type_traits>

#include <util/common/math/fuzzy.h>

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

    using fuzzy_geom_traits_t = math::fuzzy_weak_double_traits;
    using fuzzy_t = math::fuzzy < fuzzy_geom_traits_t > ;

    using status_t = size_t;

    namespace status
    {
        enum : status_t
        {
            ok = 1 << 0
        };

        inline bool is(status_t s, status_t bits)
        {
            return (s & bits) == bits;
        }

        inline bool is_any(status_t s, status_t bits)
        {
            return (s & bits) != 0;
        }

        inline bool is_not(status_t s, status_t bits)
        {
            return (s & bits) == 0;
        }

        inline bool is_ok(status_t s)
        {
            return is(s, ok);
        }
    }

    using flags_t = size_t;

    namespace flags
    {
        inline bool has(flags_t f, flags_t bits)
        {
            return (f & bits) == bits;
        }

        inline bool has_any(flags_t f, flags_t bits)
        {
            return (f & bits) != 0;
        }

        inline bool has_none(flags_t f, flags_t bits)
        {
            return (f & bits) == 0;
        }
    }

    enum class convex_type
    {
        no,
        degenerate,
        counterclockwise,
        clockwise
    };

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
