#ifndef __DATASTRUCT_H_
#define __DATASTRUCT_H_
#include <afxtempl.h>
#include "DataStructTemplate.h"

#define MAX_COMM_BUF_LEN	1024

typedef struct tagData
{
	int			m_n_len;
	char		m_sz_buf[MAX_COMM_BUF_LEN];
}DATA,*LPDATA;

typedef CList<LPDATA,LPDATA>DATA_LIST;

typedef CDataStructTemplate<DATA_LIST,DATA,100>DATA_GROUP;

#endif	//__DATASTRUCT_H_