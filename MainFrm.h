// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__EB02BF30_EAF6_4BC5_9BD4_B266FB8C9988__INCLUDED_)
#define AFX_MAINFRM_H__EB02BF30_EAF6_4BC5_9BD4_B266FB8C9988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//���������SDK��MVVideoģ��ͷ�ļ�
#include "Pcomm.h" //������Ӵ���ͨ�ŵ�ͷ�ļ�
#include "MVDAILT.h"
#include "Raw2Rgb.h"




//�Զ�����ϢID��
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

	HHV	m_hhv;			//������������
	
	BOOL m_bOpen;		//��ʼ����־
	BOOL m_bStart;		//������־
	
	BITMAPINFO *m_pBmpInfo;		//BITMAPINFO �ṹָ�룬��ʾͼ��ʱʹ��
	BYTE *m_pRawBuffer;			//�ɼ�ͼ��ԭʼ���ݻ�����
	BYTE *m_pImageBuffer;		//Bayerת���󻺳���
    BYTE *DisplayBuffer;

	char m_chBmpBuf[2048];		//BIMTAPINFO �洢��������m_pBmpInfo��ָ��˻�����

	//��ɫ���ұ�
	BYTE m_pLutR[256] ;
	BYTE m_pLutG[256] ;
	BYTE m_pLutB[256] ;
	
	/*
	 *	Snap �ص��������û�Ҳ���Զ���Ϊȫ�ֺ����� 
	 *	�����Ϊ��ĳ�Ա����������Ϊ��̬��Ա������
	 */
	static int CALLBACK SnapThreadCallback(HV_SNAP_INFO *pInfo); 
	void SetExposureTime(int nWindWidth,long lTintUpper,long lTintLower);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__060DCAD4_22AD_40C3_855E_BEB2A29F1F8A__INCLUDED_)

