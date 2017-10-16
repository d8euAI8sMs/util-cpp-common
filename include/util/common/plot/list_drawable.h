#pragma once

#include <utility>

#include <util/common/ptr.h>
#include <util/common/plot/palette.h>
#include <util/common/plot/drawable.h>
#include <util/common/plot/data_source.h>
#include <util/common/plot/custom_drawable.h>
#include <util/common/plot/utility.h>

namespace plot
{

    template
    <
        typename _container_t,
        typename _mapped_t = typename point < double >
    >
    class list_drawable : public drawable
    {

    public:

        using ptr_t = util::ptr_t < list_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        data_source_t < _container_t > data_source;
        data_mapper_t < typename _container_t::iterator, _mapped_t > data_mapper;
        drawable::ptr_t  point_painter;
        palette::pen_ptr line_pen;

    public:

        list_drawable()
        {
        }

        list_drawable
        (
            data_source_t < _container_t > data_source,
            drawable::ptr_t  point_painter = custom_drawable::create(circle_painter()),
            palette::pen_ptr line_pen = palette::pen()
        )
            : data_source(std::move(data_source))
            , point_painter(std::move(point_painter))
            , line_pen(line_pen)
        {
        }

        virtual ~list_drawable()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) override
        {
            if (!data_source) return;
            auto data = data_source(bounds);
            if (data->empty()) return;

            point < double > p;

            if (data_mapper)
            {
                p = data_mapper(data->begin(), 0);
            }
            else
            {
                p = *data->begin();
            }

            dc.MoveTo(bounds.world_to_screen().xy(p));

            size_t idx = 0;
            auto end = data->end();

            for (auto it = data->begin(); it != end; ++it, ++idx)
            {
                if (data_mapper)
                {
                    p = data_mapper(it, idx);
                }
                else
                {
                    p = *it;
                }
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
