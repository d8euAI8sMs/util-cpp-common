#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>

#include <type_traits>

namespace geom
{

    /*****************************************************/
    /*                     point                         */
    /*****************************************************/

    template
    <
        typename X,
        typename Y = X
    >
    struct point
    {
        using ptr_t = util::ptr_t < point > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        using x_type = X;
        using y_type = Y;

        x_type x;
        y_type y;

        point(x_type x, y_type y)
            : x(x), y(y)
        {
        }

        point()
            : point(x_type{}, y_type{})
        {
        }

        template < typename _X, typename _Y >
        point(point < _X, _Y > const & o)
            : point(static_cast < X > (o.x),
                    static_cast < Y > (o.y))
        {
        }

        operator POINT () const
        {
            return{ x, y };
        }

        bool empty() const
        {
            return ((x == 0) && (y == 0));
        }

        template < typename _X, typename _Y >
        bool operator == (point < _X, _Y > const & o) const
        {
            return (x == o.x) && (y == o.y);
        }

        template < typename _X, typename _Y >
        bool operator != (point < _X, _Y > const & o) const
        {
            return (x != o.x) || (y != o.y);
        }

        template < typename _X, typename _Y >
        auto operator - (point < _X, _Y > const & o) const
            -> point < std::remove_const_t < decltype(x - o.x) >,
                       std::remove_const_t < decltype(y - o.y) > >
        {
            return { x - o.x, y - o.y };
        }

        template < typename _X, typename _Y >
        auto operator + (point < _X, _Y > const & o) const
            -> point < std::remove_const_t < decltype(x - o.x) >,
                       std::remove_const_t < decltype(y - o.y) > >
        {
            return { x + o.x, y + o.y };
        }

        template < typename _X, typename _Y >
        point & operator -= (point < _X, _Y > const & o)
        {
            x -= o.x;
            y -= o.y;
            return *this;
        }

        template < typename _X, typename _Y >
        point & operator += (point < _X, _Y > const & o)
        {
            x += o.x;
            y += o.y;
            return *this;
        }
    };
}
