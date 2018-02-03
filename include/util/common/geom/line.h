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

    template
    <
        typename P
    >
    struct line
    {
        using ptr_t = util::ptr_t < line > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        using point_type = P;

        P p1, p2;

        line(P p1, P p2)
            : p1(p1), p2(p2)
        {
        }

        line()
            : line(P{}, P{})
        {
        }

        template < typename _P >
        line(line < _P > const & o)
            : line(o)
        {
        }

        bool empty() const
        {
            return p1.empty() && p2.empty();
        }

        template < typename _P >
        bool operator == (line < _P > const & o) const
        {
            return (p1 == o.p1) && (p2 == o.p2);
        }

        template < typename _P >
        bool operator != (line < _P > const & o) const
        {
            return (p1 != o.p1) || (p2 != o.p2);
        }

        double length() const
        {
            return norm(*this);
        }

        template < typename = std::enable_if_t <
            is_planar < line > :: value > >
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

    template < typename _P >
    inline auto make_line(_P p1, _P p2)
        -> line < std::remove_const_t < _P > >
    {
        return { p1, p2 };
    }

    template < typename _X, typename _Y >
    inline auto make_line(_X x1, _Y y1, _X x2, _Y y2)
        -> line < point < _X, _Y > >
    {
        return { make_point(x1, y1), make_point(x2, y2) };
    }

    /*****************************************************/
    /*                type traits                        */
    /*****************************************************/

    template < typename _P >
    struct is_planar < line < _P > >
        : std::integral_constant < bool,
            is_planar < _P > :: value >
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
