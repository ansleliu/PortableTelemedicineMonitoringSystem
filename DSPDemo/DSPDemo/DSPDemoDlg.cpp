
// DSPDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DSPDemo.h"
#include "DSPDemoDlg.h"
#include "DlgParams.h"
#include "DSP.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
////////////////////////////////////////////////////
#define MAX_SIZE  4096
#define MIN_SIZE  0
#define N 100
static int ecg;//要显示的点的Y坐标
//定义串口数据接收消息常量
#define WM_RECV_SERIAL_DATA          WM_USER + 101
////////////////////////////////////////////////////
ECGDataNode  *InitECGDataNode()
{
	ECGDataNode *L;
	L=(ECGDataNode*)malloc(sizeof(ECGDataNode));
	ASSERT(L!=NULL);
	memset(L->ECG,0,ECG_SIZE*sizeof(int));
	L->next = NULL;
	return L;
}
///////////////////////////////////////////////////
ECGDataNode *ECG = InitECGDataNode();
ECGDataNode *Ptr_ECG = ECG;
ECGDataNode *Ptr_Next_ECG = ECG;
ECGDataNode *Ptr_Next_DSP = ECG;
ECGDataNode *Ptr_Next_FOR = ECG;
//////////////////////////////////////////////////
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


// CDSPDemoDlg 对话框




CDSPDemoDlg::CDSPDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDSPDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pCSerial = NULL;
}

void CDSPDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDSPDemoDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECV_SERIAL_DATA,OnRecvSerialData)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CDSPDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDSPDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDSPDemoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, &CDSPDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDSPDemoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON4, &CDSPDemoDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDSPDemoDlg::OnBnClickedButton5)
	ON_EN_CHANGE(IDC_EDT_RECV, &CDSPDemoDlg::OnEnChangeEdtRecv)
END_MESSAGE_MAP()


// CDSPDemoDlg 消息处理程序

BOOL CDSPDemoDlg::OnInitDialog()
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

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	//初始化显示控件矩形区域大小
	CRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = 690;
	rect.bottom = 260;
	//创建曲线控件实例
	m_2DGraph.Create(_T(""),_T(""),WS_VISIBLE | WS_CHILD, rect, this,0,NULL) ; 
	////////////////////////////////////////////
	m_pointCount = 0;
	ecgpoint = 0;
	ecg = 0;
	flag = 0;
	pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_EDT_RECV);
	//启动添加点计时器
//	SetTimer(1,20,NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDSPDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDSPDemoDlg::OnPaint()
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
HCURSOR CDSPDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
DWORD CDSPDemoDlg::ThreadProc(LPVOID lpParameter)
{
	CDSPDemoDlg* args = NULL;
	args = (CDSPDemoDlg*)lpParameter;
//	FILE* fp;
//	fp = fopen("ECG.txt","r");
	return 1;

}
//定时器消息处理函数
void CDSPDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nRandomY;//要显示的点的Y坐标
//	 //坐标的赋值和滤波函数
	{//平滑滤波
		int ECGtemp = 0;
		for(int i=0;i<N;i++)
		{
			int currnum = ecgpoint+i;
			if(currnum >= ECG_SIZE )
			{
				Ptr_Next_DSP = Ptr_ECG->next;
				ECGtemp += Ptr_Next_DSP->ECG[currnum-ECG_SIZE];
			}
			else
			{
				ECGtemp += Ptr_ECG->ECG[currnum];
			}
		}
		Ptr_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
	}
	nRandomY = Ptr_ECG->ECG[ecgpoint];
	if(nRandomY<100)
	{
		nRandomY = 100;
	}
	/////////////////////////
	ecgpoint++;
	if(ecgpoint == ECG_SIZE)
	{
		if(Ptr_ECG->next != NULL)
		{
			ecgpoint=0;
			Ptr_ECG = Ptr_ECG->next;
		}
		else
		{
			KillTimer(1);
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
	
	/////////////////////////
	CDialog::OnTimer(nIDEvent);
}

void CALLBACK CDSPDemoDlg::OnSerialRead(DWORD UserParamr,char* buf,DWORD bufLen)
{
	char *pRecvBuf; //接收缓冲区
	//得到父对象指针
	CDSPDemoDlg* pThis = (CDSPDemoDlg*)UserParamr;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new char[bufLen];
	ZeroMemory(pRecvBuf,bufLen);
	MoveMemory(pRecvBuf,buf,bufLen);
	//////////////////////////////////////////////////////
	CString strOldRecv = L"";
	CString strRecv = L"";
	strRecv = CString(buf);
	DWORD id;
	HANDLE handle;
	handle = CreateThread(NULL,0,ThreadProc,NULL,0,&id);
	if(strRecv.Find(_T(","),0)>=0)
	{
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
			else
			{
				num = strRecv.Left(pos);
				int len = num.GetLength();
				strRecv.Delete(0,pos+1);
				Ptr_Next_ECG->ECG[ecg] = _ttoi(num);
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
	}
	//发送异步消息，表示收到串口数据，消息处理完，应释放内存
	pThis->PostMessage(WM_RECV_SERIAL_DATA,WPARAM(pRecvBuf),bufLen);
	delete[] pRecvBuf;
	pRecvBuf = NULL;
}
// 串口接收数据处理函数
LONG CDSPDemoDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
{
	flag++;
	//串口接收到的BUF
	char *pRecvBuf; //接收缓冲区
//	CHAR *pBuf = (CHAR*)wParam;
	//串口接收到的BUF长度
	DWORD dwBufLen = lParam;
	//将接收的缓冲区拷贝到pRecvBuf种
	pRecvBuf = new char[dwBufLen];
	ZeroMemory(pRecvBuf,dwBufLen);
	MoveMemory(pRecvBuf,(CHAR*)wParam,dwBufLen);
	//////////////////////////////////////////////////////
	CString strOldRecv = L"";
	CString strRecv = L"";
	strRecv = CString(pRecvBuf);
	ASSERT(pEdtRecvMsg != NULL);
	//得到接收框中的历史文本
	pEdtRecvMsg->GetWindowTextW(strOldRecv);	
	/////////////////////////////////////////////////////////////////
	//将新接收到的文本添加到接收框中
	pEdtRecvMsg->SetSel(-1,-1);
//	pEdtRecvMsg->ReplaceSel(strRecv);
	strOldRecv = strOldRecv+_T("\r\n")+ strRecv;
	pEdtRecvMsg->SetWindowTextW(strOldRecv);
	if(strOldRecv.GetLength()>=2048)
	{
		pEdtRecvMsg->SetWindowTextW(_T(""));
	}
	//释放内存
	//接收框
	delete[] pRecvBuf;
	pRecvBuf = NULL;
	/////////////////////////////////////////////////////////////////
	if(flag>2*N)
	{
		flag--;
		int nRandomY;//要显示的点的Y坐标
	//	 //坐标的赋值和滤波函数
		{//平滑滤波
			int ECGtemp = 0;
			for(int i=0;i<N;i++)
			{
				int currnum = ecgpoint+i;
				if(currnum >= ECG_SIZE )
				{
					Ptr_Next_DSP = Ptr_ECG->next;
					ECGtemp += Ptr_Next_DSP->ECG[currnum-ECG_SIZE];
				}
				else
				{
					ECGtemp += Ptr_ECG->ECG[currnum];
				}
			}
			Ptr_ECG->ECG[ecgpoint] = (int)(ECGtemp/N);
		}
		nRandomY = Ptr_ECG->ECG[ecgpoint];
		if(nRandomY<100)
		{
			nRandomY = 100;
		}
		/////////////////////////
		ecgpoint++;
		if(ecgpoint == ECG_SIZE)
		{
			if(Ptr_ECG->next != NULL)
			{
				ecgpoint=0;
				Ptr_Next_FOR = Ptr_ECG;
				Ptr_ECG = Ptr_ECG->next;

				Ptr_Next_FOR->next = NULL;
				free(Ptr_Next_FOR);
				Ptr_Next_FOR = NULL;
			}
			else
			{
//				KillTimer(1);
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
	}
	return 0;
}
void CDSPDemoDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//判断串口是否已经打开
	if (pCSerial != NULL)
	{
		pCSerial->Close();
		delete pCSerial;
		pCSerial = NULL;
	}
	//串口参数输入对话框
	CDlgParams dlgParams;	
	if (dlgParams.DoModal() == IDOK)
	{
		TCHAR szPort[15];
		wsprintf(szPort, L"COM%d:",dlgParams.m_portNo);
		pCSerial = new CSerial;
		pCSerial->m_OnSeriesRead = OnSerialRead;//串口接收成功回调函数 
		BOOL ret;
		ret = pCSerial->Open((DWORD)this,
							  szPort,
							  dlgParams.m_baud,
							  8,
							  0,
							  1);
		if (ret == FALSE)	//打开串口, 数据位为8,停止位为1,无校验位
		{
			AfxMessageBox(L"串口打开失败");
		}
		else
		{

		}
	}
}

void CDSPDemoDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (pCSerial != NULL)
	{
		pCSerial->Close();
		delete pCSerial;
		pCSerial = NULL;
	}
}

void CDSPDemoDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	char * buf  =NULL;  //定义发送缓冲区
	DWORD dwBufLen = 0;   //定义发送缓冲区长度
	CString strSend = L"";

	//得到发送输入框
	CEdit *pEdtSendMsg = (CEdit*)GetDlgItem(IDC_EDT_SEND);
	ASSERT(pEdtSendMsg != NULL);
	//串口如果没有打开，直接返回
	if (pCSerial == NULL)
	{
		AfxMessageBox(L"串口未打开");
		return;
	}
	//得到待发送的字符串
	pEdtSendMsg->GetWindowTextW(strSend);

	//将待发送的字符串转换成单字节，进行发送
	buf = new char[strSend.GetLength()*2+1];
	ZeroMemory(buf,strSend.GetLength()*2+1);
	//转换成单字节进行发送	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,
						strSend.GetBuffer(strSend.GetLength()),
						strSend.GetLength(),
						buf,
						strSend.GetLength()*2,
						NULL,NULL);

	dwBufLen = strlen(buf)+1;
	//发送字符串
	pCSerial->SendData(buf,dwBufLen);

	//释放内存
	delete[] buf;
	buf = NULL;
}

void CDSPDemoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CDSPDemoDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CDSPDemoDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	//输入框清空
	CEdit *pEdtSendMsg = (CEdit*)GetDlgItem(IDC_EDT_SEND);
	ASSERT(pEdtSendMsg != NULL);
	pEdtSendMsg->SetWindowTextW(_T(""));
	//接收框清空
	CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_EDT_RECV);
	ASSERT(pEdtRecvMsg != NULL);
	//得到接收框中的历史文本
	pEdtRecvMsg->SetWindowTextW(_T(""));
}

void CDSPDemoDlg::OnBnClickedButton5()
{
	//启动添加点计时器
//	SetTimer(1,20,NULL);
	
}

void CDSPDemoDlg::OnEnChangeEdtRecv()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
