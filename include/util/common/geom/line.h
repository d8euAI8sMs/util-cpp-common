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
            : line(o.p1, o.p2)
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

        /* points to math::norm specialized below */
        double length() const;

        double angle() const
        {
            return (p2 - p1).angle();
        }

        point2d_t inner_point(double q) const
        {
            return p1 + (p2 - p1) * q;
        }

        bool intersection(const line & l,
                          double & r1,
                          double & r2) const
        {
            double d =
                (l.p2.y - l.p1.y) * (p2.x - p1.x) -
                (l.p2.x - l.p1.x) * (p2.y - p1.y);

            double n1 =
                (l.p2.x - l.p1.x) * (p1.y - l.p1.y) -
                (l.p2.y - l.p1.y) * (p1.x - l.p1.x);
            double n2 =
                (p2.x - p1.x) * (p1.y - l.p1.y) -
                (p2.y - p1.y) * (p1.x - l.p1.x);

            double q1 = n1 / d;
            double q2 = n2 / d;

            if (isfinite(q1) && isfinite(q2))
            {
                r1 = q1;
                r2 = q2;
                return true;
            }

            return false;
        }

        bool intersects(const line & l) const
        {
            double q1, q2;
            return intersection(l, q1, q2);
        }

        bool segment_intersects(const line & l) const
        {
            double q1, q2;

            if (intersection(l, q1, q2) &&
                (1e-8 < q1) && (q1 < (1 - 1e-8)) &&
                (1e-8 < q2) && (q2 < (1 - 1e-8)))
            {
                return true;
            }

            return false;
        }

        /* true for right-hand rotation (p1, p2, p3) */
        bool is_clockwise(double x, double y) const
        {
            return ((p1.x - x) * (p2.y - y) -
                    (p2.x - x) * (p1.y - y)) > 0;
        }

        bool is_clockwise(const point2d_t & p3) const
        {
            return is_clockwise(p3.x, p3.y);
        }
    };

    /*****************************************************/
    /*                helper functions                   */
    /*****************************************************/

    inline bool is_clockwise(const point2d_t & p1,
                             const point2d_t & p2,
                             const point2d_t & p3)
    {
        return make_line(p1, p2).is_clockwise(p3);
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
    /*                geometry operations                */
    /*****************************************************/

    template <> inline line rotate
    (
        const line & geometry,
        double radians,
        point2d_t at
    )
    {
        return
        {
            rotate(geometry.p1, radians, at),
            rotate(geometry.p2, radians, at)
        };
    }

    template <> inline line scale
    (
        const line & geometry,
        double n,
        point2d_t at
    )
    {
        return
        {
            scale(geometry.p1, n, at),
            scale(geometry.p2, n, at)
        };
    }

    template <> inline line move
    (
        const line & geometry,
        point2d_t value
    )
    {
        return
        {
            move(geometry.p1, value),
            move(geometry.p2, value)
        };
    }

    template <> inline point2d_t center
    (
        const line & geometry
    )
    {
        return geometry.inner_point(0.5);
    }

    template <> inline bool intersects
    (
        const line & l1, const line & l2
    )
    {
        return l1.segment_intersects(l2);
    }

    /* we assume that only geometry with inner
       area can contain something */
    template <> inline bool contains
    (
        const line &, const point2d_t &
    )
    {
        return false;
    }

    /*****************************************************/
    /*                formatting                         */
    /*****************************************************/

    template < typename _Elem, typename _Traits >
    std::basic_ostream < _Elem, _Traits > & operator <<
    (
        std::basic_ostream < _Elem, _Traits > & os,
        const line & l
    )
    {
        return os << l.p1 << " - " << l.p2;
    }
}

namespace math
{

    /*****************************************************/
    /*            scalar line operations                 */
    /*****************************************************/

    template<>inline double norm(geom::line const & l)
    {
        return distance(l.p1, l.p2);
    }

    template<>inline double sqnorm(geom::line const & l)
    {
        return sqdistance(l.p1, l.p2);
    }

    template<>inline geom::line conjugate(geom::line  const & l)
    {
        return { conjugate(l.p1), conjugate(l.p2) };
    }
}

inline double geom::line::length() const
{
    return math::norm(*this);
}
