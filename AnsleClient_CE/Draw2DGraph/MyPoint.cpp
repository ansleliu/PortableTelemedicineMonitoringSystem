#include "StdAfx.h"
#include "MyPoint.h"

CMyPoint::CMyPoint(void)
{
}

CMyPoint::~CMyPoint(void)
{
}

CMyPoint::CMyPoint(const   CMyPoint&   Point) 
{ 
	this-> x=Point.x; 
	this-> y=Point.y; 
} 
CMyPoint&   CMyPoint::operator=(const   CMyPoint&   Point) 
{ 
	this-> x=Point.x; 
	this-> y=Point.y; 
	return   *this; 
} 

void   CMyPoint::operator+(const   CMyPoint&   Point1) 
{ 
	x=x+Point1.x; 
	y=y+Point1.y; 
}