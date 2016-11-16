/******************************************
// DSPDemoDlg.h
//窗体
******************************************/
#pragma once
#include "C2DGraph.h"
#include "Serial.h"
#include "afxcmn.h"
//每个链表节点所能容纳的最大数据量
#define ECG_SIZE   400
////////////////////////////////////////
//串口数据暂存数据结构
typedef struct ECGDataNode
{
	int ECG[ECG_SIZE];
	struct ECGDataNode *next; 
}ECGDataNode;
///////////////////////////////////////
ECGDataNode  *InitECGDataNode();

// CDSPDemoDlg 对话框
class CDSPDemoDlg : public CDialog
{
// 构造
public:
	CDSPDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DSPDEMO_DIALOG };
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
	// 串口接收数据处理函数
	afx_msg LONG OnRecvSerialData(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()	
/*信号显示*/
private:
	 CEdit *pEdtRecvMsg;
	 C2DGraph m_2DGraph;  //二维曲线对象
	 int m_pointCount;    //曲线点计数
	 int ecgpoint; 
	 int flag;
	/////////////////////////////////////////////////////
	CSerial* pCSerial;
	static void CALLBACK OnSerialRead(DWORD UserParam,
									  char* buf,
									  DWORD bufLen);
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);//定时器消息处理函数
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnEnChangeEdtRecv();
};
