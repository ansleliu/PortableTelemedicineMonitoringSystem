#pragma once

class CUtility  
{
public:
	CUtility();
	virtual ~CUtility();

	// 数据转化
	static void UnsignedShortToCharp(unsigned short *srcstr,char *deststr);
	static void CharpToUnsignedShort(const char *srcstr,unsigned short *deststr);
};
