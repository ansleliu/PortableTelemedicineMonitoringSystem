// DlgParams.cpp : 实现文件
//

#include "stdafx.h"
#include "DSPDemo.h"
#include "DlgParams.h"


// CDlgParams 对话框

IMPLEMENT_DYNAMIC(CDlgParams, CDialog)

CDlgParams::CDlgParams(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgParams::IDD, pParent)
{

}

CDlgParams::~CDlgParams()
{
}

void CDlgParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgParams, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgParams::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgParams::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgParams 消息处理程序
BOOL CDlgParams::OnInitDialog()
{
	CDialog::OnInitDialog();
	//
	//初始化串口参数下拉框，下拉列表
	CString strItem = L"";
	CComboBox *pCmbComNo = (CComboBox*)GetDlgItem(IDC_CMB_COM_NO);
	CComboBox *pCmbComBaud = (CComboBox*)GetDlgItem(IDC_CMB_COM_BAUD);
	CComboBox *pCmbComParity = (CComboBox*)GetDlgItem(IDC_CMB_COM_PARITY);
	CComboBox *pCmbComDatabits = (CComboBox*)GetDlgItem(IDC_CMB_COM_DATABITS);
	CComboBox *pCmbComStopbits = (CComboBox*)GetDlgItem(IDC_CMB_COM_STOPBITS);

	//初始化数据
	//串口号
	pCmbComNo->ResetContent();
	for (int i=0; i < 20; i++)
	{
		strItem.Format(L"%d",i+1);
		pCmbComNo->AddString(strItem);
	}
	pCmbComNo->SetCurSel(0);// com1:

	//波特率
	pCmbComBaud->ResetContent();
	pCmbComBaud->AddString(L"300");
	pCmbComBaud->AddString(L"600");
	pCmbComBaud->AddString(L"1200");
	pCmbComBaud->AddString(L"2400");
	pCmbComBaud->AddString(L"4800");
	pCmbComBaud->AddString(L"9600");
	pCmbComBaud->AddString(L"19200");
	pCmbComBaud->AddString(L"38400");
	pCmbComBaud->AddString(L"43000");
	pCmbComBaud->AddString(L"56000");
	pCmbComBaud->AddString(L"57600");
	pCmbComBaud->AddString(L"115200");
	pCmbComBaud->SetCurSel(5) ; //9600

	//奇偶校验
	pCmbComParity->ResetContent();
	pCmbComParity->AddString(L"None");
	pCmbComParity->AddString(L"Odd");
	pCmbComParity->AddString(L"Even");
	pCmbComParity->SetCurSel(0); // None

	//数据位
	pCmbComDatabits->ResetContent();
	pCmbComDatabits->AddString(L"8");
	pCmbComDatabits->AddString(L"7");
	pCmbComDatabits->AddString(L"6");
	pCmbComDatabits->SetCurSel(0); //8

	//停止位
	pCmbComStopbits->ResetContent();
	pCmbComStopbits->AddString(L"1");
	pCmbComStopbits->AddString(L"1.5");
	pCmbComStopbits->AddString(L"2");
	pCmbComStopbits->SetCurSel(0); //1

	return TRUE;
}

void CDlgParams::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTmp;
	CComboBox *pCmbComNo = (CComboBox*)GetDlgItem(IDC_CMB_COM_NO);
	CComboBox *pCmbComBaud = (CComboBox*)GetDlgItem(IDC_CMB_COM_BAUD);
	CComboBox *pCmbComParity = (CComboBox*)GetDlgItem(IDC_CMB_COM_PARITY);
	CComboBox *pCmbComDatabits = (CComboBox*)GetDlgItem(IDC_CMB_COM_DATABITS);
	CComboBox *pCmbComStopbits = (CComboBox*)GetDlgItem(IDC_CMB_COM_STOPBITS);

	pCmbComBaud->GetWindowTextW(strTmp);

	m_portNo = pCmbComNo->GetCurSel() + 1;		//串口号
	m_baud = 	_wtoi(strTmp);					//波特率
	m_parity = pCmbComParity->GetCurSel();		//奇偶校验
	pCmbComDatabits->GetWindowTextW(strTmp);
	m_databits = _wtoi(strTmp);					//数据位
	m_stopbits = pCmbComStopbits->GetCurSel();	//停止位
	
	//关闭对话框
	OnOK();
}

void CDlgParams::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
