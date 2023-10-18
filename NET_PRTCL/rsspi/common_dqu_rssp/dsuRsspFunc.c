/*******************************************************************************
*
* 文件名  ：dsuRsspFunc.c
* 版权说明：北京瑞安时代科技有限责任公司
* 版本号  ：1.0  
* 创建时间：2009.11.20
* 作者    ：车载及协议部
* 功能描述：与RSR协议相关的dsu查询函数函数体
* 使用注意：无
* 修改记录：无
***	2013.02.15	王佩佩	删除dsuRsspRGetLnkInfo，增加dsuRsspRGetLnkInfoWl
**	2013.02.15	王佩佩	删除dsuRsspSGetLnkInfo，增加dsuRsspSGetLnkInfoWl
**	2013.02.15	王佩佩	增加dsuInputdevvsOutputDevOutInfoWl
**	2013.02.15	王佩佩	增加dsuInputdevvsOutputdevInInfoWl
**	2013.02.15	王佩佩	删除ManageRsspData
**	2013.02.15	王佩佩	删除dsuRsrInit，增加dsuRsspInitWl
**	2013.02.15	王佩佩	增加dsuRsspRGetAddrWl
**	2013.02.15	王佩佩	增加dsuRsspRGetName_WL
*******************************************************************************/

#include "dsuRsspFunc.h"
#include "CommonMemory.h"
#include "Convert.h"
#include "dfsDataRead.h"
#include "dquDataTypeDefine.h"
#include "RsspConfigWl.h"
#include "RsspCommonWl.h"

#ifdef RSSP_NO_HLHT
#define	DSU_RSSP_LNK_INFO_STRU_SIZE_WL	(DSURSR_MAX_K_NUM_WL*8+35)	/*RSSP链接信息列占字节长度*/
#else
#define	DSU_RSSP_LNK_INFO_STRU_SIZE_WL	(DSURSR_MAX_K_NUM_WL*8+39)	/*RSSP链接信息列占字节长度*/
#endif

#define	DSU_DEVICE_COMM_INFO_STRU_SIZE_WL	11	/*设备通信属性列占字节长度*/


#define IDMAP_TDT_ID (20)  /*TDT编号数据ID*/

#define	ID_MAP_SIZE	6	/*地面设备编号对照表列占字节长度*/
#define	VOBC_ID_MAP_SIZE	10	/*车载设备编号对照表列占字节长度*/

#define DEV_HLHTID_NUM_ONE	1	/*设备对应的hlhtid数量-1个*/
#define DEV_HLHTID_NUM_TWO	2	/*设备对应的hlhtid数量-2个*/

#define	ZC_TYPE	0x1E	/*ZC设备类型*/
#define	CI_TYPE	0x3C	/*CI设备类型*/
#define	ATS_TYPE	0x03	/*ATS设备类型*/
#define	VOBC_TYPE	0x14	/*VOBC设备类型*/
#define	DSU_TYPE	0x2B	/*DSU设备类型*/
#define	AOM_TYPE	0x15	/*AOM设备类型*/
#define TDT_TYPE	0x2C	/*TDT设备类型*/
#define CHANNEL_NUM (2) /*参数校验两通道*/
#define COMM_DEV_TYPE_MAX_NUM (30) /*与本设备通信的设备类型最大数量*/
#define COMM_DEV_MAX_NUM (256*COMM_DEV_TYPE_MAX_NUM) /*与本设备通信的设备最大数量*/

/*FS的sheet页信息*/
typedef struct
{
	UINT32 Id; /*序号*/
	UINT32 Len; /*长度*/
	UINT32 StartIdx; /*相对excel开始地址*/
	UINT32 RowCount; /*行总数*/
	UINT8* Data; /*excel数据*/
} SheetInfo;

/*安全校验参数*/
typedef struct
{	
	UINT32 DevId; /*设备ID*/	
	UINT16 Addr; /*地址*/
	UINT32 Sid[CHANNEL_NUM]; /*通道源ID*/	
	UINT32 Sinit[CHANNEL_NUM]; /*通道源初始化参数*/	
	UINT32 Dataver[CHANNEL_NUM]; /*通道版本*/
} SafeParam;

static void InitSheet(SheetInfo* sheetInfo, const UINT32 dataLen, UINT8 data[], const UINT32 excelId, const UINT32 sheetId);
static UINT32 PrecSinit(UINT32 sinit, UINT32 sid, UINT8 channel, UINT8 addTimes);
static UINT32 PrecRxdata(UINT32 sinit, UINT32 sid, UINT8 channel, UINT8 addTimes);
static UINT8 CheckInOutParamSheet(const UINT32 inDataLen, UINT8 inData[], const UINT32 outDataLen, UINT8 outData[], SheetInfo* inParamSheet, SheetInfo* outParamSheet);
static void GenerateParamsRow(SafeParam* locSafeParam, SafeParam* oppSafeParam, UINT16* rsspLnkInfosLen, DSU_RSSP_LNK_INFO_STRU_WL rsspLnkInfos[], const UINT16 maxRsspLnkInfosLen);
static UINT8 GetDevSafeParamFromSheet(const UINT32 locHlhtId, SheetInfo* paramSheet, SafeParam* safeParam);
static UINT8 GetDevSafeParam(const UINT32 locHlhtId, SheetInfo* outParamSheet, SheetInfo* inParamSheet, SafeParam* safeParam);
static void GetCommTypes(const UINT16 locTypeId, SheetInfo* commSheet, UINT16* commTypesLen, UINT8 commTypes[], const UINT16 maxCommTypesLen);
static void GetHlhtIds(const UINT8 oppDevType, SheetInfo* paramSheet, UINT16* hlhtIdsLen, UINT32 hlhtIds[], const UINT16 maxHlhtIdsLen, DSU_RSSP_INFO_STRU_WL* dsuRsspInfo);
static UINT8 CalcOppHlhtIds(const UINT16 locTypeId, const UINT32 locHlhtId, SheetInfo* outParamSheet, SheetInfo* inParamSheet, const UINT32 inDataLen, UINT8 inData[], const UINT32 outDataLen, UINT8 outData[], SafeParam* locSafeParam, UINT16* oppHlhtIdsLen, UINT32 oppHlhtIds[], const UINT16 maxOppHlhtIdsLen, DSU_RSSP_INFO_STRU_WL* dsuRsspInfo);
static void MergeRsspiParam(const UINT16 oppHlhtIdsLen, UINT32 oppHlhtIds[], SheetInfo* outParamSheet, SheetInfo* inParamSheet, SafeParam* locSafeParam, DSU_RSSP_LNK_INFO_STRU_WL lnkInfos[], const UINT16 maxLnkInfosLen);

#ifndef RSSP_NO_HLHT
static UINT8 DquInitHlhtId(UINT8 *pFsName, UINT8 sheetId, UINT8 colUseLen, UINT8 hlhtIdNum, UINT16 *pRowNum, DSU_DEV_HLHTID **pDsuDevHlhtids);

static UINT32 DquGetHlhtId(UINT8 type, UINT8 id, UINT16 logicId, UINT16 rowNum, const DSU_DEV_HLHTID *pDevHlhtids);

static UINT32 DquGetDevHlhtId(UINT8 devType, UINT8 devId, UINT16 logicId, DSU_RSSP_INFO_STRU_WL *pDsuRssp);

static UINT8 DquGetDevId(UINT32 hlhtId, UINT16 rowNum, const DSU_DEV_HLHTID *pDevHlhtids, UINT8 hlhtIdNum);
#endif

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
*功能描述:	生成PrecSinit
*输入参数:	UINT32 sinit
*			UINT32 sid
*			UINT8 channel
*			UINT8 addTimes
*输入出参:	无
*输出参数:	无
*返回值:	UINT32 PrecSinit
*/
static UINT32 PrecSinit(UINT32 sinit, UINT32 sid, UINT8 channel, UINT8 addTimes)
{
	if (0 == addTimes)
	{
		return F_ADD_WL(F_ADD_WL(F_ADD_WL(sinit, channel, sid), channel, 0), channel, 0);
	}
	else
	{
		addTimes--;
		return F_ADD_WL(PrecSinit(sinit, sid, channel, addTimes), channel, 0);
	}
}

/*
*功能描述:	生成PrecRxdata
*输入参数:	UINT32 sinit 初始化
*			UINT32 sid
*			UINT8 channel
*			UINT8 addTimes
*输入出参:	无
*输出参数:	无
*返回值:	UINT32 PrecRxdata
*/
static UINT32 PrecRxdata(UINT32 sinit, UINT32 sid, UINT8 channel, UINT8 addTimes)
{
	if (0 == addTimes)
	{
		return F_ADD_WL(sinit, channel, sid);
	}
	else
	{
		addTimes--;
		return F_ADD_WL(PrecRxdata(sinit, sid, channel, addTimes), channel, 0);
	}
}

/*
*功能描述:	检查内外参数表
*输入参数:	const UINT32 inDataLen 内网数据长度
*           UINT8 inData[] 内数据
*           const UINT32 outDataLen 外网数据长度
*			UINT8 outData[] 外数据
*输入出参:	SheetInfo* inParamSheet 内网参数sheet信息
*			SheetInfo* outParamSheet 外网参数sheet信息
*输出参数:	无
*返回值:	UINT8 是否检查成功.0:不成功,1:成功
*/
static UINT8 CheckInOutParamSheet(const UINT32 inDataLen, UINT8 inData[], const UINT32 outDataLen, UINT8 outData[], SheetInfo* inParamSheet, SheetInfo* outParamSheet)
{
	UINT8 retVal = 0u; /*返回值*/
	UINT32 inParamRowLen = 0u; /*内网参数行长度*/
	UINT32 inParamRowIdx = 0u; /*内网参数行下标*/
	UINT32 inParamDevId = 0u; /*内网参数设备ID*/
	UINT32 inParamSid = 0u; /*内网参数SID*/
	UINT32 inParamSinit = 0u; /*内网参数SINIT*/
	UINT32 inParamDataver = 0u; /*内网参数DATAVER*/
	UINT8 isBreak = 0u; /*是否退出*/
	UINT32 outParamRowLen = 0u; /*外网参数行长度*/
	UINT32 outParamRowIdx = 0u; /*外网参数行下标*/
	UINT32 outParamDevId = 0u; /*外网参数设备ID*/
	UINT32 outParamSid = 0u; /*外网参数SID*/
	UINT32 outParamSinit = 0u; /*外网参数SINIT*/
	UINT32 outParamDataver = 0u; /*外网参数DATAVER*/

	if ((NULL != outData) && (NULL != outParamSheet))
	{
		InitSheet(outParamSheet, outDataLen, outData, DSU_DATA_PROTCL_TYPE, DSU_DATA_PROTCL_RSSP_LNK_ID);
		if (0u < inDataLen) /*内网参数有数据*/
		{
			if ((NULL != inData) && (NULL != inParamSheet)) /*参数检查*/
			{
				InitSheet(inParamSheet, inDataLen, inData, DSU_DATA_PROTCL_TYPE, DSU_DATA_PROTCL_RSSP_LNK_ID);
				if ((0u < inParamSheet->Len) && (0u < inParamSheet->RowCount)) /*检查内网-RSSP链接信息表-长度*/
				{
					if (0u == (inParamSheet->Len % inParamSheet->RowCount)) /*整行*/
					{
						inParamRowLen = inParamSheet->Len / inParamSheet->RowCount;
						for (inParamRowIdx = 0u; inParamRowIdx < inParamSheet->RowCount; inParamRowIdx++) /*遍历内网-RSSP链接信息表*/
						{
							inParamDevId = LongFromChar(&inParamSheet->Data[inParamSheet->StartIdx + (inParamRowIdx * inParamRowLen) + 2u]);
							inParamSid = LongFromChar(&inParamSheet->Data[inParamSheet->StartIdx + (inParamRowIdx * inParamRowLen) + 12u]);
							inParamSinit = LongFromChar(&inParamSheet->Data[inParamSheet->StartIdx + (inParamRowIdx * inParamRowLen) + 16u]);
							inParamDataver = LongFromChar(&inParamSheet->Data[inParamSheet->StartIdx + (inParamRowIdx * inParamRowLen) + 21u]);
							if ((0u < outParamSheet->Len) && (0u < outParamSheet->RowCount) && (0u == (outParamSheet->Len % outParamSheet->RowCount)))/*检查外网-RSSP链接信息表-长度*/
							{
								outParamRowLen = outParamSheet->Len / outParamSheet->RowCount;
								if (outParamRowLen == inParamRowLen)
								{
									for (outParamRowIdx = 0u; outParamRowIdx < outParamSheet->RowCount; outParamRowIdx++) /*遍历外网-RSSP链接信息表*/
									{
										outParamDevId = LongFromChar(&outParamSheet->Data[outParamSheet->StartIdx + (outParamRowIdx * outParamRowLen) + 2u]);
										outParamSid = LongFromChar(&outParamSheet->Data[outParamSheet->StartIdx + (outParamRowIdx * outParamRowLen) + 12u]);
										outParamSinit = LongFromChar(&outParamSheet->Data[outParamSheet->StartIdx + (outParamRowIdx * outParamRowLen) + 16u]);
										outParamDataver = LongFromChar(&outParamSheet->Data[outParamSheet->StartIdx + (outParamRowIdx * outParamRowLen) + 21u]);
										if ((outParamDevId == inParamDevId) && (outParamSid == inParamSid) && (outParamSinit == inParamSinit) && (outParamDataver == inParamDataver)) /*内外网参数相同*/
										{
											isBreak = 1u; /*退出*/
											break;
										}
									}
								}
								else /*内外网参行长度不同*/
								{
									isBreak = 1u;
								}
							}
							if (1u == isBreak) /*是退出*/
							{
								break;
							}
						}
						if (0u == isBreak) /*遍历完无相同参*/
						{
							retVal = 1u;
						}
					}
				}
				else /*RSSP链接信息表 无数据*/
				{
					retVal = 1u;
				}
			}
		}
		else /*无内网参数数据*/
		{
			retVal = 1u;
		}
	}

	return retVal;
}

/*
*功能描述:	产生参数行
*输入参数:	SafeParam* locSafeParam 本方安全参数
*           SafeParam* oppSafeParam 对方安全参数
*			const UINT16 maxRsspLnkInfosLen 最大参数数组长度
*输入出参:	UINT16* rsspLnkInfosLen 当前的参数数组长度
*			DSU_RSSP_LNK_INFO_STRU_WL rsspLnkInfos[] 参数数组
*输出参数:	无
*返回值:	无
*/
static void GenerateParamsRow(SafeParam* locSafeParam, SafeParam* oppSafeParam, UINT16* rsspLnkInfosLen, DSU_RSSP_LNK_INFO_STRU_WL rsspLnkInfos[], const UINT16 maxRsspLnkInfosLen)
{
	UINT32 i = 0u; /*用于循环*/
	UINT8 channelIdx = 0u; /*通道下标*/
	UINT8 maxLostIdx = 0u; /*最大丢包下标*/

	if ((NULL != locSafeParam) && (NULL != oppSafeParam) && (NULL != rsspLnkInfosLen) && (NULL != rsspLnkInfos)) /*参数检查*/
	{
		for (i = 0; i < (*rsspLnkInfosLen); i++) /*遍历检查*/
		{
			if ((rsspLnkInfos[i].EmitterName == locSafeParam->DevId) && (rsspLnkInfos[i].ReceptorName == oppSafeParam->DevId)) /*已有参数*/
			{
				break;
			}
		}
		if ((i == (*rsspLnkInfosLen)) && ((*rsspLnkInfosLen) < maxRsspLnkInfosLen)) /*未有参数,添加*/
		{
			for (channelIdx = 0u; channelIdx < CHANNEL_NUM; channelIdx++) /*遍历两通道*/
			{
				rsspLnkInfos[*rsspLnkInfosLen].Id = (UINT16)(*rsspLnkInfosLen + 1);
#ifdef RSSP_NO_HLHT
				rsspLnkInfos[*rsspLnkInfosLen].EmitterName = (UINT16)locSafeParam->DevId;
				rsspLnkInfos[*rsspLnkInfosLen].ReceptorName = (UINT16)oppSafeParam->DevId;
#else
				rsspLnkInfos[*rsspLnkInfosLen].EmitterName = locSafeParam->DevId;
				rsspLnkInfos[*rsspLnkInfosLen].ReceptorName = oppSafeParam->DevId;
#endif

				rsspLnkInfos[*rsspLnkInfosLen].ChannelNumber = channelIdx + 1;
				rsspLnkInfos[*rsspLnkInfosLen].SID_NUMBER_E = locSafeParam->Sid[channelIdx];
				rsspLnkInfos[*rsspLnkInfosLen].SINIT_NUMBER = locSafeParam->Sinit[channelIdx];
				rsspLnkInfos[*rsspLnkInfosLen].DATANUM = 1;
				rsspLnkInfos[*rsspLnkInfosLen].DATAVER_1_Number = locSafeParam->Dataver[channelIdx];
				rsspLnkInfos[*rsspLnkInfosLen].RsspAddr = locSafeParam->Addr;
				rsspLnkInfos[*rsspLnkInfosLen].BsdAddr = oppSafeParam->Addr;
				rsspLnkInfos[*rsspLnkInfosLen].SseAddr = oppSafeParam->Addr;
				rsspLnkInfos[*rsspLnkInfosLen].SID_NUMBER_R = oppSafeParam->Sid[channelIdx];
				rsspLnkInfos[*rsspLnkInfosLen].PREC_FIRSTSINIT = locSafeParam->Sinit[channelIdx] ^ oppSafeParam->Sid[channelIdx] ^ locSafeParam->Dataver[channelIdx];

				for (maxLostIdx = 0u; maxLostIdx < DSURSR_MAX_K_NUM_WL; maxLostIdx++)
				{
					/*((SINITr ADD SIDr)^(SINITr ADD SIDr ADD 0 ADD 0 ADD ... 0)) SUB SIDr SUB SIDr*/
					rsspLnkInfos[*rsspLnkInfosLen].PREC_SINIT_K[maxLostIdx] = F_SUB_WL(F_SUB_WL(F_ADD_WL(locSafeParam->Sinit[channelIdx], channelIdx, locSafeParam->Sid[channelIdx])
						^ PrecSinit(locSafeParam->Sinit[channelIdx], locSafeParam->Sid[channelIdx], channelIdx, maxLostIdx)
						, channelIdx
						, locSafeParam->Sid[channelIdx])
						, channelIdx
						, locSafeParam->Sid[channelIdx]);
				}

				for (maxLostIdx = 0; maxLostIdx < DSURSR_MAX_K_NUM_WL; maxLostIdx++)
				{
					/*expectVal ADD 0*/
					/*SINITr ADD SIDr ADD ... 0 ADD SIDr ADD 0*/
					rsspLnkInfos[*rsspLnkInfosLen].POST_RXDADA_K[maxLostIdx] = F_ADD_WL(F_ADD_WL(
						PrecRxdata(locSafeParam->Sinit[channelIdx], locSafeParam->Sid[channelIdx], channelIdx, maxLostIdx),
						channelIdx, locSafeParam->Sid[channelIdx]), channelIdx, 0u);
				}

				(*rsspLnkInfosLen)++; /*增加行数*/
			}
		}
	}
}

/*
*功能描述:	从RSSP链接信息表中获取设备安全参数
*输入参数:	const UINT32 locHlhtId 本方互联互通ID
*输入出参:	SheetInfo* paramSheet RSSP链接信息表
*           SafeParam* safeParam 安全参数
*输出参数:	无
*返回值:	UINT8 是否检查成功.0:不成功,1:成功
*/
static UINT8 GetDevSafeParamFromSheet(const UINT32 locHlhtId, SheetInfo* paramSheet, SafeParam* safeParam)
{
	UINT8 retVal = 0u; /*返回值*/
	UINT32 rowLen = 0u; /*行长度*/
	UINT32 rowIdx = 0u; /*行下标*/
	UINT32 devId = 0u; /*设备ID*/
	UINT16 curChannel = 0u; /*当前通道*/
	UINT8 channelFind[2] = { 0 }; /*通道找到标志*/
	UINT8 isBreak = 0u; /*是否退出*/

	if ((NULL != paramSheet) && (NULL != safeParam) && (0u < paramSheet->Len) && (0u < paramSheet->RowCount) && (0u == paramSheet->Len % paramSheet->RowCount)) /*参数检查*/
	{
		rowLen = paramSheet->Len / paramSheet->RowCount;
		for (rowIdx = 0u; rowIdx < paramSheet->RowCount; rowIdx++) /*遍历设备IP地址*/
		{
			devId = LongFromChar(&paramSheet->Data[paramSheet->StartIdx + (rowIdx * rowLen) + 2]);
			if (devId == locHlhtId)
			{
				safeParam->DevId = devId;
				curChannel = ShortFromChar(&paramSheet->Data[paramSheet->StartIdx + (rowIdx * rowLen) + 10]);
				if ((1u == curChannel) || (2u == curChannel)) /*通道值合法*/
				{
					if (1u == curChannel)
					{
						curChannel = 0u;
					}
					else
					{
						curChannel = 1u;
					}
					safeParam->Sid[curChannel] = LongFromChar(&paramSheet->Data[paramSheet->StartIdx + (rowIdx * rowLen) + 12]);
					safeParam->Sinit[curChannel] = LongFromChar(&paramSheet->Data[paramSheet->StartIdx + (rowIdx * rowLen) + 16]);
					safeParam->Dataver[curChannel] = LongFromChar(&paramSheet->Data[paramSheet->StartIdx + (rowIdx * rowLen) + 21]);
					safeParam->Addr = ShortFromChar(&paramSheet->Data[paramSheet->StartIdx + (rowIdx * rowLen) + 25]);
					channelFind[curChannel] = 1u;
					if ((1u == channelFind[0]) && (1u == channelFind[1])) /*俩个通道都找到*/
					{
						retVal = 1u;
						isBreak = 1u;
					}
				}
				else
				{
					isBreak = 1u;
				}
			}
			if (1u == isBreak)
			{
				break;
			}
		}
	}

	return retVal;
}

/*
*功能描述:	从内外网RSSP链接信息表中获取设备安全参数
*输入参数:	const UINT32 locHlhtId 本方互联互通ID
*输入出参:	SheetInfo* outParamSheet 外网RSSP链接信息表
*			SheetInfo* inParamSheet 内网RSSP链接信息表
*           SafeParam* safeParam 安全参数
*输出参数:	无
*返回值:	UINT8 是否获取成功.0:不成功,1:成功
*/
static UINT8 GetDevSafeParam(const UINT32 locHlhtId, SheetInfo* outParamSheet, SheetInfo* inParamSheet, SafeParam* safeParam)
{
	UINT8 retVal = 0u;

	if ((NULL != outParamSheet) && (NULL != inParamSheet) && (NULL != safeParam)) /*参数检查*/
	{
		retVal = GetDevSafeParamFromSheet(locHlhtId, outParamSheet, safeParam);
		if (0u == retVal) /*未找到*/
		{
			retVal = GetDevSafeParamFromSheet(locHlhtId, inParamSheet, safeParam);
		}
	}

	return retVal;
}

/*
*功能描述:	获取通信的设备类型列表
*输入参数:	const UINT16 locTypeId 本方类型ID
*			SheetInfo* commSheet 设备通信协议表
*			UINT16 maxCommTypesLen 最大通信设备类型数组长度
*输入出参:	UINT16* commTypesLen 设备类型数组长度
*			UINT8 commTypes[] 设备类型数组
*输出参数:	无
*返回值:	无
*/
static void GetCommTypes(const UINT16 locTypeId, SheetInfo* commSheet, UINT16* commTypesLen, UINT8 commTypes[], const UINT16 maxCommTypesLen)
{
	UINT8 locType = 0u; /*本方类型*/
	UINT32 rowLen = 0u; /*行长度*/
	UINT32 rowIdx = 0u; /*行下标*/
	UINT8 locDevType = 0u; /*本方设备类型*/
	UINT8 oppDevType = 0u; /*对方设备类型*/
	UINT8 ptclType = 0u; /*协议类型*/
	UINT32 i = 0u; /*用于循环*/

	if ((NULL != commSheet) && (NULL != commTypesLen) && (NULL != commTypes) && (0u < commSheet->Len) && (0u < commSheet->RowCount) && (0u == commSheet->Len % commSheet->RowCount)) /*检查外部-设备IP地址-长度*/
	{
		locType = (UINT8)((locTypeId >> 8) & 0xFF);
		rowLen = commSheet->Len / commSheet->RowCount;
		for (rowIdx = 0u; rowIdx < commSheet->RowCount; rowIdx++) /*遍历设备IP地址表*/
		{
			locDevType = commSheet->Data[commSheet->StartIdx + (rowIdx * rowLen)];
			oppDevType = commSheet->Data[commSheet->StartIdx + (rowIdx * rowLen) + 1];
			ptclType = commSheet->Data[commSheet->StartIdx + (rowIdx * rowLen) + 2];
			if ((locDevType == locType) && (4u == ptclType)) /*是本方设备类型且是RSSPI*/
			{
				for (i = 0; i < (*commTypesLen); i++)
				{
					if (commTypes[i] == oppDevType)
					{
						break;
					}
				}
				if ((i == (*commTypesLen)) && ((*commTypesLen) < maxCommTypesLen)) /*不重复,添加*/
				{
					commTypes[*commTypesLen] = oppDevType;
					(*commTypesLen)++;
				}
			}
		}
	}
}

/*
*功能描述:	根据对方设备类型从设备IP地址表获取互联互通ID列表
*输入参数:	const UINT8 oppDevType 对方设备类型
*			SheetInfo* paramSheet RSSP链接信息表
*			const UINT16 maxHlhtIdsLen 最大互联互通ID列表长度
*			DSU_RSSP_INFO_STRU_WL *dsuRsspInfo RSSPI配置信息
*输入出参:	UINT16* hlhtIdsLen 互联互通ID列表长度
*			UINT32 hlhtIds[] 互联互通ID列表
*输出参数:	无
*返回值:	无
*/
static void GetHlhtIds(const UINT8 oppDevType, SheetInfo* paramSheet, UINT16* hlhtIdsLen, UINT32 hlhtIds[], const UINT16 maxHlhtIdsLen, DSU_RSSP_INFO_STRU_WL* dsuRsspInfo)
{
	UINT32 rowLen = 0u; /*行长度*/
	UINT32 rowIdx = 0u; /*行下标*/
	UINT16 devTypeId = 0u; /*设备类型ID*/
	UINT32 hlhtId = 0u; /*互联互通ID*/
	UINT32 i = 0u; /*用于循环*/

	if ((NULL != paramSheet) && (NULL != hlhtIdsLen) && (NULL != hlhtIds) && (0u < paramSheet->Len) && (0u < paramSheet->RowCount) && (0u == paramSheet->Len % paramSheet->RowCount)) /*检查设备IP地址表*/
	{
		rowLen = paramSheet->Len / paramSheet->RowCount;
		for (rowIdx = 0u; rowIdx < paramSheet->RowCount; rowIdx++) /*遍历设备IP地址表*/
		{
			hlhtId = LongFromChar(&paramSheet->Data[paramSheet->StartIdx + (rowIdx * rowLen) + 2]);
			devTypeId = DquToDevTypeId(hlhtId, dsuRsspInfo);
			if (((UINT8)((devTypeId >> 8) & 0xFF) == oppDevType)) /*是目的设备类型*/
			{
				for (i = 0; i < (*hlhtIdsLen); i++)
				{
					if (hlhtIds[i] == hlhtId)
					{
						break;
					}
				}
				if ((i == (*hlhtIdsLen)) && ((*hlhtIdsLen) < maxHlhtIdsLen)) /*不重复,添加*/
				{
					hlhtIds[*hlhtIdsLen] = hlhtId;
					(*hlhtIdsLen)++;
				}
			}
		}
	}
}

/*
*功能描述:	根据本方设备计算通信的对方互联互通ID列表
*输入参数:	const UINT16 locTypeId 本方设备ID
*			const UINT32 locHlhtId, 本方HLHT ID
*			SheetInfo* outParamSheet 外网RSSP链接信息表
*			SheetInfo* inParamSheet 内网RSSP链接信息表
*			const UINT32 inDataLen 内网数据长度
*			UINT8 inData[] 内网数据
*			const UINT32 outDataLen 外网数据长度
*			UINT8 outData[] 外网数据
*			const UINT16 maxOppHlhtIdsLen 最大对方互联互通ID数组长度
*			DSU_RSSP_INFO_STRU_WL *dsuRsspInfo RSSPI配置信息
*输入出参:	SafeParam* locSafeParam 本方安全参数
*			UINT16* oppHlhtIdsLen 对方互联互通ID数组长度
*			UINT32 oppHlhtIds[] 对方互联互通ID数组
*输出参数:	无
*返回值:	UINT8 是否计算成功.0:不成功,1:成功
*/
static UINT8 CalcOppHlhtIds(const UINT16 locTypeId, const UINT32 locHlhtId, SheetInfo* outParamSheet, SheetInfo* inParamSheet, const UINT32 inDataLen, UINT8 inData[], const UINT32 outDataLen, UINT8 outData[], SafeParam* locSafeParam, UINT16* oppHlhtIdsLen, UINT32 oppHlhtIds[], const UINT16 maxOppHlhtIdsLen, DSU_RSSP_INFO_STRU_WL* dsuRsspInfo)
{
	UINT8 retVal = 0u; /*返回值*/
	UINT8 hasParam = 0u; /*有本方参数*/
	SheetInfo outCommSheet = { 0 }; /*外网设备通信协议表*/
	SheetInfo inCommSheet = { 0 }; /*内网网设备通信协议表*/
	UINT16 commTypesLen = 0u; /*通信的设备类型数组长度*/
	UINT8 commTypes[COMM_DEV_TYPE_MAX_NUM] = { 0 }; /*通信的设备类型数组*/
	UINT32 i = 0u; /*用于循环*/

	hasParam = GetDevSafeParam(locHlhtId, outParamSheet, inParamSheet, locSafeParam);
	if (1u == hasParam) /*有本方安全参数*/
	{
		InitSheet(&outCommSheet, outDataLen, outData, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_COMM_PROTCL_ID);
		InitSheet(&inCommSheet, inDataLen, inData, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_COMM_PROTCL_ID);
		GetCommTypes(locTypeId, &outCommSheet, &commTypesLen, commTypes, COMM_DEV_TYPE_MAX_NUM);
		GetCommTypes(locTypeId, &inCommSheet, &commTypesLen, commTypes, COMM_DEV_TYPE_MAX_NUM);
		if (0u < commTypesLen) /*有通信设备类型*/
		{
			for (i = 0u; i < commTypesLen; i++)
			{
				GetHlhtIds(commTypes[i], outParamSheet, oppHlhtIdsLen, oppHlhtIds, maxOppHlhtIdsLen, dsuRsspInfo);
				GetHlhtIds(commTypes[i], inParamSheet, oppHlhtIdsLen, oppHlhtIds, maxOppHlhtIdsLen, dsuRsspInfo);
			}
		}
		retVal = 1u;
	}

	return retVal;
}

/*
*功能描述:	合并RSSP链接信息
*输入参数:	const UINT16 oppHlhtIdsLen 对方互联互通ID数组长度
*			UINT32 oppHlhtIds[] 对方互联互通ID数组
*			SheetInfo* outParamSheet 外网RSSP链接信息
*			SheetInfo* inParamSheet 内网RSSP链接信息
*			SafeParam* locSafeParam 本方安全参数
*			const UINT16 maxLnkInfosLen 最大RSSP链接信息数组长度
*输入出参:	DSU_RSSP_LNK_INFO_STRU_WL lnkInfos[] RSSP链接信息数组
*输出参数:	无
*返回值:	无
*/
static void MergeRsspiParam(const UINT16 oppHlhtIdsLen, UINT32 oppHlhtIds[], SheetInfo* outParamSheet, SheetInfo* inParamSheet, SafeParam* locSafeParam, DSU_RSSP_LNK_INFO_STRU_WL lnkInfos[], const UINT16 maxLnkInfosLen)
{
	UINT16 i = 0u; /*用于循环*/
	UINT8 hasParam = 0u; /*是否有参数*/
	UINT16 curLnkInfosLen = 0u; /*当前链路长度*/
	SafeParam oppSafeParam = { 0 }; /*对方安全参数*/

	if (NULL != oppHlhtIds) /*参数检查*/
	{
		for (i = 0u; i < oppHlhtIdsLen; i++)
		{
			hasParam = GetDevSafeParam(oppHlhtIds[i], outParamSheet, inParamSheet, &oppSafeParam);
			if (1u == hasParam)
			{
				GenerateParamsRow(locSafeParam, &oppSafeParam, &curLnkInfosLen, lnkInfos, maxLnkInfosLen);
				GenerateParamsRow(&oppSafeParam, locSafeParam, &curLnkInfosLen, lnkInfos, maxLnkInfosLen);
			}
		}
	}
}

/*
*函数名:	DsuRsspInitWl
*功能描述:	RSSPI协议查询函数初始化
*输入参数:	UINT8 *pFsName	FS文件
*			UINT16 localTypeId	本方类型ID
*			UINT16 localLogicId	本方逻辑ID
*输入出参:	DSU_RSSP_INFO_STRU_WL *pDsuRsspInfo	RSSPI配置信息
*输出参数:	无
*返回值:	1:成功;0失败
*/
UINT8 DsuRsspInitWl(UINT8* pFsName, UINT32 inDataLen, UINT8 inData[], UINT16 localTypeId, UINT16 localLogicId, DSU_RSSP_INFO_STRU_WL* pDsuRsspInfo)
{
	UINT8 rtn = 0;	/*返回值*/
	UINT8 readRtn = 0;	/*读取FS返回值*/
	UINT8* pData = NULL;	/*读出的数据*/
	UINT32 dataLen = 0;	/*数据长度*/
	UINT16 rsspILinkLen = 0;	/*跟本设备通信的协议通信关系长度*/
	UINT32 mallocLen = 0;	/*分配空间大小*/
	UINT32 offset = 0;	/*数据偏移量*/
	UINT32 i = 0;	/*用于循环*/
	UINT32 j = 0;	/*用于循环*/
	UINT32 localId = 0;	/*本方唯一ID*/
	UINT32 locId = 0;	/*配置的本方唯一ID*/
	UINT32 oppId = 0;	/*配置的对方唯一ID*/

	SheetInfo inParamSheet = { 0 };
	SheetInfo outParamSheet = { 0 };
	UINT8 paramCheckRtn = 0u;
	SafeParam locSafeParam = { 0 };
	UINT16 oppHlhtIdLen = 0u;
	UINT32 oppHlhtIds[COMM_DEV_MAX_NUM] = { 0 };
	UINT8 calcOppHlhtIdsRtn = 0u;

	SheetInfo configSheet = { 0 }; /*设备通信属性sheet页*/
	UINT32 rowLen = 0u; /*行长度*/
	UINT8 hasSame = 0u; /*有相同参*/
	UINT16 inLocDevType = 0u; /*内部本方设备类型*/
	UINT16 inOppDevType = 0u; /*内部对方设备类型*/
	UINT16 rowCount = 0u; /*增加行数*/

	if (NULL != pDsuRsspInfo)
	{
#ifdef RSSP_NO_HLHT
		localId = localTypeId;
#else
		/*读取ZC编号对照表*/
		readRtn = DquInitHlhtId(pFsName, IDMAP_ZC_ID, ID_MAP_SIZE, DEV_HLHTID_NUM_ONE, &pDsuRsspInfo->ZcRowNum, &pDsuRsspInfo->ZcHlhtids);
		if (1 == readRtn)
		{
			/*读取CI编号对照表*/
			readRtn = DquInitHlhtId(pFsName, IDMAP_CI_ID, ID_MAP_SIZE, DEV_HLHTID_NUM_ONE, &pDsuRsspInfo->CiRowNum, &pDsuRsspInfo->CiHlhtids);
			if (1 == readRtn)
			{
				/*读取ATS编号对照表*/
				readRtn = DquInitHlhtId(pFsName, IDMAP_ATS_ID, ID_MAP_SIZE, DEV_HLHTID_NUM_ONE, &pDsuRsspInfo->AtsRowNum, &pDsuRsspInfo->AtsHlhtids);
				if (1 == readRtn)
				{
					/*读取VOBC编号对照表*/
					readRtn = DquInitHlhtId(pFsName, IDMAP_VOBC_ID, VOBC_ID_MAP_SIZE, DEV_HLHTID_NUM_TWO, &pDsuRsspInfo->VobcRowNum, &pDsuRsspInfo->VobcHlhtids);
					if (1 == readRtn)
					{
						/*读取DSU编号对照表*/
						readRtn = DquInitHlhtId(pFsName, IDMAP_DSU_ID, ID_MAP_SIZE, DEV_HLHTID_NUM_ONE, &pDsuRsspInfo->DsuRowNum, &pDsuRsspInfo->DsuHlhtids);
						if (1 == readRtn)
						{
							/*读取AOM编号对照表*/
							readRtn = DquInitHlhtId(pFsName, IDMAP_AOMSEC_ID, VOBC_ID_MAP_SIZE, DEV_HLHTID_NUM_TWO, &pDsuRsspInfo->AomRowNum, &pDsuRsspInfo->AomHlhtids);
							if (1 == readRtn)
							{
								/*读取TDT编号对照表*/
								readRtn = DquInitHlhtId(pFsName, IDMAP_TDT_ID, ID_MAP_SIZE, DEV_HLHTID_NUM_ONE, &pDsuRsspInfo->TdtRowNum, &pDsuRsspInfo->TdtHlhtids);
							}
						}
					}
				}
			}
		}
		localId = DquTypeIdToHlhtId(localTypeId, localLogicId, pDsuRsspInfo);	/*本方RSSPI唯一ID*/
#endif
		paramCheckRtn = CheckInOutParamSheet(inDataLen, inData, g_DataBufLen, g_pDataBuf, &inParamSheet, &outParamSheet);
		if (1u == paramCheckRtn)
		{
			calcOppHlhtIdsRtn = CalcOppHlhtIds(localTypeId, localId, &outParamSheet, &inParamSheet, inDataLen, inData, g_DataBufLen, g_pDataBuf, &locSafeParam, &oppHlhtIdLen, oppHlhtIds, COMM_DEV_MAX_NUM, pDsuRsspInfo);
			if (1u == calcOppHlhtIdsRtn)
			{
				pDsuRsspInfo->RsspInfoStruLen = 4 * oppHlhtIdLen; /*与一个对方设备需要4个参数*/
				pDsuRsspInfo->pRsspLnkInfoStru = (DSU_RSSP_LNK_INFO_STRU_WL*)malloc(sizeof(DSU_RSSP_LNK_INFO_STRU_WL) * pDsuRsspInfo->RsspInfoStruLen); /*为RSSP链接信息表申请空间*/
				if (NULL != pDsuRsspInfo->pRsspLnkInfoStru)
				{
					MergeRsspiParam(oppHlhtIdLen, oppHlhtIds, &outParamSheet, &inParamSheet, &locSafeParam, pDsuRsspInfo->pRsspLnkInfoStru, pDsuRsspInfo->RsspInfoStruLen);

					InitSheet(&configSheet, inDataLen, inData, DSU_DATA_PROTCL_TYPE, DSU_DATA_PROTCL_COMM_INFO_ID);
					/*读取设备通信属性表*/
					readRtn = dquGetConfigData((CHAR*)pFsName, DSU_DATA_PROTCL_TYPE, DSU_DATA_PROTCL_COMM_INFO_ID, &pData, &dataLen);
					if ((1 == readRtn) && (DSU_DEVICE_COMM_INFO_STRU_SIZE_WL <= dataLen))
					{
						pDsuRsspInfo->DeviceCommStruLen = (UINT16)(dataLen / DSU_DEVICE_COMM_INFO_STRU_SIZE_WL);  /*计算表格中数据的个数*/
						mallocLen = sizeof(DSU_DEVICE_COMM_INFO_STRU_WL) * (pDsuRsspInfo->DeviceCommStruLen + configSheet.RowCount);	/*需分配空间大小*/
						pDsuRsspInfo->pDevCommInfoStru = (DSU_DEVICE_COMM_INFO_STRU_WL*)malloc(mallocLen); /*为设备通信属性表申请空间*/
						if (NULL != pDsuRsspInfo->pDevCommInfoStru)
						{
							CommonMemSet(pDsuRsspInfo->pDevCommInfoStru, mallocLen, 0, mallocLen);

							offset = 0;	/*再次使用清零*/
							for (i = 0; i < pDsuRsspInfo->DeviceCommStruLen; i++)
							{
								/*2字节本方类型*/
								pDsuRsspInfo->pDevCommInfoStru[i].EmitterType = ShortFromChar(&pData[offset]);
								offset += 2;
								/*2字节对方类型*/
								pDsuRsspInfo->pDevCommInfoStru[i].ReceptorType = ShortFromChar(&pData[offset]);
								offset += 2;
								/*1字节最大丢包数*/
								pDsuRsspInfo->pDevCommInfoStru[i].MaxLost = pData[offset];
								offset++;
								/*1字节连续收不到数据周期数*/
								pDsuRsspInfo->pDevCommInfoStru[i].DLost = pData[offset];
								offset++;
								/*1字节等待回复SSR周期数*/
								pDsuRsspInfo->pDevCommInfoStru[i].WSsrOut = pData[offset];
								offset++;
								/*1字节定期同步周期数*/
								pDsuRsspInfo->pDevCommInfoStru[i].AntiDelayCount = pData[offset];
								offset++;
								/*1字节报文超时周期数*/
								pDsuRsspInfo->pDevCommInfoStru[i].TOut = pData[offset];
								offset++;
								/*1字节消息ID*/
								pDsuRsspInfo->pDevCommInfoStru[i].MsgId = pData[offset];
								offset++;
								/*1字节接口版本*/
								pDsuRsspInfo->pDevCommInfoStru[i].IftVer = pData[offset];
								offset++;

							}
							if (offset == dataLen)	/*设备通信属性表赋值成功*/
							{
								if ((0u < configSheet.Len) && (0 == configSheet.Len % configSheet.RowCount)) /*检查sheet长度*/
								{
									rowLen = configSheet.Len / configSheet.RowCount;
									if (DSU_DEVICE_COMM_INFO_STRU_SIZE_WL == rowLen) /*检查行长度*/
									{
										rowCount = pDsuRsspInfo->DeviceCommStruLen;
										for (i = 0; i < configSheet.RowCount; i++)
										{
											hasSame = 0u;
											inLocDevType = ShortFromChar(&configSheet.Data[configSheet.StartIdx + (i * rowLen)]);
											inOppDevType = ShortFromChar(&configSheet.Data[configSheet.StartIdx + (i * rowLen) + 2]);
											for (j = 0; j < pDsuRsspInfo->DeviceCommStruLen; j++)
											{
												if ((inLocDevType == pDsuRsspInfo->pDevCommInfoStru[j].EmitterType) && (inOppDevType == pDsuRsspInfo->pDevCommInfoStru[j].ReceptorType))
												{
													hasSame = 1u;
												}
											}
											if (0u == hasSame) /*无不同参,添加*/
											{
												pDsuRsspInfo->pDevCommInfoStru[rowCount].EmitterType = inLocDevType;
												pDsuRsspInfo->pDevCommInfoStru[rowCount].ReceptorType = inOppDevType;
												pDsuRsspInfo->pDevCommInfoStru[rowCount].MaxLost = configSheet.Data[configSheet.StartIdx + (i * rowLen) + 4];
												pDsuRsspInfo->pDevCommInfoStru[rowCount].DLost = configSheet.Data[configSheet.StartIdx + (i * rowLen) + 5];
												pDsuRsspInfo->pDevCommInfoStru[rowCount].WSsrOut = configSheet.Data[configSheet.StartIdx + (i * rowLen) + 6];
												pDsuRsspInfo->pDevCommInfoStru[rowCount].AntiDelayCount = configSheet.Data[configSheet.StartIdx + (i * rowLen) + 7];
												pDsuRsspInfo->pDevCommInfoStru[rowCount].TOut = configSheet.Data[configSheet.StartIdx + (i * rowLen) + 8];
												pDsuRsspInfo->pDevCommInfoStru[rowCount].MsgId = configSheet.Data[configSheet.StartIdx + (i * rowLen) + 9];
												pDsuRsspInfo->pDevCommInfoStru[rowCount].IftVer = configSheet.Data[configSheet.StartIdx + (i * rowLen) + 10];

												rowCount++;
											}
										}
										pDsuRsspInfo->DeviceCommStruLen = rowCount;
									}
								}

								for (i = 0; i < pDsuRsspInfo->RsspInfoStruLen; i++)
								{
									if (pDsuRsspInfo->pRsspLnkInfoStru[i].EmitterName == localId)	/*找到赋值退出循环*/
									{
										pDsuRsspInfo->LocEmitterName = pDsuRsspInfo->pRsspLnkInfoStru[i].EmitterName;
										pDsuRsspInfo->LocRsspiAddr = pDsuRsspInfo->pRsspLnkInfoStru[i].RsspAddr;
										break;
									}
								}

								rtn = 1;
							}
						}
					}
				}
			}
		}
	}

	return rtn;
}

/*
*函数名:	dsuSetCommInfo_WL
*功能描述:	根据本方对方设备类型设置作为接收方时配置参数
*输入参数:	UINT8 localType	本方类型
*			UINT8 DestType	对方类型
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	DSU_RSSP_LINK_RINFO_STRU_WL *pRsspLnkRInfo	接收方时配置参数
*返回值:	1:成功;0失败
*/
UINT8 dsuSetCommInfo_WL(DSU_RSSP_LINK_RINFO_STRU_WL *pRsspLnkRInfo,UINT8 localType, UINT8 DestType, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL)
{
	UINT16 i;/*循环用*/
	DSU_DEVICE_COMM_INFO_STRU_WL* pTempDevCommInfoStru; 
	UINT8 chReturnValue;
	UINT16 offset = 0U;
	pTempDevCommInfoStru=pDsuRsrInfoStru_WL->pDevCommInfoStru;
	for (i=0;i<pDsuRsrInfoStru_WL->DeviceCommStruLen;i++)
	{
		if ((localType==pTempDevCommInfoStru[offset].EmitterType)
			&&(DestType== pTempDevCommInfoStru[offset].ReceptorType))
		{
			/*根据输入的SSTy，找到所在的行*/
			pRsspLnkRInfo->MaxLost= pTempDevCommInfoStru[offset].MaxLost;
			pRsspLnkRInfo->DLost= pTempDevCommInfoStru[offset].DLost;
			pRsspLnkRInfo->WSsrOut= pTempDevCommInfoStru[offset].WSsrOut;
			pRsspLnkRInfo->AntiDelayCount= pTempDevCommInfoStru[offset].AntiDelayCount;
			pRsspLnkRInfo->TOut= pTempDevCommInfoStru[offset].TOut;
			
			chReturnValue=1;
			return chReturnValue;
		}
		else
		{
			/*本行不是要查找的数据，继续查找*/
		}
		offset++;
	}

	/*未找到对应通信关系的数据，关系不合法*/
	chReturnValue=0;
	return chReturnValue;
}

/*
*函数名:	dsuRsspGetDevTypeWl
*功能描述:	获取设备类型
*输入参数:	UINT16 DevName	设备名称
*输入出参:	无
*输出参数:	无
*返回值:	设备类型
*/
UINT8 dsuRsspGetDevTypeWl(UINT16 DevName)
{
	UINT8 devType;
	devType = (UINT8)(DevName >> 8);

	return devType;
}

/*
*函数名:	dsuRsspRGetLnkInfoWl
*功能描述:	用于查询RSSP层的链路相关信息（接收节点信息）的映射数据
*输入参数:	UINT16 RsspAddr	Rssp地址
*			UINT16 SrcAddr	Src地址
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	DSU_RSSP_LINK_RINFO_STRU_WL *pRsspLnkRInfo	接收方时配置参数
*返回值:	1:成功;0失败
*/
UINT8 dsuRsspRGetLnkInfoWl(UINT16 RsspAddr, UINT16 SrcAddr, DSU_RSSP_LINK_RINFO_STRU_WL* pRsspLnkRInfo, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL)
{
	UINT8 chReturnValue = 0; /*函数返回用*/
	UINT16 i = 0; /*循环用*/
	DSU_RSSP_LNK_INFO_STRU_WL* pTempRsspLnkInfoStru = NULL;
	UINT16 pTempRsspLnkInfoStruLen = 0;
	UINT8 ChannelCount = 0;
	UINT16 CurrentChannel = 0;
	UINT8 LocalDevType = 0;
	UINT8 DestDevType = 0;
	UINT16 offset = 0U;
	/*找到的Channel数为0*/
	ChannelCount = 0;

	if ((NULL != pRsspLnkRInfo)&&(NULL!= pDsuRsrInfoStru_WL))
	{
		pTempRsspLnkInfoStru = pDsuRsrInfoStru_WL->pRsspLnkInfoStru;
		pTempRsspLnkInfoStruLen = pDsuRsrInfoStru_WL->RsspInfoStruLen;

		/*循环查找*/
		for (i = 0; i < pTempRsspLnkInfoStruLen; i++)
		{
			if ((pTempRsspLnkInfoStru[offset].RsspAddr == RsspAddr)
				&& (pTempRsspLnkInfoStru[offset].SseAddr == SrcAddr))
			{
				/*已经查找到目标*/
				CurrentChannel = pTempRsspLnkInfoStru[offset].ChannelNumber - 1;
				ChannelCount++;

				pRsspLnkRInfo->LocalLogId = 0xff;
				pRsspLnkRInfo->DestName = pTempRsspLnkInfoStru[offset].ReceptorName;
				pRsspLnkRInfo->DestLogId = 0xff;
				pRsspLnkRInfo->SseAddr = pTempRsspLnkInfoStru[offset].SseAddr;
				pRsspLnkRInfo->SID_NUMBER_E[CurrentChannel] = pTempRsspLnkInfoStru[offset].SID_NUMBER_E;
				pRsspLnkRInfo->SID_NUMBER_R[CurrentChannel] = pTempRsspLnkInfoStru[offset].SID_NUMBER_R;
				pRsspLnkRInfo->PREC_FIRSTSINIT[CurrentChannel] = pTempRsspLnkInfoStru[offset].PREC_FIRSTSINIT;
				CommonMemCpy(pRsspLnkRInfo->POST_RXDADA_K[CurrentChannel], sizeof(pRsspLnkRInfo->POST_RXDADA_K[CurrentChannel]), pTempRsspLnkInfoStru[offset].POST_RXDADA_K, sizeof(pRsspLnkRInfo->POST_RXDADA_K[CurrentChannel]));
				CommonMemCpy(pRsspLnkRInfo->PREC_SINIT_K[CurrentChannel], sizeof(pRsspLnkRInfo->PREC_SINIT_K[CurrentChannel]), pTempRsspLnkInfoStru[offset].PREC_SINIT_K, sizeof(pRsspLnkRInfo->PREC_SINIT_K[CurrentChannel]));
#ifdef RSSP_NO_HLHT
				LocalDevType = (pTempRsspLnkInfoStru->EmitterName >> 8) & 0xff;
				DestDevType = (pTempRsspLnkInfoStru->ReceptorName >> 8) & 0xff;
#else
				LocalDevType = DquToDevType(pTempRsspLnkInfoStru[offset].EmitterName, pDsuRsrInfoStru_WL);
				DestDevType = DquToDevType(pTempRsspLnkInfoStru[offset].ReceptorName, pDsuRsrInfoStru_WL);
#endif
				chReturnValue = dsuSetCommInfo_WL(pRsspLnkRInfo, LocalDevType, DestDevType, pDsuRsrInfoStru_WL);
				if (chReturnValue == 0)
				{
					break;
				}

				if (ChannelCount >= 2)
				{
					/*两个通道信息都已找到，函数返回*/
					chReturnValue = 1;
					break;
				}
				else
				{
					/*还未找到两个通道，继续查找*/
				}
			}
			else
			{
				/*当前项非目标项，继续查找*/
			}
			offset++;
		}

	}
	
	return chReturnValue;
}

/*
*函数名:	dsuRsspSGetLnkInfoWl
*功能描述:	用于查询RSSP层的链路相关信息（发送节点信息）的映射数据
*输入参数:	UINT16 RsspAddr	源地址
*			UINT16 BsdAddr	目的地址
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	DSU_RSSP_LINK_SINFO_STRU_WL* pRsspLnkSInfo,	发送方时配置参数
*返回值:	1:成功;0失败
*/
UINT8 dsuRsspSGetLnkInfoWl(UINT16 RsspAddr, UINT16 BsdAddr, DSU_RSSP_LINK_SINFO_STRU_WL* pRsspLnkSInfo, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL)
{
	UINT8 chReturnValue = 0; /*函数返回用*/
	UINT16 i = 0; /*循环用*/
	DSU_RSSP_LNK_INFO_STRU_WL* pTempRsspLnkInfoStru = NULL;
	UINT16 pTempRsspLnkInfoStruLen = 0;
	UINT8 ChannelCount = 0;
	UINT16 CurrentChannel = 0;
	UINT16 offset = 0U;
	if ((NULL != pRsspLnkSInfo) && (NULL != pDsuRsrInfoStru_WL))
	{
		pTempRsspLnkInfoStru = pDsuRsrInfoStru_WL->pRsspLnkInfoStru;
		pTempRsspLnkInfoStruLen = pDsuRsrInfoStru_WL->RsspInfoStruLen;

		/*找到的Channel数为0*/
		ChannelCount = 0;

		/*循环查找*/
		for (i = 0; i < pTempRsspLnkInfoStruLen; i++)
		{
			if ((pTempRsspLnkInfoStru[offset].RsspAddr == RsspAddr)
				&& (pTempRsspLnkInfoStru[offset].BsdAddr == BsdAddr))
			{
				/*已经查找到目标*/
				CurrentChannel = pTempRsspLnkInfoStru[offset].ChannelNumber - 1;
				ChannelCount++;

				pRsspLnkSInfo->LocalLogId = 0xff;
				pRsspLnkSInfo->DestName = pTempRsspLnkInfoStru[offset].ReceptorName;
				pRsspLnkSInfo->DestLogId = 0xff;
				pRsspLnkSInfo->SseAddr = pTempRsspLnkInfoStru[offset].SseAddr;
				pRsspLnkSInfo->SID_NUMBER_E[CurrentChannel] = pTempRsspLnkInfoStru[offset].SID_NUMBER_E;
				pRsspLnkSInfo->SID_NUMBER_R[CurrentChannel] = pTempRsspLnkInfoStru[offset].SID_NUMBER_R;
				pRsspLnkSInfo->SINIT_NUMBER[CurrentChannel] = pTempRsspLnkInfoStru[offset].SINIT_NUMBER;
				pRsspLnkSInfo->DATANUM[CurrentChannel] = pTempRsspLnkInfoStru[offset].DATANUM;
				pRsspLnkSInfo->DATAVER_1_Number[CurrentChannel] = pTempRsspLnkInfoStru[offset].DATAVER_1_Number;
				pRsspLnkSInfo->DATANUM[CurrentChannel] = pTempRsspLnkInfoStru[offset].DATANUM;

				if (ChannelCount >= 2)
				{
					chReturnValue = 1;
					break;
				}
				else
				{
					/*还未找到两个通道，继续查找*/
				}
			}
			else
			{
				/*当前项非目标项，继续查找*/
			}
			offset++;
		}
	}
	
	return chReturnValue;
}

/*
*函数名:	dsuRsspRGetAddrWl
*功能描述:	根据本方设备名称和对方设备名称，查询RSSP链路表，获取源地址、目标地址
*输入参数:	UINT32 destDevName	目的设备名称
*			UINT32 locDevName	本方设备名称
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	UINT16* rsspSrcAddr	源地址
*			UINT16* rsspDestAddr	目的地址
*返回值:	1:成功;0失败
*/
UINT8 dsuRsspRGetAddrWl(UINT32 destDevName,UINT32 locDevName ,UINT16* rsspSrcAddr,UINT16* rsspDestAddr, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL)
{
	UINT8 ret = 0;
	UINT16 i; /*循环用*/
	
	for (i = 0;i < pDsuRsrInfoStru_WL->RsspInfoStruLen;i++)
	{
		if ((pDsuRsrInfoStru_WL->pRsspLnkInfoStru[i].EmitterName == locDevName) && (pDsuRsrInfoStru_WL->pRsspLnkInfoStru[i].ReceptorName == destDevName))
		{
			*rsspSrcAddr = pDsuRsrInfoStru_WL->pRsspLnkInfoStru[i].RsspAddr;
			*rsspDestAddr = pDsuRsrInfoStru_WL->pRsspLnkInfoStru[i].SseAddr;
			break;
		}
		else
		{
		}
	}
	
	if (i == pDsuRsrInfoStru_WL->RsspInfoStruLen)
	{
		return ret;
	}
	ret = 1;
	return ret;
}

/*
*函数名:	dsuRsspRGetName_WL
*功能描述:	根据源地址、目标地址，查询RSSP链路表，获取本方设备名称和对方设备名称
*输入参数:	UINT16 rsspSrcAddr	源地址
*			UINT16 rsspDestAddr	目的地址
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	UINT32* destDevName	目的设备名称
*返回值:	1:成功;0失败
*/
UINT8 dsuRsspRGetName_WL(UINT16 rsspSrcAddr,UINT16 rsspDestAddr,UINT32* destDevName, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL)
{
	UINT8 ret = 0;

	UINT16 i; /*循环用*/
	
	if ((NULL != destDevName) && (NULL != pDsuRsrInfoStru_WL) && (NULL != pDsuRsrInfoStru_WL->pRsspLnkInfoStru))
	{
		for (i = 0; i < pDsuRsrInfoStru_WL->RsspInfoStruLen; i++)
		{
			if ((pDsuRsrInfoStru_WL->pRsspLnkInfoStru[i].RsspAddr == rsspSrcAddr) && (pDsuRsrInfoStru_WL->pRsspLnkInfoStru[i].SseAddr == rsspDestAddr))
			{
				*destDevName = pDsuRsrInfoStru_WL->pRsspLnkInfoStru[i].EmitterName;
				break;
			}
			else
			{
			}
		}

		if (i == pDsuRsrInfoStru_WL->RsspInfoStruLen)
		{
			return ret;
		}
	}

	ret = 1;
	return ret;
}
#ifndef RSSP_NO_HLHT
/*
*函数名:	DquInitHlhtId
*功能描述:	初始化编号对照表
*输入参数:	UINT8 *pFsName	文件名称
*			UINT8 sheetId	sheet页ID
*			UINT8 colUseLen	sheet列所占字节长度
*			UINT8 hlhtIdNum	一设备对应的hlhtid数量
*输入出参:	无
*输出参数:	UINT16 *pRowNum	行数量
*			DSU_DEV_HLHTID **pDsuDevHlhtids	hlhtid值列表
*返回值:	1:成功;0失败
*/
static UINT8 DquInitHlhtId(UINT8* pFsName, UINT8 sheetId, UINT8 colUseLen, UINT8 hlhtIdNum, UINT16* pRowNum, DSU_DEV_HLHTID** pDsuDevHlhtids)
{
	UINT8 rtn = 0;	/*返回值*/
	UINT8* pData = NULL;	/*读出的数据*/
	UINT32 dataLen = 0;	/*数据长度*/
	UINT32 mallocLen = 0;	/*分配空间大小*/
	UINT32 offset = 0;	/*数据偏移量*/
	UINT32 i = 0;	/*用于循环*/
	UINT32 j = 0;	/*用于循环*/

	if ((NULL != pFsName) && (0 < sheetId) && (5 < colUseLen) && (0 < hlhtIdNum) && (NULL != pRowNum) && (NULL != pDsuDevHlhtids))	/*输入参非空*/
	{
		dquGetConfigData((CHAR*)pFsName, IDMAP_DATA_TYPE, sheetId, &pData, &dataLen);
		if (0 < dataLen)	/*有数据*/
		{
			*pRowNum = (UINT16)(dataLen / colUseLen);  /*计算表格中数据的个数*/
			mallocLen = sizeof(DSU_DEV_HLHTID) * (*pRowNum);	/*需分配空间大小*/
			*pDsuDevHlhtids = (DSU_DEV_HLHTID*)malloc(mallocLen); /*为设备通信属性表申请空间*/
			if (NULL != (*pDsuDevHlhtids))
			{
				CommonMemSet(*pDsuDevHlhtids, mallocLen, 0, mallocLen);

				for (i = 0; i < *pRowNum; i++)	/*遍历行*/
				{
					/*2字节设备ID*/
					(*pDsuDevHlhtids)[i].DevId = ShortFromChar(&pData[offset]);
					offset += 2;
					/*n个4字节HLHTID*/
					(*pDsuDevHlhtids)[i].HlhtIds = (UINT32*)malloc(sizeof(UINT32) * hlhtIdNum);	/*分配HLHTIDs空间*/
					for (j = 0; j < hlhtIdNum; j++)	/*遍历设置HlhtIds*/
					{
						(*pDsuDevHlhtids)[i].HlhtIds[j] = LongFromChar(&pData[offset]);
						offset += 4;
					}
				}
				if (offset == dataLen)	/*编号对照表赋值成功*/
				{
					rtn = 1;
				}
			}
		}
		else /*数据长度为0*/
		{
			rtn = 1;
		}
	}

	return rtn;
}

/*
*函数名:	DquGetHlhtId
*功能描述:	根据设备ID,logid,对应表获取hlhtid
*输入参数:	UINT8 type	类型
*			UINT8 id	设备ID
*			UINT16 logicId	类型是车，	逻辑ID：1:1端 2：2端
*							类型是其他，逻辑ID：0
*			UINT16 rowNum	行数
*			const DSU_DEV_HLHTID *pDevHlhtids	对应编号对照表信息
*输入出参:	无
*输出参数:	无
*返回值:	对应的HLHTID
*/
static UINT32 DquGetHlhtId(UINT8 type, UINT8 id, UINT16 logicId, UINT16 rowNum, const DSU_DEV_HLHTID* pDevHlhtids)
{
	UINT32 hlhtId = 0;	/*返回值*/
	UINT32 i = 0;	/*用于循环*/

	if ((NULL != pDevHlhtids) && (NULL != pDevHlhtids->HlhtIds) 
		&& (((0u == type) && ((0u == logicId)|| (1u == logicId)))
		|| ((1u == type) && ((0u == logicId)|| (1u == logicId)|| (2u == logicId)))))
	{
		if (0u < logicId)	/*0:地面或冗余车,1:一端,2:二端*/
		{
			logicId -= 1;
		}
		for (i = 0u; i < rowNum; i++)
		{
			if (pDevHlhtids[i].DevId == id)	/*找到ID退出遍历*/
			{
				hlhtId = pDevHlhtids[i].HlhtIds[logicId];
				break;
			}
		}
	}
	return hlhtId;
}

/*
*函数名:	DquGetDevId
*功能描述:	获取设备ID
*输入参数:	UINT32 hlhtId	HLHTID
*			UINT16 rowNum	行数
*			const DSU_DEV_HLHTID *pDevHlhtids	对应编号对照表信息
*			UINT8 hlhtIdNum	一设备对应的hlhtid数量
*输入出参:	无
*输出参数:	无
*返回值:	对应的ID
*/
static UINT8 DquGetDevId(UINT32 hlhtId, UINT16 rowNum, const DSU_DEV_HLHTID *pDevHlhtids, UINT8 hlhtIdNum)
{
	UINT8 id = 0;	/*设备ID*/
	UINT32 i = 0;	/*用于循环*/
	UINT32 j = 0;	/*用于循环*/
	UINT8 isBreak = 0;	/*是否退出循环*/

	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < hlhtIdNum; j++)
		{
			if (pDevHlhtids[i].HlhtIds[j] == hlhtId)	/*找到HLHTID退出遍历*/
			{
				id = (UINT8)pDevHlhtids[i].DevId;	/*设备的ID是一字节的*/
				isBreak = 1;
				break;
			}
		}
		if (1 == isBreak)	/*找到退出循环*/
		{
			break;
		}
	}

	return id;
}

/*
*函数名:	DquGetDevHlhtId
*功能描述:	根据设备类型,设备ID,逻辑ID从编号对照表中获取HLHT ID
*输入参数:	UINT8 devType	设备类型
*			UINT8 devId		设备ID
*			UINT16 logicId	逻辑ID
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	无
*返回值:	HLHTID
*/
static UINT32 DquGetDevHlhtId(UINT8 devType, UINT8 devId, UINT16 logicId, DSU_RSSP_INFO_STRU_WL *pDsuRssp)
{
	UINT32 hlhtId = 0;	/*返回值HLHTID*/
	if (NULL != pDsuRssp)
	{
		switch (devType)
		{
		case ZC_TYPE:	/*ZC*/
			hlhtId = DquGetHlhtId(0, devId, logicId, pDsuRssp->ZcRowNum, pDsuRssp->ZcHlhtids);
			break;
		case CI_TYPE:	/*CI*/
			hlhtId = DquGetHlhtId(0, devId, logicId, pDsuRssp->CiRowNum, pDsuRssp->CiHlhtids);
			break;
		case ATS_TYPE:	/*ATS*/
			hlhtId = DquGetHlhtId(0, devId, logicId, pDsuRssp->AtsRowNum, pDsuRssp->AtsHlhtids);
			break;
		case VOBC_TYPE:	/*VOBC*/
			hlhtId = DquGetHlhtId(1, devId, logicId, pDsuRssp->VobcRowNum, pDsuRssp->VobcHlhtids);
			break;
		case DSU_TYPE:	/*DSU*/
			hlhtId = DquGetHlhtId(0, devId, logicId, pDsuRssp->DsuRowNum, pDsuRssp->DsuHlhtids);
			break;
		case AOM_TYPE:	/*AOM*/
			hlhtId = DquGetHlhtId(0, devId, logicId, pDsuRssp->AomRowNum, pDsuRssp->AomHlhtids);
			break;
		case TDT_TYPE:	/*TDT*/
			hlhtId = DquGetHlhtId(0, devId, logicId, pDsuRssp->TdtRowNum, pDsuRssp->TdtHlhtids);
			break;
		default:
			break;
		}
	}
	
	return hlhtId;
}

/*
*函数名:	DquToHlhtId
*功能描述:	根据设备ID,逻辑ID转成HLHTID
*输入参数:	UINT8 devType	设备类型
*			UINT8 devId		设备ID
*			UINT16 logicId	逻辑ID
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	无
*返回值:	HLHTID
*/
UINT32 DquToHlhtId(UINT8 devType, UINT8 devId, UINT16 logicId, DSU_RSSP_INFO_STRU_WL *pDsuRssp)
{
	UINT32 hlhtId = 0;	/*返回值HLHTID*/
	UINT16 devName = 0U;
	if (NULL != pDsuRssp)
	{
		hlhtId = DquGetDevHlhtId(devType, devId, logicId, pDsuRssp);
	}
	
	if (0 == hlhtId)	/*编号对照表中未找到*/
	{
		devName = devType;
		hlhtId = ((UINT16)(devName << 8) & 0xFF00) | (devId & 0x00FF);	/*使用设备类型ID作为RSSPI唯一ID*/
	}

	return hlhtId;
}

/*
*函数名:	DquTypeIdToHlhtId
*功能描述:	根据设备ID,逻辑ID转成HLHTID
*输入参数:	UINT16 devTypeId	设备类型ID
*			UINT16 logicId	逻辑ID
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	无
*返回值:	HLHTID
*/
UINT32 DquTypeIdToHlhtId(UINT16 devTypeId, UINT16 logicId, DSU_RSSP_INFO_STRU_WL *pDsuRssp)
{
	UINT32 hlhtId = 0;	/*返回值HLHTID*/
	UINT8 devType = (UINT8)((devTypeId >> 8) & 0x00FF);	/*设备类型*/
	UINT8 devId = (UINT8)(devTypeId & 0x00FF);	/*设备ID*/

	hlhtId = DquGetDevHlhtId(devType, devId, logicId, pDsuRssp);
	if (0 == hlhtId)	/*编号对照表中未找到*/
	{
		hlhtId = devTypeId;	/*使用设备类型ID作为RSSPI唯一ID*/
	}

	return hlhtId;
}

/*
*函数名:	DquToDevTypeId
*功能描述:	根据HLHTID转成设备类型ID
*输入参数:	UINT32 hlhtid	HLHTID
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	无
*返回值:	设备类型ID
*/
UINT16 DquToDevTypeId(UINT32 hlhtid, DSU_RSSP_INFO_STRU_WL *pDsuRssp)
{
	UINT16 typeId = 0;	/*设备类型ID*/
	UINT8 id = 0;	/*设备ID*/
	UINT16 devName = 0U;
	id = DquGetDevId(hlhtid, pDsuRssp->ZcRowNum, pDsuRssp->ZcHlhtids, DEV_HLHTID_NUM_ONE);
	if (0 == id)	/*ZC编号对照表中未找到*/
	{
		id = DquGetDevId(hlhtid, pDsuRssp->CiRowNum, pDsuRssp->CiHlhtids, DEV_HLHTID_NUM_ONE);
		if (0 == id)	/*CI编号对照表中未找到*/
		{
			id = DquGetDevId(hlhtid, pDsuRssp->AtsRowNum, pDsuRssp->AtsHlhtids, DEV_HLHTID_NUM_ONE);
			if (0 == id)	/*ATS编号对照表中未找到*/
			{
				id = DquGetDevId(hlhtid, pDsuRssp->VobcRowNum, pDsuRssp->VobcHlhtids, DEV_HLHTID_NUM_TWO);
				if (0 == id)	/*VOBC编号对照表中未找到*/
				{
					id = DquGetDevId(hlhtid, pDsuRssp->DsuRowNum, pDsuRssp->DsuHlhtids, DEV_HLHTID_NUM_ONE);
					if (0 == id)	/*DSU编号对照表中未找到*/
					{
						id = DquGetDevId(hlhtid, pDsuRssp->AomRowNum, pDsuRssp->AomHlhtids, DEV_HLHTID_NUM_ONE);
						if (0 == id)	/*AOM编号对照表中未找到*/
						{
							id = DquGetDevId(hlhtid, pDsuRssp->TdtRowNum, pDsuRssp->TdtHlhtids, DEV_HLHTID_NUM_ONE);
							if (0 == id)	/*TDT编号对照表中未找到*/
							{
								typeId = 0;
							}/*找到,设置TDT类型ID*/
							else
							{
								devName = (UINT8)TDT_TYPE;
							}
						}
						else	/*找到,设置AOM类型ID*/
						{
							devName = (UINT8)AOM_TYPE;
						}
					}
					else	/*找到,设置DSU类型ID*/
					{
						devName = (UINT8)DSU_TYPE;
					}
				}
				else	/*找到,设置VOBC类型ID*/
				{
					devName = (UINT8)VOBC_TYPE;
				}
			}
			else	/*找到,设置ATS类型ID*/
			{
				devName = (UINT8)ATS_TYPE;
			}
		}
		else	/*找到,设置CI类型ID*/
		{
			devName = (UINT8)CI_TYPE;
		}
	}
	else	/*找到,设置ZC类型ID*/
	{
		devName = (UINT8)ZC_TYPE;
	}
	typeId = (UINT16)(((devName << 8) & 0xFF00) | id);

	if (0 == typeId)	/*编号对照表中未找到*/
	{
		typeId = (UINT16)(hlhtid & 0x0000FFFF);	/*使用此RSSPI唯一ID作为设备类型ID*/
	}
	return typeId;
}

/*
*函数名:	DquToDevType
*功能描述:	根据HLHTID转成设备类型
*输入参数:	UINT32 hlhtid	HLHTID
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	无
*返回值:	设备类型
*/
UINT8 DquToDevType(UINT32 hlhtid, DSU_RSSP_INFO_STRU_WL *pDsuRssp)
{
	UINT8 devType = 0;	/*设备类型*/
	UINT16 typeId = 0;	/*设备类型ID*/

	typeId = DquToDevTypeId(hlhtid, pDsuRssp);
	devType = (UINT8)((typeId >> 8) & 0x00FF);

	return devType;
}

/*
*函数名:	DquToDevId
*功能描述:	根据HLHTID转成设备ID
*输入参数:	UINT32 hlhtid	HLHTID
*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	无
*返回值:	设备ID
*/
UINT8 DquToDevId(UINT32 hlhtid, DSU_RSSP_INFO_STRU_WL *pDsuRssp)
{
	UINT8 devId = 0;	/*设备ID*/
	UINT16 typeId = 0;	/*设备类型ID*/

	typeId = DquToDevTypeId(hlhtid, pDsuRssp);
	devId = (UINT8)(typeId & 0x00FF);

	return devId;
}

/*
*函数名:	FreeDsuRsspi
*功能描述:	释放RSSPI查询函数使用的空间
*输入参数:	DSU_RSSP_INFO_STRU_WL **pDsuRsrInfoStru_WL	RSSPI配置信息
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void FreeDsuRsspi(DSU_RSSP_INFO_STRU_WL **pDsuRsspInfo)
{
	UINT32 i = 0;	/*用于循环*/

	free((*pDsuRsspInfo)->pRsspLnkInfoStru);
	(*pDsuRsspInfo)->pRsspLnkInfoStru = NULL;

	free((*pDsuRsspInfo)->pDevCommInfoStru);
	(*pDsuRsspInfo)->pDevCommInfoStru = NULL;

	/*ZC编号对照表*/
	if (0 < (*pDsuRsspInfo)->ZcRowNum)
	{
		for (i = 0; i < (*pDsuRsspInfo)->ZcRowNum; i++)
		{
			free((*pDsuRsspInfo)->ZcHlhtids[i].HlhtIds);
			(*pDsuRsspInfo)->ZcHlhtids[i].HlhtIds = NULL;
		}
		free((*pDsuRsspInfo)->ZcHlhtids);
		(*pDsuRsspInfo)->ZcHlhtids = NULL;
	}

	/*CI编号对照表*/
	if (0 < (*pDsuRsspInfo)->CiRowNum)
	{
		for (i = 0; i < (*pDsuRsspInfo)->CiRowNum; i++)
		{
			free((*pDsuRsspInfo)->CiHlhtids[i].HlhtIds);
			(*pDsuRsspInfo)->CiHlhtids[i].HlhtIds = NULL;
		}
		free((*pDsuRsspInfo)->CiHlhtids);
		(*pDsuRsspInfo)->CiHlhtids = NULL;
	}

	/*ATS编号对照表*/
	if (0 < (*pDsuRsspInfo)->AtsRowNum)
	{
		for (i = 0; i < (*pDsuRsspInfo)->AtsRowNum; i++)
		{
			free((*pDsuRsspInfo)->AtsHlhtids[i].HlhtIds);
			(*pDsuRsspInfo)->AtsHlhtids[i].HlhtIds = NULL;
		}
		free((*pDsuRsspInfo)->AtsHlhtids);
		(*pDsuRsspInfo)->AtsHlhtids = NULL;
	}

	/*VOBC编号对照表*/
	if (0 < (*pDsuRsspInfo)->VobcRowNum)
	{
		for (i = 0; i < (*pDsuRsspInfo)->VobcRowNum; i++)
		{
			free((*pDsuRsspInfo)->VobcHlhtids[i].HlhtIds);
			(*pDsuRsspInfo)->VobcHlhtids[i].HlhtIds = NULL;
		}
		free((*pDsuRsspInfo)->VobcHlhtids);
		(*pDsuRsspInfo)->VobcHlhtids = NULL;
	}

	/*DSU编号对照表*/
	if (0 < (*pDsuRsspInfo)->DsuRowNum)
	{
		for (i = 0; i < (*pDsuRsspInfo)->DsuRowNum; i++)
		{
			free((*pDsuRsspInfo)->DsuHlhtids[i].HlhtIds);
			(*pDsuRsspInfo)->DsuHlhtids[i].HlhtIds = NULL;
		}
		free((*pDsuRsspInfo)->DsuHlhtids);
		(*pDsuRsspInfo)->DsuHlhtids = NULL;
	}

	/*AOM编号对照表*/
	if (0 < (*pDsuRsspInfo)->AomRowNum)
	{
		for (i = 0; i < (*pDsuRsspInfo)->AomRowNum; i++)
		{
			free((*pDsuRsspInfo)->AomHlhtids[i].HlhtIds);
			(*pDsuRsspInfo)->AomHlhtids[i].HlhtIds = NULL;
		}
		free((*pDsuRsspInfo)->AomHlhtids);
		(*pDsuRsspInfo)->AomHlhtids = NULL;
	}

	/*释放空间*/
	free(*pDsuRsspInfo);
	(*pDsuRsspInfo) = NULL;
}
#endif
