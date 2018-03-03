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

    inline double sqrt_tolerance()
    {
        static double tol = std::sqrt(fuzzy_geom_traits_t::tolerance());
        return tol;
    }

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

        inline bool is_trusted(status_t s, status_t bits)
        {
            return is(s, bits | (bits << 1));
        }

        inline math::confidence_t get(status_t s, status_t bits)
        {
            return is(s, bits) ?
                        (is(s, bits << 1) ?
                            math::confidence::positive :
                            math::confidence::zero) :
                        math::confidence::negative;
        }

        inline status_t trusted(status_t s)
        {
            return s | (s << 1);
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
}
