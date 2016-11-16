// Draw2DGraph.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once
#include "resourceppc.h"
#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

// CDraw2DGraphApp:
// 有关此类的实现，请参阅 Draw2DGraph.cpp
//

class CDraw2DGraphApp : public CWinApp
{
public:
	CDraw2DGraphApp();
	
// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CDraw2DGraphApp theApp;
