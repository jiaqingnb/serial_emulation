/**
@file CM_CRC32.h
@brief 计算32位CRC函数
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/

#ifndef CM_CRC32_H_  //修改飞检——不得使用保留标识符
#define CM_CRC32_H_

#include "CM_Types.h"
#include "CRC32.h"
#ifdef __cplusplus
extern "C" {
#endif


/*
*  功能描述： CRC32函数，计算过程如下，注意：计算CRC是从输入数据第一个字节的最高BIT开始的
*  参数说明：
*             pData,    数据指针
*             DataLen,  数据长度（字节）
*  返回值：   返回值为CRC值
*/
CM_UINT32 CM_Crc32(const CM_UINT8 *pData,CM_UINT32 DataLen);

/*
*  功能描述： BTM的CRC32函数，计算过程如下，注意：计算CRC是从输入数据第一个字节的最高BIT开始的
*             采用的多项式0x4c11db7L，初始值为0xFFFFFFFF,未镜像，结果异或系数为0x00000000
*             如果DataLen=0，返回值也为0.
*  参数说明： pData,    数据指针
*             DataLen,  数据长度（字节）
*  返回值：   返回值为CRC值
*/
UINT32 BTM_Crc32(const UINT8 *pData,UINT32 DataLen);

/*
*  功能描述： CRC32函数，计算过程如下，注意：计算CRC是从输入数据第一个字节的最高BIT开始的
*             采用的多项式0x4c11db7L，初始值为0x00000000,未镜像，结果异或系数为0x00000000
*             如果DataLen=0，返回值也为0.
*  参数说明： pData,    数据指针
*             DataLen,  数据长度（字节）
*		CM_UINT8 CrcNum 拆分校验的次数
*  返回值：   返回值无
*合并说明：由互联互通引入 add by qxt 20161210
*/
CM_UINT8 CM_BigFileCrc32(const CM_UINT8 *pData,CM_UINT32 DataLen,CM_UINT8 CrcNum,CM_UINT32 *CrcCode);
/*
*  功能描述： 第一种Crc32函数0x1942B361
*  参数说明：
*             pData,    数据指针
*             DataLen,  数据长度（字节）
*  返回值：   返回值为CRC值
*/
CM_UINT32 CM_Crc32_1(const CM_UINT8 *pData,CM_UINT32 DataLen);
/*
*  功能描述： 第二种Crc32函数0x1EF09C7E
*  参数说明：
*             pData,    数据指针
*             DataLen,  数据长度（字节）
*  返回值：   返回值为CRC值
*/
CM_UINT32 CM_Crc32_2(const CM_UINT8 *pData,CM_UINT32 DataLen);

/*
*  功能描述： nvram分片CRC32计算函数
*  参数说明：
*             pData,    数据指针
*             DataLen,  数据长度（字节）
*  返回值：   返回值为CRC值
*/
UINT32 Crc32Slice(UINT32 Crc,const UINT8 *pData,UINT32 DataLen);

UINT32 CM_Crc32_common(const UINT8 *pData, UINT32 DataLen, const UINT32 *CRC32Table);

#ifdef __cplusplus
}
#endif

#endif


