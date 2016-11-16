#pragma once
#include "Serial.h"

class CGPRS
{
public:
	CGPRS(void);
	~CGPRS(void);
	BOOL GPRS_Init(LPCTSTR Port, 
				   int BaudRate, 
				   DWORD UserParam);
	BOOL GPRS_DeleteShortMsg(DWORD num);
	BOOL GPRS_ReadShortMessage(DWORD num, 
							   CString *strTelCode, 
							   CString *Msg);
	BOOL GPRS_SendShortMessage(CString strTelNum, CString Msg);
	BOOL GPRS_SetShortMSGCenterTel(CString strTelNum);
	BOOL GPRS_CheckMsgNum(CString *strNum, CString *strTotal);
	BOOL GPRS_SetUpAPN(void);
	BOOL GPRS_TCPInit(void);
	BOOL GPRS_TCPListen(void);
	BOOL GPRS_CheckTCPLink(void);
	CString GPRS_GetIPAddress(void);
	void GPRS_ClosePort(void);
private:
	CSerial* pCSerial;
	DWORD m_UserParam;	//使用本模块用户
	HANDLE m_ATCmdRespEvent;//AT 命令回复事件
	CString m_strRespBuf;// 回复缓冲区字符串
	DWORD m_RespCnt;
	BOOL bSendATCmd;// 是否发送了命令 
	void ResetGlobalVarial(void);
	BOOL GPRS_SendATCmd(CString strCmd);// 发送 AT 命令 
	BOOL GPRS_WriteMsg(CString Msg);// 发送短信内容 
	BOOL GPRS_WaitResponseOK(void);	//等待 AT 命令的响应
	int FindMsgPos(int *posnum, 
				   int *numlen, 
				   int *posmsg, 
				   int *msglen);
	static void CALLBACK OnSerialRead(DWORD UserParam,
									  char* buf,
									  DWORD bufLen);
};
