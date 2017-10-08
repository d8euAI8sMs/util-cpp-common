#pragma once

#include <util/common/plot/auto_viewport.h>
#include <util/common/plot/list_drawable.h>

namespace plot
{

    template < typename _points_t >
    class simple_list_plot
    {

    public:

        ptr_t < _points_t >                 data;
        points_source_t < _points_t >       data_source;

        typename list_drawable < _points_t > ::ptr_t view;

        world_mapper_t                      viewport_mapper;

        world_mapper_t                      custom_mapper;
        world_t::ptr_t                      custom_world;
        typename auto_viewport < _points_t > ::ptr_t custom_manager;

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

        simple_list_plot & with_view_point_painter(drawable::ptr_t point_painter)
        {
            if (this->view) this->view->point_painter = std::move(point_painter);
            return *this;
        }

        simple_list_plot & with_view_line_pen(palette::pen_ptr line_pen)
        {
            if (this->view) this->view->line_pen = std::move(line_pen);
            return *this;
        }

        simple_list_plot & with_viewport(world_t::ptr_t vp)
        {
            this->custom_world = vp;
            this->viewport_mapper = make_world_mapper(std::move(vp));
            return *this;
        }

        simple_list_plot & with_viewport(const world_t & vp)
        {
            this->custom_world = world_t::create(vp);
            this->viewport_mapper = make_world_mapper(this->custom_world);
            return *this;
        }

        simple_list_plot & with_viewport(world_mapper_t vp)
        {
            this->custom_mapper = vp;
            this->viewport_mapper = std::move(vp);
            return *this;
        }

        simple_list_plot & with_viewport(typename auto_viewport < _points_t > ::ptr_t vp)
        {
            this->custom_manager = vp;
            this->viewport_mapper = make_world_mapper(std::move(vp));
            return *this;
        }

        simple_list_plot & with_data()
        {
            this->data = create < _points_t > ();
            this->data_source = make_points_source(this->data);
            if (this->view) this->view->data_factory = this->data_source;
            return *this;
        }

        simple_list_plot & with_data(ptr_t < _points_t > data)
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
            if (this->custom_manager && this->data)
            {
                this->custom_manager->setup(*this->data);
            }
        }
    };
}
