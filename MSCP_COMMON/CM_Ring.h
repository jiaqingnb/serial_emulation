/*
* 环形队列
*/
#ifndef CM_RING_H  //修改飞检——不得使用保留标识符
#define CM_RING_H

#include "CM_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		CM_BOOL Initialized;/*初始化标志*/
		CM_UINT16 MsgNum; /*最大消息数目*/
		CM_UINT16 MsgSize;/*消息大小*/
		CM_UINT16 MsgCount;/*当前存储的消息数目*/
		CM_INT32 rear;/*队尾索引，最后一次写入的消息的首地址是buf+rear*MsgSize */
		CM_UINT8 *buf;/*数据缓冲区*/
	}STRU_CM_RQ_Light;

	/**
	* @brief 环形队列初始化
	* 初始内存分配
	* 只能保存固定长度的消息，队列写满后新写入的数据将覆盖最老的数据
	* @param[inout] STRU_CM_RQ_Light *pRQ 待初始化的环形队列
	* @param[in] CM_UINT16 MsgNum 环形队列中保存的最大消息数目
	* @param[in] CM_UINT16 MsgSize 环形队列中单条消息的长度
	* @return ENUM_CM_RETURN
	*/
	CM_BOOL CM_RQ_Init(STRU_CM_RQ_Light *pRQ,CM_UINT16 MsgNum,CM_UINT16 MsgSize);

	/**
	* @brief 写队列
	* 只能保存固定长度的消息，队列写满后新写入的数据将覆盖最老的数据
	* @param[inout] STRU_CM_RQ_Light *pRQ 待写入的环形队列
	* @param[in] CM_UINT8 msg[] 待写入的消息（消息长度是初始化时指定的MsgSize）
	* @return ENUM_CM_RETURN
	*/
	CM_BOOL CM_RQ_Write(STRU_CM_RQ_Light *pRQ, CM_UINT8 msg[]);

	/**
	* @brief 获取此前第n次写入的消息
	* （不会出队）
	* @param[in] STRU_CM_RQ_Light *pRQ 待读取的环形队列
	* @param[in] CM_UINT16 offset 之前第几次写入的消息（非负数），0表示最近写入的消息
	* @param[out] CM_UINT8 msg[] 读到的消息（消息长度是初始化时指定的MsgSize）
	* @return ENUM_CM_RETURN
	*/
	CM_BOOL CM_RQ_Get(STRU_CM_RQ_Light *pRQ, CM_UINT16 offset, CM_UINT8 msg[]);

	/**
	* @brief 清空环形队列
	* 清空环形队列中的内容（不释放内存）
	* @param[inout] STRU_CM_RQ_Light *pRQ 待清空的环形队列
	* @return ENUM_CM_RETURN
	*/
	CM_BOOL CM_RQ_Clear(STRU_CM_RQ_Light *pRQ);

	/**
	* @brief 获取环形队列中的消息总数
	* 当队列写满后返回的消息总数就是初始化时制定的MsgNum
	* 队列被Clear后Count变0
	* @param[inout] STRU_CM_RQ_Light *pRQ 待判断长度的环形队列
	* @return CM_UINT16 当前队列中保存的消息总数
	*/
	CM_UINT16 CM_RQ_GetCount(STRU_CM_RQ_Light *pRQ);
#ifdef __cplusplus
}
#endif

#endif
