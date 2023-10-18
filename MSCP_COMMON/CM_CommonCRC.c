/**
@file CM_CommonCRC.c
@brief 
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/


#include "CM_CommonCRC.h"

void CM_CommonCRCInit(COMMON_CRC_STRU *CommonCrcStru, UINT8 order, UINT32 poly, UINT8 refin, UINT8 refout, UINT32 crcinit, UINT32 crcxor)
{
	CommonCRCInit(CommonCrcStru, order, poly, refin, refout, crcinit, crcxor);

}

CM_UINT32 CM_CommonCRC(UINT8 *p, UINT32 Length, COMMON_CRC_STRU *CommonCrcStru)
{
	return CommonCRC(p, Length, CommonCrcStru);
}
