// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__EB02BF30_EAF6_4BC5_9BD4_B266FB8C9988__INCLUDED_)
#define AFX_MAINFRM_H__EB02BF30_EAF6_4BC5_9BD4_B266FB8C9988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//数字摄像机SDK的MVVideo模块头文件
#include "Pcomm.h" //这里添加串口通信的头文件
#include "MVDAILT.h"
#include "Raw2Rgb.h"




//自定义消息ID号
#define WM_SNAP_CHANGE		(WM_USER + 100)

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Resize();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSnapexOpen();
	afx_msg void OnSnapexStart();
	afx_msg void OnSnapexStop();
    afx_msg void ScanSet();
    //afx_msg void OnOK();
	afx_msg void OnStartscan();
	//afx_msg void OnUpdateStartscan(CCmdUI* pCmdUI);
	afx_msg void Scan();
	//afx_msg BOOL data_out();
	afx_msg void OnBtnSend(unsigned char message); 
	afx_msg void OnSnapexClose();
	afx_msg LRESULT OnSnapChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnStartsnap();
	afx_msg void OnStopsnap();
	afx_msg void OnUpdateStartsnap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStopsnap(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:

	HHV	m_hhv;			//数字摄像机句柄
	
	BOOL m_bOpen;		//初始化标志
	BOOL m_bStart;		//启动标志
	
	BITMAPINFO *m_pBmpInfo;		//BITMAPINFO 结构指针，显示图像时使用
	BYTE *m_pRawBuffer;			//采集图像原始数据缓冲区
	BYTE *m_pImageBuffer;		//Bayer转换后缓冲区
    BYTE *DisplayBuffer;

	char m_chBmpBuf[2048];		//BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区

	//颜色查找表
	BYTE m_pLutR[256] ;
	BYTE m_pLutG[256] ;
	BYTE m_pLutB[256] ;
	
	/*
	 *	Snap 回调函数，用户也可以定义为全局函数， 
	 *	如果作为类的成员函数，必须为静态成员函数。
	 */
	static int CALLBACK SnapThreadCallback(HV_SNAP_INFO *pInfo); 
	void SetExposureTime(int nWindWidth,long lTintUpper,long lTintLower);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__060DCAD4_22AD_40C3_855E_BEB2A29F1F8A__INCLUDED_)

