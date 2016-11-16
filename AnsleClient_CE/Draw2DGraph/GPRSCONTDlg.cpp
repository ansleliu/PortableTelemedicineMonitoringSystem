// GPRSCONTDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GPRSCONTDlg.h"

#define BUFFER_LENGTH 32
// GPRSCONTDlg 对话框

IMPLEMENT_DYNAMIC(GPRSCONTDlg, CDialog)

GPRSCONTDlg::GPRSCONTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GPRSCONTDlg::IDD, pParent)
{
//	m_status = _T("");
	m_remotehost = _T("");
	m_remoteport = 0;
}

GPRSCONTDlg::~GPRSCONTDlg()
{
}

void GPRSCONTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_status);
	DDX_Text(pDX, IDC_EDIT_REMOTEHOST, m_remotehost);
	DDX_Text(pDX, IDC_EDIT_REMOTEPORT, m_remoteport);
}


BEGIN_MESSAGE_MAP(GPRSCONTDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SET_MSGCENTR_TELCODE, &GPRSCONTDlg::OnBnClickedSetMsgcentrTelcode)
	ON_BN_CLICKED(IDC_SENDMSG, &GPRSCONTDlg::OnBnClickedSendmsg)
	ON_BN_CLICKED(IDC_INITSMS, &GPRSCONTDlg::OnBnClickedInitsms)
END_MESSAGE_MAP()

BOOL GPRSCONTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);				// Set big icon
	SetIcon(m_hIcon, FALSE);			// Set small icon
	CenterWindow(GetDesktopWindow());	// center to the hpc screen
	///////////////////////////////////////////////////////////////////////
	// 初始化 GPRS 模块发送短信专用
	SetDlgItemText(IDC_SENDMSG_TELCODE, _T("18392621985"));		
	SetDlgItemText(IDC_EDIT_SENDMSG, _T("I Need Your IP Address"));	
	SetDlgItemText(IDC_MSGCENTR_TELCODE, _T("+8613800290500"));
	m_GPRS.OnGPRSRecv = OnGPRSRecv;	/* 设置回调函数	*/
	//////////////////////////////////////////////////////////////////////
	///拨号连接专用
	m_remotehost = _T("192.168.16.1");
	m_remoteport = 5000;
	UpdateData(FALSE);

	return TRUE;
}


// GPRSCONTDlg 消息处理程序
/*******************************************************************************************
函数名称: CALLBACK GPRSCONTDlg::OnGPRSRecv
描    述: GPRS 接收回调函数, 当有电话打入或对方挂机时, 将执行该函数
输入参数: DWORD UserParam: 用户在调用 GPRS_Init() 函数时传入的参数.
		  DWORD Status	 : GPRS 状态
		  CString strData: 状态对应的字符串, 如果有电话打入, 则该字符为来电号码	
输出参数: 无
返    回: 无
********************************************************************************************/
void CALLBACK GPRSCONTDlg::OnGPRSRecv(DWORD UserParam, DWORD Status, CString strData)
{

}

/*******************************************************************************************
函数名称: OnTimer
描    述: 定时服务处理函数
********************************************************************************************/
void GPRSCONTDlg::OnTimer(UINT nIDEvent) 
{
	BOOL ret;
	CString strTelCode, strMsg;
	ret = m_GPRS.GPRS_ReadShortMessage(1, &strTelCode, &strMsg);// 读取第 0 条短信 
	if (ret == TRUE)
	{
		SetDlgItemText(IDC_RECVMSG_TELCODE, strTelCode);// 显示电话号码
		SetDlgItemText(IDC_DISP_RECVMSG, strMsg);//显示短信内容
		BOOL ret = IsWanted(strMsg,L"IP");
		if(ret == TRUE)
		{
			KillTimer(1);
			CString Ip = GetIpFromSMS(L"IP",L"END",strMsg); /*提取主机IP地址*/
			m_remotehost = Ip;
			SetDlgItemText(IDC_EDIT_REMOTEHOST, Ip);
			m_GPRS.GPRS_DeleteShortMsg(1);	
			m_GPRS.GPRS_ClosePort();
		}
		else
			m_GPRS.GPRS_DeleteShortMsg(1);/* 删除短信 */
	}
	CDialog::OnTimer(nIDEvent);
}
BOOL GPRSCONTDlg::IsWanted(CString sms,CString cmd)
{
	if(sms.Find(cmd,0) != 0)
	{
		PrintStatusInfo(_T("未收到正确的响应短信，请重新发送请求短信"));
		return FALSE;
	}
	else 
		return TRUE;
}
CString GPRSCONTDlg::GetIpFromSMS(CString scmd,CString ecmd,CString sms)
{
	CString smsstr;
	int len1,len2;
	len1 = scmd.GetLength();
	len2 = ecmd.GetLength();
	int pos1,pos2;
	pos1 = sms.Find(scmd,0) + len1;
	pos2 = sms.Find(ecmd,0);
	smsstr = sms.Mid(pos1,pos2-pos1);
	return smsstr;
}
void GPRSCONTDlg::OnBnClickedSetMsgcentrTelcode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strCode;

	GetDlgItemText(IDC_MSGCENTR_TELCODE, strCode);

	BOOL ret = m_GPRS.GPRS_SetShortMSGCenterTel(strCode);/* 设置短信中号码 */
	if (ret == TRUE)
		PrintStatusInfo(_T("设置短信中心号码成功."));
	else
		PrintStatusInfo(_T("设置短信中心号码失败."));
}

void GPRSCONTDlg::OnBnClickedSendmsg()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTelCode, strMsg;

	GetDlgItemText(IDC_SENDMSG_TELCODE, strTelCode);/* 获取发送短信电话号码及内容*/	
	GetDlgItemText(IDC_EDIT_SENDMSG, strMsg);	

	if ((strTelCode == "") || (strMsg == ""))
	{/* 判断输入内容是否正确 */
		PrintStatusInfo(_T("电话号码或短信内容不能为空."));
		return;
	}

	BOOL ret = m_GPRS.GPRS_SendShortMessage(strTelCode, strMsg);/* 发送短信 */
	if (ret == TRUE)
		PrintStatusInfo(_T("短信发送成功."));
	else
		PrintStatusInfo(_T("短信发送失败."));
}
void GPRSCONTDlg::OnBnClickedInitsms()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret = m_GPRS.GPRS_Init(_T("COM2:"), 115200, (DWORD)this);
	if (ret == FALSE)
	{
		PrintStatusInfo(_T("GPRS初始化失败, 请检查是否安装正确."));
	}
	else
	{
		PrintStatusInfo(_T("GPRS初始化成功."));
		m_GPRS.GPRS_DeleteShortMsg(1);/* 删除第 1 条短信*/
		SetTimer(1, 1500, NULL);			/* 每 1.5 秒读取一次短信 */
	}
}
void GPRSCONTDlg::PrintStatusInfo(CString strinfo)
{
	CString str;
	str.Format(_T("%s\r\n"),strinfo);
	m_status += str;
	UpdateData(FALSE);
}