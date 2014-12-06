// 3D_SCANView.h : interface of the CMy3D_SCANView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_3D_SCANVIEW_H__243F0BAC_1DEB_4ACD_B1D0_B589B51AA218__INCLUDED_)
#define AFX_3D_SCANVIEW_H__243F0BAC_1DEB_4ACD_B1D0_B589B51AA218__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMy3D_SCANView : public CView
{
protected: // create from serialization only
	CMy3D_SCANView();
	DECLARE_DYNCREATE(CMy3D_SCANView)

// Attributes
public:
	CMy3D_SCANDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy3D_SCANView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMy3D_SCANView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMy3D_SCANView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in 3D_SCANView.cpp
inline CMy3D_SCANDoc* CMy3D_SCANView::GetDocument()
   { return (CMy3D_SCANDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3D_SCANVIEW_H__243F0BAC_1DEB_4ACD_B1D0_B589B51AA218__INCLUDED_)
