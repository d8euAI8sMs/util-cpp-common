#pragma once

#include <vector>

#include <util/common/ptr.h>
#include <util/common/plot/drawable.h>

namespace plot
{

    class layer_drawable : public drawable
    {

    public:

        using ptr_t = util::ptr_t < layer_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        std::vector < drawable::ptr_t > layers;

    public:

        layer_drawable()
        {
        }

        layer_drawable(const std::vector < drawable::ptr_t > & layers)
            : layers(layers)
        {
        }

        layer_drawable(std::vector < drawable::ptr_t > && layers)
            : layers(std::move(layers))
        {
        }

        layer_drawable & with(drawable::ptr_t layer)
        {
            layers.emplace_back(std::move(layer));
            return *this;
        }

        virtual ~layer_drawable()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) override
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
