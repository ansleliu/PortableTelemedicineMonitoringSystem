// LedCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "Draw2DGraph.h"
#include "LedCtrl.h"
//    
//   128
//  4|2|64
//  8|_|32
//    16
const DWORD NOTCH1 = 128;	//表示第1画
const DWORD NOTCH2 = 64;	//表示第2画
const DWORD NOTCH3 = 32;	//表示第3画
const DWORD NOTCH4 = 16;	//表示第4画
const DWORD NOTCH5 = 8;		//表示第5画
const DWORD NOTCH6 = 4;		//表示第6画
const DWORD NOTCH7 = 2;		//表示第7画

const DWORD STCOUNTER0  = 252;		//代表画0时，需要笔画组合
const DWORD STCOUNTER1  = 96;		//代表画1时，需要笔画组合
const DWORD STCOUNTER2  = 218;		//代表画2时，需要笔画组合
const DWORD STCOUNTER3  = 242;		//代表画3时，需要笔画组合
const DWORD STCOUNTER4  = 102;		//代表画4时，需要笔画组合
const DWORD STCOUNTER5  = 182;		//代表画5时，需要笔画组合
const DWORD STCOUNTER6  = 190;		//代表画6时，需要笔画组合
const DWORD STCOUNTER7  = 224;		//代表画7时，需要笔画组合
const DWORD STCOUNTER8  = 254;		//代表画8时，需要笔画组合
const DWORD STCOUNTER9  = 246;		//代表画9时，需要笔画组合
const DWORD STCOUNTERALL  = 999;	//代表绘制所有

// CLedCtrl

IMPLEMENT_DYNAMIC(CLedCtrl, CStatic)

CLedCtrl::CLedCtrl()
{
	m_bDrawFadedNotches = false;
	m_crColorForeground = 0x0000FF00;
	m_crColorBackground = 0;
	m_brBackground.CreateSolidBrush(m_crColorBackground);
	m_strNumber = "0";
	m_bGotMetrics = false;
}

CLedCtrl::~CLedCtrl()
{
}


BEGIN_MESSAGE_MAP(CLedCtrl, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CLedCtrl 消息处理程序
//方法名称：绘制一个数字
//入口参数：pDC :表示内存绘图环境
//          dwChar : 数字0 － 9 某个数字的笔划组合常量
//          nCol : 表示绘制数字的位置  
void CLedCtrl::Draw(CMemDC* pDC, DWORD dwChar, int nCol)
{
	//计算点宽度和长度
	if (!m_bGotMetrics)
	{
		int nHeight = m_recClient.bottom;
		(nHeight * 0.07) < 1 ? m_nNotchWidth = 1 : m_nNotchWidth = (int)(nHeight * 0.07);
		(nHeight * 0.35) < 1 ? m_nNotchLength = 1 : m_nNotchLength = (int)(nHeight * 0.35);
		m_nNotchWidth = m_nNotchWidth;
		m_bGotMetrics = true;
	}

	//设置列宽度
	if ( nCol > 0 )		
		nCol = (nCol*m_nNotchLength) + (m_nNotchWidth*4) * nCol;

	//设置淡入淡出颜色
	COLORREF crNotchColor = m_crColorForeground;
	if (dwChar == STCOUNTERALL)	{	
		int r = GetRValue(m_crColorForeground)/3;
		int g = GetGValue(m_crColorForeground)/3;
		int b = GetBValue(m_crColorForeground)/3;
		crNotchColor = RGB(r,g,b);
	}

	//创建画笔
	CPen pen(PS_SOLID , m_nNotchWidth, crNotchColor);
	pDC->SelectObject(&pen);

	//如果有第1画，则画第1画
	if ( (dwChar & NOTCH1) || dwChar == STCOUNTERALL)	
	{	
		pDC->MoveTo( nCol + m_nNotchWidth*2, m_nNotchWidth );
		pDC->LineTo( nCol + m_nNotchLength, m_nNotchWidth );
	}

	//如果有第2画，则画第2画
	if ( dwChar & NOTCH2 || dwChar == STCOUNTERALL)	
	{	
		pDC->MoveTo(nCol + m_nNotchLength + m_nNotchWidth, m_nNotchWidth*2);
		pDC->LineTo(nCol + m_nNotchLength + m_nNotchWidth, m_nNotchLength + (m_nNotchWidth*2) );
	}
	//如果有第3画，则画第3画
	if ( dwChar & NOTCH3 || dwChar == STCOUNTERALL)	
	{
		pDC->MoveTo(nCol + m_nNotchLength + m_nNotchWidth, m_nNotchLength + (m_nNotchWidth*4) );
		pDC->LineTo(nCol + m_nNotchLength + m_nNotchWidth, m_nNotchLength*2 + (m_nNotchWidth*3) );
	}
	//如果有第4画，则画第4画
	if ( dwChar & NOTCH4 || dwChar == STCOUNTERALL)	
	{	
		pDC->MoveTo( nCol + m_nNotchWidth*2, m_nNotchLength*2 + (m_nNotchWidth*4) );
		pDC->LineTo( nCol + m_nNotchLength, m_nNotchLength*2 + (m_nNotchWidth*4) );
	}
	//如果有第5画，则画第5画
	if ( dwChar & NOTCH5 || dwChar == STCOUNTERALL)	
	{	
		pDC->MoveTo(nCol + m_nNotchWidth, m_nNotchLength + (m_nNotchWidth*4) );
		pDC->LineTo(nCol + m_nNotchWidth, m_nNotchLength*2 + (m_nNotchWidth*3) );
	}
	//如果有第6画，则画第6画
	if ( dwChar & NOTCH6 || dwChar == STCOUNTERALL)	
	{	
		pDC->MoveTo(nCol + m_nNotchWidth, m_nNotchWidth*2);
		pDC->LineTo(nCol + m_nNotchWidth, m_nNotchLength + (m_nNotchWidth*2) );
	}
	//如果有第7画，则画第7画
	if ( dwChar & NOTCH7 || dwChar == STCOUNTERALL)	
	{	
		pDC->MoveTo(nCol + m_nNotchWidth*2, m_nNotchLength + (m_nNotchWidth*3) );
		pDC->LineTo(nCol + m_nNotchWidth + m_nNotchLength - m_nNotchWidth, m_nNotchLength + (m_nNotchWidth*3) );
	}
}

//设置是否显示淡入淡出效果
void CLedCtrl::SetDrawFaded(bool bState)
{
	m_bDrawFadedNotches = bState;
}

//设置背景色
void CLedCtrl::SetColorBackGround(COLORREF crColor)
{
	if (crColor != 0xffffffff)
		m_crColorBackground = crColor;
	else 
		m_crColorBackground = ::GetSysColor(COLOR_BTNFACE);

	//重新生成背景刷
    m_brBackground.DeleteObject();
    m_brBackground.CreateSolidBrush(m_crColorBackground);

	//重画
	Invalidate();
}

//设置前景色
void CLedCtrl::SetColorForeGround(COLORREF crColor)
{
	if (crColor != 0xffffffff)
	{
		m_crColorForeground = crColor;
	}
	else 
	{
		m_crColorForeground = ::GetSysColor(COLOR_BTNTEXT);
	}

	//重画
	Invalidate(FALSE);
}
//显示数字
void CLedCtrl::Display(int nNumber)
{
	m_strNumber.Format(_T("%d"), nNumber);
	//调用OnPaint方法重画
	Invalidate(FALSE);
}


//控件绘制事件处理方法
void CLedCtrl::OnPaint() 
{
	GetClientRect(&m_recClient);

	CPaintDC dc(this);
	CMemDC memDC(&dc, m_recClient);
	CMemDC* pDC = &memDC;

	CRect clip;
	CString	 str;
	pDC->GetClipBox(&clip);
	pDC->FillSolidRect(&m_recClient, m_crColorBackground );

	for (int nCount = 0; nCount< m_strNumber.GetLength(); nCount++)
	{
		if (m_bDrawFadedNotches)
			Draw( pDC, STCOUNTERALL, nCount );	// Draw the faded bits

		str = m_strNumber[nCount];
		if ( m_strNumber[nCount] == '0' )	Draw( pDC, STCOUNTER0, nCount );
		else if ( m_strNumber[nCount] == '1' )	Draw( pDC, STCOUNTER1, nCount );
		else if ( m_strNumber[nCount] == '2' )	Draw( pDC, STCOUNTER2, nCount );
		else if ( m_strNumber[nCount] == '3' )	Draw( pDC, STCOUNTER3, nCount );
		else if ( m_strNumber[nCount] == '4' )	Draw( pDC, STCOUNTER4, nCount );
		else if ( m_strNumber[nCount] == '5' )	Draw( pDC, STCOUNTER5, nCount );
		else if ( m_strNumber[nCount] == '6' )	Draw( pDC, STCOUNTER6, nCount );
		else if ( m_strNumber[nCount] == '7' )	Draw( pDC, STCOUNTER7, nCount );
		else if ( m_strNumber[nCount] == '8' )	Draw( pDC, STCOUNTER8, nCount );
		else if ( m_strNumber[nCount] == '9' )	Draw( pDC, STCOUNTER9, nCount );
	}
}