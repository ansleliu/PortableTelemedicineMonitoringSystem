// AnsleServerDlg.h : 头文件
//
#pragma once
#include "2DGraph.h"
#include <stdlib.h>
#include "TCPServer_CE.h"
#include "TCPCustom_CE.h"
#include "LedCtrl.h"
#include "GPRS.h"
#include "afxwin.h"
/////////////////////////////////////
#define ECG_SIZE   500
/////////////////////////////////////
//定义通用缓冲区
typedef struct  _DATA_BUF
{
	DWORD dwBufLen;
	char* sBuf;	
	TCHAR szAddress[MAX_PATH];
}DATA_BUF,*PDATA_BUF;
//接收数据存储区
typedef struct ReceiveDataNode
{
	int ECG[ECG_SIZE];
	int SPO2;
	int BP[2];
	int Pulse;
	struct ReceiveDataNode *next;
}ReceiveDataNode;
ReceiveDataNode *InitReceiveDataNode();
//////////////////////////////////////////////////////
//全局变量
static C2DGraph m_2DGraph;  //二维曲线对象
static CLedCtrl m_led;
static CLedCtrl m_led1;
static CLedCtrl m_led2;
static CLedCtrl m_led3;
static int m_pointCount = 0;    //曲线点计数
static int ecg;
// CAnsleServerDlg 对话框
class CAnsleServerDlg : public CDialog
{
// 构造
public:
	CAnsleServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ANSLESERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//TCP接收数据处理函数
	afx_msg LONG OnRecvTCPData(WPARAM wParam,LPARAM lParam);
	//客户端连接断开消息函数
	afx_msg LONG OnClientConnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	/*心电信号显示*/
private:
//	C2DGraph m_2DGraph;  //二维曲线对象
//	int m_pointCount;    //曲线点计数
	//测试辅助
//	int ecg;

private:
	//定义CTCPServer_CE对象
	CTCPServer_CE m_tcpServer;
	CGPRS m_GPRS;
	//得到本地的IP地址
	CString GetLocalIP();
	void PrintStatusInfo(CString strinfo);
	BOOL IS_GPRSInit;
	BOOL IS_LISTEN;
	CString GPRS_IPAddress;

public:
	//定义端口号
	DWORD m_localPort;
//	CLedCtrl m_led;
//	CLedCtrl m_led1;
//	CLedCtrl m_led2;
//	CLedCtrl m_led3;

private:
	//客户端连接建立事件处理函数
	static void CALLBACK	OnClientConnect(void* pOwner,CTCPCustom_CE *pTcpCustom);
	//客户端SOCKET关闭事件处理函数
	static void  CALLBACK OnClientClose(void* pOwner,CTCPCustom_CE*pTcpCustom);
	//服务器端收到来自客户端的数据
	static  void CALLBACK OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen );
	//客户端Socket错误事件处理函数
	static  void CALLBACK OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode);
	//服务器端Socket错误事件处理函数
	static void CALLBACK OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);//定时器消息处理函数
	afx_msg void OnBnClickedMonitor();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSetMsgcentrTelcode();
	afx_msg void OnBnClickedInitsms();
	CString m_status;
};
