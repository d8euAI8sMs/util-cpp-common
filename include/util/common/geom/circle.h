#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/geom/geom_fwd.h>
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
        double sqradius;

    private:

        mutable double _radius;

    public:

        circle()
            : sqradius(0)
            , _radius(0)
        {
        }

        circle(const point2d_t & c,
               double sqr)
               : center(c)
               , sqradius(sqr)
               , _radius(0)
        {
        }

        double radius() const
        {
            if (_radius != 0) return _radius;
            return _radius = std::sqrt(sqradius);
        }

        math::confidence_t contains(const point2d_t & p) const
        {
            return fuzzy_t::less(sqdistance(p, center), sqradius);
        }

        void invalidate()
        {
            _radius = 0;
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
        return { scale(geometry.center, n, at), geometry.sqradius * n * n };
    }

    template <> inline circle move
    (
        const circle & geometry,
        point2d_t value
    )
    {
        return { move(geometry.center, value), geometry.sqradius };
    }

    inline bool contains
    (
        const circle & c, const point2d_t & p
    )
    {
        return c.contains(p) > 0;
    }
}
