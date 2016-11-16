#pragma once
#define DATA_READ_TIMEOUT	      100   // Milliseconds
#define STATUS_CHECK_TIMEOUT      100   // Milliseconds

typedef void(CALLBACK* ONSERIESREAD)(DWORD UserParam,
									 char* buf,
									 DWORD dwBufLen);
class CSerial
{
public:
	CSerial(void);
	~CSerial(void);
	public:	
	bool SendData(char szData[],int len);
	bool ReadData(char szData[],int&len);
	bool	m_bOpened;
	BOOL Close();
	BOOL	Open(DWORD UserParam,LPCTSTR Port, DWORD dwBaudRate,
					BYTE byByteSize,BYTE byParity,BYTE byStopBits);
	ONSERIESREAD m_OnSeriesRead;
private:
	HANDLE	m_hCommPort;

	HANDLE  m_start_data_thread;
	HANDLE  m_start_analysis_thread;
	HANDLE  m_start_event_thread;

	HANDLE  m_end_event_thread;
	HANDLE  m_end_analysis_thread;
	HANDLE  m_end_data_thread;

	HANDLE  m_event_thread_end;
	HANDLE  m_analysis_thread_end;
	HANDLE  m_data_thread_end;

	HANDLE  m_data_come_event;
	HANDLE  m_event_come_event;

	HANDLE  m_end_send_thread;
	HANDLE  m_send_thread_end;
	HANDLE  m_send_data_event;
	DWORD   m_UserParam;
private:
	static	UINT SendDataProc(LPVOID lpParam);
	void	ClearSysReadBufferContent();
	void	Init();
	void	UnInit();
	void	CloseAndCleanHandle(HANDLE&handle);
	static	UINT ReadEventProc(LPVOID lpParam);
	static	UINT AnalysisEventProc(LPVOID lpParam);
	static	UINT ReadDataProc(LPVOID lpParam);
};
