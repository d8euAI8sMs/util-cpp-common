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
                edge_contains_point_2   = 1 << 16,
                edge_contains_both      = edge_contains_point
                                        | edge_contains_point_2,
                touches_line            = 1 << 18,
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

    private:

        mutable std::vector < line_view > _lines;
        mutable point2d_t _extent_min;
        mutable point2d_t _extent_max;

        template < typename _C > friend struct polygon;

    public:

        polygon(const container_type & c)
            : points(c)
        {
        }

        polygon(container_type && c)
            : points(std::move(c))
        {
        }

        polygon()
        {
        }

        polygon(const polygon & o)
            : polygon(o.points)
        {
        }

        polygon(polygon && o)
            : polygon(std::move(o.points))
        {
            o.invalidate();
        }

        polygon & operator = (const polygon & o)
        {
            invalidate();
            points = o.points;
            return *this;
        }

        polygon & operator = (polygon && o)
        {
            invalidate();
            o.invalidate();
            points = std::move(o.points);
            return *this;
        }

        void invalidate()
        {
            _lines.clear();
            _extent_min = {};
            _extent_max = {};
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
        template < typename _P2 >
        std::vector < point2d_t >
        intersections(const line_base < _P2 > & l) const
        {
            std::vector < point2d_t > v;

            if (points.size() < 3) return v;

            double q1, q2;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                if (i == _lines.size()) _lines.emplace_back(points[i], points[j]);
                if (status::is(l.segment_intersection(_lines[i], q1, q2),
                    status::line::intersects | status::line::both_segments))
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
         * returns 0;
         *
         * implemented for generic concave polygon without
         * self intersections;
         *
         * statuses:
         *      `intersects` - if this polygon is intersected
         *          by the given line
         *      `contains_line` - if the line is inside
         *          the polygon (trusted) or is contained
         *          by the polygon edge(untrusted)
         *      `touches_line` - if the line starts or ends (or both)
         *          at the polygon edge but does not coincide
         *          or intersect with the polygon
         *      `edge_contains_point` - if the polygon edge
         *          contains the starting point of the line.
         *          trust level exactly matches the trust
         *          level of contains(l.p1)
         *      `edge_contains_point_2` - same as previous
         *          but for the l.p2 point
         *      `coincides_with_line` - if the given line
         *          coincides with the polygon but does not
         *          intersect it. may also be set if the
         *          starting (or end) point of the passed line
         *          is the starting (or end) point of the
         *          polygon edge and these lines coincide
         *
         * implementation notes:
         *      flag `edge_intersection_only` is not implemented
         *      currently if the intersection status is set,
         *          no further checks are performed
         */
        template < typename _P2 >
        status_t intersects(const line_base < _P2 > & l, flags_t flg = 0) const
        {
            if (points.size() < 3) return 0;

            status_t r = trusted(status::ok);

            double q1, q2;

            std::vector < double > possible_intersection_points;
            bool possible_edge_coincide = false;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                if (i == _lines.size()) _lines.emplace_back(points[i], points[j]);
                auto &l0 = _lines[i];
                auto it = l.segment_intersection(l0, q1, q2);
                if (status::is_trusted(it,
                        status::line::intersects | status::line::both_segments))
                    return r | status::trusted(status::polygon::intersects);
                else if (status::is(it, status::line::both_segments))
                {
                    /* no checking if lines are parallel since
                       if it is the case, segment_intersection
                       will return -1 */
                    possible_intersection_points.push_back(q1);
                }
                else if (status::is_not(it, status::line::both_segments))
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

            if (status::is(c1, status::polygon::edge_contains_point))
                r |= c1 & status::trusted(status::polygon::edge_contains_point);
            if (status::is(c2, status::polygon::edge_contains_point))
            {
                auto t = status::get(c2, status::polygon::edge_contains_point);
                r |= ((t > 0) ? status::trusted(status::polygon::edge_contains_point_2)
                              : status::polygon::edge_contains_point_2);
            }

            bool possible_contains_line = false;
            bool possible_contains_line_trust = false;
            bool possible_touches_line = false;

            if (status::is_trusted(c1, status::polygon::contains_point) &&
                status::is_not(c2, status::polygon::contains_point) ||
                status::is_trusted(c2, status::polygon::contains_point) &&
                status::is_not(c1, status::polygon::contains_point))
                return r | status::trusted(status::polygon::intersects);
            if ((status::get(c1, status::polygon::contains_point) >= 0) &&
                (status::get(c2, status::polygon::contains_point) > 0) ||
                (status::get(c1, status::polygon::contains_point) > 0) &&
                (status::get(c2, status::polygon::contains_point) >= 0))
                return r | status::trusted(status::polygon::contains_line);
            if ((status::get(c1, status::polygon::contains_point) <= 0) &&
                (status::get(c2, status::polygon::contains_point) <= 0))
            {
                if (!possible_intersection_points.empty())
                {
                    auto s = l.sqlength();
                    for(size_t i = 0; i < possible_intersection_points.size(); ++i)
                    {
                        auto q = possible_intersection_points[i];
                        /* skip l.p1 or l.p2 as an algorithm may
                           report incorrect results */
                        auto p0 = l.inner_point(q);
                        auto p1 = p0 - point2d_t{ (l.p2.x - l.p1.x) * 2 * sqrt_tolerance(),
                                         (l.p2.y - l.p1.y) * 2 * sqrt_tolerance() };
                        auto p2 = p0 + point2d_t{ (l.p2.x - l.p1.x) * 2 * sqrt_tolerance(),
                                         (l.p2.y - l.p1.y) * 2 * sqrt_tolerance() };
                        auto c3 = contains(p1);
                        if (status::is(c3, status::polygon::contains_point) &&
                            fuzzy_t::eq(0, q * q * s))
                        {
                            possible_touches_line = true;
                        }
                        else if (status::is(c3, status::polygon::contains_point) &&
                                 status::is(r, status::polygon::edge_contains_both))
                        {
                            possible_contains_line = true;
                            if (status::is_trusted(c3, status::polygon::contains_point))
                            {
                                possible_contains_line_trust = true;
                            }
                        }
                        else if (status::is_trusted(c3, status::polygon::contains_point)
                                 && !status::is(r, status::polygon::edge_contains_both))
                        {
                            return r | status::trusted(status::polygon::intersects);
                        }

                        auto c4 = contains(p2);
                        if (status::is(c4, status::polygon::contains_point) &&
                            fuzzy_t::eq(s, q * q * s))
                        {
                            possible_touches_line = true;
                        }
                        else if (status::is(c4, status::polygon::contains_point) &&
                                 status::is(r, status::polygon::edge_contains_both))
                        {
                            possible_contains_line = true;
                            if (status::is_trusted(c4, status::polygon::contains_point))
                            {
                                possible_contains_line_trust = true;
                            }
                        }
                        else if (status::is_trusted(c4, status::polygon::contains_point)
                                 && !status::is(r, status::polygon::edge_contains_both))
                        {
                            return r | status::trusted(status::polygon::intersects);
                        }
                    }
                }

                if (possible_contains_line && possible_contains_line_trust)
                    r |= status::trusted(status::polygon::contains_line);
                else if (possible_contains_line)
                    r |= status::polygon::contains_line;
                if (possible_touches_line)
                    r |= status::trusted(status::polygon::touches_line);
                if (possible_edge_coincide)
                {
                    r |= status::trusted(status::polygon::coincides_with_line);
                    if (status::is_not(r, status::polygon::touches_line))
                    {
                        r |= status::polygon::contains_line;
                    }
                }
            }

            return r;
        }

        /**
         * checks for intersection with the given
         * polygon;
         *
         * if number of vertices in this polygon is < 3,
         * returns 0;
         *
         * statuses:
         *      `intersects` - if this polygon is intersected
         *          by the given one
         *      `contains_polygon` - if the given polygon is
         *          exactly inside this one
         *      `coincides_with_polygon` - if the given polygon
         *          has lines that coincide with this one
         *
         * implementation notes:
         *      flag `edge_intersection_only` is not implemented
         *      currently if the intersection status is set,
         *          no further checks are performed
         *      see implementation notes to polygon-line
         *          intersection for more details
         */
        template < typename _C >
        status_t intersects(const polygon < _C > & p, flags_t flg = 0) const
        {
            if (points.size() < 3) return 0;

            status_t r = status::trusted(status::ok);

            bool has_outer = false;
            bool has_inner = false;
            bool has_coincidence = false;

            for (size_t i = 0, j = 1; i < p.points.size(); ++i, ++j)
            {
                if (j == p.points.size()) j = 0;
                if (i == p._lines.size()) p._lines.emplace_back(p.points[i], p.points[j]);
                auto &l = p._lines[i];
                auto has_int = intersects(l);
                if (status::is_trusted(has_int, status::polygon::intersects))
                    return r | status::trusted(status::polygon::intersects);
                if (status::is_trusted(has_int, status::polygon::coincides_with_line))
                    has_coincidence = true;
                if (status::is_trusted(has_int, status::polygon::contains_line))
                    has_inner = true;
                else if ((status::is_trusted(has_int, status::polygon::touches_line) ||
                          status::is_not(has_int, status::polygon::intersects)) &&
                         status::is_not(has_int, status::polygon::contains_line))
                    has_outer = true;
                if (has_inner && has_outer)
                {
                    return r | status::trusted(status::polygon::intersects);
                }
            }

            if (has_coincidence)
                r |= status::trusted(status::polygon::coincides_with_polygon);
            if (has_inner)
                r |= status::trusted(status::polygon::contains_polygon);

            return r;
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

            if (!(_extent_min.empty() && _extent_max.empty()))
            {
                if ((p.x < _extent_min.x) || (p.x > _extent_max.x) ||
                    (p.y < _extent_min.y) || (p.y > _extent_max.y))
                    return r;
            }

            min_x = max_x = points[0].x;
            min_y = max_y = points[0].y;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                if (i == _lines.size()) _lines.emplace_back(points[i], points[j]);
                auto sc = _lines[i].segment_contains(p);
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

            _extent_min = { min_x - 2 * sqrt_tolerance(), min_y - 2 * sqrt_tolerance() };
            _extent_max = { max_x + 2 * sqrt_tolerance(), max_y + 2 * sqrt_tolerance() };

            if ((p.x < min_x) || (p.x > max_x) || (p.y < min_y) || (p.y > max_y))
                return r;

            d = 2 * (max_x + max_y - min_x - min_y);

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
                    if (i == _lines.size()) _lines.emplace_back(points[i], points[j]);
                    auto has_int = status::get(
                        _lines[i].segment_intersects(make_line_view(p, pivot)),
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
