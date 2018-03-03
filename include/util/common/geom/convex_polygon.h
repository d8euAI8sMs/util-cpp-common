#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/math/scalar.h>
#include <util/common/geom/geom_fwd.h>
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

        convex_polygon(container_type && c)
            : polygon(std::move(c))
        {
        }

        convex_polygon()
        {
        }

        convex_polygon(polygon const & o)
            : polygon(o.points)
        {
        }

        convex_polygon(polygon && o)
            : polygon(std::move(o))
        {
        }

        convex_polygon & operator = (convex_polygon const & o)
        {
            invalidate();
            points = o.points;
            return *this;
        }

        convex_polygon & operator = (convex_polygon && o)
        {
            invalidate();
            o.invalidate();
            points = std::move(o.points);
            return *this;
        }

        /**
         * calculates the polygon convexity;
         *
         * convexity checking is simplified to just
         * checking the convexity of the first
         * non-degenerate triangle;
         */
        convex_type convexity() const
        {
            if (points.size() < 3) return convex_type::degenerate;

            size_t j = 1, k = 2;

            for(;;)
            {
                if (k == points.size()) return convex_type::degenerate;

                auto c = geom::convexity(points[0], points[j], points[k]);

                if (c == convex_type::degenerate)
                {
                    j = k;
                    ++k;
                }
                else
                {
                    return c;
                }
            }
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
}
