/************************************************************************
*
* 文件名   ：  dquTslConfigInit.h
* 版权说明 ：  交控科技股份有限公司
* 版本号   ：  1.0
* 创建时间 ：
* 作者     ：  软件部
* 功能描述 ：  TSL配置数据表初始化定义
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/
#ifndef  DQU_TSLCONFIG_INIT_H
#define  DQU_TSLCONFIG_INIT_H

#include "dfsDataRead.h"
#include "dsuTslConfigVar.h"
#include "dsuTslConfigStruct.h"
#include "dquTslConfigQuery.h"
#include "dquDataTypeDefine.h"

#define  INIT_TSLCONFIG_ERR_NUM   20u

extern UINT8  initTslConfigErrNo[INIT_TSLCONFIG_ERR_NUM];      /*各数据初始化失败故障号数组*/

#ifndef DEBUG_GET_DATA_FAIL
#define DEBUG_GET_DATA_FAIL    0x01u  /*获取数据失败*/
#endif

#ifndef DEBUG_DATA_LENTH_FAIL
#define DEBUG_DATA_LENTH_FAIL  0x02u  /*数据长度有误*/
#endif

#ifndef DEBUG_CALC_FAIL
#define DEBUG_CALC_FAIL        0x04u  /*计算过程错误*/
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*初始化TSL配置数据表*/
UINT8 dfsTslConfigInit(CHAR * FSName);

/*初始化TSL配置数据表扩展接口*/
UINT8 dfsTslConfigInitExp(UINT8 * pDataBuf, DSU_TSLCONFIG_STRU *dsuTslConfigStru, UINT8 mode, UINT32 timeSlice);

/*TSL配置数据表初始化完毕，执行TSL配置数据表内部公共变量的清理工作*/
UINT8 dfsTslConfigInitFinally(void);

/*初始化TSL配置数据表*/
UINT8 initTslConfig(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/*TSL配置数据结构体初始化函数*/
void PoseStru_init(UINT8 * pDataAdd, DSU_TSL_CONFIG_STRU* pTslConfigStru);

/*获取故障码*/
UINT8 dquGetTslConfigErrNo(UINT8* errRecord, UINT32 * errRecordLen);


#ifdef __cplusplus
}
#endif
#endif 