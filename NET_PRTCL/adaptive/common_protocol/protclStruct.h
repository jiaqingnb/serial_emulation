/*
*
* 文件名   ： ProtclStruct.h
* 版权说明 ：  北京北京交控科技有限公司 
* 版本号   ：  1.0
* 创建时间 ：  2013.09.15
* 作者     ：  王佩佩
* 功能描述 ：  协议主处理类，主要实现了周期性调用协议。
* 使用注意 ：  
* 修改记录 ：
*    时间			修改人		修改理由
*---------------------------------------------
*/
#ifndef PROTCL_STRUCT_H
#define PROTCL_STRUCT_H

#include "CommonTypes.h"
#include "CommonQueue.h"
#include "protocolConfig.h"
#include "dsuProtclFun.h"

#ifdef USE_RSSPII
#include "PrtclBaseParse.h"
#include "Rssp2Struct.h"
#endif
#ifdef USE_SFP
#include "sfpplatform.h"
#endif
#ifdef USE_RSR
#include "RsspStruct.h"
#endif
#ifdef USE_RSSPI
#include "RsspStructWl.h"
#endif
#ifdef USE_RP
#include "RpPlatform.h"
#endif
#ifdef USE_FSFB
#include "fsfbProtclStruct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 协议适配层版本号定义 */
#define PROTOCOLCODE_VER_PRODUCT_NO     88/*产品编号*/
#define PROTOCOLCODE_VER_MODULE_NO      24/*软件模块编号*/
#define PROTOCOLCODE_VER_MAIN_VER       13/*软件模块主版本号*/
#define PROTOCOLCODE_VER_SUB_VER        3 /*软件模块子版本号*/
	/*接收数据*/
	typedef struct
	{
		UINT8 proType;		/*协议类型*/
		UINT8* pData;		/*接收的数据*/
		UINT32 rcvDataLen;	/*接收数据长度*/
	}PROTCL_RCV_DATA_TYPE;

	/*应用到协议的数据结构体*/
	typedef struct
	{
		UINT8* pAppData;			   /* 应用数据 */
		UINT16 appDataMaxLen;					   /* 应用数据最大长度 */
		UINT8 devType;							   /* 设备类型 */
		UINT8 devId;							   /* 设备ID */
		UINT16 devLogId;						   /* 设备动态ID */
		UINT16 appDataLen;						   /* 应用数据长度 */
	}PROTCL_APP_TO_STRU;

	/* 协议发送块配置结构体 */
	typedef struct
	{
		PROTCL_APP_TO_STRU* pProtclAppStru;
		UINT8 DevDataCnt;
	}PROTCL_SND_CFG_STRU;


	/*全局变量*/
	typedef struct
	{
		DSU_PROTCL_BOOK_IP_INFO_STRU DsuProtclBookIpInfo;	/*适配层使用表*/

#ifdef USE_SFP
		struc_Unify_Info *gpsfpUnitInfo;	/*SFP协议结构体*/
		UINT8* gpSfpRcvDataBuff; /*收的外部数据*/
		UINT16 gSfpRcvDataBuffLen; /*收的外部数据长度*/
#endif
#ifdef USE_RP
		RP_INFO_STRU *gpRedunUnitInfo;	/*冗余层协议结构体*/
		UINT8* gpRpRcvDataBuff; /*收的外部数据*/
		UINT16 gRpRcvDataBuffLen; /*收的外部数据长度*/
#endif
#ifdef USE_RSR
		RSR_INFO_STRU *gpRsrUnitInfo;	/*RSR协议结构体*/
		UINT8* gpRsrRcvDataBuff; /*收的外部数据*/
		UINT16 gRsrRcvDataBuffLen; /*收的外部数据长度*/
#endif
#ifdef USE_RSSPI
		RSSP_INFO_STRU *gpRsspUnitInfo;	/*RSSP协议结构体*/
		UINT8* gpRsspRcvDataBuff; /*收的外部数据*/
		UINT16 gRsspRcvDataBuffLen; /*收的外部数据长度*/
#endif
#ifdef USE_RSSPII
		RSSP2_INFO_STRU *gpRssp2UnitInfo;	/*RSSP2协议结构体*/
		PRTCL_BASE_CONF Rssp2PrtclHead;	/*RSSP2协议分帧使用的协议包头*/
		UINT8* gpRssp2RcvDataBuff; /*收的外部数据*/
		UINT16 gRssp2RcvDataBuffLen; /*收的外部数据长度*/
#endif
#ifdef USE_FSFB
		FSFB_INFO_STRU* gpFsfbUnitInfo;	/*FSFB协议结构体*/
		UINT8* gpFsfbRcvDataBuff; /*收的外部数据*/
		UINT16 gFsfbRcvDataBuffLen; /*收的外部数据长度*/
#endif

		PROTCL_APP_TO_STRU* pRcvProtclAppStru; /*接收单帧相关信息*/
		PROTCL_RCV_DATA_TYPE *RcvData; /*临时存放协议数据的结构体*/
		PROTCL_APP_TO_STRU* pSndProtclAppStru; /*发送单帧相关信息*/
		PROTCL_SND_CFG_STRU protclSndCfgStru[PROTCL_MGECFG_MAXCNT + 1];	/*发送相关信息结构体*/
		UINT8 pProtclNumBuff[PROTCL_MGECFG_MAXCNT + 1];	/*各协议最大链路数*/
		UINT32 curDevRecordLen[PROTCL_MGECFG_MAXCNT];	/*本设备使用日志长度*/
		DSU_PROTCL_TYPE_STRU protclTypeInfoStru;	/*查询函数相关信息结构体*/
	}GlobalVar;

/* 协议统一结构体 */
typedef struct  
{
    UINT8 cpuId;                    /* cpuID,小型化双CPU交互SVC*/
	UINT8 LocalID;            		/* 设备ID*/
	UINT8 LocalType;            	/* 设备type*/
	UINT16 LocalLogicId;			/*本方逻辑ID*/
	UINT32* CycleNum;               /* 周期号 */
	UINT16 InputSize; 				/* 接受数据中最大一帧的应用数据长度*/
	UINT16 OutputSize; 				/* 发送数据中最大一帧的应用数据长度*/
	QueueStruct OutnetQueueA;		/* 接收队列A */
	QueueStruct OutnetQueueB;		/* 接收队列B */
	UINT32 OutnetQueueSize;			/* 接收队列长度，由协议初始化时填写 */
	QueueStruct DataToApp;			/* 协议输入给应用的数据队列 */
	UINT32 DataToAppSize;			/* 协议输入给应用的数据队列长度 */
	QueueStruct OutputDataQueue;	/* 应用给协议输出的数据队列 */
	UINT32 OutputDataQueueSize;		/* 应用给协议输出的数据队列长度 */
	UINT32 VarArraySize;		    /* 中间变量数组长度 */
	UINT8 *VarArray;			    /* 中间变量数组 */
	UINT8 *AppArray;				/* 输出给应用的数据 */
	UINT32 AppArraySize;			/* 输出给应用的数组长度 */
	UINT8 *RecordArray;				/* 协议记录数组 */
	UINT16 RecordArraySize;			/* 协议记录数组大小 */
	GlobalVar *GlobalVar;			/*用到的全局变量*/
	UINT8 MaxNumPerCycle;           /*一周期允许每个对象的最大有效数据，由应用按文档在初始化时填写*/
}ProtclConfigInfoStru;

#ifdef __cplusplus
}
#endif

#endif
