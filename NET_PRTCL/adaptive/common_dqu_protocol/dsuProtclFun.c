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

#include "dsuProtclFun.h"
#include "CommonMemory.h"
#include "Convert.h"
#include "dfsDataRead.h"
#include "dquDataTypeDefine.h"

#ifdef DEV_IP_TABLE_NO_HLHT
#define DSU_IP_INFO_STRU_SIZE (22) /*设备IP地址表列占字节长度*/
#else
#define DSU_IP_INFO_STRU_SIZE (30) /*设备IP地址表列占字节长度*/
#endif

#define DSU_COMM_PROTCL_STRU_SIZE (3) /*设备通信协议表列占字节长度*/

/*FS的sheet页信息*/
typedef struct
{
	UINT32 Id; /*序号*/
	UINT32 Len; /*长度*/	
	UINT32 StartIdx; /*相对excel开始地址*/
	UINT32 RowCount; /*行总数*/
	UINT8* Data; /*excel数据*/
} SheetInfo;

static void InitSheet(SheetInfo* sheetInfo, const UINT32 dataLen, UINT8 data[], const UINT32 excelId, const UINT32 sheetId);
static UINT8 GetCommOppDevTypes(SheetInfo* commSheet, UINT8 locType, UINT8* oppTypes, UINT8* oppTypePtls, UINT16* oppTypeNum, const UINT16 maxOppTypeNum);
static void GetCommOppDevNum(SheetInfo* devIpSheet, UINT8 oppType, UINT16* dupOppHlhtIdsLen, UINT32* dupOppHlhtIds, const UINT16 maxDupOppHlhtIdsLen, UINT8* devNum);

/*
*函数名:	DsuCommonInfoInit
*功能描述:	适配层查询函数初始化
*输入参数:	UINT8 *pFsName	FS文件
*输入出参:	DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo	适配层使用表
*输出参数:	无
*返回值:	1:成功;0失败
*/
UINT8 DsuCommonInfoInit(UINT8 *pFsName, DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo)
{
	UINT8 rtn = 0; /*返回值*/
	UINT8 readRtn = 0;	/*读取FS返回值*/
	UINT8 *pData = NULL;	/*读出的数据*/
	UINT32 dataLen = 0;	/*读出的数据长度*/
	UINT32 mallocLen = 0;	/*分配空间大小*/
	UINT32 offset = 0;	/*数据偏移量*/
	UINT32 i = 0;	/*用于循环*/

	if ((NULL != pFsName) && (NULL != pDsuProtclBookIpInfo))	/*输入参非空*/
	{
		readRtn = dquGetConfigData((CHAR*)pFsName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_IP_ID, &pData, &dataLen);	/*读取设备IP地址表*/
		if ((1 == readRtn) && (DSU_IP_INFO_STRU_SIZE <= dataLen))
		{
			pDsuProtclBookIpInfo->IpDevLen = (UINT16)(dataLen / DSU_IP_INFO_STRU_SIZE);  /*行数*/
			mallocLen = sizeof(UINT16)*(pDsuProtclBookIpInfo->IpDevLen);	/*需分配空间大小*/
			pDsuProtclBookIpInfo->IpDevTypeIds = (UINT16*)malloc(mallocLen);	/*分配空间*/
			if (NULL != pDsuProtclBookIpInfo->IpDevTypeIds)
			{
				CommonMemSet(pDsuProtclBookIpInfo->IpDevTypeIds, mallocLen, 0, mallocLen);	/*初始清零*/

				mallocLen = sizeof(UINT32)*(pDsuProtclBookIpInfo->IpDevLen);	/*需分配空间大小*/
#ifndef DEV_IP_TABLE_NO_HLHT
				pDsuProtclBookIpInfo->IpDevHlhtIds = (UINT32*)malloc(mallocLen);	/*分配空间*/
				if (NULL != pDsuProtclBookIpInfo->IpDevHlhtIds)
				{
					CommonMemSet(pDsuProtclBookIpInfo->IpDevHlhtIds, mallocLen, 0, mallocLen);	/*初始清零*/
#endif

					for (i = 0; i < pDsuProtclBookIpInfo->IpDevLen; i++)
					{
						/*2字节设备类型ID*/
						pDsuProtclBookIpInfo->IpDevTypeIds[i] = ShortFromChar(&pData[offset]);
#ifndef DEV_IP_TABLE_NO_HLHT
						/*4字节互联互通ID*/
						pDsuProtclBookIpInfo->IpDevHlhtIds[i] = LongFromChar(&pData[offset + 2]);
#endif
						offset += DSU_IP_INFO_STRU_SIZE;
					}
					if (offset == dataLen)	/*设备类型ID赋值成功*/
					{
						readRtn = dquGetConfigData((CHAR*)pFsName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_COMM_PROTCL_ID, &pData, &dataLen);	/*读取设备通信协议表*/
						if ((1 == readRtn) && (DSU_COMM_PROTCL_STRU_SIZE <= dataLen))
						{
							pDsuProtclBookIpInfo->CommProtclStruLen = (UINT16)(dataLen / DSU_COMM_PROTCL_STRU_SIZE);	/*行数*/
							mallocLen = sizeof(DSU_PROTCL_COMM_PROTCL_STRU)*(pDsuProtclBookIpInfo->CommProtclStruLen);	/*需分配空间大小*/
							pDsuProtclBookIpInfo->pCommProtclStru = (DSU_PROTCL_COMM_PROTCL_STRU*)malloc(mallocLen);	/*分配空间*/
							if (NULL != pDsuProtclBookIpInfo->pCommProtclStru)
							{
								CommonMemSet(pDsuProtclBookIpInfo->pCommProtclStru, mallocLen, 0, mallocLen);	/*初始清零*/

								offset = 0;	/*偏移从零开始*/
								for (i = 0; i < pDsuProtclBookIpInfo->CommProtclStruLen; i++)
								{
									/*1字节本方类型*/
									pDsuProtclBookIpInfo->pCommProtclStru[i].EmitterType = pData[offset];
									offset++;
									/*1字节对方类型*/
									pDsuProtclBookIpInfo->pCommProtclStru[i].ReceptorType = pData[offset];
									offset++;
									/*1字节协议类型*/
									if ((DSU_PROTCL_RSSP2 + 4) == pData[offset]) /*RSSPII协议数据配置中为9,适配层代码中用的下标为5*/
									{
										pDsuProtclBookIpInfo->pCommProtclStru[i].CommuType = DSU_PROTCL_RSSP2;
									}
									else
									{
										pDsuProtclBookIpInfo->pCommProtclStru[i].CommuType = pData[offset];
									}
									offset++;
								}
								if (offset == dataLen)	/*设备通信协议表赋值成功*/
								{
									rtn = 1;
								}
							}
						}
					}
#ifndef DEV_IP_TABLE_NO_HLHT
				}
#endif				
			}
		}
	}

	return rtn;
}

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
UINT8 DsuProtclGetProtclType(UINT16 locTypeId, UINT16 oppTypeId, UINT8 *pProtclType, DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo)
{
	UINT8 rtn = 0;	/*返回值*/
	UINT8 locType = (UINT8)((locTypeId >> 8) & 0x00FF);	/*本方类型*/
	UINT8 oppType = (UINT8)((oppTypeId >> 8) & 0x00FF);	/*对方类型*/
	UINT32 i = 0;	/*用于循环*/

	if ((NULL != pProtclType) && (NULL != pDsuProtclBookIpInfo)&&(NULL!= pDsuProtclBookIpInfo->pCommProtclStru))	/*输入参非空*/
	{
		for (i = 0; i < pDsuProtclBookIpInfo->CommProtclStruLen; i++)	/*遍历设备通信协议表*/
		{
			/*匹配本方对方类型*/
			if ((pDsuProtclBookIpInfo->pCommProtclStru[i].EmitterType == locType) && (pDsuProtclBookIpInfo->pCommProtclStru[i].ReceptorType == oppType))
			{
				*pProtclType = pDsuProtclBookIpInfo->pCommProtclStru[i].CommuType;
				break;	/*找到退出循环*/
			}
		}
		if (i != pDsuProtclBookIpInfo->CommProtclStruLen)	/*找到*/
		{
			rtn = 1;
		}
	}

	return rtn;
}

/*
*函数名:	DsuGetProtclTypeInfo
*功能描述:	根据本方类型ID查找与对方通信时使用协议的最大通信设备数量(最大链路数)
*输入参数:	UINT16 locTypeId	本方类型ID
*			DSU_PROTCL_BOOK_IP_INFO_STRU *pDsuProtclBookIpInfo 适配层使用表
*输入出参:	无
*输出参数:	DSU_PROTCL_TYPE_STRU *dsuProtclTypeInfo	各协议通信设备数量
*返回值:	1:成功;0失败
*/
UINT8 DsuGetProtclTypeInfo(UINT16 locTypeId, DSU_PROTCL_TYPE_STRU* dsuProtclTypeInfo, DSU_PROTCL_BOOK_IP_INFO_STRU* pDsuProtclBookIpInfo, UINT32 inDataLen, UINT8 inData[])
{
	UINT8 rtn = 0;	/*返回值*/
	UINT8 locType = (UINT8)(locTypeId >> 8);	/*本方类型*/
	UINT32 i = 0;	/*用于循环*/
	UINT8 outCanGetComm = 0; /*外网是否配置错误*/
	UINT8 inCanGetComm = 0;	/*内网是否配置错误*/
	UINT16 oppTypeNum = 0;	/*对方类型数量*/
	UINT16 dupOppHlhtIdLen = 0;	/*重复对方类型ID长度*/
	UINT8 devNum = 0;	/*设备数量*/
	UINT32 oppTypeSpLen = 0; /*对方设备类型数组分配空间长度*/
	UINT32 dupOppHlhtIdSpLen = 0; /*重复对方互联互通ID数组分配空间长度*/
	UINT8* oppType = NULL; /*对方设备类型数组*/
	UINT8* oppTypePtl = NULL; /*对方设备类型协议数组*/
	UINT32* dupOppHlhtId = NULL; /*重复对方互联互通ID数组*/
	SheetInfo outCommSheet = { 0 }; /*外网不同类型sheet页*/
	SheetInfo inCommSheet = { 0 }; /*内网不同类型sheet页*/
	SheetInfo outDevIpSheet = { 0 }; /*外网相同类型sheet页*/
	SheetInfo inDevIpSheet = { 0 }; /*内网相同类型sheet页*/
	UINT32 outInCommRowCount = 0u; /*外内网通信协议类型sheet页行总数*/
	UINT32 outInDevIpRowCount = 0u; /*外内网设备IP地址sheet页行总数*/

	if ((NULL != dsuProtclTypeInfo) && (NULL != pDsuProtclBookIpInfo))
	{
		InitSheet(&outCommSheet, g_DataBufLen, g_pDataBuf, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_COMM_PROTCL_ID);
		InitSheet(&inCommSheet, inDataLen, inData, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_COMM_PROTCL_ID);
		InitSheet(&outDevIpSheet, g_DataBufLen, g_pDataBuf, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_IP_ID);
		InitSheet(&inDevIpSheet, inDataLen, inData, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_IP_ID);

		outInCommRowCount = pDsuProtclBookIpInfo->CommProtclStruLen + inCommSheet.RowCount;
		outInDevIpRowCount = pDsuProtclBookIpInfo->IpDevLen + inDevIpSheet.RowCount;

		oppTypeSpLen = sizeof(UINT8) * (outInCommRowCount);	/*对方类型数组空间长度*/
		dupOppHlhtIdSpLen = sizeof(UINT32) * (outInDevIpRowCount);	/*重复对方类型ID数组空间长度*/
		oppType = (UINT8*)malloc(oppTypeSpLen);	/*对方类型数组*/
		oppTypePtl = (UINT8*)malloc(oppTypeSpLen);	/*各对方类型使用协议*/
		dupOppHlhtId = (UINT32*)malloc(dupOppHlhtIdSpLen);	/*重复对方类型ID数组*/

		if ((NULL != oppType) && (NULL != oppTypePtl) && (NULL != dupOppHlhtId))	/*输入参数非空*/
		{
			CommonMemSet(oppType, oppTypeSpLen, 0, oppTypeSpLen);	/*初始清零*/
			CommonMemSet(oppTypePtl, oppTypeSpLen, 0, oppTypeSpLen);	/*初始清零*/
			CommonMemSet(dupOppHlhtId, dupOppHlhtIdSpLen, 0, dupOppHlhtIdSpLen);	/*初始清零*/

			outCanGetComm = GetCommOppDevTypes(&outCommSheet, locType, oppType, oppTypePtl, &oppTypeNum, (UINT16)(outInCommRowCount));
			if (1u == outCanGetComm) /*配置正常*/
			{
				inCanGetComm = GetCommOppDevTypes(&inCommSheet, locType, oppType, oppTypePtl, &oppTypeNum, (UINT16)(outInCommRowCount));
				if ((1u == outCanGetComm) && ((0 == inCommSheet.RowCount) || (0 < inCommSheet.RowCount && 1u == inCanGetComm))) /*配置正常*/
				{
					for (i = 0; i < oppTypeNum; i++)	/*遍历对方类型数组*/
					{
						devNum = 0;
						if ((VOBC_TYPE == locType) && ((AOM_TYPE == oppType[i]) || (ITE_TYPE == oppType[i])))
						{
							/*本方是ATP对方是AOM,ITE时通信数量固定为2*/
							devNum = 2;
						}
						else if ((VOBC_TYPE == locType) && ((CI_TYPE == oppType[i]) || (ZC_TYPE == oppType[i]) || (ATS_TYPE == oppType[i]) || (PSC_TYPE == oppType[i])))
						{
							/*本方是ATP对方是CI,ZC,ATS时通信数量固定为3*/
							devNum = 3;
						}
						else
						{
							GetCommOppDevNum(&outDevIpSheet, oppType[i], &dupOppHlhtIdLen, dupOppHlhtId, (UINT16)(outInDevIpRowCount), &devNum);
							GetCommOppDevNum(&inDevIpSheet, oppType[i], &dupOppHlhtIdLen, dupOppHlhtId, (UINT16)(outInDevIpRowCount), &devNum);
						}
						/*加到对应协议类型的通信设备数量上*/
						switch (oppTypePtl[i])
						{
						case DSU_PROTCL_SFP:
							(dsuProtclTypeInfo->sfpProtclNum) += devNum;
							break;
						case DSU_PROTCL_RSSP:
							(dsuProtclTypeInfo->rsspProtclNum) += devNum;
							break;
						case DSU_PROTCL_RSR:
							(dsuProtclTypeInfo->rsrProtclNum) += devNum;
							break;
						case DSU_PROTCL_RP:
							(dsuProtclTypeInfo->redunProtclNum) += devNum;
							break;
						case DSU_PROTCL_RSSP2:
							(dsuProtclTypeInfo->rssp2ProtclNum) += devNum;
							break;
						case DSU_PROTCL_FSFB:
							(dsuProtclTypeInfo->fsfbProtclNum) += devNum;
							break;
						default:
							break;
						}
					}

					rtn = 1;	/*执行成功*/
				}
			}

			/*释放动态内存*/
			free(oppType);
			oppType = NULL;
			free(oppTypePtl);
			oppTypePtl = NULL;
			free(dupOppHlhtId);
			dupOppHlhtId = NULL;
		}
	}

	return rtn;
}

/*
*功能描述:	初始化sheet页信息
*输入参数:	const UINT32 dataLen 数据长度
*			UINT8 data[] 数据
*			const UINT32 excelId excel序号
*			const UINT32 sheetId sheet页序号
*输入出参:	SheetInfo* sheetInfo sheet页信息
*输出参数:	无
*返回值:	无
*/
static void InitSheet(SheetInfo* sheetInfo, const UINT32 dataLen, UINT8 data[], const UINT32 excelId, const UINT32 sheetId)
{
	UINT32 idx = 0u; /*数据下标*/
	UINT32 crcVal = 0u; /*crc值*/
	UINT32 crc = 0u; /*计算的crc*/
	UINT32 curLen = 0u; /*当前长度*/
	UINT32 excelCount = 0u; /*excel数量*/
	UINT32 excelIdx = 0u; /*excel下标*/
	UINT32 curExcelId = 0u; /*当前excel序号*/
	UINT32 curExcelPos = 0u; /*当前在excel位置*/
	UINT32 curExcelStartPos = 0u; /*当前excel开始位置*/
	UINT32 sheetCount = 0u; /*sheet数量*/
	UINT32 sheetIdx = 0u; /*sheet下标*/
	UINT32 curSheetId = 0u; /*当前sheet序号*/
	UINT8 isBreak = 0u; /*是否退出*/
	UINT8 isFind = 0u; /*是否找到*/

	if ((0u < dataLen) && (NULL != data) && (NULL != sheetInfo)) /*参数检查*/
	{
		if ((idx + 40) <= dataLen) /*DH下标检查*/
		{
			/*4字节CRC*/
			crcVal = LongFromChar(&data[idx]);
			idx += 4;
			/*4字节版本*/
			idx += 4;
			/*4字节长度*/
			curLen = LongFromChar(&data[idx]);
			idx += 4;

			if ((curLen == dataLen) && (4 < curLen))
			{
				crc = Crc32(&data[4], curLen - 4);
				if (crc == crcVal)
				{
					/*4字节excel数量*/
					excelCount = LongFromChar(&data[idx]);
					idx += 4;
					/*24字节预留*/
					idx += 24;

					for (excelIdx = 0; excelIdx < excelCount; excelIdx++)
					{
						if ((idx + 24) <= dataLen) /*DI下标检查*/
						{
							/*4字节excel索引*/
							curExcelId = LongFromChar(&data[idx]);
							idx += 4;
							if (curExcelId == excelId)
							{
								/*4字节长度*/
								idx += 4;
								/*4字节excel位置*/
								curExcelPos = LongFromChar(&data[idx]);
								curExcelStartPos = curExcelPos;
								idx += 4;
								/*12字节预留*/
								idx += 12;

								if ((curExcelPos + 40) <= dataLen) /*DH下标检查*/
								{
									/*4字节excel CRC*/
									crcVal = LongFromChar(&data[curExcelPos]);
									curExcelPos += 4;
									/*4字节excel版本*/
									curExcelPos += 4;
									/*4字节excel长度*/
									curLen = LongFromChar(&data[curExcelPos]);
									curExcelPos += 4;

									if (4 < curLen)
									{
										crc = Crc32(&data[curExcelStartPos + 4], curLen - 4);
										if (crc == crcVal)
										{
											/*4字节sheet数量*/
											sheetCount = LongFromChar(&data[curExcelPos]);
											curExcelPos += 4;
											/*24字节预留*/
											curExcelPos += 24;

											for (sheetIdx = 0; sheetIdx < sheetCount; sheetIdx++)
											{
												if ((curExcelPos + 24) <= dataLen) /*DH下标检查*/
												{
													/*4字节sheet索引*/
													curSheetId = LongFromChar(&data[curExcelPos]);
													curExcelPos += 4;
													if (curSheetId == sheetId)
													{
														sheetInfo->Id = curSheetId;
														/*4字节长度*/
														sheetInfo->Len = LongFromChar(&data[curExcelPos]);
														curExcelPos += 4;
														/*4字节sheet相对excel位置*/
														sheetInfo->StartIdx = LongFromChar(&data[curExcelPos]);
														curExcelPos += 4;
														/*2字节行数*/
														sheetInfo->RowCount = ShortFromChar(&data[curExcelPos]);
														curExcelPos += 2;
														/*2字节列数*/
														curExcelPos += 2;
														/*8字节预留*/
														curExcelPos += 8;

														sheetInfo->Data = &data[curExcelStartPos];
														isFind = 1u;
													}
													else
													{
														curExcelPos += 20; /*偏移一个DI*/
													}
												}
												else /*长度错误*/
												{
													isBreak = 1u;
												}
												if ((1u == isFind) || (1u == isBreak))
												{
													break;
												}
											}
										}
										else /*crc错误*/
										{
											isBreak = 1u;
										}
									}
									else /*长度错误*/
									{
										isBreak = 1u;
									}
								}
								else /*长度错误*/
								{
									isBreak = 1u;
								}
							}
							else /*继续找*/
							{
								idx += 20; /*偏移一个DI*/
							}
						}
						else /*长度错误*/
						{
							isBreak = 1u;
						}
						if ((1u == isFind) || (1u == isBreak))
						{
							break;
						}
					}
				}
			}
		}
	}
}

/*
*功能描述:	获取通信的对方设备类型数组
*输入参数:	SheetInfo* commSheet 设备通信协议表
*			UINT8 locType 本方类型
*输入出参:	UINT8* oppTypes 对方类型数组
*			UINT8* oppTypePtls 对方类型协议数组
*			UINT16* oppTypeNum 对方类型数量
*			const UINT16 maxOppTypeNum 最大对方类型数量
*输出参数:	无
*返回值:	无
*/
static UINT8 GetCommOppDevTypes(SheetInfo* commSheet, UINT8 locType, UINT8* oppTypes, UINT8* oppTypePtls, UINT16* oppTypeNum, const UINT16 maxOppTypeNum)
{
	UINT8 retVal = 0u; /*返回值*/
	UINT32 rowLen = 0u; /*行长度*/
	UINT32 i = 0u; /*用于循环*/
	UINT32 j = 0u; /*用于循环*/
	UINT8 locDevType = 0u; /*本方设备类型*/
	UINT8 oppDevType = 0u; /*对方设备类型*/
	UINT8 ptlType = 0u; /*协议类型*/
	UINT8 isAddOppType = 0;	/*是否添加对方类型*/
	UINT8 isConfigErr = 0u; /*是否配置错误*/

	if ((NULL != commSheet) && (NULL != oppTypes) && (NULL != oppTypePtls) && (NULL != oppTypeNum) && (0u < commSheet->Len) && (0u < commSheet->RowCount) && (0 == commSheet->Len % commSheet->RowCount)) /*检查sheet长度*/
	{
		rowLen = commSheet->Len / commSheet->RowCount;
		for (i = 0; i < commSheet->RowCount; i++)	/*遍历设备通信协议表*/
		{
			locDevType = commSheet->Data[commSheet->StartIdx + (i * rowLen)];
			oppDevType = commSheet->Data[commSheet->StartIdx + (i * rowLen) + 1];
			ptlType = commSheet->Data[commSheet->StartIdx + (i * rowLen) + 2];
			if (9 == ptlType) /*调整外网RSSPII数据中9为5*/
			{
				ptlType = DSU_PROTCL_RSSP2;
			}
			if (locDevType == locType)/* 本方为发送方 */
			{
				isAddOppType = 0;
				for (j = 0; j < (*oppTypeNum); j++)	/*遍历查看是否有重复对方类型*/
				{
					if (oppTypes[j] == oppDevType)	/*有重复对方类型*/
					{
						if (oppTypePtls[j] != ptlType)	/*使用协议类型不同,配置错误*/
						{
							isConfigErr = 1u;
						}
						break;
					}
				}
				if (j == (*oppTypeNum))	/*类型不重复,添加*/
				{
					isAddOppType = 1u;
				}
				if (0u == isConfigErr)	/*配置正常*/
				{
					if ((1u == isAddOppType) && ((*oppTypeNum) < maxOppTypeNum)) /*赋值*/
					{
						oppTypes[*oppTypeNum] = oppDevType;	/*对方类型*/
						oppTypePtls[*oppTypeNum] = ptlType;	/*使用协议类型*/
						(*oppTypeNum)++;
					}
				}
				else /*配置错误,退出遍历*/
				{
					break;
				}
			}
		}
		if (0 == isConfigErr)	/*配置正常*/
		{
			retVal = 1u;
		}
	}

	return retVal;
}

/*
*功能描述:	获取通信的对方设备数量
*输入参数:	SheetInfo* devIpSheet 设备IP表
*			UINT8 oppType 对方类型
*			UINT16* dupOppHlhtIdLen 重复的对方互联互通ID长度
*			UINT32* dupOppHlhtId 重复的对方互联互通ID
*输入出参:	UINT16* dupOppHlhtIdLen 重复的对方互联互通ID长度
*			UINT32* dupOppHlhtId 重复的对方互联互通ID
*			UINT8* devNum 设备数量
*输出参数:	无
*返回值:	无
*/
static void GetCommOppDevNum(SheetInfo* devIpSheet, UINT8 oppType, UINT16* dupOppHlhtIdsLen, UINT32* dupOppHlhtIds, const UINT16 maxDupOppHlhtIdsLen, UINT8* devNum)
{
	UINT32 rowLen = 0u; /*行长度*/
	UINT32 i = 0u; /*用于循环*/
	UINT32 j = 0u; /*用于循环*/
	UINT16 devId = 0u; /*设备ID*/
	UINT32 hlhtId = 0u; /*HLHT ID*/

	if ((NULL != devIpSheet) && (NULL != dupOppHlhtIdsLen) && (NULL != dupOppHlhtIds) && (NULL != devNum) && (0u < devIpSheet->Len) && (0u < devIpSheet->RowCount) && (0 == devIpSheet->Len % devIpSheet->RowCount)) /*检查sheet长度*/
	{
		rowLen = devIpSheet->Len / devIpSheet->RowCount;
		for (i = 0; i < devIpSheet->RowCount; i++)	/*遍历设备通信协议表*/
		{
			devId = ShortFromChar(&devIpSheet->Data[devIpSheet->StartIdx + (i * rowLen)]);
			hlhtId = LongFromChar(&devIpSheet->Data[devIpSheet->StartIdx + (i * rowLen) + 2]);
			if (((UINT8)(devId >> 8)) == oppType)	/*设备类型相同*/
			{
				for (j = 0; j < (*dupOppHlhtIdsLen); j++)
				{
#ifdef DEV_IP_TABLE_NO_HLHT
					if (dupOppHlhtIds[j] == devId)	/*此HLHT ID重复,退出遍历*/
					{
						break;
					}
#else
					if (dupOppHlhtIds[j] == hlhtId)	/*此HLHT ID重复,退出遍历*/
					{
						break;
					}
#endif
				}
				if ((j == (*dupOppHlhtIdsLen)) && ((*dupOppHlhtIdsLen) < maxDupOppHlhtIdsLen))	/*此HLHT ID不重复*/
				{
#ifdef DEV_IP_TABLE_NO_HLHT
					dupOppHlhtIds[*dupOppHlhtIdsLen] = devId;	/*添加HLHT ID*/
#else
					dupOppHlhtIds[*dupOppHlhtIdsLen] = hlhtId;	/*添加HLHT ID*/
#endif
					(*dupOppHlhtIdsLen)++;	/*用来比较的HLHT ID加1*/
					(*devNum)++;	/*设备数量加1*/
				}
			}
		}
	}
}
