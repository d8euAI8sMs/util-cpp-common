#pragma once

#include <afxwin.h>

#include <memory>

namespace plot
{

    namespace palette
    {

        using pen_ptr   = std::shared_ptr < CPen > ;
        using brush_ptr = std::shared_ptr < CBrush > ;

        inline pen_ptr pen(COLORREF color = 0x000000, int width = 1, int style = 0)
        {
            return pen_ptr(new CPen(style, width, color));
        }

        inline brush_ptr brush(COLORREF color = 0x000000)
        {
            return brush_ptr(new CBrush(color));
        }
    };
}
