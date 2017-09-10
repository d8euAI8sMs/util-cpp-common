#pragma once


#include <Afxwin.h>
#include <util/common/math/common.h>
#include <util/common/plot/plot.h>


namespace common
{


    using data_t       = plot::list_drawable::data_t;
    using data_ptr     = std::shared_ptr < data_t > ;
    using pair_t       = std::pair < double, double > ;
    using plot::world_t;




    // plotting






    struct simple_list_plot
    {
        data_ptr data;
        world_t  world;
        size_t   point_size;
        size_t   line_width;
        COLORREF color;
        bool     connect;
        bool     visible;

        static simple_list_plot curve(COLORREF color    = 0x000000,
                                      size_t line_width = 2)
        {
            return simple_list_plot(0, {}, color, 0, line_width, true);
        }

        static simple_list_plot points(COLORREF color    = 0x000000,
                                       size_t point_size = 2)
        {
            return simple_list_plot(0, {}, color, point_size, 0, false);
        }

        static simple_list_plot connected_points(COLORREF color    = 0x000000,
                                                 size_t point_size = 2,
                                                 size_t line_width = 1)
        {
            return simple_list_plot(0, {}, color, point_size, line_width, true);
        }

        simple_list_plot(size_t   initial_size = 0,
                         world_t  world        = {},
                         COLORREF color        = 0x000000,
                         size_t   point_size   = 2,
                         size_t   line_width   = 1,
                         bool     connect      = true,
                         bool     visible      = true)
            : data(std::make_shared<data_ptr::element_type>(initial_size))
            , world(world)
            , color(color)
            , point_size(point_size)
            , line_width(line_width)
            , connect(connect)
            , visible(visible)
        {
        }
    };

    inline void setup(simple_list_plot &plot,
                      sampled_t        &source,
                      size_t           n_points = (std::numeric_limits<size_t>::max)(),
                      int              shift = 0,
                      un_op_t          op = identity_un_op(),
                      bool             update_world    = true,
                      bool             symmetric_world = false,
                      double           y_factor        = 1.1,
                      int              x_spacing       = 1)
    {
        double ymin = (std::numeric_limits<double>::max)();
        double ymax = (std::numeric_limits<double>::min)();
        n_points = min(n_points, source.count);
        if (plot.visible)
        {
            plot.data->resize(n_points);
        }
        else
        {
            plot.data->clear();
        }
        for (size_t i = 0; i < n_points; i++)
        {
            double y = op(i, source.samples[i]);
            if (plot.visible)
            {
                plot.data->at(i) = { source.period * (int(i) + shift), y };
            }
            if (y < ymin)
            {
                ymin = y;
            }
            if (y > ymax)
            {
                ymax = y;
            }
        }
        if (update_world)
        {
            if (symmetric_world)
            {
                if (abs(ymin) > abs(ymax))
                {
                    ymax = -ymin;
                }
                else
                {
                    ymin = -ymax;
                }
            }
            plot.world.ymin = ymin * ((ymin < 0) ? y_factor : 1 / y_factor);
            plot.world.ymax = ymax * ((ymax > 0) ? y_factor : 1 / y_factor);
            plot.world.xmin = source.period * (shift - x_spacing);
            plot.world.xmax = source.period * (source.count + shift + x_spacing);
        }
    }

    inline void setup(simple_list_plot &plot,
                      double           *x,
                      double           *y,
                      size_t           n,
                      bool             update_world   = true,
                      bool             symmetric_y    = false,
                      bool             symmetric_x    = false,
                      bool             both_symmetric = false,
                      double           y_factor       = 1.1,
                      int              x_factor       = 1.1)
    {
        world_t min_max = {
            (std::numeric_limits<double>::max)(), (std::numeric_limits<double>::min)(),
            (std::numeric_limits<double>::max)(), (std::numeric_limits<double>::min)()
        };
        plot.data->resize(n);
        for (size_t i = 0; i < n; i++)
        {
            plot.data->at(i) = { x[i], y[i] };
            if (y[i] < min_max.ymin) { min_max.ymin = y[i]; }
            if (x[i] < min_max.xmin) { min_max.xmin = x[i]; }
            if (y[i] > min_max.ymax) { min_max.ymax = y[i]; }
            if (x[i] > min_max.xmax) { min_max.xmax = x[i]; }
        }
        if (update_world)
        {
            if (symmetric_y)
            {
                if (abs(min_max.ymin) > abs(min_max.ymax)) { min_max.ymax = -min_max.ymin; }
                else                                       { min_max.ymin = -min_max.ymax; }
            }
            if (symmetric_x)
            {
                if (abs(min_max.xmin) > abs(min_max.xmax)) { min_max.xmax = -min_max.xmin; }
                else                                       { min_max.xmin = -min_max.xmax; }
            }
            if (both_symmetric)
            {
                assert(symmetric_x && symmetric_y);
                if (min_max.xmax > min_max.ymax) { min_max = { -min_max.xmax, min_max.xmax, -min_max.xmax, min_max.xmax }; }
                else                             { min_max = { -min_max.ymax, min_max.ymax, -min_max.ymax, min_max.ymax }; }
            }
            plot.world.ymin = min_max.ymin * y_factor;
            plot.world.ymax = min_max.ymax * y_factor;
            plot.world.xmin = min_max.xmin * x_factor;
            plot.world.xmax = min_max.xmax * x_factor;
        }
    }

    inline plot::plot_builder & operator << (plot::plot_builder &builder, simple_list_plot &plot_struct)
    {
        return builder
            .in_world(&plot_struct.world)
            .with_data(plot::list_drawable::const_data_factory(plot_struct.data),
                       plot::list_drawable::circle_painter(plot_struct.point_size, plot::palette::brush(plot_struct.color)),
                       (plot_struct.connect ? plot::palette::pen(plot_struct.color, plot_struct.line_width) : plot::palette::pen_ptr()));
    }
}
