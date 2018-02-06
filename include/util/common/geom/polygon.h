#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom.h>
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
    /*                     polygon                       */
    /*****************************************************/

    enum class convex_type
    {
        no,
        counterclockwise,
        clockwise
    };

    /* forward declaration */
    template < typename _C >
    inline void clockwise_sort(_C & c, bool counterclockwise = true);

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
        virtual bool intersects(const line & l) const
        {
            if (points.size() < 3) return false;

            double q1, q2;

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                if (l.segment_intersection(line(points[i], points[j]), q1, q2))
                {
                    return true;
                }
            }

            return (contains(l.p1) ^ contains(l.p2));
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
                if (p.intersects(line(points[i], points[j])))
                {
                    return true;
                }
                bool contains_new = p.contains(points[i]);
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
         * does not touch the first point;
         *
         * the function does not test if the polygon
         * is convex;
         */
        virtual void sort(bool counterclockwise = true)
        {
            clockwise_sort(points, counterclockwise);
        }

        /**
         * tests if this polygon contains the given
         * point;
         *
         * implemented for only convex polygons;
         * throws exception if applied to non-convex polygon;
         */
        virtual bool contains(const point2d_t & p) const
        {
            if (points.size() < 3) return false;

            auto c = convexity();
            if (c == convex_type::no)
            {
                throw std::domain_error(
                    "only convex polygons"
                    "are supported now");
            }

            for (size_t i = 0, j = 1; i < points.size(); ++i, ++j)
            {
                if (j == points.size()) j = 0;
                bool clockwise = is_clockwise(points[i], points[j], p);
                if ((!clockwise && (c == convex_type::clockwise))
                 || (clockwise && (c == convex_type::counterclockwise)))
                {
                    return false;
                }
            }

            return true;
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
    inline void clockwise_sort(_C & c, bool counterclockwise)
    {
        if (c.empty()) return;
        std::vector < std::pair < double, point2d_t > >
            angles(points.size());
        auto & p0 = c[0];
        for (size_t i = 0; i < c.size(); ++i)
        {
            angles[i] = { angle(p0, c[i]), c[i] };
            if (!counterclockwise) angles[i].first = - angles[i].first;
            if (angles[i].first < 0) angles[i].first += 2 * M_PI;
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
