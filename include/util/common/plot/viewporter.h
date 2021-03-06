#pragma once

#include <functional>
#include <memory>

#include <util/common/ptr.h>
#include <util/common/plot/viewport.h>
#include <util/common/plot/drawable.h>

namespace plot
{

    class viewporter : public drawable
    {

    public:

        using ptr_t = util::ptr_t < viewporter > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        drawable::ptr_t   layer;
        viewport_mapper_t mapper;

    public:

        viewporter()
        {
        }

        viewporter(drawable::ptr_t layer, viewport_mapper_t mapper)
            : layer(std::move(layer))
            , mapper(std::move(mapper))
        {
        }

        virtual ~viewporter()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) override
        {
            if (layer)
            {
                if (mapper)
                {
                    layer->draw(dc, mapper(bounds));
                }
                else
                {
                    layer->draw(dc, bounds);
                }
            }
        }
    };
};
