
/************************************************************************
*
* 文件名   ：  CRC32.h
* 版权说明 ：  北京瑞安时代科技有限责任公司 
* 版本号   ：  1.0
* 创建时间 ：  2009.08.21
* 作者     ：  车载及协议部
* 功能描述 ：  计算32位CRC函数  
* 使用注意 ： 
* 修改记录 ：  
*
************************************************************************/

#ifndef CRC32_H
#define CRC32_H

#include "CommonTypes.h"

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
UINT32 Crc32(const UINT8 *pData,UINT32 DataLen);

/*
*  功能描述： CRC32函数，计算过程如下，注意：计算CRC是从输入数据第一个字节的最高BIT开始的
*             采用的多项式0x4c11db7L，初始值为0x00000000,未镜像，结果异或系数为0x00000000
*             如果DataLen=0，返回值也为0.
*  参数说明： pData,    数据指针
*             DataLen,  数据长度（字节）
*		UINT8 CrcNum 拆分校验的次数
*  返回值：   返回值无
*合并说明：由互联互通引入 add by qxt 20161210
*/
UINT8 BigFileCrc32(const UINT8 *pData,UINT32 DataLen,UINT8 CrcNum,UINT32 *CrcCode);

/*************************************************************
*  功能描述： Crc32计算函数，使用CRC32查找表1
*  参数说明：
*             pData,    数据指针
*             DataLen,  数据长度（字节）
*  返回值：   返回值crc32Result为CRC值
*  说明：     为保障平台小型化系统处理的安全性增加该CRC算法
*             add by lmy 20180411
**************************************************************/
UINT32 Crc32_1(const UINT8 *pData, UINT32 DataLen);

/*************************************************************
*  功能描述： Crc32函数，使用CRC32查找表2
*  参数说明：
*             pData,    数据指针
*             DataLen,  数据长度（字节）
*  返回值：   返回值crc32Result为CRC值
*  说明：     为保障平台小型化系统处理的安全性增加该CRC算法
*             add by lmy 20180411
**************************************************************/
UINT32 Crc32_2(const UINT8 *pData, UINT32 DataLen);

/*************************************************************
*  功能描述： Crc32计算函数
*  参数说明：
*             pData,    	数据指针
*             DataLen,  	数据长度（字节）
*			  CRC32Table,	CRC32查找表
*  返回值：   返回值crc32Result为CRC值
*  说明：     为保障平台小型化系统处理的安全性增加该CRC算法,
*			  该函数与RSSP-I协议中的(RsspCommon.c)CRC_Compute_Crc函数功能相同，
*			  但是参数不一样，需要注意。
*             add by lmy 20180411
**************************************************************/
UINT32 Crc32_common(const UINT8 *pData, UINT32 DataLen, const UINT32 *CRC32Table);

#ifdef __cplusplus
}
#endif

#endif

