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

#include <chrono>
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UM_TRAYNOTIFY	(WM_USER + 1)

#define INITGUID
#include <guiddef.h>

DEFINE_GUID(MICARRAY1_CUSTOM_NAME,
	0x6994AD04, 0x93EF, 0x11D0, 0xA3, 0xCC, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96);


#define ENABLE_DRIVER_POOLING TRUE

enum {
	ID_EV_SEND_DATA = 12345,
	ID_EV_RETRIEVE_DATA,
	ID_EV_TOGGLE_RECORD,
	ID_EV_TOGGLE_LOOP,
	ID_EV_SHOW_APP,
	ID_EV_EXIT,
	ID_EV_FIRST_DYNAMIC_ITEM
};

const int MAX_NUMBER_OF_DYNAMIC_ITEMS = 7;

bool dynamicItems[MAX_NUMBER_OF_DYNAMIC_ITEMS] = {false};


/////////////////////////////////////////////////////////////////////////////
// CHelloAppDlg dialog

CHelloAppDlg::CHelloAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelloAppDlg::IDD, pParent),
	m_close(false),
	m_stopped_record(true),
	m_stopped_loop(true)
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Initialize NOTIFYICONDATA
	memset(&m_nid, 0 , sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	

#if ENABLE_DRIVER_POOLING
	m_driver = new Driver((LPGUID)&MICARRAY1_CUSTOM_NAME);
#endif
}

CHelloAppDlg::~CHelloAppDlg ()
{
	StopRecord();
	StopLoop();

	m_nid.hIcon = NULL;
	Shell_NotifyIcon (NIM_DELETE, &m_nid);
}

void CHelloAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHelloAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_TRAYNOTIFY, OnTrayNotify)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CHelloAppDlg::OnInitDialog()
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

void CHelloAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CHelloAppDlg::OnClose()
{
	if (!m_close) {
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialog::OnClose();
	}
}

void CHelloAppDlg::OnPaint()
{
	const ULONG paddingRight = 10;
	const ULONG paddingBottom = 50;

	CWnd::ModifyStyle(WS_SYSMENU | WS_CAPTION, 0);

	// Get this window's area
	CRect dialogInfo;
	CRect desktopInfo;

	GetClientRect(&dialogInfo);
	GetDesktopWindow()->GetClientRect(&desktopInfo);

	// Redraw the window in the right corner
	SetWindowPos(NULL, desktopInfo.Width() - dialogInfo.Width()- paddingRight, desktopInfo.Height() - dialogInfo.Height()- paddingBottom, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	CDialog::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHelloAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CHelloAppDlg::OnTrayNotify(WPARAM /*wp*/, LPARAM lp)
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

BOOL CHelloAppDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nCode == CN_COMMAND)
	{
		bool isManaged = true;

		switch (nID) {
			case(ID_EV_SEND_DATA):
				SendStaticDataSample();
				break;
			case(ID_EV_RETRIEVE_DATA):
				RetrieveStaticDataSample();
				break;
			case(IDC_TOGGLE_RECORD):
			case(ID_EV_TOGGLE_RECORD):
				ToggleRecord();
				break;
			case(IDC_TOGGLE_LOOP):
			case(ID_EV_TOGGLE_LOOP):
				ToggleLoop();
				break;
			case(ID_EV_SHOW_APP):
				OnAppOpen();
				break;
			case(ID_EV_EXIT):
				OnAppExit();
				break;
			case(IDC_HIDE):
				OnClose();
				break;
			default:
			{
				if (nID >= ID_EV_FIRST_DYNAMIC_ITEM && nID < ID_EV_FIRST_DYNAMIC_ITEM + MAX_NUMBER_OF_DYNAMIC_ITEMS)
				{
					OnDynamicItemClick(nID - ID_EV_FIRST_DYNAMIC_ITEM);
				}
				else {
					isManaged = false;
				}
			}
		}

		if (isManaged) {
			return true;
		}
	}

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CHelloAppDlg::OnDynamicItemClick(int id)
{
	dynamicItems[id] = !dynamicItems[id];
}

void CHelloAppDlg::OnTrayContextMenu()
{
	CPoint point;
	::GetCursorPos(&point);

	CMenu menu;
	menu.CreatePopupMenu();

	if (!IsWindowVisible()) {
		menu.AppendMenu(MF_STRING, ID_EV_SHOW_APP, "Show &App");
	}

	menu.AppendMenu(MF_STRING, ID_EV_SEND_DATA, "&Send Data To Driver");
	menu.AppendMenu(MF_STRING, ID_EV_RETRIEVE_DATA, "&Get Data From Driver");

	{
		char message[256];
		snprintf(message, sizeof(message), "%s listening &thread", m_stopped_record ? "Start" : "Stop");
		menu.AppendMenu(MF_STRING, ID_EV_TOGGLE_RECORD, message);
	}

	{
		char message[256];
		snprintf(message, sizeof(message), "%s loop &thread", m_stopped_loop ? "Start" : "Stop");
		menu.AppendMenu(MF_STRING, ID_EV_TOGGLE_LOOP, message);
	}

	CMenu* chmenu = new CMenu;
	chmenu->CreatePopupMenu();
	for (short i = 0; i < sizeof(dynamicItems); i++) {
		int eventId = ID_EV_FIRST_DYNAMIC_ITEM + i;

		char message[256];
		snprintf(message, sizeof(message), "Dynamic item %d", i + 1);

		UINT flags = dynamicItems[i] ? MF_CHECKED : MF_UNCHECKED;
		chmenu->AppendMenu(flags | MF_STRING, eventId, message);
	}

	menu.AppendMenu(MF_POPUP, (UINT)chmenu->m_hMenu, "Dynamic entries");
	menu.AppendMenu(MF_STRING, ID_EV_EXIT, "&Exit");


	HMENU hMenu = menu.Detach ();
	CMFCPopupMenu* pMenu = theApp.GetContextMenuManager()->ShowPopupMenu(hMenu, point.x, point.y, this, TRUE);

	pMenu->SetForegroundWindow ();
}

void CHelloAppDlg::OnAppAbout() 
{
	char* buffer = "Just a simple demo app to highlight how to interact with the gui. Formless with dynamic tray. So, with the titlebar hidden this message won't be seen. Hmm... Decisions, decisions ...";
	::MessageBox(NULL, buffer, _T("Send"), MB_OK);
}

void CHelloAppDlg::OnAppExit() 
{
	m_close = true;
	PostMessage (WM_CLOSE);
}

void CHelloAppDlg::OnAppOpen() 
{
	ShowWindow (SW_SHOWNORMAL);
}

void CHelloAppDlg::SendStaticDataSample() 
{
#if ENABLE_DRIVER_POOLING
	char message[256];
	char* buffer = "Ana are mere";
	bool status = m_driver->send_data(buffer, strlen(buffer) + 1); // also the null character

	snprintf(message, sizeof(message), "Sent: [%s] [%d] status:[%d]", buffer, strlen(buffer), status);
	::MessageBox (NULL, message, _T("Send"), MB_OK);
#endif
}


void CHelloAppDlg::RetrieveStaticDataSample() 
{
#if ENABLE_DRIVER_POOLING
	char message[256];
	char buffer[256];
	int len = m_driver->retrieve_data(buffer, sizeof(buffer));

	snprintf(message, sizeof(message), "Retrieved: [%s] [%d]", buffer, len);
	::MessageBox (NULL, message, _T("Retrieve"), MB_OK);
#endif
}

void CHelloAppDlg::CaptureAudio()
{
	char dataBuffer[4 * 1024]; // TODO: must love them magic numbers
	BYTE waveFormat[128];  // WAVEFORMATEX*

	if (m_driver->retrieve_speaker_format((WAVEFORMATEX*)waveFormat) > 0) {
	}

	DataFile file("fisier.wav", (WAVEFORMATEX*)waveFormat);

	while(!m_stopped_record) {
		int dataRead = m_driver->retrieve_speaker_data(dataBuffer, sizeof(dataBuffer));
		if (dataRead) {
			file.write_data(dataBuffer, dataRead);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void CHelloAppDlg::ToggleRecord() 
{
#if ENABLE_DRIVER_POOLING
	if (m_stopped_record) {
		StopLoop();

		m_stopped_record = false;
		m_thread_record = std::thread(std::bind(&CHelloAppDlg::CaptureAudio, this));

		//::MessageBox(NULL, _T("Listening thread started"), _T("HelloApp"), MB_OK);
	}
	else {
		StopRecord();
		//::MessageBox(NULL, _T("Listening thread stopped"), _T("HelloApp"), MB_OK);
	}
#endif
}

void CHelloAppDlg::LoopAudio()
{
	bool sendMicData = false;
	char micBuffer[8 * 1024];
	DWORD micBufferOffset = 0;

	while (!m_stopped_loop) {
		micBufferOffset += m_driver->retrieve_speaker_data(micBuffer + micBufferOffset, sizeof(micBuffer) / 2);

		// => 5ms delay
		if (sendMicData && micBufferOffset > 0) {
			m_driver->send_mic_data(micBuffer, micBufferOffset);
			micBufferOffset = 0;
		}

		sendMicData = !sendMicData;

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void CHelloAppDlg::ToggleLoop()
{
#if ENABLE_DRIVER_POOLING
	if (m_stopped_loop) {
		StopRecord();

		m_stopped_loop = false;
		m_thread_record = std::thread(std::bind(&CHelloAppDlg::LoopAudio, this));

		//::MessageBox(NULL, _T("Loop thread started"), _T("HelloApp"), MB_OK);
	}
	else {
		StopLoop();
		//::MessageBox(NULL, _T("Loop thread stopped"), _T("HelloApp"), MB_OK);
	}
#endif
}

void CHelloAppDlg::StopRecord()
{
	m_stopped_record = true;
	if (m_thread_record.joinable()) {
		m_thread_record.join();
	}
}

void CHelloAppDlg::StopLoop()
{
	m_stopped_loop = true;
	if (m_thread_loop.joinable()) {
		m_thread_loop.join();
	}
}