#pragma once

#include <util/common/plot/ptr.h>
#include <util/common/plot/viewport.h>
#include <util/common/plot/points_source.h>

namespace plot
{

    template < typename _points_t >
    class auto_viewport
    {

    public:

        using ptr_t = plot::ptr_t < auto_viewport > ;

    protected:

        viewport::ptr_t vp;

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

        virtual void clear()
        {
            *this->vp = { this->vp->screen, { } };
        }

        virtual viewport::ptr_t adjust(const _points_t & data) = 0;

        virtual viewport::ptr_t setup(const _points_t & data)
        {
            clear();
            return adjust(data);
        }
    };
}
