#pragma once

#include <functional>
#include <memory>

#include <util/common/plot/viewport.h>
#include <util/common/plot/drawable.h>

namespace plot
{

    class viewporter : public drawable
    {

    public:

        drawable_ptr      layer;
        viewport_mapper_t mapper;

    public:

        viewporter()
        {
        }

        viewporter(drawable_ptr layer, viewport_mapper_t mapper)
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
