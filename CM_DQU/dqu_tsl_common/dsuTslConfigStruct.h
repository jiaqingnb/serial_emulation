/************************************************************************
* 文件名	：  dsuTslConfigStruct.h
* 版本号	：  1.0
* 创建时间	：	2016.03.20
* 作者		：	软件部
* 功能描述	：	dsu结构体定义头文件
* 使用注意	：
* 修改记录	：  无。
************************************************************************/

#ifndef TSLCONFIG_STRUCT_H
#define TSLCONFIG_STRUCT_H

#include "CommonTypes.h"

/*TSL配置数据*/
#define  TSL_CONFIG_DATA_TYPE			0xFDu

/*TSL配置数据表号*/
#define TSL_CONFIG_DATA_ID				1u

/*配置数据相邻TSL最大通信数量*/
#define TSL_COMM_MAX_NUM				5u


#define LEN_WORD sizeof(UINT16)
#define LEN_DWORD sizeof(UINT32)

#ifdef __cplusplus
extern "C" {
#endif


/* 静态数据长度信息结构体*/
typedef struct
{
	UINT16 wTslConfigNumLen;			/*数据库中TSL配置数据表的结构体的数量*/
}DSU_TSLCONFIG_LEN_STRU;

/*静态数据库结构体*/
/*TSL配置数据表*/
typedef struct {
	UINT16 index;				/*索引编号*/
	UINT16 tslId;				/*TSL_ID*/
	UINT16 linkId;				/*所处link编号*/
	UINT32 off;					/*所处link偏移量*/
	UINT32 tslIvocValidation;	/*TSL-IVOC共用数据校验信息*/
	UINT16 tslSwitchID;			/*TSL关联得道岔ID*/
} DSU_TSL_CONFIG_STRU;

/*存放静态数据各个数据结构的头指针*/
typedef struct DSU_StaticTslConfig
{
	DSU_TSL_CONFIG_STRU *pTslConfigStru;		/*TSL配置数据表结构体指针*/
}DSU_STATIC_TSLCONFIG_STRU;

/*为管理函数建立全局索引*/
typedef struct DSU_TslConfigIndexStruct
{
	UINT16 TSLCONFIGINDEXNUM;				/*TSL配置数据表索引数组空间大小*/
	UINT16* dsuTslConfigIndex;				/*TSL配置信息索引数组*/

}DSU_TSLCONFIG_INDEX_STRU;

/*TSL配置数据表总结构体*/
typedef struct DSU_TslConfigStruct
{
	DSU_TSLCONFIG_INDEX_STRU *dsuTslConfigIndexStru;

	DSU_TSLCONFIG_LEN_STRU *dsuTslConfigLenStru;            /*全局变量，记录读取的DSU数据中的各类型数据的数量*/

	DSU_STATIC_TSLCONFIG_STRU *dsuStaticTslConfigStru;		/*存放数据库中每种结构的头指针*/
} DSU_TSLCONFIG_STRU;

/*配置数据库结构体*/
extern DSU_TSLCONFIG_STRU	*g_dsuTslConfigStru;

#ifdef __cplusplus
}
#endif

#endif
