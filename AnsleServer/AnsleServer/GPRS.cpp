#include "StdAfx.h"
#include "GPRS.h"

CGPRS::CGPRS(void)
{
	ResetGlobalVarial();
}

CGPRS::~CGPRS(void)
{
	if (pCSerial->m_bOpened== TRUE)
	{
		pCSerial->Close();
	}

	if (pCSerial)
		delete pCSerial;
}
void CGPRS::GPRS_ClosePort()
{
	if (pCSerial->m_bOpened == TRUE)
	{
		pCSerial->Close();
	}

	if (pCSerial)
		delete pCSerial;
}
void CGPRS::ResetGlobalVarial(void)
{
	bSendATCmd = FALSE;					/* 当前不处于发送命令状态 */	
	m_RespCnt = 0;							
	m_strRespBuf = _T("");
	ResetEvent(m_ATCmdRespEvent);		/* 复位 GPRS 响应事件*/
}
void CALLBACK CGPRS::OnSerialRead(DWORD UserParamr,char* buf,DWORD bufLen)
{
	CGPRS *pGPRS = (CGPRS *)UserParamr;
	CString strTmp = _T("");
	strTmp = CString(buf);
	if (pGPRS->bSendATCmd == TRUE)
	{//之前用户发送了 AT 指令 
		pGPRS->m_strRespBuf.Insert(pGPRS->m_RespCnt, strTmp);			// 保存数据 
		SetEvent(pGPRS->m_ATCmdRespEvent);
		return;
	}
	pGPRS->bSendATCmd = FALSE;							//复位相关全局变量
	pGPRS->m_RespCnt = 0;							
	pGPRS->m_strRespBuf = _T("");
	ResetEvent(pGPRS->m_ATCmdRespEvent);
}


BOOL CGPRS::GPRS_Init(LPCTSTR Port, int BaudRate, DWORD UserParam)
{
	BOOL ret;
	DWORD Trycount = 2;

	do
	{	// 尝试初始化 GPRS, 最多尝试两次
	     Trycount--;

		pCSerial = new CSerial;
		ret = pCSerial->Open((DWORD)this,Port,BaudRate,8,0,1);
		if (ret == FALSE)	//打开串口, 数据位为8,停止位为1,无校验位
		{
			delete pCSerial;
			return FALSE;
		}
		pCSerial->m_OnSeriesRead = OnSerialRead;//串口接收成功回调函数 
		m_UserParam = UserParam;//保存用户传进来的参数 
		m_ATCmdRespEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//创建 GPRS 响应事件
		for(int i=0;i<=1;i++)
		{
			GPRS_SendATCmd(_T("AT"));//尝试模块是否处于激活状态 
			GPRS_WaitResponseOK();
		}
		GPRS_SendATCmd(_T("AT+IPR=115200"));							
		ret = GPRS_WaitResponseOK();
		if (ret == FALSE)
		{
//			AfxMessageBox(_T("AT+IPR=115200ERROR"));
			delete pCSerial;
			if (Trycount == 0) 
				return FALSE;
		}
		else
			Trycount = 0;
	}while(Trycount > 0);
	Sleep(100);
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CREG=1"));	
		ret = GPRS_WaitResponseOK();
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CREG=1ERROR"));
		return FALSE;
	}
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CLIP=1"));// 设置来电显示 
		ret = GPRS_WaitResponseOK();		
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CLIP=1ERROR"));
		return FALSE;
	}
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CMGF=1"));//设置为文本模式 
		ret = GPRS_WaitResponseOK();
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CMGF=1ERROR"));
		return FALSE;
	}	
	for(int i=0;i<=1;i++)
	{
		GPRS_SendATCmd(_T("AT+CSCS=\"GSM\""));//设置字符集 
		ret = GPRS_WaitResponseOK();
	}
	if (ret == FALSE)
	{
//		AfxMessageBox(_T("AT+CSCS=\"GSM\"ERROR"));
		return FALSE;
	}
	return TRUE;
}
BOOL CGPRS::GPRS_SendATCmd(CString strCmd)
{
	int len = strCmd.GetLength();
	char *psendbuf = new char[len + 2];
	
	m_RespCnt = 0;
	bSendATCmd = TRUE;	//进入发送命令状态 

	for(int i = 0; i < len;i++)
		psendbuf[i] = (char)strCmd.GetAt(i);//转换为单字节字符 

	psendbuf[len] = '\r';	//加入结束符 
	psendbuf[len + 1] = '\n';
	//从串口发送数据
	pCSerial->SendData(psendbuf,len + 2);
	delete[] psendbuf;
	return TRUE;
}
BOOL CGPRS::GPRS_WaitResponseOK(void)
{
	DWORD ret;
	BOOL bOK = TRUE;

	while(1)
	{	// 等待** 秒, 如果得不到回复, 即为错误
		ret = WaitForSingleObject(m_ATCmdRespEvent, 1000);
		if (ret == WAIT_OBJECT_0)
		{
			if ((m_strRespBuf.Find(_T("ERROR"), 0) >= 0) ||
				(m_strRespBuf.Find(_T("error"), 0) >= 0))
			{	//响应中有"ERROR" 
//				AfxMessageBox(_T("ERROR"));
				bOK = FALSE;
				break;
			}
			if ((m_strRespBuf.Find(_T("OK\r\n"), 0) >= 0) ||
				(m_strRespBuf.Find(_T("ok\r\n"), 0) >= 0))
			{	//响应中有"OK"
//				AfxMessageBox(_T("OK"));
				bOK = TRUE;
				break;
			}

			ResetEvent(m_ATCmdRespEvent);
		}
		else
		{
//			AfxMessageBox(_T("NODATA"));
			bOK = FALSE;//响应超时 
			break;
		}
	}

	ResetGlobalVarial();//复位用到的全局变量
	if (FALSE == bOK)
		return FALSE;

	return TRUE;
}
BOOL CGPRS::GPRS_WriteMsg(CString Msg)
{
	int len = Msg.GetLength();
	char *psendbuf = new char[len + 3];

	for(int i = 0; i < len;i++)
		psendbuf[i] = (char)Msg.GetAt(i);	// 转换为单字节字符

	psendbuf[len] = '\r';	// 加入结束符 
	psendbuf[len + 1] = '\n';
	psendbuf[len + 2] = 0x1A;
	//从串口发送数据 
	pCSerial->SendData(psendbuf,len + 3);
	delete[] psendbuf;
	return TRUE;
}
BOOL CGPRS::GPRS_SetShortMSGCenterTel(CString strTelNum)
{
	CString strTmp;

	strTmp = strTelNum + _T(",145");
	bSendATCmd = TRUE;
	GPRS_SendATCmd(strTelNum);

	return (GPRS_WaitResponseOK());
}
int CGPRS::FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen)
{
	// 短信开始位置
	int posStart = m_strRespBuf.Find(_T("+CMGR:"), 0);					
	if (posStart < 0)
		return -1;
	// 电话号码开始位置
	*posnum = m_strRespBuf.Find(_T("\"REC READ\",\""), posStart);	
	if (*posnum < 0)
		return -1;
	*posnum = *posnum + wcslen(L"\"REC READ\",\"");			
	// 电话号码结束位置
	int posnumend = m_strRespBuf.Find(_T("\""), *posnum + 1);
	if (posnumend < 0)
		return -1;
	// 电话号码长度
	*numlen = posnumend - *posnum;					
	// 短信开始位置
	*posmsg = m_strRespBuf.Find(_T("\r\n"), posStart);				
	if (*posmsg < 0)
		return -1;
	*posmsg = *posmsg + wcslen(L"\r\n");		
	// 短信结束位置
	int posmsgend = m_strRespBuf.Find(_T("\r\n\r\nOK"), *posmsg);
	*msglen = posmsgend - *posmsg;	
	return 1;
}
BOOL CGPRS::GPRS_ReadShortMessage(DWORD num, CString *strTelCode, CString *Msg)
{
	CString strReadNum;
	BOOL ret, retvalue;
	strReadNum.Format(_T("AT+CMGR=%d"), num);
	bSendATCmd = TRUE;
	GPRS_SendATCmd(strReadNum);	//命令: 读第 num 条短信
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)
		{
			if (m_strRespBuf.Find(_T("ERROR"), 0) >= 0)	//响应为 ERROR
			{
				retvalue = FALSE;
				break;
			}
			
			int posnum, numlen, posmsg, msglen;	
			int pos = FindMsgPos(&posnum, &numlen, &posmsg, &msglen);//寻找短信位置
			if (pos > 0)
			{
				*strTelCode = m_strRespBuf.Mid(posnum, numlen);
				*Msg = m_strRespBuf.Mid(posmsg, msglen);// 取出短信内容
				retvalue = TRUE;
				break;
			}		
			ResetEvent(m_ATCmdRespEvent);		
		}//end of if (ret == WAIT_OBJECT_0)
		else
		{
			retvalue = FALSE;
			break;
		}
	}
	ResetGlobalVarial();
	if (retvalue == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_SendShortMessage(CString strTelNum, CString Msg)
{
	BOOL ret, retvalue;
	int pos;
	/* 
	*	1. 发送电话号码
	*/
	CString strNum;
	strNum.Format(_T("AT+CMGS=\"%s\""), strTelNum);
	bSendATCmd = TRUE;
	GPRS_SendATCmd(strNum);	//发送电话号码
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)	
		{	
			pos = m_strRespBuf.Find(_T('>'), 0);
			if (pos >=0)
			{	//收到提示符, 现在可以往模块写入短信内容了
				retvalue = TRUE;
				break;
			}
			pos = m_strRespBuf.Find(_T("ERROR"), 0);
			if (pos >=0)
			{
				retvalue = FALSE;//GPRS 响应字符中包含有"ERROR"
				break;
			}
		}
		else
		{
			retvalue = FALSE;
			break;
		}
	}
	ResetGlobalVarial();
	if (retvalue == FALSE) return FALSE;

	/* 
	*	2. 等待 '>' 提示符, 发送短信内容
	*/
	bSendATCmd = TRUE;
	GPRS_WriteMsg(Msg);	
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent,1000);
		if (ret == WAIT_OBJECT_0)	
		{
			int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
			if (pos > 0)
			{
				retvalue = FALSE;
				break;
			}
			pos = m_strRespBuf.Find(_T("CMGS"), 0);
			if (pos > 0)
			{	//响应正确, 发送成功 
				retvalue = TRUE;
				break;
			}
		}
	}	
	ResetGlobalVarial();
	if (retvalue == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CheckMsgNum(CString str, CString *strNum, CString *strTotal)
{
	int pos1 = str.Find(_T("\"SM\","), 0);			/* 标识字符 */
	if (pos1 < 0)
		return FALSE;

	int pos2 = str.Find(_T(','), pos1 + 5);			/* 分隔逗号 */
	if (pos2 < 0)
		return FALSE;

	*strNum = str.Mid(pos1 + 5, pos2 - (pos1 + 5));	/* 现有短信数量 */

	int pos3 = str.Find(_T(","), pos2 + 1);
	if (pos3 < 0)
		return FALSE;

	*strTotal = str.Mid(pos2 + 1, pos3 - pos2 - 1); /* 可纳容的最大短信数量 */

	return TRUE;
}

BOOL CGPRS::GPRS_CheckMsgNum(CString *strNum, CString *strTotal)
{	
	BOOL ret, retvalue;
	CString strTmp;

	bSendATCmd = TRUE;
	GPRS_SendATCmd(_T("AT+CPMS?"));//查询命令
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 1000);
		if (ret == WAIT_OBJECT_0)	
		{
			int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
			if (pos > 0)
			{
				retvalue = FALSE;
				break;
			}

			if (CheckMsgNum(m_strRespBuf, strNum, strTotal) == TRUE)
			{	// 查询成功 
				retvalue = TRUE;
				break;
			}
			ResetEvent(m_ATCmdRespEvent);
		}
		else
		{
			retvalue = FALSE;
			break;			
		}
	}
	ResetGlobalVarial();
	if (retvalue == FALSE) 
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_DeleteShortMsg(DWORD num)
{
	CString strCmd;
	BOOL ret;
	strCmd.Format(_T("AT+CMGD=%d"), num);
	GPRS_SendATCmd(strCmd);
	ret = GPRS_WaitResponseOK();
	if (ret == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_SetUpAPN(void)
{
	CString strCmd = _T("AT+CGDCONT=1,\"IP\",\"CMNET\"");
	BOOL ret;
	GPRS_SendATCmd(strCmd);
	ret = GPRS_WaitResponseOK();
	if (ret == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_TCPInit(void)
{
	CString strCmd = _T("AT%ETCPIP");
	BOOL ret;
	GPRS_SendATCmd(strCmd);
	ret = GPRS_WaitResponseOK();
	if (ret == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_TCPListen(void)
{
	CString strCmd = _T("AT%IPLISTEN=\"TCP\",5000");
	BOOL ret;
	GPRS_SendATCmd(strCmd);
	ret = GPRS_WaitResponseOK();
	if (ret == FALSE)
		return FALSE;
	return TRUE;
}
BOOL CGPRS::GPRS_CheckTCPLink(void)
{
	CString strCmd = _T("AT%IPOPEN?");
	BOOL ret;
	GPRS_SendATCmd(strCmd);
	ret = GPRS_WaitResponseOK();
	if (ret == FALSE)
		return FALSE;
	return TRUE;
}
CString CGPRS::GPRS_GetIPAddress(void)
{
	CString strCmd = _T("AT%ETCPIP?");
	CString nationalIP;
	DWORD ret,retvalue;
	GPRS_SendATCmd(strCmd);
	while(1)
	{
		ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
		if (ret == WAIT_OBJECT_0)
		{
			if (m_strRespBuf.Find(_T("ERROR\r\n"), 0) > 0)	// 响应为 ERROR 
			{
				retvalue = FALSE;
				break;
			}
			else
			{
				int pos1;
				pos1 = m_strRespBuf.Find(_T("\""), 0);
				int pos2;
				pos2 = m_strRespBuf.Find(_T("\",,"), pos1+1);
				nationalIP = m_strRespBuf.Mid(pos1+1,pos2-pos1-1);
				retvalue = TRUE;
				break;
			}
			ResetEvent(m_ATCmdRespEvent);
			
		}
		else
		{
			retvalue = FALSE;
			break;
		}
	}

	ResetGlobalVarial();
	if (retvalue == FALSE)
		return _T("IP地址未成功获得");

	return nationalIP;
}