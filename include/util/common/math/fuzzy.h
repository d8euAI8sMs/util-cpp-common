#pragma once

#include <limits>

namespace math
{

    /*****************************************************/
    /*                     traits                        */
    /*****************************************************/

    template < typename _T >
    struct fuzzy_traits
    {
        using type = _T;
        static type tolerance();
    };

    template < typename _T, typename _R, _R (*_TolF) () >
    struct fuzzy_delegating_traits
        : fuzzy_traits < _T >
    {
        static type tolerance() { return static_cast < type > (_TolF()); }
    };

    template < typename _T, typename _R = _T >
    struct fuzzy_numeric_traits
        : fuzzy_delegating_traits < _T, _R, std::numeric_limits < _R > :: epsilon >
    {
    };

    struct fuzzy_weak_double_traits
        : fuzzy_delegating_traits < double, float, std::numeric_limits < float > :: epsilon >
    {
    };

    /*****************************************************/
    /*                     fuzzy                         */
    /*****************************************************/

    template < typename _Traits >
    class fuzzy
    {

        enum : int
        {
            _sure_not = -1,
            _unsure = 0,
            _sure = 1
        };

        enum : int
        {
            _less = -1,
            _equal = 0,
            _greater = 1
        };

    public:

        using traits = _Traits;
        using type   = typename traits::type;

        static int equals(type x1, type x2, type tolerance = traits::tolerance())
        {
            if (x1 == x2) return _sure;
            if ((x1 <= x2 + tolerance)
                && (x2 <= x1 + tolerance)) return _unsure;
            return _sure_not;
        }

        static int less(type x1, type x2, type tolerance = traits::tolerance())
        {
            if (x2 > x1 + tolerance) return _sure;
            if (x1 > x2 + tolerance) return _sure_not;
            return _unsure;
        }

        static int greater(type x1, type x2, type tolerance = traits::tolerance())
        {
            if (x2 > x1 + tolerance) return _sure_not;
            if (x1 > x2 + tolerance) return _sure;
            return _unsure;
        }

    public:

        type value;

    public:

        fuzzy()           : value()      { }
        fuzzy(type value) : value(value) { }

    public:

        explicit operator type () const { return value; }

        type operator () () const { return value; }

    public:

        bool operator == (const fuzzy & o)
        {
            return (equals(value, o.value) >= 0);
        }

        bool operator != (const fuzzy & o)
        {
            return (equals(value, o.value) < 0);
        }

        bool operator < (const fuzzy & o)
        {
            return (less(value, o.value) > 0);
        }

        bool operator > (const fuzzy & o)
        {
            return (greater(value, o.value) > 0);
        }

        bool operator <= (const fuzzy & o)
        {
            return (less(value, o.value) >= 0);
        }

        bool operator >= (const fuzzy & o)
        {
            return (greater(value, o.value) >= 0);
        }
    };

    /*****************************************************/
    /*                     factory                       */
    /*****************************************************/

    template < typename _Traits >
    fuzzy < _Traits > make_fuzzy(typename _Traits::type value)
    {
        return fuzzy < _Traits > (value);
    }
}
