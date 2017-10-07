#pragma once

#include <functional>

#include <util/common/plot/drawable.h>

namespace plot
{

    using painter_t = std::function < void (CDC &, const viewport &) > ;

    class custom_drawable : public drawable
    {

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
