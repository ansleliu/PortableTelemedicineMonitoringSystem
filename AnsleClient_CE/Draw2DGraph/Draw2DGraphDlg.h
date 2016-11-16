// Draw2DGraphDlg.h : 头文件
//
#pragma once
#include "CESerials.h"
#include "CESeries.h"
///////////////////////////////
#include "TCPClient_CE.h"
#include "GPRSCONTDlg.h"
//////////////////////////////
#include "2DGraph.h"
#include "LedCtrl.h"
#include "GPRS.h"
/////////////////////////////
#include "afxcmn.h"
#include "afxwin.h"
//////////////////////////////////////////////////////
//消息定义
#define WM_RECV_SERIAL_DATA			WM_USER + 101
#define WM_COMM_MESSAGE				WM_USER + 102
//定义TCP 客户端接收到数据消息
#define WM_RECV_TCP_DATA			WM_USER + 103
//定义TCP客户端连接断开消息
#define WM_TCP_CLIENT_DISCONNECT	WM_USER + 104
//////////////////////////////////////////////////////
//全局变量
static C2DGraph m_2DGraph;  //二维曲线对象
static CTCPClient_CE m_tcpClient;//定义CTCPClient_CE对象
static CLedCtrl m_led;
static CLedCtrl m_led1;
static CLedCtrl m_led2;
static CLedCtrl m_led3;
static int m_pointCount = 0;    //曲线点计数
static int ecgpoint = 0;
//////////////////////////////////////////////////////
// CDraw2DGraphDlg 对话框
class CDraw2DGraphDlg : public CDialog
{
// 构造
public:
	CDraw2DGraphDlg(CWnd* pParent = NULL);	// 标准构造函数
	CDraw2DGraphDlg *pDlg;

// 对话框数据
	enum { IDD = IDD_DRAW2DGRAPH_DIALOG };


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
	// 串口接收数据处理函数
	afx_msg LONG OnRecvSerialData(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCommMessage(WPARAM pbuf,LPARAM lbufLen);
	//TCP接收数据处理函数
	afx_msg LONG OnRecvTCPData(WPARAM wParam,LPARAM lParam);
	//客户端连接断开消息函数
	afx_msg LONG OnClientDisconnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	//定义串口接收数据函数类型
	static void CALLBACK OnSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	//当有数据接收事件处理函数
	static void CALLBACK OnRead(CWnd* pWnd,const char * buf,int len );
	//串口读取事件
	static void OnSeriesRead(CWnd* pWnd,BYTE* buf,int bufLen);

private:
	//TCP连接断开事件处理函数
	static void  CALLBACK OnDisConnect(void* pOwner);
	//当有数据接收事件处理函数
	static void  CALLBACK OnTcpRead(void* pOwner,const char * buf,DWORD dwBufLen );
	//Socket错误事件处理函数
	static void CALLBACK OnTcpError(void* pOwner,int nErrorCode);

/*心电信号显示*/
private:
//	C2DGraph m_2DGraph;  //二维曲线对象
//	int m_pointCount;    //曲线点计数
	int number;
	
private:
	//定义CTCPClient_CE对象
//	CTCPClient_CE m_tcpClient;
	CString	m_remotehost;
	int		m_remoteport;
public:
//    int ecgpoint;
	int ecgsend;
	GPRSCONTDlg *remotemode;
	BOOL ISOpenRemoteMode;
	int IsClicked;

protected:
	CGPRS m_GPRS;

/*GPRS串口*/
protected:
	CCESerials *m_pSerial1;//串口对象指针
	CCESeries *m_pSerial2;//串口对象指针

public:
	//////////////////////////////////////
	//辅助函数
	BOOL Spo2;
	BOOL Pulse;

	//////////////////////////////////////
private:
	//////////////////////////////////////
	//线程函数定义
//	static DWORD WINAPI FilterThread(PVOID pArg);
	static DWORD WINAPI ECGThread(PVOID pArg);
	static DWORD WINAPI HBPThread(PVOID pArg);
//	static DWORD WINAPI LBPThread(PVOID pArg);
//	static DWORD WINAPI SPO2Thread(PVOID pArg);
//	static DWORD WINAPI PulseThread(PVOID pArg);
	static DWORD WINAPI SendThread(PVOID pArg);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);//定时器消息处理函数
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedRemotemode();
	afx_msg void OnBnClickedEcg();
	afx_msg void OnBnClickedBoolpressure();
	afx_msg void OnBnClickedSpo2();
	afx_msg void OnBnClickedPulse();
};
