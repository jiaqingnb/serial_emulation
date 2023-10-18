#include "protclInterFun.h"
#include "ProtclManage.h"
#include "Convert.h"
#include "protclMgeCfg.h"
#include "dsuRsspFunc.h"
#include "CommonMemory.h"
#include "CommonRecord.h"

#define RSSPI_OPP_IDS_NUM (255) /*RSSPI的对方唯一ID数量*/

static UINT16 DelRsspiNotOppId(UINT16 linkDataLen, UINT8 linkData[], UINT16 singleLinkLen, UINT16 oppIdIdx, UINT8 oppIdsNum, const UINT32 oppIds[]);

/*
* 协议初始化模块
* 参数： 
* 输入参数：    char*  fileName         存放数据的地址
*				UINT32 inDataLen 内部参数长度
*				UINT8 inData[] 内部参数
*    			ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*   1 ：初始化成功
*   0: 初始化失败
*/
UINT8 ProtclInitFunc(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

	if ((NULL != pUnifyInfo) && (NULL != fileName))
	{
		ret = Init(fileName, inDataLen, inData, pUnifyInfo);
	}

	return ret;
}

/*
*函数名:	ProtclEndFunc
*功能描述:	适配层销毁清理协议
*输入参数:	ProtclConfigInfoStru *pUnifyInfo	适配层协议统一结构体
*输入出参:	无
*输出参数:	无
*返回值:	1;销毁成功,0:销毁失败
*/
UINT8 ProtclEndFunc(ProtclConfigInfoStru *pUnifyInfo)
{
	UINT8 ret = 0;
	if (NULL != pUnifyInfo)
	{
		ret = FreeSpace(pUnifyInfo);
	}
	

	return ret;
}

/*
* 功能：协议发送数据
*
* 参数： 
* 输入输出参数：  ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    UINT8 1 正常，其他不正常
*/

UINT8 ProtclSendDataFunc(ProtclConfigInfoStru * pUnifyInfo)
{
	UINT8 ret =0;
	if (NULL != pUnifyInfo)
	{
		if ((pUnifyInfo->AppArray != NULL) && (pUnifyInfo->VarArray != NULL))
		{
			ret = SendAppData(pUnifyInfo);
		}
		else
		{
			ret = 0;
		}
	}
	else
	{
		ret = 0;
	}
	return ret;
}

/*
* 功能：协议接收数组
* 参数：
* 输入输出参数：ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    1：接收数据成功
0：失败
*/

UINT8 ProtclReceiveDataFunc (ProtclConfigInfoStru * pUnifyInfo)
{
	UINT8 ret =0;

	if (pUnifyInfo != NULL)
	{
		ret = ReceiveAppData(pUnifyInfo);
	}
	else
	{
		ret=0;
	}

	return ret;
}

/*
* 功能：查询链路状态
*
* 参数： 
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
*             ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值： 
*     0：不存在该条链路，返回错误
*     0x17：链路在数据传输正常状态
*     0x2b：链路在通信中断状态
*     0x33：链路在准备建立链接状态
*     0x4e：链路在等待建立链接状态
*     0x01: 表示刷新无用的链路
*/
UINT8 ProtclGetLnkStatus (UINT8 destType,UINT8 destId,UINT16 logId,ProtclConfigInfoStru * pUnifyInfo)
{
	UINT8 ret =0;
	if (NULL!=pUnifyInfo)
	{
		ret = GetLnkStatus(destType,destId,logId,pUnifyInfo);
	}

	return ret;
}


/*
* 功能：删除链路
*
* 参数： 
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      动态ID
*             ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    UINT8 1 删链成功
*          0 删链失败
*/

UINT8 ProtclDeleteLnkFunc(UINT8 DestType,UINT8 DestID,UINT16 logId,ProtclConfigInfoStru * pUnifyInfo)
{
	UINT8 retnVal =0;

	if (NULL!= pUnifyInfo)
	{
		retnVal = DelLnkFunc(DestType, DestID, logId, pUnifyInfo);
	}
	
	return retnVal;
}


/*
* 功能：刷新链路状态
*
* 参数： 
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值： 
*     0：刷新链路失败
*     1：刷新链路成功 
*/
UINT8 ProtclReFreshFunc(UINT8* pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo)
{
	UINT8 retVal =0;
	if ((NULL != pLnkStatusData) && (NULL != pUnifyInfo))
	{
		retVal = ReFresh(pLnkStatusData, pUnifyInfo);
	}
	
	return retVal;
}

/************************************************************************
功能：向协议层输入TCP链路状态信息
描述: 用户需要每周期调用且仅调用一次本函数,为协议层更新TCP链路状态信息，
    注意，如果先调用协议接收函数ProtclReceiveDataFunc()，后调用本函数，可
	能导致本周期的协议接收函数无法正确工作，极端情况下会因为未正确接收AU1
	消息导致建链时间延长
输入参数：
	UINT16 TcpStateDataLen,  TCP状态信息字节数
	UINT8* pTcpStateData	TCP状态信息
返回值：0失败 1成功
************************************************************************/
UINT8 ProtclReceiveTcpStateFunc(UINT16 TcpStateDataLen,UINT8* pTcpStateData, ProtclConfigInfoStru *pUnifyInfo)
{
	UINT8 ret =0;
	if ((NULL!= pTcpStateData)&&(NULL!= pUnifyInfo))
	{
		ret = ReceiveTcpState(TcpStateDataLen, pTcpStateData, pUnifyInfo);
	}
	
	return ret;
}

/************************************************************************
功能：协议层输出TCP链路控制信息
描述：使用TCP通信（如RSSP2协议）时，用户需要每周期调用且仅调用一次本函数,
    获取协议层输出的TCP链路控制信息，并将该信息透明转发给2个通信控制器，
	注意，建议先向通信控制器发送TCP链路控制信息，后发送通信报文。
输出参数:
	UINT16 *pTcpCtrlDataLen, TCP控制信息字节数
	UINT8 *pTcpCtrlData      TCP控制信息
返回值：
************************************************************************/
UINT8 ProtclSendTcpCtrlFunc(UINT16 *pTcpCtrlDataLen,UINT8 *pTcpCtrlData, ProtclConfigInfoStru *pUnifyInfo)
{
	UINT8 ret = 0;
	if ((NULL != pTcpCtrlDataLen)&&(NULL!= pTcpCtrlData)&&(NULL!= pUnifyInfo))
	{
		ret = SendTcpCtrl(pTcpCtrlDataLen, pTcpCtrlData, pUnifyInfo);
	}
	
	return ret;
}

/*
*函数名:	DelRsspiNotOppId
*功能描述:	删除RSSPI链路中非指定的对方地址链路
*输入参数:	UINT16 linkDataLen 链路数据长度
*			UINT8 linkData[] 链路数据
*			UINT16 singleLinkLen 单个链路长度
*			UINT16 oppIdIdx 对方唯一ID在链路中位置
*			UINT8 oppIdsNum 对方唯一ID数量
*			const UINT32 oppIds[] 对方唯一ID数组
*输入出参:	无
*输出参数:	无
*返回值:	UINT16 aftLinkDataLen 处理后的链路数据长度
*/
static UINT16 DelRsspiNotOppId(UINT16 linkDataLen, UINT8 linkData[], UINT16 singleLinkLen, UINT16 oppIdIdx, UINT8 oppIdsNum, const UINT32 oppIds[])
{
	UINT16 aftLinkDataLen = linkDataLen; /*处理后链路数据长度*/
	UINT16 i = 0; /*用于循环*/
	UINT16 j = 0; /*用于循环*/
	UINT16 linkNum = linkDataLen / singleLinkLen; /*包含的单个链路数量*/
	UINT32 oppIdLink = 0; /*链路信息中对方唯一ID*/
	UINT16 curIdx = 0; /*当前链路下标位置*/

	if (( NULL != linkData)&&( NULL != oppIds))
	{
		for (i = 0; i < linkNum; i++) /*遍历链路信息*/
		{
			oppIdLink = LongFromChar(&linkData[curIdx + oppIdIdx]); /*链路中对方唯一ID*/
			for (j = 0; j < oppIdsNum; j++)
			{
				if (oppIds[j] == oppIdLink) /*找到退出循环*/
				{
					break;
				}
			}
			if (j == oppIdsNum) /*未找到,删除此对方唯一ID的链路*/
			{
				if (i < (linkNum - 1)) /*非最后一个,移动链路信息*/
				{
					CommonMemCpy(&linkData[curIdx], linkDataLen - (singleLinkLen * (i + 1)), &linkData[curIdx + singleLinkLen], linkDataLen - (singleLinkLen * (i + 1)));
				}
				aftLinkDataLen -= singleLinkLen; /*调整处理后长度*/
			}
			else
			{
				curIdx += singleLinkLen; /*下标偏移一个链路长度*/
			}
		}

	}
	
	return aftLinkDataLen;
}

/*
*函数名:	AdjustRsspiVarArray
*功能描述:	调整RSSPI链路,只保留指定的对方地址链路
*输入参数:	UINT8 oppIdsNum 对方类型ID逻辑ID数量
*			const UINT16 oppTypeIds[] 对方类型ID数组
*			const UINT16 oppLogicIds[] 对方逻辑ID数组
*			ProtclConfigInfoStru *pUnifyInfo 适配结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void AdjustRsspiVarArray(UINT8 oppIdsNum, const UINT16 oppTypeIds[], const UINT16 oppLogicIds[], ProtclConfigInfoStru *pUnifyInfo)
{
	const UINT16 oppIdSafeIdx = 7; /*对方唯一ID在安全层中位置*/
	const UINT16 oppIdRedundIdx = 0; /*对方唯一ID在冗余层中位置*/

	UINT32 oppIds[RSSPI_OPP_IDS_NUM] = { 0 }; /*对方唯一ID*/
	UINT32 i = 0; /*用于循环*/
	UINT8 *linkData = NULL; /*链路信息*/
	UINT16 totalLen = 0; /*链路总长度*/
	UINT16 curIdx = 0; /*当前链路下标位置*/
	UINT32 curProtcl = 1; /*当前协议标识*/
	UINT16 curLen = 0; /*当前协议链路长度*/
	UINT16 curLenIdx = 0; /*当前协议链路长度位置*/
	UINT16 safeLen = 0; /*RSSPI安全层链路长度*/
	UINT16 safeLenIdx = 0; /*RSSPI安全层链路长度位置*/
	UINT16 aftSafeLen = 0; /*处理后的RSSPI安全层链路长度*/
	UINT16 redundLen = 0; /*RSSPI冗余层链路长度*/
	UINT16 redundLenIdx = 0; /*RSSPI冗余层链路长度位置*/
	UINT16 aftRedundLen = 0; /*处理后的RSSPI冗余层链路长度*/
	UINT8 isAdjustRear = 0; /*是否调整尾部数据*/

#ifdef USE_RSSPI
	if ((0 < oppIdsNum) && (RSSPI_OPP_IDS_NUM > oppIdsNum) && (NULL != oppTypeIds) && (NULL != oppLogicIds) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->VarArray)
		&& (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo) && (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo->pDsuRsspInfoStruWL)) /*参数检查*/
	{
		for (i = 0; i < oppIdsNum; i++) /*获取到RSSPI对方唯一ID*/
		{
#ifdef RSSP_NO_HLHT
			oppIds[i] = oppTypeIds[i];
#else
			oppIds[i] = DquTypeIdToHlhtId(oppTypeIds[i], oppLogicIds[i], pUnifyInfo->GlobalVar->gpRsspUnitInfo->pDsuRsspInfoStruWL); /*对方唯一ID*/
#endif
		}

		linkData = pUnifyInfo->VarArray; /*链路信息*/
		/*链路总长度*/
		totalLen = 2 + ShortFromChar(linkData);
		curIdx += 2;

		for (curProtcl = 1; curProtcl <= PROTCL_MGECFG_MAXCNT; curProtcl++) /*遍历协议*/
		{
			/*当前链路下标位置*/
			curLen = ShortFromChar(&linkData[curIdx]);
			curLenIdx = curIdx;
			curIdx += 2;

			if ((curIdx + curLen) <= totalLen) /*协议链路长度有效*/
			{
				if ((DSU_PROTCL_RSSP == curProtcl) && (2 < curLen)) /*当前是RSSPI协议,有链路信息*/
				{
					safeLen = ShortFromChar(&linkData[curIdx]); /*安全层链路长度*/
					if (((2 + safeLen) <= curLen) && (0 == (safeLen%RSSP_VAR_NODE_LEN_WL))) /*安全层链路长度有效*/
					{
						safeLenIdx = curIdx; /*安全层链路长度位置*/
						curIdx += 2;

						if (0 < safeLen) /*有安全层链路信息*/
						{
							aftSafeLen = DelRsspiNotOppId(safeLen, &linkData[curIdx], RSSP_VAR_NODE_LEN_WL, oppIdSafeIdx, oppIdsNum, oppIds);
						}

						redundLenIdx = curIdx + safeLen;
						redundLen = curLen - 2 - safeLen;
						if ((0 < redundLen) && (0 == (redundLen % RP_LINK_LEN)))
						{
							aftRedundLen = DelRsspiNotOppId(redundLen, &linkData[redundLenIdx], RP_LINK_LEN, oppIdRedundIdx, oppIdsNum, oppIds);
						}

						if ((aftSafeLen <= safeLen) && (aftRedundLen <= redundLen)) /*处理后长度有效*/
						{
							curIdx += aftSafeLen; /*处理后冗余层开始位置*/
							if (aftSafeLen < safeLen) /*安全层调整过*/
							{
								CommonMemCpy(&linkData[curIdx], aftRedundLen, &linkData[redundLenIdx], aftRedundLen); /*调整冗余层数据*/
								ShortToChar(aftSafeLen, &linkData[safeLenIdx]); /*设置处理后的安全层长度*/
								isAdjustRear = 1;
							}
							else
							{
								if (aftRedundLen < redundLen) /*冗余层调整过*/
								{
									isAdjustRear = 1;
								}
							}
							curIdx += aftRedundLen; /*尾部数据开始位置*/
							if (1 == isAdjustRear) /*需调整尾部数据*/
							{
								CommonMemCpy(&linkData[curIdx], totalLen - curLenIdx - 2 - curLen, &linkData[curLenIdx + 2 + curLen], totalLen - curLenIdx - 2 - curLen); /*调整尾部数据*/
								ShortToChar(2 + aftSafeLen + aftRedundLen, &linkData[curLenIdx]); /*设置处理后的RSSPI链路长度*/
								ShortToChar(totalLen - 2 - (safeLen - aftSafeLen) - (redundLen - aftRedundLen), linkData); /*设置适配层链路总长度*/
							}
						}
					}

					break; /*找到RSSPI处理完链路退出循环*/
				}
				else /*非RSSPI协议继续循环处理*/
				{
					curIdx += curLen;
				}
			}
			else /*协议链路长度无效,退出循环*/
			{
				break;
			}
		}
	}
#endif
}

/*
*功能描述:	获取各网收到包数
*输入参数:	ProtclConfigInfoStru* pUnifyInfo 适配层变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
void ProtclGetNetsRecvPkgNumFunc(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
	if ((NULL != pUnifyInfo) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum)) /*参数检查*/
	{
		GetNetsRecvPkgNum(pUnifyInfo, dstType, dstId, dstLogicId, redRecvPkgNum, blueRecvPkgNum);
	}
}

/*
*函数名:	ProtclGetSwapData
*功能描述:	获取输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据 2字节总长度+SFP(2字节长度+N个[4字节CRC])+RSSPI(2字节长度+N个[4字节SVC]) 空间大于2046
*           const UINT16 swapDataMaxLen 交互数据最大长度 大于2046
*           ProtclConfigInfoStru* protcl 协议结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void ProtclGetSwapData(UINT8 swapData[], const UINT16 swapDataMaxLen, ProtclConfigInfoStru* protcl)
{
	UINT16 swapDataLen = 0u; /*交互数据长度*/
	UINT8* record = NULL; /*日志数组*/
	UINT16 recordSize = 0u; /*日志数组长度*/

	if ((NULL != swapData) && (NULL != protcl) && (NULL != protcl->GlobalVar) && (2u < swapDataMaxLen)) /*非空检查*/
	{
		record = protcl->RecordArray;
		recordSize = protcl->RecordArraySize;
		PrintRecArrayByte(record, recordSize, 0xD1u);

		ShortToChar(0u, &swapData[0]); /*总长度清零*/

#ifdef USE_SFP
		SfpGetSwapData(&swapData[2], swapDataMaxLen, protcl->GlobalVar->gpsfpUnitInfo);
#else
		ShortToChar(0u, &swapData[2]); /*长度设置为0*/
#endif
		swapDataLen = 2u + 2u + ShortFromChar(&swapData[2]); /*累加上SFP长度,即rsspi开始位置*/
		if (swapDataLen < swapDataMaxLen) /*有空间*/
		{
#ifdef USE_RSSPI
			RsspiGetSwapData(&swapData[swapDataLen], swapDataMaxLen - swapDataLen, protcl->GlobalVar->gpRsspUnitInfo);
#else
			ShortToChar(0u, &swapData[swapDataLen]); /*长度设置为0*/
#endif
			swapDataLen += (2u + ShortFromChar(&swapData[swapDataLen])); /*累加上RSSPI长度*/

			ShortToChar(swapDataLen - 2u, &swapData[0]); /*设置总长度*/
		}
		else /*无空间*/
		{
			PrintRecArrayByte(record, recordSize, 1u);
			PrintRecArray2ByteB(record, recordSize, swapDataLen);
			PrintRecArray2ByteB(record, recordSize, swapDataMaxLen);
		}
	}
}

/*
*函数名:	ProtclSetSwapData
*功能描述:	设置输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据 2字节总长度+SFP(2字节长度+N个[4字节CRC])+RSSPI(2字节长度+N个[4字节SVC])
*			ProtclConfigInfoStru* protcl 协议结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void ProtclSetSwapData(UINT8 swapData[], ProtclConfigInfoStru* protcl)
{
	UINT16 swapDataLen = 0u; /*交互数据长度*/
	UINT32 allLen = 0u; /*总长度*/
	UINT32 outAppIdx = 0u; /*输出应用数据位置*/
	UINT8* outAppData = NULL; /*输出应用数据*/
	UINT16 pkgLen = 0u; /*包长度*/
	UINT8  protclType = 0u; /*协议类型*/
	UINT16 sfpLen = 0u; /*SFP长度*/
	UINT16 rsspiLen = 0u; /*RSSPI长度*/
	UINT16 sfpStartIdx = 0u; /*SFP开始位置*/
	UINT16 rsspiStartIdx = 0u; /*RSSPI开始位置*/
	UINT8 isProSfp = 0u; /*是否处理过SFP*/
	UINT8 isProRsspi = 0u; /*是否处理过RSSPI*/
	UINT8* record = NULL; /*日志数组*/
	UINT16 recordSize = 0u; /*日志数组长度*/

	if ((NULL != swapData) && (NULL != protcl) && (NULL != protcl->GlobalVar))
	{
		record = protcl->RecordArray;
		recordSize = protcl->RecordArraySize;
		PrintRecArrayByte(record, recordSize, 0xD2u);

		swapDataLen = ShortFromChar(&swapData[0]); /*总长度*/

		sfpStartIdx = 2u; /*RSSPI开始位置*/
		sfpLen = ShortFromChar(&swapData[2]);
		rsspiStartIdx = 2u + sfpStartIdx + sfpLen; /*RSSPI开始位置*/
		rsspiLen = ShortFromChar(&swapData[rsspiStartIdx]);
		if (((2u + sfpLen + 2u + rsspiLen) == swapDataLen) && (0u == (sfpLen % 4u)) && (0u == (rsspiLen % 4u))) /*长度检查*/
		{
			/*AppArray:4字节总长度+|&2字节长度+1字节协议类型+1字节目的类型+1字节目的ID+2字节预留+网络上发送数据(协议包)*/
			allLen = LongFromChar(&protcl->AppArray[0]);
			if (0u < allLen) /*有输出*/
			{
				outAppData = &protcl->AppArray[4];
				while (outAppIdx < allLen) /*遍历输出应用数据*/
				{
					pkgLen = 2u + ShortFromChar(&outAppData[outAppIdx]);
					if ((outAppIdx + pkgLen) <= allLen) /*当前输出应用位置加长度小于等于输出应用长度,合法*/
					{
						protclType = outAppData[outAppIdx + 2u]; /*协议类型*/
						if ((DSU_PROTCL_SFP == protclType) || (DSU_PROTCL_RSSP == protclType)) /*SFP或RSSPI*/
						{
							if (DSU_PROTCL_SFP == protclType) /*SFP*/
							{
								if (0u == isProSfp) /*只处理一次,适配层输出数据中,SFP数据一份*/
								{
#ifdef USE_SFP
									SfpSetSwapDataForOut(&swapData[sfpStartIdx], &outAppData[outAppIdx], protcl->GlobalVar->gpsfpUnitInfo);
#endif
									isProSfp = 1u;
								}
							}
							else /*RSSPI*/
							{
								if (0u == isProRsspi) /*只处理一次,适配层输出数据中,RSSPI数据一份*/
								{
#ifdef USE_RSSPI
									RsspiSetSwapDataForOut(&swapData[rsspiStartIdx], &outAppData[outAppIdx], protcl->GlobalVar->gpRsspUnitInfo);
#endif
									isProRsspi = 1u;
								}
							}
						}

						outAppIdx += pkgLen;
					}
					else /*包长度非法*/
					{
						PrintRecArrayByte(record, recordSize, 3u);
						PrintRecArray2ByteB(record, recordSize, pkgLen);
						PrintRecArray4ByteB(record, recordSize, outAppIdx);
						PrintRecArray4ByteB(record, recordSize, allLen);

						break;
					}
				}
			}
			else /*无输出*/
			{
				PrintRecArrayByte(record, recordSize, 2u);
				PrintRecArrayByte(record, recordSize, 0u);
			}
		}
		else /*长度检查错误*/
		{
			PrintRecArrayByte(record, recordSize, 1u);
			PrintRecArray2ByteB(record, recordSize, swapDataLen);
			PrintRecArray2ByteB(record, recordSize, sfpLen);
			PrintRecArray2ByteB(record, recordSize, rsspiLen);
		}
	}
}

/*
*函数名:	ProtclDelAllLink
*功能描述:	适配层删除所有链路
*输入参数:	ProtclConfigInfoStru* protcl 协议结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void ProtclDelAllLink(ProtclConfigInfoStru* protcl)
{
	if ((NULL != protcl) && (NULL != protcl->GlobalVar)) /*参数检查*/
	{
#ifdef USE_SFP
		SfpDelAllLink(protcl->GlobalVar->gpsfpUnitInfo);
#endif
#ifdef USE_RSSPI
		RsspiDelAllLink(protcl->GlobalVar->gpRsspUnitInfo);
#endif
	}
}
