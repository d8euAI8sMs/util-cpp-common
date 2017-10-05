#pragma once

#include <util/common/plot/custom_drawable.h>
#include <util/common/plot/palette.h>

namespace plot
{

    static inline painter_t circle_painter
    (
        int radius = 5,
        palette::brush_ptr brush = palette::brush()
    )
    {
        return [brush, radius] (CDC &dc, const viewport &bounds)
        {
            CBrush *_old = dc.SelectObject(brush.get());
            CPen p(PS_NULL, 1, RGB(0, 0, 0));
            CPen *_old_pen = dc.SelectObject(&p);
            RECT rct;
            rct.left   = bounds.screen.xmin - radius;
            rct.top    = bounds.screen.ymin - radius;
            rct.right  = bounds.screen.xmin + radius;
            rct.bottom = bounds.screen.ymin + radius;
            dc.Ellipse(&rct);
            dc.SelectObject(_old_pen);
            dc.SelectObject(_old);
        };
    }
}
