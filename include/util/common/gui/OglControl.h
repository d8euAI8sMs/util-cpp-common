#pragma once

#include <functional>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// COglControl

class COglControl : public CStatic
{
    DECLARE_DYNAMIC(COglControl)

private:

    HGLRC glRC;
    bool m_bLMousePressed;
    bool m_bRMousePressed;
    CPoint m_lPressPoint;
    CPoint m_rPressPoint;
    double m_rotAngleH;
    double m_rotAngleV;
    double m_rotAngleHDelta;
    double m_rotAngleVDelta;
    double m_zoom;
    double m_translateX;
    double m_translateY;
    double m_translateXDelta;
    double m_translateYDelta;

public:

    double m_rotAngleHMultiplier;
    double m_rotAngleVMultiplier;
    double m_translateXMultiplier;
    double m_translateYMultiplier;
    double m_zoomMultiplier;

    std::function < void () > painter;

public:

    HGLRC GetGLRC() { return glRC; }
    double GetRotAngleH() { return m_rotAngleH + m_rotAngleHDelta; }
    double GetRotAngleV() { return m_rotAngleV + m_rotAngleVDelta; }
    double GetZoom() { return m_zoom; }
    double GetTranslateX() { return m_translateX + m_translateXDelta; }
    double GetTranslateY() { return m_translateY + m_translateYDelta; }

public:
    COglControl();
    virtual ~COglControl();
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

protected:
    DECLARE_MESSAGE_MAP()
public:
    virtual void OnCreateOGL() { }
    virtual void OnDrawItemOGL() { if (painter) painter(); }
    virtual void OnDestroyOGL();
    afx_msg void OnDestroy();
    virtual void PreSubclassWindow();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
};
