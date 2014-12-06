// 3D_SCAN.h : main header file for the 3D_SCAN application
//

#if !defined(AFX_3D_SCAN_H__D18C2433_B090_41BE_BDA1_36047E2AF1B8__INCLUDED_)
#define AFX_3D_SCAN_H__D18C2433_B090_41BE_BDA1_36047E2AF1B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANApp:
// See 3D_SCAN.cpp for the implementation of this class
//

class CMy3D_SCANApp : public CWinApp
{
public:
	CMy3D_SCANApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy3D_SCANApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMy3D_SCANApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3D_SCAN_H__D18C2433_B090_41BE_BDA1_36047E2AF1B8__INCLUDED_)
