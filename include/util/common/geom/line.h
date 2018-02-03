#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom.h>
#include <util/common/geom/point.h>

#include <type_traits>
#include <iostream>

namespace geom
{

    /*****************************************************/
    /*                     line                          */
    /*****************************************************/

    struct line
    {
        using ptr_t = util::ptr_t < line > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        point2d_t p1, p2;

        line(point2d_t p1, point2d_t p2)
            : p1(p1), p2(p2)
        {
        }

        line()
            : line(point2d_t{}, point2d_t{})
        {
        }

        line(line const & o)
            : line(o)
        {
        }

        bool empty() const
        {
            return p1.empty() && p2.empty();
        }

        bool operator == (line const & o) const
        {
            return (p1 == o.p1) && (p2 == o.p2);
        }

        bool operator != (line const & o) const
        {
            return (p1 != o.p1) || (p2 != o.p2);
        }

        double length() const
        {
            return norm(*this);
        }

        double angle() const
        {
            return (p2 - p1).angle();
        }
    };

    /*****************************************************/
    /*            scalar point operations                */
    /*****************************************************/

    template < typename _P >
    inline double norm(line < _P > const & l)
    {
        return distance(l.p1, l.p2);
    }

    template < typename _P >
    inline double sqnorm(line < _P > const & l)
    {
        return sqdistance(l.p1, l.p2);
    }

    template < typename _P >
    inline line < _P > conjugate(line < _P > const & l)
    {
        return { conjugate(l.p1), conjugate(l.p2) };
    }

    /*****************************************************/
    /*                factory functions                  */
    /*****************************************************/

    template < typename _P1, typename _P2 >
    inline line make_line(_P1 p1, _P2 p2)
    {
        return { p1, p2 };
    }

    template < typename _X1, typename _Y1,
               typename _X2, typename _Y2 >
    inline line make_line(_X1 x1, _Y1 y1, _X2 x2, _Y2 y2)
    {
        return { make_point(x1, y1), make_point(x2, y2) };
    }

    /*****************************************************/
    /*                type traits                        */
    /*****************************************************/

    template <> struct is_planar < line >
        : std::integral_constant < bool, true >
    {
    };

    /*****************************************************/
    /*                formatting                         */
    /*****************************************************/

    template < typename _Elem, typename _Traits, typename _P >
    std::basic_ostream < _Elem, _Traits > & operator <<
    (
        std::basic_ostream < _Elem, _Traits > & os,
        const line < _P > & l
    )
    {
        return os << l.p1 << " - " << l.p2;
    }
}
