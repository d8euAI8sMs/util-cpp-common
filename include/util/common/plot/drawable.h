#pragma once

#include <afxwin.h>

#include <memory>
#include <functional>

#include <util/common/plot/viewport.h>

namespace plot
{

    class drawable
    {

    public:

        virtual void draw(CDC &dc, const viewport &bounds) = 0;

        virtual ~drawable()
        {
        }
    };

    using drawable_ptr = std::shared_ptr < drawable > ;
}
