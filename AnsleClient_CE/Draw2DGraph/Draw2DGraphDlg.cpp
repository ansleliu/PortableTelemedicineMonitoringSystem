// Draw2DGraphDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resourceppc.h"
#include "Draw2DGraph.h"
#include "Draw2DGraphDlg.h"
#include "DataStructures.h"

#define BUFFER_LENGTH 32
#define MAX_SIZE  1000
#define MIN_SIZE  500
#define N 10
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////////
ECGDataNode *ECG = InitECGDataNode();
SPO2DataNode *SPO2 = InitSPO2DataNode();
BPDataNode *BP = InitBPDataNode();

ECGDataNode *Ptr_ECG = ECG;
ECGDataNode *Ptr_Next_ECG = ECG;
ECGDataNode *Ptr_Send_ECG = ECG;

SPO2DataNode *Ptr_SPO2 = SPO2;
SPO2DataNode *Ptr_Next_SPO2 = SPO2;
SPO2DataNode *Ptr_Send_SPO2 = SPO2;

BPDataNode *Ptr_BP = BP;
BPDataNode *Ptr_Next_BP = BP;
BPDataNode *Ptr_Send_BP = BP;

int ecg = 0;

SendDataNode *SendDate = InitSendDataNode();
SendDataNode *Ptr_SendDate = SendDate;
SendDataNode *Ptr_Next_Send = SendDate;
//////////////////////////////////////////////////

// CDraw2DGraphDlg 对话框
//对话框构造函数
CDraw2DGraphDlg::CDraw2DGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDraw2DGraphDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSerial1 = NULL;//初始化串口对象指针为空
	Spo2 = FALSE;
	Pulse = FALSE;
}

void CDraw2DGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LED, m_led);
	DDX_Control(pDX, IDC_LED1, m_led1);
	DDX_Control(pDX, IDC_LED2, m_led2);
	DDX_Control(pDX, IDC_LED3, m_led3);
}

BEGIN_MESSAGE_MAP(CDraw2DGraphDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_MESSAGE(WM_RECV_SERIAL_DATA,OnRecvSerialData)
	ON_MESSAGE(WM_COMM_MESSAGE,&OnCommMessage)
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_DISCONNECT,OnClientDisconnect)
	ON_BN_CLICKED(IDC_START, &CDraw2DGraphDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_REMOTEMODE, &CDraw2DGraphDlg::OnBnClickedRemotemode)
	ON_BN_CLICKED(IDC_ECG, &CDraw2DGraphDlg::OnBnClickedEcg)
	ON_BN_CLICKED(IDC_BOOLPRESSURE, &CDraw2DGraphDlg::OnBnClickedBoolpressure)
	ON_BN_CLICKED(IDC_SPO2, &CDraw2DGraphDlg::OnBnClickedSpo2)
	ON_BN_CLICKED(IDC_PULSE, &CDraw2DGraphDlg::OnBnClickedPulse)
END_MESSAGE_MAP()

// CDraw2DGraphDlg 消息处理程序
//初始化对话框函数
BOOL CDraw2DGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//初始化显示控件矩形区域大小
	CRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = 475;
	rect.bottom = 140;
	//创建曲线控件实例
	m_2DGraph.Create(_T(""),_T(""),WS_VISIBLE | WS_CHILD, rect, this,0,NULL) ; 
///////////////////////////////////////////////////////
	remotemode = new GPRSCONTDlg(); 
	ISOpenRemoteMode = FALSE;//关闭远程监控模式
	IsClicked = 0;
	m_remotehost = _T("192.168.16.1");
	m_remoteport = 5000;
///////////////////////////////////////////////////////
//	m_pointCount = 0;
//	ecgpoint = 0;
	ecgsend = 0;
	number = 1;
//////////////////////////////////////////////////////
	//设置背景色
	m_led.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////////
	//设置背景色
	m_led1.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led1.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////
	//设置背景色
	m_led2.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led2.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////
	//设置背景色
	m_led3.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led3.SetColorForeGround(RGB(0,255,255));
/////////////////////////////////////////////////////////////////
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CDraw2DGraphDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_DRAW2DGRAPH_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_DRAW2DGRAPH_DIALOG));
	}
}
#endif
///////////////////////////////////////////////////////////////////////////
//线程函数
/*DWORD CDraw2DGraphDlg::FilterThread(PVOID pArg)
{
	int ecgfilter = 0;
	while(1)
	{
		{//平滑滤波		
			int ECGtemp = 0;
			for(int i=0;i<N;i++)
			{
				ECGtemp += Ptr_ECG->ECG[(ecgfilter+i)%ECG_SIZE];
			}
			Ptr_ECG->ECG[ecgfilter] = (int)(ECGtemp/N);
		}
		Ptr_ECG->ECG[ecgfilter] = (Ptr_ECG->ECG[ecgfilter]-500)*5;
		ecgfilter++;
		if(ecgfilter == ECG_SIZE)
		{
			if(Ptr_ECG->next != NULL)
			{
				ecgfilter = 0;
				Ptr_Show_ECG = Ptr_ECG->next;
			}
		}
	Sleep(20);
	}
	return 1;
}*/
DWORD CDraw2DGraphDlg::ECGThread(PVOID pArg)
{
	
	while(1)
	{
		int nRandomY;//要显示的点的Y坐标
		if(Ptr_ECG->next == NULL)
		{
			nRandomY = rand()%(900-200+1)+200;
		}
		else
		{
			{//平滑滤波		
				int ECGtemp = 0;
				for(int i=0;i<N;i++)
				{
					ECGtemp += Ptr_ECG->ECG[(ecgpoint+i)%ECG_SIZE];
				}
				Ptr_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
			}
//			Ptr_Show_ECG->ECG[ecgpoint] = (Ptr_Show_ECG->ECG[ecgpoint]-500)*6;
//			nRandomY = ((Ptr_ECG->ECG[ecgpoint])*5/1024)*1000;
			nRandomY = (Ptr_ECG->ECG[ecgpoint]-600)*7;
			////////////////////////
			ecgpoint++;
			if(ecgpoint == ECG_SIZE)
			{
				if(Ptr_ECG->next != NULL)
				{
					ecgpoint=0;
					Ptr_ECG = Ptr_ECG->next;
				}
			}
		}	
		//如果曲线点数大于X_MAX个点，则删除第1个点。
		if (m_pointCount > X_MAX )
		{
			m_2DGraph.DeleteFirstPoint();
			m_pointCount--;
		}				
		//给曲线添加点
		m_2DGraph.AppendPoint(nRandomY);				
		m_pointCount++;					
		//////////////////////////////////
		Sleep(100);
	}
	return 1;
}

DWORD CDraw2DGraphDlg::HBPThread(PVOID pArg)
{
	//////////////////////////////////
	//设置LED显示数字
	while(1)
	{
		int led = rand()%(130-110+1)+110;
	    m_led.Display(led); 
		Sleep(500);
		int led1 = rand()%(85-70+1)+70;
		m_led1.Display(led1);
		Sleep(500);
	} 	
	return 1;
}
/*
DWORD CDraw2DGraphDlg::LBPThread(PVOID pArg)
{ 	
	//设置LED显示数字
	while(1)
	{
		int led = rand()%(85-70+1)+70;
		m_led1.Display(led);
		Sleep(800);
	}		
	return 1;
}
DWORD CDraw2DGraphDlg::SPO2Thread(PVOID pArg)
{
	//设置LED显示数字
	while(1)
	{
		int led = rand()%(98-93+1)+93;
		m_led2.Display(led);
		Sleep(800);
	}
	return 1;
}
DWORD CDraw2DGraphDlg::PulseThread(PVOID pArg)
{
	//设置LED显示数字
	while(1)
	{
		int led = rand()%(78-70+1)+70;
		m_led3.Display(led);
		Sleep(800);
	} 
	return 1;
}
*/
DWORD CDraw2DGraphDlg::SendThread(PVOID pArg)
{
	while(1)
	{
		/////////////////////////////////////////////////
		//通过TCP发送数据
		CString strSendData = L"";
		CString ECGSend = L"ECG$#";
		CString SPO2Send = L"SPO2$#";
		CString BPSend = L"BP$#";
		CString PulseSend = L"Pulse$#";
		//////////////////////////////////////////
		//ECG
		{
			for(int i=0;i<ECG_SIZE;i++)
			{
				int pos = 0;
				pos = ECGSend.Find(L"#",0);
				CString temp = L"";
				temp.Format(L"%d;",Ptr_Send_ECG->ECG[i]);
				ECGSend.Insert(pos,temp);
			}
			if(Ptr_Send_ECG->next != NULL)
			{
				Ptr_Send_ECG = Ptr_Send_ECG->next;
			}
			else
			{
				int pos = 0;
				pos = ECGSend.Find(L"#");
				CString temp = L"";
				temp.Format(L"%s;",L"0");
				ECGSend.Insert(pos,temp);
			}
		}
		//////////////////////////////////////////
		//SPO2
		{
			int pos = 0;
			pos = SPO2Send.Find(L"#",0);
			CString temp = L"";
			temp.Format(L"%s;",L"0");
			SPO2Send.Insert(pos,temp);
		}
		//////////////////////////////////////////
		//BP
		{
			int pos = 0;
			pos = BPSend.Find(L"#",0);
			CString temp = L"";
			temp.Format(L"%s;",L"0");
			BPSend.Insert(pos,temp);
			int pos1 = 0;
			pos1 = BPSend.Find(L"#",0);
			CString temp1 = L"";
			temp1.Format(L"%s;",L"0");
			BPSend.Insert(pos,temp);
		}
		//////////////////////////////////////////
		//Pulse
		{
			int pos = 0;
			pos = PulseSend.Find(L"#",0);
			CString temp = L"";
			temp.Format(L"%s;",L"0		");
			PulseSend.Insert(pos,temp);
		}
		///////////////////////////////////////////////////
		//组成发送数据
		strSendData = ECGSend+SPO2Send+BPSend+PulseSend;
		char * sendBuf = NULL;
		int sendLen=0;
		//设置发送缓冲区
		sendLen = strSendData.GetLength()*2 + 2;
		sendBuf = new char[sendLen];
		ZeroMemory(sendBuf,sendLen);
		wcstombs(sendBuf,strSendData,sendLen);
		sendLen = strlen(sendBuf)+1;
//		Ptr_Next_Send->SendDate = sendBuf;
//		Ptr_Next_Send->next = InitSendDataNode();
//		Ptr_Next_Send = Ptr_Next_Send->next;
		///////////////////////////////////////////////////////////
//		int sendLen=0;
//		sendLen = strlen(Ptr_SendDate->SendDate)+1;
		//发送数据
		if (!m_tcpClient.SendData(sendBuf,sendLen))
		{
			AfxMessageBox(_T("发送失败"));
						
		}
//		int led = rand()%(78-70+1)+70;
//		m_led3.Display(led);
		Sleep(600);
	}
	return 1;
}
///////////////////////////////////////////////////////////////////////////
//定时器消息处理函数
void CDraw2DGraphDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
//		switch(nIDEvent)
//		{
/*		case 1:
			{
				int nRandomY;//要显示的点的Y坐标

				if(Ptr_Show_ECG->next == NULL)
				{
					nRandomY = rand()%(900-200+1)+200;
				}
				else
				{
					//坐标的赋值和滤波函数
					{//滤波
						int ECGtemp = 0;
						for(int i=0;i<N;i++)
						{
							ECGtemp += Ptr_Show_ECG->ECG[(ecgpoint+i)%ECG_SIZE];
						}
						Ptr_Show_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
					}
					nRandomY =(int)((Ptr_Show_ECG->ECG[ecgpoint]*5)/1024);
					nRandomY = Ptr_Show_ECG->ECG[ecgpoint];
					////////////////////////
					ecgpoint++;
					if(ecgpoint == ECG_SIZE)
					{
						if(Ptr_Show_ECG->next != NULL)
						{
							ecgpoint=0;
							Ptr_Show_ECG = Ptr_Show_ECG->next;
						}
					}
				}	
				//如果曲线点数大于X_MAX个点，则删除第1个点。
				if (m_pointCount > X_MAX )
				{
					m_2DGraph.DeleteFirstPoint();
					m_pointCount--;
				}				
				//给曲线添加点
				m_2DGraph.AppendPoint(nRandomY);				
				TRACE(L" y = %d \n",nRandomY);
				m_pointCount++;					
				//////////////////////////////////
				CDialog::OnTimer(nIDEvent);
				break;
			}*/
//		case 2:
			{
				//设置LED显示数字
				int led = rand()%(130-110+1)+110;
				m_led.Display(led);  
				//设置LED显示数字
				int led1 = rand()%(85-70+1)+70;
				m_led1.Display(led1);  
				if(Spo2 == TRUE)
				{
					int led2 = rand()%(98-93+1)+93;
					m_led2.Display(led2);
				}
				if(Pulse == TRUE)
				{
					int led3 = rand()%(78-70+1)+70;
					m_led3.Display(led3);
				}
				CDialog::OnTimer(nIDEvent);
//				break;
			}
/*		case 3:
			{
				//设置LED显示数字
				int led1 = rand()%(85-70+1)+70;
				m_led1.Display(led1);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}*/
/*		case 4:
			{
				//设置LED显示数字
				int led2 = rand()%(98-93+1)+93;
				m_led2.Display(led2);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 5:
			{
				//设置LED显示数字
				int led3 = rand()%(78-70+1)+70;
				m_led3.Display(led3);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}*/
/*		case 6:
			{
					/////////////////////////////////////////////////
					//通过TCP发送数据
					CString strSendData = L"";
					CString ECGSend = L"ECG$#";
					CString SPO2Send = L"SPO2$#";
					CString BPSend = L"BP$#";
					CString PulseSend = L"Pulse$#";
					//////////////////////////////////////////
					//ECG
					{
						for(int i=0;i<ECG_SIZE;i++)
						{
							int pos = 0;
							pos = ECGSend.Find(L"#",0);
							CString temp = L"";
							temp.Format(L"%d;",Ptr_Send_ECG->ECG[i]);
							ECGSend.Insert(pos,temp);
						}
						if(Ptr_Send_ECG->next != NULL)
						{
							Ptr_Send_ECG = Ptr_Send_ECG->next;
						}
						else
						{
							int pos = 0;
							pos = ECGSend.Find(L"#");
							CString temp = L"";
							temp.Format(L"%s;",L"0");
							ECGSend.Insert(pos,temp);
						}
					}
					//////////////////////////////////////////
					//SPO2
					{
						int pos = 0;
						pos = SPO2Send.Find(L"#",0);
						CString temp = L"";
						temp.Format(L"%s;",L"0");
						SPO2Send.Insert(pos,temp);
					}
					//////////////////////////////////////////
					//BP
					{
							int pos = 0;
							pos = BPSend.Find(L"#",0);
							CString temp = L"";
							temp.Format(L"%s;",L"0");
							BPSend.Insert(pos,temp);
							int pos1 = 0;
							pos1 = BPSend.Find(L"#",0);
							CString temp1 = L"";
							temp1.Format(L"%s;",L"0");
							BPSend.Insert(pos,temp);
					}
					//////////////////////////////////////////
					//Pulse
					{
						int pos = 0;
						pos = PulseSend.Find(L"#",0);
						CString temp = L"";
						temp.Format(L"%s;",L"0		");
						PulseSend.Insert(pos,temp);
					}
					///////////////////////////////////////////////////
					//组成发送数据
					strSendData = ECGSend+SPO2Send+BPSend+PulseSend;
					///////////////////////////////////////////////////
					/////////////////////////////////////////////////
					//通过TCP发送数据
					CString strSendData = L"";
					strSendData = Ptr_SendDate->SendDate;
					char * sendBuf = NULL;
					int sendLen=0;
					//设置发送缓冲区
					sendLen = strSendData.GetLength()*2 + 2;
					sendBuf = new char[sendLen];
					ZeroMemory(sendBuf,sendLen);
					wcstombs(sendBuf,strSendData,sendLen);
					sendLen = strlen(sendBuf)+1;
					int sendLen=0;
					sendLen = strlen(Ptr_SendDate->SendDate)+1;
					//发送数据
					if (!m_tcpClient.SendData(Ptr_SendDate->SendDate,sendLen))
					{
						KillTimer(6);
						AfxMessageBox(_T("发送失败"));
						
					}
					else
					{
						if(Ptr_SendDate->next == NULL)
						{
							KillTimer(6);
						}
						Ptr_SendDate = Ptr_SendDate->next;
					}
					//释放内存
					CDialog::OnTimer(nIDEvent);
				break;
			}*/
//		default:
//			break;
//	}	
}
///////////////////////////////////////////////////////////
//串口处理代码段
///////////////////////////////////////////////////////////
//定义串口接收数据函数类型
void CALLBACK CDraw2DGraphDlg::OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	BYTE *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CDraw2DGraphDlg* pThis = (CDraw2DGraphDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new BYTE[bufLen];
	CopyMemory(pRecvBuf,buf,bufLen);

	//发送异步消息，表示收到串口数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_SERIAL_DATA,WPARAM(pRecvBuf),bufLen);

}

// 串口接收数据处理函数
LONG CDraw2DGraphDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
{
	CString strRecv = L"";
	//串口接收到的BUF
	CHAR *pBuf = (CHAR*)wParam;
	//串口接收到的BUF长度
	DWORD dwBufLen = lParam;
    strRecv = CString(pBuf);
	//////////////////////////////////////
    //处理接收到的数据
	CString num = L"";
	int pos = 0;
	while (!strRecv.IsEmpty())
	{
		int arg = 0;
		pos = strRecv.Find(_T(","),0);
		if(pos == 0)
		{//","
			strRecv.Delete(0,pos+1);
			continue;
		}
		if(pos<0)
		{//只剩一个数
			num = strRecv;
			strRecv.Empty();
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-1];
			ecg++;
			if(ecg ==ECG_SIZE)
			{
				ecg = 0;
				Ptr_Next_ECG->next = InitECGDataNode();
				Ptr_Next_ECG = Ptr_Next_ECG->next;
			}
		}
		else
		{
			num = strRecv.Left(pos);
			int len = num.GetLength();
			strRecv.Delete(0,pos+1);
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-1];
			ecg++;
			if(ecg ==ECG_SIZE)
			{
				ecg = 0;
				Ptr_Next_ECG->next = InitECGDataNode();
				Ptr_Next_ECG = Ptr_Next_ECG->next;
			}
		}
	}
	//////////////////////////////////////
	//释放内存
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}
void CDraw2DGraphDlg::OnSeriesRead(CWnd* pWnd,BYTE* buf,int bufLen)
{
	CDraw2DGraphDlg *pDlg = (CDraw2DGraphDlg *)pWnd;
	::PostMessage(pWnd->m_hWnd,WM_COMM_MESSAGE,(WPARAM)buf,(LPARAM)bufLen);
}

LRESULT CDraw2DGraphDlg::OnCommMessage(WPARAM pbuf,LPARAM lbufLen)
{
	CString strRecv = L"";
	//串口接收到的BUF
	CHAR *pBuf = (CHAR*)pbuf;
	//串口接收到的BUF长度
	DWORD dwBufLen = lbufLen;
    strRecv = CString(pBuf);
	//////////////////////////////////////
    //处理接收到的数据
	CString num = L"";
	int pos = 0;
	while (!strRecv.IsEmpty())
	{
		int arg = 0;
		pos = strRecv.Find(_T(","),0);
		if(pos == 0)
		{//","
			strRecv.Delete(0,pos+1);
			continue;
		}
		if(pos<0)
		{//只剩一个数
			num = strRecv;
			strRecv.Empty();
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-10];
			ecg++;
			if(ecg == ECG_SIZE)
			{
				ecg = 0;
				Ptr_Next_ECG->next = InitECGDataNode();
				Ptr_Next_ECG = Ptr_Next_ECG->next;
			}
		}
		else
		{
			num = strRecv.Left(pos);
			int len = num.GetLength();
			strRecv.Delete(0,pos+1);
			Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
			if(Ptr_Next_ECG->ECG[ecg]<MIN_SIZE||Ptr_Next_ECG->ECG[ecg]>MAX_SIZE)
				Ptr_Next_ECG->ECG[ecg] = Ptr_Next_ECG->ECG[ecg-1];
			ecg++;
			if(ecg == ECG_SIZE)
			{
				ecg = 0;
				Ptr_Next_ECG->next = InitECGDataNode();
				Ptr_Next_ECG = Ptr_Next_ECG->next;
			}
		}
	}
	//////////////////////////////////////
	//释放内存
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}
//数据接收事件
void CALLBACK CDraw2DGraphDlg::OnRead(CWnd* pWnd,const char * buf,int len )
{

	CDraw2DGraphDlg * pDlg = (CDraw2DGraphDlg*)pWnd;
	TCHAR *wbuf = NULL;
	wbuf = new TCHAR[len];
	if (wbuf == NULL)
	{
		return;
	}
	CUtility::CharpToUnsignedShort(buf,(unsigned short*)wbuf);	
	CString strRecv;
	strRecv.Format(_T("%s"),wbuf);	
	delete [] wbuf;
}
//////////////////////////////////////////////////////////////////////
//TCP处理代码段
//////////////////////////////////////////////////////////////////////
//连接断开事件
void CALLBACK CDraw2DGraphDlg::OnDisConnect(void* pOwner)
{
	//得到父对象指针
	CDraw2DGraphDlg* pThis = (CDraw2DGraphDlg*)pOwner;
	//发送消息表示客户端连接断开
	pThis->PostMessage(WM_TCP_CLIENT_DISCONNECT,0,0);
}
//数据接收事件
void CALLBACK CDraw2DGraphDlg::OnTcpRead(void* pOwner,const char * buf,DWORD dwBufLen )
{
	BYTE *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CDraw2DGraphDlg* pThis = (CDraw2DGraphDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new BYTE[dwBufLen];
	CopyMemory(pRecvBuf,buf,dwBufLen);
	//发送异步消息，表示收到TCP数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pRecvBuf),dwBufLen);
}

//Socket错误事件
void CALLBACK CDraw2DGraphDlg::OnTcpError(void* pOwner,int nErrorCode)
{
	TRACE(L"客户端socket发生错误");
}

//TCP接收数据处理函数
LONG CDraw2DGraphDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	CString strOldRecv = L"";
	CString strRecv = L"";
	//接收到的BUF
	CHAR *pBuf = (CHAR*)wParam;
	//接收到的BUF长度
	DWORD dwBufLen = lParam;
	strRecv = CString(pBuf);
    ///////////////////////////////////////////////
	//TCP数据接收处理

	///////////////////////////////////////////////
	//释放内存
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}

//客户端连接断开消息函数
LONG CDraw2DGraphDlg::OnClientDisconnect(WPARAM wParam,LPARAM lParam)
{
	AfxMessageBox(_T("连接断开"));
	return 0;
}
/////////////////////////////////////////////////////////////
//处理按钮事件
/////////////////////////////////////////////////////////////
//开始监控
void CDraw2DGraphDlg::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	///////////////////////////////////////////////////////////
	//打开COM3和COM4分别用来接收心电数据和血压-脉搏血氧数据
	//判断串口是否已经打开
	int flag = 0;
	if (m_pSerial1 != NULL)
	{
		m_pSerial1->ClosePort();

		delete m_pSerial1;
		m_pSerial1 = NULL;
	}
	//新建串口通讯对象
	m_pSerial1 = new CCESerials();
	m_pSerial1-> m_OnSerialsRead = OnSerialRead; //

	//打开串口
	if (m_pSerial1->OpenPort(this,3,9600,NOPARITY,8,0))
	{
		flag++;
//		AfxMessageBox(L"COM3打开成功");
	}
	else
	{
		m_pSerial1->ClosePort();
		AfxMessageBox(L"COM3打开失败");
		flag++;
	}
	///////////////////////////////////////////////////////////////
	//COM4
/*	if (m_pSerial2 != NULL)
	{
		m_pSerial2->ClosePort();

//		delete m_pSerial1;
//		m_pSerial1 = NULL;
	}
	m_pSerial2 = new CCESeries();
	m_pSerial2->m_OnSeriesRead = OnSeriesRead;
	if (m_pSerial2->OpenPort(this,4,9600))
	{
		flag++;
//		AfxMessageBox(_T("COM4打开成功"));
		
	}
	else  // 打开串口成功
	{
		m_pSerial2->ClosePort();
		AfxMessageBox(_T("COM4打开失败"));
		flag++;
		
	}*/
//	Sleep(50);
	///////////////////////////////////////////////////////////
	//创建线程，用于滤波
/*	if(flag == 2)
	{
		DWORD FilterThreadID;
		HANDLE filterhandle;
		///////////////////////////////////////////////////////////
		//创建线程，用于发送数据的处理
//		DWORD SendThreadID;
//		HANDLE Sendhandle;
		filterhandle = CreateThread(NULL,0,FilterThread,NULL,0,&FilterThreadID);
//		Sleep(5);		
//		Sendhandle = CreateThread(NULL,0,SendThread,NULL,0,&SendThreadID);
//		Sleep(5);
//		if(!Sendhandle)
//		{
//			AfxMessageBox(_T("线程创建失败"));
//		}
		if(!filterhandle)
		{
			AfxMessageBox(_T("滤波器线程创建失败"));
		}
	}*/
}
//远程模式
void CDraw2DGraphDlg::OnBnClickedRemotemode()
{
	// TODO: 在此添加控件通知处理程序代码
	///////////////////////////////////////////////////////////////
	//获得服务器IP地址和端口号
	if (remotemode->DoModal() == IDOK)
	{
		m_remotehost = remotemode-> m_remotehost;
		m_remotehost = _T("192.168.16.1");
		remotemode->m_GPRS.GPRS_ClosePort();
	}
	Sleep(50);
	///////////////////////////////////////////////////////////////
	// 初始化 GPRS 模块
//	BOOL ret = m_GPRS.GPRS_Init(_T("COM2:"), 115200, (DWORD)this);
//	if (ret == FALSE)
//	{
//		m_GPRS.GPRS_ClosePort();
//		MessageBox(_T("GPRS初始化失败"));
//	}
//	else
//	{//建立TCP连接
//		m_GPRS.GPRS_SetUpPPPLink();
//		m_remotehost = m_GPRS.GPRS_AskForPPPState();
//		AfxMessageBox(m_remotehost);
//		BOOL ret = m_GPRS.GPRS_SetUpTCPLink(m_remotehost);
//		if(ret == FALSE)
//		{
//			AfxMessageBox(_T("TCP连接建立失败"));
//		}
//		else
//		{
//			AfxMessageBox(_T("TCP连接已经建立"));
//		}
		///////////////////////////////////////////////////////
		//传输数据
//		SetTimer(6,200,NULL);
//	}
//	///////////////////////////////////////////////////////////
//	//创建线程，用于发送数据的处理
	DWORD SendThreadID;
	HANDLE Sendhandle;
	//////////////////////////////////////////////////////////
	//打开TCP连接
	//设置m_tcpClient属性
	m_tcpClient.m_remoteHost = m_remotehost;
	m_tcpClient.m_port = m_remoteport;
	m_tcpClient.OnDisConnect = OnDisConnect;
	m_tcpClient.OnRead = OnTcpRead;
	m_tcpClient.OnError = OnTcpError;
	//打开客户端socket
	m_tcpClient.Open(this);
	//建立与服务器端连接
	if (m_tcpClient.Connect())
	{
		///////////////////////////////////////////////////////
		//传输数据
//		SetTimer(6,300,NULL);
		AfxMessageBox(L"建立连接");
		
		Sendhandle = CreateThread(NULL,0,SendThread,NULL,0,&SendThreadID);
//		Sleep(1000);
		if(!Sendhandle)
		{
			AfxMessageBox(_T("线程创建失败"));
		}
	}
	else
	{
		AfxMessageBox(_T("建立连接失败"));
//		Sendhandle = CreateThread(NULL,0,SendThread,NULL,0,&SendThreadID);
//		Sleep(1000);
//		if(!Sendhandle)
//		{
//		AfxMessageBox(_T("线程创建失败"));
//		}
//		AfxMessageBox(_T("线程创建"));
		return;
	}
}
//心电显示与处理
void CDraw2DGraphDlg::OnBnClickedEcg()
{
	// TODO: 在此添加控件通知处理程序代码
	//启动添加点计时器
//	Sleep(1000);
//	SetTimer(1,500,NULL);
	DWORD ECGThreadID;
	HANDLE ECGThreadhandle;
	ECGThreadhandle = CreateThread(NULL,0,ECGThread,NULL,0,&ECGThreadID);
//	Sleep(1000);
	if(!ECGThreadhandle)
	{
		AfxMessageBox(_T("线程创建失败"));
	}
}
//血压显示与处理
void CDraw2DGraphDlg::OnBnClickedBoolpressure()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置定时器
//	SetTimer(2,TIMER_ARGS,NULL);
//	SetTimer(3,TIMER_ARGS,NULL);
	///////////////////////////////////////////////////////////
	//创建线程，用于血压显示
	DWORD HBPThreadID;
	HANDLE HBPThreadhandle;
	HBPThreadhandle = CreateThread(NULL,0,HBPThread,NULL,0,&HBPThreadID);
//	Sleep(1000);
	if(!HBPThreadhandle)
	{
		AfxMessageBox(_T("线程创建失败"));
	}
	///////////////////////////////////////////////////////////
	//创建线程，用于血压显示
//	DWORD LBPThreadID;
//	HANDLE LBPThreadhandle;
//	LBPThreadhandle = CreateThread(NULL,0,LBPThread,NULL,0,&LBPThreadID);
//	Sleep(1000);
//	if(!LBPThreadhandle)
//	{
//		AfxMessageBox(_T("线程创建失败"));
	//}
}
//血氧饱和度显示与处理
void CDraw2DGraphDlg::OnBnClickedSpo2()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置定时器
//	SetTimer(4,TIMER_ARGS,NULL);
//	Spo2 = TRUE;
	///////////////////////////////////////////////////////////
	//创建线程，用于血压显示
//	DWORD SPO2ThreadID;
//	HANDLE SPO2Threadhandle;
//	SPO2Threadhandle = CreateThread(NULL,0,SPO2Thread,NULL,0,&SPO2ThreadID);
//	Sleep(1000);
//	if(!SPO2Threadhandle)
//	{
//		AfxMessageBox(_T("线程创建失败"));
//	}
}
//脉搏显示与处理
void CDraw2DGraphDlg::OnBnClickedPulse()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置定时器
//	SetTimer(5,TIMER_ARGS,NULL);
//	Pulse = TRUE;
	///////////////////////////////////////////////////////////
	//创建线程，用于血压显示
//	DWORD PulseThreadID;
//	HANDLE PulseThreadhandle;
//	PulseThreadhandle = CreateThread(NULL,0,PulseThread,NULL,0,&PulseThreadID);
//	Sleep(1000);
//	if(!PulseThreadhandle)
//	{
//		AfxMessageBox(_T("线程创建失败"));
//	}
}