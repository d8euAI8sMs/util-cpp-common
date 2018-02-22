#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom_fwd.h>
#include <util/common/geom/point.h>
#include <util/common/geom/line.h>

#include <type_traits>
#include <iostream>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

namespace geom
{

    /*****************************************************/
    /*             status codes and flags                */
    /*****************************************************/

    namespace status
    {
        namespace polygon
        {
            enum : status_t
            {
                contains_point          = 1 << 2,
                edge_contains_point     = 1 << 4,
                intersects              = 1 << 6,
                contains_line           = 1 << 8,
                coincides_with_line     = 1 << 10,
                contains_polygon        = 1 << 12,
                coincides_with_polygon  = 1 << 14,
            };
        }
    }

    namespace flags
    {
        namespace polygon
        {
            enum : flags_t
            {
                edge_intersection_only = 1 << 0,
            };
        }
    }

    /*****************************************************/
    /*                     polygon                       */
    /*****************************************************/

    /* forward declaration */
    template < typename _C >
    inline void clockwise_sort(_C & c,
                               bool counterclockwise = true,
                               bool keep_first = true);

    /**
     * a polygon abstraction;
     *
     * polygon is valid if it has >= 3 vertices;
     * however, no internal validation is performed;
     */
    template < typename C = std::vector < point2d_t > >
    struct polygon
    {
        using ptr_t = util::ptr_t < polygon > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        using container_type = C;

        container_type points;

        polygon(const container_type & c)
            : points(c)
        {
        }

        template < typename _C >
        polygon(const _C & c)
            : points(std::begin(c), std::end(c))
        {
        }

        polygon()
        {
        }

        template < typename _C >
        polygon(polygon < _C > const & o)
            : polygon(o.points)
        {
        }

        bool empty() const
        {
            return points.empty();
        }

        template < typename _C >
        bool operator == (polygon < _C > const & o) const
        {
            if (points.size() != o.points.size()) return false;
            for (size_t i = 0; i < points.size(); ++i)
            {
                if (points[i] != o.points[i]) return false;
            }
            return true;
        }

        template < typename _C >
        bool operator != (polygon < _C > const & o) const
        {
            if (points.size() != o.points.size()) return true;
            for (size_t i = 0; i < points.size(); ++i)
            {
                if (points[i] != o.points[i]) return true;
            }
            return false;
        }

        /**
         * returns all intersection points with the given
         * line segment;
         *
         * if number of vertices in this polygon is < 3,
         * returns empty vector;
         *
         * does not report special intersection points
         * (polygon vertices);
         */
        virtual std::vector < point2d_t >
        intersections(const line & l) const
        {
            std::vector < point2d_t > v;

            if (points.size() < 3) return v;

            double q1, q2;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                if (l.segment_intersection(line(points[i], points[j]), q1, q2))
                {
                    v.push_back(l.inner_point(q1));
                }
            }

            return v;
        }

        /**
         * checks for intersection with the given
         * line segment;
         *
         * if number of vertices in this polygon is < 3,
         * returns false;
         *
         * the function also checks if one point of line
         * is in and other is out of this polygon; it may
         * be helpful in case if intersection point is
         * very close to polygon vertex (special
         * intersection case);
         */
        virtual math::confidence_t intersects(const line & l) const
        {
            if (points.size() < 3) return math::confidence::negative;

            double q1, q2;

            std::vector < double > possible_intersection_points;
            bool possible_edge_coincide = false;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                auto l0 = line(points[i], points[j]);
                auto it = l.segment_intersection(l0, q1, q2);
                if (it > 0)
                    return math::confidence::positive;
                else if (it == 0)
                {
                    /* no checking if lines are parallel since
                       if it is the case, segment_intersection
                       will return -1 */
                    possible_intersection_points.push_back(q1);
                }
                else if (it < 0)
                {
                    /* due to previous comment, if points lie on
                       an edge of a polygon and no other intersection
                       point found, an algorithm should report 0 */
                    if ((l0.contains(l.p1) >= 0) && (l0.contains(l.p2) >= 0))
                    {
                        possible_edge_coincide = true;
                    }
                }
            }

            auto c1 = contains(l.p1),
                 c2 = contains(l.p2);

            bool possible_contains_line = false;

            if (c1 * c2 < 0)
                return math::confidence::positive;
            if ((c1 > 0) && (c2 > 0))
                return math::confidence::negative;
            if ((c1 <= 0) && (c2 <= 0))
            {
                if (!possible_intersection_points.empty())
                {
                    auto s = l.length();
                    for(size_t i = 0; i < possible_intersection_points.size(); ++i)
                    {
                        auto q = possible_intersection_points[i];
                        /* skip l.p1 or l.p2 as an algorithm may
                           report incorrect results */
                        q1 = ((q * s) - 2 * fuzzy_t::traits::tolerance()) / s;
                        q2 = ((q * s) + 2 * fuzzy_t::traits::tolerance()) / s;
                        auto c3 = contains(l.inner_point(q1));
                        if (c3 > 0)
                        {
                            if (fuzzy_t::eq(0, q * s))
                                return math::confidence::zero;
                            if ((c1 == 0) && (c2 == 0))
                                possible_contains_line = true;
                            return math::confidence::positive;
                        }
                        auto c4 = contains(l.inner_point(q2));
                        if (c4 > 0)
                        {
                            if (fuzzy_t::eq(s, q * s))
                                return math::confidence::zero;
                            if ((c1 == 0) && (c2 == 0))
                                possible_contains_line = true;
                            return math::confidence::positive;
                        }
                    }
                }
                if (possible_contains_line)
                    return math::confidence::zero;
                if (possible_edge_coincide)
                    return math::confidence::zero;
            }

            return math::confidence::negative;
        }

        /**
         * checks for intersection with the given
         * polygon;
         *
         * if number of vertices in this polygon is < 3,
         * returns false;
         *
         * the function also checks if some points of polygon
         * are in and some are out of this polygon; it may
         * be helpful in case if intersection point is
         * very close to polygon vertex (special
         * intersection case);
         */
        template < typename _C >
        bool intersects(const polygon < _C > & p) const
        {
            if (points.size() < 3) return false;

            bool contains_point = false;
            bool contains_unknown = true;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                auto l = line(points[i], points[j]);
                if (p.intersects(l))
                {
                    return true;
                }
                bool contains_new = p._contains_any(l);
                if (!contains_unknown && (contains_new != contains_point))
                {
                    return true;
                }
                contains_point = contains_new;
                contains_unknown = false;
            }

            return false;
        }

        /**
         * calculates the polygon convexity;
         *
         * returns convex_type::clockwise or
         * convex_type::counterclockwise for convex
         * polygon, convex_type::degenerate for
         * degenerate polygon, convex_type::no otherwise;
         *
         * examples of degenerate polygons:
         *    - n < 3
         *    - all points ~lay on one line
         *
         * if some adjacent points ~lay on one line, the
         * polygon is still not degenerate unless all its
         * points ~lay on that line;
         */
        virtual convex_type convexity() const
        {
            if (points.size() < 3) return convex_type::degenerate;

            bool clockwise = false;
            bool clockwise_selected = false;

            for (size_t i = 0, j = 1, k = 2; i < points.size(); i = j, j = k, ++k)
            {
                for(;;)
                {
                    if (k == points.size()) k = 0;
                    if (k == i) return convex_type::degenerate;

                    auto c = geom::convexity(points[i], points[j], points[k]);

                    if (c == convex_type::degenerate)
                    {
                        j = k;
                        ++k;
                    }
                    else
                    {
                        bool new_clockwise = (c == convex_type::clockwise);
                        if (clockwise_selected && (new_clockwise != clockwise))
                        {
                            return convex_type::no;
                        }
                        clockwise = new_clockwise;
                        clockwise_selected = true;
                        break;
                    }
                }
                if (j < i) break;
            }

            if (!clockwise_selected) return convex_type::degenerate;

            return clockwise ? convex_type::clockwise :
                               convex_type::counterclockwise;
        }

        bool is_convex() const
        {
            auto c = convexity();
            return (c == convex_type::clockwise) || (c == convex_type::counterclockwise);
        }

        /**
         * sorts polygon vertices clockwise or
         * counterclockwise;
         *
         * does not touch the first point
         * if keep_first = true;
         *
         * the function does not test if the polygon
         * is convex;
         */
        virtual void sort(bool counterclockwise = true,
                          bool keep_first = true)
        {
            clockwise_sort(points,
                           counterclockwise,
                           keep_first);
        }

        /**
         * tests if this polygon contains the given
         * point;
         *
         * implemented for generic concave polygon without
         * self intersections;
         *
         * statuses:
         *      `edge_contains_point` - if point is on edge
         *          trusted if edge represented by the line
         *          segment surely contains the point
         *      `contains_point` - if point is on edge or
         *          inside the polygon. trusted if the point
         *          is exactly inside
         *
         */
        virtual status_t contains(const point2d_t & p, flags_t flg = 0) const
        {
            if (points.size() < 3) return 0;

            status_t r = status::trusted(status::ok);

            double min_x, max_x, min_y, max_y;
            double d;

            min_x = max_x = points[0].x;
            min_y = max_y = points[0].y;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                auto sc = line(points[i], points[j]).segment_contains(p);
                if (sc >= 0)
                    r |= status::polygon::contains_point;
                if (sc > 0)
                    return r | status::trusted(status::polygon::edge_contains_point);
                if (sc == 0)
                    return r | status::polygon::edge_contains_point;
                if (points[i].x < min_x) min_x = points[i].x;
                if (points[i].y < min_y) min_y = points[i].y;
                if (points[i].x > max_x) max_x = points[i].x;
                if (points[i].y > max_y) max_y = points[i].y;
            }

            if ((p.x < min_x) || (p.x > max_x) || (p.y < min_y) || (p.y > max_y))
                return r;

            d = 2 * (max_x + max_y - min_x - min_y);

            double q1, q2;

            point2d_t pivot;

            for (;;)
            {
                pivot.x = p.x + ((rand() % 2) * 2 - 1) * (1 + rand() / (RAND_MAX + 1.)) * d;
                pivot.y = p.y + ((rand() % 2) * 2 - 1) * (1 + rand() / (RAND_MAX + 1.)) * d;

                size_t c = 0;
                size_t i, j;

                for (i = 0, j = 1; i < points.size(); ++i, ++j)
                {
                    if (j == points.size()) j = 0;
                    auto has_int = status::get(
                        line(points[i], points[j]).segment_intersects({ p, pivot }),
                        status::line::intersects | status::line::both_segments);
                    if (has_int == 0)
                        break;
                    if (has_int > 0)
                        ++c;
                }

                if (i != points.size()) continue;

                return ((c % 2) == 1) ? r | status::trusted(status::polygon::contains_point)
                                      : r;
            }
        }

    private:

        /**
         * checks if any point of the given line is
         * contained by the polygon;
         *
         * checks points: q=0, q=0.45, q=0.5, q=0.55
         */
        bool _contains_any(const line & l) const
        {
            return (contains(l.p1) ||
                    contains(l.inner_point(0.45)) ||
                    contains(l.inner_point(0.5))  ||
                    contains(l.inner_point(0.55)));
        }
    };

    /*****************************************************/
    /*                factory functions                  */
    /*****************************************************/

    template < typename _C, typename _R = polygon < _C > >
    inline _R make_polygon(_C const & o)
    {
        return _R(o);
    }

    /*****************************************************/
    /*                helper functions                   */
    /*****************************************************/

    template < typename _C >
    inline bool is_convex(const _C & c)
    {
        return make_polygon(c).is_convex();
    }

    template < typename _C >
    inline void clockwise_sort(_C & c,
                               bool counterclockwise,
                               bool keep_first)
    {
        if (c.empty()) return;
        std::vector < std::pair < double, point2d_t > >
            angles(c.size());
        point2d_t p0;
        for (size_t i = 0; i < c.size(); ++i)
        {
            p0 += c[i];
        }
        p0 = p0 / c.size();
        auto a0 = angle(p0, c[0]);
        for (size_t i = 0; i < c.size(); ++i)
        {
            angles[i] = { angle(p0, c[i]), c[i] };
            if (keep_first)
            {
                angles[i].first -= a0;
                if (angles[i].first < 0)
                    angles[i].first += 2 * M_PI;
            }
            if (!counterclockwise)
            {
                angles[i].first = - angles[i].first;
            }
            if (keep_first && (angles[i].first < 0))
            {
                angles[i].first += 2 * M_PI;
            }
        }
        auto cmp = [&counterclockwise]
        (const std::pair < double, point2d_t > & p1,
         const std::pair < double, point2d_t > & p2)
        {
            return p1.first < p2.first;
        };
        std::sort(angles.begin(), angles.end(), cmp);
        for (size_t i = 0; i < c.size(); ++i)
        {
            c[i] = angles[i].second;
        }
    }

    /*****************************************************/
    /*                type traits                        */
    /*****************************************************/

    template < typename _C > struct is_planar < polygon < _C > >
        : std::integral_constant < bool, true >
    {
    };

    /*****************************************************/
    /*                geometry operations                */
    /*****************************************************/

    template < typename _C, typename _R = polygon < _C > > inline _R rotate
    (
        const polygon < _C > & geometry,
        double radians,
        point2d_t at = {}
    )
    {
        _R g = geometry;
        for (size_t i = 0; i < g.points.size(); ++i)
        {
            g.points[i] = rotate(g.points[i], radians, at);
        }
        return g;
    }

    template < typename _C, typename _R = polygon < _C > > inline _R scale
    (
        const polygon < _C > & geometry,
        double n,
        point2d_t at = {}
    )
    {
        _R g = geometry;
        for (size_t i = 0; i < g.points.size(); ++i)
        {
            g.points[i] = scale(g.points[i], n, at);
        }
        return g;
    }

    template < typename _C, typename _R = polygon < _C > > inline _R move
    (
        const polygon < _C > & geometry,
        point2d_t value
    )
    {
        _R g = geometry;
        for (size_t i = 0; i < g.points.size(); ++i)
        {
            g.points[i] = move(g.points[i], value);
        }
        return g;
    }

    template < typename _C > inline point2d_t center
    (
        const polygon < _C > & geometry
    )
    {
        size_t n;
        point2d_t c = {};
        auto & g = geometry;
        for (n = 0; n < g.points.size(); ++n)
        {
            c += g.points[n];
        }
        return (n == 0) ? c : (c / n);
    }

    template < typename _C, typename _D >
    inline bool intersects
    (
        const polygon < _C > & p1,
        const polygon < _D > & p2
    )
    {
        return p1.intersects(p2);
    }

    template < typename _C >
    inline bool intersects
    (
        const polygon < _C > & p, const line & l
    )
    {
        return p.intersects(l);
    }

    template < typename _C >
    inline bool intersects
    (
        const line & l, const polygon < _C > & p
    )
    {
        return p.intersects(l);
    }

    template < typename _C >
    inline bool contains
    (
        const polygon < _C > & g, const point2d_t & p
    )
    {
        return g.contains(p);
    }

    /*****************************************************/
    /*                formatting                         */
    /*****************************************************/

    template < typename _Elem, typename _Traits, typename _C >
    std::basic_ostream < _Elem, _Traits > & operator <<
    (
        std::basic_ostream < _Elem, _Traits > & os,
        const polygon < _C > & p
    )
    {
        os << "{ ";
        for (size_t i = 0; i < g.points.size(); ++i)
        {
            os << g.points[i];
            if (i != (points.size() - 1))
            {
                os << ", ";
            }
        }
        return os << " }";
    }
}
