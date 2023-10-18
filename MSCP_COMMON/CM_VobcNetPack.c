
/**
@file  CM_VobcNetPack.c
@brief vobc内网信息层协议函数 
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#include "CM_Convert.h"
#include "CM_CRC32.h"
#include "CM_VobcNetPack.h"
#include "VobcNetPack.h"   /*whj*/   /*该文件包含位于h文件的函数*/

/*
*  功能描述： 内网信息层打包函数
*  参数说明： IN CM_UINT8 DataType，   报文类型
*             IN CM_UINT32 Seq,        控制周期序列号
*             IN const CM_UINT8 *pData,应用数据
*             IN CM_UINT16 DataLen,    应用数据长度
*             OUT CM_UINT8 *pPackData, 打包数据指针
*  返回值：   打包信息的数据长度
*/
CM_UINT16 CM_VobcNetPack(IN CM_UINT8 DataType,IN CM_UINT32 Seq,IN const CM_UINT8 *pData, IN CM_UINT16 DataLen,OUT CM_UINT8 *pPackData)
{
	return VobcNetPack(DataType, Seq, pData, DataLen, pPackData);
}

/*
*  功能描述： 内网信息层解包函数
*  参数说明： IN const CM_UINT8 *pPackData,     打包数据
*             IN const CM_UINT16 PackDataLen,   打包数据长度
*             OUT CM_UINT8 *pDataType,          数据类型
*             OUT CM_UINT32 *pSeq,              控制周期序列号
*             OUT CM_UINT8  *pData,             解包后的应用数据
*  返回值：   >0,  解包后的数据长度
*             0,   表示解包错误,或者解包后的数据长度为0
*/
CM_UINT16 CM_VobcNetUnpack(IN const CM_UINT8 *pPackData,IN const CM_UINT16 PackDataLen,OUT CM_UINT8 *pDataType,OUT CM_UINT32 *pSeq,OUT CM_UINT8 *pData)
{
	return VobcNetUnpack(pPackData, PackDataLen, pDataType, pSeq, pData);
}
