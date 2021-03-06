#pragma once

#include <limits>

#include <util/common/ptr.h>
#include <util/common/iterable.h>
#include <util/common/plot/viewport.h>

namespace plot
{

    /*****************************************************/
    /*               auto_viewport_params                */
    /*****************************************************/

    struct auto_viewport_params
    {
        using ptr_t = util::ptr_t < auto_viewport_params > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

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

    template
    <
        typename _container_t,
        typename _mapped_t = point < double >
    >
    class auto_viewport
    {

    public:

        using ptr_t = util::ptr_t < auto_viewport > ;

    protected:

        using _iterator_t = typename _container_t::const_iterator;

    protected:

        world_t              world;
        auto_viewport_params params;

    public:

        util::data_mapper_t < _container_t, _iterator_t, _mapped_t > data_mapper;

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

        virtual void adjust(const _container_t & data) = 0;

        virtual const world_t & flush() = 0;

        virtual const world_t & setup(const _container_t & data)
        {
            clear();
            adjust(data);
            return flush();
        }
    };

    /*****************************************************/
    /*                -viewport mappers-                 */
    /*****************************************************/

    template < typename _container_t >
    static inline world_mapper_t make_world_mapper(const auto_viewport < _container_t > & vp)
    {
        return [&vp] (const world_t & w) { return vp.get_world(); };
    }

    template < typename _container_t >
    static inline world_mapper_t make_world_mapper(typename auto_viewport < _container_t > ::ptr_t vp)
    {
        return [vp] (const world_t & w) { return vp->get_world(); };
    }

    template < typename _container_t >
    static inline viewport_mapper_t make_viewport_mapper(const auto_viewport < _container_t > & vp)
    {
        return [&vp] (const viewport & v) { return { v.screen, vp.get_world() }; };
    }

    template < typename _container_t >
    static inline viewport_mapper_t make_viewport_mapper(typename auto_viewport < _container_t > ::ptr_t vp)
    {
        return [vp] (const viewport & v) { return { v.screen, vp->get_world() }; };
    }

    /*****************************************************/
    /*               min_max_auto_viewport               */
    /*****************************************************/

    template
    <
        typename _container_t,
        typename _mapped_t = point < double >
    >
    class min_max_auto_viewport : public auto_viewport < _container_t, _mapped_t >
    {

    public:

        using ptr_t = util::ptr_t < min_max_auto_viewport > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    protected:

        world_t enclosing;

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
            this->enclosing =
            {
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)(),
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)()
            };
        }

        virtual void clear() override
        {
            this->world = { };
            this->enclosing =
            {
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)(),
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)()
            };
        }

        virtual void set_params(const auto_viewport_params & params) override
        {
            this->params = params;
        }

        virtual void adjust(const _container_t & data) override
        {
            if ((!data_mapper) && !(util::is_identity_mappable < _iterator_t, _mapped_t > ()) || data.empty())
            {
                return;
            }

            size_t idx = 0;
            _iterator_t end = data.end();
            for (_iterator_t it = data.begin(); it != end; ++it, ++idx)
            {
                _mapped_t p = util::get_mapped_value_or_default(data_mapper, data, it, idx);
                if ((this->params.enabled.empty() || this->params.enabled.xmin) && (this->enclosing.xmin > p.x)) this->enclosing.xmin = p.x;
                if ((this->params.enabled.empty() || this->params.enabled.xmax) && (this->enclosing.xmax < p.x)) this->enclosing.xmax = p.x;
                if ((this->params.enabled.empty() || this->params.enabled.ymin) && (this->enclosing.ymin > p.y)) this->enclosing.ymin = p.y;
                if ((this->params.enabled.empty() || this->params.enabled.ymax) && (this->enclosing.ymax < p.y)) this->enclosing.ymax = p.y;
            }
        }

        virtual const world_t & flush() override
        {
            world_t & current = this->world;

            bool is_current_empty = current.empty();

            double width, height;

            if (current.empty())
            {
                width  = this->enclosing.width();
                height = this->enclosing.height();
            }
            else
            {
                width  = ((this->params.enabled.empty() || this->params.enabled.xmax) ? max(current.xmax, this->enclosing.xmax) : current.xmax) - ((this->params.enabled.empty() || this->params.enabled.xmin) ? min(current.xmin, this->enclosing.xmin) : current.xmin);
                height = ((this->params.enabled.empty() || this->params.enabled.ymax) ? max(current.ymax, this->enclosing.ymax) : current.ymax) - ((this->params.enabled.empty() || this->params.enabled.ymin) ? min(current.ymin, this->enclosing.ymin) : current.ymin);
            }

            if ((this->params.enabled.empty() || this->params.enabled.xmin) && (((this->enclosing.xmin - this->params.paddings.xmin) < current.xmin) || is_current_empty)) current.xmin = this->enclosing.xmin - max(this->params.factors.xmin * width,  this->params.paddings.xmin);
            if ((this->params.enabled.empty() || this->params.enabled.xmax) && (((this->enclosing.xmax + this->params.paddings.xmax) > current.xmax) || is_current_empty)) current.xmax = this->enclosing.xmax + max(this->params.factors.xmax * width,  this->params.paddings.xmax);
            if ((this->params.enabled.empty() || this->params.enabled.ymin) && (((this->enclosing.ymin - this->params.paddings.ymin) < current.ymin) || is_current_empty)) current.ymin = this->enclosing.ymin - max(this->params.factors.ymin * height, this->params.paddings.ymin);
            if ((this->params.enabled.empty() || this->params.enabled.ymax) && (((this->enclosing.ymax + this->params.paddings.ymax) > current.ymax) || is_current_empty)) current.ymax = this->enclosing.ymax + max(this->params.factors.ymax * height, this->params.paddings.ymax);

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
