#pragma once

#include <util/common/ptr.h>
#include <util/common/plot/palette.h>
#include <util/common/plot/drawable.h>
#include <util/common/plot/data_source.h>
#include <util/common/plot/custom_drawable.h>
#include <util/common/plot/utility.h>
#include <util/common/geom/mesh.h>

namespace plot
{

    class dirichlet_cell_drawable : public drawable
    {

    public:

        using ptr_t = util::ptr_t < dirichlet_cell_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        data_source_t < geom::mesh > data_source;
        drawable::ptr_t  point_painter;
        palette::pen_ptr line_pen;

    public:

        dirichlet_cell_drawable()
        {
        }

        dirichlet_cell_drawable
        (
            data_source_t < geom::mesh > data_source,
            drawable::ptr_t  point_painter = custom_drawable::create(circle_painter()),
            palette::pen_ptr line_pen = palette::pen()
        )
            : data_source(std::move(data_source))
            , point_painter(std::move(point_painter))
            , line_pen(line_pen)
        {
        }

        virtual ~dirichlet_cell_drawable()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) override
        {
            if (!data_source) return;
            auto m = data_source(bounds);
            if (!m || m->triangles().empty()) return;

            auto & data = *m;

            if (data.vertices().empty()) return;

            for (size_t i = 0; i < data.vertices().size(); ++i)
            {
                auto & path = data.vertices()[i].neighborhood.path;

                if (path.empty()) continue;

                auto j = path.begin();

                if (line_pen)
                {
                    auto old = dc.SelectObject(line_pen.get());

                    dc.MoveTo(bounds.world_to_screen().xy(
                        data.triangles()[*(j++)].enclosing.center));

                    for (; j != path.end(); ++j)
                    {
                        dc.LineTo(bounds.world_to_screen().xy(
                            data.triangles()[*j].enclosing.center));
                    }

                    /* close path */
                    dc.LineTo(bounds.world_to_screen().xy(
                        data.triangles()[path.front()].enclosing.center));

                    dc.SelectObject(old);
                }

                if (point_painter)
                {
                    point_painter->draw_at(dc, bounds, data.point_at(i));
                }
            }
        }
    };
}
