// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include <thread>
#include "Driver.h"

/////////////////////////////////////////////////////////////////////////////
// CHelloAppDlg dialog

#define CDialog CDialogEx

class CHelloAppDlg : public CDialog
{
// Construction
public:
	CHelloAppDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CHelloAppDlg();

// Dialog Data
	enum { IDD = IDD_HELLOAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
protected:
	HICON m_hIcon;

	Driver * m_driver;
	std::thread m_thread_record;
	std::thread m_thread_loop;
	bool m_close;
	bool m_stopped_record;
	bool m_stopped_loop;

	void CaptureAudio();
	void LoopAudio();

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	afx_msg void OnAppOpen();
	afx_msg void OnClose();

	afx_msg void SendStaticDataSample();
	afx_msg void RetrieveStaticDataSample();
	afx_msg void ToggleRecord();
	afx_msg void ToggleLoop();

	afx_msg void StopRecord();
	afx_msg void StopLoop();

	afx_msg void OnDynamicItemClick(int id);
	afx_msg LRESULT OnTrayNotify(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

	NOTIFYICONDATA	m_nid;			// struct for Shell_NotifyIcon args

	void OnTrayContextMenu ();
};

