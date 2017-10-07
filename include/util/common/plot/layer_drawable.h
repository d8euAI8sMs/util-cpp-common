#pragma once

#include <vector>

#include <util/common/plot/drawable.h>

namespace plot
{

    class layer_drawable : public drawable
    {

    public:

        using ptr_t = std::shared_ptr < layer_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return std::make_shared < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        std::vector < drawable::ptr_t > layers;

    public:

        layer_drawable & with(drawable::ptr_t layer)
        {
            layers.emplace_back(std::move(layer));
            return *this;
        }

        virtual ~layer_drawable()
        {
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
