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

        std::vector < point2d_t >
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

        bool intersects(const line & l) const
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

        convex_type convexity() const
        {
            if (points.size() < 3) convex_type::no;

            bool clockwise = false;
            bool clockwise_selected = false;

            for (size_t i = 0, j = 1, k = 2; i < points.size(); ++i, ++j, ++k)
            {
                if (j == points.size()) j = 0;
                if (k == points.size()) k = 0;
                bool new_clockwise = is_clockwise(points[i], points[j], points[k]);
                if (clockwise_selected && (new_clockwise != clockwise))
                {
                    return convex_type::no;
                }
                clockwise = new_clockwise;
                clockwise_selected = true;
            }

            return clockwise ? convex_type::clockwise :
                               convex_type::counterclockwise;
        }

        bool is_convex() const
        {
            return convexity() != convex_type::no;
        }

        bool contains(const point2d_t & p) const
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
    /*                helper functions                   */
    /*****************************************************/

    template < typename _C >
    inline bool is_convex(const _C & c)
    {
        return make_polygon(c).is_convex();
    }

    /*****************************************************/
    /*                factory functions                  */
    /*****************************************************/

    template < typename _C >
    inline polygon < _C > make_polygon(_C const & o)
    {
        return polygon < _C > (o);
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

    template < typename _C > inline polygon < _C > rotate
    (
        const polygon < _C > & geometry,
        double radians,
        point2d_t at = {}
    )
    {
        auto g = geometry;
        for (size_t i = 0; i < g.points.size(); ++i)
        {
            g.points[i] = rotate(g.points[i], radians, at);
        }
        return g;
    }

    template < typename _C > inline polygon < _C > scale
    (
        const polygon < _C > & geometry,
        double n,
        point2d_t at = {}
    )
    {
        auto g = geometry;
        for (size_t i = 0; i < g.points.size(); ++i)
        {
            g.points[i] = scale(g.points[i], n, at);
        }
        return g;
    }

    template < typename _C > inline polygon < _C > move
    (
        const polygon < _C > & geometry,
        point2d_t value
    )
    {
        auto g = geometry;
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

namespace math
{

    /*****************************************************/
    /*            scalar line operations                 */
    /*****************************************************/

    template < typename _C > inline geom::polygon < _C > conjugate
    (
        const geom::polygon < _C > & g
    )
    {
        auto g = geometry;
        for (size_t i = 0; i < g.points.size(); ++i)
        {
            g.points[i] = conjugate(g.points[i]);
        }
        return g;
    }
}
