#pragma once

#include <functional>

#include <util/common/plot/drawable.h>

namespace plot
{

    using painter_t = std::function < void (CDC &, const viewport &) > ;

    class custom_drawable : public drawable
    {

    public:

        using ptr_t = std::shared_ptr < custom_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return std::make_shared < typename ptr_t::element_type > (std::forward < T > (t) ...); }

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

    public:

        virtual void draw(CDC &dc, const viewport &bounds) override
        {
            painter(dc, bounds);
        }
    };
}
