#pragma once

#include <afxwin.h>

namespace plot
{

    /*****************************************************/
    /*                     point                         */
    /*****************************************************/

    template
    <
        typename N
    >
    struct point
    {
        using value_type = N;

        N x, y;

        point(N x, N y)
            : x(x), y(y)
        {
        }

        point()
            : point(0, 0)
        {
        }

        operator POINT () const
        {
            return{ x, y };
        }
    };

    /*****************************************************/
    /*                     rect                          */
    /*****************************************************/

    template
    <
        typename N
    >
    struct rect
    {
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

    using world_t  = rect < double > ;
    using screen_t = rect < int > ;
}
