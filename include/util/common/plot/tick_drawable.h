#pragma once

#include <util/common/plot/drawable.h>
#include <util/common/plot/layer_drawable.h>
#include <util/common/plot/tick_factory.h>
#include <util/common/plot/palette.h>

namespace plot
{
    class tick_drawable : public drawable
    {

        std::shared_ptr < tick_factory > xtf, ytf;

        drawable_ptr content;
        palette::pen_ptr pen;

    public:

        tick_drawable
        (
            drawable_ptr content,
            std::shared_ptr < tick_factory > xtf,
            std::shared_ptr < tick_factory > ytf,
            palette::pen_ptr pen = {}
        )
            : content(std::move(content))
            , xtf(std::move(xtf))
            , ytf(std::move(ytf))
            , pen(std::move(pen))
        {
        }

        virtual void draw(CDC &dc, const viewport &bounds) override
        {
            if (pen)
            {
                dc.SelectObject(pen.get());
            }

            std::shared_ptr < std::vector < tick_t > > x_ticks
                = xtf->produce(bounds);
            std::shared_ptr < std::vector < tick_t > > y_ticks
                = xtf->produce(bounds);

            CString str;
            CSize tbounds;
            int x_offset = -1;
            int y_offset = -1;

            screen_t new_screen = bounds.screen;

            for each (auto & tick in *y_ticks)
            {
                str = tick.second.c_str();
                tbounds = dc.GetTextExtent(str);
                x_offset = max(x_offset, tbounds.cx);
            }

            for each (auto & tick in *x_ticks)
            {
                str = tick.second.c_str();
                tbounds = dc.GetTextExtent(str);
                y_offset = max(y_offset, tbounds.cy);
            }

            new_screen.xmin += x_offset + 5;
            new_screen.ymax -= y_offset + 5;

            viewport new_bounds(new_screen, bounds.world);

            dc.MoveTo(new_screen.xmin, 0);
            dc.LineTo(new_screen.xmin, new_screen.ymax);
            dc.LineTo(new_screen.xmax, new_screen.ymax);
            for each (auto & tick in *x_ticks)
            {
                int x = new_bounds.world_to_screen().x(tick.first);
                str = tick.second.c_str();
                tbounds = dc.GetTextExtent(str);
                dc.MoveTo(x, new_screen.ymin);
                dc.LineTo(x, new_screen.ymax);
                dc.TextOut(x - tbounds.cx / 2, bounds.screen.ymax - y_offset - 2, str);
            }
            for each (auto & tick in *y_ticks)
            {
                int y = new_bounds.world_to_screen().y(tick.first);
                str = tick.second.c_str();
                tbounds = dc.GetTextExtent(str);
                dc.MoveTo(new_screen.xmin, y);
                dc.LineTo(new_screen.xmax, y);
                dc.TextOut(x_offset - tbounds.cx - 2, y - tbounds.cy / 2, str);
            }

            RECT clip = new_screen;
            dc.IntersectClipRect(&clip);
            content->draw(dc, new_bounds);
        }
    };
}
