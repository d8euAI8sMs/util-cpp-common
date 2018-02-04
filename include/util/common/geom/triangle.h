#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom.h>
#include <util/common/geom/point.h>
#include <util/common/geom/line.h>
#include <util/common/geom/polygon.h>
#include <util/common/geom/convex_polygon.h>

#include <type_traits>
#include <array>

namespace geom
{

    /*****************************************************/
    /*                 triangle                          */
    /*****************************************************/

    struct triangle : public convex_polygon < std::array < point2d_t, 3 > >
    {
        using ptr_t = util::ptr_t < triangle > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        triangle(const container_type & c)
            : convex_polygon(c)
        {
        }

        template < typename _C >
        triangle(const _C & c)
        {
            if (c.size() <= 3)
            {
                for (size_t i = 0; i < c.size(); ++i)
                {
                    points[i] = c[i];
                }
            }
            else
            {
                throw std::domain_error(
                    "triangle cannot be created "
                    "from polygon with n > 3");
            }
        }

        triangle()
        {
        }

        template < typename _C >
        triangle(polygon < _C > const & o)
            : triangle(o.points)
        {
        }

        triangle(const point2d_t & p1,
                 const point2d_t & p2,
                 const point2d_t & p3)
        {
            points = { { p1, p2, p3 } };
        }
    };

    /*****************************************************/
    /*                factory functions                  */
    /*****************************************************/

    template < typename _C, typename _R = triangle >
    inline _R make_triangle(_C const & o)
    {
        return make_polygon < _C, _R > (o);
    }

    template < typename _P1, typename _P2, typename _P3 >
    inline triangle make_triangle(
        const _P1 & p1, const _P2 & p2, const _P3 & p3)
    {
        return { p1, p2, p3 };
    }

    template < typename _X1, typename _Y1,
               typename _X2, typename _Y2,
               typename _X3, typename _Y3 >
    inline triangle make_triangle(
        _X1 x1, _Y1 y1, _X2 x2, _Y2 y2, _X3 x3, _Y3 y3)
    {
        return { point2d_t(x1, y1), point2d_t(x2, y2), point2d_t(x3, y3) };
    }

    /*****************************************************/
    /*                type traits                        */
    /*****************************************************/

    template <> struct is_planar < triangle >
        : std::integral_constant < bool, true >
    {
    };

    /*****************************************************/
    /*                geometry operations                */
    /*****************************************************/

    template < typename _R = triangle > inline _R rotate
    (
        const triangle & geometry,
        double radians,
        point2d_t at = {}
    )
    {
        return rotate < triangle::container_type, _R > (geometry, radians, at);
    }

    template < typename _R = triangle > inline _R scale
    (
        const triangle & geometry,
        double n,
        point2d_t at = {}
    )
    {
        return scale < triangle::container_type, _R > (geometry, n, at);
    }

    template < typename _R = triangle > inline _R move
    (
        const triangle & geometry,
        point2d_t value
    )
    {
        return move < triangle::container_type, _R > (geometry, value);
    }
}
