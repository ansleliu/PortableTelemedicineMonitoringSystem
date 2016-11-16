
// AnsleServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AnsleServer.h"
#include "AnsleServerDlg.h"
#include "math.h"
#define N 10
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
////////////////////////////////////////////////////////////
ReceiveDataNode *InitDataNode()
{
	ReceiveDataNode *L;
	L=(ReceiveDataNode*)malloc(sizeof(ReceiveDataNode));
	ASSERT(L!=NULL);
	memset(L->ECG,0,ECG_SIZE*sizeof(int));
	int SPO2 = 0;
	memset(L->BP,0,2*sizeof(int));
	int Pulse = 0;
	L->next = NULL;
	return L;
}
ReceiveDataNode *ECG = InitDataNode();
ReceiveDataNode *Ptr_ECG = ECG;
ReceiveDataNode *Ptr_next = ECG;
/////////////////////////////////////////////////////////////
//定义TCP 收到客户端数据消息
#define WM_RECV_TCP_DATA		WM_USER + 101
//定义TCP客户端连接消息
#define WM_TCP_CLIENT_CONNECT	WM_USER + 102
/////////////////////////////////////////////////////////////

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAnsleServerDlg 对话框

CAnsleServerDlg::CAnsleServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnsleServerDlg::IDD, pParent)
	, m_localPort(0)
	, m_status(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnsleServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, LOCALPORT, m_localPort);
	DDX_Control(pDX, IDC_LED, m_led);
	DDX_Control(pDX, IDC_LED1, m_led1);
	DDX_Control(pDX, IDC_LED2, m_led2);
	DDX_Control(pDX, IDC_LED3, m_led3);
	DDX_Text(pDX, IDC_STATE, m_status);
}

BEGIN_MESSAGE_MAP(CAnsleServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_CONNECT,OnClientConnect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(MONITOR, &CAnsleServerDlg::OnBnClickedMonitor)
	ON_BN_CLICKED(CLOSE, &CAnsleServerDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_SET_MSGCENTR_TELCODE, &CAnsleServerDlg::OnBnClickedSetMsgcentrTelcode)
	ON_BN_CLICKED(IDC_INITSMS, &CAnsleServerDlg::OnBnClickedInitsms)
END_MESSAGE_MAP()


// CAnsleServerDlg 消息处理程序

BOOL CAnsleServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//初始化显示控件矩形区域大小
	CRect rect;
	rect.left = 20;
	rect.top = 30;
	rect.right = 675;
	rect.bottom = 300;
	//创建曲线控件实例
	m_2DGraph.Create(_T(""),_T(""),WS_VISIBLE | WS_CHILD, rect, this,0,NULL) ; 
	
	//端口号初始化
	m_localPort = 5000;
	UpdateData(FALSE);
	///////////////////////////////////////////////////////////////////////
	//设置背景色
	m_led.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led.SetColorForeGround(RGB(0,255,255));
	m_led.Display(0);
    //设置定时器
	SetTimer(2,20,NULL);
	/////////////////////////////////////////////////////////////////////
	//设置背景色
	m_led1.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led1.SetColorForeGround(RGB(0,255,255));
	m_led1.Display(0);
    //设置定时器
	SetTimer(3,20,NULL);
	//////////////////////////////////////////////////////////////////
	//设置背景色
	m_led2.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led2.SetColorForeGround(RGB(0,255,255));
	m_led2.Display(0);
    //设置定时器
	SetTimer(4,20,NULL);
	//////////////////////////////////////////////////////////////////
	//设置背景色
	m_led3.SetColorBackGround(RGB(0,0,0));
	//设置前景色
	m_led3.SetColorForeGround(RGB(0,255,255));
	m_led3.Display(0);
    //设置定时器
	SetTimer(5,20,NULL);
	//////////////////////////////////////////////////////////////////
	SetDlgItemText(IDC_SENDMSG_TELCODE, _T("18710846798"));		
	SetDlgItemText(IDC_EDIT_SENDMSG, _T("1.初始化SMS\r\n2.获得服务器IP\r\n3.监听远端连接请求"));
	SetDlgItemText(IDC_MSGCENTR_TELCODE, _T("+8613800290500"));
	IS_GPRSInit = FALSE;//GPRS未初始化
	IS_LISTEN = FALSE;//公网ip未开启监听
	GPRS_IPAddress = _T("");
	//////////////////////////////////////////////////////////////////
	m_pointCount = 0;
	ecg = 0;
	//启动添加点计时器
	SetTimer(1,100,NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAnsleServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CAnsleServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAnsleServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//定时器消息处理函数
void CAnsleServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
		case 1:
			{//心电显示处理代码段
				//坐标的赋值和滤波函数
				int nRandomY;//要显示的点的Y坐标
				{//平滑滤波
					int ECGtemp = 0;
					for(int i=0;i<N;i++)
					{
						ECGtemp += Ptr_ECG->ECG[(ecg+i)%ECG_SIZE];
					}
					Ptr_ECG->ECG[ecg] = (int)(ECGtemp/N);
				}
//				Ptr_ECG->ECG[ecgpoint] = (Ptr_ECG->ECG[ecgpoint]-500)*5;
//				nRandomY = (Ptr_ECG->ECG[ecg]*5/1024)*1000;
				nRandomY = (Ptr_ECG->ECG[ecg]-600)*7;

				//如果曲线点数大于***个点，则删除第1个点。
				if (m_pointCount > X_MAX )
				{
					m_2DGraph.DeleteFirstPoint();
					m_pointCount--;
				}
				
				//给曲线添加点
				m_2DGraph.AppendPoint(nRandomY);	
				


				m_pointCount++;
				/////////////////////////
				//测试用例代码实际工程不用
				ecg++;
				if(ecg == ECG_SIZE)
				{
					ecg=0;
					if(Ptr_ECG->next != NULL)
						Ptr_ECG = Ptr_ECG->next;
					else 
						nRandomY = 0;
				}
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 2:
			{//血压显示处理代码段
				//设置LED显示数字
//				int led = rand()%1000;
				int led = Ptr_ECG->BP[0];
				m_led.Display(led);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 3:
			{//血压显示处理代码段
				//设置LED显示数字
//				int led1 = rand()%100;
				int led1 =Ptr_ECG->BP[1];
				m_led1.Display(led1);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 4:
			{//血氧饱和度显示处理代码段
				//设置LED显示数字
//				int led2 = rand()%100;
				int led2 =Ptr_ECG->SPO2;
				m_led2.Display(led2);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 5:
			{//脉搏显示处理代码段
				//设置LED显示数字
//				int led3 = rand()%100;
				int led3 =Ptr_ECG->Pulse;
				m_led3.Display(led3);  	
				CDialog::OnTimer(nIDEvent);
				break;
			}
		case 6:
			{//短信接收处理代码段
				BOOL ret;
				CString strTelCode, strMsg;
				ret = m_GPRS.GPRS_ReadShortMessage(1, &strTelCode, &strMsg);// 读取第 0 条短信
				if (ret == TRUE)
				{
					SetDlgItemText(IDC_RECVMSG_TELCODE, strTelCode);//显示电话号码 
					SetDlgItemText(IDC_DISP_RECVMSG, strMsg);	// 显示短信内容
					m_GPRS.GPRS_DeleteShortMsg(1);	
					CString askIP = _T("I Need Your IP Address");
					if(strMsg.Find(askIP,0)>=0)
					{
						SetDlgItemText(IDC_SENDMSG_TELCODE,strTelCode);
						//向客户端回送本地IP地址
						CString myIPAddress;
						myIPAddress = _T("IP")+GetLocalIP()+_T("END");
						BOOL ret = m_GPRS.GPRS_SendShortMessage(strTelCode, myIPAddress);//发送短信
						SetDlgItemText(IDC_EDIT_SENDMSG, myIPAddress);
						if (ret == TRUE)
						{
							KillTimer(6);
							PrintStatusInfo(_T("短信发送成功."));
						}
							
						else
							PrintStatusInfo(_T("短信发送失败."));
					}
				}
				CDialog::OnTimer(nIDEvent);
				break;
			}
		default:
			{
				break;
			}
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//SMS 、GPRS处理代码段
/////////////////////////////////////////////////////////////////////////////////////////////
void CAnsleServerDlg::OnBnClickedSetMsgcentrTelcode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCode;
	GetDlgItemText(IDC_MSGCENTR_TELCODE, strCode);
	BOOL ret = m_GPRS.GPRS_SetShortMSGCenterTel(strCode);	//设置短信中号码 
	if (ret == TRUE)
		PrintStatusInfo(_T("设置短信中心号码成功."));
	else
		PrintStatusInfo(_T("设置短信中心号码失败."));
}

void CAnsleServerDlg::OnBnClickedInitsms()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret = m_GPRS.GPRS_Init(_T("COM4:"), 115200, (DWORD)this);
	if (ret == FALSE)
	{
	//	m_GPRS.GPRS_ClosePort();
		PrintStatusInfo(_T("初始化失败, 请检查是否安装正确"));
	}	
	else
	{
		PrintStatusInfo(_T("SMS初始化成功"));
		m_GPRS.GPRS_DeleteShortMsg(1);	//删除第 1 条短信
		SetTimer(6, 5000, NULL);			/* 每 1 秒读取一次短信 */
	}
}
/////////////////////////////////////////////////////////////////////////////////////
//TCP-IP连接处理代码段
/////////////////////////////////////////////////////////////////////////////////////
//客户端连接建立事件处理函数
void CALLBACK  CAnsleServerDlg::OnClientConnect(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//拷贝内存，得到客户端IP地址
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);
	
	CAnsleServerDlg *pThis = (CAnsleServerDlg*)pOwner;
	
	//发送异步消息，表示有客户端连接，消息处理完后，需要释放内存
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,0,LPARAM(szAddress));
}

//客户端SOCKET关闭事件处理函数
void  CALLBACK CAnsleServerDlg::OnClientClose(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//拷贝内存，得到客户端IP地址
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);

	CAnsleServerDlg *pThis = (CAnsleServerDlg*)pOwner;
	
	//发送异步消息，表示有客户端连接，消息处理完后，需要释放内存
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,1,LPARAM(szAddress));

}

//服务器端收到来自客户端的数据
void CALLBACK CAnsleServerDlg::OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen )
{
	DATA_BUF *pGenBuf = new DATA_BUF;
	char *pRecvBuf = NULL; //接收缓冲区
	//得到父对象指针
	CAnsleServerDlg* pThis = (CAnsleServerDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf中
	pRecvBuf = new char[dwBufLen];
	MoveMemory(pRecvBuf,buf,dwBufLen);

	ZeroMemory(pGenBuf,sizeof(DATA_BUF));
	pGenBuf->dwBufLen = dwBufLen;
	pGenBuf->sBuf = pRecvBuf;
	
	//
	wcscpy(pGenBuf->szAddress,pTcpCustom->m_RemoteHost);

	//发送异步消息，表示收到TCP数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pGenBuf),LPARAM(pTcpCustom));

   
}

//客户端Socket错误事件处理函数
void CALLBACK CAnsleServerDlg::OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode)
{	
}

//服务器端Socket错误事件处理函数
void CALLBACK CAnsleServerDlg::OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode)
{	
}

//TCP接收数据处理函数
LONG CAnsleServerDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	DATA_BUF *pGenBuf = (DATA_BUF*)wParam; //通用缓冲区
	CTCPCustom_CE* pTcpCustom= (CTCPCustom_CE* )lParam; //TCP客户端对象
	//接收显示列表
	CListBox * pLstRecv = (CListBox*)GetDlgItem(RECEIVELIST);
	ASSERT(pLstRecv != NULL);
	//接收到的数据
	CString strRecv;
	CString strLen;
	strLen.Format(L"%d",pGenBuf->dwBufLen);
	strRecv = CString(pGenBuf->sBuf);

	pLstRecv->AddString(_T("*************************************"));
	pLstRecv->AddString(_T("来自于: ") + CString(pGenBuf->szAddress) );
	pLstRecv->AddString(_T("数据长度为:")+strLen);
	pLstRecv->AddString(strRecv);
	///////////////////////////////////////////////
	int flag_1;
	int flag_2;
	int pos = 0;
	CString temp = L"";
	CString num = L"";

	while (!strRecv.IsEmpty())
	{
		int args = 0;
		CString arg = L"";
		flag_1 = strRecv.Find('$');
		arg = strRecv.Left(flag_1);
		////////////////////////////////////
		if(arg.Compare(L"ECG") == 0)
		{
			arg.Replace(L"ECG" ,L"1");
			args = _ttoi(arg);
		}
		else if(arg.Compare(L"SPO2") == 0)
		{
			arg.Replace(L"SPO2" ,L"2");
			args = _ttoi(arg);
		}
		else if(arg.Compare(L"BP") == 0)
		{
			arg.Replace(L"BP" ,L"3");
			args = _ttoi(arg);
		}
		else if(arg.Compare(L"Pulse") == 0)
		{
			arg.Replace(L"Pulse" ,L"4");
			args = _ttoi(arg);
		}
        /////////////////////////////////////
		flag_2 = strRecv.Find('#');
		temp = strRecv.Mid(flag_1+1,flag_2-flag_1-1);
		strRecv.Delete(0,flag_2+1);
		switch(args)
		{
			case 1:
				{
					int i = 0;
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->ECG[i] =  _ttoi(num);
						i++;
						temp.Delete(0,pos+1);
					}
					break;
				}
			case 2:
				{
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->SPO2 =  _ttoi(num);
						temp.Delete(0,pos+1);
					}
					break;
				}
			case 3:
				{
					int i=0;
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->BP[i] =  _ttoi(num);
						i++;
						temp.Delete(0,pos+1);
					}
					break;
				}
			case 4:
				{
					while(!temp.IsEmpty())
					{
						pos = temp.Find(';');
						num = temp.Left(pos);
						Ptr_next->Pulse =  _ttoi(num);
						temp.Delete(0,pos+1);
					}
					break;
				}
		}
	}
	Ptr_next->next = InitDataNode();
	Ptr_next = Ptr_next->next;
	//启动添加点计时器
//	SetTimer(1,20,NULL);
	///////////////////////////////////////////////    
	
	//发送回应命令
	char * sendBuf = "发送成功!";
	int sendLen=0;
	sendLen = strlen(sendBuf)+1;
	if (!m_tcpServer.SendData(pTcpCustom,sendBuf,sendLen))
	{
		AfxMessageBox(_T("发送失败"));
	}

	//释放内存
	delete[] pGenBuf->sBuf;
	pGenBuf->sBuf = NULL;
	delete pGenBuf;
	pGenBuf = NULL;
	return 0;
}

//客户端连接断开消息函数
LONG CAnsleServerDlg::OnClientConnect(WPARAM wParam,LPARAM lParam)
{
	int iIndex = 0;
	TCHAR *szAddress = (TCHAR*)lParam;
	CString strAddrss = szAddress;
	
	CListBox * pLstConn = (CListBox*)GetDlgItem(CONNECTLIST);
	ASSERT(pLstConn != NULL);

	if (wParam == 0)
	{
		pLstConn->AddString(strAddrss + _T("建立连接"));
	}
	else
	{
		iIndex = pLstConn->FindString(iIndex,strAddrss + _T("建立连接"));
		if (iIndex != LB_ERR)
		{
			pLstConn->DeleteString(iIndex); 
		}
	}

	//释放内存
	delete[] szAddress;
	szAddress = NULL;
	return 0;
}

void CAnsleServerDlg::OnBnClickedMonitor()
{
	// TODO: 在此添加控件通知处理程序代码
	IS_LISTEN = TRUE;
	UpdateData(TRUE);
	//设置m_tcpServer属性
   	m_tcpServer.m_LocalPort = m_localPort;
	m_tcpServer.m_pOwner = this;
	m_tcpServer.OnClientConnect = OnClientConnect;
	m_tcpServer.OnClientClose = OnClientClose;
	m_tcpServer.OnClientRead = OnClientRead;
	m_tcpServer.OnClientError = OnClientError;
	m_tcpServer.OnServerError = OnServerError;
	if (m_tcpServer.Open() <= 0)
	{
		AfxMessageBox(_T("监听失败"));
		return;
	}
	else
	{
		PrintStatusInfo(_T("监听成功"));
	}
}

void CAnsleServerDlg::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	CListBox * pLstConn = (CListBox*)GetDlgItem(CONNECTLIST);
	ASSERT(pLstConn != NULL);
	
	CListBox * pLstRecv = (CListBox*)GetDlgItem(RECEIVELIST);
	ASSERT(pLstRecv != NULL);
	
	//
	if (m_tcpServer.Close() <=0)
	{
		AfxMessageBox(_T("关闭TCP服务器失败"));
		return;
	}
	
	//清空列表
	pLstConn->ResetContent();
	pLstRecv->ResetContent();
}
////////////////////////////////////////////////////////////////////////////////////////
//辅助函数
////////////////////////////////////////////////////////////////////////////////////////
CString CAnsleServerDlg::GetLocalIP()
{
	HOSTENT *LocalAddress;
	char	*Buff;
	TCHAR	*wBuff;
	CString strReturn = _T("");
	//创建新的缓冲区
	Buff = new char[256];
	wBuff = new TCHAR[256];
	//置空缓冲区
	memset(Buff, '\0', 256);
	memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
	//得到本地计算机名
	if (gethostname(Buff, 256) == 0)
	{
		//转换成双字节字符串
		mbstowcs(wBuff, Buff, 256);
		//得到本地地址
		LocalAddress = gethostbyname(Buff);
		//置空buff
		memset(Buff, '\0', 256);
		//组合本地IP地址
		sprintf(Buff, "%d.%d.%d.%d\0", LocalAddress->h_addr_list[0][0] & 0xFF,
			LocalAddress->h_addr_list[0][1] & 0x00FF, LocalAddress->h_addr_list[0][2] & 0x0000FF, LocalAddress->h_addr_list[0][3] & 0x000000FF);
		//置空wBuff
		memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
		//转换成双字节字符串
		mbstowcs(wBuff, Buff, 256);
		//设置返回值
		strReturn = wBuff;
	}
	else
	{
	}
	//释放Buff缓冲区
	delete[] Buff;
	Buff = NULL;
	//释放wBuff缓冲区
	delete[] wBuff;
	wBuff = NULL;
	return strReturn;
}
void CAnsleServerDlg::PrintStatusInfo(CString strinfo)
{
	CString str;
	str.Format(_T("%s\r\n"),strinfo);
	m_status += str;
	UpdateData(FALSE);
}