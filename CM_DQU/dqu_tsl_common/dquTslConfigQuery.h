/************************************************************************
*
* 文件名   ：  dsuTslConfigQuery.h
* 版权说明 ：  北京交控科技科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2016.04.12
* 作者     ：  软件部
* 功能描述 ：  dsu查询函数公共函数头文件
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/

#ifndef DSU_TslConfig_QUERY_H
#define DSU_TslConfig_QUERY_H

#include "dsuTslConfigStruct.h"

#define DQU_TSLCONFIG_INIT_TRUE 0x55u
#define DQU_TSLCONFIG_INIT_FALSE 0xAAu

#ifdef __cplusplus
extern "C" {
#endif

/* 设置当前数据源 */
UINT8 dquSetCurTslConfigQueryDataSource(DSU_TSLCONFIG_STRU* pDsuTslConfigStru);

/* TSL配置数据表查询函数访问接口 */
UINT8 dquTslInfoById(IN UINT16 id, OUT DSU_TSL_CONFIG_STRU* pTSLConfig);

#ifdef __cplusplus
}
#endif

#endif