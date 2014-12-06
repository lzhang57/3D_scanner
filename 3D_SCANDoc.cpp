// 3D_SCANDoc.cpp : implementation of the CMy3D_SCANDoc class
//

#include "stdafx.h"
#include "3D_SCAN.h"

#include "3D_SCANDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANDoc

IMPLEMENT_DYNCREATE(CMy3D_SCANDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy3D_SCANDoc, CDocument)
	//{{AFX_MSG_MAP(CMy3D_SCANDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANDoc construction/destruction

CMy3D_SCANDoc::CMy3D_SCANDoc()
{
	// TODO: add one-time construction code here

}

CMy3D_SCANDoc::~CMy3D_SCANDoc()
{
}

BOOL CMy3D_SCANDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANDoc serialization

void CMy3D_SCANDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANDoc diagnostics

#ifdef _DEBUG
void CMy3D_SCANDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy3D_SCANDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANDoc commands

void CMy3D_SCANDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString str="3Dº§π‚…®√Ë“«-by’≈¡º‚˝";
	CDocument::SetTitle(str);
}
