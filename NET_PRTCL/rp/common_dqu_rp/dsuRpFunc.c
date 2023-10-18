/*******************************************************************************
*
* 文件名  ：dsuRpFunc.c
* 版权说明：北京瑞安时代科技有限责任公司
* 版本号  ：1.0  
* 创建时间：2009.11.20
* 作者    ：车载及协议部
* 功能描述：与Rp协议相关的dsu查询函数函数体
* 使用注意：无
* 修改记录：无
*
*******************************************************************************/

#include "dsuRpFunc.h"
#include "CommonMemory.h"
#include "dfsDataRead.h"
#include "dquDataTypeDefine.h"

#define DSU_COMM_PROTCL_STRU_SIZE 3

DSU_BOOK_IP_INFO_STRU DsuBookIpInfoStru;     /*初始化之后用于保存初始化之后的数据的变量*/

/*
*函数名:	dsuIpInit
*功能描述:	RP查询函数初始化
*输入参数:	UINT8 *pFsName	FS文件
*输入出参:	无
*输出参数:	无
*返回值:	1:成功;0失败
*/
UINT8 dsuIpInit(UINT8 *pFsName)
{
	UINT8 rtn = 0;	/*返回值*/
	UINT8 readRtn = 0;	/*读取FS返回值*/
	UINT8 *pData = NULL;	/*读出的数据*/
	UINT32 dataLen = 0;	/*读出的数据长度*/
	UINT32 mallocLen = 0;	/*分配空间大小*/
	UINT32 offset = 0;	/*数据偏移量*/
	UINT32 i = 0;	/*用于循环*/

	if (NULL != pFsName)	/*输入参非空*/
	{
		readRtn = dquGetConfigData((CHAR*)pFsName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_COMM_PROTCL_ID, &pData, &dataLen);	/*读取设备通信协议表*/
		if ((1 == readRtn) && (DSU_COMM_PROTCL_STRU_SIZE <= dataLen))
		{
			DsuBookIpInfoStru.CommProtclStruLen = (UINT16)(dataLen / DSU_COMM_PROTCL_STRU_SIZE);  /*行数*/
			mallocLen = sizeof(DSU_COMM_PROTCL_STRU)*(DsuBookIpInfoStru.CommProtclStruLen);	/*需分配空间大小*/
			DsuBookIpInfoStru.pCommProtclStru = (DSU_COMM_PROTCL_STRU*)malloc(mallocLen); /*为结构体申请空间*/
			if (NULL != DsuBookIpInfoStru.pCommProtclStru)
			{
				CommonMemSet(DsuBookIpInfoStru.pCommProtclStru, mallocLen, 0, mallocLen);

				for (i = 0; i < DsuBookIpInfoStru.CommProtclStruLen; i++)
				{
					/*1字节本方类型*/
					DsuBookIpInfoStru.pCommProtclStru[i].EmitterType = pData[offset];
					offset++;
					/*1字节对方类型*/
					DsuBookIpInfoStru.pCommProtclStru[i].ReceptorType = pData[offset];
					offset++;
					/*1字节协议类型*/
					DsuBookIpInfoStru.pCommProtclStru[i].CommuType = pData[offset];
					offset++;
				}
				if (offset == dataLen)	/*设备通信协议表赋值成功*/
				{
					rtn = 1;
				}
			}
		}
	}

	return rtn;
}

/*
*函数名:	dsuRpGetDevType
*功能描述:	根据设备名称获取设备类型
*输入参数:	UINT16 DevName	设备名称
*输入出参:	无
*输出参数:	无
*返回值:	设备类型
*/
UINT8 dsuRpGetDevType(UINT16 DevName)
{
	UINT8 devType;
	devType=(UINT8)(DevName>>8);

	return devType;
}

/*
*函数名:	dsuRpTypeId2DevName
*功能描述:	根据类型 ID获取设备名称
*输入参数:	UINT8 Type	设备类型
*			UINT8 Id	设备ID
*输入出参:	无
*输出参数:	UINT16 DevName	设备名称
*返回值:	无
*/
void dsuRpTypeId2DevName(UINT8 Type, UINT8 Id, UINT16* pDevName)
{
	*pDevName = Type;
	*pDevName = (*pDevName)*0x100 + Id;
}

/*
*函数名:	dsurRpGetProtclType
*功能描述:	根据本方名称对方名称获取协议类型
*输入参数:	UINT16 LocalName	本方名称
*			UINT16 DestName	对方名称
*输入出参:	无
*输出参数:	UINT8* protclTye	协议类型
*返回值:	1:成功;0失败
*/
UINT8 dsurRpGetProtclType(UINT16 LocalName, UINT16 DestName,UINT8* protclTye)
{
	UINT8 chReturnValue;/*函数返回值*/
	UINT16 i;/*循环用*/

	UINT8 TempEmitterType;/*发送者设备类型*/
	UINT8 TempReceptorType;/*接受者设备类型*/

	DSU_COMM_PROTCL_STRU *pTempCommProtclStru;/*通信协议表临时变量*/

	/*获取输入参数的设备类型*/
	TempEmitterType=dsuRpGetDevType(LocalName);
	TempReceptorType=dsuRpGetDevType(DestName);

	pTempCommProtclStru=DsuBookIpInfoStru.pCommProtclStru;
	for (i=0;i<DsuBookIpInfoStru.CommProtclStruLen;i++)
	{
		if ((pTempCommProtclStru->EmitterType==TempEmitterType)&&(pTempCommProtclStru->ReceptorType == TempReceptorType))
		{

			*protclTye=pTempCommProtclStru->CommuType;
			break;
		}
		pTempCommProtclStru++;
	}

	if (i==DsuBookIpInfoStru.CommProtclStruLen)
	{
		/*没有查找到需要的通信类型*/
		chReturnValue=0;
		return chReturnValue;
	}

	chReturnValue =1;
	return chReturnValue;
}
