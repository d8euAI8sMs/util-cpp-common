#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/geom/geom.h>
#include <util/common/geom/point.h>

namespace geom
{

    /*****************************************************/
    /*                 circle                            */
    /*****************************************************/

    struct circle
    {
        using ptr_t = util::ptr_t < circle > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        point2d_t center;
        double radius;

        circle()
        {
        }

        circle(const point2d_t & c,
               double r)
               : center(c)
               , radius(r)
        {
        }

        double sqradius() const
        {
            return radius * radius;
        }

        bool contains(const point2d_t & p) const
        {
            return (sqdistance(p, center) < sqradius());
        }

        bool border_contains(const point2d_t & p) const
        {
            return std::abs(sqdistance(p, center) - sqradius())
                / sqradius() <= 1e-8;
        }

        bool inner_contains(const point2d_t & p) const
        {
            return contains(p) && !border_contains(p);
        }

        bool outer_contains(const point2d_t & p) const
        {
            return contains(p) || border_contains(p);
        }
    };

    /*****************************************************/
    /*                factory functions                  */
    /*****************************************************/

    inline circle make_circle(double x, double y, double r)
    {
        return circle(point2d_t(x, y), r);
    }

    /*****************************************************/
    /*                type traits                        */
    /*****************************************************/

    template <> struct is_planar < circle >
        : std::integral_constant < bool, true >
    {
    };

    /*****************************************************/
    /*                geometry operations                */
    /*****************************************************/

    template <> inline circle rotate
    (
        const circle & geometry,
        double radians,
        point2d_t at
    )
    {
        return geometry;
    }

    template <> inline circle scale
    (
        const circle & geometry,
        double n,
        point2d_t at
    )
    {
        return { scale(geometry.center, n, at), geometry.radius * n };
    }

    template <> inline circle move
    (
        const circle & geometry,
        point2d_t value
    )
    {
        return { move(geometry.center, value), geometry.radius };
    }

    inline bool contains
    (
        const circle & c, const point2d_t & p
    )
    {
        return c.contains(p);
    }
}
