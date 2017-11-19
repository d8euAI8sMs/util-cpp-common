#pragma once

#include <afxwin.h>
#include <afxdialogex.h>
#include <functional>

class CSimulationDialog : public CDialogEx
{

public:

	CSimulationDialog(UINT nIDTemplate, CWnd* pParent);

protected:

    virtual afx_msg LRESULT OnInvoke(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:

    CWinThread * m_pWorkerThread;
    volatile BOOL m_bWorking;

    virtual void Invoke(const std::function < void () > & fn);
    virtual void StartSimulationThread();
    virtual void StopSimulationThread();
    virtual void SuspendSimulationThread();
    virtual void ResumeSimulationThread();

    virtual BOOL DestroyWindow();
    virtual void OnSimulation();
};
