#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom.h>

#include <type_traits>
#include <iostream>

namespace geom
{

    /*****************************************************/
    /*              some common operations               */
    /*****************************************************/

    /* use forward implementation of functions instead of
       just forward declaration to make code shorter */

    template
    <
        typename _X1, typename _Y1,
        typename _X2, typename _Y2
    >
    inline double distance
    (
        point < _X1, _Y1 > const & p1,
        point < _X2, _Y2 > const & p2
    )
    {
        return math::norm(p1 - p2);
    }

    template
    <
        typename _X1, typename _Y1,
        typename _X2, typename _Y2
    >
    inline double sqdistance
    (
        point < _X1, _Y1 > const & p1,
        point < _X2, _Y2 > const & p2
    )
    {
        return math::sqnorm(p1 - p2);
    }

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

        template < typename = std::enable_if_t <
            is_planar < point > :: value > >
        operator POINT () const
        {
            return{ x, y };
        }

        bool empty() const
        {
            return ((x == x_type{}) && (y == y_type{}));
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

        template < typename = std::enable_if_t <
            ! std::is_unsigned < X > :: value &&
            ! std::is_unsigned < Y > :: value && > >
        auto operator - () const
            -> point < std::remove_const_t < decltype(- x) >,
                       std::remove_const_t < decltype(- y) > >
        {
            return { - x, - y };
        }

        template < typename _Z >
        auto operator * (_Z n)
            -> point < std::remove_const_t < decltype(x * n) >,
                       std::remove_const_t < decltype(y * n) > >
        {
            return { x * n, y * n };
        }

        template < typename _Z >
        auto operator / (_Z n)
            -> point < std::remove_const_t < decltype(x / n) >,
                       std::remove_const_t < decltype(y / n) > >
        {
            return { x / n, y / n };
        }

        double length() const
        {
            return math::norm(*this);
        }

        template < typename = std::enable_if_t <
            is_planar < point > :: value > >
        double angle() const
        {
            return atan2(static_cast < double > (y),
                         static_cast < double > (x));
        }

        template < typename _X, typename _Y >
        double distance(point < _X, _Y > const & o) const
        {
            return geom::distance(*this, o);
        }

        template < typename _X, typename _Y >
        double distance(_X x, _Y y) const
        {
            return geom::distance(*this, make_point(x, y));
        }

        auto rotate (double radians) const
        -> point < std::remove_const_t < decltype(x * 1. - y * 1.) >,
                   std::remove_const_t < decltype(x * 1. + y * 1.) > >
        {
            double sine = std::sin(radians);
            double cosine = std::cos(radians);
            auto & p = *this;
            return
            {
                p.x * cosine - p.y * sine,
                p.x * sine + p.y * cosine
            };
        }

        template < typename _X, typename _Y >
        auto rotate
        (
            double radians,
            const point < _X, _Y > & at
        ) const
        -> std::remove_const_t < decltype((std::declval<point>() - at).rotate(radians) + at) >
        {
            return (*this - at).rotate(radians) + at;
        }
    };

    /*****************************************************/
    /*                factory functions                  */
    /*****************************************************/

    template < typename _X, typename _Y >
    inline auto make_point(_X x, _Y y)
        -> point < std::remove_const_t < _X >,
                   std::remove_const_t < _Y > >
    {
        return { x, y };
    }

    /*****************************************************/
    /*                type traits                        */
    /*****************************************************/

    template < typename _X, typename _Y >
    struct is_planar < point < _X, _Y > >
        : std::integral_constant < bool,
            std::is_convertible < _X, double > :: value &&
            std::is_convertible < _Y, double > :: value >
    {
    };

    /*****************************************************/
    /*                geometry operations                */
    /*****************************************************/

    template <> inline point2d_t rotate
    (
        const point2d_t & geometry,
        double radians,
        point2d_t at
    )
    {
        return geometry.rotate(radians, at);
    }

    template <> inline point2d_t scale
    (
        const point2d_t & geometry,
        double n,
        point2d_t at
    )
    {
        return (geometry - at) * n + at;
    }

    template <> inline point2d_t move
    (
        const point2d_t & geometry,
        point2d_t value
    )
    {
        return geometry + value;
    }

    template <> inline point2d_t center
    (
        const point2d_t & geometry
    )
    {
        return geometry;
    }

    inline bool intersects
    (
        const point2d_t &, const point2d_t &
    )
    {
        return false;
    }

    template < typename G > inline bool intersects
    (
        const point2d_t &, const G &
    )
    {
        return false;
    }

    template < typename G > inline bool intersects
    (
        const G &, const point2d_t &
    )
    {
        return false;
    }

    /* point cannot contain anything */
    template < typename G >
    inline bool contains
    (
        const point2d_t &, const G &
    )
    {
        return false;
    }

    /*****************************************************/
    /*                formatting                         */
    /*****************************************************/

    template < typename _Elem, typename _Traits,
               typename _X, typename _Y >
    std::basic_ostream < _Elem, _Traits > & operator <<
    (
        std::basic_ostream < _Elem, _Traits > & os,
        const point < _X, _Y > & p
    )
    {
        return os << "(" << p.x << ", " << p.y << ")";
    }
}

namespace math
{

    /*****************************************************/
    /*            scalar point operations                */
    /*****************************************************/

    template < typename _X, typename _Y >
    inline double norm(geom::point < _X, _Y > const & p)
    {
        return std::sqrt(sqnorm(p));
    }

    template < typename _X, typename _Y >
    inline double sqnorm(geom::point < _X, _Y > const & p)
    {
        return sqnorm(p.x) + sqnorm(p.y);
    }

    template < typename _X, typename _Y >
    inline geom::point < _X, _Y > conjugate(geom::point < _X, _Y > const & p)
    {
        return { conjugate(p.x), conjugate(p.y) };
    }
}
