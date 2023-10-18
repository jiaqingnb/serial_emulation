/**
@file CM_Queue.c
@brief 队列函数实现
1.本队列适用的应用方式有(单任务应用），（两任务并行），（一个应用+一个中断）。
2 本队列支持的两任务或者中断函数必须是一读一写，不支持两个任务同时读或者两个任务同时写
3 申请足够空间防止队列写满。
4 请不要输入 0 qsize。
5 如果多任务或者中断情况下，如果空间小于两帧数据的容量，会发生死锁：数据长度不够读，剩余空间又不够写，发生死锁
6 本队列最大支持4g的数据，但是使用者应根据平台情况酌情申请空间。
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#include "CM_Queue.h"


/*
* 功能描述： 队列初始化函数，用于初始化一个队列，与CM_QueueElementFree配对使用
*            为队列结构体中的q分配空间，并将队列的size初始化
* 参数说明： STRU_CM_QUEUE *pQueue,    待分配空间的队列
*            CM_UINT32 QSize,           队列需要的长度
* 返回值  ： 0x55，成功
*            0xAA，失败
*/
ENUM_CM_RETURN CM_QueueInitial(STRU_CM_QUEUE *pQueue, CM_UINT32 QSize)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == QueueInitial(pQueue,QSize))
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
* 功能描述： 队列初始化函数，用于初始化一个队列
*            QSize初始化，与CM_QueueInitial相比，采用传入的空间作为队列元素的空间。
* 参数说明： STRU_CM_QUEUE *pQueue,               待分配空间的队列
*            CM_UINT32 QSize,                      队列需要的长度
*            CM_QueueElement pQueueElementSpace[], 队列需要的内存空间
* 返 回 值： 0x55，成功
*            0xAA，失败
* 使用注意： 1，确认pQueueElementSpace数组的大小大于等于QSize，以免异常。
*            2，如果使用这个函数初始化，则不再调用CM_QueueElementFree进行空间释放。
*/
ENUM_CM_RETURN CM_StaticQueueInitial(STRU_CM_QUEUE *pQueue, CM_UINT32 QSize,CM_QueueElement pQueueElementSpace[])
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == StaticQueueInitial(pQueue, QSize, pQueueElementSpace))
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
* 功能描述： 队列扫描函数，可以读出队列中的数据，但是对队列的读写指针都不产生影响，即读到的数据还在队列中
* 参数说明： CM_UINT32 DataLen,              要读出的数据长度;
*            CM_QueueElement *pData,         读出的数据要存放的数组;
*            STRU_CM_QUEUE *pQueue,         要读出数据的队列
* 返回值：   0x55，成功
*            0xAA，失败，队列数据长度不够
*/
ENUM_CM_RETURN CM_QueueScan(CM_UINT32 DataLen,CM_QueueElement *pData,const STRU_CM_QUEUE *pQueue)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == QueueScan( DataLen, pData, pQueue))
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
* 功能描述： 读队列函数，可以读出队列中的数据,并存放在一个数组中，数据读出后读指针发生变化，即读出的数据已不存在队列中
* 参数说明： CM_UINT32  DataLen,                 要读出的数据长度;
*            CM_QueueElement     *pData,         读出的数据要存放的数组;
*            STRU_CM_QUEUE *pQueue,             要读出数据的队列
* 返回值 ：  1，成功
*            0，失败，队列数据长度不够
*/
ENUM_CM_RETURN CM_QueueRead(CM_UINT32 DataLen, CM_QueueElement *pData,STRU_CM_QUEUE *pQueue)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == QueueRead(DataLen, pData, pQueue))
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
* 功能描述： 清队列函数，把队列中的in，out值置0
* 参数说明： STRU_CM_QUEUE *pQueue， 需要清的队列指针
* 返回值：   0x55，成功;0xAA，失败
*/
ENUM_CM_RETURN CM_QueueClear(STRU_CM_QUEUE *pQueue)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == QueueClear(pQueue))
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
*    功能描述： 读出队列中可用数据长度
*    参数说明： STRU_CM_QUEUE *pQueue,        要读出的数据长度的队列
*    返回值  ： 返回队列中可用的数据长度，如果队列为空，返回0
*/
CM_UINT32 CM_QueueStatus (const STRU_CM_QUEUE *pQueue)
{
	return QueueStatus(pQueue);
}

/*
* 功能描述： 写队列函数，将一个数组中的数据写到一个队列中
*            对一个队列的操作不可以两个任务同时写入
*            队列是不可以写满的，如果写不下函数返回0
* 参数说明： CM_UINT32  DataLen,             要写入的数据长度;
*            CM_QueueElement *pData,         要写入的数据存放的数组;
*            STRU_CM_QUEUE *pQueue,         要写入数据的队列
* 返回值 ：  0x55，成功
*            0xAA，失败,队列已满
*/
ENUM_CM_RETURN CM_QueueWrite(CM_UINT32 DataLen,const CM_QueueElement *pData,STRU_CM_QUEUE *pQueue)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == QueueWrite(DataLen, pData, pQueue))
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
* 函数描述： 队列存储空间释放函数
*            在不需要某个队列的时候，把队列的元素数组空间释放掉,与CM_QueueInitial配对使用
* 参数说明： STRU_CM_QUEUE *pQueue,       队列
* 返回值  ： 0x55，成功，0xAA，失败
*/
ENUM_CM_RETURN CM_QueueElementFree(STRU_CM_QUEUE *pQueue)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == QueueElementFree(pQueue))
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
* 功能描述： 在队列中删除一定的元素。
* 参数说明： CM_UINT16 Length,             要删除的元素个数
*            STRU_CM_QUEUE *pQueue,       要删除元素的队列
* 返回值 ：  0x55，成功，0xAA，失败
*/
ENUM_CM_RETURN CM_QueueElementDiscard( CM_UINT16 Length, STRU_CM_QUEUE *pQueue)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == QueueElementDiscard(Length, pQueue))
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
*    功能描述： 读出队列中可用空间大小
*    参数说明： STRU_CM_QUEUE *pQueue,  指定的队列
*    返回值  ： 返回队列中可用的空间大小,由于队列不能写满，队列中有一个空间不能用
*/
CM_UINT32 CM_QueueGetSpace (const STRU_CM_QUEUE *pQueue)
{
	return QueueGetSpace(pQueue);
}

