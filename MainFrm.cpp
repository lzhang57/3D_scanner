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

#define WM_READY WM_USER+500 //自定义消息

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

#define BaudRate B9600//波特率
#define DataBits BIT_8 //数据位
#define Parity P_NONE //效验位
#define StopBits STOP_1 //停止位

/*******三角测距参数*******/
#define FS             1585  
#define PIX_SIZE       0.005396
#define PIX_OFF        0.7968
#define DIST_OFF       22.65
#define LaserAngle     1.1345
#define f              8
#define rotation_r     100
#define PI             3.1415927

/******************************/

int    Scan_num;   //扫描序列号
#define Num 2
#define num  '2'
#define  Scan_step     0.1*PI/180  //扫描步进角度
#define    Scan_total  250//500/Num         //扫描总次数

#define   dataout    "C:\\Users\\ZLK\\Desktop\\output\\data.asc"
/*****测量结果结构体数组*****/
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
	 *	初始化所有成员变量，同时打开数字摄像机
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

	//	打开数字摄像机 1
	status = BeginHVDevice(1, &m_hhv);
	//	检验函数执行状态，如果失败，则返回错误状态消息框
	HV_VERIFY(status);
	
}

CMainFrame::~CMainFrame()
{
	HVSTATUS status = STATUS_OK;
	
	//	关闭数字摄像机，释放数字摄像机内部资源
	status = EndHVDevice(m_hhv);
	HV_VERIFY(status);

	//	回收图像缓冲区
	delete []m_pRawBuffer;
	delete []m_pImageBuffer;
    delete []DisplayBuffer;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
		/*
		*	初始化数字摄像机硬件状态，用户也可以在其他位置初始化数字摄像机，
		*	但应保证数字摄像机已经打开，建议用户在应用程序初始化时，
		*	同时初始化数字摄像机硬件。
	*/
	
	//	设置数字摄像机分辨率
	HVSetResolution(m_hhv, Resolution);		
	
	//	采集模式，包括 CONTINUATION(连续)、TRIGGER(外触发)
	HVSetSnapMode(m_hhv, SnapMode);
	
	//  设置各个分量的增益
	for (int i = 0; i < 4; i++){
		HVAGCControl(m_hhv, RED_CHANNEL + i, Gain);
	}

	//	设置曝光时间
	SetExposureTime(Width,ExposureTint_Upper,ExposureTint_Lower);	

	//  设置ADC的级别
	HVADCControl(m_hhv, ADC_BITS, ADCLevel);
	
	/*
	*	视频输出窗口，即视频输出范围，输出窗口取值范围必须在输入窗口范围以内，
	*  视频窗口左上角X坐标和窗口宽度应为4的倍数，左上角Y坐标和窗口高度应为2的倍数
	*	输出窗口的起始位置一般设置为(0, 0)即可。
	*/
	HVSetOutputWindow(m_hhv, XStart, YStart, Width, Height);
	
	//	m_pBmpInfo即指向m_chBmpBuf缓冲区，用户可以自己分配BTIMAPINFO缓冲区	
	m_pBmpInfo								= (BITMAPINFO *)m_chBmpBuf;
	//	初始化BITMAPINFO 结构，此结构在保存bmp文件、显示采集图像时使用
	m_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	//	图像宽度，一般为输出窗口宽度
	m_pBmpInfo->bmiHeader.biWidth			= Width/2;
	//	图像宽度，一般为输出窗口高度
	m_pBmpInfo->bmiHeader.biHeight			= Height/2;
	
	/*
	*	以下设置一般相同，
	*	对于低于8位的位图，还应设置相应的位图调色板
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
	*	分配原始图像缓冲区，一般用来存储采集图像原始数据
	*  一般图像缓冲区大小由输出窗口大小和视频格式确定。
	*/
	m_pRawBuffer = new BYTE[Width * Height];
	ASSERT(m_pRawBuffer);
	
	/*
	分配Bayer转换后图像数据缓冲
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
	 *	初始化数字摄像机采集图像到内存的控制，
	 *	指定回调函数SnapThreadCallback和用户参数m_hWnd
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
	 *	启动数字摄像机采集图像到内存
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
	
	//	停止采集图像到内存，可以再次调用HVStartSnapEx启动数字摄像机采集
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
	 *	终止数字摄像机采集图像到内存，同时释放所有采集环境，
	 *	再次启动数字摄像机采集，必须重新初始化	
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
	 *	发送自定义消息WM_SNAP_EX_CHANGE到主窗口，	
	 *	同时传入当前可以处理的图像序号
	 *	注意：用SendMessage发送消息，必须等待消息处理完毕后，才能退出整个SendMessage函数
	 */
	::SendMessage(hwnd, WM_SNAP_CHANGE, 0, 0);

	return 1;
}


/***********************************************************/
LRESULT CMainFrame::OnSnapChange(WPARAM wParam, LPARAM lParam)
{
	HVSTATUS status = STATUS_OK;
	
	CView *pView		= GetActiveView();		//获取当前VIEW视图
	CDC *pDC			= pView->GetDC();		//得到VIEW的DC
	
	//	将原始图像数据进行Bayer转换，转换后为24位。
	ConvertBayer2Rgb(m_pImageBuffer,m_pRawBuffer,Width,Height,ConvertType,m_pLutR,m_pLutG,m_pLutB,true,Layout);
    //同时将原始数据进行上下翻转	
  	Resize();
   
    //在视图客户区显示图像
	StretchDIBits(pDC->GetSafeHdc(),
					0,						
					0,
					Width/2,					//显窗口宽度
					Height/2,					//显示窗口高度
					0,
					0,
					Width/2,					//图像宽度
					Height/2,					//图像高度
					DisplayBuffer,			//图像缓冲区
					m_pBmpInfo,				//BMP图像描述信息
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
	 *	用户在没有通过菜单项正常关闭数字摄像机采集，	
	 *	而直接关闭应用程序时，应保证数字摄像机采集被关闭
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
	//请参考曝光系数转换公式
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

    CView *pView		= GetActiveView();		//获取当前VIEW视图
	CDC *pDC			= pView->GetDC();		//得到VIEW的DC
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
/******************将图片缩小至640*512方便显示***********************/
void CMainFrame::Resize()
 {
	CvSize cvSize;
	cvSize.width = Width;
 	cvSize.height = Height;
 	
	//生成支持OPENCV的IPLIMAGE数据结构，并使用相机采集的图像数据初始化
	IplImage *iplImage = cvCreateImageHeader(cvSize,IPL_DEPTH_8U,3);
 	cvSetData(iplImage,m_pImageBuffer,Width*3);  
	
    CvSize my_cvSize;           //新图片尺寸
	my_cvSize.width = Width/2;
 	my_cvSize.height = Height/2;

	IplImage *iplgraytemp = cvCreateImage(my_cvSize,IPL_DEPTH_8U,3); //新图片矩阵

	cvResize(iplImage,iplgraytemp,CV_INTER_NN);
    	
	//从LPLIMAGE数据结构中提取图像数据
    memcpy(DisplayBuffer,(BYTE*)iplgraytemp->imageData,Height/2*Width/2*3);

 	//释放申请的图象空间
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


/**************串口中断回调函数***************/

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
			MessageBox("串口打开错误");
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
 /**********扫描函数**********************/

void CMainFrame::Scan()
{    
	 CView *pView		= GetActiveView();		//获取当前VIEW视图
     CDC *pDC			= pView->GetDC();		//得到VIEW的DC
    
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
	/********采集单帧图像**********/
    unsigned char *ppbuffer[1];
	ppbuffer[0]=m_pRawBuffer;
   	

	status =HVSnapShot(m_hhv,ppbuffer,1);

    HV_VERIFY(status);

	/********显示当前帧************/
	OnSnapChange(NULL,NULL);

    
	//生成支持OPENCV的IPLIMAGE数据结构，并使用相机采集的图像数据初始化//
    CvSize cvSize;
	cvSize.width = Width;
 	cvSize.height = Height;
 	
	
	IplImage *iplImage = cvCreateImageHeader(cvSize,IPL_DEPTH_8U,3);
 	cvSetData(iplImage,m_pImageBuffer,Width*3);
 	
	//申请灰度空间，将BGR格式转化为灰度数据
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
 

        
        /************************计算某行扫描结果*****************************************/

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
  /************如果扫描完成，关闭串口，并输出数据********/
	if(Scan_num==Scan_total-1)
	{  
	   MessageBox("扫描完成");
	   Scan_num=0;
	   OnBtnSend('2');
       //OnBtnSend('2');
	   sio_close(Port); //关闭串口
	   /*************数据输出**************/
	   char* fileName=dataout;
	   CString strtemp;

       CFile file;
	   CFileException fileException;
	   if(!file.Open(fileName,CFile::modeCreate | CFile::modeWrite, &fileException))
	   {
		   CString errorInfo;
		   errorInfo.Format("不能打开文件%s,错误:%u\n",fileName,fileException.m_cause);
		   MessageBox(errorInfo,"错误",MB_OK|MB_ICONERROR);
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
		MessageBox("已成功输出数据","信息",MB_OK|MB_ICONINFORMATION);  

	   return;
	}
  /**********扫描未完成，发送串口指令***********/
    else
    { 
		OnBtnSend('1');
		//OnBtnSend('1');
		Scan_num=Scan_num+1;
	
    }  
 
}

/***********************串口发送函数*****************************/
void CMainFrame::OnBtnSend(unsigned char message) 
{
 	// TODO: Add your control notification handler code here
    sio_putch(Port,message); //发送
}

void CMainFrame::ScanSet()
{
        CView *pView		= GetActiveView();		//获取当前VIEW视图
    	CDC *pDC			= pView->GetDC();		//得到VIEW的DC
    
	    CString cs;
        cs.Format("%d",100);


	    pDC->TextOut(0,Height/2,cs,cs.GetLength());
        pView->ReleaseDC(pDC);

/*//MessageBox("已成功输出数据","信息",MB_OK|MB_ICONINFORMATION);
	char* fileName="E:\\程序\\3D_SCAN\\Data_Out\\data.asc";
	   CString strtemp;

       CFile file;
	   CFileException fileException;
	   if(!file.Open(fileName,CFile::modeCreate | CFile::modeWrite, &fileException))
	   {
		   CString errorInfo;
		   errorInfo.Format("不能打开文件%s,错误:%u\n",fileName,fileException.m_cause);
		   MessageBox(errorInfo,"错误",MB_OK|MB_ICONERROR);
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
