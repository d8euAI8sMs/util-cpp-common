#pragma once

#include <utility>

#include <util/common/plot/palette.h>
#include <util/common/plot/drawable.h>
#include <util/common/plot/points_source.h>
#include <util/common/plot/custom_drawable.h>
#include <util/common/plot/utility.h>

namespace plot
{

    class list_drawable : public drawable
    {

    protected:

        points_source_t  data_factory;
        drawable_ptr     point_painter;
        palette::pen_ptr line_pen;

    public:

        list_drawable
        (
            points_source_t  data_factory,
            drawable_ptr     point_painter = std::make_shared < custom_drawable > (circle_painter()),
            palette::pen_ptr line_pen = palette::pen()
        )
            : data_factory(std::move(data_factory))
            , point_painter(std::move(point_painter))
            , line_pen(line_pen)
        {
        }

        virtual ~list_drawable()
        {
        }

        virtual void draw(CDC &dc, const viewport &bounds) override
        {
            auto data = data_factory(bounds);
            if (data->empty()) return;
            dc.MoveTo(bounds.world_to_screen().xy((*data)[0]));
            for each (auto &p in *data)
            {
                if (line_pen)
                {
                    auto _old = dc.SelectObject(*line_pen);
                    dc.LineTo(bounds.world_to_screen().xy(p));
                    dc.SelectObject(_old);
                }
                if (point_painter)
                {
                    point_painter->draw_at(dc, bounds, p);
                }
            }
        }
    };
}
