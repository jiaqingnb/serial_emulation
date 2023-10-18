/************************************************************************
*
* 文件名   ：  dsuTslConfigVar.h
* 版权说明 ：  交控科技股份有限公司
* 版本号   ：  1.0
* 创建时间 ：  2016.03.20
* 作者     ：  研发中心
* 功能描述 ：   dsu变量头文件
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/
#ifndef DSU_TSLCONFIG_VAR_H
#define DSU_TSLCONFIG_VAR_H

#include "dsuTslConfigStruct.h"

#ifdef  TSLCONFIG_VAR_DEFINE
#define EXTERN
#else
#define EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*设置TSL配置数据表结构体指针*/
UINT8 dquSetInitTslConfigRef(DSU_TSLCONFIG_STRU *pDsuTslConfigStru);

/*设置TSL配置数据表对应数据项数组的个数*/
UINT8 dquSetTslConfigDataStruArrayLen(UINT16 wDataID, UINT16 wStruDataLen);

/*设置TSL配置数据表中对应索引数组的个数*/
UINT8 dquSetTslConfigDataIndexArrayLen(UINT16 wDataID, UINT16 DataIndexLen);

/*获取TSL配置数据表对应数据项数组的个数*/
UINT8 dquGetTslConfigDataStruArrayLen(UINT16 wDataID, UINT16 *pStruDataLen);

/*设置TSL配置数据表中对应索引数组的个数*/
UINT8 dquGetTslConfigDataIndexArrayLen(UINT16 wDataID, UINT16 *pDataIndexLen);

#ifdef __cplusplus
}
#endif

#endif