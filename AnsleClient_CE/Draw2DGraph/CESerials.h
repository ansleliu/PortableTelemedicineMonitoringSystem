#pragma once
//串口类
//用于串口收发数据
class CCESerials
{
public:
	CCESerials(void);
	~CCESerials(void);
	//定义串口接收数据函数类型
public:
	//打开串口
	BOOL OpenPort(void* pOwner,/*指向父指针*/
				  UINT portNo	= 1,		/*串口号*/
				  UINT baud		= 9600,	/*波特率*/
				  UINT parity	= NOPARITY, /*奇偶校验*/
				  UINT databits	= 8,		/*数据位*/
				  UINT stopbits	= 0        /*停止位*/
				  );
	//关闭串口
	void ClosePort();
	//同步写入数据
	BOOL WriteSyncPort(const BYTE*buf , DWORD bufLen);
	//设置串口读取、写入超时
	BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);
	//得到串口是否打开
	BOOL GetComOpened();
	typedef void (CALLBACK* ONSERIALSREAD)(void * pOwner /*父对象指针*/,
										   BYTE* buf  /*接收到的缓冲区*/ ,
										   DWORD dwBufLen /*接收到的缓冲区长度*/);
private:
    //串口读线程函数
    static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);
private:
	//关闭读线程
	void CloseReadThread();
private:
    //已打开的串口句柄
	HANDLE	m_hComm;
	//读线程句柄
	HANDLE m_hReadThread;
	//读线程ID标识
	DWORD m_dwReadThreadID;
	//读线程退出事件
	HANDLE m_hReadCloseEvent;
	BOOL m_bOpened; //串口是否打开
	void * m_pOwner; //指定父对象指针
public:
	ONSERIALSREAD m_OnSerialsRead;
};
