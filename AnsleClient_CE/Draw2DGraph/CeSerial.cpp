#include "stdafx.h"
#include "CeSerial.h"

CCeSerial::CCeSerial()
{
	m_hComm = INVALID_HANDLE_VALUE;		/* 串口操作句柄无效 */
	hRecvThread = NULL;
	m_ExitThreadEvent = NULL;			/* 串口接收线程退出事件无效 */
}

CCeSerial::~CCeSerial()
{
	ClosePort();
	if (hRecvThread != NULL)
	{
		CloseHandle(hRecvThread);
		SetEvent(m_ExitThreadEvent);				/* 通知串口接收线程退出 */
		Sleep(200);
		CloseHandle(m_ExitThreadEvent);				/* 关闭线程退出事件 */
	}
}


/*******************************************************************************************
函数名称: CCeSerial::CommRecvTread
描    述: 串口接收线程
输入参数: LPVOID lparam: 线程参数,创建线程时传入
输出参数: 无
返    回: 0: 线程退出, 返回值没特殊含义
********************************************************************************************/
DWORD CCeSerial::CommRecvTread(LPVOID lparam)
{
	DWORD dwLength;
	BYTE *recvBuf = new BYTE[1024];
	CCeSerial *pCeSerial = (CCeSerial *)lparam;

	while(TRUE)
	{																/* 等待线程退出事件 */
		if (WaitForSingleObject(pCeSerial->m_ExitThreadEvent, 0) == WAIT_OBJECT_0)
			break;	

		if (pCeSerial->m_hComm != INVALID_HANDLE_VALUE)
		{															/* 从串口读取数据 */
			BOOL fReadState = ReadFile(pCeSerial->m_hComm, recvBuf, 1024, &dwLength, NULL);
			if(!fReadState)
			{
				//MessageBox(_T("无法从串口读取数据!"));
			}
			else
			{
				if(dwLength != 0)									/* 接收成功调用回调函数 */
					pCeSerial->OnCommRecv(pCeSerial->m_UserParam, recvBuf, dwLength);		
			}
		}
	}		

	delete[] recvBuf;
	return 0;
}



/*******************************************************************************************
函数名称: CCeSerial::OpenPort
描    述: 打开串口
输入参数: LPCTSTR Port: 串口名,如"COM0:","COM1:"
		  int BaudRate: 波特率
		  int DataBits: 数据位, 取值为7或8
		  int StopBits: 停止位
		  int Parity  : 奇偶校验位
输出参数: 无
返    回: FALSE: 失败;    TRUE: 成功
********************************************************************************************/
BOOL CCeSerial::OpenPort(LPCTSTR Port, int BaudRate, int DataBits, int StopBits, int Parity,
						 BOOL bDTRShake, BOOL bRTSShake, DWORD UserParam)
{
	COMMTIMEOUTS CommTimeOuts;

	// 打开串口
	m_hComm = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("无法打开端口或端口已打开!请检查是否已被占用."));
		return FALSE;
	}

	GetCommState(m_hComm, &dcb);						/* 读取串口的DCB */
	dcb.BaudRate = BaudRate;			
	dcb.ByteSize = DataBits;
	dcb.Parity = Parity;
	dcb.StopBits = StopBits;	
	dcb.fParity = FALSE;								/* 禁止奇偶校验 */
	dcb.fBinary = TRUE;

	if (bDTRShake == FALSE)
		dcb.fDtrControl = 0;							/* 禁止流量控制 */
	else
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;

	if (bRTSShake == FALSE)
		dcb.fRtsControl = 0;
	else
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;

	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fTXContinueOnXoff = 0;
	
	//设置状态参数
	SetCommMask(m_hComm, EV_RXCHAR);					/* 串口事件:接收到一个字符 */	
	SetupComm(m_hComm, 16384, 16384);					/* 设置接收与发送的缓冲区大小 */
	if(!SetCommState(m_hComm, &dcb))					/* 设置串口的DCB */
	{
		AfxMessageBox(_T("无法按当前参数配置端口，请检查参数!"));
		ClosePort();
		return FALSE;
	}
		
	//设置超时参数
	GetCommTimeouts(m_hComm, &CommTimeOuts);		
	CommTimeOuts.ReadIntervalTimeout = 100;				/* 接收字符间最大时间间隔 */
	CommTimeOuts.ReadTotalTimeoutMultiplier = 1;		
	CommTimeOuts.ReadTotalTimeoutConstant = 100;		/* 读数据总超时常量 */
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;		
	if(!SetCommTimeouts(m_hComm, &CommTimeOuts))
	{
		AfxMessageBox(_T("无法设置超时参数!"));
		ClosePort();
		return FALSE;
	}
		
	PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);	 /* 清除收/发缓冲区 */	
	
	m_UserParam = UserParam;							 /* 传递用户参数 */

	// 创建线程及句柄
	DWORD IDThread;

	/* 创建串口接收线程退出事件*/
	m_ExitThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		

	// 创建串口接收线程
	hRecvThread = CreateThread(0, 0, CommRecvTread, this, 0, &IDThread);
	if (hRecvThread == NULL) 
	{
		ClosePort();
		AfxMessageBox(_T("创建接收线程失败!"));
		return FALSE;
	}	
	
	return TRUE;		
}



BOOL CCeSerial::IsPortOpen(void)
{
	if(m_hComm != INVALID_HANDLE_VALUE) 
		return TRUE;

	return FALSE;
}


/*******************************************************************************************
函数名称: CCeSerial::ClosePort
描    述: 关闭串口
输入参数: 无
输出参数: 无
返    回: FALSE: 失败;    TRUE: 成功
********************************************************************************************/
BOOL CCeSerial::ClosePort(void)
{
	if(m_hComm != INVALID_HANDLE_VALUE)
	{
		SetCommMask(m_hComm, 0);		
		PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);	/* 清除收/发缓冲 */
		CloseHandle(m_hComm);								/* 关闭串口操作句柄 */
		m_hComm = INVALID_HANDLE_VALUE;
		return TRUE;
	}

	return FALSE;
}


/*******************************************************************************************
函数名称: CCeSerial::OnOpenCom
描    述: "关闭端口" 代码
输入参数: 无
输出参数: 无
返    回: 无
********************************************************************************************/
void CCeSerial::Senddata(BYTE *psendbuf, DWORD length) 
{
	DWORD dwactlen;

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("串口未打开!"));
		return;
	}

	WriteFile(m_hComm, psendbuf, length, &dwactlen, NULL);	 /* 从串口发送数据 */
}




