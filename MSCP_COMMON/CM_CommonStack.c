/**
@file CM_CommonStack.c
@brief 堆栈相关函数的源文件
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2018-11-13
*/


#include "CM_CommonStack.h"
#include "stdlib.h"

/*
* 功能描述： 堆栈初始化函数，用于初始化一个堆栈，与StackFree配对使用
*            主要为堆栈分配空间，并进行堆栈内部变量初始化
* 参数说明： CM_CommonStack *pStack,      待初始化的的堆栈
*            CM_UINT32 TotalSize,         堆栈大小
* 返回值  ： 0x55，成功
*            0xAA，失败
*/
ENUM_CM_RETURN CM_StackInitial(INOUT CommonStack *pStack, IN CM_UINT16 TotalSize)
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = StackInitial(pStack, TotalSize);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}

/******************************************************************************************
* 功能描述      : 从堆栈中弹出栈顶元素，
* 输入参数      :
* 输入输出参数  : CM_CommonStack *pStack，    堆栈的指针
* 输出参数      : CM_STACK_DATA_TYPE* pData， 弹出的栈顶元素
* 全局变量      :
* 返回值        : 成功返回0x55，否则返回0xAA
*******************************************************************************************/
ENUM_CM_RETURN CM_StackPop(INOUT CommonStack *pStack, OUT STACK_DATA_TYPE* pData)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = StackPop(pStack, pData);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}

/******************************************************************************************
* 功能描述      : 向堆栈中压入一个数据
* 输入参数      : CM_STACK_DATA_TYPE Data，    堆栈数据
* 输入输出参数  : CM_CommonStack *pStack，     堆栈的指针
* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回0x55,失败返回0xAA
*******************************************************************************************/
ENUM_CM_RETURN CM_StackPush(INOUT CommonStack *pStack, IN STACK_DATA_TYPE Data)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = StackPush(pStack, Data);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}


/*
* 功能描述： 清堆栈元素函数
* 参数说明： CM_CommonStack *pStack， 堆栈指针
* 返回值：   0x55，成功
*/
ENUM_CM_RETURN CM_StackClear(INOUT CommonStack *pStack)
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = StackClear(pStack);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}

/*
* 函数描述： 堆栈存储空间释放函数
*            在不需要某个堆栈的时候，把堆栈的数据存储空间释放掉,与StackInitial配对使用
* 参数说明： CM_CommonStack *pStack,       堆栈指针
* 返回值  ： 0x55，成功
*/
ENUM_CM_RETURN CM_StackFree(INOUT CommonStack *pStack)
{

	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = StackFree(pStack);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}


/*
* 函数描述： 堆栈存储空间是否已满
* 参数说明： CM_CommonStack *pStack,       堆栈指针
* 返回值  ： 0x55，已满
*         ： 0xAA，未满
*/
ENUM_CM_RETURN CM_StackIsFull(INOUT CommonStack *pStack)
{

	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = StackIsFull(pStack);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}

/*
* 函数描述： 堆栈存储空间中元素个数
* 参数说明： CM_CommonStack *pStack,       堆栈指针
* 返回值  ： 堆栈中元素个数
*/
CM_UINT16 CM_StackStatus(INOUT CommonStack *pStack)
{
	return StackStatus(pStack);
}

