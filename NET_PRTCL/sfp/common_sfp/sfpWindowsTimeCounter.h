
/************************************************************************
* 文件名：WindowsTimeCounter.h
* 文件描述：此文件为Windows平台下获取当前时间（以ms为单位）的头文件，
            仅协议内部调试时使用，在正式Windows平台程序中由应用提供
			获取当前时间的函数
* 创建时间：2008.07.29     
************************************************************************/

#ifndef SFPWINDOWSTIMECOUNTER_H
#define SFPWINDOWSTIMECOUNTER_H


#ifdef __cplusplus
extern "C" {
#endif

#include "CommonTypes.h"

#if CBTC_WINDOWS
#include <windows.h>
	
	LARGE_INTEGER Time_StartPoint;							/*时间的起点,记录协议开启的时间*/

/*
功能描述：获得当前的时间，距离总的起点
入口：
	LARGE_INTEGER CounterStartValue				时间起始点	
出口:
	当前时间点，单位是ms
返回值：
	当前时间点，单位是ms
*/
UINT32 GetCurTime(LARGE_INTEGER CounterStartValue);

#endif

#ifdef __cplusplus
}
#endif

#endif
