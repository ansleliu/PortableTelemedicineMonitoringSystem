#pragma once
#include "resourceppc.h"
#include "GPRS.h"
#include "Utility.h"
// GPRSCONTDlg 对话框

#define WM_COMM_MESSAGE WM_USER + 102

class GPRSCONTDlg : public CDialog
{
	DECLARE_DYNAMIC(GPRSCONTDlg)

public:
	GPRSCONTDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~GPRSCONTDlg();
	//////////////////////////////
	CGPRS m_GPRS;
	void PrintStatusInfo(CString strinfo);

// 对话框数据
	enum { IDD = IDD_DIALOG1 };
	CString	m_status;
	CString	m_remotehost;
	int		m_remoteport;

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
	static void CALLBACK OnGPRSRecv(DWORD UserParam, DWORD Status, CString strData); 
private:
	BOOL IsWanted(CString sms,CString cmd);
	CString GetIpFromSMS(CString scmd,CString ecmd,CString sms);
private:
	//串口读取事件
	static void OnSeriesRead(CWnd* pWnd,BYTE* buf,int bufLen);
public:
	afx_msg void OnBnClickedSetMsgcentrTelcode();
	afx_msg void OnBnClickedSendmsg();
	afx_msg void OnBnClickedInitsms();
};
