// OglControl.cpp : implementation file
//

#include "stdafx.h"
#include <util/common/gui/OglControl.h>

#include <GL/GL.h>

// COglControl

IMPLEMENT_DYNAMIC(COglControl, CStatic)

COglControl::COglControl()
    : glRC(NULL)
    , m_bLMousePressed(false)
    , m_bRMousePressed(false)
    , m_rotAngleH(0)
    , m_rotAngleV(0)
    , m_rotAngleHDelta(0)
    , m_rotAngleVDelta(0)
    , m_zoom(1)
    , m_zoomMultiplier(1)
    , m_translateX(0)
    , m_translateY(0)
    , m_translateXDelta(0)
    , m_translateYDelta(0)
    , m_rotAngleHMultiplier(2 * M_PI)
    , m_rotAngleVMultiplier(2 * M_PI)
    , m_translateXMultiplier(1)
    , m_translateYMultiplier(1)
{
}

COglControl::~COglControl()
{
}


static PIXELFORMATDESCRIPTOR _pfd =
{
    sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
    1,                              // version number
    PFD_DRAW_TO_WINDOW |            // support window
    PFD_SUPPORT_OPENGL |            // support OpenGL
    PFD_DOUBLEBUFFER,               // double buffered
    PFD_TYPE_RGBA,                  // RGBA type
    24,                             // 24-bit color depth
    0, 0, 0, 0, 0, 0,               // color bits ignored
    0,                              // no alpha buffer
    0,                              // shift bit ignored
    0,                              // no accumulation buffer
    0, 0, 0, 0,                     // accum bits ignored
    32,                             // 32-bit z-buffer
    0,                              // no stencil buffer
    0,                              // no auxiliary buffer
    PFD_MAIN_PLANE,                 // main layer
    0,                              // reserved
    0, 0, 0                         // layer masks ignored
};


BEGIN_MESSAGE_MAP(COglControl, CStatic)
    ON_WM_DRAWITEM()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// COglControl message handlers

void COglControl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CStatic::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void COglControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC * pDC = GetDC();
    HDC hDC = pDC->GetSafeHdc();

    __try
    {
        wglMakeCurrent(hDC, glRC);
        OnDrawItemOGL();
        SwapBuffers(hDC);
        wglMakeCurrent(hDC, NULL);
    }
    __finally
    {
        ReleaseDC(pDC);
    }
}

void COglControl::OnDestroy()
{
    CStatic::OnDestroy();

    if (!glRC) return;

    OnDestroyOGL();

    wglDeleteContext(glRC);
}

void COglControl::OnDestroyOGL()
{
    glFinish();
}

void COglControl::PreSubclassWindow()
{
    CDC * pDC = GetDC();
    HDC hDC = pDC->GetSafeHdc();

    __try
    {
        int pixelformat;

        if (!(pixelformat = ChoosePixelFormat(hDC, &_pfd)))
            return;

        if (!SetPixelFormat(hDC, pixelformat, &_pfd))
            return;

        glRC = wglCreateContext(hDC);
        if (!glRC)
            return;

        OnCreateOGL();
    }
    __finally
    {
        ReleaseDC(pDC);
    }

    CStatic::PreSubclassWindow();
}

void COglControl::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_bLMousePressed = true;
    m_lPressPoint = point;
    SetFocus();
}

void COglControl::OnRButtonDown(UINT nFlags, CPoint point)
{
    m_bRMousePressed = true;
    m_rPressPoint = point;
    SetFocus();
}

void COglControl::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_bLMousePressed = false;
    
    m_rotAngleV += m_rotAngleVDelta;
    m_rotAngleH += m_rotAngleHDelta;
    m_rotAngleVDelta = 0;
    m_rotAngleHDelta = 0;
}

void COglControl::OnRButtonUp(UINT nFlags, CPoint point)
{
    m_bRMousePressed = false;

    m_translateX += m_translateXDelta;
    m_translateY += m_translateXDelta;
    m_translateXDelta = 0;
    m_translateYDelta = 0;
}

void COglControl::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bRMousePressed || m_bLMousePressed)
    {
        CRect b; GetClientRect(&b);
        if (m_bLMousePressed)
        {
            auto r = point - m_lPressPoint;
            m_rotAngleVDelta = (double) r.cx / b.Width() * m_rotAngleVMultiplier;
            m_rotAngleHDelta = (double) r.cy / b.Height() * m_rotAngleHMultiplier;
        }
        if (m_bRMousePressed)
        {
            auto r = point - m_rPressPoint;
            m_translateXDelta = (double) r.cx / b.Width() * m_translateXMultiplier;
            m_translateYDelta = - (double) r.cy / b.Height() * m_translateYMultiplier;
        }
        RedrawWindow();
    }
}

BOOL COglControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
    m_zoom += zDelta * m_zoomMultiplier / WHEEL_DELTA;
    RedrawWindow();
    return 1;
}
