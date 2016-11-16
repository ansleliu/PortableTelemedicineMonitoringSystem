#pragma once

class CMyPoint
{
public:
	CMyPoint(void);
	CMyPoint(const   CMyPoint&   Point); 
	~CMyPoint(void);
	CMyPoint&   operator=(const   CMyPoint&   Point); 
	void   operator+(const   CMyPoint&   Point1); 
	double   x; 
	double   y;
};
