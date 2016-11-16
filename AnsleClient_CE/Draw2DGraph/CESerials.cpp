#include "StdAfx.h"
#include "CESerials.h"

CCESerials::CCESerials(void)
{
	//初始化内部变量
	m_hComm = INVALID_HANDLE_VALUE;
	m_OnSerialsRead = NULL;
	m_bOpened = 0;
}

CCESerials::~CCESerials(void)
{
	if (m_bOpened)
	{
		//关闭串口
		ClosePort();
	}
}
//串口读线程函数
DWORD CCESerials::ReadThreadFunc(LPVOID lparam)
{
	CCESerials *ceSeries = (CCESerials*)lparam;
	
	DWORD	evtMask;
	BYTE * readBuf = NULL;//读取的字节
	DWORD actualReadLen=0;//实际读取的字节数
	DWORD willReadLen;
	
	DWORD dwReadErrors;
	COMSTAT	cmState;
	
	// 清空缓冲，并检查串口是否打开。
	ASSERT(ceSeries->m_hComm !=INVALID_HANDLE_VALUE); 
	
	
	//清空串口
	PurgeComm(ceSeries->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR );
	
	SetCommMask (ceSeries->m_hComm, EV_RXCHAR | EV_CTS | EV_DSR );
	while (TRUE)
	{   	
		if (WaitCommEvent(ceSeries->m_hComm,&evtMask,0))
		{			
			SetCommMask (ceSeries->m_hComm, EV_RXCHAR | EV_CTS | EV_DSR );
			//表示串口收到字符		
			if (evtMask & EV_RXCHAR) 
			{
				ClearCommError(ceSeries->m_hComm,&dwReadErrors,&cmState);
				willReadLen = cmState.cbInQue ;
				if (willReadLen <= 0)
				{
					continue;
				}
				
				//分配内存
				readBuf = new BYTE[willReadLen];
				ZeroMemory(readBuf,willReadLen);
				//读取串口数据
				ReadFile(ceSeries->m_hComm, readBuf, willReadLen, &actualReadLen,0);
				
				//如果读取的数据大于0，
				if (actualReadLen>0)
				{
					//触发读取回调函数
					if (ceSeries->m_OnSerialsRead)
					{
						ceSeries->m_OnSerialsRead(ceSeries->m_pOwner,readBuf,actualReadLen);
					}
				}

				//释放内存
				delete[] readBuf;
				readBuf = NULL;
			}
		}
		//如果收到读线程退出信号，则退出线程
		if (WaitForSingleObject(ceSeries->m_hReadCloseEvent,500) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

//关闭读线程
void CCESerials::CloseReadThread()
{
	SetEvent(m_hReadCloseEvent);
	//设置所有事件无效无效
	SetCommMask(m_hComm, 0);
	//清空所有将要读的数据
    PurgeComm( m_hComm,  PURGE_RXCLEAR );
    //等待4秒，如果读线程没有退出，则强制退出
    if (WaitForSingleObject(m_hReadThread,4000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hReadThread,0);
	}
	m_hReadThread = NULL;
}

/*
*函数介绍：打开串口
*入口参数：pPortOwner	:使用此串口类的窗体句柄
		   portNo		:串口号
		   baud			:波特率
		   parity		:奇偶校验
		   databits		:数据位
		   stopbits		:停止位
*出口参数：(无)
*返回值：TRUE:成功打开串口;FALSE:打开串口失败
*/
BOOL CCESerials::OpenPort(void * pOwner,
						 UINT portNo	,			/*串口号*/
						 UINT baud		,			/*波特率*/
						 UINT parity	,			/*奇偶校验*/
						 UINT databits	,			/*数据位*/
						 UINT stopbits			   /*停止位*/
						 )
{
	DCB commParam;
	TCHAR szPort[15];	

	ASSERT(pOwner!=NULL);
	m_pOwner = pOwner;
	
	// 已经打开的话，直接返回
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	
	//设置串口名
	wsprintf(szPort, L"COM%d:", portNo);
	//打开串口
	m_hComm = CreateFile(
		szPort,
		GENERIC_READ | GENERIC_WRITE,	//允许读和写
		0,								//独占方式（共享模式）
		NULL,
		OPEN_EXISTING,					//打开而不是创建（创建方式）
		0,
		NULL 
		);
	
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// 无效句柄,返回。		
		TRACE(_T("CreateFile 返回无效句柄\n"));
		return FALSE;
		
	}
	
	// 得到打开串口的当前属性参数，修改后再重新设置串口。
	if (!GetCommState(m_hComm,&commParam))
	{		
		//关闭串口
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	
	//设置串口参数
	commParam.BaudRate = baud;					// 设置波特率 
	commParam.fBinary = TRUE;					// 设置二进制模式，此处必须设置TRUE
	commParam.fParity = TRUE;					// 支持奇偶校验 
	commParam.ByteSize = databits;				// 数据位,范围:4-8 
	commParam.Parity = parity;				// 校验模式
	commParam.StopBits = stopbits;				// 停止位 
	
	commParam.fOutxCtsFlow = FALSE;				// No CTS output flow control 
	commParam.fOutxDsrFlow = FALSE;				// No DSR output flow control 
	commParam.fDtrControl = DTR_CONTROL_ENABLE; 
	// DTR flow control type 
	commParam.fDsrSensitivity = FALSE;			// DSR sensitivity 
	commParam.fTXContinueOnXoff = TRUE;			// XOFF continues Tx 
	commParam.fOutX = FALSE;					// No XON/XOFF out flow control 
	commParam.fInX = FALSE;						// No XON/XOFF in flow control 
	commParam.fErrorChar = FALSE;				// Disable error replacement 
	commParam.fNull = FALSE;					// Disable null stripping 
	commParam.fRtsControl = RTS_CONTROL_ENABLE; 
	// RTS flow control 
	commParam.fAbortOnError = FALSE;			// 当串口发生错误，并不终止串口读写
	
	//设置串口参数
	if (!SetCommState(m_hComm, &commParam))
	{
		TRACE(_T("SetCommState error"));	
		//关闭串口
		CloseHandle (m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;		
		return FALSE;
	}
	
    //设置串口读写时间
	COMMTIMEOUTS CommTimeOuts;
	GetCommTimeouts (m_hComm, &CommTimeOuts);
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;  
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;  
	CommTimeOuts.ReadTotalTimeoutConstant = 0;    
	CommTimeOuts.WriteTotalTimeoutMultiplier = 10;  
	CommTimeOuts.WriteTotalTimeoutConstant = 1000;  
	if(!SetCommTimeouts( m_hComm, &CommTimeOuts ))
	{
		TRACE( _T("SetCommTimeouts 返回错误") );
		//关闭串口
		CloseHandle (m_hComm);

		m_hComm = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	
	//指定端口监测的事件集
	SetCommMask (m_hComm, EV_RXCHAR);
	//分配串口设备缓冲区
	SetupComm(m_hComm,512,512);
	//初始化缓冲区中的信息
	PurgeComm(m_hComm,PURGE_TXCLEAR|PURGE_RXCLEAR);
	
	CString strEvent;
	strEvent.Format(L"Com_ReadCloseEvent%d",portNo);
	m_hReadCloseEvent = CreateEvent(NULL,TRUE,FALSE,strEvent);

	//创建串口读数据监听线程
	m_hReadThread = CreateThread(NULL,0,ReadThreadFunc,this,0,&m_dwReadThreadID);
	
	TRACE(_T("串口打开成功"));
	m_bOpened = TRUE;
	return TRUE;
}

/*
*函数介绍：关闭串口
*入口参数：(无)
*出口参数：(无)
*返回值：  (无)
*/
void CCESerials::ClosePort()
{	
	//表示串口还没有打开
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	
	//关闭读线程
	CloseReadThread();
	
	//关闭串口
	CloseHandle (m_hComm);
	//关闭事件
	CloseHandle(m_hReadCloseEvent);

	m_hComm = INVALID_HANDLE_VALUE;
	m_bOpened = FALSE;
}

/*
*函数介绍：往串口写入数据
*入口参数：buf ：待写入数据缓冲区
	       bufLen : 待写入缓冲区长度
*出口参数：(无)
*返回值：TRUE:设置成功;FALSE:设置失败
*/
BOOL CCESerials::WriteSyncPort(const BYTE*buf , DWORD bufLen)
{
	DWORD dwNumBytesWritten;
	DWORD dwHaveNumWritten =0 ; //已经写入多少
	
	int iInc = 0; //如果3次写入不成功，返回FALSE
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);
	do
	{
		if (WriteFile (m_hComm,					//串口句柄 
			buf+dwHaveNumWritten,				//被写数据缓冲区 
			bufLen - dwHaveNumWritten,          //被写数据缓冲区大小
			&dwNumBytesWritten,					//函数执行成功后，返回实际向串口写的个数	
			NULL))								//此处必须设置NULL
		{
			dwHaveNumWritten = dwHaveNumWritten + dwNumBytesWritten;
			//写入完成
			if (dwHaveNumWritten == bufLen)
			{
				break;
			}
			iInc++;
			if (iInc >= 3)
			{
				return FALSE;
			}
			Sleep(10);
		}
		else
		{
			return FALSE;
		}
	}while (TRUE);
	
	return TRUE;		
}

/*
*函数介绍：设置串口读取、写入超时
*入口参数：CommTimeOuts : 指向COMMTIMEOUTS结构
*出口参数：(无)
*返回值：TRUE:设置成功;FALSE:设置失败
*/
BOOL CCESerials::SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts)
{
	ASSERT(m_hComm != INVALID_HANDLE_VALUE);
	return SetCommTimeouts(m_hComm,&CommTimeOuts);
}


//得到串口是否打开
BOOL CCESerials::GetComOpened()
{
	return m_bOpened;
}