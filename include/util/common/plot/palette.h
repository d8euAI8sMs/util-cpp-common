#pragma once

#include <afxwin.h>

#include <util/common/plot/ptr.h>

namespace plot
{

    namespace palette
    {

        using pen_ptr   = plot::ptr_t < CPen > ;
        using brush_ptr = plot::ptr_t < CBrush > ;

        inline pen_ptr pen(COLORREF color = 0x000000, int width = 1, int style = 0)
        {
            return create < CPen > (style, width, color);
        }

        inline brush_ptr brush(COLORREF color = 0x000000)
        {
            return create < CBrush > (color);
        }
    };
}
