// PlotStatic.cpp : implementation file
//

#include "stdafx.h"
#include <util/common/plot/PlotStatic.h>


// PlotStatic

IMPLEMENT_DYNAMIC(PlotStatic, CStatic)

PlotStatic::PlotStatic()
    : buffer(nullptr)
    , buffer2(nullptr)
    , background(plot::palette::brush(0xffffff))
    , triple_buffered(false)
{
}

PlotStatic::~PlotStatic()
{
    delete buffer;
    delete buffer2;
}


BEGIN_MESSAGE_MAP(PlotStatic, CStatic)
    ON_WM_DRAWITEM()
END_MESSAGE_MAP()



// PlotStatic message handlers




void PlotStatic::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CStatic::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void PlotStatic::RedrawBuffer()
{
    CDC dc; dc.CreateCompatibleDC(NULL);

    dc.SetBkMode(TRANSPARENT);

    RECT bounds;  GetClientRect(&bounds);
    plot::screen_t screen(0, bounds.right, 0, bounds.bottom);

    if (buffer2 == nullptr)
    {
        buffer2 = new CBitmap;
        buffer2->CreateBitmap(bounds.right, bounds.bottom, 1, 32, NULL);
    }

    auto old = dc.SelectObject(buffer2);

    if (symmetric)
    {
        int min = min(bounds.bottom, bounds.right);
        screen.xmin = (bounds.right - min) / 2;
        screen.xmax = screen.xmin + min;
        screen.ymin = (bounds.bottom - min) / 2;
        screen.ymax = screen.ymin + min;
    }

    plot::viewport vp(screen, {});

    dc.FillRect(&bounds, background.get());

    this->plot_layer.draw(dc, vp);

    dc.SelectObject(old);
}


void PlotStatic::SwapBuffers()
{
    std::swap(buffer, buffer2);
}



void PlotStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    RECT bounds;  GetClientRect(&bounds);

    if (!triple_buffered)
    {
        SwapBuffers();
        RedrawBuffer();
        SwapBuffers();
    }

    if (!buffer)
    {
        pDC->FillRect(&bounds, background.get());
        return;
    }

    CDC dc; dc.CreateCompatibleDC(pDC);
    auto old = dc.SelectObject(buffer);

    pDC->BitBlt(0, 0, bounds.right, bounds.bottom, &dc, 0, 0, SRCCOPY);

    dc.SelectObject(old);
}
