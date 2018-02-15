#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom_fwd.h>
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
            return p1 == p2;
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

        /* angle between OX and this line;
           >0 for counterclockwise rotation */
        double angle() const
        {
            return (p2 - p1).angle();
        }

        /* returns point between p1 and p2;

           0 => p1; 1 => p2;

           q - any number
         */
        point2d_t inner_point(double q) const
        {
            return p1 + (p2 - p1) * q;
        }

        math::fuzzy_value < double > sqdistance(const point2d_t & p) const
        {
            if (empty()) return math::fuzzy_values::negative_confidence(0.0);
            auto d = p2 - p1;
            auto v = point2d_t(p.x + d.y, p.y - d.x);
            double q1, q2;
            auto l = line(p, v);
            auto i = intersection(l, q1, q2);
            /* should not occur, possible only in case
               if line segment is too small or point is too
               close to line, so return 0 */
            if (i < 0) return math::fuzzy_values::zero_confidence(0.0);
            return math::fuzzy_values::positive_confidence(math::sqnorm(inner_point(q1) - p));
        }

        math::fuzzy_value < double > segment_sqdistance(const point2d_t & p) const
        {
            if (empty()) return math::fuzzy_values::negative_confidence(0.0);
            auto d = p2 - p1;
            auto v = point2d_t(p.x + d.y, p.y - d.x);
            double q1, q2;
            auto l = line(p, v);
            auto i = intersection(l, q1, q2);
            /* should not occur, possible only in case
               if line segment is too small or point is too
               close to line, so return 0 */
            if (i < 0) return math::fuzzy_values::zero_confidence(0.0);
            /* check if intersection is inside the line segment */
            double d1 = length(), s1 = d1 * min(q1, 1 - q1);
            auto i1 = fuzzy_t::greater(s1, 0);
            if (i1 > 0)
                return math::fuzzy_values::positive_confidence(
                    math::sqnorm(inner_point(q1) - p));
            if (i1 < 0)
                return math::fuzzy_values::negative_confidence(0.0);
            return math::fuzzy_values::zero_confidence(0.0);
        }

        math::fuzzy_value < double > distance(const point2d_t & p) const
        {
            auto d = sqdistance(p);
            return { d.confidence, std::sqrt(d.value) };
        }

        math::fuzzy_value < double > segment_distance(const point2d_t & p) const
        {
            auto d = segment_sqdistance(p);
            return { d.confidence, std::sqrt(d.value) };
        }

        math::confidence_t contains(const point2d_t & p) const
        {
            auto d = distance(p);
            if (d.confidence <= 0) return d.confidence;
            return fuzzy_t::equals(0, d);
        }

        math::confidence_t segment_contains(const point2d_t & p) const
        {
            auto d = segment_distance(p);
            if (d.confidence <= 0) return d.confidence;
            return fuzzy_t::equals(0, d);
        }

        /* calculates an intersection point;

           returns true if two lines have
           intersection point and fills r1 and r2
           with parameters on this and given lines
           correspondingly;

           intersection point = inner_point(r1)
                              = l.inner_point(r2)

           r1, r2 - any numbers
         */
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

        /* calculates an intersection point of two
           line segments;

           returns true if two line segments have
           intersection point and fills r1 and r2
           with parameters on this and given lines
           correspondingly;

           intersection point = inner_point(r1)
                              = l.inner_point(r2)

           r1, r2 in range (0, 1)
         */
        bool segment_intersection(const line & l,
                                  double & r1,
                                  double & r2) const
        {
            double q1, q2;

            if (intersection(l, q1, q2) &&
                fuzzy_t::gt(q1, 0) && fuzzy_t::lt(q1, 1) &&
                fuzzy_t::gt(q2, 0) && fuzzy_t::lt(q2, 1))
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
            return segment_intersection(l, q1, q2);
        }

        /* calculates convexity type of the triangle
           given p1, p2 of this line and the given p3;

           returns convex_type::degenerate if (p1, p2, p3)
           is not triangle (pi ~= pj or pk ~lies on the
           pi-pj line);
         */
        convex_type convexity(const point2d_t & p3) const
        {
            double n = ((p1.x - p3.x) * (p2.y - p3.y) -
                        (p2.x - p3.x) * (p1.y - p3.y));
            double dd = (sqdistance(p1, p3) +
                         sqdistance(p2, p3) +
                         sqdistance(p1, p2)) / 3;
            double q = n * n / dd;
            if (!isfinite(q) || fuzzy_t::eq(0, q))
            {
                return convex_type::degenerate;
            }
            if (n < 0)
            {
                return convex_type::clockwise;
            }
            return convex_type::counterclockwise;
        }

        convex_type convexity(double x, double y) const
        {
            return convexity({ x, y });
        }
    };

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
    /*                helper functions                   */
    /*****************************************************/

    inline convex_type convexity(const point2d_t & p1,
                                 const point2d_t & p2,
                                 const point2d_t & p3)
    {
        return make_line(p1, p2).convexity(p3);
    }

    inline double angle(const point2d_t & p1,
                        const point2d_t & p2)
    {
        return make_line(p1, p2).angle();
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
}

inline double geom::line::length() const
{
    return math::norm(*this);
}
