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
    /*                   confidence                      */
    /*****************************************************/

    using confidence_t = int;

    namespace confidence
    {
        static const confidence_t positive = 1;
        static const confidence_t zero     = 0;
        static const confidence_t negative = -1;
    }

    /*****************************************************/
    /*                     fuzzy                         */
    /*****************************************************/

    template < typename _Traits >
    class fuzzy
    {

    public:

        using traits = _Traits;
        using type   = typename traits::type;

        /**
         *  requires:
         *      { x1 == x2 } => bool
         *      { x1 <= x2 } => bool
         *      { x1 + x2 } => type
         *
         *  returns:
         *      1 - equal exactly
         *      0 - equal with respect
         *          to the given tolerance
         *     -1 - not equal
         */
        static confidence_t equals(type x1, type x2, type tolerance = traits::tolerance())
        {
            if (x1 == x2) return confidence::positive;
            if ((x1 <= x2 + tolerance)
                && (x2 <= x1 + tolerance)) return confidence::zero;
            return confidence::negative;
        }

        /**
         *  requires:
         *      { x1 > x2 } => bool
         *      { x1 + x2 } => type
         *
         *  returns:
         *      1 - less
         *      0 - equal with respect
         *          to the given tolerance
         *     -1 - greater
         */
        static confidence_t less(type x1, type x2, type tolerance = traits::tolerance())
        {
            if (x2 > x1 + tolerance) return confidence::positive;
            if (x1 > x2 + tolerance) return confidence::negative;
            return confidence::zero;
        }

        /**
         *  requires:
         *      { x1 > x2 } => bool
         *      { x1 + x2 } => type
         *
         *  returns:
         *      1 - greater
         *      0 - equal with respect
         *          to the given tolerance
         *     -1 - less
         */
        static confidence_t greater(type x1, type x2, type tolerance = traits::tolerance())
        {
            if (x2 > x1 + tolerance) return confidence::negative;
            if (x1 > x2 + tolerance) return confidence::positive;
            return confidence::zero;
        }

        static bool eq(type x1, type x2, type tolerance = traits::tolerance())
        {
            return equals(x1, x2, tolerance) >= 0;
        }

        static int lt(type x1, type x2, type tolerance = traits::tolerance())
        {
            return less(x1, x2, tolerance) > 0;
        }

        static int gt(type x1, type x2, type tolerance = traits::tolerance())
        {
            return greater(x1, x2, tolerance) > 0;
        }

        static int le(type x1, type x2, type tolerance = traits::tolerance())
        {
            return less(x1, x2, tolerance) >= 0;
        }

        static int ge(type x1, type x2, type tolerance = traits::tolerance())
        {
            return greater(x1, x2, tolerance) >= 0;
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
