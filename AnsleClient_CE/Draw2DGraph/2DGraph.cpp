// 2DGraph.cpp : 实现文件
//

#include "stdafx.h"
#include "Draw2DGraph.h"
#include "2DGraph.h"


// C2DGraph

IMPLEMENT_DYNAMIC(C2DGraph, CWnd)

//构造函数
C2DGraph::C2DGraph()
{
	//初始化控件属性
	m_crBackColor = RGB(0,0,0);
	m_crGridColor = RGB(0,255,255);
	m_crLineColor = RGB(0,255,0);
	m_crTextColor = RGB(255,0,255);
	
	m_strXCaption = _T("时间");
	m_strYCaption = _T("E C G");
	m_dXMaxValue = X_MAX;
	m_dXMinValue = 0;
	m_dYMaxValue = Y_MAX;
	m_dYMinValue = 0; 

	m_pOldBitmapGrid = NULL;
	m_pOldBitmapLine = NULL;

}

//析构函数
C2DGraph::~C2DGraph()
{	
	//还原GDI对象
	if (m_dcGrid.GetSafeHdc() != NULL)
	{
		m_dcGrid.SelectObject(m_pOldBitmapGrid);
	}

	if (m_dcLine.GetSafeHdc() != NULL)
	{
		m_dcLine.SelectObject(m_pOldBitmapLine);
	}
}


BEGIN_MESSAGE_MAP(C2DGraph, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// C2DGraph 消息处理程序

BOOL C2DGraph::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL result ;
	//注册窗体类
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW) ;
	//创建窗体类
	result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 
		className, NULL, dwStyle, 
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID) ;

	
	return TRUE;
}


//将用户添加的点值转换成屏幕坐标
CPoint C2DGraph::GpToSp(CPoint& point)
{
	CPoint rPoint;
   	CRect rectClient;
	CRect rectLine;
	
	GetClientRect(rectClient) ;
	rectLine.left = rectClient.left + 10;
	rectLine.right = rectClient.right - 10;
	rectLine.top = rectClient.top + 10;
	rectLine.bottom = rectClient.bottom - 10;

	rPoint.x = rectLine.left + (point.x / (m_dXMaxValue - m_dXMinValue))*rectLine.Width();
	rPoint.y = rectLine.top + (1 - point.y / (m_dYMaxValue - m_dYMinValue))*rectLine.Height(); 
	return rPoint;
}

//用于刷新控件显示
void C2DGraph::InvalidateCtrl()
{
    CPen *oldPen;
	CPen solidPen(PS_SOLID,0,m_crGridColor);
	CPen newPen(PS_DOT,0,m_crGridColor);
    CFont xFont,yFont,*oldFont;
	CBrush brushBack;
	brushBack.CreateSolidBrush(m_crBackColor) ;
	
	CRect rectClient;
	GetClientRect(rectClient);
	
	CClientDC dc(this);
	
	//创建表格设备环境以及创建相应缓冲区
	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc);
		m_bitmapGrid.CreateCompatibleBitmap(&dc,rectClient.Width(),rectClient.Height());
		m_pOldBitmapGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}
	
	//设置背景颜色
	m_dcGrid.SetBkColor(m_crBackColor);
	m_dcGrid.FillRect(rectClient,&brushBack);
	
	//画边框
	oldPen = m_dcGrid.SelectObject(&solidPen);
	m_dcGrid.MoveTo(rectClient.left + 10,rectClient.top + 10);
	m_dcGrid.LineTo(rectClient.right-10,rectClient.top + 10);
	m_dcGrid.LineTo (rectClient.right-10,rectClient.bottom - 10) ;
	m_dcGrid.LineTo (rectClient.left + 10 , rectClient.bottom - 10) ;
	m_dcGrid.LineTo (rectClient.left + 10 , rectClient.top  + 10) ;
	
	//画中央分割线
	oldPen = m_dcGrid.SelectObject(&newPen);
	m_dcGrid.MoveTo(rectClient.CenterPoint().x,rectClient.bottom - 10);
	m_dcGrid.LineTo(rectClient.CenterPoint().x,rectClient.top + 10);
	m_dcGrid.MoveTo(rectClient.left + 10,rectClient.CenterPoint().y);
	m_dcGrid.LineTo(rectClient.right-10,rectClient.CenterPoint().y);

	//还原GDI对象
	m_dcGrid.SelectObject(oldPen);
	
	//创建Y轴字体
	yFont.CreateFont (8, 0, 900, 0, 300,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, _T("宋体")) ;
	
	//创建X轴字体
	xFont.CreateFont (10, 0, 0, 0, 200,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, _T("宋体")) ;
	
	//画Y坐标标题
	m_dcGrid.SetTextColor(m_crTextColor);
	oldFont = m_dcGrid.SelectObject(&yFont);
	m_dcGrid.ExtTextOut (rectClient.left, 
		(rectClient.top + rectClient.bottom ) / 2 + 5, ETO_CLIPPED,NULL,m_strYCaption,wcslen(m_strYCaption),NULL) ;
	
	//画Y坐标最大值
	CString strTmp;
	strTmp.Format(_T("%.2f"),m_dYMaxValue);
	m_dcGrid.ExtTextOut (rectClient.left  , 
		rectClient.top + 35 , ETO_CLIPPED,NULL
		,strTmp,strTmp.GetLength(),NULL) ;
	
	//画Y坐标最小值
	strTmp.Format(_T("%.2f"),m_dYMinValue);
	m_dcGrid.ExtTextOut (rectClient.left , 
		rectClient.bottom - 10 , ETO_CLIPPED,NULL
		,strTmp,strTmp.GetLength(),NULL) ;
	
	//还原GDI对象
	m_dcGrid.SelectObject(oldPen);

	//画X坐标标题
	oldFont = m_dcGrid.SelectObject(&xFont);
	m_dcGrid.ExtTextOut ((rectClient.left+ rectClient.right)/2 - 6, 
		rectClient.bottom - 9 , ETO_CLIPPED,NULL,m_strXCaption,wcslen(m_strXCaption) ,NULL) ;
	
	//画X坐标最大值
	strTmp.Format(_T("%.2f"),m_dXMaxValue);
	m_dcGrid.ExtTextOut (rectClient.right - 35, 
		rectClient.bottom - 9 , ETO_CLIPPED,NULL
		,strTmp,strTmp.GetLength(),NULL) ;
	
	//画X坐标最小值
	strTmp.Format(_T("%.2f"),m_dXMinValue);
	m_dcGrid.ExtTextOut (rectClient.left + 10, 
		rectClient.bottom - 9 , ETO_CLIPPED,NULL
		,strTmp,strTmp.GetLength(),NULL) ;
	
	//还原GDI对象
	m_dcGrid.SelectObject(oldPen);
	
	//创建画线设备环境以及创建相应缓冲区
	if (m_dcLine.GetSafeHdc() == NULL)
	{
		m_dcLine.CreateCompatibleDC(&dc) ;
		m_bitmapLine.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height()) ;
		m_pOldBitmapLine = m_dcLine.SelectObject(&m_bitmapLine) ;
	}
	
	m_dcLine.SetBkColor (m_crBackColor) ;
	m_dcLine.FillRect(rectClient, &brushBack) ;

	//删除创建的GDI对象
	solidPen.DeleteObject();
	xFont.DeleteObject();
	yFont.DeleteObject();
	brushBack.DeleteObject();
}

//绘制屏幕点
void C2DGraph::DrawPoints()
{
	CPen *oldPen;
	CRect rectCleanUp;
	CPoint points[2];
	CPoint point_old;
	CPen penLine;
	//创建画笔
	penLine.CreatePen(PS_SOLID, PIXEL, m_crLineColor) ;

	//得到绘图区域
	CRect rectClient;
	GetClientRect(rectClient);

	m_dcLine.FillSolidRect(rectClient,m_crBackColor);

	int i = 0;
	oldPen = m_dcLine.SelectObject(&penLine) ;

	if (m_dcLine.GetSafeHdc() == NULL)
	{
		return;
	}

	
	//绘制曲线
	if (m_lstPointsY.GetCount() == 0 )
	{
		
	}
	//如果是第1个点
	else if (m_lstPointsY.GetCount() == 1) 
	{
		POSITION pos = m_lstPointsY.GetHeadPosition();
		points[0].y = m_lstPointsY.GetAt(pos);
		points[0].x = 0; //表示第1个点
        points[0] = GpToSp(points[0]); //转换成屏幕坐标
		m_dcLine.SetPixel(points[0],m_crLineColor);
	}
	//如果多于或等于2个点
	else
	{
		POSITION pos = m_lstPointsY.GetHeadPosition();
		
		points[0].y = m_lstPointsY.GetNext(pos);
		points[0].x = 0; //表示第1个点
		point_old = points[0];
		i = 0;
		while (TRUE)
		{
			if (pos == NULL)
			{
				break;
			}

			points[1] = point_old;
			points[0].y = m_lstPointsY.GetNext(pos);
			points[0].x = i+1;
			point_old = points[0];
			points[0] = GpToSp(points[0]);
			points[1] = GpToSp(points[1]);
			//绘制曲线
			m_dcLine.MoveTo (points[1].x, points[1].y) ;	
			m_dcLine.LineTo (points[0].x,points[0].y) ;
			i++;
		}
	}
	
	m_dcLine.SelectObject(oldPen) ;
	//删除画笔GDI对象
	penLine.DeleteObject();
}

//覆盖WM_PAINT消息方法
void C2DGraph::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectClient;
	GetClientRect(rectClient) ;
	
	CDC memDC ;
	CBitmap memBitmap ;
	CBitmap* oldBitmap ; 
	
	memDC.CreateCompatibleDC(&dc) ;
	memBitmap.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height()) ;
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;
	
	//更新背景显示
	InvalidateCtrl() ;
	//画点
	DrawPoints();
	//将m_dcGrid和m_dcLine绘制到控件上
	if (memDC.GetSafeHdc() != NULL)
	{

		memDC.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), 
			         &m_dcGrid, 0, 0, SRCCOPY) ;
		memDC.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), 
			         &m_dcLine, 0, 0, SRCPAINT) ;
		
		dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), 
			&memDC, 0, 0, SRCCOPY) ;
	}
	
	memDC.SelectObject(oldBitmap) ;		
	
	//删除内存位图GDI对象
	memBitmap.DeleteObject();
	//删除内存绘图环境
	memDC.DeleteDC();

}

//在曲线末尾添加一个点
void C2DGraph::AppendPoint(DWORD dwPointY)
{
	//在队列中添加1个点
    m_lstPointsY.AddTail(dwPointY);	
	//更新显示
	Invalidate();
}

//删除曲线第一点
void C2DGraph::DeleteFirstPoint()
{
	//删除1个节点
	if (m_lstPointsY.GetCount() > 0)
	{
		m_lstPointsY.RemoveHead();
	}
	//更新显示
	Invalidate();		
}

