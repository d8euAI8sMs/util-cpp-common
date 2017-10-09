#pragma once

#include <limits>

#include <util/common/plot/ptr.h>
#include <util/common/plot/viewport.h>

namespace plot
{

    /*****************************************************/
    /*               auto_viewport_params                */
    /*****************************************************/

    struct auto_viewport_params
    {
        using ptr_t = plot::ptr_t < auto_viewport_params > ;

        template < class ... T > static ptr_t create(T && ... t) { return plot::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

        rect < bool >   enabled;

        rect < double > factors;

        rect < double > paddings;

        screen_t        screen;

        rect < bool >   upper,
                        lower,
                        fixed;

        world_t::ptr_t  upper_bound,
                        lower_bound,
                        fixed_bound;
    };

    /*****************************************************/
    /*                 auto_viewport                     */
    /*****************************************************/

    template < typename _points_t >
    class auto_viewport
    {

    public:

        using ptr_t = plot::ptr_t < auto_viewport > ;

    protected:

        world_t              world;
        auto_viewport_params params;

    public:

        auto_viewport()
        {
        }

        auto_viewport(const world_t & world)
            : world(world)
        {
        }

        virtual ~auto_viewport()
        {
        }

    public:

        virtual const world_t & get_world() const
        {
            return world;
        }

        virtual void set_world(const world_t & world)
        {
            this->world = world;
        }

        virtual const auto_viewport_params & get_params() const
        {
            return params;
        }

        virtual void set_params(const auto_viewport_params & params)
        {
            this->params = params;
        }

        virtual void clear()
        {
            this->world = { };
        }

        virtual world_t adjust(const _points_t & data) = 0;

        virtual world_t setup(const _points_t & data)
        {
            clear();
            return adjust(data);
        }
    };

    /*****************************************************/
    /*                -viewport mappers-                 */
    /*****************************************************/

    template < typename _points_t >
    static inline world_mapper_t make_world_mapper(const auto_viewport < _points_t > & vp)
    {
        return [&vp] (const world_t & w) { return vp.get_world(); };
    }

    template < typename _points_t >
    static inline world_mapper_t make_world_mapper(typename auto_viewport < _points_t > ::ptr_t vp)
    {
        return [vp] (const world_t & w) { return vp->get_world(); };
    }

    template < typename _points_t >
    static inline viewport_mapper_t make_viewport_mapper(const auto_viewport < _points_t > & vp)
    {
        return [&vp] (const viewport & v) { return { v.screen, vp.get_world() }; };
    }

    template < typename _points_t >
    static inline viewport_mapper_t make_viewport_mapper(typename auto_viewport < _points_t > ::ptr_t vp)
    {
        return [vp] (const viewport & v) { return { v.screen, vp->get_world() }; };
    }

    /*****************************************************/
    /*               min_max_auto_viewport               */
    /*****************************************************/

    template < typename _points_t >
    class min_max_auto_viewport : public auto_viewport < _points_t >
    {

    public:

        using ptr_t = plot::ptr_t < min_max_auto_viewport > ;

        template < class ... T > static ptr_t create(T && ... t) { return plot::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        min_max_auto_viewport()
            : auto_viewport()
        {
        }

        min_max_auto_viewport(const world_t & vp)
            : auto_viewport(vp)
        {
        }

        virtual ~min_max_auto_viewport()
        {
        }

    public:

        virtual void set_world(const world_t & world) override
        {
            this->world = world;
            ensure_bounded();
        }

        virtual void clear() override
        {
            this->world = { };
            ensure_bounded();
        }

        virtual void set_params(const auto_viewport_params & params) override
        {
            this->params = params;
            ensure_bounded();
        }

        virtual world_t adjust(const _points_t & data) override
        {
            if (data.empty())
            {
                ensure_bounded();
                return this->world;
            }

            world_t & current = this->world;
            world_t enclosing =
            {
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)(),
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)()
            };

            for each (auto & p in data)
            {
                if ((this->params.enabled.empty() || this->params.enabled.xmin) && (enclosing.xmin > p.x)) enclosing.xmin = p.x;
                if ((this->params.enabled.empty() || this->params.enabled.xmax) && (enclosing.xmax < p.x)) enclosing.xmax = p.x;
                if ((this->params.enabled.empty() || this->params.enabled.ymin) && (enclosing.ymin > p.y)) enclosing.ymin = p.y;
                if ((this->params.enabled.empty() || this->params.enabled.ymax) && (enclosing.ymax < p.y)) enclosing.ymax = p.y;
            }

            bool is_current_empty = current.empty();

            double width, height;

            if (current.empty())
            {
                width  = enclosing.width();
                height = enclosing.height();
            }
            else
            {
                width  = ((this->params.enabled.empty() || this->params.enabled.xmax) ? max(current.xmax, enclosing.xmax) : current.xmax) - ((this->params.enabled.empty() || this->params.enabled.xmin) ? min(current.xmin, enclosing.xmin) : current.xmin);
                height = ((this->params.enabled.empty() || this->params.enabled.ymax) ? max(current.ymax, enclosing.ymax) : current.ymax) - ((this->params.enabled.empty() || this->params.enabled.ymin) ? min(current.ymin, enclosing.ymin) : current.ymin);
            }

            if ((this->params.enabled.empty() || this->params.enabled.xmin) && (((enclosing.xmin - this->params.paddings.xmin) < current.xmin) || is_current_empty)) current.xmin = enclosing.xmin - max(this->params.factors.xmin * width,  this->params.paddings.xmin);
            if ((this->params.enabled.empty() || this->params.enabled.xmax) && (((enclosing.xmax + this->params.paddings.xmax) > current.xmax) || is_current_empty)) current.xmax = enclosing.xmax + max(this->params.factors.xmax * width,  this->params.paddings.xmax);
            if ((this->params.enabled.empty() || this->params.enabled.ymin) && (((enclosing.ymin - this->params.paddings.ymin) < current.ymin) || is_current_empty)) current.ymin = enclosing.ymin - max(this->params.factors.ymin * height, this->params.paddings.ymin);
            if ((this->params.enabled.empty() || this->params.enabled.ymax) && (((enclosing.ymax + this->params.paddings.ymax) > current.ymax) || is_current_empty)) current.ymax = enclosing.ymax + max(this->params.factors.ymax * height, this->params.paddings.ymax);

            ensure_bounded();

            return this->world;
        }

    private:

        void ensure_bounded()
        {
            bool initially_empty = this->world.empty();

            if ((this->params.enabled.empty() || this->params.enabled.xmin) && this->params.upper_bound && (this->params.upper.xmin || this->params.upper.empty()) && (this->world.xmin < this->params.upper_bound->xmin) && (!initially_empty)) this->world.xmin = this->params.upper_bound->xmin;
            if ((this->params.enabled.empty() || this->params.enabled.xmax) && this->params.upper_bound && (this->params.upper.xmax || this->params.upper.empty()) && (this->world.xmax > this->params.upper_bound->xmax) && (!initially_empty)) this->world.xmax = this->params.upper_bound->xmax;
            if ((this->params.enabled.empty() || this->params.enabled.ymin) && this->params.upper_bound && (this->params.upper.ymin || this->params.upper.empty()) && (this->world.ymin < this->params.upper_bound->ymin) && (!initially_empty)) this->world.ymin = this->params.upper_bound->ymin;
            if ((this->params.enabled.empty() || this->params.enabled.ymax) && this->params.upper_bound && (this->params.upper.ymax || this->params.upper.empty()) && (this->world.ymax > this->params.upper_bound->ymax) && (!initially_empty)) this->world.ymax = this->params.upper_bound->ymax;

            if ((this->params.enabled.empty() || this->params.enabled.xmin) && this->params.lower_bound && (this->params.lower.xmin || this->params.lower.empty()) && ((this->world.xmin > this->params.lower_bound->xmin) || initially_empty)) this->world.xmin = this->params.lower_bound->xmin;
            if ((this->params.enabled.empty() || this->params.enabled.xmax) && this->params.lower_bound && (this->params.lower.xmax || this->params.lower.empty()) && ((this->world.xmax < this->params.lower_bound->xmax) || initially_empty)) this->world.xmax = this->params.lower_bound->xmax;
            if ((this->params.enabled.empty() || this->params.enabled.ymin) && this->params.lower_bound && (this->params.lower.ymin || this->params.lower.empty()) && ((this->world.ymin > this->params.lower_bound->ymin) || initially_empty)) this->world.ymin = this->params.lower_bound->ymin;
            if ((this->params.enabled.empty() || this->params.enabled.ymax) && this->params.lower_bound && (this->params.lower.ymax || this->params.lower.empty()) && ((this->world.ymax < this->params.lower_bound->ymax) || initially_empty)) this->world.ymax = this->params.lower_bound->ymax;

            if ((this->params.enabled.empty() || this->params.enabled.xmin) && this->params.fixed.xmin && this->params.fixed_bound) this->world.xmin = this->params.fixed_bound->xmin;
            if ((this->params.enabled.empty() || this->params.enabled.xmax) && this->params.fixed.xmax && this->params.fixed_bound) this->world.xmax = this->params.fixed_bound->xmax;
            if ((this->params.enabled.empty() || this->params.enabled.ymin) && this->params.fixed.ymin && this->params.fixed_bound) this->world.ymin = this->params.fixed_bound->ymin;
            if ((this->params.enabled.empty() || this->params.enabled.ymax) && this->params.fixed.ymax && this->params.fixed_bound) this->world.ymax = this->params.fixed_bound->ymax;
        }
    };
}
