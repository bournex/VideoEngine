#include "stdafx.h"
#ifndef _LINUX
#include "PVSDialog.h"
#include "pvs.h"

//////////////////////////////////////////////////////////////////////////
// PVS对话框的消息处理程序
INT_PTR CALLBACK PvsDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		break;

	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		::PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 100, 0);
		break;
	}
	return (INT_PTR)FALSE;
}
//////////////////////////////////////////////////////////////////////////

CPVSDialog::CPVSDialog(void)
{
	m_hParent = NULL;
	m_hWnd = NULL;
	for (int i=0; i<MAX_PLAY_CHANNEL; ++i)
	{
		m_hPlayWnds[i] = NULL;
	}
}

CPVSDialog::~CPVSDialog(void)
{

}

BOOL CPVSDialog::Create(HWND hParent, int nPlayWnd)
{
	HINSTANCE hInst = (HINSTANCE)thePVSModule;
	m_hWnd = ::CreateDialog(hInst, MAKEINTRESOURCE(IDD), hParent, PvsDialogProc);

	if (!m_hWnd)
		return FALSE;

	for (int i=0; i<MAX_PLAY_CHANNEL; ++i)
	{
		m_hPlayWnds[i] = ::GetDlgItem(m_hWnd, IDC_PLAY_WND1+i);
	}

	m_hParent = hParent;
	return TRUE;
}

BOOL CPVSDialog::Destroy()
{
	BOOL bRet = FALSE;
	if (m_hWnd)
	{
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
	}

	return bRet;
}

int CPVSDialog::Show(BOOL bShow)
{
	if (!m_hWnd)
		return 0;

	int nCmdShow;
	if (bShow)
	{
		nCmdShow = SW_SHOW;
	}
	else
	{
		nCmdShow = SW_HIDE;
	}
	
	::ShowWindow(m_hWnd, nCmdShow);

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	m_hWnd = NULL;
	return (int)msg.wParam;
}
#endif
