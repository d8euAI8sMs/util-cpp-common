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

        world_t upper_bound,
                lower_bound;
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

        viewport::ptr_t vp;
        auto_viewport_params params;

    public:

        auto_viewport()
            : vp(viewport::create())
        {
        }

        auto_viewport(const viewport & vp)
            : vp(viewport::create(vp))
        {
        }

        auto_viewport(viewport::ptr_t vp)
            : vp(std::move(vp))
        {
            if (!vp) this->vp = viewport::create();
        }

        virtual ~auto_viewport()
        {
        }

    public:

        virtual viewport::ptr_t get()
        {
            return vp;
        }

        virtual void set(const viewport & vp)
        {
            *this->vp = vp;
        }

        virtual void set(viewport::ptr_t vp)
        {
            this->vp = vp;
            if (!vp) this->vp = viewport::create();
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
            this->vp->world = { };
        }

        virtual viewport::ptr_t adjust(const _points_t & data) = 0;

        virtual viewport::ptr_t setup(const _points_t & data)
        {
            clear();
            return adjust(data);
        }
    };

    /*****************************************************/
    /*                 viewport_mapper                   */
    /*****************************************************/

    template < typename _points_t >
    static inline viewport_mapper_t make_viewport_mapper(const auto_viewport < _points_t > & vp)
    {
        return [&vp] (const viewport &) { return *vp.get(); };
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

        min_max_auto_viewport(const viewport & vp)
            : auto_viewport(vp)
        {
        }

        min_max_auto_viewport(viewport::ptr_t vp)
            : auto_viewport(std::move(vp))
        {
        }

        virtual ~min_max_auto_viewport()
        {
        }

    public:

        virtual void set_params(const auto_viewport_params & params) override
        {
            this->params = params;

            if (this->vp->empty()) return;

            world_t current = this->vp->world;

            ensure_bounded(current, get_enabled_flags_with_defaults());

            this->vp->world = current;
        }

        virtual viewport::ptr_t adjust(const _points_t & data) override
        {
            if ((this->vp->empty()) || data.empty()) return this->vp;

            rect < bool > enabled = get_enabled_flags_with_defaults();

            world_t & current = this->vp->world;
            world_t enclosing =
            {
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)(),
                (std::numeric_limits < double > :: max)   (),
                (std::numeric_limits < double > :: lowest)()
            };

            for each (auto & p in data)
            {
                if (enabled.xmin && (enclosing.xmin > p.x)) enclosing.xmin = p.x;
                if (enabled.xmax && (enclosing.xmax < p.x)) enclosing.xmax = p.x;
                if (enabled.ymin && (enclosing.ymin > p.y)) enclosing.ymin = p.y;
                if (enabled.ymax && (enclosing.ymax < p.y)) enclosing.ymax = p.y;
            }

            double width  = (enabled.xmax ? max(current.xmax, enclosing.xmax) : current.xmax) - (this->params.enabled.xmin ? min(current.xmin, enclosing.xmin) : current.xmin);
            double height = (enabled.ymax ? max(current.ymax, enclosing.ymax) : current.ymax) - (this->params.enabled.ymin ? min(current.ymin, enclosing.ymin) : current.ymin);

            if (enabled.xmin && ((enclosing.xmin - this->params.paddings.xmin) < current.xmin)) current.xmin = enclosing.xmin - max(this->params.factors.xmin * width,  this->params.paddings.xmin);
            if (enabled.xmax && ((enclosing.xmax + this->params.paddings.xmax) > current.xmax)) current.xmax = enclosing.xmax + max(this->params.factors.xmax * width,  this->params.paddings.xmax);
            if (enabled.ymin && ((enclosing.ymin - this->params.paddings.ymin) < current.ymin)) current.ymin = enclosing.ymin - max(this->params.factors.ymin * height, this->params.paddings.ymin);
            if (enabled.ymax && ((enclosing.ymax + this->params.paddings.ymax) > current.ymax)) current.ymax = enclosing.ymax + max(this->params.factors.ymax * height, this->params.paddings.ymax);

            ensure_bounded(current, enabled);

            return this->vp;
        }

    private:

        rect < bool > get_enabled_flags_with_defaults()
        {
            return
            {
                this->params.enabled.empty() || this->params.enabled.xmin,
                this->params.enabled.empty() || this->params.enabled.xmax,
                this->params.enabled.empty() || this->params.enabled.ymin,
                this->params.enabled.empty() || this->params.enabled.ymax
            };
        }

        void ensure_bounded(world_t & current, const rect < bool > & enabled)
        {
            if (enabled.xmin && (!this->params.upper_bound.empty()) && (current.xmin < this->params.upper_bound.xmin)) current.xmin = this->params.upper_bound.xmin;
            if (enabled.xmax && (!this->params.upper_bound.empty()) && (current.xmax > this->params.upper_bound.xmax)) current.xmax = this->params.upper_bound.xmax;
            if (enabled.ymin && (!this->params.upper_bound.empty()) && (current.ymin < this->params.upper_bound.ymin)) current.ymin = this->params.upper_bound.ymin;
            if (enabled.ymax && (!this->params.upper_bound.empty()) && (current.ymax > this->params.upper_bound.ymax)) current.ymax = this->params.upper_bound.ymax;

            if (enabled.xmin && (!this->params.lower_bound.empty()) && (current.xmin > this->params.lower_bound.xmin)) current.xmin = this->params.lower_bound.xmin;
            if (enabled.xmax && (!this->params.lower_bound.empty()) && (current.xmax < this->params.lower_bound.xmax)) current.xmax = this->params.lower_bound.xmax;
            if (enabled.ymin && (!this->params.lower_bound.empty()) && (current.ymin > this->params.lower_bound.ymin)) current.ymin = this->params.lower_bound.ymin;
            if (enabled.ymax && (!this->params.lower_bound.empty()) && (current.ymax < this->params.lower_bound.ymax)) current.ymax = this->params.lower_bound.ymax;
        }
    };
}
