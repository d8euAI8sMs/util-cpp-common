#pragma once

#include <util/common/plot/plot.h>


// CPlotControl

class CPlotControl : public CStatic
{
	DECLARE_DYNAMIC(CPlotControl)

public:
	CPlotControl();
	virtual ~CPlotControl();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    virtual void RedrawBuffer();
    virtual void SwapBuffers();
    plot::layer_drawable plot_layer;
    plot::palette::brush_ptr background;
    bool symmetric;
    bool triple_buffered;
private:
    CBitmap *buffer;
    CBitmap *buffer2;
};


