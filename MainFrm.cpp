// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "3D_SCAN.h"
#include <math.h>
#include "MainFrm.h"
#include "cv.h"
#include "highgui.h"
#include <io.h>  
#include <fcntl.h>  



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//void InitConsole() ;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

#define WM_READY WM_USER+500 //�Զ�����Ϣ

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
    ON_MESSAGE(WM_SNAP_CHANGE, OnSnapChange)
	ON_WM_CLOSE()
	ON_COMMAND(ID_STARTSNAP, OnStartsnap)
	ON_COMMAND(ID_STOPSNAP, OnStopsnap)
    ON_COMMAND(IDC_ScanSet, ScanSet)
    //ON_COMMAND(IDC_BUTTON_OK, OnOK)
	ON_MESSAGE(WM_READY, Scan)
	ON_COMMAND(ID_EDIT_ScanStart,OnStartscan)	
	//ON_UPDATE_COMMAND_UI(ID_STARTSNAP, OnUpdateStartscan)

    ON_UPDATE_COMMAND_UI(ID_STARTSNAP, OnUpdateStartsnap)
	ON_UPDATE_COMMAND_UI(ID_STOPSNAP, OnUpdateStopsnap)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define  MY_ZERO 0.000000001

//const
const HV_RESOLUTION Resolution = RES_MODE0;
const HV_SNAP_MODE SnapMode = CONTINUATION;
const HV_BAYER_LAYOUT Layout = BAYER_GR;
const HV_BAYER_CONVERT_TYPE ConvertType = BAYER2RGB_NEIGHBOUR;
const long Gain = 8;
const long ExposureTint_Upper = 60;
const long ExposureTint_Lower = 1000;

const long ADCLevel = ADC_LEVEL2;
const int XStart = 0;
const int YStart = 0;
const int Width =1280; //800;
const int Height = 1024;//600;
static bool canny=false; 

unsigned int my_flag;

unsigned char Port=10;
//double data[1000][Height];

#define BaudRate B9600//������
#define DataBits BIT_8 //����λ
#define Parity P_NONE //Ч��λ
#define StopBits STOP_1 //ֹͣλ

/*******���ǲ�����*******/
#define FS             1585  
#define PIX_SIZE       0.005396
#define PIX_OFF        0.7968
#define DIST_OFF       22.65
#define LaserAngle     1.1345
#define f              8
#define rotation_r     100
#define PI             3.1415927

/******************************/

int    Scan_num;   //ɨ�����к�
#define Num 2
#define num  '2'
#define  Scan_step     0.1*PI/180  //ɨ�貽���Ƕ�
#define    Scan_total  250//500/Num         //ɨ���ܴ���

#define   dataout    "C:\\Users\\ZLK\\Desktop\\output\\data.asc"
/*****��������ṹ������*****/
struct result
{
  float X;
  float  Y;
  float  Z;
}Scan_result[Scan_total][Height];
/***********************************************************/
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	//InitConsole() ;
	/*
	 *	��ʼ�����г�Ա������ͬʱ�����������
	 */

	HVSTATUS status = STATUS_OK;
	
	m_bStart		= FALSE;

	m_pBmpInfo		= NULL;
	m_pRawBuffer	= NULL;
	m_pImageBuffer	= NULL;
    DisplayBuffer	= NULL;

	for(int i=0;i<256;i++)
	{
		m_pLutR[i] = i;
		m_pLutG[i] = i;
		m_pLutB[i] = i;
	}

	//	����������� 1
	status = BeginHVDevice(1, &m_hhv);
	//	���麯��ִ��״̬�����ʧ�ܣ��򷵻ش���״̬��Ϣ��
	HV_VERIFY(status);
	
}

CMainFrame::~CMainFrame()
{
	HVSTATUS status = STATUS_OK;
	
	//	�ر�������������ͷ�����������ڲ���Դ
	status = EndHVDevice(m_hhv);
	HV_VERIFY(status);

	//	����ͼ�񻺳���
	delete []m_pRawBuffer;
	delete []m_pImageBuffer;
    delete []DisplayBuffer;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
		/*
		*	��ʼ�����������Ӳ��״̬���û�Ҳ����������λ�ó�ʼ�������������
		*	��Ӧ��֤����������Ѿ��򿪣������û���Ӧ�ó����ʼ��ʱ��
		*	ͬʱ��ʼ�����������Ӳ����
	*/
	
	//	��������������ֱ���
	HVSetResolution(m_hhv, Resolution);		
	
	//	�ɼ�ģʽ������ CONTINUATION(����)��TRIGGER(�ⴥ��)
	HVSetSnapMode(m_hhv, SnapMode);
	
	//  ���ø�������������
	for (int i = 0; i < 4; i++){
		HVAGCControl(m_hhv, RED_CHANNEL + i, Gain);
	}

	//	�����ع�ʱ��
	SetExposureTime(Width,ExposureTint_Upper,ExposureTint_Lower);	

	//  ����ADC�ļ���
	HVADCControl(m_hhv, ADC_BITS, ADCLevel);
	
	/*
	*	��Ƶ������ڣ�����Ƶ�����Χ���������ȡֵ��Χ���������봰�ڷ�Χ���ڣ�
	*  ��Ƶ�������Ͻ�X����ʹ��ڿ��ӦΪ4�ı��������Ͻ�Y����ʹ��ڸ߶�ӦΪ2�ı���
	*	������ڵ���ʼλ��һ������Ϊ(0, 0)���ɡ�
	*/
	HVSetOutputWindow(m_hhv, XStart, YStart, Width, Height);
	
	//	m_pBmpInfo��ָ��m_chBmpBuf���������û������Լ�����BTIMAPINFO������	
	m_pBmpInfo								= (BITMAPINFO *)m_chBmpBuf;
	//	��ʼ��BITMAPINFO �ṹ���˽ṹ�ڱ���bmp�ļ�����ʾ�ɼ�ͼ��ʱʹ��
	m_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	//	ͼ���ȣ�һ��Ϊ������ڿ��
	m_pBmpInfo->bmiHeader.biWidth			= Width/2;
	//	ͼ���ȣ�һ��Ϊ������ڸ߶�
	m_pBmpInfo->bmiHeader.biHeight			= Height/2;
	
	/*
	*	��������һ����ͬ��
	*	���ڵ���8λ��λͼ����Ӧ������Ӧ��λͼ��ɫ��
	*/
	m_pBmpInfo->bmiHeader.biPlanes			= 1;
	m_pBmpInfo->bmiHeader.biBitCount		= 24;
	m_pBmpInfo->bmiHeader.biCompression		= BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage		= 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biClrUsed			= 0;
	m_pBmpInfo->bmiHeader.biClrImportant	= 0;
	
	/*
	*	����ԭʼͼ�񻺳�����һ�������洢�ɼ�ͼ��ԭʼ����
	*  һ��ͼ�񻺳�����С��������ڴ�С����Ƶ��ʽȷ����
	*/
	m_pRawBuffer = new BYTE[Width * Height];
	ASSERT(m_pRawBuffer);
	
	/*
	����Bayerת����ͼ�����ݻ���
	*/
	m_pImageBuffer = new BYTE[Width * Height * 3];
    DisplayBuffer = new BYTE[Width/2 * Height/2 * 3];
	ASSERT(m_pImageBuffer);
    ASSERT(DisplayBuffer);

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/***********************************************************/
void CMainFrame::OnSnapexOpen() 
{
	// TODO: Add your command handler code here
	HVSTATUS status = STATUS_OK;

	/*
	 *	��ʼ������������ɼ�ͼ���ڴ�Ŀ��ƣ�
	 *	ָ���ص�����SnapThreadCallback���û�����m_hWnd
	 */
	status = HVOpenSnap(m_hhv, SnapThreadCallback, m_hWnd);					
	HV_VERIFY(status);
}
/***********************************************************/
void CMainFrame::OnSnapexStart() 
{
	// TODO: Add your command handler code here
	HVSTATUS status = STATUS_OK;

	/*
	 *	��������������ɼ�ͼ���ڴ�
	 */
	BYTE *ppBuf[1];
	ppBuf[0] = m_pRawBuffer;
	status = HVStartSnap(m_hhv, ppBuf,1);

	HV_VERIFY(status);
	if (HV_SUCCESS(status)) {
		m_bStart = TRUE;
	}
}
/***********************************************************/
void CMainFrame::OnSnapexStop() 
{
	// TODO: Add your command handler code here
	HVSTATUS status =STATUS_OK;
	
	//	ֹͣ�ɼ�ͼ���ڴ棬�����ٴε���HVStartSnapEx��������������ɼ�
	status = HVStopSnap(m_hhv);
	HV_VERIFY(status);
	if (HV_SUCCESS(status)) {
		m_bStart = FALSE;
	}
}
/***********************************************************/
void CMainFrame::OnSnapexClose() 
{
	// TODO: Add your command handler code here
	HVSTATUS status = STATUS_OK;

	/*
	 *	��ֹ����������ɼ�ͼ���ڴ棬ͬʱ�ͷ����вɼ�������
	 *	�ٴ���������������ɼ����������³�ʼ��	
	 */
	status = HVCloseSnap(m_hhv);
	HV_VERIFY(status);

	if (HV_SUCCESS(status)) {
		m_bOpen		= FALSE;
		m_bStart	= FALSE;
	}
}
/***********************************************************/
int CALLBACK CMainFrame::SnapThreadCallback(HV_SNAP_INFO *pInfo)
{
	HWND hwnd = (HWND)(pInfo->pParam);
	
	/*
	 *	�����Զ�����ϢWM_SNAP_EX_CHANGE�������ڣ�	
	 *	ͬʱ���뵱ǰ���Դ����ͼ�����
	 *	ע�⣺��SendMessage������Ϣ������ȴ���Ϣ������Ϻ󣬲����˳�����SendMessage����
	 */
	::SendMessage(hwnd, WM_SNAP_CHANGE, 0, 0);

	return 1;
}


/***********************************************************/
LRESULT CMainFrame::OnSnapChange(WPARAM wParam, LPARAM lParam)
{
	HVSTATUS status = STATUS_OK;
	
	CView *pView		= GetActiveView();		//��ȡ��ǰVIEW��ͼ
	CDC *pDC			= pView->GetDC();		//�õ�VIEW��DC
	
	//	��ԭʼͼ�����ݽ���Bayerת����ת����Ϊ24λ��
	ConvertBayer2Rgb(m_pImageBuffer,m_pRawBuffer,Width,Height,ConvertType,m_pLutR,m_pLutG,m_pLutB,true,Layout);
    //ͬʱ��ԭʼ���ݽ������·�ת	
  	Resize();
   
    //����ͼ�ͻ�����ʾͼ��
	StretchDIBits(pDC->GetSafeHdc(),
					0,						
					0,
					Width/2,					//�Դ��ڿ��
					Height/2,					//��ʾ���ڸ߶�
					0,
					0,
					Width/2,					//ͼ����
					Height/2,					//ͼ��߶�
					DisplayBuffer,			//ͼ�񻺳���
					m_pBmpInfo,				//BMPͼ��������Ϣ
					DIB_RGB_COLORS,
					SRCCOPY
					);	
	
    
	pView->ReleaseDC(pDC);
	
	return 1;
}
/***********************************************************/
void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	/*
	 *	�û���û��ͨ���˵��������ر�����������ɼ���	
	 *	��ֱ�ӹر�Ӧ�ó���ʱ��Ӧ��֤����������ɼ����ر�
	 */
	if (m_bOpen) {
		HVCloseSnap(m_hhv);
	}

	CFrameWnd::OnClose();
}
/***********************************************************/
void CMainFrame::SetExposureTime(int nWindWidth,long lTintUpper,long lTintLower)
{
	int size = sizeof(HVTYPE);
	HVTYPE type;
	HVGetDeviceInfo(m_hhv,DESC_DEVICE_TYPE, &type, &size);	
	
	//When outputwindow changes, change the exposure 
	//��ο��ع�ϵ��ת����ʽ
	long lClockFreq = 24000000; 
	int nOutputWid = nWindWidth;
	double dExposure = 0.0;
	double dTint = max((double)lTintUpper/(double)lTintLower,MY_ZERO);
	if(type == MV1300UCTYPE || type == MV1301UCTYPE)	
	{
		long lTb = 0;
		dExposure = (dTint* lClockFreq + 180.0)/((double)nOutputWid + 244.0 + lTb);
	}
	else
	{		
		long lTb = 0;
		dExposure = (dTint* lClockFreq + 180.0)/((double)nOutputWid + 305.0 + lTb) + 1 ;
	}
	
	if (dExposure > 16383) 
		dExposure = 16383;
	HVAECControl(m_hhv, AEC_EXPOSURE_TIME, (long)dExposure);
	
}
/***********************************************************/
void CMainFrame::OnStartsnap() 
{
	// TODO: Add your command handler code here
    OnSnapexOpen();
    OnSnapexStart();
	
}
/***********************************************************/
void CMainFrame::OnUpdateStartsnap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( !m_bStart);
	pCmdUI->SetCheck(m_bStart);
}
/***********************************************************/
void CMainFrame::OnStopsnap() 
{
	// TODO: Add your command handler code here
	OnSnapexStop();
	OnSnapexClose();

    CView *pView		= GetActiveView();		//��ȡ��ǰVIEW��ͼ
	CDC *pDC			= pView->GetDC();		//�õ�VIEW��DC
	RECT rect;       
	GetClientRect(&rect);
    pDC->FillSolidRect(&rect, RGB(255, 255, 255));
	pView->ReleaseDC(pDC);
}

/***********************************************************/
void CMainFrame::OnUpdateStopsnap(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( m_bStart);
}
/***********************************************************/
/******************��ͼƬ��С��640*512������ʾ***********************/
void CMainFrame::Resize()
 {
	CvSize cvSize;
	cvSize.width = Width;
 	cvSize.height = Height;
 	
	//����֧��OPENCV��IPLIMAGE���ݽṹ����ʹ������ɼ���ͼ�����ݳ�ʼ��
	IplImage *iplImage = cvCreateImageHeader(cvSize,IPL_DEPTH_8U,3);
 	cvSetData(iplImage,m_pImageBuffer,Width*3);  
	
    CvSize my_cvSize;           //��ͼƬ�ߴ�
	my_cvSize.width = Width/2;
 	my_cvSize.height = Height/2;

	IplImage *iplgraytemp = cvCreateImage(my_cvSize,IPL_DEPTH_8U,3); //��ͼƬ����

	cvResize(iplImage,iplgraytemp,CV_INTER_NN);
    	
	//��LPLIMAGE���ݽṹ����ȡͼ������
    memcpy(DisplayBuffer,(BYTE*)iplgraytemp->imageData,Height/2*Width/2*3);

 	//�ͷ������ͼ��ռ�
	cvReleaseImage(&iplgraytemp);
	

 }

	/*void InitConsole()  
{  
    int nRet= 0;  
    FILE* fp;  
    AllocConsole();  
    nRet= _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);  
    fp = _fdopen(nRet, "w");  
    *stdout = *fp;  
    setvbuf(stdout, NULL, _IONBF, 0); 
}  */


/**************�����жϻص�����***************/

 VOID CALLBACK CntIrq(int port) 
{
	if(::AfxGetMainWnd()) 
	{
		if(::AfxGetMainWnd()->m_hWnd)
		{
			::PostMessage(::AfxGetMainWnd()->m_hWnd,WM_READY,0,0); 
		}
	}
}



 /******************* Start Scan ***************/
 void CMainFrame:: OnStartscan()
 {
	 if(SIO_OK!=sio_open(Port))
		{
			MessageBox("���ڴ򿪴���");
			return;
		}
   else
		{
			sio_ioctl(Port,BaudRate,DataBits | StopBits | Parity);
			sio_cnt_irq(Port,CntIrq,1);
		}

   Scan_num=0;
   OnBtnSend(num);
   //OnBtnSend('1');
 }

/* void CMainFrame:: OnUpdateStartscan(CCmdUI* pCmdUI)
 {
    pCmdUI->Enable( !m_bOpen);		
	pCmdUI->SetCheck(m_bOpen);
 }
*/

/****************************************************/
 /**********ɨ�躯��**********************/

void CMainFrame::Scan()
{    
	 CView *pView		= GetActiveView();		//��ȡ��ǰVIEW��ͼ
     CDC *pDC			= pView->GetDC();		//�õ�VIEW��DC
    
	 CString cs;
     cs.Format("%d",Scan_num+1);
     pDC->TextOut(0,Height/2,cs,cs.GetLength());
     pView->ReleaseDC(pDC);
     


     int i,k;
	 unsigned char MAX=0;
	 int MAX_i=0;
	 unsigned char seq[3];
	 float  peak;
	 float  distance;
	 float  Phi;
	 //float  Theta;
	 float Z;
	 float L1,L2,L3,L4;
	 float r;
	 uchar* ptr;

	 HVSTATUS status =STATUS_OK;
	/********�ɼ���֡ͼ��**********/
    unsigned char *ppbuffer[1];
	ppbuffer[0]=m_pRawBuffer;
   	

	status =HVSnapShot(m_hhv,ppbuffer,1);

    HV_VERIFY(status);

	/********��ʾ��ǰ֡************/
	OnSnapChange(NULL,NULL);

    
	//����֧��OPENCV��IPLIMAGE���ݽṹ����ʹ������ɼ���ͼ�����ݳ�ʼ��//
    CvSize cvSize;
	cvSize.width = Width;
 	cvSize.height = Height;
 	
	
	IplImage *iplImage = cvCreateImageHeader(cvSize,IPL_DEPTH_8U,3);
 	cvSetData(iplImage,m_pImageBuffer,Width*3);
 	
	//����Ҷȿռ䣬��BGR��ʽת��Ϊ�Ҷ�����
 	IplImage *iplgray = cvCreateImage(cvGetSize(iplImage),IPL_DEPTH_8U,1);

 	cvCvtColor(iplImage,iplgray,CV_BGR2GRAY);
    
	for(k=0;k<Height;k++)
	{
		ptr=(uchar*)(iplgray->imageData+k*iplgray->widthStep);

        for(i=0;i<Width;i++)


     	for(i=0;i<Width;i++)
		{
	    	if( ptr[i]>=MAX)
			{
			  MAX=ptr[i];
			  MAX_i=i;
			}
		}
        
		if(MAX<=50)
		{
           Scan_result[Scan_num][k].X=0;
           Scan_result[Scan_num][k].Y=0;
		   Scan_result[Scan_num][k].Z=0;
		   continue;
		}

		if(MAX_i<=1)
		   seq[2]=0;
		else 
		  seq[2]=ptr[MAX_i-1];

		seq[1]=ptr[MAX_i];
		
		if(MAX_i>=Width)
		  seq[0]=0;
		else
		  seq[0]=ptr[MAX_i+1];


		//peak=1/2*(log(seq[0])-log(seq[2]))/(log(seq[0])-2*log(seq[1])+log(seq[2]))+Width-MAX_i;
              peak=(seq[2]-seq[0])/(seq[0]+seq[1]+seq[2])+Width-MAX_i;
           //peak=(2*ptr[MAX_i-2]+ptr[MAX_i-1]-ptr[MAX_i+1]-2*ptr[MAX_i+2])/(ptr[MAX_i+2]+ptr[MAX_i+1]+ptr[MAX_i]+ptr[MAX_i-1]+ptr[MAX_i-2])+Width-MAX_i;
 

        
        /************************����ĳ��ɨ����*****************************************/

        distance=FS/(PIX_SIZE*peak+ PIX_OFF)+DIST_OFF; 
		if(distance>1800)
		{
           Scan_result[Scan_num][k].X=0;
           Scan_result[Scan_num][k].Y=0;
		   Scan_result[Scan_num][k].Z=0;
		   continue;
		}
		//Theta=atan((k-Height/2)*PIX_SIZE/f);
        Z=(distance-DIST_OFF)*(Height/2-k)*PIX_SIZE/f;
        L1=sqrt(distance*distance+Z*Z);
        

		L3=  distance*tan(PI/2- LaserAngle);
		L2=  sqrt(L1*L1+L3*L3);
		L4=  sqrt(L3*L3+distance*distance);

		r=   sqrt( (L3-rotation_r)*(L3-rotation_r)+distance*distance);

        Phi=Scan_num*Scan_step+PI/2-acos( (rotation_r*rotation_r+r*r-L4*L4)/2.0f/rotation_r/r);


		Scan_result[Scan_num][k].X=r*cos(Phi);
        Scan_result[Scan_num][k].Y=r*sin(Phi);
		Scan_result[Scan_num][k].Z=Z;
		/****************************************************************************/
	
	}
 


	cvReleaseImage(&iplgray);
  /************���ɨ����ɣ��رմ��ڣ����������********/
	if(Scan_num==Scan_total-1)
	{  
	   MessageBox("ɨ�����");
	   Scan_num=0;
	   OnBtnSend('2');
       //OnBtnSend('2');
	   sio_close(Port); //�رմ���
	   /*************�������**************/
	   char* fileName=dataout;
	   CString strtemp;

       CFile file;
	   CFileException fileException;
	   if(!file.Open(fileName,CFile::modeCreate | CFile::modeWrite, &fileException))
	   {
		   CString errorInfo;
		   errorInfo.Format("���ܴ��ļ�%s,����:%u\n",fileName,fileException.m_cause);
		   MessageBox(errorInfo,"����",MB_OK|MB_ICONERROR);
		   return;
	   }

	   for(i=0;i<Scan_total;i++)
		   for(k=0;k<Height;k++)
		   {
			   if(Scan_result[i][k].X==0 & Scan_result[i][k].Y==0 & Scan_result[i][k].Z==0)
                  continue;
			   else
			   { strtemp.Format("%.3f, %.3f, %.3f\r\n\r\n",Scan_result[i][k].X,Scan_result[i][k].Y,Scan_result[i][k].Z);
				   file.Write(strtemp,strtemp.GetLength());
			   }
				  
		   }
	    file.Close();
		MessageBox("�ѳɹ��������","��Ϣ",MB_OK|MB_ICONINFORMATION);  

	   return;
	}
  /**********ɨ��δ��ɣ����ʹ���ָ��***********/
    else
    { 
		OnBtnSend('1');
		//OnBtnSend('1');
		Scan_num=Scan_num+1;
	
    }  
 
}

/***********************���ڷ��ͺ���*****************************/
void CMainFrame::OnBtnSend(unsigned char message) 
{
 	// TODO: Add your control notification handler code here
    sio_putch(Port,message); //����
}

void CMainFrame::ScanSet()
{
        CView *pView		= GetActiveView();		//��ȡ��ǰVIEW��ͼ
    	CDC *pDC			= pView->GetDC();		//�õ�VIEW��DC
    
	    CString cs;
        cs.Format("%d",100);


	    pDC->TextOut(0,Height/2,cs,cs.GetLength());
        pView->ReleaseDC(pDC);

/*//MessageBox("�ѳɹ��������","��Ϣ",MB_OK|MB_ICONINFORMATION);
	char* fileName="E:\\����\\3D_SCAN\\Data_Out\\data.asc";
	   CString strtemp;

       CFile file;
	   CFileException fileException;
	   if(!file.Open(fileName,CFile::modeCreate | CFile::modeWrite, &fileException))
	   {
		   CString errorInfo;
		   errorInfo.Format("���ܴ��ļ�%s,����:%u\n",fileName,fileException.m_cause);
		   MessageBox(errorInfo,"����",MB_OK|MB_ICONERROR);
		   return;
	   }


     
     

     int i, j,k;
	 for(k=0;k<100;k++)
	 for(i=0;i<100;i++)
		 for(j=0;j<100;j++)
		 {
			 strtemp.Format("%d,%d,%d\r\n\r\n",i,j,k);
			 file.Write(strtemp,strtemp.GetLength());
		 }

     file.Close();
*/
}
