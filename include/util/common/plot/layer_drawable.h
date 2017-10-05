#pragma once

#include <vector>

#include <util/common/plot/drawable.h>

namespace plot
{

    class layer_drawable : public drawable
    {

    public:

        std::vector < drawable_ptr > layers;

    public:

        layer_drawable & with(drawable_ptr layer)
        {
            layers.emplace_back(std::move(layer));
            return *this;
        }

        virtual void draw(CDC &dc, const viewport &bounds) override
        {
            for each (auto &layer in layers)
            {
                int _old = dc.SaveDC();

                RECT clip = bounds.screen;
                dc.IntersectClipRect(&clip);

                layer->draw(dc, bounds);

                dc.RestoreDC(_old);
            }
        }
    };
}
