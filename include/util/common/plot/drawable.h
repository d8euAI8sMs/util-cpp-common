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

        bool visible;

    public:

        drawable()
            : visible(true)
        {
        }

    public:

        virtual void draw(CDC &dc, const viewport &bounds)
        {
            if (!visible) return;
            do_draw(dc, bounds);
        }

        virtual void draw_at(CDC &dc, const viewport &bounds, const point < double > origin)
        {
            point < int > origin_at_screen = bounds.world_to_screen().xy(origin);
            /* don't change scale and screen size, only position */
            viewport other =
            {
                {
                    origin_at_screen.x,
                    origin_at_screen.x + bounds.screen.width(),
                    origin_at_screen.y,
                    origin_at_screen.y + bounds.screen.height(),
                },
                bounds.world
            };
            draw(dc, other);
        }

        virtual ~drawable()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) = 0;
    };
}
