#pragma once

#include <functional>
#include <memory>

#include <util/common/plot/ptr.h>
#include <util/common/plot/viewport.h>
#include <util/common/plot/drawable.h>

namespace plot
{

    class viewporter : public drawable
    {

    public:

        using ptr_t = plot::ptr_t < viewporter > ;

        template < class ... T > static ptr_t create(T && ... t) { return plot::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

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

        virtual void draw(CDC &dc, const viewport &bounds) override
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
