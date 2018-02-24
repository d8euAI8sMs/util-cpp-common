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
    /*             status codes and flags                */
    /*****************************************************/

    namespace status
    {
        namespace line
        {
            enum : status_t
            {
                contains_point       = 1 << 2,
                self_segment         = 1 << 4,
                other_segment        = 1 << 6,
                both_segments        = self_segment | other_segment,
                coincides            = 1 << 8,
                parallel             = 1 << 12,
                intersects           = 1 << 14,
                contains_segment     = 1 << 16,
                contained_by_segment = 1 << 18,
            };
        }
    }

    namespace flags
    {
        namespace line
        {
            enum : flags_t
            {
                inside_segment            = 1 << 0,
                check_coincidence         = 1 << 1,
                check_segment_coincidence = 1 << 2,
                check_parallelism         = 1 << 3,
            };
        }
    }

    /*****************************************************/
    /*                     line                          */
    /*****************************************************/

    template < typename _Point >
    struct line_base;

    using line = line_base < point2d_t > ;
    using line_view = line_base < const point2d_t & > ;

    template < typename _Point >
    struct line_base
    {
        using ptr_t = util::ptr_t < line_base > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        using point_t = _Point;

        point_t p1, p2;

        line_base(point_t p1, point_t p2)
            : p1(p1), p2(p2)
        {
        }

        line_base()
            : line_base(point_t{}, point_t{})
        {
        }

        template < typename _P2 >
        line_base(line_base < _P2 > const & o)
            : line_base(o.p1, o.p2)
        {
        }

        bool empty() const
        {
            return p1 == p2;
        }

        template < typename _P2 >
        bool operator == (line_base < _P2 > const & o) const
        {
            return (p1 == o.p1) && (p2 == o.p2);
        }

        template < typename _P2 >
        bool operator != (line_base < _P2 > const & o) const
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
            auto l = line_view(p, v);
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
            auto l = line_view(p, v);
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
            d1 = math::norm(inner_point(q1) - p);
            if (fuzzy_t::eq(0, d1))
                return math::fuzzy_values::zero_confidence(0.0);
            return math::fuzzy_values::negative_confidence(0.0);
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
            return fuzzy_t::eq(0, d) ? math::confidence::positive : math::confidence::negative;
        }

        math::confidence_t segment_contains(const point2d_t & p) const
        {
            auto d = segment_distance(p);
            if (d.confidence <= 0) return d.confidence;
            return fuzzy_t::eq(0, d) ? math::confidence::positive : math::confidence::negative;
        }

        /* calculates an intersection point;

           if `check_coincidence` flag set, checks
           if two lines coincide and sets `coincides`
           status bits if check succeeds; if the flag is
           not set, but lines coincide, sets `intersects`
           status bits unless lines are exactly parallel
           or exactly coincide;

           if `inside_segment` flag set and line
           intersection detected, sets `self_segment` and/or
           `other_segment` bits if intersection point is
           inside this line segment and/or the given line
           segment; the segment intersection is not trusted
           if the intersection point is too close to one
           of the segment boundary points;

           implementation notes:
                `check_parallelism` flag
                    not implemented
                `check_segment_coincidence`
                    not implemented

           intersection point = inner_point(r1)
                              = l.inner_point(r2)

           r1, r2 - any numbers
         */
        template < typename _P2 >
        status_t intersection(const line_base < _P2 > & l,
                              double & r1,
                              double & r2,
                              flags_t flg = 0) const
        {
            status_t r = status::trusted(status::ok);

            /* parallelism check is not supported now */
            if (flags::has_any(flg, flags::line::check_coincidence))
            {
                if ((contains(l.p1) >= 0) && (contains(l.p2) >= 0))
                {
                    r |= status::trusted(status::line::coincides);
                    /* segment coincidence check is not supported now */
                    return r;
                }
            }

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

                r |= status::trusted(status::line::intersects);

                if (flags::has_none(flg, flags::line::inside_segment))
                    return r;

                double d1 = length(), d2 = l.length();
                double s1 = d1 * min(q1, 1 - q1),
                       s2 = d2 * min(q2, 1 - q2);

                auto i1 = fuzzy_t::greater(s1, 0),
                     i2 = fuzzy_t::greater(s2, 0);

                if (i1 == 0)
                    r |= status::line::self_segment;
                else if (i1 > 0)
                    r |= status::trusted(status::line::self_segment);

                if (i2 == 0)
                    r |= status::line::other_segment;
                else if (i2 > 0)
                    r |= status::trusted(status::line::other_segment);
            }

            return r;
        }

        /* adds `inside_segment` and `check_segment_coincidence`
           flags to the `intersection` function */
        template < typename _P2 >
        status_t segment_intersection(const line_base < _P2 > & l,
                                      double & r1,
                                      double & r2,
                                      flags_t flg = 0) const
        {
            return intersection(l, r1, r2, flg |
                                flags::line::inside_segment |
                                flags::line::check_segment_coincidence);
        }

        template < typename _P2 >
        status_t intersects(const line_base < _P2 > & l, flags_t flg = 0) const
        {
            double q1, q2;
            return intersection(l, q1, q2, flg);
        }

        template < typename _P2 >
        status_t segment_intersects(const line_base < _P2 > & l, flags_t flg = 0) const
        {
            double q1, q2;
            return segment_intersection(l, q1, q2, flg);
        }

        /* calculates convexity type of the triangle
           given p1, p2 of this line and the given p3;

           returns convex_type::degenerate if (p1, p2, p3)
           is not triangle (pi ~= pj or pk ~lies on the
           pi-pj line);
         */
        convex_type convexity(const point2d_t & p3) const
        {
            if ((contains(p3) >= 0) || fuzzy_t::eq(0, geom::distance(p1, p2)))
            {
                return convex_type::degenerate;
            }
            double n = ((p1.x - p3.x) * (p2.y - p3.y) -
                        (p2.x - p3.x) * (p1.y - p3.y));
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

    template < typename _P1, typename _P2 >
    inline line_view make_line_view(_P1 & p1, _P2 & p2)
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
        return make_line_view(p1, p2).convexity(p3);
    }

    inline double angle(const point2d_t & p1,
                        const point2d_t & p2)
    {
        return make_line_view(p1, p2).angle();
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

    template < typename _P2 > inline line rotate
    (
        const line_base < _P2 > & geometry,
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

    template < typename _P2 > inline line scale
    (
        const line_base < _P2 > & geometry,
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

    template < typename _P2 > inline line move
    (
        const line_base < _P2 > & geometry,
        point2d_t value
    )
    {
        return
        {
            move(geometry.p1, value),
            move(geometry.p2, value)
        };
    }

    template < typename _P2 > inline point2d_t center
    (
        const line_base < _P2 > & geometry
    )
    {
        return geometry.inner_point(0.5);
    }

    template < typename _P1, typename _P2 > inline bool intersects
    (
        const line_base < _P1 > & l1, const line_base < _P2 > & l2
    )
    {
        return status::is_trusted(l1.segment_intersects(l2),
                                  status::line::both_segments);
    }

    template < typename _P2 > inline bool contains
    (
        const line_base < _P2 > & l, const point2d_t & p
    )
    {
        return l.contains(p) > 0;
    }

    /*****************************************************/
    /*                formatting                         */
    /*****************************************************/

    template <  typename _P2, typename _Elem, typename _Traits >
    std::basic_ostream < _Elem, _Traits > & operator <<
    (
        std::basic_ostream < _Elem, _Traits > & os,
        const line_base < _P2 > & l
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

    template< typename _P2 >inline double norm(geom::line_base < _P2 > const & l)
    {
        return distance(l.p1, l.p2);
    }

    template< typename _P2 >inline double sqnorm(geom::line_base < _P2 > const & l)
    {
        return sqdistance(l.p1, l.p2);
    }
}

template < typename _P2 > inline double geom::line_base < _P2 > ::length() const
{
    return math::norm(*this);
}
