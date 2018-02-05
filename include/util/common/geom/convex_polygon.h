#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom.h>
#include <util/common/geom/point.h>
#include <util/common/geom/line.h>
#include <util/common/geom/polygon.h>

#include <type_traits>
#include <vector>

namespace geom
{

    /*****************************************************/
    /*                 convex_polygon                    */
    /*****************************************************/

    /**
     * a convex polygon abstraction;
     * 
     * a convexity of the polygon MUST be guaranteed
     * by the USER!
     */
    template < typename C = std::vector < point2d_t > >
    struct convex_polygon : public polygon < C >
    {
        using ptr_t = util::ptr_t < convex_polygon > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        convex_polygon(const container_type & c)
            : polygon(c)
        {
        }

        template < typename _C >
        convex_polygon(const _C & c)
            : polygon(c)
        {
        }

        convex_polygon()
        {
        }

        template < typename _C >
        convex_polygon(polygon < _C > const & o)
            : polygon(o.points)
        {
        }

        convex_type convexity() const
        {
            if (points.size() < 3) return convex_type::no;

            return is_clockwise(points[0], points[1], points[2]) ?
                               convex_type::clockwise :
                               convex_type::counterclockwise;
        }
    };

    /*****************************************************/
    /*                factory functions                  */
    /*****************************************************/

    template < typename _C, typename _R = convex_polygon < _C > >
    inline _R make_convex_polygon(_C const & o)
    {
        return make_polygon < _C, _R > (o);
    }

    /*****************************************************/
    /*                type traits                        */
    /*****************************************************/

    template < typename _C > struct is_planar < convex_polygon < _C > >
        : std::integral_constant < bool, true >
    {
    };

    /*****************************************************/
    /*                geometry operations                */
    /*****************************************************/

    template < typename _C, typename _R = convex_polygon < _C > > inline _R rotate
    (
        const convex_polygon < _C > & geometry,
        double radians,
        point2d_t at = {}
    )
    {
        return rotate < _C, _R > (static_cast < const polygon < _C > & > (geometry), radians, at);
    }

    template < typename _C, typename _R = convex_polygon < _C > > inline _R scale
    (
        const convex_polygon < _C > & geometry,
        double n,
        point2d_t at = {}
    )
    {
        return scale < _C, _R > (static_cast < const polygon < _C > & > (geometry), n, at);
    }

    template < typename _C, typename _R = convex_polygon < _C > > inline _R move
    (
        const convex_polygon < _C > & geometry,
        point2d_t value
    )
    {
        return move < _C, _R > (static_cast < const polygon < _C > & > (geometry), value);
    }
}
