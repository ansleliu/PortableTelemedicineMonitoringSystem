#pragma once

#include "winsock.h"
#include "TCPServer_CE.h"
//#pragma comment(lib,"winsock.lib")

class CTCPCustom_CE
{
public:
	CTCPCustom_CE(void);
	~CTCPCustom_CE(void);
private:
    //通讯线程函数
	static DWORD WINAPI SocketThreadFunc(PVOID lparam);
public:
	//打开socket，创建通讯线程
	BOOL Open(CTCPServer_CE *pTCPServer);
	//关闭socket，关闭线程，释放Socket资源
	BOOL Close();
public:
	//发送数据
	BOOL SendData(const char * buf , DWORD dwBufLen);
public:
	CTCPServer_CE * m_pTCPServer_CE; //引用TCP服务端监听Socket
	CString m_RemoteHost; //远程主机IP地址
	DWORD m_RemotePort; //远程主机端口号
	SOCKET m_socket;      //通讯Socket句柄
private:
	HANDLE m_exitThreadEvent;  //通讯线程退出事件句柄
	HANDLE m_tcpThreadHandle;  //通讯线程句柄
};
