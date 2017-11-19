#include "stdafx.h"

#include <util/common/gui/SimulationDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_INVOKE WM_USER + 1234

static UINT SimulationThreadProc(LPVOID pParam)
{
    ((CSimulationDialog *) pParam)->OnSimulation();
    return 0;
}

CSimulationDialog::CSimulationDialog(UINT nIDTemplate, CWnd* pParent)
	: CDialogEx(nIDTemplate, pParent)
    , m_pWorkerThread(NULL)
    , m_bWorking(FALSE)
{
}

BEGIN_MESSAGE_MAP(CSimulationDialog, CDialogEx)
    ON_MESSAGE(WM_INVOKE, &CSimulationDialog::OnInvoke)
END_MESSAGE_MAP()

void CSimulationDialog::StartSimulationThread()
{
    if (this->m_bWorking)
    {
        return;
    }
    this->m_bWorking = TRUE;
    this->m_pWorkerThread = AfxBeginThread(&SimulationThreadProc, this, 0, 0, CREATE_SUSPENDED);
    this->m_pWorkerThread->m_bAutoDelete = FALSE;
    ResumeThread(this->m_pWorkerThread->m_hThread);
}

void CSimulationDialog::StopSimulationThread()
{
    if (this->m_bWorking)
    {
        this->m_bWorking = FALSE;
        while (MsgWaitForMultipleObjects(
            1, &this->m_pWorkerThread->m_hThread, FALSE, INFINITE, QS_SENDMESSAGE) != WAIT_OBJECT_0)
        {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        delete this->m_pWorkerThread;
        this->m_pWorkerThread = NULL;
    }
}

void CSimulationDialog::SuspendSimulationThread()
{
    if (!this->m_bWorking) return;
    SuspendThread(this->m_pWorkerThread);
}

void CSimulationDialog::ResumeSimulationThread()
{
    if (!this->m_bWorking) return;
    ResumeThread(this->m_pWorkerThread);
}

void CSimulationDialog::Invoke(const std::function < void () > & fn)
{
    SendMessage(WM_INVOKE, 0, (LPARAM)&fn);
}

afx_msg LRESULT CSimulationDialog::OnInvoke(WPARAM wParam, LPARAM lParam)
{
    (*(const std::function < void () > *) lParam)();
    return 0;
}

BOOL CSimulationDialog::DestroyWindow()
{
    StopSimulationThread();

    return CDialogEx::DestroyWindow();
}

void CSimulationDialog::OnSimulation()
{
    Invoke([&] () { this->m_bWorking = FALSE; });
}
