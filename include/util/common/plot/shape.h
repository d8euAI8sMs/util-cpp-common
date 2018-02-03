#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>
#include <util/common/geom/point.h>

namespace plot
{

    /*****************************************************/
    /*                     point                         */
    /*****************************************************/

    /**
     *  add point type to `plot` namespace to allow using
     *  convenient name `plot::point` instead of new
     * `geom::point`
     */
    using geom::point;

    /*****************************************************/
    /*                     rect                          */
    /*****************************************************/

    template
    <
        typename N
    >
    struct rect
    {
        using ptr_t = util::ptr_t < rect > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        using value_type = N;

        N xmin, xmax, ymin, ymax;

        rect(N xmin, N xmax, N ymin, N ymax)
            : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax)
        {
        }

        rect()
            : rect(0, 0, 0, 0)
        {
        }

        N width() const
        {
            return xmax - xmin;
        }

        N height() const
        {
            return ymax - ymin;
        }

        bool empty() const
        {
            return ((xmin == 0) && (xmax == 0) && (ymin == 0) && (ymax == 0));
        }

        operator RECT () const
        {
            return {
                xmin, ymin, xmax, ymax
            };
        }

        rect<N> & operator=(const rect<N> &other)
        {
            this->xmin = other.xmin;
            this->xmax = other.xmax;
            this->ymin = other.ymin;
            this->ymax = other.ymax;
            return *this;
        }
    };
}
