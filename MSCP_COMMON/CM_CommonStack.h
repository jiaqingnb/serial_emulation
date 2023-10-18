#ifndef CM_COMMONSTATCK_H  //修改飞检——不得使用保留标识符
#define CM_COMMONSTATCK_H

#if 0
#pragma once  /*修改飞检——文件中的整个代码应该受多重包含保护机制保障*/
#endif
/********************************************************
*
*  文件名   ： CM_CommonStack.h
*  版权说明 ： 北京交控科技有限公司
*  版本号   ： 1.0
*  创建时间 ： 2018.11.13
*  作者     ： 研发中心软件部
*  功能描述 ： 堆栈相关函数的源文件
*  修改记录 ：
*
********************************************************/


#include "CM_Types.h"
#include "CommonStack.h"

#ifdef __cplusplus
extern "C" {
#endif

	/*
	* 功能描述： 堆栈初始化函数，用于初始化一个堆栈，与StackFree配对使用
	*            主要为堆栈分配空间，并进行堆栈内部变量初始化
	* 参数说明： CM_CommonStack *pStack,      待初始化的的堆栈
	*            CM_UINT32 TotalSize,         堆栈大小
	* 返回值  ： 0X55，成功
	*            0xAA，失败
	*/
	ENUM_CM_RETURN CM_StackInitial(CommonStack *pStack, CM_UINT16 TotalSize);


	/******************************************************************************************
	* 功能描述      : 从堆栈中弹出栈顶元素，
	* 输入参数      :
	* 输入输出参数  : CM_CommonStack *pStack，    堆栈的指针
	* 输出参数      : STACK_DATA_TYPE* pData， 弹出的栈顶元素
	* 全局变量      :
	* 返回值        : 成功返回0x55，否则返回0xAA
	*******************************************************************************************/
	ENUM_CM_RETURN CM_StackPop(CommonStack *pStack, STACK_DATA_TYPE* pData);


	/******************************************************************************************
	* 功能描述      : 向堆栈中压入一个数据
	* 输入参数      : STACK_DATA_TYPE Data，    堆栈数据
	* 输入输出参数  : CM_CommonStack *pStack，     堆栈的指针
	* 输出参数      :
	* 全局变量      :
	* 返回值        : 成功返回0x55,失败返回0xAA
	*******************************************************************************************/
	ENUM_CM_RETURN CM_StackPush(CommonStack *pStack, STACK_DATA_TYPE Data);


	/*
	* 功能描述： 清堆栈元素函数
	* 参数说明： CM_CommonStack *pStack， 堆栈指针
	* 返回值：   0x55，成功
	*/
	ENUM_CM_RETURN CM_StackClear(CommonStack *pStack);


	/*
	* 函数描述： 堆栈存储空间释放函数
	*            在不需要某个堆栈的时候，把堆栈的数据存储空间释放掉,与StackInitial配对使用
	* 参数说明： CM_CommonStack *pStack,       堆栈指针
	* 返回值  ： 0x55，成功
	*/
	ENUM_CM_RETURN CM_StackFree(CommonStack *pStack);

	/*
	* 函数描述： 堆栈存储空间是否已满
	* 参数说明： CM_CommonStack *pStack,       堆栈指针
	* 返回值  ： 0x55，已满
	*         ： 0xAA，未满
	*/
	ENUM_CM_RETURN CM_StackIsFull(CommonStack *pStack);

	/*
	* 函数描述： 堆栈存储空间中元素个数
	* 参数说明： CM_CommonStack *pStack,       堆栈指针
	* 返回值  ： 堆栈中元素个数
	*/
	CM_UINT16 CM_StackStatus(CommonStack *pStack);

#ifdef __cplusplus
}
#endif

#endif

