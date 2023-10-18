/**
@file CM_CommonCRC.h
@brief
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2018-05-10
*/

#ifndef CM_COMMONCRC_H  //修改飞检——不得使用保留标识符
#define CM_COMMONCRC_H

#include "CM_Types.h"
#include "CommonCRC.h"

#ifdef __cplusplus
extern "C" {
#endif

void CM_CommonCRCInit(COMMON_CRC_STRU *CommonCrcStru, UINT8 order, UINT32 poly, UINT8 refin, UINT8 refout, UINT32 crcinit, UINT32 crcxor);
	
CM_UINT32 CM_CommonCRC(UINT8 *p, UINT32 Length, COMMON_CRC_STRU *CommonCrcStru);
	
#ifdef __cplusplus
}
#endif

#endif
