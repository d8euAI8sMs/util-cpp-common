#pragma once

#include <afxwin.h>

#include <functional>

#include <util/common/plot/ptr.h>
#include <util/common/plot/viewport.h>
#include <util/common/plot/shape.h>

namespace plot
{

    class drawable
    {

    public:

        using ptr_t = plot::ptr_t < drawable > ;

    public:

        virtual void draw(CDC &dc, const viewport &bounds) = 0;

        virtual void draw_at(CDC &dc, const viewport &bounds, const point < double > origin)
        {
            point < int > origin_at_screen = bounds.world_to_screen().xy(origin);
            dc.OffsetViewportOrg(origin_at_screen.x, origin_at_screen.y);
            draw(dc, bounds);
            dc.OffsetViewportOrg(-origin_at_screen.x, -origin_at_screen.y);
        }

        virtual ~drawable()
        {
        }
    };
}
