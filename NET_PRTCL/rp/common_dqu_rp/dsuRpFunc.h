/************************************************************************
*
* 文件名   ：  
* 版权说明 ：  北京交控科技有限公司 
* 版本号   ：  1.0
* 创建时间 ：  2011.12.29
* 作者     ：  研发中心软件部
* 功能描述 ：  冗余层查询函数  
* 使用注意 ： 
* 修改记录 ：  
*
************************************************************************/

#ifndef DSU_RP_FUNC_H
#define DSU_RP_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CommonTypes.h"

	/*设备通信协议表0x4002*/
	typedef struct
	{
		UINT8 EmitterType;	/*发送设备类型*/
		UINT8 ReceptorType;	/*接受设备类型*/
		UINT8 CommuType;	/*所使用的通信协议，1为Sfp，2为Rp，3为Rsr，4为RSSPI,5为RSSPII*/
	}DSU_COMM_PROTCL_STRU;

	/*设备通信协议表信息*/
	typedef struct
	{
		UINT16 CommProtclStruLen;	/*设备通信协议表行数*/
		DSU_COMM_PROTCL_STRU *pCommProtclStru;	/*设备通信协议表*/
	}DSU_BOOK_IP_INFO_STRU;

	/*
	*函数名:	dsuIpInit
	*功能描述:	RP查询函数初始化
	*输入参数:	UINT8 *pFsName	FS文件
	*输入出参:	无
	*输出参数:	无
	*返回值:	1:成功;0失败
	*/
	UINT8 dsuIpInit(UINT8 *pFsName);

	/*
	*函数名:	dsuRpGetDevType
	*功能描述:	根据设备名称获取设备类型
	*输入参数:	UINT16 DevName	设备名称
	*输入出参:	无
	*输出参数:	无
	*返回值:	设备类型
	*/
	UINT8 dsuRpGetDevType(UINT16 DevName);

	/*
	*函数名:	dsuRpTypeId2DevName
	*功能描述:	根据类型 ID获取设备名称
	*输入参数:	UINT8 Type	设备类型
	*			UINT8 Id	设备ID
	*输入出参:	无
	*输出参数:	UINT16 DevName	设备名称
	*返回值:	无
	*/
	void dsuRpTypeId2DevName(UINT8 Type, UINT8 Id, UINT16* pDevName);

	/*
	*函数名:	dsurRpGetProtclType
	*功能描述:	根据本方名称对方名称获取协议类型
	*输入参数:	UINT16 LocalName	本方名称
	*			UINT16 DestName	对方名称
	*输入出参:	无
	*输出参数:	UINT8* protclTye	协议类型
	*返回值:	1:成功;0失败
	*/
	UINT8 dsurRpGetProtclType(UINT16 LocalName, UINT16 DestName, UINT8* protclTye);

#ifdef __cplusplus
}
#endif

#endif
