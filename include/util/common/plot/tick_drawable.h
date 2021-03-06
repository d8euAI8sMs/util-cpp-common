#pragma once

#include <util/common/ptr.h>
#include <util/common/plot/drawable.h>
#include <util/common/plot/layer_drawable.h>
#include <util/common/plot/tick_factory.h>
#include <util/common/plot/palette.h>

namespace plot
{
    class tick_drawable : public drawable
    {

    public:

        using ptr_t = util::ptr_t < tick_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        tick_factory::ptr_t xtf, ytf;

        drawable::ptr_t  content;
        palette::pen_ptr pen;
        COLORREF         text_color;
        palette::brush_ptr background;

    public:

        tick_drawable()
        {
        }

        tick_drawable
        (
            drawable::ptr_t content,
            tick_factory::ptr_t xtf,
            tick_factory::ptr_t ytf,
            palette::pen_ptr pen = {},
            COLORREF text_color = 0x000000,
            palette::brush_ptr background = {}
        )
            : content(std::move(content))
            , xtf(std::move(xtf))
            , ytf(std::move(ytf))
            , pen(std::move(pen))
            , text_color(text_color)
            , background(std::move(background))
        {
        }

        virtual ~tick_drawable()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) override
        {
            dc.SetTextColor(text_color);
            if (pen)
            {
                dc.SelectObject(pen.get());
            }
            if (background)
            {
                RECT screen = bounds.screen;
                dc.FillRect(&screen, background.get());
            }

            util::ptr_t < std::vector < tick_t > > x_ticks;
            if (xtf)
            {
                x_ticks = xtf->produce(bounds);
            }
            util::ptr_t < std::vector < tick_t > > y_ticks;
            if (ytf)
            {
                y_ticks = ytf->produce(bounds);
            }

            CString str;
            CSize tbounds;
            int x_offset = 0;
            int y_offset = 0;

            screen_t new_screen = bounds.screen;

            if (y_ticks)
            {
                for each (auto & tick in *y_ticks)
                {
                    str = tick.second.c_str();
                    tbounds = dc.GetTextExtent(str);
                    x_offset = max(x_offset, tbounds.cx);
                }
                new_screen.xmin += x_offset + 10;
            }

            if (x_ticks)
            {
                for each (auto & tick in *x_ticks)
                {
                    str = tick.second.c_str();
                    tbounds = dc.GetTextExtent(str);
                    y_offset = max(y_offset, tbounds.cy);
                }
                new_screen.ymax -= y_offset + 5;
            }


            viewport new_bounds(new_screen, bounds.world);

            dc.MoveTo(new_screen.xmin, 0);
            dc.LineTo(new_screen.xmin, new_screen.ymax);
            dc.LineTo(new_screen.xmax, new_screen.ymax);

            if (x_ticks)
            {
                for each (auto & tick in *x_ticks)
                {
                    int x = new_bounds.world_to_screen().x(tick.first);
                    str = tick.second.c_str();
                    tbounds = dc.GetTextExtent(str);
                    dc.MoveTo(x, new_screen.ymin);
                    dc.LineTo(x, new_screen.ymax);
                    dc.TextOut(x - tbounds.cx / 2, bounds.screen.ymax - y_offset - 2, str);
                }
            }
            if (y_ticks)
            {
                for each (auto & tick in *y_ticks)
                {
                    int y = new_bounds.world_to_screen().y(tick.first);
                    str = tick.second.c_str();
                    tbounds = dc.GetTextExtent(str);
                    dc.MoveTo(new_screen.xmin, y);
                    dc.LineTo(new_screen.xmax, y);
                    dc.TextOut(x_offset - tbounds.cx + 5, y - tbounds.cy / 2, str);
                }
            }

            if (content)
            {
                RECT clip = new_screen;
                dc.IntersectClipRect(&clip);
                content->draw(dc, new_bounds);
            }
        }
    };
}
