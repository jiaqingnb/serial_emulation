/************************************************************************
*
* 文件名   ：  
* 版权说明 ：  北京交控科技有限公司 
* 版本号   ：  1.0
* 创建时间 ：  2011.12.29
* 作者     ：  研发中心软件部
* 功能描述 ：  协议适配层查询函数  
* 使用注意 ： 
* 修改记录 ：  
*
************************************************************************/

#ifndef DSU_PROTCL_FUN_H
#define DSU_PROTCL_FUN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CommonTypes.h"
#include "protocolConfig.h"
/* 协议适配层查询函数版本号定义 */
#define DQU_PROTOCOL_VER_PRODUCT_NO     88/*产品编号*/
#define DQU_PROTOCOL_VER_MODULE_NO      25/*软件模块编号*/
#define DQU_PROTOCOL_VER_MAIN_VER       13/*软件模块主版本号*/
#define DQU_PROTOCOL_VER_SUB_VER        3 /*软件模块子版本号*/

#define DSU_PROTCL_NTP		((UINT8)0)	/*NTP*/
#define DSU_PROTCL_SFP		((UINT8)1)	/*SFP*/
#define DSU_PROTCL_RP		((UINT8)2)	/*RP*/
#define DSU_PROTCL_RSR		((UINT8)3)	/*RSR*/
#define DSU_PROTCL_RSSP		((UINT8)4)	/*RSSPI*/
#define DSU_PROTCL_RSSP2	((UINT8)5)	/*RSSPII*/
#define DSU_PROTCL_FSFB		((UINT8)6)	/*FSFB*/

#define PROTCL_MGECFG_MAXCNT	((UINT8)0x06)	/*协议数量*/

#define	VOBC_TYPE	((UINT8)0x14)	/*VOBC设备类型*/
#define	AOM_TYPE	((UINT8)0x15)	/*AOM设备类型*/
#define	CI_TYPE		((UINT8)0x3C)	/*CI设备类型*/
#define	ZC_TYPE		((UINT8)0x1E)	/*ZC设备类型*/
#define	ATS_TYPE	((UINT8)0x03)	/*ATS设备类型*/
#define PSC_TYPE	((UINT8)0x61)	/*PSC设备类型*/
#define ITE_TYPE	((UINT8)0x18)	/*ITE设备类型*/

	/*设备通信协议表0x4002*/
	typedef struct
	{
		UINT8 EmitterType;	/*发送设备类型*/
		UINT8 ReceptorType;	/*接受设备类型*/
		UINT8 CommuType;	/*所使用的通信协议，1为Sfp，2为Rp，3为Rsr，4为RSSPI,5为RSSPII*/
	}DSU_PROTCL_COMM_PROTCL_STRU;

	/*适配层使用表*/
	typedef struct
	{
		UINT16 IpDevLen;	/*设备IP地址表0x4001-行数*/
		UINT16* IpDevTypeIds;	/*设备IP地址表0x4001-设备类型ID数组*/

#ifndef DEV_IP_TABLE_NO_HLHT
		UINT32* IpDevHlhtIds;	/*设备IP地址表0x4001-设备HLHT ID数组*/
#endif

		UINT16 CommProtclStruLen;	/*设备通信协议表行数*/
		DSU_PROTCL_COMM_PROTCL_STRU *pCommProtclStru;/*设备通信协议表*/
	}DSU_PROTCL_BOOK_IP_INFO_STRU;

	/*各协议通信设备数量*/
	typedef struct
	{
		UINT8 sfpProtclNum;	/*SFP通信设备数量*/
		UINT8 rsrProtclNum;	/*RSR通信设备数量*/
		UINT8 rsspProtclNum;	/*RSSPI通信设备数量*/
		UINT8 redunProtclNum;	/*RP通信设备数量*/
		UINT8 rssp2ProtclNum;	/*RSSPII通信设备数量*/
		UINT8 fsfbProtclNum;	/*FSFB通信设备数量*/
	}DSU_PROTCL_TYPE_STRU;

	/*
	*函数名:	DsuCommonInfoInit
	*功能描述:	适配层查询函数初始化
	*输入参数:	UINT8 *pFsName	FS文件
	*输入出参:	DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo	适配层使用表
	*输出参数:	无
	*返回值:	1:成功;0失败
	*/
	UINT8 DsuCommonInfoInit(UINT8 *pFsName, DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo);

	/*
	*函数名:	DsuProtclGetProtclType
	*功能描述:	根据本方类型ID与对方类型ID查找所使用的协议类型
	*输入参数:	UINT16 locTypeId	本方类型ID
	*			UINT16 oppTypeId	对方类型ID
	*			DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo 适配层使用表
	*输入出参:	无
	*输出参数:	UINT8 *pProtclType	协议类型
	*返回值:	1:成功;0失败
	*/
	UINT8 DsuProtclGetProtclType(UINT16 locTypeId, UINT16 oppTypeId, UINT8 *pProtclType, DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo);

	/*
	*函数名:	DsuGetProtclTypeInfo
	*功能描述:	根据本方类型ID查找与对方通信时使用协议的最大通信设备数量(最大链路数)
	*输入参数:	UINT16 locTypeId	本方类型ID
	*			DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo 适配层使用表
	*输入出参:	无
	*输出参数:	DSU_PROTCL_TYPE_STRU *dsuProtclTypeInfo	各协议通信设备数量
	*返回值:	1:成功;0失败
	*/
	UINT8 DsuGetProtclTypeInfo(UINT16 locTypeId, DSU_PROTCL_TYPE_STRU* dsuProtclTypeInfo, DSU_PROTCL_BOOK_IP_INFO_STRU* pDsuProtclBookIpInfo, UINT32 inDataLen, UINT8 inData[]);

#ifdef __cplusplus
}
#endif

#endif
