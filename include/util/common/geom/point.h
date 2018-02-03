#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>

namespace geom
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
        using ptr_t = util::ptr_t < point > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

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

        bool empty() const
        {
            return ((x == 0) && (y == 0));
        }

        template < typename _N >
        bool operator == (point < _N > const & o)
        {
            return (x == o.x) && (y == o.y);
        }

        template < typename _N >
        bool operator != (point < _N > const & o)
        {
            return (x != o.x) || (y != o.y);
        }
    };
}
