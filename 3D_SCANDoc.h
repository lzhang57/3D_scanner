// 3D_SCANDoc.h : interface of the CMy3D_SCANDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_3D_SCANDOC_H__A0D63A89_3968_42C5_A313_FF1AE5190FF7__INCLUDED_)
#define AFX_3D_SCANDOC_H__A0D63A89_3968_42C5_A313_FF1AE5190FF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMy3D_SCANDoc : public CDocument
{
protected: // create from serialization only
	CMy3D_SCANDoc();
	DECLARE_DYNCREATE(CMy3D_SCANDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy3D_SCANDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMy3D_SCANDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMy3D_SCANDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3D_SCANDOC_H__A0D63A89_3968_42C5_A313_FF1AE5190FF7__INCLUDED_)
