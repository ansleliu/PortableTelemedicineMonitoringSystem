#pragma once


// CLedCtrl
class CMemDC : public CDC {
private:
	CBitmap m_bitmap; //离屏位图
	CBitmap* m_oldBitmap; //存储旧位图
	CDC* m_pDC; //存储绘图环境
	CRect m_rect; //存储控件矩形区域
	BOOL m_bMemDC; //判断绘图环境是否准备好
public:
	//构造函数
	CMemDC(CDC* pDC, CRect rect = CRect(0,0,0,0)) : CDC(), m_oldBitmap(NULL), m_pDC(pDC)
	{
		//判断绘图是否为空
		ASSERT(m_pDC != NULL); 
		//判断绘图环境是否准备好
		m_bMemDC = !pDC->IsPrinting();
		if (m_bMemDC)
		{
			//创建离屏绘制环境
			CreateCompatibleDC(pDC);
			if ( rect == CRect(0,0,0,0) )
				pDC->GetClipBox(&m_rect);
			else
				m_rect = rect;

			//创建离屏位图
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);
		} else 
		{
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}
	
	//析构函数
	~CMemDC()
	{
		if (m_bMemDC) {
			//将离屏位图绘制屏幕上
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);
			//将旧位图重新选择到绘图环境
			SelectObject(m_oldBitmap);
		} else {
			m_hDC = m_hAttribDC = NULL;
		}
	}
	
	//指针操作符操作
	CMemDC* operator->() {return this;}
	
	//指针操作符操作
	operator CMemDC*() {return this;}
};


class CLedCtrl : public CStatic
{
	DECLARE_DYNAMIC(CLedCtrl)

public:
	CLedCtrl();
	virtual ~CLedCtrl();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	//存储是否绘制淡入淡出效果开关变量
	bool m_bDrawFadedNotches;
	//表示是否已经设置换算关系
	bool m_bGotMetrics;

	//存储控件客户区域
	RECT m_recClient;

	//设置点宽度
	int m_nNotchWidth;
	//设置点长度
	int m_nNotchLength;

	//存储显示的数字
	CString m_strNumber;
    
	//控件背景色
	COLORREF m_crColorBackground;
	//控件前景色，也就是数字颜色
	COLORREF m_crColorForeground;
    
	//背景画刷
	CBrush m_brBackground;
 protected:
	//绘制一个数字
	void Draw(CMemDC* pDC, DWORD dwChar, int nCol);
public:
	//设置是否显示淡入淡出效果
	void SetDrawFaded(bool bState);
	//设置背景色
	void SetColorBackGround(COLORREF crColor);
	//设置前景色
	void SetColorForeGround(COLORREF crColor);
public:
	//显示数字
	void Display(int nNumber);

	afx_msg void OnPaint();
};


