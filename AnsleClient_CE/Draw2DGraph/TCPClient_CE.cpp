#include "StdAfx.h"
#include "TCPClient_CE.h"

//构造函数
CTCPClient_CE::CTCPClient_CE()
{
	//初始化socket环境
	WSADATA wsd;
	WSAStartup(MAKEWORD(2,2),&wsd);
	//置空缓冲区
	ZeroMemory(m_recvBuf,4096);
	OnDisConnect = NULL;	//连接断开事件，回调函数
	OnRead = NULL;	//接收数据事件，回调函数
	OnError = NULL;	//发生错误事件，回调函数
	//创建线程退出事件句柄
	m_exitThreadEvent = CreateEvent(NULL,FALSE,FALSE,L"EVENT_TCP_CLIENT_THREAD");
}

//析构函数
CTCPClient_CE::~CTCPClient_CE()
{
	//关闭线程退出事件句柄
	CloseHandle(m_exitThreadEvent);
	//释放socket资源
	WSACleanup();
}


/*------------------------------------------------------------------
【函数介绍】:  此线程用于监听TCP客户端通讯的事件，例如当接收到数据、
连接断开和通讯过程发生错误等事件
【入口参数】:  lparam:无类型指针，可以通过此参数，向线程中传入需要用到的资源。
在这里我们将CTCPClient_CE类实例指针传进来
【出口参数】:  (无)
【返回  值】:  返回值没有特别的意义，在此我们将返回值设为0。
------------------------------------------------------------------*/
DWORD CTCPClient_CE::SocketThreadFunc(LPVOID lparam)
{
	CTCPClient_CE *pSocket;
	//得到CTCPClient_CE实例指针
	pSocket = (CTCPClient_CE*)lparam;
	//定义读事件集合
	fd_set fdRead;
	int ret;
	//定义事件等待时间
	TIMEVAL	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;
	while (TRUE)
	{
		//收到退出事件，结束线程
		if (WaitForSingleObject(pSocket->m_exitThreadEvent,0) == WAIT_OBJECT_0)
		{
			break;
		}
		//置空fdRead事件为空
		FD_ZERO(&fdRead);
		//给客户端socket设置读事件
		FD_SET(pSocket->m_socket,&fdRead);
		//调用select函数，判断是否有读事件发生
		ret = select(0,&fdRead,NULL,NULL,&aTime);

		if (ret == SOCKET_ERROR)
		{
			if (pSocket->OnError)
			{
				//触发错误事件
				pSocket->OnError(pSocket->m_pOwner,1);
			}
			if (pSocket->OnDisConnect)
			{
				//触发连接断开事件
				pSocket->OnDisConnect(pSocket->m_pOwner);
			}
			//关闭客户端socket
			closesocket(pSocket->m_socket);
			break;
		}

		if (ret > 0)
		{
			//发生读事件
			if (FD_ISSET(pSocket->m_socket,&fdRead))
			{
				ZeroMemory(pSocket->m_recvBuf,4096);
				//接收数据
				int recvLen = recv(pSocket->m_socket,pSocket->m_recvBuf, 4096,0); 
				if (recvLen == SOCKET_ERROR)
				{
					int iError = WSAGetLastError();
					if (pSocket->OnError)
					{
						//触发socket错误事件
						pSocket->OnError(pSocket->m_pOwner,iError);
					}
					if (pSocket->OnDisConnect)
					{
						//触发与服务器断开事件
						pSocket->OnDisConnect(pSocket->m_pOwner);
					}
					//关闭客户端socket
					closesocket(pSocket->m_socket);
					break;
				}
				else if (recvLen == 0)
				{
					if (pSocket->OnDisConnect)
					{
						//触发与服务器端断开事件
						pSocket->OnDisConnect(pSocket->m_pOwner);
					}
					//关闭客户端socket
					closesocket(pSocket->m_socket);
					break;
				}
				else
				{
					if (pSocket->OnRead)
					{
						//触发数据接收事件
						pSocket->OnRead(pSocket->m_pOwner,pSocket->m_recvBuf,recvLen);
					}
				}
			}
		}
	}
	TRACE(L"客户端线程退出\n");
	return 0;
}

/*-----------------------------------------------------------------
【函数介绍】: 用于打开客户端socket
【入口参数】: pOwner 用于指定父对象
【出口参数】: (无)
【返回  值】: TRUE:打开成功;FALSE:打开失败
-----------------------------------------------------------------*/
BOOL CTCPClient_CE::Open(void * pOwner)
{
	//复位线程退出事件
	ResetEvent(m_exitThreadEvent);
	//存储父对象指针
	m_pOwner = pOwner;
	//创建TCP套接字 
	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_socket == SOCKET_ERROR)
	{
		return FALSE;
	}
	//创建通讯线程
	m_tcpThreadHandle = CreateThread(NULL,0,SocketThreadFunc,this,0,NULL);
	if (m_tcpThreadHandle == NULL)
	{
		closesocket(m_socket);
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------
【函数介绍】: 用于关闭客户端socket
【入口参数】:  (无)
【出口参数】:  (无)
【返回  值】: TRUE:关闭成功;FALSE:关闭失败
-----------------------------------------------------------------*/
BOOL CTCPClient_CE::Close()
{
	//发送通讯线程结束事件
	SetEvent(m_exitThreadEvent);

	//等待1秒，如果读线程没有退出，则强制退出
	if (WaitForSingleObject(m_tcpThreadHandle,1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_tcpThreadHandle,0);
		TRACE(L"强制终止客户端线程\n");
	}
	m_tcpThreadHandle = NULL;
	//关闭Socket，释放资源
	int err = closesocket(m_socket);
	if (err == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------
【函数介绍】: 用于建立与TCP服务器连接
【入口参数】: (无)
【出口参数】: (无)
【返回  值】: TRUE:建立连接成功;FALSE:建立连接失败
------------------------------------------------------------------*/
BOOL CTCPClient_CE::Connect()
{
	struct sockaddr_in addr;
	int err;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	//此处要将双字节转换成单字节
	char ansiRemoteHost[255];
	ZeroMemory(ansiRemoteHost,255);
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,m_remoteHost,wcslen(m_remoteHost)
		,ansiRemoteHost,wcslen(m_remoteHost),NULL,NULL);

	addr.sin_addr.s_addr=inet_addr(ansiRemoteHost);
	//此时采用同步连接方式,connect直接返回成功或是失败
	err = connect(m_socket,(struct sockaddr *)&addr,sizeof(addr));
	if (err == SOCKET_ERROR) 
	{
		return FALSE;
	}
	//设置通讯模式为异步模式
	DWORD ul= 1;
	ioctlsocket(m_socket,FIONBIO,&ul);
	return TRUE;
}

/*-----------------------------------------------------------------
【函数介绍】: 向服务器端发送数据
【入口参数】: buf:待发送的数据
dwBufLen:待发送的数据长度
【出口参数】: (无)
【返回  值】: TRUE:发送数据成功;FALSE:发送数据失败
------------------------------------------------------------------*/
BOOL CTCPClient_CE::SendData(const char * buf , DWORD dwBufLen)
{
	int nBytes = 0;
	UINT nSendBytes=0;

	while (nSendBytes < dwBufLen)
	{
		nBytes = send(m_socket,buf+nSendBytes,dwBufLen-nSendBytes,0);
		if (nBytes==SOCKET_ERROR )
		{
			int iErrorCode = WSAGetLastError();
			//触发socket的Error事件
			if (OnError)
			{
				OnError(m_pOwner,iErrorCode);
			}
			//触发与服务器端断开连接事件
			if (OnDisConnect)
			{
				OnDisConnect(m_pOwner);
			}
			//关闭socket
			Close();
			return FALSE;
		}
		
		//累计发送的字节数
		nSendBytes = nSendBytes + nBytes;

		if (nSendBytes < dwBufLen)
		{
			Sleep(1000);
		}
	} 
	return TRUE; 
}

