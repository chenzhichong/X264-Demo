
// X264-Demo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CX264DemoApp:
// See X264-Demo.cpp for the implementation of this class
//

class CX264DemoApp : public CWinApp
{
public:
	CX264DemoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CX264DemoApp theApp;