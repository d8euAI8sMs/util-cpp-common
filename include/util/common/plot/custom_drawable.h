#pragma once

#include <functional>

#include <util/common/plot/ptr.h>
#include <util/common/plot/drawable.h>

namespace plot
{

    using painter_t = std::function < void (CDC &, const viewport &) > ;

    class custom_drawable : public drawable
    {

    public:

        using ptr_t = plot::ptr_t < custom_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return plot::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        painter_t painter;

    public:

        custom_drawable()
        {
        }

        custom_drawable(painter_t painter)
            : painter(std::move(painter))
        {
        }

        virtual ~custom_drawable()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) override
        {
            painter(dc, bounds);
        }
    };
}
