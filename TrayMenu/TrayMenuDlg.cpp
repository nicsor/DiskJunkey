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
#include "TrayMenu.h"
#include "TrayMenuDlg.h"

#include <chrono>
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UM_TRAYNOTIFY	(WM_USER + 1)

#define INITGUID
#include <guiddef.h>

DEFINE_GUID(MICARRAY1_CUSTOM_NAME,
	0x6994AD04, 0x93EF, 0x11D0, 0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96);


/////////////////////////////////////////////////////////////////////////////
// CTrayMenuDlg dialog

CTrayMenuDlg::CTrayMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayMenuDlg::IDD, pParent)
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Initialize NOTIFYICONDATA
	memset(&m_nid, 0 , sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	
	m_driver = new Driver((LPGUID)&MICARRAY1_CUSTOM_NAME);
	m_stop = True;
}

CTrayMenuDlg::~CTrayMenuDlg ()
{
	m_stop = True;
	if (m_thread.joinable()) {
		m_thread.join();
	}

	m_nid.hIcon = NULL;
	Shell_NotifyIcon (NIM_DELETE, &m_nid);
}

void CTrayMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTrayMenuDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_APP_OPEN, OnAppOpen)
	ON_COMMAND(ID_ITEM1, OnItem1)
	ON_COMMAND(ID_ITEM2, OnItem2)
	ON_COMMAND(ID_ITEM3, OnItem3)
	ON_COMMAND(ID_ITEM4, OnItem4)
	ON_UPDATE_COMMAND_UI(ID_ITEM4, OnUpdateItem4)
	ON_MESSAGE(UM_TRAYNOTIFY, OnTrayNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrayMenuDlg message handlers

BOOL CTrayMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bValidString;
		CString strAboutMenu;
		bValidString = strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Set tray notification window:
	m_nid.hWnd = GetSafeHwnd ();
	m_nid.uCallbackMessage = UM_TRAYNOTIFY;

	// Set tray icon and tooltip:
	m_nid.hIcon = m_hIcon;

	CString strToolTip = _T("MFCTrayDemo");
	_tcsncpy_s (m_nid.szTip, strToolTip, strToolTip.GetLength ());

	Shell_NotifyIcon (NIM_ADD, &m_nid);

	CMFCToolBar::AddToolBarForImageCollection (IDR_MENUIMAGES);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrayMenuDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrayMenuDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrayMenuDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CTrayMenuDlg::OnTrayNotify(WPARAM /*wp*/, LPARAM lp)
{
	UINT uiMsg = (UINT) lp;

	switch (uiMsg)
	{
	case WM_RBUTTONUP:
		OnTrayContextMenu ();
		return 1;

	case WM_LBUTTONDBLCLK:
		ShowWindow (SW_SHOWNORMAL);
		return 1;
	}

	return 0;
}

void CTrayMenuDlg::OnTrayContextMenu ()
{
	CPoint point;
	::GetCursorPos (&point);

	CMenu menu;
	menu.LoadMenu (IDR_MENU1);

	CMFCPopupMenu::SetForceShadow (TRUE);

	HMENU hMenu = menu.GetSubMenu (0)->Detach ();
	CMFCPopupMenu* pMenu = theApp.GetContextMenuManager()->ShowPopupMenu(hMenu, point.x, point.y, this, TRUE);

	pMenu->SetForegroundWindow ();
}

void CTrayMenuDlg::OnAppAbout() 
{
}

void CTrayMenuDlg::OnAppExit() 
{
	PostMessage (WM_CLOSE);
}

void CTrayMenuDlg::OnAppOpen() 
{
	ShowWindow (SW_SHOWNORMAL);
}

void CTrayMenuDlg::OnItem1() 
{
	char message[256];
	char* buffer = "Ana are mere";
	bool status = m_driver->send_data(buffer, strlen(buffer));

	snprintf(message, sizeof(message), "Sent: [%s] [%d] status:[%d]", buffer, strlen(buffer), status);
	::MessageBox (NULL, message, _T("Send"), MB_OK);
}


void CTrayMenuDlg::OnItem2() 
{
	char message[256];
	char buffer[256];
	int len = m_driver->retrieve_data(buffer, sizeof(buffer));

	snprintf(message, sizeof(message), "Retrieved: [%s] [%d]", buffer, len);
	::MessageBox (NULL, message, _T("Retrieve"), MB_OK);
}

void CTrayMenuDlg::capture_audio()
{
	char dataBuffer[4 * 1024]; // TODO: must love them magic numbers
	BYTE waveFormat[128];  // WAVEFORMATEX*

	if (m_driver->retrieve_speaker_format((WAVEFORMATEX*)waveFormat) > 0) {
	}

	DataFile file("fisier.wav", (WAVEFORMATEX*)waveFormat);

	while(!m_stop) {
		int dataRead = m_driver->retrieve_speaker_data(dataBuffer, sizeof(dataBuffer));
		if (dataRead) {
			file.write_data(dataBuffer, dataRead);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void CTrayMenuDlg::OnItem3() 
{
	if (m_stop) {
		m_stop = false;
		m_thread = std::thread(std::bind(&CTrayMenuDlg::capture_audio, this));

		::MessageBox(NULL, _T("Listening thread started"), _T("TrayMenu"), MB_OK);
	}
	else {
		m_stop = true;

		if (m_thread.joinable()) {
			m_thread.join();
		}

		::MessageBox(NULL, _T("Listening thread stopped"), _T("TrayMenu"), MB_OK);
	}

}

static BOOL bIsItem4Checked = TRUE;

void CTrayMenuDlg::OnItem4() 
{
	bIsItem4Checked = !bIsItem4Checked;
}

void CTrayMenuDlg::OnUpdateItem4(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (bIsItem4Checked);
}

