#pragma once

#include <util/common/plot/auto_viewport.h>
#include <util/common/plot/list_drawable.h>

namespace plot
{

    template < typename _points_t >
    class simple_list_plot
    {

    public:

        util::ptr_t < _points_t >           data;
        points_source_t < _points_t >       data_source;

        typename list_drawable < _points_t > ::ptr_t view;

        world_mapper_t                      viewport_mapper;

        world_mapper_t                      mapped_world;
        world_t::ptr_t                      static_world;
        typename auto_viewport < _points_t > ::ptr_t auto_world;

    public:

        simple_list_plot & with_view()
        {
            this->view = list_drawable < _points_t > ::create();
            return *this;
        }

        simple_list_plot & with_view(typename list_drawable < _points_t > ::ptr_t view)
        {
            this->view = std::move(view);
            return *this;
        }

        simple_list_plot & with_view_point_painter(drawable::ptr_t point_painter = custom_drawable::create(circle_painter()))
        {
            if (this->view) this->view->point_painter = std::move(point_painter);
            return *this;
        }

        simple_list_plot & with_view_line_pen(palette::pen_ptr line_pen)
        {
            if (this->view) this->view->line_pen = std::move(line_pen);
            return *this;
        }

        simple_list_plot & with_static_viewport(world_t::ptr_t vp)
        {
            this->static_world = vp;
            this->viewport_mapper = make_world_mapper(std::move(vp));
            return *this;
        }

        simple_list_plot & with_static_viewport(const world_t & vp)
        {
            this->static_world = world_t::create(vp);
            this->viewport_mapper = make_world_mapper(this->static_world);
            return *this;
        }

        simple_list_plot & with_mapped_viewport(world_mapper_t vp)
        {
            this->mapped_world = vp;
            this->viewport_mapper = std::move(vp);
            return *this;
        }

        simple_list_plot & with_auto_viewport(typename auto_viewport < _points_t > ::ptr_t vp = min_max_auto_viewport < _points_t > ::create())
        {
            this->auto_world = vp;
            this->viewport_mapper = make_world_mapper < _points_t > (std::move(vp));
            return *this;
        }

        simple_list_plot & with_data()
        {
            this->data = util::create < _points_t > ();
            this->data_source = make_points_source(this->data);
            if (this->view) this->view->data_factory = this->data_source;
            return *this;
        }

        simple_list_plot & with_data(util::ptr_t < _points_t > data)
        {
            this->data = data;
            this->data_source = make_points_source(std::move(data));
            if (this->view) this->view->data_factory = this->data_source;
            return *this;
        }

        simple_list_plot & with_data(points_source_t < _points_t > data)
        {
            this->data_source = std::move(data);
            if (this->view) this->view->data_factory = this->data_source;
            return *this;
        }

        void refresh()
        {
            if (this->auto_world && this->data)
            {
                this->auto_world->setup(*this->data);
            }
        }
    };
}
