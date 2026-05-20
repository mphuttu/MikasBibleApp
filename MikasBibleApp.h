
// MikasBibleApp.h : main header file for the MikasBibleApp application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMikasBibleAppApp:
// See MikasBibleApp.cpp for the implementation of this class
//

class CMikasBibleAppApp : public CWinApp
{
public:
	CMikasBibleAppApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMikasBibleAppApp theApp;
