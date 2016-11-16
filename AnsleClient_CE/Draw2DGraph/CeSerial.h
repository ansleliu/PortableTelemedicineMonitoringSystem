#pragma once
//串口类
//用于发送短信和建立PPP/TCP连接
class CCeSerial  
{
public:
	CCeSerial();
	virtual ~CCeSerial();

	// 打开串口
	BOOL OpenPort(LPCTSTR Port, 
				  int BaudRate, 
				  int DataBits, 
				  int StopBits, 
				  int Parity,
				  BOOL bDTRShake, 
				  BOOL bRTSShake, 
				  DWORD UserParam);

	// 串口是否打开
	BOOL IsPortOpen();
	
	// 关闭串口
	BOOL ClosePort(void);
	
	// 发送数据
	void Senddata(BYTE *sendbuf, DWORD length);

	// 串口接收数据成功回调函数
	typedef void (CALLBACK *ONCOMMRECV)(DWORD UserParam, BYTE *buf, DWORD buflen);
	ONCOMMRECV OnCommRecv;

	DWORD m_UserParam;

private:

	DCB dcb;								/* 串口参数结构体 */
	HANDLE hRecvThread;						/* 接收线程句柄 */
    HANDLE m_ExitThreadEvent;				/* 串口接收线程退出事件 */

	HANDLE m_hComm;							/* 串口操作句柄 */

	// 串口接收线程
	static DWORD CommRecvTread(LPVOID lparam);
};