#pragma once


// CDlgParams 对话框

class CDlgParams : public CDialog
{
	DECLARE_DYNAMIC(CDlgParams)

public:
	CDlgParams(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParams();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	//
	UINT m_portNo;		//串口号
	UINT m_baud;		//波特率
	UINT m_parity;		//奇偶校验
	UINT m_databits;	//数据位
	UINT m_stopbits;	//停止位
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
