// 3D_SCANView.cpp : implementation of the CMy3D_SCANView class
//

#include "stdafx.h"
#include "3D_SCAN.h"

#include "3D_SCANDoc.h"
#include "3D_SCANView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANView

IMPLEMENT_DYNCREATE(CMy3D_SCANView, CView)

BEGIN_MESSAGE_MAP(CMy3D_SCANView, CView)
	//{{AFX_MSG_MAP(CMy3D_SCANView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANView construction/destruction

CMy3D_SCANView::CMy3D_SCANView()
{
	// TODO: add construction code here

}

CMy3D_SCANView::~CMy3D_SCANView()
{
}

BOOL CMy3D_SCANView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANView drawing

void CMy3D_SCANView::OnDraw(CDC* pDC)
{
	CMy3D_SCANDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANView printing

BOOL CMy3D_SCANView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMy3D_SCANView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMy3D_SCANView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANView diagnostics

#ifdef _DEBUG
void CMy3D_SCANView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3D_SCANView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3D_SCANDoc* CMy3D_SCANView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3D_SCANDoc)));
	return (CMy3D_SCANDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMy3D_SCANView message handlers
