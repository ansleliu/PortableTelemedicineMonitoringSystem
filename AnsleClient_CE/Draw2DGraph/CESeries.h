#pragma once

//CE串口通讯类
//专用于拨号连接的建立
class CCESeries
{

public:
	CCESeries();
	virtual ~CCESeries();
public:
	//打开串口
	BOOL OpenPort(CWnd* pPortOwner,			/*使用串口类，窗体句柄*/
				  UINT portNo	= 1,		/*串口号*/
				  UINT baud		= 9600,	/*波特率*/
				  UINT parity	= NOPARITY, /*奇偶校验*/
				  UINT databits	= 8,		/*数据位*/
				  UINT stopbits	= 0        /*停止位*/
				  );
	//关闭串口
	void ClosePort();
	//设置串口读取、写入超时
	BOOL SetSeriesTimeouts(COMMTIMEOUTS CommTimeOuts);
	//向串口写入数据
	BOOL WritePort(const BYTE *buf,DWORD bufLen);
	//定义串口接收数据函数类型
typedef void (CALLBACK* ONSERIESREAD)(CWnd*,BYTE* buf,int bufLen);
private:
    //串口读线程函数
    static  DWORD WINAPI ReadThreadFunc(LPVOID lparam);
	//串口写线程函数
    static DWORD WINAPI WriteThreadFunc(LPVOID lparam);

	//向串口写入数据
	static BOOL WritePort(HANDLE hComm,const BYTE *buf,DWORD bufLen);

	//关闭读线程
	void CloseReadThread();
	//关闭写线程
	void CloseWriteThread();
private:
    //已打开的串口句柄
	HANDLE	m_hComm;
	CWnd* m_pPortOwner;

	//读写线程句柄
	HANDLE m_hReadThread;
	HANDLE m_hWriteThread;

	//读写线程ID标识
	DWORD m_dwReadThreadID;
	DWORD m_dwWriteThreadID;

	//读线程退出事件
	HANDLE m_hReadCloseEvent;
	//写线程退出事件
	HANDLE m_hWriteCloseEvent;
public:
	ONSERIESREAD m_OnSeriesRead; //串口读取回调函数
};
