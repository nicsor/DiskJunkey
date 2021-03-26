// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "HelloApp.h"
#include "HelloAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelloAppApp

BEGIN_MESSAGE_MAP(CHelloAppApp, CWinAppEx)
	ON_COMMAND(ID_HELP, CWinAppEx::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelloAppApp construction

CHelloAppApp::CHelloAppApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHelloAppApp object

CHelloAppApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHelloAppApp initialization

BOOL CHelloAppApp::InitInstance()
{
	AfxEnableControlContainer();

	InitContextMenuManager ();

	CMFCVisualManager::SetDefaultManager (RUNTIME_CLASS (CMFCVisualManagerOffice2003));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CHelloAppDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = (int) dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
