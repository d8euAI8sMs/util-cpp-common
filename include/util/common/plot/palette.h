#pragma once

#include <afxwin.h>

#include <util/common/ptr.h>

namespace plot
{

    namespace palette
    {

        using pen_ptr   = util::ptr_t < CPen > ;
        using brush_ptr = util::ptr_t < CBrush > ;

        inline pen_ptr pen(COLORREF color = 0x000000, int width = 1, int style = 0)
        {
            return util::create < CPen > (style, width, color);
        }

        inline brush_ptr brush(COLORREF color = 0x000000)
        {
            return util::create < CBrush > (color);
        }
    };
}
