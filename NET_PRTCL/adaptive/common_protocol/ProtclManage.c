/*
*
* 文件名   ：  ProtclManage.c
* 版权说明 ：  北京北京交控科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2013.09.15
* 作者     ：  王佩佩
* 功能描述 ：  协议主处理类,主要实现了周期性调用协议。
* 使用注意 ：
* 修改记录 ：
*    时间			修改人		修改理由
*    2014.03.06     yw.lou      发送数据如果没有发送报文,对应的链路状态没有赋值,则导致最终在刷新链路时候链路不一致;
*                               无法区别开到底是哪个协议,因此修改,如果没有中间状态数据,则赋值0,以便能正常处理;
*    2014.03.20     yw.lou      只为ATS通信开辟了1个空间,而ATS最多可能发送2或3帧,导致数组越界;修改分配空间:
*                               最大可能链路数*各协议通信对象个数;
*    2014.04.13     yw.lou      为协议数据接收结构体分配空间后未初始化,接收各协议数据后调用ReceiveAppData函数,proType字段未初始化
*                               导致其它未收到数据未初始化的协议类型清空了已收到数据的协议类型;
*                               malloc申请空间失败后均返回失败,在初始化阶段完成检查,避免随机故障;
*
*---------------------------------------------
*/
#include <stdlib.h>
#include "CommonMemory.h"
#include "dsuProtclFun.h"
#include "protclMgeCfg.h"
#include "ProtclManage.h"
#include "Convert.h"
#include "dfsDataRead.h"

#ifdef USE_SFP
#include "sfpPrtclCommon.h"
#endif
#ifdef USE_RSR
#include "dsuRsrFunc.h"
#include "RsrCommon.h"
#endif
#ifdef USE_RSSPII
#include "Rssp2.h"
#include "PrtclParseManage.h"
#endif
#ifdef USE_RP
#include "RpInterface.h"
#endif
#ifdef USE_FSFB
#include "FsfbProtcl.h"
#endif

/***********协议初始化相关函数**************************************************************/
static UINT8 InitProtclInfoStru(ProtclConfigInfoStru* pUnifyInfo, UINT16 maxLnkNum);
static UINT8 InitSfpProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo);
static UINT8 InitRpProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo);
static UINT8 InitRsspProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo);
static UINT8 InitRsrProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo);
static UINT8 InitRssp2ProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo);
static UINT8 InitFsfbProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo);
static void GetSfpNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);
static void GetRsspiNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);
static void GetRsspiiNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);
static void GetFsfbNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);
static void TypeIdToDevName(UINT8 type, UINT8 id, UINT16* pDevName);
static PROTCL_APP_TO_STRU* NewProtclAppToStru(const UINT16 outputSize, const UINT16 inputSize);
static UINT8* NewAppData(UINT16* appDataMaxLen, const UINT16 outputSize, const UINT16 inputSize);

/* 各协议函数指针配置表 */
static PROTCL_MGECFG_STRU protclCfgStruBuff[PROTCL_MGECFG_MAXCNT + 1] = {
	{protcl_nul_Init,protcl_nul_Snd,protcl_nul_Rcv,protcl_nul_Lnk,protcl_nul_NetsRecvPkgNum,protcl_nul_DelLnk,protcl_nul_ReFreshLnk,protcl_nul_End}

#ifdef USE_SFP
	,{InitSfpProtclStru,sfpSendData,SfpRcvData,GetSfpLnkStatus,GetSfpNetsRecvPkgNum, SfpDelLnk,SfpReFreshLnk,SfpFreeSpaceFunc}
#else
	,{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#endif

#ifdef USE_RP
	,{InitRpProtclStru,RpSendData,RpRcvData,protcl_nul_Lnk,protcl_nul_NetsRecvPkgNum,RpDelLnk,RpReFreshLnk,RpFreeSpaceFunc}
#else
	,{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#endif

#ifdef USE_RSR
	,{InitRsrProtclStru,RsrSendData,RsrRcvData,GetRsrLnkStatus,protcl_nul_NetsRecvPkgNum,RsrDelLnk,RsrReFreshLnk,RsrFreeSpaceFunc}
#else
	,{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#endif

#ifdef USE_RSSPI
	,{InitRsspProtclStru,RsspSendData,RsspRcvData,GetRsspLnkStatus,GetRsspiNetsRecvPkgNum,RsspDelLnk,RsspReFreshLnk,RsspFreeSpaceFunc}
#else
	,{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#endif

#ifdef USE_RSSPII
	,{InitRssp2ProtclStru,Rssp2SendData,Rssp2RcvData,GetRssp2LnkStatus,GetRsspiiNetsRecvPkgNum,Rssp2DelLnk,Rssp2ReFreshLnk,Rssp2FreeSpaceFunc}
#else
	,{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#endif

#ifdef USE_FSFB
	,{InitFsfbProtclStru,FsfbSendData,FsfbRcvData,GetFsfbLnkStatus,GetFsfbNetsRecvPkgNum,FsfbDelLnk,FsfbReFreshLnk,FsfbFreeSpaceFunc}
#else
	,{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#endif
};

/*
* 从配置文件读取相对应的协议配置数据
*/
UINT8 Init(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;
	UINT8 tempProtclCnt = 0;
	UINT8 tmpI = 0;
	UINT8 flag = 0;
	UINT8 rslt = 0;
	F_INIT tmpInit;
	/* 本设备使用日志长度比率*/
	UINT32 curDevRecordLenRatio[PROTCL_MGECFG_MAXCNT] = { 0 };
	/* 本设备使用日志总长度*/
	UINT32 curDevRecordTotalLen = 0;
	/* 设备一个链路长度*/
	UINT32 devRecFitLen[PROTCL_MGECFG_MAXCNT] = { 0 };
	/* 临时变量*/
	UINT32 tmpVal = 0;
	PROTCL_APP_TO_STRU* pCurRcvProtclAppStru = NULL; /*接收单帧相关信息*/
	PROTCL_RCV_DATA_TYPE* ptemProtclRcvDataStru = NULL;	/*临时存放协议数据的结构体*/
	PROTCL_APP_TO_STRU* pCurSndProtclAppStru = NULL; /*发送单帧相关信息*/
	PROTCL_SND_CFG_STRU* protclSndCfgStru = NULL;	/*发送结构*/
	UINT8* pProtclNumBuff = NULL; /*各协议最大链路数*/
	UINT32* curDevRecordLen = NULL;	/*本设备使用日志长度*/
	DSU_PROTCL_TYPE_STRU* protclTypeInfoStru = NULL;	/*各协议通信数量*/
	UINT32 i = 0u; /*用于循环*/
	UINT8 funRtn = 0u; /*调用函数返回值*/

	/* VOBC的记录数据大小 */
	if ((NULL == pUnifyInfo))
	{
		return retVal;
	}

	pUnifyInfo->GlobalVar = (GlobalVar*)malloc(sizeof(GlobalVar));
	if (NULL != pUnifyInfo->GlobalVar)
	{
		/*初始化协议数据的结构体*/
		CommonMemSet(pUnifyInfo->GlobalVar, sizeof(GlobalVar), 0, sizeof(GlobalVar));

		protclSndCfgStru = pUnifyInfo->GlobalVar->protclSndCfgStru;	/*发送结构*/
		pProtclNumBuff = pUnifyInfo->GlobalVar->pProtclNumBuff; /*各协议最大链路数*/
		curDevRecordLen = pUnifyInfo->GlobalVar->curDevRecordLen;	/*本设备使用日志长度*/
	}
	else
	{
		return retVal;
	}

	tempProtclCnt = InitGetCfgInfo(fileName, inDataLen, inData, &pProtclNumBuff[1], pUnifyInfo);

	if (tempProtclCnt == 0)
	{
		retVal = 0;
		return retVal;
	}
	protclTypeInfoStru = &pUnifyInfo->GlobalVar->protclTypeInfoStru;

	if (2 < pUnifyInfo->RecordArraySize)	/*长度大于2,才做日志空间分配处理*/
	{
		/*按比率分配日志空间*/
		devRecFitLen[0] = SFP_PER_LNK_RECORD_MAX_LEN;
		devRecFitLen[1] = RP_PER_LNK_RECORD_MAX_LEN;
		devRecFitLen[2] = RSR_PER_LNK_RECORD_MAX_LEN;
		devRecFitLen[3] = RSSPI_PER_LNK_RECORD_MAX_LEN;
		devRecFitLen[4] = RSSPII_PER_LNK_RECORD_MAX_LEN;
		devRecFitLen[5] = FSFB_PER_LNK_RECORD_MAX_LEN;
		for (tmpI = 0; tmpI < PROTCL_MGECFG_MAXCNT; tmpI++)
		{
			if (pProtclNumBuff[tmpI + 1] > 0)
			{
				curDevRecordLenRatio[tmpI] = devRecFitLen[tmpI] * pProtclNumBuff[tmpI + 1];
				curDevRecordTotalLen += curDevRecordLenRatio[tmpI];
			}
		}
		for (tmpI = 0; tmpI < PROTCL_MGECFG_MAXCNT; tmpI++)
		{
			if (pProtclNumBuff[tmpI + 1] > 0)
			{
				curDevRecordLen[tmpI] = (pUnifyInfo->RecordArraySize - 2) * curDevRecordLenRatio[tmpI] / curDevRecordTotalLen;
				/*最小日志长度*/
				tmpVal += devRecFitLen[tmpI];
			}
		}
		/*调整日志空间.总分配日志长度小于最小日志长度:按比率分配; 否则:在按比率分配基础上,调整使任一可用协议日志长度不小于一个链路长度*/
		if ((pUnifyInfo->RecordArraySize - 2) >= tmpVal)
		{
			/*调整后,需减掉的长度*/
			tmpVal = 0;
			for (tmpI = 0; tmpI < PROTCL_MGECFG_MAXCNT; tmpI++)
			{
				/*只处理可用协议日志长度*/
				if (pProtclNumBuff[tmpI + 1] > 0)
				{
					if (curDevRecordLen[tmpI] < devRecFitLen[tmpI]) /*长度小于一个链路长度,调整为一个链路长度,并累加需减掉的长度*/
					{
						tmpVal += (devRecFitLen[tmpI] - curDevRecordLen[tmpI]);
						curDevRecordLen[tmpI] = devRecFitLen[tmpI];
					}
					else
					{
						/*处理减掉的长度*/
						if (tmpVal > 0)
						{
							if ((curDevRecordLen[tmpI] > tmpVal) && (curDevRecordLen[tmpI] - tmpVal >= devRecFitLen[tmpI])) /*可减掉*/
							{
								curDevRecordLen[tmpI] -= tmpVal;
								tmpVal = 0;
							}
							else /*不可减掉,调整为一个链路长度,并累加需减掉的长度*/
							{
								tmpVal += (devRecFitLen[tmpI] - curDevRecordLen[tmpI]);
								curDevRecordLen[tmpI] = devRecFitLen[tmpI];
							}
						}
					}
				}
			}
			/*还有需减掉的长度*/
			if (tmpVal > 0)
			{
				for (tmpI = 0; tmpI < PROTCL_MGECFG_MAXCNT; tmpI++)
				{
					/*此日志长度可减掉需减掉的长度*/
					if ((pProtclNumBuff[tmpI + 1] > 0) && (curDevRecordLen[tmpI] > tmpVal) && (curDevRecordLen[tmpI] - tmpVal >= devRecFitLen[tmpI]))
					{
						curDevRecordLen[tmpI] -= tmpVal;
						tmpVal = 0;
						break;
					}
				}
			}
		}
	}

	/* 初始化各协议*/
	for (tmpI = 0; tmpI < tempProtclCnt + 1; tmpI++)
	{
		if (0 < pProtclNumBuff[tmpI])	/*此协议有通信,初始化*/
		{
			tmpInit = protclCfgStruBuff[tmpI].fInit;
			if (NULL != tmpInit)
			{
				retVal = tmpInit(fileName, inDataLen, inData, pProtclNumBuff[tmpI], pUnifyInfo);
			}
			else /*无此协议*/
			{
				retVal = 1u;
			}

			if (0 == retVal)
			{
				/* 若有一个初始化失败，将不再继续。 */
				return retVal;
			}
		}
		else /*无需初始化*/
		{
			retVal = 1;
		}
	}

	/* 给临时存放协议数据的结构体分配空间 */
	pUnifyInfo->GlobalVar->RcvData = (PROTCL_RCV_DATA_TYPE*)malloc(sizeof(PROTCL_RCV_DATA_TYPE) * (PROTCL_MGECFG_MAXCNT + 1));
	ptemProtclRcvDataStru = pUnifyInfo->GlobalVar->RcvData;

	/* 给接收数据的临时存档数据的结构体分配空间 */
	if (ptemProtclRcvDataStru != NULL)
	{
		/*初始化协议数据的结构体*/
		CommonMemSet(ptemProtclRcvDataStru, sizeof(PROTCL_RCV_DATA_TYPE) * (PROTCL_MGECFG_MAXCNT + 1), 0, sizeof(PROTCL_RCV_DATA_TYPE) * (PROTCL_MGECFG_MAXCNT + 1));

		/*申请并初始化协议数据结构体的接收数据缓冲区*/
		ptemProtclRcvDataStru[DSU_PROTCL_SFP].pData = (UINT8*)malloc(sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->sfpProtclNum);
		CommonMemSet(ptemProtclRcvDataStru[DSU_PROTCL_SFP].pData, sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->sfpProtclNum,
			0, pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->sfpProtclNum);

		/*规定协议数据结构体第2个空间存放SFP数据类型,第一个为ptemProtclRcvDataStru[0],默认不使用*/
		ptemProtclRcvDataStru[DSU_PROTCL_SFP].proType = DSU_PROTCL_SFP;

		/*申请并初始化协议数据结构体的接收数据缓冲区*/
		ptemProtclRcvDataStru[DSU_PROTCL_RP].pData = (UINT8*)malloc(sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->redunProtclNum);
		CommonMemSet(ptemProtclRcvDataStru[DSU_PROTCL_RP].pData, sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->redunProtclNum,
			0, pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->redunProtclNum);

		/*规定协议数据结构体第3个空间存放redun数据类型*/
		ptemProtclRcvDataStru[DSU_PROTCL_RP].proType = DSU_PROTCL_RP;

		/*申请并初始化协议数据结构体的接收数据缓冲区*/
		ptemProtclRcvDataStru[DSU_PROTCL_RSR].pData = (UINT8*)malloc(sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rsrProtclNum);
		CommonMemSet(ptemProtclRcvDataStru[DSU_PROTCL_RSR].pData, sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rsrProtclNum,
			0, pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rsrProtclNum);

		/*规定协议数据结构体第4个空间存放redun数据类型*/
		ptemProtclRcvDataStru[DSU_PROTCL_RSR].proType = DSU_PROTCL_RSR;

		/*申请并初始化协议数据结构体的接收数据缓冲区*/
		ptemProtclRcvDataStru[DSU_PROTCL_RSSP].pData = (UINT8*)malloc(sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rsspProtclNum);
		CommonMemSet(ptemProtclRcvDataStru[DSU_PROTCL_RSSP].pData, sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rsspProtclNum,
			0, pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rsspProtclNum);

		/*规定协议数据结构体第5个空间存放RSSP数据类型*/
		ptemProtclRcvDataStru[DSU_PROTCL_RSSP].proType = DSU_PROTCL_RSSP;

		/*申请并初始化协议数据结构体的接收数据缓冲区*/
		ptemProtclRcvDataStru[DSU_PROTCL_RSSP2].pData = (UINT8*)malloc(sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rssp2ProtclNum);
		CommonMemSet(ptemProtclRcvDataStru[DSU_PROTCL_RSSP2].pData, sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rssp2ProtclNum,
			0, pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->rssp2ProtclNum);

		/*规定协议数据结构体第5个空间存放RSSP数据类型*/
		ptemProtclRcvDataStru[DSU_PROTCL_RSSP2].proType = DSU_PROTCL_RSSP2;

		/*申请并初始化协议数据结构体的接收数据缓冲区*/
		ptemProtclRcvDataStru[DSU_PROTCL_FSFB].pData = (UINT8*)malloc(sizeof(UINT8) * pUnifyInfo->InputSize * pUnifyInfo->MaxNumPerCycle * protclTypeInfoStru->fsfbProtclNum);
		CommonMemSet(ptemProtclRcvDataStru[DSU_PROTCL_FSFB].pData, sizeof(UINT8)* pUnifyInfo->InputSize* pUnifyInfo->MaxNumPerCycle* protclTypeInfoStru->fsfbProtclNum,
			0, pUnifyInfo->InputSize* pUnifyInfo->MaxNumPerCycle* protclTypeInfoStru->fsfbProtclNum);
		/*规定协议数据结构体第5个空间存放RSSP数据类型*/
		ptemProtclRcvDataStru[DSU_PROTCL_FSFB].proType = DSU_PROTCL_FSFB;
	}
	else
	{
		/* 不做处理 */
		retVal = 0;
		return retVal;
	}


	/*初始化接收单帧应用数据*/
	pUnifyInfo->GlobalVar->pRcvProtclAppStru = NewProtclAppToStru(pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
	if (NULL == pUnifyInfo->GlobalVar->pRcvProtclAppStru)
	{
		retVal = 0;
		return retVal;
	}

	/*初始化发送单帧应用数据*/
	pUnifyInfo->GlobalVar->pSndProtclAppStru = NewProtclAppToStru(pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
	if (NULL == pUnifyInfo->GlobalVar->pSndProtclAppStru)
	{
		retVal = 0;
		return retVal;
	}


	for (tmpI = 0; tmpI < PROTCL_MGECFG_MAXCNT + 1; tmpI++)
	{
		/* 防止野指针 */
		protclSndCfgStru[tmpI].pProtclAppStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU) * 0);
		protclSndCfgStru[tmpI].DevDataCnt = 0;
	}

	/* 给发送数据的临时存档数据的结构体分配空间 */
	if (0 < protclTypeInfoStru->sfpProtclNum)
	{
#ifdef USE_SFP
		tmpVal = pUnifyInfo->GlobalVar->gpsfpUnitInfo->MaxNumLink * pUnifyInfo->MaxNumPerCycle;
		protclSndCfgStru[DSU_PROTCL_SFP].pProtclAppStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU) *tmpVal);
		if ((NULL == protclSndCfgStru[DSU_PROTCL_SFP].pProtclAppStru) || (NULL == ptemProtclRcvDataStru[DSU_PROTCL_SFP].pData))
		{
			flag = 1;
		}
		else
		{
			/*为声明的发送数据临时存储区初始化*/
			CommonMemSet(protclSndCfgStru[DSU_PROTCL_SFP].pProtclAppStru, sizeof(PROTCL_APP_TO_STRU)* tmpVal, 0u, sizeof(PROTCL_APP_TO_STRU)* tmpVal);

			/*初始化单帧应用数据*/
			for (i = 0; i < tmpVal; i++)
			{
				protclSndCfgStru[DSU_PROTCL_SFP].pProtclAppStru[i].pAppData = NewAppData(&protclSndCfgStru[DSU_PROTCL_SFP].pProtclAppStru[i].appDataMaxLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == protclSndCfgStru[DSU_PROTCL_SFP].pProtclAppStru[i].pAppData)
				{
					break;
				}
			}
			if (i == tmpVal)
			{
				pUnifyInfo->GlobalVar->gpSfpRcvDataBuff = NewAppData(&pUnifyInfo->GlobalVar->gSfpRcvDataBuffLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == pUnifyInfo->GlobalVar->gpSfpRcvDataBuff)
				{
					flag = 1;
				}
			}
			else
			{
				flag = 1;
			}

		}
#endif
	}

	if (protclTypeInfoStru->redunProtclNum > 0)
	{
#ifdef USE_RP
		tmpVal = pUnifyInfo->GlobalVar->gpRedunUnitInfo->MaxNumLink * pUnifyInfo->MaxNumPerCycle;
		protclSndCfgStru[DSU_PROTCL_RP].pProtclAppStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU) * tmpVal);
		if ((NULL == protclSndCfgStru[DSU_PROTCL_RP].pProtclAppStru) || (NULL == ptemProtclRcvDataStru[DSU_PROTCL_RP].pData))
		{
			flag = 1;
		}
		else
		{
			/*为声明的发送数据临时存储区初始化*/
			CommonMemSet(protclSndCfgStru[DSU_PROTCL_RP].pProtclAppStru, sizeof(PROTCL_APP_TO_STRU)* tmpVal, 0u, sizeof(PROTCL_APP_TO_STRU)* tmpVal);

			/*初始化单帧应用数据*/
			for (i = 0; i < tmpVal; i++)
			{
				protclSndCfgStru[DSU_PROTCL_RP].pProtclAppStru[i].pAppData = NewAppData(&protclSndCfgStru[DSU_PROTCL_RP].pProtclAppStru[i].appDataMaxLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == protclSndCfgStru[DSU_PROTCL_RP].pProtclAppStru[i].pAppData)
				{
					break;
				}
			}
			if (i == tmpVal)
			{
				pUnifyInfo->GlobalVar->gpRpRcvDataBuff = NewAppData(&pUnifyInfo->GlobalVar->gRpRcvDataBuffLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == pUnifyInfo->GlobalVar->gpRpRcvDataBuff)
				{
					flag = 1;
				}
			}
			else
			{
				flag = 1;
			}

		}
#endif
	}

	if (protclTypeInfoStru->rsrProtclNum > 0)
	{
#ifdef USE_RSR
		tmpVal = pUnifyInfo->GlobalVar->gpRsrUnitInfo->MaxNumLink * pUnifyInfo->MaxNumPerCycle;
		protclSndCfgStru[DSU_PROTCL_RSR].pProtclAppStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU) * tmpVal);
		if ((NULL == protclSndCfgStru[DSU_PROTCL_RSR].pProtclAppStru) || (NULL == ptemProtclRcvDataStru[DSU_PROTCL_RSR].pData))
		{
			flag = 1;
		}
		else
		{
			/*为声明的发送数据临时存储区初始化*/
			CommonMemSet(protclSndCfgStru[DSU_PROTCL_RSR].pProtclAppStru, sizeof(PROTCL_APP_TO_STRU) * tmpVal, 0u, sizeof(PROTCL_APP_TO_STRU) * tmpVal);

			/*初始化单帧应用数据*/
			for (i = 0; i < tmpVal; i++)
			{
				protclSndCfgStru[DSU_PROTCL_RSR].pProtclAppStru[i].pAppData = NewAppData(&protclSndCfgStru[DSU_PROTCL_RSR].pProtclAppStru[i].appDataMaxLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == protclSndCfgStru[DSU_PROTCL_RSR].pProtclAppStru[i].pAppData)
				{
					break;
				}
			}
			if (i == tmpVal)
			{
				pUnifyInfo->GlobalVar->gpRsrRcvDataBuff = NewAppData(&pUnifyInfo->GlobalVar->gRsrRcvDataBuffLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == pUnifyInfo->GlobalVar->gpRsrRcvDataBuff)
				{
					flag = 1;
				}
			}
			else
			{
				flag = 1;
			}

		}
#endif
	}

	if (protclTypeInfoStru->rsspProtclNum > 0)
	{
#ifdef USE_RSSPI
		tmpVal = pUnifyInfo->GlobalVar->gpRsspUnitInfo->MaxNumLink * pUnifyInfo->MaxNumPerCycle;
		protclSndCfgStru[DSU_PROTCL_RSSP].pProtclAppStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU) * tmpVal);
		if ((NULL == protclSndCfgStru[DSU_PROTCL_RSSP].pProtclAppStru) || (NULL == ptemProtclRcvDataStru[DSU_PROTCL_RSSP].pData))
		{
			flag = 1;
		}
		else
		{
			/*为声明的发送数据临时存储区初始化*/
			CommonMemSet(protclSndCfgStru[DSU_PROTCL_RSSP].pProtclAppStru, sizeof(PROTCL_APP_TO_STRU)* tmpVal, 0u, sizeof(PROTCL_APP_TO_STRU)* tmpVal);

			/*初始化单帧应用数据*/
			for (i = 0; i < tmpVal; i++)
			{
				protclSndCfgStru[DSU_PROTCL_RSSP].pProtclAppStru[i].pAppData = NewAppData(&protclSndCfgStru[DSU_PROTCL_RSSP].pProtclAppStru[i].appDataMaxLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == protclSndCfgStru[DSU_PROTCL_RSSP].pProtclAppStru[i].pAppData)
				{
					break;
				}
			}
			if (i == tmpVal)
			{
				pUnifyInfo->GlobalVar->gpRsspRcvDataBuff = NewAppData(&pUnifyInfo->GlobalVar->gRsspRcvDataBuffLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == pUnifyInfo->GlobalVar->gpRsspRcvDataBuff)
				{
					flag = 1;
				}
			}
			else
			{
				flag = 1;
			}

		}
#endif
	}

	if (protclTypeInfoStru->rssp2ProtclNum > 0)
	{
#ifdef USE_RSSPII
		tmpVal = pUnifyInfo->GlobalVar->gpRssp2UnitInfo->MaxNumLink * pUnifyInfo->MaxNumPerCycle;
		Prtcl_RSSP2_CheckInit(&pUnifyInfo->GlobalVar->Rssp2PrtclHead);
		/* add 20161017 xb */
		for (tmpI = 0; tmpI < PRTCL_RQ_NUM; tmpI++)
		{
			rslt = (UINT8)QUEUE_RING_Init(&Prtcl_RQ[tmpI].RingQue, PRTCL_RQ_SIZE);
			if (CM_TRUE != rslt)
			{
				flag = 1;
				break;
			}
			Prtcl_RQ[tmpI].RingQue.m_bIsInited = CM_TRUE;
			Prtcl_RQ[tmpI].RingQue.m_IfCoverQueueFull = CM_TRUE;
			Prtcl_RQ[tmpI].RingQue.m_QueueState = QUEUE_RING_STATE_Empty;
		}

		protclSndCfgStru[DSU_PROTCL_RSSP2].pProtclAppStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU) * tmpVal);
		if ((NULL == protclSndCfgStru[DSU_PROTCL_RSSP2].pProtclAppStru) || (NULL == ptemProtclRcvDataStru[DSU_PROTCL_RSSP2].pData))
		{
			flag = 1;
		}
		else
		{
			/*为声明的发送数据临时存储区初始化*/
			CommonMemSet(protclSndCfgStru[DSU_PROTCL_RSSP2].pProtclAppStru, sizeof(PROTCL_APP_TO_STRU)* tmpVal, 0u, sizeof(PROTCL_APP_TO_STRU)* tmpVal);

			/*初始化单帧应用数据*/
			for (i = 0; i < tmpVal; i++)
			{
				protclSndCfgStru[DSU_PROTCL_RSSP2].pProtclAppStru[i].pAppData = NewAppData(&protclSndCfgStru[DSU_PROTCL_RSSP2].pProtclAppStru[i].appDataMaxLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == protclSndCfgStru[DSU_PROTCL_RSSP2].pProtclAppStru[i].pAppData)
				{
					break;
				}
			}
			if (i == tmpVal)
			{
				pUnifyInfo->GlobalVar->gpRssp2RcvDataBuff = NewAppData(&pUnifyInfo->GlobalVar->gRssp2RcvDataBuffLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == pUnifyInfo->GlobalVar->gpRssp2RcvDataBuff)
				{
					flag = 1;
				}
			}
			else
			{
				flag = 1;
			}

		}
#endif
	}

	if (protclTypeInfoStru->fsfbProtclNum > 0)
	{
#ifdef USE_FSFB
		tmpVal = pUnifyInfo->GlobalVar->gpFsfbUnitInfo->MaxNumLink * pUnifyInfo->MaxNumPerCycle;
		protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU) * tmpVal);
		if ((NULL == protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru) || (NULL == ptemProtclRcvDataStru[DSU_PROTCL_FSFB].pData))
		{
			flag = 1;
		}
		else
		{
			/*为声明的发送数据临时存储区初始化*/
			CommonMemSet(protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru, sizeof(PROTCL_APP_TO_STRU)* tmpVal, 0u, sizeof(PROTCL_APP_TO_STRU)* tmpVal);


			/*初始化单帧应用数据*/
			for (i = 0; i < tmpVal; i++)
			{
				protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru[i].pAppData = NewAppData(&protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru[i].appDataMaxLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru[i].pAppData)
				{
					break;
				}
			}
			if (i == tmpVal)
			{
				pUnifyInfo->GlobalVar->gpFsfbRcvDataBuff = NewAppData(&pUnifyInfo->GlobalVar->gFsfbRcvDataBuffLen, pUnifyInfo->OutputSize, pUnifyInfo->InputSize);
				if (NULL == pUnifyInfo->GlobalVar->gpFsfbRcvDataBuff)
				{
					flag = 1;
				}
			}
			else
			{
				flag = 1;
			}

		}
#endif
	}

	if (1 == flag)
	{
		retVal = 0;
	}
	return retVal;
}

/*
* 功能：协议发送数据
* 参数：
* 输入输出参数：  ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*   1：接收数据成功
*   0：失败
*   2:非宕机错误码
*/
UINT8 SendAppData(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;
	UINT8 temLenbuf[2] = { 0 };
	PROTCL_APP_TO_STRU* tempProtclAppStru = NULL;
	UINT16 locDevName = 0;
	UINT16 destDevName = 0;
	UINT8 protclType = 0;
	UINT32 sendDataLen = 0;
	UINT32 tmpDataLen = 0;
	UINT16 temLnkDataLen = 0;
	UINT16 tempRecordLen = 0;
	UINT16 lnkDataLen = 0;
	UINT16 recordDataLen = 0;

	UINT8 tmpDevDataCnt = 0;
	UINT8 tmpI = 0;
	F_SND tmpSndF;
	UINT8 tmpSign = 0;
	PROTCL_SND_CFG_STRU* protclSndCfgStru = NULL;
	UINT8* pProtclNumBuff = NULL; /*各协议最大链路数*/

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->pSndProtclAppStru) && (NULL != pUnifyInfo->GlobalVar->protclSndCfgStru) && (NULL != pUnifyInfo->GlobalVar->pProtclNumBuff))
	{
		tempProtclAppStru = pUnifyInfo->GlobalVar->pSndProtclAppStru;
		protclSndCfgStru = pUnifyInfo->GlobalVar->protclSndCfgStru;
		pProtclNumBuff = pUnifyInfo->GlobalVar->pProtclNumBuff;

		/* 将应用需要发送的数据写入各协议的队列 */
		TypeIdToDevName(pUnifyInfo->LocalType, pUnifyInfo->LocalID, &locDevName);
		while (2 < QueueStatus(&pUnifyInfo->OutputDataQueue))
		{
			QueueScan(2, temLenbuf, &pUnifyInfo->OutputDataQueue);

			ret = AppSendInputDataAnalysis(&pUnifyInfo->OutputDataQueue, tempProtclAppStru);
			if (1 == ret)
			{
				/* 根据设备类型，决定协议的使用类型 */
				TypeIdToDevName(tempProtclAppStru->devType, tempProtclAppStru->devId, &destDevName);
				protclType = 0;
				DsuProtclGetProtclType(locDevName, destDevName, &protclType, &pUnifyInfo->GlobalVar->DsuProtclBookIpInfo);
			}
			else
			{
				break;
			}
			/* 防越界 */
			if ((PROTCL_MGECFG_MAXCNT >= protclType) && (0 != protclType))
			{
				if (0 < pProtclNumBuff[protclType]) /*有通信设备*/
				{
					/*按帧数填写*/
					tmpDevDataCnt = protclSndCfgStru[protclType].DevDataCnt;

					protclSndCfgStru[protclType].pProtclAppStru[tmpDevDataCnt].appDataMaxLen = tempProtclAppStru->appDataMaxLen;
					protclSndCfgStru[protclType].pProtclAppStru[tmpDevDataCnt].devType = tempProtclAppStru->devType;
					protclSndCfgStru[protclType].pProtclAppStru[tmpDevDataCnt].devId = tempProtclAppStru->devId;
					protclSndCfgStru[protclType].pProtclAppStru[tmpDevDataCnt].devLogId = tempProtclAppStru->devLogId;
					protclSndCfgStru[protclType].pProtclAppStru[tmpDevDataCnt].appDataLen = tempProtclAppStru->appDataLen;
					CommonMemCpy(protclSndCfgStru[protclType].pProtclAppStru[tmpDevDataCnt].pAppData, tempProtclAppStru->appDataLen, tempProtclAppStru->pAppData, tempProtclAppStru->appDataLen);

					protclSndCfgStru[protclType].DevDataCnt++;
				}
			}
			else
			{
				/* break;*/
			}
		}

		/* 执行各协议的发送函数 */
		for (tmpI = 1; tmpI < PROTCL_MGECFG_MAXCNT + 1; tmpI++)
		{
			tmpDataLen = 0;
			temLnkDataLen = 0;
			tempRecordLen = 0;
			if (0 < pProtclNumBuff[tmpI])
			{
				/* 调用发送函数 */
				tmpSndF = protclCfgStruBuff[tmpI].fSnd;
				if (NULL != tmpSndF)
				{
					ret = tmpSndF(pUnifyInfo, protclSndCfgStru[tmpI].DevDataCnt, pUnifyInfo->AppArray + 4 + sendDataLen, &tmpDataLen, pUnifyInfo->VarArray + 2 + lnkDataLen, &temLnkDataLen, pUnifyInfo->RecordArray
						+ 2 + recordDataLen, &tempRecordLen);
				}
				else /*无此协议*/
				{
					temLnkDataLen = 2;
					ShortToChar(0, pUnifyInfo->VarArray + 2 + lnkDataLen);
					ret = 1u;
				}
				/* 增加赋零*/
				protclSndCfgStru[tmpI].DevDataCnt = 0;

			}
			else
			{
				temLnkDataLen = 2;
				ShortToChar(0, pUnifyInfo->VarArray + 2 + lnkDataLen);
				ret = 1;
				/* 无此通信类型 */
			}
			if (1 == ret)
			{
				sendDataLen += tmpDataLen;
				lnkDataLen += temLnkDataLen;
				recordDataLen += tempRecordLen;
			}
			else
			{
				tmpSign = 1;
			}
		}
		/* 发送数据的长度 */
		LongToChar(sendDataLen, pUnifyInfo->AppArray);
		/* 中间变量的长度 */
		ShortToChar(lnkDataLen, pUnifyInfo->VarArray);
		/* 记录数据的长度 */
		ShortToChar(recordDataLen, pUnifyInfo->RecordArray);
	}
	else
	{
		ret = 0;
	}

	if (1 == tmpSign)
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
*    0：失败
*    2:非宕机错误码
*/
UINT8 ReceiveAppData(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;
	UINT8 flag = 0;
	UINT8 lnkState = 0;
	UINT8 i = 0;  /* 循环使用 */
	UINT16 locDevName = 0;
	UINT8 DevType = 0;
	UINT8 DevId = 0;
	UINT8 LogId = 0;
	PROTCL_RCV_DATA_TYPE *ptemProtclRcvDataStru = NULL;
	DSU_PROTCL_TYPE_STRU *protclTypeInfoStru = NULL;
	
#ifdef USE_SFP		
		struc_Unify_Info* gpsfpUnitInfo = NULL;	/*SFP协议结构体*/
#endif
#ifdef USE_RP
		RP_INFO_STRU* gpRedunUnitInfo = NULL;	/*冗余层协议结构体*/
#endif
#ifdef USE_RSR
		RSR_INFO_STRU* gpRsrUnitInfo = NULL;	/*RSR协议结构体*/
#endif
#ifdef USE_RSSPI
		RSSP_INFO_STRU* gpRsspUnitInfo = NULL;	/*RSSP协议结构体*/
#endif
#ifdef USE_RSSPII
		RSSP2_INFO_STRU* gpRssp2UnitInfo = NULL; /*RSSP2协议结构体*/
#endif
#ifdef USE_FSFB
		FSFB_INFO_STRU* gpFsfbUnitInfo = NULL; /*FSFB协议结构体*/
#endif

	if ((NULL!= pUnifyInfo)&&(NULL != pUnifyInfo->GlobalVar))
	{
		ptemProtclRcvDataStru = pUnifyInfo->GlobalVar->RcvData;	/*临时存放协议数据的结构体*/
		protclTypeInfoStru = &pUnifyInfo->GlobalVar->protclTypeInfoStru;	/*各协议通信数量*/

#ifdef USE_SFP		
		gpsfpUnitInfo = pUnifyInfo->GlobalVar->gpsfpUnitInfo;	/*SFP协议结构体*/
#endif
#ifdef USE_RP
		gpRedunUnitInfo = pUnifyInfo->GlobalVar->gpRedunUnitInfo;	/*冗余层协议结构体*/
#endif
#ifdef USE_RSR
		gpRsrUnitInfo = pUnifyInfo->GlobalVar->gpRsrUnitInfo;	/*RSR协议结构体*/
#endif
#ifdef USE_RSSPI
		gpRsspUnitInfo = pUnifyInfo->GlobalVar->gpRsspUnitInfo;	/*RSSP协议结构体*/
#endif
#ifdef USE_RSSPII
		gpRssp2UnitInfo = pUnifyInfo->GlobalVar->gpRssp2UnitInfo; /*RSSP2协议结构体*/
#endif
#ifdef USE_FSFB
		gpFsfbUnitInfo = pUnifyInfo->GlobalVar->gpFsfbUnitInfo; /*FSFB协议结构体*/
#endif

		QueueClear(&pUnifyInfo->DataToApp);
		if (protclTypeInfoStru->sfpProtclNum > 0)
		{
#ifdef USE_SFP
			QueueClear(&gpsfpUnitInfo->OutnetQueueA);
			QueueClear(&gpsfpUnitInfo->OutnetQueueB);
#endif
		}

		if (protclTypeInfoStru->redunProtclNum > 0)
		{
#ifdef USE_RP
			QueueClear(&gpRedunUnitInfo->OutnetQueueA);
			QueueClear(&gpRedunUnitInfo->OutnetQueueB);
#endif
		}

		if (protclTypeInfoStru->rsrProtclNum > 0)
		{
#ifdef USE_RSR
			QueueClear(&gpRsrUnitInfo->OutnetQueueA);
			QueueClear(&gpRsrUnitInfo->OutnetQueueB);
#endif
		}

		if (protclTypeInfoStru->rsspProtclNum > 0)
		{
#ifdef USE_RSSPI
			QueueClear(&gpRsspUnitInfo->OutnetQueueA);
			QueueClear(&gpRsspUnitInfo->OutnetQueueB);
#endif
		}

		if (protclTypeInfoStru->rssp2ProtclNum > 0)
		{
#ifdef USE_RSSPII
			QueueClear(&gpRssp2UnitInfo->OutnetQueueA);
			QueueClear(&gpRssp2UnitInfo->OutnetQueueB);
#endif
		}

		TypeIdToDevName(pUnifyInfo->LocalType, pUnifyInfo->LocalID, &locDevName);

		/* 读取红网队列数据 */
		ret = readQueueDataToApp(&pUnifyInfo->OutnetQueueA, 1, locDevName, ptemProtclRcvDataStru, pUnifyInfo);
		if (0 == ret)
		{
			return ret;
		}

		for (i = 1; i < PROTCL_MGECFG_MAXCNT + 1; i++)
		{
			if (ptemProtclRcvDataStru[i].rcvDataLen > 0)
			{
				switch (ptemProtclRcvDataStru[i].proType)
				{
				case DSU_PROTCL_SFP:
#ifdef USE_SFP
					/* 设置SFP协议的红网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpsfpUnitInfo->OutnetQueueA);
#endif
					break;
				case DSU_PROTCL_RP:
#ifdef USE_RP
					/* 设置RP协议的红网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRedunUnitInfo->OutnetQueueA);
#endif
					break;
				case DSU_PROTCL_RSR:
#ifdef USE_RSR
					/* 设置RSR协议的红网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRsrUnitInfo->OutnetQueueA);
#endif
					break;
				case DSU_PROTCL_RSSP:
#ifdef USE_RSSPI
					/* 设置RSSP协议的红网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRsspUnitInfo->OutnetQueueA);
#endif
					break;
				case DSU_PROTCL_RSSP2:
#ifdef USE_RSSPII
					/* 设置RSSP2协议的红网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRssp2UnitInfo->OutnetQueueA);
#endif
					break;
				case DSU_PROTCL_FSFB:
#ifdef USE_FSFB
					/* 设置FSFB协议的红网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpFsfbUnitInfo->OutnetQueueR);
#endif
					break;
				default:
					break;
				}
			}
		}
		/* 读取蓝网队列数据 */
		ret = readQueueDataToApp(&pUnifyInfo->OutnetQueueB, 2, locDevName, ptemProtclRcvDataStru, pUnifyInfo);
		if (0 == ret)
		{
			return ret;
		}
		/* TO BE DONE switch 挪到readQueueDataToApp 里面 */
		for (i = 1; i < PROTCL_MGECFG_MAXCNT + 1; i++)
		{
			if (ptemProtclRcvDataStru[i].rcvDataLen > 0)
			{
				switch (ptemProtclRcvDataStru[i].proType)
				{
				case DSU_PROTCL_SFP:
#ifdef USE_SFP
					/* 设置SFP协议的蓝网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpsfpUnitInfo->OutnetQueueB);
#endif
					break;
				case DSU_PROTCL_RP:
#ifdef USE_RP
					/* 设置RP协议的蓝网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRedunUnitInfo->OutnetQueueB);
#endif
					break;
				case DSU_PROTCL_RSR:
#ifdef USE_RSR
					/* 设置RSR协议的蓝网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRsrUnitInfo->OutnetQueueB);
#endif
					break;
				case DSU_PROTCL_RSSP:
#ifdef USE_RSSPI
					/* 设置RSSP协议的蓝网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRsspUnitInfo->OutnetQueueB);
#endif
					break;
				case DSU_PROTCL_RSSP2:
#ifdef USE_RSSPII
					/* 设置RSSP2协议的蓝网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpRssp2UnitInfo->OutnetQueueB);
#endif
					break;
				case DSU_PROTCL_FSFB:
#ifdef USE_FSFB
					/* 设置RSSP2协议的蓝网队列数据的帧格式 */
					SetDataToQue(ptemProtclRcvDataStru[i].rcvDataLen, ptemProtclRcvDataStru[i].pData, &gpFsfbUnitInfo->OutnetQueueB);
#endif
					break;
				default:
					break;
				}
			}
		}
		if (protclTypeInfoStru->sfpProtclNum > 0)
		{
#ifdef USE_SFP
			/* SFP协议接收数据 */
			gpsfpUnitInfo->RadamSN = *gpsfpUnitInfo->CycleNum;
#ifdef CI_TASKLOCK
			taskLock();
			ret = ProtclReceive(gpsfpUnitInfo);
			taskUnlock();
#else
			ret = ProtclReceive(gpsfpUnitInfo);
#endif
			if (ret == 1)
			{
				SfpRcvData(&pUnifyInfo->DataToApp, pUnifyInfo);
			}
			else
			{
				flag = 1;
			}
#endif
		}

		if (protclTypeInfoStru->redunProtclNum > 0)
		{
#ifdef USE_RP
			/* RP协议接收数据 */
			ret = RpReceive(gpRedunUnitInfo);
			if (ret == 1)
			{
				RpRcvData(&pUnifyInfo->DataToApp, pUnifyInfo);
			}
			else
			{
				flag = 1;
			}
#endif
		}

		if (protclTypeInfoStru->rsrProtclNum > 0)
		{
#ifdef USE_RSR
			/* RSR协议接收数据 */
			ret = RsrReceive(gpRsrUnitInfo);
			if (ret == 1)
			{
				ret = RsrRcvData(&pUnifyInfo->DataToApp, pUnifyInfo);
			}
			else
			{
				flag = 1;
			}
#endif
		}

		if (protclTypeInfoStru->rsspProtclNum > 0)
		{
#ifdef USE_RSSPI
			/* RSSP协议接收数据 */
			ret = Rssp2Receive(gpRsspUnitInfo);
			if (ret == 1)
			{
				RsspRcvData(&pUnifyInfo->DataToApp, pUnifyInfo);
			}
			else
			{
				flag = 1;
			}
#endif
		}

		if (protclTypeInfoStru->rssp2ProtclNum > 0)
		{
#ifdef USE_RSSPII
			/* RSSP2协议接收数据 */
			ret = RsspIIReceive(gpRssp2UnitInfo);
			if (ret == 1)
			{
				Rssp2RcvData(&pUnifyInfo->DataToApp, pUnifyInfo);
			}
			else
			{
				flag = 1;
			}
#endif
		}

#ifdef USE_RSSPII
		/* 清理环形队列 */
		for (i = 0; i < PRTCL_RQ_NUM; i++)
		{
			if (Prtcl_RQ[i].bIsInit == CM_TRUE)
			{
				DevType = (UINT8)(Prtcl_RQ[i].SourceID / 256 / 256 / 256);
				DevId = (UINT8)(Prtcl_RQ[i].SourceID / 256 / 256 % 256);
				LogId = (UINT8)(Prtcl_RQ[i].SourceID % 256);
				lnkState = GetLnkStatus(DevType, DevId, (UINT16)LogId, pUnifyInfo);
				if (lnkState == 0)
				{
					Prtcl_Manage_DelRQ(Prtcl_RQ[i].SourceID);
				}
			}
		}
#endif

		if (protclTypeInfoStru->fsfbProtclNum > 0)
		{
#ifdef USE_FSFB

			if ((NULL != pUnifyInfo->CycleNum) && (0u == ((*pUnifyInfo->CycleNum) % 2)))/*FSFB协议的周期为600ms*/
			{
				ret = FsfbProtclReceiveData(gpFsfbUnitInfo); /*FSFB协议接收数据*/
				if (ret == 1)
				{
					FsfbRcvData(&pUnifyInfo->DataToApp, pUnifyInfo);
				}
				else
				{
					flag = 1;
				}

				/*通信周期清队列*/
				QueueClear(&gpFsfbUnitInfo->OutnetQueueR);
				QueueClear(&gpFsfbUnitInfo->OutnetQueueB);
			}
#endif
		}


		if (flag == 1)
		{
			ret = 0;
		}
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
UINT8 GetLnkStatus(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;
	UINT16 destDevName = 0;
	UINT16 locDevName = 0;
	UINT8   protclType = 0;
	F_LNK   tmpLnkF = NULL;
	if ((NULL!= pUnifyInfo)&&(NULL != pUnifyInfo->GlobalVar))
	{
		TypeIdToDevName(destType, destId, &destDevName);
		TypeIdToDevName(pUnifyInfo->LocalType, pUnifyInfo->LocalID, &locDevName);
		ret = DsuProtclGetProtclType(locDevName, destDevName, &protclType, &pUnifyInfo->GlobalVar->DsuProtclBookIpInfo);

		if (ret != 0)
		{
			/* 防越界 */
			if ((PROTCL_MGECFG_MAXCNT >= protclType) && (0 != protclType))
			{
				/* 调用具体协议的获取链路状态函数 */
				tmpLnkF = protclCfgStruBuff[protclType].fLnk;
				if (NULL != tmpLnkF)
				{
					ret = tmpLnkF(destType, destId, logId, pUnifyInfo);
				}
				else /*无此协议*/
				{
					ret = 0u;
				}
			}
			else
			{
				ret = 0;
			}

		}
	}
	return ret;
}




/*
* 功能：删除链路
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
*             ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*     1：删除链路成功
*     0：删除链路失败
*/
UINT8 DelLnkFunc(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;
	UINT16 destDevName = 0;
	UINT16 locDevName = 0;
	UINT8   protclType = 0;
	F_DELLNK tmpDelLnkF = NULL;

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar))
	{
		TypeIdToDevName(destType, destId, &destDevName);
		TypeIdToDevName(pUnifyInfo->LocalType, pUnifyInfo->LocalID, &locDevName);
		retVal = DsuProtclGetProtclType(locDevName, destDevName, &protclType, &pUnifyInfo->GlobalVar->DsuProtclBookIpInfo);

		if (retVal != 0)
		{
			/* 防越界 */
			if ((PROTCL_MGECFG_MAXCNT >= protclType) && (0 != protclType))
			{
				/* 调用具体协议的删除链路函数 */
				tmpDelLnkF = protclCfgStruBuff[protclType].fDelLnk;
				if (NULL != tmpDelLnkF)
				{
					retVal = tmpDelLnkF(destType, destId, logId, pUnifyInfo);
				}
				else /*无此协议*/
				{
					retVal = 0u;
				}
			}
			else
			{
				retVal = 0;
			}

		}
	}
	return retVal;
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
UINT8 ReFresh(UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;
	UINT16 inLnkLen = 0;
	UINT16 evrLnkLen = 0;
	UINT16 index = 0;/* 数组偏移量 */
	UINT8 flag = 0;

	UINT8 i = 0;/* 循环使用 */
	F_REFRESHLNK tmpReFreshLnk;

	inLnkLen = ShortFromChar(&pLnkStatusData[0]);
	for (i = 1; i < PROTCL_MGECFG_MAXCNT + 1; i++)
	{
		evrLnkLen = ShortFromChar(&pLnkStatusData[2 + index]);
		index += 2;
		if (evrLnkLen > 0)
		{
			if (evrLnkLen + 2 <= inLnkLen)
			{
				/* 调用具体协议的刷新链路函数 */
				tmpReFreshLnk = protclCfgStruBuff[i].fReFreshLnk;
				if (NULL != tmpReFreshLnk)
				{
					if (DSU_PROTCL_FSFB == i) /*FSFB刷链函数2个长度值*/
					{
						retVal = tmpReFreshLnk(evrLnkLen, &pLnkStatusData[index], pUnifyInfo);
					}
					else /*非FSFB*/
					{
						retVal = tmpReFreshLnk(evrLnkLen, &pLnkStatusData[2 + index], pUnifyInfo);
					}
				}
				else /*无此协议*/
				{
					retVal = 1u;
				}
				index += evrLnkLen;
			}
			else
			{
				flag = 1;
				break;
			}

		}
		else
		{
			retVal = 1;
		}
		inLnkLen = inLnkLen - evrLnkLen - 2;
		if (retVal == 0)
		{
			flag = 1;
		}
	}

	if (flag == 1)
	{
		retVal = 0;
	}
	return retVal;
}





/*
* 协议销毁协议模块
* 参数：
*    ProtclConfigInfoStru *pUnifyInfo 协议结构体
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 FreeSpace(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;
	UINT8 flag = 0;

	UINT8 i = 0;/* 循环使用 */
	F_END tmpEndF = NULL;
	if (NULL != pUnifyInfo)
	{
		for (i = 0; i < PROTCL_MGECFG_MAXCNT + 1; i++)
		{
			/* 调用具体协议的释放空间函数 */
			tmpEndF = protclCfgStruBuff[i].fEnd;
			if (NULL != tmpEndF)
			{
				retVal = tmpEndF(pUnifyInfo);
			}
			else /*无此协议*/
			{
				retVal = 1u;
			}

			if (retVal == 0)
			{
				flag = 1;
			}
		}
		/* 释放适配层分配的结构体空间 */
		FreeSpaceFunc(pUnifyInfo);
		if (flag == 1)
		{
			retVal = 0;
		}
	}
	
	return retVal;
}

UINT8 ReceiveTcpState(UINT16 TcpStateDataLen, UINT8* pTcpStateData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RSSPII
	if ((NULL != pTcpStateData) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRssp2UnitInfo))
	{
		ret = RsspIIInputTcpState(TcpStateDataLen, pTcpStateData, pUnifyInfo->GlobalVar->gpRssp2UnitInfo);
	}
#endif

	return ret;
}

UINT8 SendTcpCtrl(UINT16* pTcpCtrlDataLen, UINT8* pTcpCtrlData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RSSPII
	if ((NULL != pTcpCtrlDataLen) && (NULL != pTcpCtrlData) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRssp2UnitInfo))
	{
		ret = RsspIIOutputTcpCtrl(pTcpCtrlDataLen, pTcpCtrlData, pUnifyInfo->GlobalVar->gpRssp2UnitInfo);
	}

#endif

	return ret;
}
/***********协议初始化相关函数**************************************************************/
/*
* 初始从配置文件读取相对应的协议配置数据
* 输入参数      :UINT8 *pFileName   协议配置数据文件名
*				 ProtclConfigInfoStru *pUnifyInfo
* 输出参数      :UINT8 *pEvenPrtclNumBuff 存放本方通信的相关协议个数
* 全局变量      :
* 返回值        : 成功,返回协议个数;失败返回0
*/
UINT8 InitGetCfgInfo(UINT8* pFileName, UINT32 inDataLen, UINT8 inData[], UINT8* pEvenPrtclNumBuff, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;
	UINT16 locDevName = 0;
	UINT8 tmpIndex = 0;
	UINT16 maxLnkNum = 0;
	DSU_PROTCL_TYPE_STRU tempProtclTypeNum = { 0 };
	if ((NULL!= pFileName)&&(NULL!= pEvenPrtclNumBuff)&&(NULL!= pUnifyInfo)&&(NULL!= pUnifyInfo->GlobalVar))
	{
		/* 本方的设备类型 */
		TypeIdToDevName(pUnifyInfo->LocalType, pUnifyInfo->LocalID, &locDevName);

		ret = DsuCommonInfoInit(pFileName, &pUnifyInfo->GlobalVar->DsuProtclBookIpInfo); /* 查询函数初始化,获取设备IP配置表中的设备IP地址及设备通信协议表数据 */
		if (ret == 0)
		{
			return ret;
		}

		DsuGetProtclTypeInfo(locDevName, &pUnifyInfo->GlobalVar->protclTypeInfoStru, &pUnifyInfo->GlobalVar->DsuProtclBookIpInfo, inDataLen, inData);
		CommonMemCpy(&tempProtclTypeNum, sizeof(DSU_PROTCL_TYPE_STRU), &pUnifyInfo->GlobalVar->protclTypeInfoStru, sizeof(DSU_PROTCL_TYPE_STRU));

		maxLnkNum = tempProtclTypeNum.rsspProtclNum + tempProtclTypeNum.rsrProtclNum + tempProtclTypeNum.redunProtclNum + tempProtclTypeNum.sfpProtclNum + tempProtclTypeNum.rssp2ProtclNum + tempProtclTypeNum.fsfbProtclNum;
		ret = InitProtclInfoStru(pUnifyInfo, maxLnkNum);
		if (ret == 0)
		{
			return ret;
		}

#ifdef USE_SFP
		pEvenPrtclNumBuff[tmpIndex] = tempProtclTypeNum.sfpProtclNum;
#endif
		tmpIndex++;

#ifdef USE_RP
		pEvenPrtclNumBuff[tmpIndex] = tempProtclTypeNum.redunProtclNum;
#endif
		tmpIndex++;

#ifdef USE_RSR
		pEvenPrtclNumBuff[tmpIndex] = tempProtclTypeNum.rsrProtclNum;
#endif
		tmpIndex++;

#ifdef USE_RSSPI
		pEvenPrtclNumBuff[tmpIndex] = tempProtclTypeNum.rsspProtclNum;
#endif
		tmpIndex++;

#ifdef USE_RSSPII
		pEvenPrtclNumBuff[tmpIndex] = tempProtclTypeNum.rssp2ProtclNum;
#endif
		tmpIndex++;

#ifdef USE_FSFB
		pEvenPrtclNumBuff[tmpIndex] = tempProtclTypeNum.fsfbProtclNum;
#endif
		tmpIndex++;


		ret = tmpIndex;
	}
	
	return ret;
}

/******************************************************************************************
* 功能描述      : 初始化统一结构体
1. 动态分配统一结构体空间，并对其进行初始化；
* 输入参数      :

* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0
*******************************************************************************************/
static UINT8 InitProtclInfoStru(ProtclConfigInfoStru* pUnifyInfo, UINT16 maxLnkNum)
{
	UINT8  retnVal = 0;
	UINT32 lTemp = 0;

	/* 协议输入给应用的数据队列长度 */
	lTemp = (pUnifyInfo->InputSize + 7) * maxLnkNum;
	pUnifyInfo->DataToAppSize = lTemp;
	retnVal = QueueInitial(&pUnifyInfo->DataToApp, pUnifyInfo->DataToAppSize);
	if (0 == retnVal)
	{
		return retnVal;
	}


	/* 应用给协议输出的数据队列长度 */
	lTemp = (pUnifyInfo->OutputSize + 6) * maxLnkNum;
	pUnifyInfo->OutputDataQueueSize = lTemp;
	retnVal = QueueInitial(&pUnifyInfo->OutputDataQueue, pUnifyInfo->OutputDataQueueSize);
	if (0 == retnVal)
	{
		return retnVal;
	}


	/*初始化红网接收队列*/
	pUnifyInfo->OutnetQueueSize = (pUnifyInfo->InputSize + 38 + 6) * maxLnkNum * pUnifyInfo->MaxNumPerCycle;
	retnVal = QueueInitial(&pUnifyInfo->OutnetQueueA, pUnifyInfo->OutnetQueueSize);
	if (0 == retnVal)
	{
		return retnVal;
	}


	/* 初始化蓝网接收队列 */
	retnVal = QueueInitial(&pUnifyInfo->OutnetQueueB, pUnifyInfo->OutnetQueueSize);
	if (0 == retnVal)
	{
		return retnVal;
	}

	/* 输出模块输出数组:指向冗余层的输出数组空间 */

	pUnifyInfo->AppArraySize = (pUnifyInfo->OutputSize + 38 + 7) * maxLnkNum + 4;
	pUnifyInfo->AppArray = NULL;
	pUnifyInfo->AppArray = (UINT8*)malloc(pUnifyInfo->AppArraySize);
	if (NULL != pUnifyInfo->AppArray)
	{
		/*初始化*/
		CommonMemSet(pUnifyInfo->AppArray, pUnifyInfo->AppArraySize, 0, pUnifyInfo->AppArraySize);
	}
	else
	{
		retnVal = 0;
		/* 记录 */
		return retnVal;
	}
	/* 输出模块输出中间变量数组大小 */
	pUnifyInfo->VarArraySize = VAR_NODE_LEN * maxLnkNum + 32;
	pUnifyInfo->VarArray = NULL;
	pUnifyInfo->VarArray = (UINT8*)malloc(pUnifyInfo->VarArraySize);
	if (NULL == pUnifyInfo->VarArray)
	{
		retnVal = 0;
		/* 记录 */
		return retnVal;
	}
	else
	{
		/* 初始化 */
		CommonMemSet(pUnifyInfo->VarArray, pUnifyInfo->VarArraySize, 0, pUnifyInfo->VarArraySize);
	}

	/*  记录数组  */
	pUnifyInfo->RecordArray = NULL;
	pUnifyInfo->RecordArray = (UINT8*)malloc(pUnifyInfo->RecordArraySize);
	if (NULL != pUnifyInfo->RecordArray)
	{
		/* 初始化 */
		CommonMemSet(pUnifyInfo->RecordArray, pUnifyInfo->RecordArraySize, 0, pUnifyInfo->RecordArraySize);
	}
	else
	{
		retnVal = 0;
		/* 记录 */
		return retnVal;
	}

	retnVal = 1;
	return retnVal;
}


/******************************************************************************************
* 功能描述      : 初始化SFP协议结构体
1. 动态分配SFP协议结构体空间，并对其进行初始化；
* 输入参数      :UINT8  typeNum   本设备通信使用SFP协议的个数

* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0
*******************************************************************************************/

static UINT8 InitSfpProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_SFP
	UINT32* curDevRecordLen = pUnifyInfo->GlobalVar->curDevRecordLen;
	struc_Unify_Info* gpsfpUnitInfo = NULL;	/*SFP协议结构体*/

	if (typeNum != 0)
	{
		if (gpsfpUnitInfo == NULL)
		{
			gpsfpUnitInfo = (struc_Unify_Info*)malloc(sizeof(struc_Unify_Info));
		}
		if (gpsfpUnitInfo != NULL)
		{
			pUnifyInfo->GlobalVar->gpsfpUnitInfo = gpsfpUnitInfo;	/*设置全局SFP结构*/
			CommonMemSet(gpsfpUnitInfo, sizeof(struc_Unify_Info), 0, sizeof(struc_Unify_Info));
			/* 协议初始化 */
			/* 初始化值设定 */
			gpsfpUnitInfo->CpuId = pUnifyInfo->cpuId;
			gpsfpUnitInfo->CycleNum = pUnifyInfo->CycleNum;
			gpsfpUnitInfo->TimeCounter = pUnifyInfo->CycleNum;
			gpsfpUnitInfo->LocalID = pUnifyInfo->LocalID;		/* 设备ID*/
			gpsfpUnitInfo->LocalType = pUnifyInfo->LocalType;		/* 设备type*/
			gpsfpUnitInfo->MaxNumLink = typeNum;		/* 通信的对象数*/
			gpsfpUnitInfo->MaxNumPerCycle = pUnifyInfo->MaxNumPerCycle; /* 一周期允许每个对象的最大有效数据*/
			gpsfpUnitInfo->InputSize = pUnifyInfo->InputSize;	 /* 接受数据中最大一帧的应用数据长度*/
			gpsfpUnitInfo->OutputSize = pUnifyInfo->OutputSize;	 /* 发送数据中最大一帧的应用数据长度*/
			gpsfpUnitInfo->RecordArraySize = (UINT16)curDevRecordLen[0];	/* 记录数组大小*/
			gpsfpUnitInfo->OutnetQueueSize = typeNum * (pUnifyInfo->InputSize + 23) * 3 * 2 + 100;
			gpsfpUnitInfo->RedundTimeout = 0xffff;
			gpsfpUnitInfo->OutputTimeout = 0xffff;
			gpsfpUnitInfo->RadamSN = 0x1;

			ret = ProtclInit(fileName, inDataLen, inData, gpsfpUnitInfo);

			if (ret != 1)
			{
				return ret;
			}
		}
		else
		{
			ret = 0;
			return ret;
		}
	}
	else
	{
		/* 不做处理 */
	}
	ret = 1;
#endif

	return ret;
}


/******************************************************************************************
* 功能描述      : 初始化Rp协议结构体
1. 动态分配RP协议结构体空间，并对其进行初始化；
* 输入参数      :UINT8  typeNum   本设备通信使用RP协议的个数

* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0
*******************************************************************************************/
static UINT8 InitRpProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RP
	UINT32* curDevRecordLen = pUnifyInfo->GlobalVar->curDevRecordLen;
	RP_INFO_STRU* gpRedunUnitInfo = NULL;	/*冗余层协议结构体*/

	if (typeNum != 0)
	{
		if (gpRedunUnitInfo == NULL)
		{
			gpRedunUnitInfo = (RP_INFO_STRU*)malloc(sizeof(RP_INFO_STRU));

		}
		if (gpRedunUnitInfo != NULL)
		{
			pUnifyInfo->GlobalVar->gpRedunUnitInfo = gpRedunUnitInfo;	/*设置冗余层协议结构体*/

			CommonMemSet(gpRedunUnitInfo, sizeof(RP_INFO_STRU), 0, sizeof(RP_INFO_STRU));
			/* 协议初始化 */
			/* 初始化值设定 */
			gpRedunUnitInfo->LocalID = pUnifyInfo->LocalID;		/* 设备ID*/
			gpRedunUnitInfo->LocalType = pUnifyInfo->LocalType;		/* 设备type*/
			gpRedunUnitInfo->MaxNumLink = typeNum;		/* 冗余层通信的对象数*/
			gpRedunUnitInfo->MaxNumPerCycle = pUnifyInfo->MaxNumPerCycle; /* 冗余层一周期允许每个对象的最大有效数据*/
			gpRedunUnitInfo->InputSize = pUnifyInfo->InputSize;	 /* 冗余层接受数据中最大一帧的应用数据长度*/
			gpRedunUnitInfo->OutputSize = pUnifyInfo->OutputSize;	 /* 冗余层发送数据中最大一帧的应用数据长度*/
			gpRedunUnitInfo->RecordArraySize = (UINT16)curDevRecordLen[1]; /* 记录数组大小*/
			gpRedunUnitInfo->OutnetQueueSize = typeNum * (pUnifyInfo->InputSize + 23) * 3 * 2 + 100;

			ret = RpInit(fileName, gpRedunUnitInfo);

			if (ret != 1)
			{
				return ret;
			}
		}
		else
		{
			ret = 0;
			return ret;
		}
	}
	else
	{
		/* 不做处理 */
	}
	ret = 1;
#endif

	return ret;
}

/******************************************************************************************
* 功能描述      : 初始化RSSP协议结构体
1. 动态分配RSSP协议结构体空间，并对其进行初始化；
* 输入参数      :UINT8  typeNum   本设备通信使用RSSP协议的个数

* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0
*******************************************************************************************/
static UINT8 InitRsspProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RSSPI
	UINT32* curDevRecordLen = pUnifyInfo->GlobalVar->curDevRecordLen;
	RSSP_INFO_STRU* gpRsspUnitInfo = NULL;

	if (0 != typeNum)
	{
		if (NULL == gpRsspUnitInfo)
		{
			gpRsspUnitInfo = (RSSP_INFO_STRU*)malloc(sizeof(RSSP_INFO_STRU));
		}
		if (NULL != gpRsspUnitInfo)
		{
			pUnifyInfo->GlobalVar->gpRsspUnitInfo = gpRsspUnitInfo;

			CommonMemSet(gpRsspUnitInfo, sizeof(RSSP_INFO_STRU), 0, sizeof(RSSP_INFO_STRU));
			/* 协议初始化,初始化值设定 */
			gpRsspUnitInfo->cpuId = pUnifyInfo->cpuId;
			gpRsspUnitInfo->CycleNum = pUnifyInfo->CycleNum;
			gpRsspUnitInfo->LocalID = pUnifyInfo->LocalID;		/* 设备ID*/
			gpRsspUnitInfo->LocalType = pUnifyInfo->LocalType;		/* 设备type*/
			gpRsspUnitInfo->LocalLogicId = pUnifyInfo->LocalLogicId;	/*本方逻辑ID*/
			gpRsspUnitInfo->MaxNumLink = typeNum;		/* 通信的对象数*/
			gpRsspUnitInfo->MaxNumPerCycle = pUnifyInfo->MaxNumPerCycle; /* 一周期允许每个对象的最大有效数据*/
			gpRsspUnitInfo->InputSize = pUnifyInfo->InputSize;	 /* 接受数据中最大一帧的应用数据长度*/
			gpRsspUnitInfo->OutputSize = pUnifyInfo->OutputSize;	 /* 发送数据中最大一帧的应用数据长度*/
			gpRsspUnitInfo->RecordArraySize = (UINT16)curDevRecordLen[3]; /* 记录数组大小*/

			gpRsspUnitInfo->OutnetQueueSize = typeNum * (pUnifyInfo->InputSize + 23) * 3 * 2 + 100;

			ret = Rssp2Init(fileName, inDataLen, inData, gpRsspUnitInfo);

			if (ret != 1)
			{
				return ret;
			}
		}
		else
		{
			ret = 0;
			return ret;
		}
	}
	else
	{
		/* 不做处理 */
	}
	ret = 1;
#endif

	return ret;
}

/******************************************************************************************
* 功能描述      : 初始化RSSP2协议结构体
1. 动态分配RSSP2协议结构体空间，并对其进行初始化；
* 输入参数      :UINT8  typeNum   本设备通信使用RSSP2协议的个数

* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0
*******************************************************************************************/
static UINT8 InitRssp2ProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;
#ifdef USE_RSSPII
	UINT32* curDevRecordLen = pUnifyInfo->GlobalVar->curDevRecordLen;
	RSSP2_INFO_STRU* gpRssp2UnitInfo = NULL;

	if (0 != typeNum)
	{
		if (200 < typeNum)
		{
			typeNum = 200;
		}
		if (NULL == gpRssp2UnitInfo)
		{
			gpRssp2UnitInfo = (RSSP2_INFO_STRU*)malloc(sizeof(RSSP2_INFO_STRU));
		}

		if (NULL != gpRssp2UnitInfo)
		{
			pUnifyInfo->GlobalVar->gpRssp2UnitInfo = gpRssp2UnitInfo;

			CommonMemSet(gpRssp2UnitInfo, sizeof(RSSP2_INFO_STRU), 0, sizeof(RSSP2_INFO_STRU));
			/* 协议初始化,初始化值设定 */
			gpRssp2UnitInfo->CycleNum = pUnifyInfo->CycleNum;
			gpRssp2UnitInfo->RandomNum = pUnifyInfo->CycleNum; /* add 20160819 xb 增加随机数功能 */
			gpRssp2UnitInfo->LocalID = pUnifyInfo->LocalID;		/* 设备ID*/
			gpRssp2UnitInfo->LocalType = pUnifyInfo->LocalType;	/* 设备type*/
			gpRssp2UnitInfo->MaxNumLink = typeNum;		        /* 通信的对象数*/
			gpRssp2UnitInfo->MaxNumPerCycle = pUnifyInfo->MaxNumPerCycle; /* 一周期允许每个对象的最大有效数据*/
			if ((pUnifyInfo->InputSize > RSSP2_MAX_LEN) || (pUnifyInfo->OutputSize > RSSP2_MAX_LEN))
			{
				gpRssp2UnitInfo->InputSize = RSSP2_MAX_LEN;	 /* 接受数据中最大一帧的应用数据长度*/
				gpRssp2UnitInfo->OutputSize = RSSP2_MAX_LEN;	 /* 发送数据中最大一帧的应用数据长度*/
			}
			else
			{
				gpRssp2UnitInfo->InputSize = pUnifyInfo->InputSize;	 /* 接受数据中最大一帧的应用数据长度*/
				gpRssp2UnitInfo->OutputSize = pUnifyInfo->OutputSize;	 /* 发送数据中最大一帧的应用数据长度*/
			}
			gpRssp2UnitInfo->RecordArraySize = (UINT16)curDevRecordLen[4]; /* 记录数组大小*/

			gpRssp2UnitInfo->OutnetQueueSize = typeNum * (pUnifyInfo->InputSize + 23) * 3 * 2 + 100;

			ret = RsspIIInit(fileName, gpRssp2UnitInfo);

			if (ret != 1)
			{
				return ret;
			}
		}
		else
		{
			ret = 0;
			return ret;
		}
	}
	else
	{
		/* 不做处理 */
	}

	ret = 1;
#endif

	return ret;
}


/******************************************************************************************
* 功能描述      : 初始化RSR协议结构体
1. 动态分配RSR协议结构体空间，并对其进行初始化；
* 输入参数      :UINT8  typeNum   本设备通信使用RSR协议的个数

* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0
*******************************************************************************************/
static UINT8 InitRsrProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RSR
	UINT32* curDevRecordLen = pUnifyInfo->GlobalVar->curDevRecordLen;
	RSR_INFO_STRU* gpRsrUnitInfo = NULL;

	if (0 != typeNum)
	{
		if (NULL == gpRsrUnitInfo)
		{
			gpRsrUnitInfo = (RSR_INFO_STRU*)malloc(sizeof(RSR_INFO_STRU));
		}
		if (NULL != gpRsrUnitInfo)
		{
			pUnifyInfo->GlobalVar->gpRsrUnitInfo = gpRsrUnitInfo;

			CommonMemSet(gpRsrUnitInfo, sizeof(RSR_INFO_STRU), 0, sizeof(RSR_INFO_STRU));
			/* 协议初始化,初始化值设定 */
			gpRsrUnitInfo->CycleNum = pUnifyInfo->CycleNum;
			gpRsrUnitInfo->LocalID = pUnifyInfo->LocalID;		/* 设备ID*/
			gpRsrUnitInfo->LocalType = pUnifyInfo->LocalType;		/* 设备type*/
			gpRsrUnitInfo->MaxNumLink = typeNum;		/* 通信的对象数*/
			gpRsrUnitInfo->MaxNumPerCycle = pUnifyInfo->MaxNumPerCycle; /* 一周期允许每个对象的最大有效数据*/
			if ((pUnifyInfo->InputSize > RSR_RSSP_MAX_LEN) || (pUnifyInfo->OutputSize > RSR_RSSP_MAX_LEN))
			{
				gpRsrUnitInfo->InputSize = RSR_RSSP_MAX_LEN;	 /* 接受数据中最大一帧的应用数据长度*/
				gpRsrUnitInfo->OutputSize = RSR_RSSP_MAX_LEN;	 /* 发送数据中最大一帧的应用数据长度*/
			}
			else
			{
				gpRsrUnitInfo->InputSize = pUnifyInfo->InputSize;	 /* 接受数据中最大一帧的应用数据长度*/
				gpRsrUnitInfo->OutputSize = pUnifyInfo->OutputSize;	 /* 发送数据中最大一帧的应用数据长度*/
			}
			gpRsrUnitInfo->RecordArraySize = (UINT16)curDevRecordLen[2]; /* 记录数组大小*/

			gpRsrUnitInfo->OutnetQueueSize = typeNum * (pUnifyInfo->InputSize + 23) * 3 * 2 + 100;

			ret = RsrInit(fileName, gpRsrUnitInfo);

			if (ret != 1)
			{
				return ret;
			}
		}
		else
		{
			ret = 0;
			return ret;
		}
	}
	else
	{
		/* 不做处理 */
	}
	ret = 1;

#endif

	return ret;
}

/******************************************************************************************
* 功能描述      : 初始化FSFB协议结构体
1. 动态分配RSR协议结构体空间，并对其进行初始化；
* 输入参数      :UINT8  typeNum   本设备通信使用RSR协议的个数

* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0
*******************************************************************************************/
static UINT8 InitFsfbProtclStru(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], UINT8 typeNum, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 rtn = 0;

#ifdef USE_FSFB

	FSFB_INFO_STRU* fsfbUnitInfo = NULL;

	if (0 < typeNum) /*有通信设备数量*/
	{
		fsfbUnitInfo = (FSFB_INFO_STRU*)malloc(sizeof(FSFB_INFO_STRU));
		if (NULL != fsfbUnitInfo) /*分配内存成功*/
		{
			pUnifyInfo->GlobalVar->gpFsfbUnitInfo = fsfbUnitInfo;
			CommonMemSet(fsfbUnitInfo, sizeof(FSFB_INFO_STRU), 0, sizeof(FSFB_INFO_STRU));

			fsfbUnitInfo->CycleNum = pUnifyInfo->CycleNum;
			fsfbUnitInfo->LocalType = pUnifyInfo->LocalType;
			fsfbUnitInfo->LocalID = pUnifyInfo->LocalID;
			fsfbUnitInfo->MaxNumLink = typeNum;
			fsfbUnitInfo->MaxNumPerCycle = pUnifyInfo->MaxNumPerCycle;
			if ((pUnifyInfo->InputSize > FSFB_MAX_LEN) || (pUnifyInfo->OutputSize > FSFB_MAX_LEN)) /*超出每帧应用最大长度*/
			{
				fsfbUnitInfo->InputSize = FSFB_MAX_LEN;
				fsfbUnitInfo->OutputSize = FSFB_MAX_LEN;
			}
			else
			{
				fsfbUnitInfo->InputSize = pUnifyInfo->InputSize;
				fsfbUnitInfo->OutputSize = pUnifyInfo->OutputSize;
			}
			fsfbUnitInfo->RecordArraySize = (UINT16)pUnifyInfo->GlobalVar->curDevRecordLen[5]; /* 记录数组大小*/

			rtn = FsfbProtclInit(fileName, fsfbUnitInfo);
		}
		else /*分配内存失败*/
		{
			rtn = 0;
		}
	}
	else /*无通信设备数量*/
	{
		rtn = 1;
	}

#endif

	return rtn;
}

/***********协议发送相关函数**************************************************************/
/*
* 功能：应用发送的数据解析
*
* 参数：
* 输入参数：  const QueueStruct *pAppQueueData    应用输入到协议的数据队列
* 输出参数：  PROTCL_APP_TO_STRU *protclAppStru   解析后的数据
* 返回值：
*    UINT8 1 收到完整数据帧，0 无数据
*/
UINT8 AppSendInputDataAnalysis(QueueStruct* pAppQueueData, PROTCL_APP_TO_STRU* pProtclAppStru)
{
	UINT8 retVal = 0;
	UINT8 tempBuff[2] = { 0 };
	UINT16 tempDatalen = 0;

	QueueScan(2, tempBuff, pAppQueueData);	/* 肯定存在两个字节 */
	tempDatalen = ShortFromChar(tempBuff);
	if (((tempDatalen + 2) <= QueueStatus(pAppQueueData)) && (4 <= tempDatalen))
	{
		pProtclAppStru->appDataLen = tempDatalen - 4;
		QueueRead(2, tempBuff, pAppQueueData);/* 长度 */
		QueueRead(1, &pProtclAppStru->devType, pAppQueueData);/* 类型 */
		QueueRead(1, &pProtclAppStru->devId, pAppQueueData);/* ID */
		QueueRead(2, tempBuff, pAppQueueData);/* LogicID */
		pProtclAppStru->devLogId = ShortFromChar(tempBuff);
		QueueRead((tempDatalen - 4), pProtclAppStru->pAppData, pAppQueueData);/* 应用数据 */

		retVal = 1;
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}

/*
* 功能：SFP协议入口组帧
*
* 参数：
* 输入参数：  const PROTCL_APP_TO_STRU *protclAppStru   需要组的数据
* 输出参数：  QueueStruct *pAppQueueData    协议入口数据队列
* 返回值：
*    UINT8 1 正常，0 不正常
*/
UINT8 sfpInputPackFms(const PROTCL_APP_TO_STRU protclAppStru, QueueStruct* pAppQueueData)
{
	UINT8 retVal = 0;
	UINT8 tmpHeadBuf[6] = { 0 };

	if (protclAppStru.appDataLen > 0)
	{
		/* 长度 */
		ShortToChar(protclAppStru.appDataLen + 4, tmpHeadBuf);
		/* 2个字节的预留 */
		tmpHeadBuf[2] = 0;
		tmpHeadBuf[3] = 0;
		/* 设备ID */
		tmpHeadBuf[4] = protclAppStru.devId;
		/* 设备类型 */
		tmpHeadBuf[5] = protclAppStru.devType;
		QueueWrite(6, tmpHeadBuf, pAppQueueData);
		QueueWrite(protclAppStru.appDataLen, protclAppStru.pAppData, pAppQueueData);

		retVal = 1;
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}


/*
* 功能：SFP协议发送数据
*
* 参数：
* 输入参数：  devDataCount	设备数据个数	UINT8
* 输出参数：
*             sendDataLen	发送数据长度	UINT32*
*             pSndData	    发送数据	    UINT8*
*             pLnkStatusData	协议状态数据	UINT8*
*             lnkDataLen	协议状态数据长度	UINT16*
*             recordArray 	协议记录数据	UINT8*
*             recordDataLen	协议记录数据长度	UINT16*

* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 sfpSendData(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen)
{
	UINT8 ret = 0;

#ifdef USE_SFP
	UINT8 i = 0;/* 循环使用 */
	UINT32 len = 0;
	UINT32 dataLen = 0;
	UINT16 lenInput = 0;
	UINT16 tempRecordSize = 0;
	PROTCL_APP_TO_STRU* pAppDataStru = pUnifyInfo->GlobalVar->protclSndCfgStru[DSU_PROTCL_SFP].pProtclAppStru;
	struc_Unify_Info* gpsfpUnitInfo = pUnifyInfo->GlobalVar->gpsfpUnitInfo;	/*SFP协议结构体*/

	QueueClear(&gpsfpUnitInfo->OutputDataQueue);

	if (pAppDataStru != NULL)
	{
		for (i = 0; i < devDataCount; i++)
		{
			sfpInputPackFms(pAppDataStru[i], &gpsfpUnitInfo->OutputDataQueue);
		}
#ifdef CI_TASKLOCK
		taskLock();
		ret = ProtclOutput(gpsfpUnitInfo);
		taskUnlock();
#else
		ret = ProtclOutput(gpsfpUnitInfo);
#endif
		if (ret == 1)
		{
			/* 该长度包含中间变量和报文数据 丢弃该长度 */
			/* 从数据长度0到通信链路管理表的数据长度 4字节 */
			len = LongFromChar(gpsfpUnitInfo->PrtclSndBuff);
			/* 报文的总长度*/
			if (len > 4)
			{
				dataLen = LongFromChar(gpsfpUnitInfo->PrtclSndBuff + 4);

				*sendDataLen = dataLen;
				CommonMemCpy(pSndData, dataLen, gpsfpUnitInfo->PrtclSndBuff + 8, dataLen);
				if (len - dataLen - 4 > 0)
				{

					lenInput = (UINT16)(len - dataLen - 4);
					(*lnkDataLen) = lenInput + 2;
					ShortToChar(lenInput, pLnkStatusData);
					CommonMemCpy(&pLnkStatusData[2], lenInput, gpsfpUnitInfo->PrtclSndBuff + 8 + dataLen, lenInput);
				}
				else
				{
					/* 不做任何处理 */
					*sendDataLen = 0;
					ShortToChar(0, pLnkStatusData);
					*lnkDataLen = 2;
				}
			}
			else
			{
				/* 不做处理 */
				*sendDataLen = 0;
				ShortToChar(0, pLnkStatusData);
				*lnkDataLen = 2;
			}
		}
		else
		{
			pSndData = NULL;
			pLnkStatusData = NULL;

			*sendDataLen = 0;
			*lnkDataLen = 0;
		}

		tempRecordSize = ShortFromChar(gpsfpUnitInfo->RecordArray) + 1;
		if (gpsfpUnitInfo->RecordArraySize > 3)
		{
			if (gpsfpUnitInfo->RecordArraySize >= (tempRecordSize + 2))
			{
				ShortToChar(0, gpsfpUnitInfo->RecordArray);
				ShortToChar(tempRecordSize, recordArray);
				CommonMemCpy(&recordArray[2], tempRecordSize - 1, &gpsfpUnitInfo->RecordArray[2], tempRecordSize - 1);
				recordArray[2 + tempRecordSize - 1] = '\0';
				*recordDataLen = tempRecordSize + 2;
			}
			else
			{
				ShortToChar(0, gpsfpUnitInfo->RecordArray);
				ShortToChar(gpsfpUnitInfo->RecordArraySize - 2, recordArray);
				CommonMemCpy(&recordArray[2], gpsfpUnitInfo->RecordArraySize - 3, &gpsfpUnitInfo->RecordArray[2], gpsfpUnitInfo->RecordArraySize - 3);
				recordArray[gpsfpUnitInfo->RecordArraySize - 1] = '\0';
				*recordDataLen = gpsfpUnitInfo->RecordArraySize;
			}
		}

	}
	else
	{
		ret = 0;
		pSndData = NULL;
		pLnkStatusData = NULL;
		ShortToChar(0, recordArray);
		*sendDataLen = 0;
		*lnkDataLen = 0;
		*recordDataLen = 0;
	}
#endif

	return ret;
}


/*
* 功能：RSSP协议入口组帧
*
* 参数：
* 输入参数：  const PROTCL_APP_TO_STRU *protclAppStru   需要组的数据
* 输出参数：  QueueStruct *pAppQueueData    协议入口数据队列
* 返回值：
*    UINT8 1 正常，0 不正常
*/
UINT8 RsspInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct* pAppQueueData)
{
	UINT8 retVal = 0;
	UINT8 tmpHeadBuf[6] = { 0 };

	if (protclAppStru.appDataLen > 0)
	{
		/* 两个字节的长度 */
		ShortToChar(protclAppStru.appDataLen + 4, tmpHeadBuf);
		/*一个字节的设备ID */
		tmpHeadBuf[3] = protclAppStru.devId;
		/*一个字节的设备类型 */
		tmpHeadBuf[2] = protclAppStru.devType;
		/*2个字节逻辑ID*/
		ShortToChar(protclAppStru.devLogId, &tmpHeadBuf[4]);
		QueueWrite(6, tmpHeadBuf, pAppQueueData);
		QueueWrite(protclAppStru.appDataLen, protclAppStru.pAppData, pAppQueueData);

		retVal = 1;
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}


/*
* 功能：RSSP协议发送数据
*
* 参数：
* 输入参数：  devDataCount	设备数据个数	UINT8
* 输出参数：  sendDataLen	发送数据长度	UINT32*
*             pSndData	    发送数据	    UINT8*
*             pLnkStatusData	协议状态数据	UINT8*
*             lnkDataLen	协议状态数据长度	UINT16*
*             recordArray 	协议记录数据	UINT8*
*             recordDataLen	协议记录数据长度	UINT16*
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 RsspSendData(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen)
{
	UINT8 ret = 0;

#ifdef USE_RSSPI
	UINT8 i = 0;/* 循环使用 */
	UINT32 len = 0;
	UINT16 varlen = 0;
	UINT16 tempRecordSize = 0;
	PROTCL_APP_TO_STRU* pAppDataStru = NULL;
	RSSP_INFO_STRU* gpRsspUnitInfo = NULL;

	if ((NULL != pUnifyInfo) && (NULL != sendDataLen) && (NULL != lnkDataLen) && (NULL != recordArray) && (NULL != recordDataLen) && (NULL != pUnifyInfo->GlobalVar))
	{
		pAppDataStru = pUnifyInfo->GlobalVar->protclSndCfgStru[DSU_PROTCL_RSSP].pProtclAppStru;
		gpRsspUnitInfo = pUnifyInfo->GlobalVar->gpRsspUnitInfo;

		if ((pAppDataStru != NULL) && (pSndData != NULL) && (pLnkStatusData != NULL))
		{
			QueueClear(&gpRsspUnitInfo->OutputDataQueue);

			for (i = 0; i < devDataCount; i++)
			{
				RsspInputPackFms(pAppDataStru[i], &gpRsspUnitInfo->OutputDataQueue);
			}


			ret = Rssp2Output(gpRsspUnitInfo);
			if (ret == 1)
			{
				/* 整个数组长度 4字节 */
				len = LongFromChar(gpRsspUnitInfo->AppArray);
				*sendDataLen = len;

				/* 中间变量整个数组长度 2字节 */
				varlen = ShortFromChar(gpRsspUnitInfo->VarArray);
				*lnkDataLen = varlen + 2;
				/* 报文的总长度*/
				if (len > 2)
				{
					CommonMemCpy(pSndData, len, gpRsspUnitInfo->AppArray + 4, len);
				}
				else
				{
					/* 不做处理 */
					*sendDataLen = 0;
					/**lnkDataLen = 0;*/
				}
				if (varlen > 2)
				{
					CommonMemCpy(pLnkStatusData, varlen + 2, gpRsspUnitInfo->VarArray, varlen + 2);

				}
				else
				{
					/* 不做处理 */
					ShortToChar(0, pLnkStatusData);
					*lnkDataLen = 2;
				}
			}
			else
			{
				pSndData = NULL;
				pLnkStatusData = NULL;
				*sendDataLen = 0;
				*lnkDataLen = 0;
				ret = 0;
			}

			tempRecordSize = ShortFromChar(gpRsspUnitInfo->RecordArray) + 1;
			if (gpRsspUnitInfo->RecordArraySize > 3)
			{
				if (gpRsspUnitInfo->RecordArraySize >= (tempRecordSize + 2))
				{
					ShortToChar(0, gpRsspUnitInfo->RecordArray);
					ShortToChar(tempRecordSize, recordArray);
					CommonMemCpy(&recordArray[2], tempRecordSize - 1, &gpRsspUnitInfo->RecordArray[2], tempRecordSize - 1);
					recordArray[2 + tempRecordSize - 1] = '\0';
					*recordDataLen = tempRecordSize + 2;
				}
				else
				{
					ShortToChar(0, gpRsspUnitInfo->RecordArray);
					ShortToChar(gpRsspUnitInfo->RecordArraySize - 2, recordArray);
					CommonMemCpy(&recordArray[2], gpRsspUnitInfo->RecordArraySize - 3, &gpRsspUnitInfo->RecordArray[2], gpRsspUnitInfo->RecordArraySize - 3);
					recordArray[gpRsspUnitInfo->RecordArraySize - 1] = '\0';
					*recordDataLen = gpRsspUnitInfo->RecordArraySize;
				}
			}
		}
		else
		{
			pSndData = NULL;
			pLnkStatusData = NULL;
			ShortToChar(0, recordArray);
			*sendDataLen = 0;
			*lnkDataLen = 0;
			*recordDataLen = 0;
			ret = 0;
		}
	}


#endif

	return ret;
}


/*
* 功能：RSSP2协议入口组帧
*
* 参数：
* 输入参数：  const PROTCL_APP_TO_STRU *protclAppStru   需要组的数据
* 输出参数：  QueueStruct *pAppQueueData    协议入口数据队列
* 返回值：
*    UINT8 1 正常，0 不正常
*/
UINT8 Rssp2InputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct* pAppQueueData)
{
	UINT8 retVal = 0;
	UINT8 tmpHeadBuf[6] = { 0 };

	if (protclAppStru.appDataLen > 0)
	{
		/* 两个字节的长度 */
		ShortToChar(protclAppStru.appDataLen + 4, tmpHeadBuf);
		/*一个字节的设备ID */
		tmpHeadBuf[3] = protclAppStru.devId;
		/*一个字节的设备类型 */
		tmpHeadBuf[2] = protclAppStru.devType;
		/*2个字节的逻辑ID*/
		ShortToChar(protclAppStru.devLogId, &tmpHeadBuf[4]);
		QueueWrite(6, tmpHeadBuf, pAppQueueData);
		QueueWrite(protclAppStru.appDataLen, protclAppStru.pAppData, pAppQueueData);

		retVal = 1;
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}

/*
* 功能：RSSP2协议发送数据
*
* 参数：
* 输入参数：  devDataCount	设备数据个数	UINT8
* 输出参数：  sendDataLen	发送数据长度	UINT32*
*             pSndData	    发送数据	    UINT8*
*             pLnkStatusData	协议状态数据	UINT8*
*             lnkDataLen	协议状态数据长度	UINT16*
*             recordArray 	协议记录数据	UINT8*
*             recordDataLen	协议记录数据长度	UINT16*
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 Rssp2SendData(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen)
{
	UINT8 ret = 0;
#ifdef USE_RSSPII
	UINT8 i = 0;/* 循环使用 */
	UINT32 len = 0;
	UINT16 varlen = 0;
	UINT16 recordLen = 0;
	PROTCL_APP_TO_STRU* pAppDataStru = pUnifyInfo->GlobalVar->protclSndCfgStru[DSU_PROTCL_RSSP2].pProtclAppStru;
	RSSP2_INFO_STRU* gpRssp2UnitInfo = pUnifyInfo->GlobalVar->gpRssp2UnitInfo;

	if ((pAppDataStru != NULL) && (pSndData != NULL) && (pLnkStatusData != NULL))
	{
		QueueClear(&gpRssp2UnitInfo->OutputDataQueue);

		for (i = 0; i < devDataCount; i++)
		{
			Rssp2InputPackFms(pAppDataStru[i], &gpRssp2UnitInfo->OutputDataQueue);
		}


		ret = RsspIIOutput(gpRssp2UnitInfo);

		if (ret == 1)
		{

			/* 报文的总长度*/
			len = (UINT32)(gpRssp2UnitInfo->AppArraySize);
			if (len > 2)
			{
				CommonMemCpy(pSndData, len, gpRssp2UnitInfo->AppArray, len);
				*sendDataLen = len;
			}
			else
			{
				/* 不做处理 */
				*sendDataLen = 0;
			}
			/* 中间变量整个数组长度 2字节 */
			varlen = ShortFromChar(gpRssp2UnitInfo->VarArray);
			if (varlen > 2)
			{
				CommonMemCpy(pLnkStatusData, varlen + 2, gpRssp2UnitInfo->VarArray, varlen + 2);
				*lnkDataLen = varlen + 2;
			}
			else
			{
				/* 不做处理 */
				ShortToChar(0, pLnkStatusData);
				*lnkDataLen = 2;
			}

			recordLen = ShortFromChar(gpRssp2UnitInfo->RecordArray);
			if (recordLen > 2)
			{
				CommonMemCpy(recordArray, recordLen + 2, gpRssp2UnitInfo->RecordArray, recordLen + 2);
				*recordDataLen = recordLen + 2;
			}
			else
			{
				ShortToChar(0, recordArray);
				*recordDataLen = 2;
			}
		}
		else
		{
			pSndData = NULL;
			pLnkStatusData = NULL;
			*sendDataLen = 0;
			*lnkDataLen = 0;
			*recordDataLen = 0;
			ret = 0;
		}
	}
	else
	{
		pSndData = NULL;
		pLnkStatusData = NULL;
		ShortToChar(0, recordArray);
		*sendDataLen = 0;
		*lnkDataLen = 0;
		*recordDataLen = 0;
		ret = 0;
	}
#endif

	return ret;
}




#ifdef USE_RSR
/*
* 功能：RSR协议入口组帧
*
* 参数：
* 输入参数：  const PROTCL_APP_TO_STRU *protclAppStru   需要组的数据
* 输出参数：  QueueStruct *pAppQueueData    协议入口数据队列
* 返回值：
*    UINT8 1 正常，0 不正常
*/
UINT8 RsrInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct* pAppQueueData, RSR_INFO_STRU* gpRsrUnitInfo)
{
	UINT8 retVal = 0;
	UINT8 tmpHeadBuf[8] = { 0 };
	UINT16 destDevName = 0;
	UINT16 locDevName = 0;
	UINT8 index = 0;

	UINT16 rsspSrcAdd = 0;
	INT16 bsdAdd = 0;
	UINT16 destRsspAdd = 0;

	if (protclAppStru.appDataLen > 0)
	{
		TypeIdToDevName(protclAppStru.devType, protclAppStru.devId, &destDevName);
		TypeIdToDevName(gpRsrUnitInfo->LocalType, gpRsrUnitInfo->LocalID, &locDevName);
		if (protclAppStru.devType == VOBC_TYPE)
		{
			retVal = dsuVOBCvsCIOutInfo(protclAppStru.devLogId, 0, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
		}
		else if (gpRsrUnitInfo->LocalType == VOBC_TYPE)
		{
			retVal = dsuVOBCvsCIOutInfo(protclAppStru.devLogId, 1, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
		}
		else
		{
			retVal = dsuZCvsCIOutInfo(locDevName, destDevName, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
		}
		if (retVal != 0)
		{
			/* 两个字节的长度 */
			ShortToChar(protclAppStru.appDataLen + 6, tmpHeadBuf);
			index += 2;
			/* 两个字节的源地址 */
			ShortToChar(rsspSrcAdd, tmpHeadBuf + index);
			index += 2;
			/* 两个字节的BSD地址 */
			ShortToChar(bsdAdd, tmpHeadBuf + index);
			index += 2;
			/* 两个字节的目标地址 */
			ShortToChar(destRsspAdd, tmpHeadBuf + index);
			QueueWrite(8, tmpHeadBuf, pAppQueueData);
			QueueWrite(protclAppStru.appDataLen, protclAppStru.pAppData, pAppQueueData);
		}
		else
		{
			retVal = 0;
		}
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}
#endif


/*
* 功能：RSR协议发送数据
*
* 参数：
* 输入参数：  devDataCount	设备数据个数	UINT8
* 输出参数：  sendDataLen	发送数据长度	UINT32*
*             pSndData	    发送数据	    UINT8*
*             pLnkStatusData	协议状态数据	UINT8*
*             lnkDataLen	协议状态数据长度	UINT16*
*             recordArray 	协议记录数据	UINT8*
*             recordDataLen	协议记录数据长度	UINT16*
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 RsrSendData(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen)
{
	UINT8 ret = 0;

#ifdef USE_RSR
	UINT8 i = 0;
	UINT16 len = 0;
	UINT16 varlen = 0;
	UINT16 tempRecordSize = 0;
	PROTCL_APP_TO_STRU* pAppDataStru = pUnifyInfo->GlobalVar->protclSndCfgStru[DSU_PROTCL_RSR].pProtclAppStru;
	RSR_INFO_STRU* gpRsrUnitInfo = pUnifyInfo->GlobalVar->gpRsrUnitInfo;

	UINT8 protclType = GetCbtcSysType();

	if ((pAppDataStru != NULL) && (pSndData != NULL) && (pLnkStatusData != NULL))
	{
		QueueClear(&gpRsrUnitInfo->OutputDataQueue);
		for (i = 0; i < devDataCount; i++)
		{
			RsrInputPackFms(pAppDataStru[i], &gpRsrUnitInfo->OutputDataQueue, gpRsrUnitInfo);
		}

		ret = RsrOutput(gpRsrUnitInfo);
		if (ret == 1)
		{
			/* 整个数组长度 2字节 */
			len = ShortFromChar(gpRsrUnitInfo->AppArray);
			if (DQU_CBTC_HLT != protclType)
			{
				*sendDataLen = len;
			}
			/* 中间变量整个数组长度 2字节 */
			varlen = ShortFromChar(gpRsrUnitInfo->VarArray);
			/* yw.lou 和RP一样也需要处理不同的情况*/
			/* 报文的总长度*/
			if (len > 2)
			{
				if (DQU_CBTC_HLT == protclType)
				{
					RsrAndRpAppArrayPage(gpRsrUnitInfo->AppArray + 2, len, pSndData, sendDataLen);
				}
				else
				{
					CommonMemCpy(pSndData, len, gpRsrUnitInfo->AppArray + 2, len);
				}
			}
			else
			{
				/* 不做处理 */
				*sendDataLen = 0;
			}
			if (varlen > 2)
			{
				*lnkDataLen = varlen + 2;
				CommonMemCpy(pLnkStatusData, varlen + 2, gpRsrUnitInfo->VarArray, varlen + 2);

			}
			else
			{
				/* 不做处理 */
				ShortToChar(0, pLnkStatusData);

				*lnkDataLen = 2;
			}
		}
		else
		{
			pSndData = NULL;
			pLnkStatusData = NULL;
			*lnkDataLen = 0;
			*sendDataLen = 0;
			ret = 0;
		}

		tempRecordSize = ShortFromChar(gpRsrUnitInfo->RecordArray) + 1;
		if (gpRsrUnitInfo->RecordArraySize > 3)
		{
			if (gpRsrUnitInfo->RecordArraySize >= (tempRecordSize + 2))
			{
				ShortToChar(0, gpRsrUnitInfo->RecordArray);
				ShortToChar(tempRecordSize, recordArray);
				CommonMemCpy(&recordArray[2], tempRecordSize - 1, &gpRsrUnitInfo->RecordArray[2], tempRecordSize - 1);
				recordArray[2 + tempRecordSize - 1] = '\0';
				*recordDataLen = tempRecordSize + 2;
			}
			else
			{
				ShortToChar(0, gpRsrUnitInfo->RecordArray);
				ShortToChar(gpRsrUnitInfo->RecordArraySize - 2, recordArray);
				CommonMemCpy(&recordArray[2], gpRsrUnitInfo->RecordArraySize - 3, &gpRsrUnitInfo->RecordArray[2], gpRsrUnitInfo->RecordArraySize - 3);
				recordArray[gpRsrUnitInfo->RecordArraySize - 1] = '\0';
				*recordDataLen = gpRsrUnitInfo->RecordArraySize;
			}
		}

	}
	else
	{
		pSndData = NULL;
		pLnkStatusData = NULL;
		ShortToChar(0, recordArray);
		*sendDataLen = 0;
		*lnkDataLen = 0;
		*recordDataLen = 0;
		ret = 0;
	}
#endif

	return ret;
}

/*
* 功能：RP协议入口组帧
*
* 参数：
* 输入参数：  const PROTCL_APP_TO_STRU *protclAppStru   需要组的数据
* 输出参数：  QueueStruct *pAppQueueData    协议入口数据队列
* 返回值：
*    UINT8 1 正常，0 不正常
*/
UINT8 RpInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct* pAppQueueData)
{
	UINT8 retVal = 0;
	UINT8 tmpHeadBuf[4] = { 0 };

	if (protclAppStru.appDataLen > 0)
	{
		/* 两个字节的长度 */
		ShortToChar(protclAppStru.appDataLen + 2, tmpHeadBuf);
		/* 1个字节的设备类型 */
		tmpHeadBuf[2] = protclAppStru.devType;
		/* 1个字节的设备ID */
		tmpHeadBuf[3] = protclAppStru.devId;
		QueueWrite(4, tmpHeadBuf, pAppQueueData);
		QueueWrite(protclAppStru.appDataLen, protclAppStru.pAppData, pAppQueueData);

		retVal = 1;
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}


/*
* 功能：RP协议发送数据
*
* 参数：
* 输入参数：  devDataCount	设备数据个数	UINT8
* 输出参数：  sendDataLen	发送数据长度	UINT32*
*             pSndData	    发送数据	    UINT8*
*             pLnkStatusData	协议状态数据	UINT8*
*             lnkDataLen	协议状态数据长度	UINT16*
*             recordArray 	协议记录数据	UINT8*
*             recordDataLen	协议记录数据长度	UINT16*
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 RpSendData(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen)
{
	UINT8 ret = 0;

#ifdef USE_RP
	UINT8 i = 0;
	UINT16 len = 0;
	UINT16 varlen = 0;
	UINT16 tempRecordSize = 0;
	PROTCL_APP_TO_STRU* pAppDataStru = pUnifyInfo->GlobalVar->protclSndCfgStru[DSU_PROTCL_RP].pProtclAppStru;
	RP_INFO_STRU* gpRedunUnitInfo = pUnifyInfo->GlobalVar->gpRedunUnitInfo;	/*冗余层协议结构体*/

	if ((pAppDataStru != NULL) && (pSndData != NULL) && (pLnkStatusData != NULL))
	{
		QueueClear(&gpRedunUnitInfo->OutputDataQueue);
		for (i = 0; i < devDataCount; i++)
		{
			RpInputPackFms(pAppDataStru[i], &gpRedunUnitInfo->OutputDataQueue);
		}


		ret = RpOutput(gpRedunUnitInfo);
		if (ret == 1)
		{
			/* 整个数组长度 2字节 */
			len = ShortFromChar(gpRedunUnitInfo->AppArray);
			/* 中间变量整个数组长度 2字节 */
			varlen = ShortFromChar(gpRedunUnitInfo->VarArray);

			/*yw.lou 修改: 由于发送应用数据可能存在为空的情况，而此时有状态数据，则就存在问题*/
			/* 报文的总长度*/
			if ((len > 2))
			{
				CommonMemCpy(pSndData, len, gpRedunUnitInfo->AppArray + 2, len);
				*sendDataLen = len;
			}
			else
			{
				/* 不做处理 */
				*sendDataLen = 0;
			}
			if (varlen > 2)
			{
				*lnkDataLen = varlen + 2;
				CommonMemCpy(pLnkStatusData, varlen + 2, gpRedunUnitInfo->VarArray, varlen + 2);

			}
			else
			{
				/* 不做处理 */
				ShortToChar(0, pLnkStatusData);
				*lnkDataLen = 2;
			}
		}
		else
		{
			pSndData = NULL;
			pLnkStatusData = NULL;
			*sendDataLen = 0;
			*lnkDataLen = 0;
			ret = 0;
		}

		tempRecordSize = ShortFromChar(gpRedunUnitInfo->RecordArray) + 1;
		if (gpRedunUnitInfo->RecordArraySize > 3)
		{
			if (gpRedunUnitInfo->RecordArraySize >= (tempRecordSize + 2))
			{
				ShortToChar(0, gpRedunUnitInfo->RecordArray);
				ShortToChar(tempRecordSize, recordArray);
				CommonMemCpy(&recordArray[2], tempRecordSize - 1, &gpRedunUnitInfo->RecordArray[2], tempRecordSize - 1);
				recordArray[2 + tempRecordSize - 1] = '\0';
				*recordDataLen = tempRecordSize + 2;
			}
			else
			{
				ShortToChar(0, gpRedunUnitInfo->RecordArray);
				ShortToChar(gpRedunUnitInfo->RecordArraySize - 2, recordArray);
				CommonMemCpy(&recordArray[2], gpRedunUnitInfo->RecordArraySize - 3, &gpRedunUnitInfo->RecordArray[2], gpRedunUnitInfo->RecordArraySize - 3);
				recordArray[gpRedunUnitInfo->RecordArraySize - 1] = '\0';
				*recordDataLen = gpRedunUnitInfo->RecordArraySize;
			}
		}
	}
	else
	{
		pSndData = NULL;
		pLnkStatusData = NULL;
		ShortToChar(0, recordArray);
		recordArray = NULL;
		*sendDataLen = 0;
		*lnkDataLen = 0;
		recordDataLen = 0;
		ret = 0;
	}
#endif

	return ret;
}


/*
* 功能：FSFB协议入口组帧
*
* 参数：
* 输入参数：  const PROTCL_APP_TO_STRU *protclAppStru   需要组的数据
* 输出参数：  QueueStruct *pAppQueueData    协议入口数据队列
* 返回值：
*    UINT8 1 正常，0 不正常
*/
UINT8 FsfbInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct* pAppQueueData)
{
	UINT8 retVal = 0u; /*返回值*/
	UINT8 headBuf[5] = { 0 }; /*帧头数组*/

	if (protclAppStru.appDataLen > 0)
	{
		ShortToChar(protclAppStru.appDataLen + 3, headBuf); /*2字节长度*/
		headBuf[2] = protclAppStru.devType; /*1字节设备类型*/
		headBuf[3] = protclAppStru.devId; /*1字节设备ID*/
		headBuf[4] = 0x01; /*1字节帧类型*/

		QueueWrite(5, headBuf, pAppQueueData);
		QueueWrite(protclAppStru.appDataLen, protclAppStru.pAppData, pAppQueueData);

		retVal = 1u;
	}

	return retVal;
}

/*
* 功能：FSFB协议发送数据
*
* 参数：
* 输入参数：  devDataCount	设备数据个数	UINT8
* 输出参数：  sendDataLen	发送数据长度	UINT32*
*             pSndData	    发送数据	    UINT8*
*             pLnkStatusData	协议状态数据	UINT8*
*             lnkDataLen	协议状态数据长度	UINT16*
*             recordArray 	协议记录数据	UINT8*
*             recordDataLen	协议记录数据长度	UINT16*
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 FsfbSendData(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen)
{
	UINT8 retVal = 0u;

#ifdef USE_FSFB

	UINT32* curCycleNum = NULL; /*当前周期*/
	PROTCL_APP_TO_STRU* appToStru = NULL; /*应用到协议的数据结构体*/
	FSFB_INFO_STRU* fsfbInfo = NULL; /*FSFB协议结构体*/
	UINT8 i = 0u;/*用于循环*/
	UINT32 outQueDataLen = 0u; /*FSFB输出队列数据长度*/
	UINT8 outQueData[FSFB_MAX_LEN * 16] = { 0 }; /*FSFB输出队列数据*/
	UINT32 outQueIdx = 0u; /*FSFB输出队列位置*/
	UINT16 outQueFramLen = 0u; /*FSFB输出队列帧长度*/
	UINT8 btmHead[5] = { 0 }; /*FSFB BTM帧头*/
	UINT8 queWriteRtn = 0u; /*队列写返回值*/
	UINT32 dataLen = 0u; /*FSFB输出处理后总数据长度*/
	UINT32 dataIdx = 0u; /*FSFB输出处理后总数据当前位置*/
	UINT16 frameLen = 0u; /*FSFB输出处理后帧长度*/
	UINT16 dstTypeId = 0u; /*目的类型ID*/
	UINT16 recordLen = 0u; /*记录长度*/

	if ((NULL != pUnifyInfo) && (NULL != pSndData) && (NULL != sendDataLen) && (NULL != pLnkStatusData) && (NULL != lnkDataLen) && (NULL != recordArray) && (NULL != recordDataLen)
		&& (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpFsfbUnitInfo) && (NULL != pUnifyInfo->GlobalVar->protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru)
		&& (NULL != pUnifyInfo->CycleNum))
	{
		curCycleNum = pUnifyInfo->CycleNum;
		appToStru = pUnifyInfo->GlobalVar->protclSndCfgStru[DSU_PROTCL_FSFB].pProtclAppStru;
		fsfbInfo = pUnifyInfo->GlobalVar->gpFsfbUnitInfo;

		QueueClear(&fsfbInfo->OutputDataQueue);
		LongToChar(0u, fsfbInfo->AppArray);

		if (0u == ((*curCycleNum) % 2))/*FSFB协议的周期为600ms*/
		{
			for (i = 0; i < devDataCount; i++) /*组各目的设备数据到队列*/
			{
				FsfbInputPackFms(appToStru[i], &fsfbInfo->OutputDataQueue);
			}

			if (0 == ((*curCycleNum) % 50)) /*CI300ms一周期,50周期,15s一次BTC*/
			{
				outQueDataLen = QueueStatus(&fsfbInfo->OutputDataQueue);
				if (outQueDataLen <= FSFB_MAX_LEN * 16) /*未超出最大长度*/
				{
					QueueScan(outQueDataLen, outQueData, &fsfbInfo->OutputDataQueue);
					while ((outQueIdx + 4) < outQueDataLen)
					{
						outQueFramLen = ShortFromChar(&outQueData[outQueIdx]); /*2字节长度*/
						outQueIdx += 2;
						ShortToChar((UINT16)3, &btmHead[0]); /*2字节BTM帧头长度*/
						memcpy(&btmHead[2], &outQueData[outQueIdx], 2); /*2字节目的LEU类型ID*/
						btmHead[4] = 0x10; /*BTM帧类型*/

						queWriteRtn = QueueWrite(5, btmHead, &fsfbInfo->OutputDataQueue);
						if (1u == queWriteRtn)
						{
							outQueIdx += outQueFramLen;
						}
						else
						{
							break;
						}
					}
				}
				else /*超出最大长度*/
				{
					QueueClear(&fsfbInfo->OutputDataQueue);
				}
			}

			retVal = FsfbProtclOutPut(fsfbInfo);

			if (1u == retVal)
			{
				dataLen = 4 + LongFromChar(&fsfbInfo->AppArray[dataIdx]);
				dataIdx += 4;
				while ((dataIdx + 4) < dataLen)
				{
					/*2字节长度*/
					frameLen = ShortFromChar(&fsfbInfo->AppArray[dataIdx]);
					dataIdx += 2;
					ShortToChar(frameLen + 3, &pSndData[*sendDataLen]);
					*sendDataLen += 2;

					/*1字节协议类型*/
					pSndData[*sendDataLen] = DSU_PROTCL_FSFB;
					*sendDataLen += 1;

					/*2字节目的类型ID*/
					dstTypeId = ShortFromChar(&fsfbInfo->AppArray[dataIdx]);
					dataIdx += 2;
					ShortToChar(dstTypeId, &pSndData[*sendDataLen]);
					*sendDataLen += 2;

					/*2字节目的逻辑ID*/
					ShortToChar(0u, &pSndData[*sendDataLen]);
					*sendDataLen += 2;

					if ((2 <= frameLen) && ((dataIdx + frameLen - 2) <= dataLen)) /*帧长度正常*/
					{
						frameLen -= 2;
						CommonMemCpy(&pSndData[*sendDataLen], frameLen, &fsfbInfo->AppArray[dataIdx], frameLen);

						dataIdx += frameLen;
						*sendDataLen += frameLen;
					}
					else
					{
						break;
					}
				}

				/*中间变量*/
				*lnkDataLen = 2 + ShortFromChar(fsfbInfo->VarArray);
				CommonMemCpy(pLnkStatusData, *lnkDataLen, fsfbInfo->VarArray, *lnkDataLen);
			}

			if (fsfbInfo->RecordArraySize > 3) /*有记录空间*/
			{
				recordLen = ShortFromChar(fsfbInfo->RecordArray); /*日志长度*/
				if ((2 + recordLen + 1) <= fsfbInfo->RecordArraySize) /*可存所有日志,1字节为结束字符*/
				{
					ShortToChar(recordLen + 1, &recordArray[0]);
					CommonMemCpy(&recordArray[2], recordLen, &fsfbInfo->RecordArray[2], recordLen);
					recordArray[2 + recordLen] = '\0';

					*recordDataLen = 2 + recordLen + 1;
				}
				else /*不可存所有日志*/
				{
					ShortToChar(fsfbInfo->RecordArraySize - 2, &recordArray[0]);
					CommonMemCpy(&recordArray[2], fsfbInfo->RecordArraySize - 3, &fsfbInfo->RecordArray[2], fsfbInfo->RecordArraySize - 3);
					recordArray[fsfbInfo->RecordArraySize - 1] = '\0';

					*recordDataLen = fsfbInfo->RecordArraySize;
				}
			}
		}
		else
		{
			retVal = 1u;
		}
	}

#endif

	return retVal;
}



/*
* 功能：RP和RSR协议发送输出数据组帧
*
* 参数：
* 输入参数：  UINT8* pInAppArray         输入应用数据
*             UINT32 appDtaLen           输入数据长度
* 输出参数：  UINT32* outAppLen          组包后的应用数据长度
*             UINT8* pOutAppArray        组包后的应用数据
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 RsrAndRpAppArrayPage(UINT8* pInAppArray, UINT32 appDtaLen, UINT8* pOutAppArray, UINT32* outAppLen)
{
	UINT8 retnVal = 0;
	UINT32 index = 0;
	UINT32 outIndex = 0;
	UINT16 dataLen = 0;

	if ((pInAppArray != NULL) && (pOutAppArray != NULL) && (outAppLen != NULL))
	{
		while (appDtaLen > 0)
		{
			/*两个子节点的长度长度等于源长度+2(目标设备个数和源设备个数)*/
			dataLen = ShortFromChar(&pInAppArray[index]);
			if (appDtaLen >= (dataLen + 2))
			{
				ShortToChar(dataLen + 2, &pOutAppArray[outIndex]);
				/*1个字节的目标设备个数*/
				index += 2;
				outIndex += 2;
				pOutAppArray[outIndex++] = 1;
				/*蓝网的IP地址和端口号  总共12个字节*/
				CommonMemCpy(&pOutAppArray[outIndex], 12, &pInAppArray[index], 12);

				index += 12;
				outIndex += 12;
				/*1个字节的源设备个数*/
				pOutAppArray[outIndex++] = 1;
				/*红网的IP地址和端口号  总共12个字节*/
				CommonMemCpy(&pOutAppArray[outIndex], 12, &pInAppArray[index], 12);
				index += 12;
				outIndex += 12;
				/*应用数据*/
				CommonMemCpy(&pOutAppArray[outIndex], dataLen - 24, &pInAppArray[index], dataLen - 24);
				appDtaLen = appDtaLen - dataLen - 2;
				outIndex += (dataLen - 24);
				index += (dataLen - 24);
			}
			else
			{
				break;
			}
		}
		*outAppLen = outIndex;

		retnVal = 1;
	}
	else
	{
		retnVal = 0;
	}
	return retnVal;
}

/***********协议接收相关函数**************************************************************/
/*
* 功能：SFP协议接收数据
*
* 参数：
* 输入参数：	struc_Unify_Info *gpsfpUnitInfo	SFP结构体
* 输出参数：	QueueStruct* pOutToApp 输出给应用的数据帧格式
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 SfpRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_SFP
	UINT8 tempDatabuff[2] = { 0 };
	UINT16 tempDataLen = 0;
	UINT8* sfpRcvDataBuff = NULL;
	UINT16 tmpLogid = 0;
	struc_Unify_Info* gpsfpUnitInfo = NULL;

	if ((NULL != pOutToApp) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpSfpRcvDataBuff) && (NULL != pUnifyInfo->GlobalVar->gpsfpUnitInfo))
	{
		sfpRcvDataBuff = pUnifyInfo->GlobalVar->gpSfpRcvDataBuff;
		gpsfpUnitInfo = pUnifyInfo->GlobalVar->gpsfpUnitInfo;

		CommonMemSet(sfpRcvDataBuff, sizeof(UINT8) * pUnifyInfo->GlobalVar->gSfpRcvDataBuffLen, 0u, sizeof(UINT8) * pUnifyInfo->GlobalVar->gSfpRcvDataBuffLen);
		while (QueueStatus(&gpsfpUnitInfo->DataToApp) > 2)
		{
			QueueRead(2, tempDatabuff, &gpsfpUnitInfo->DataToApp);/* 两个字节长度 */

			tempDataLen = ShortFromChar(tempDatabuff);
			QueueRead(tempDataLen, sfpRcvDataBuff, &gpsfpUnitInfo->DataToApp);
			tempDataLen = tempDataLen - 5 + PROTCL_OUT_FONT_LEN;
			/*ShortToChar(tempDataLen,tempDatabuff);*/
			RcvDataToAppQueue(sfpRcvDataBuff[3], sfpRcvDataBuff[2], tmpLogid, sfpRcvDataBuff[4], tempDataLen, sfpRcvDataBuff + 5, pOutToApp);
		}

	}
#endif

	ret = 1;

	return ret;
}




/*
* 功能：RSR协议接收数据
*
* 参数：
* 输入参数：
* 输出参数：  QueueStruct* pOutToApp 输出给应用的数据帧格式
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 RsrRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RSR
	UINT8 tempDatabuff[2] = { 0 };
	UINT16 tempDataLen = 0;
	UINT8* rsrRcvDataBuff = NULL;
	UINT16 tmpLogid = 0;
	UINT16 srcAddr = 0;
	UINT16 destAddr = 0;
	UINT16 destDevName = 0;
	UINT8 destDevType = 0;
	UINT8 destDevId = 0;
	RSR_INFO_STRU* gpRsrUnitInfo = NULL;

	if ((NULL != pOutToApp) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsrRcvDataBuff) && (NULL != pUnifyInfo->GlobalVar->gpRsrUnitInfo))
	{
		rsrRcvDataBuff = pUnifyInfo->GlobalVar->gpRsrRcvDataBuff;
		gpRsrUnitInfo = pUnifyInfo->GlobalVar->gpRsrUnitInfo;

		CommonMemSet(rsrRcvDataBuff, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRsrRcvDataBuffLen, 0u, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRsrRcvDataBuffLen);
		while (QueueStatus(&gpRsrUnitInfo->DataToApp) > 2)
		{
			QueueRead(2, tempDatabuff, &gpRsrUnitInfo->DataToApp);/* 两个字节长度 */

			tempDataLen = ShortFromChar(tempDatabuff);
			QueueRead(tempDataLen, rsrRcvDataBuff, &gpRsrUnitInfo->DataToApp);
			tempDataLen = tempDataLen - 4 + PROTCL_OUT_FONT_LEN;
			srcAddr = ShortFromChar(rsrRcvDataBuff);
			destAddr = ShortFromChar(rsrRcvDataBuff + 2);

			if (gpRsrUnitInfo->LocalType == VOBC_TYPE)
			{

				dsuVOBCvsCIInInfo(srcAddr, destAddr, &tmpLogid);
				destDevType = CI_TYPE;
			}
			else if (gpRsrUnitInfo->LocalType == ZC_TYPE)
			{
				dsuZCvsCIInInfo(srcAddr, destAddr, &destDevName);
				DevName2TypeId(destDevName, &destDevType, &destDevId);
				tmpLogid = 0;
			}
			else if (gpRsrUnitInfo->LocalType == CI_TYPE)
			{
				if (dsuVOBCvsCIInInfo(srcAddr, destAddr, &tmpLogid) == 0)
				{
					dsuZCvsCIInInfo(srcAddr, destAddr, &destDevName);
					DevName2TypeId(destDevName, &destDevType, &destDevId);
					tmpLogid = 0;
				}
				else
				{
					/*dsuVOBCvsCIInInfo(srcAddr,destAddr,&tmpLogid);*/
					destDevType = VOBC_TYPE;
				}
			}
			else
			{
				ret = 0;
				return ret;
			}
			RcvDataToAppQueue(destDevType, destDevId, tmpLogid, 0, tempDataLen, rsrRcvDataBuff + 4, pOutToApp);

		}

	}
#endif

	ret = 1;

	return ret;
}

/*将接收到的数据写入到输出队列中 */
UINT8 RcvDataToAppQueue(UINT8 devType, UINT8 devId, UINT16 logId, UINT8 lifeTime, UINT16 appDatalen, UINT8* pAppData, QueueStruct* pDataToApp)
{
	UINT8 ret = 0;
	UINT8 tempDataBuff[2] = { 0 };
	ShortToChar(appDatalen, tempDataBuff);
	QueueWrite(2, tempDataBuff, pDataToApp);
	QueueWrite(1, &devType, pDataToApp);
	QueueWrite(1, &devId, pDataToApp);
	ShortToChar(logId, tempDataBuff);
	QueueWrite(2, tempDataBuff, pDataToApp);
	QueueWrite(1, &lifeTime, pDataToApp);
	QueueWrite(appDatalen - PROTCL_OUT_FONT_LEN, pAppData, pDataToApp);
	ret = 1;
	return ret;
}


/*
* 功能：RSSP协议接收数据
*
* 参数：
* 输入参数：
* 输出参数：  QueueStruct* pOutToApp 输出给应用的数据帧格式
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 RsspRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RSSPI
	UINT8 tempDatabuff[2] = { 0 };
	UINT16 tempDataLen = 0;
	UINT8* rsspRcvDataBuff = NULL;
	UINT16 tmpLogid = 0;
	UINT16 logid = 0U;
	RSSP_INFO_STRU* gpRsspUnitInfo = NULL;

	if ((NULL != pOutToApp) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsspRcvDataBuff) && (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo))
	{
		rsspRcvDataBuff = pUnifyInfo->GlobalVar->gpRsspRcvDataBuff;
		gpRsspUnitInfo = pUnifyInfo->GlobalVar->gpRsspUnitInfo;

		CommonMemSet(rsspRcvDataBuff, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRsspRcvDataBuffLen, 0u, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRsspRcvDataBuffLen);
		while (QueueStatus(&gpRsspUnitInfo->DataToApp) > 2)
		{
			QueueRead(2, tempDatabuff, &gpRsspUnitInfo->DataToApp);
			tempDataLen = ShortFromChar(tempDatabuff);
			QueueRead(tempDataLen, rsspRcvDataBuff, &gpRsspUnitInfo->DataToApp);
			if (4 <= tempDataLen)
			{
				logid = rsspRcvDataBuff[2];
				tmpLogid = (UINT16)(((logid << 8) & 0xFF00) | (rsspRcvDataBuff[3] & 0x00FF));
				tempDataLen = tempDataLen - 4 + PROTCL_OUT_FONT_LEN;
				RcvDataToAppQueue(rsspRcvDataBuff[0], rsspRcvDataBuff[1], tmpLogid, 0, tempDataLen, rsspRcvDataBuff + 4, pOutToApp);
			}
		}

	}
#endif

	ret = 1;

	return ret;
}

/*
* 功能：RSSP2协议接收数据
*
* 参数：
* 输入参数：
* 输出参数：  QueueStruct* pOutToApp 输出给应用的数据帧格式
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 Rssp2RcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;
#ifdef USE_RSSPII
	UINT8 tempDatabuff[2] = { 0 };
	UINT16 tempDataLen = 0;
	UINT8* rssp2RcvDataBuff = NULL;
	UINT16 logId = 0;
	UINT8 devType = 0;
	UINT8 devId = 0;
	RSSP2_INFO_STRU* gpRssp2UnitInfo = NULL;

	if ((NULL != pOutToApp) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRssp2RcvDataBuff) && (NULL != pUnifyInfo->GlobalVar->gpRssp2UnitInfo))
	{
		rssp2RcvDataBuff = pUnifyInfo->GlobalVar->gpRssp2RcvDataBuff;
		gpRssp2UnitInfo = pUnifyInfo->GlobalVar->gpRssp2UnitInfo;

		CommonMemSet(rssp2RcvDataBuff, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRssp2RcvDataBuffLen, 0u, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRssp2RcvDataBuffLen);
		while (QueueStatus(&gpRssp2UnitInfo->DataToApp) > 2)
		{
			QueueRead(2, tempDatabuff, &gpRssp2UnitInfo->DataToApp);
			tempDataLen = ShortFromChar(tempDatabuff);/* 获取长度 */
			tempDataLen -= 4;
			QueueRead(1, &devType, &gpRssp2UnitInfo->DataToApp);
			QueueRead(1, &devId, &gpRssp2UnitInfo->DataToApp);
			QueueRead(2, tempDatabuff, &gpRssp2UnitInfo->DataToApp);
			logId = ShortFromChar(tempDatabuff);
			QueueRead(tempDataLen, rssp2RcvDataBuff, &gpRssp2UnitInfo->DataToApp);/* 获取组包后的数据 */

			tempDataLen = tempDataLen + PROTCL_OUT_FONT_LEN;/* 应用数据的长度,加上5个 */

			RcvDataToAppQueue(devType, devId, logId, 0, tempDataLen, rssp2RcvDataBuff, pOutToApp);
		}

	}
#endif

	ret = 1;

	return ret;
}


/*
* 功能：RP协议接收数据
*
* 参数：
* 输入参数：
* 输出参数：  QueueStruct* pOutToApp 输出给应用的数据帧格式
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 RpRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_RP
	UINT8 tempDatabuff[2] = { 0 };
	UINT16 tempDataLen = 0;
	UINT8* rpRcvDataBuff = NULL;
	UINT8 tempType = 0;
	UINT8 tempId = 0;
	UINT16 tmpLogid = 0;
	RP_INFO_STRU* gpRedunUnitInfo = NULL; /*冗余层协议结构体*/

	if ((NULL != pOutToApp) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRpRcvDataBuff) && (NULL != pUnifyInfo->GlobalVar->gpRedunUnitInfo))
	{
		rpRcvDataBuff = pUnifyInfo->GlobalVar->gpRpRcvDataBuff;
		gpRedunUnitInfo = pUnifyInfo->GlobalVar->gpRedunUnitInfo;

		CommonMemSet(rpRcvDataBuff, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRpRcvDataBuffLen, 0u, sizeof(UINT8) * pUnifyInfo->GlobalVar->gRpRcvDataBuffLen);
		while (QueueStatus(&gpRedunUnitInfo->DataToApp) > 2)
		{
			QueueRead(2, tempDatabuff, &gpRedunUnitInfo->DataToApp);
			QueueRead(1, &tempType, &gpRedunUnitInfo->DataToApp);
			QueueRead(1, &tempId, &gpRedunUnitInfo->DataToApp);

			tempDataLen = ShortFromChar(tempDatabuff);
			QueueRead(tempDataLen - 2, rpRcvDataBuff, &gpRedunUnitInfo->DataToApp);
			tempDataLen = tempDataLen - 2 + PROTCL_OUT_FONT_LEN;
			RcvDataToAppQueue(tempType, tempId, tmpLogid, 0, tempDataLen, rpRcvDataBuff, pOutToApp);
		}

	}
#endif

	ret = 1;

	return ret;
}

/*
* 功能：FSFB协议接收数据
*
* 参数：
* 输入参数：
* 输出参数：  QueueStruct* pOutToApp 输出给应用的数据帧格式
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 FsfbRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;

#ifdef USE_FSFB
	UINT8 dataLenBuf[2] = { 0 };
	UINT16 dataLen = 0u;
	UINT8* rcvBuf = NULL;
	FSFB_INFO_STRU* gpFsfbUnitInfo = NULL;

	if ((NULL != pOutToApp) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpFsfbRcvDataBuff) && (NULL != pUnifyInfo->GlobalVar->gpFsfbUnitInfo))
	{
		rcvBuf = pUnifyInfo->GlobalVar->gpFsfbRcvDataBuff;
		gpFsfbUnitInfo = pUnifyInfo->GlobalVar->gpFsfbUnitInfo;

		CommonMemSet(rcvBuf, sizeof(UINT8) * pUnifyInfo->GlobalVar->gFsfbRcvDataBuffLen, 0u, sizeof(UINT8) * pUnifyInfo->GlobalVar->gFsfbRcvDataBuffLen);
		while (QueueStatus(&gpFsfbUnitInfo->DataToApp) > 2)
		{
			QueueRead(2, dataLenBuf, &gpFsfbUnitInfo->DataToApp);
			dataLen = ShortFromChar(dataLenBuf);
			QueueRead(dataLen, rcvBuf, &gpFsfbUnitInfo->DataToApp);

			dataLen = dataLen - 2 + PROTCL_OUT_FONT_LEN;
			RcvDataToAppQueue(rcvBuf[0], rcvBuf[1], 0, 0, dataLen, &rcvBuf[2], pOutToApp);
		}

	}
#endif

	ret = 1;

	return ret;
}


/* 读取队列中的数据 */
UINT8 readQueueDataToApp(QueueStruct* rcvQueueData, UINT8 OutNetFlag, UINT16 locDevName, PROTCL_RCV_DATA_TYPE* pDataToApp, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 0;
	UINT8 retFun = 0;
	UINT8 i = 0;                               /*  循环使用 */
	UINT16 destDevName = 0;
	UINT8 protclType = 0;
	PROTCL_APP_TO_STRU* tempProtclAppStru = NULL;

#ifdef USE_RSSPII
	QueueRingID pRingQue = NULL;
	CM_BOOL mbRet = CM_FALSE;
	UINT32 len = 0;
#endif

	if ((NULL != rcvQueueData) && (NULL != pDataToApp) && (NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->pRcvProtclAppStru))
	{
		tempProtclAppStru = pUnifyInfo->GlobalVar->pRcvProtclAppStru;

		for (i = 0; i < PROTCL_MGECFG_MAXCNT + 1; i++)
		{
			/*清除长度信息,避免后续拷贝出错;*/
			pDataToApp[i].rcvDataLen = 0;
		}

		while (2 < QueueStatus(rcvQueueData))
		{
			retFun = 0;
			ret = AppReceiveDataAnalysis(rcvQueueData, tempProtclAppStru);
			if (ret == 1)
			{
				TypeIdToDevName(tempProtclAppStru->devType, tempProtclAppStru->devId, &destDevName);
				retFun = DsuProtclGetProtclType(locDevName, destDevName, &protclType, &pUnifyInfo->GlobalVar->DsuProtclBookIpInfo);
			}
			else
			{
				break;
			}

			if (1 == retFun)
			{
				if (DSU_PROTCL_RSSP2 == protclType)/* RSSP-II协议中没有目的设备类型与ID */
				{
#ifdef USE_RSSPII
					pRingQue = NULL;
					Prtcl_Manage_GetRQ(tempProtclAppStru->devType * 256 * 256 * 256 + tempProtclAppStru->devId * 256 * 256 + OutNetFlag * 256 + tempProtclAppStru->devLogId, &pRingQue);
					QUEUE_RING_Write(pRingQue, tempProtclAppStru->pAppData, tempProtclAppStru->appDataLen);
					mbRet = CM_TRUE;
					while (CM_TRUE == mbRet)
					{
						mbRet = Prtcl_Base_Check(pRingQue, &pUnifyInfo->GlobalVar->Rssp2PrtclHead, tempProtclAppStru->pAppData, pUnifyInfo->GlobalVar->gRssp2RcvDataBuffLen, &len);
						if (CM_TRUE == mbRet)
						{
							tempProtclAppStru->appDataLen = (UINT16)len;/* 通信数据长度 */
							/*将新一帧数据往缓冲区尾部填写*/
							ShortToChar(tempProtclAppStru->appDataLen + 4, pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen);/*适配层帧长度=通信数据长度+4字节帧头*/
							/*修改缓冲区总长度*/
							pDataToApp[protclType].rcvDataLen += 2;

							/* 增加发送方的设备类型 */
							*(pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen) = tempProtclAppStru->devType;
							pDataToApp[protclType].rcvDataLen++;

							/* 增加发送方的设备ID */
							*(pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen) = tempProtclAppStru->devId;
							pDataToApp[protclType].rcvDataLen++;

							/* 增加逻辑ID */
							ShortToChar(tempProtclAppStru->devLogId, pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen);
							pDataToApp[protclType].rcvDataLen += 2;

							/*拷贝接收数据至缓冲区尾部*/
							CommonMemCpy(pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen, tempProtclAppStru->appDataLen, tempProtclAppStru->pAppData, tempProtclAppStru->appDataLen);
							/*更新缓冲区总长度*/
							pDataToApp[protclType].rcvDataLen += tempProtclAppStru->appDataLen;
						}
					}
#endif
				}
				else
				{
					if (DSU_PROTCL_RSSP == protclType)	/*RSSPI协议需添加2字节逻辑ID*/
					{
						/*2字节总长度*/
						ShortToChar(tempProtclAppStru->appDataLen + 2, pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen);
						pDataToApp[protclType].rcvDataLen += 2;
						/*2字节逻辑ID*/
						ShortToChar(tempProtclAppStru->devLogId, pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen);
						pDataToApp[protclType].rcvDataLen += 2;
					}
					else if (DSU_PROTCL_FSFB == protclType) /*FSFB协议*/
					{
						/*2字节总长度*/
						ShortToChar(tempProtclAppStru->appDataLen + 4, &pDataToApp[protclType].pData[pDataToApp[protclType].rcvDataLen]);
						pDataToApp[protclType].rcvDataLen += 2;

						/*1字节类型*/
						pDataToApp[protclType].pData[pDataToApp[protclType].rcvDataLen] = tempProtclAppStru->devType;
						pDataToApp[protclType].rcvDataLen++;

						/*1字节ID*/
						pDataToApp[protclType].pData[pDataToApp[protclType].rcvDataLen] = tempProtclAppStru->devId;
						pDataToApp[protclType].rcvDataLen++;

						/*2字节逻辑ID*/
						ShortToChar(tempProtclAppStru->devLogId, &pDataToApp[protclType].pData[pDataToApp[protclType].rcvDataLen]);
						pDataToApp[protclType].rcvDataLen += 2;
					}
					else /*其他协议*/
					{
						/*协议类型不再填写,使用全局规定的各协议类型*/
						/*pDataToApp[protclType].proType = protclType;*/
						/*将新一帧数据往缓冲区尾部填写*/
						ShortToChar(tempProtclAppStru->appDataLen, pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen);
						/*修改缓冲区总长度*/
						pDataToApp[protclType].rcvDataLen += 2;
					}
					/*拷贝接收数据至缓冲区尾部*/
					CommonMemCpy(pDataToApp[protclType].pData + pDataToApp[protclType].rcvDataLen, tempProtclAppStru->appDataLen, tempProtclAppStru->pAppData, tempProtclAppStru->appDataLen);
					/*更新缓冲区总长度*/
					pDataToApp[protclType].rcvDataLen += tempProtclAppStru->appDataLen;
				}
			}
			else
			{
				break;
			}
		}

		ret = 1;
	}

	return ret;
}


/*
* 功能：接收应用层的数据解析
*
* 参数：
* 输入参数：  const QueueStruct *pAppQueueData    应用输入到协议的数据队列
* 输出参数：  PROTCL_APP_TO_STRU *protclAppStru   解析后的数据
* 返回值：
*    UINT8 1 收到完整数据帧，0 无数据
*/
UINT8 AppReceiveDataAnalysis(QueueStruct* pAppQueueData, PROTCL_APP_TO_STRU* pProtclAppStru)
{
	UINT8 retVal = 0;
	UINT8 tempBuff[2] = { 0 };
	UINT16 tempDatalen = 0;

	UINT8 protclType = GetCbtcSysType();

	QueueScan(2, tempBuff, pAppQueueData);	/* 肯定存在两个字节 */
	tempDatalen = ShortFromChar(tempBuff);

	if ((tempDatalen + 2) <= QueueStatus(pAppQueueData))
	{

		QueueRead(2, tempBuff, pAppQueueData);/* 长度 */
		QueueRead(1, &pProtclAppStru->devType, pAppQueueData);/* 类型 */
		QueueRead(1, &pProtclAppStru->devId, pAppQueueData);/* ID */

		if ((DQU_CBTC_CPK == protclType) || (DQU_CBTC_FAO == protclType))	/*合库或FAO*/
		{
			pProtclAppStru->appDataLen = tempDatalen - 2;
			QueueRead((tempDatalen - 2), pProtclAppStru->pAppData, pAppQueueData);/* 应用数据 */
			retVal = 1;
		}
		else /*其他*/
		{
			if (4 <= tempDatalen)
			{
				pProtclAppStru->appDataLen = tempDatalen - 4;
				QueueRead(2, tempBuff, pAppQueueData);
				pProtclAppStru->devLogId = ShortFromChar(tempBuff);
				QueueRead((tempDatalen - 4), pProtclAppStru->pAppData, pAppQueueData);/* 应用数据 */

				retVal = 1;
			}
			else
			{
				retVal = 0;
			}
		}
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}


/*
*	将Buff的格式的数据压入队列
*/
UINT8 SetDataToQue(UINT32 dataLen, UINT8* buff, QueueStruct* q)
{
	UINT8 ret = 0;
	QueueClear(q);
	ret = QueueWrite(dataLen, buff, q);
	return ret;
}



/************************************链路状态查询******************************************/
/*
* 功能：SFP协议查询链路状态
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
*			  struc_Unify_Info *pSfpUnitInfo SFP统一结构体
* 返回值：
*     0：不存在该条链路，返回错误
*     0x17：链路在数据传输正常状态
*     0x2b：链路在通信中断状态
*     0x33：链路在准备建立链接状态
*     0x4e：链路在等待建立链接状态
*/
UINT8 GetSfpLnkStatus(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_SFP
	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpsfpUnitInfo))
	{
		retVal = ProtclCheckLnkStatus(destType, destId, pUnifyInfo->GlobalVar->gpsfpUnitInfo);
	}

#endif

	return retVal;
}

/*
* 功能：RSR协议查询链路状态
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     0：不存在该条链路，返回错误
*     0x17：链路在数据传输正常状态
*     0x2b：链路在通信中断状态
*/
UINT8 GetRsrLnkStatus(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSR
	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsrUnitInfo))
	{

		UINT16 locDevName = 0;
		UINT16 DestDevName = 0;
		UINT16 rsspSrcAdd = 0;
		INT16   bsdAdd = 0;
		UINT16	destRsspAdd = 0;
		RSR_INFO_STRU* gpRsrUnitInfo = pUnifyInfo->GlobalVar->gpRsrUnitInfo;

		TypeIdToDevName(gpRsrUnitInfo->LocalType, gpRsrUnitInfo->LocalID, &locDevName);
		TypeIdToDevName(destType, destId, &DestDevName);

		if (destType == VOBC_TYPE)
		{
			retVal = dsuVOBCvsCIOutInfo(logId, 0, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
		}
		else if (gpRsrUnitInfo->LocalType == VOBC_TYPE)
		{
			retVal = dsuVOBCvsCIOutInfo(logId, 1, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
		}
		else
		{
			retVal = dsuZCvsCIOutInfo(locDevName, DestDevName, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
		}
		if (1 == retVal)
		{
			retVal = RsrLnkStatus(rsspSrcAdd, bsdAdd, destRsspAdd, gpRsrUnitInfo);
		}
	}
#endif

	return retVal;
}

/*
* 功能：RSSP协议查询链路状态
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     0：不存在该条链路，返回错误
*     0x17：链路在数据传输正常状态
*     0x2b：链路在通信中断状态
*/
UINT8 GetRsspLnkStatus(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPI
	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo))
	{
		retVal = Rssp2LnkStatus(destType, destId, logId, pUnifyInfo->GlobalVar->gpRsspUnitInfo);
	}

#endif

	return retVal;
}

/*
* 功能：RSSP2协议查询链路状态
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     0：不存在该条链路，返回错误
*     0x17：链路在数据传输正常状态
*     0x2b：链路在通信中断状态
*/
UINT8 GetRssp2LnkStatus(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPII
	retVal = RsspIILnkStatus(destType, destId, logId, pUnifyInfo->GlobalVar->gpRssp2UnitInfo);
#endif

	return retVal;
}

/*
* 功能：FSFB协议查询链路状态
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     0：不存在该条链路，返回错误
*     0x17：链路在数据传输正常状态
*     0x2b：链路在通信中断状态
*/
UINT8 GetFsfbLnkStatus(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_FSFB

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpFsfbUnitInfo))
	{
		retVal = FsfbProtclGetLnkStatus(destType, destId, pUnifyInfo->GlobalVar->gpFsfbUnitInfo);
	}

#endif

	return retVal;
}

/*******************************************删除链路子函数开始 **********************************************/

/*
* 功能：SFP协议删除链路
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
*			  struc_Unify_Info *pSfpUnitInfo  SFP统一结构体
* 返回值：
*     1：删除链路成功
*     0：删除链路失败
*/
UINT8 SfpDelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_SFP
	retVal = ProtclDeleteLnk(destType, destId, pUnifyInfo->GlobalVar->gpsfpUnitInfo);
#endif

	return retVal;
}

/*
* 功能：Rp协议删除链路
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     1：删除链路成功
*     0：删除链路失败
*/
UINT8 RpDelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RP
	UINT16 destDevName = 0;
	TypeIdToDevName(destType, destId, &destDevName);
	retVal = RpLnkDelete(destDevName, pUnifyInfo->GlobalVar->gpRedunUnitInfo);
#endif

	return retVal;
}

/*
* 功能：RSR协议删除链路
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     1：删除链路成功
*     0：删除链路失败
*/
UINT8 RsrDelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSR
	UINT16 locDevName = 0;
	UINT16 DestDevName = 0;
	UINT16 rsspSrcAdd = 0;
	INT16   bsdAdd = 0;
	UINT16	destRsspAdd = 0;
	RSR_INFO_STRU* gpRsrUnitInfo = pUnifyInfo->GlobalVar->gpRsrUnitInfo;

	TypeIdToDevName(gpRsrUnitInfo->LocalType, gpRsrUnitInfo->LocalID, &locDevName);
	TypeIdToDevName(destType, destId, &DestDevName);

	if (destType == VOBC_TYPE)
	{
		retVal = dsuVOBCvsCIOutInfo(logId, 0, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
	}
	else if (gpRsrUnitInfo->LocalType == VOBC_TYPE)
	{
		retVal = dsuVOBCvsCIOutInfo(logId, 1, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
	}
	else
	{
		retVal = dsuZCvsCIOutInfo(locDevName, DestDevName, &rsspSrcAdd, &bsdAdd, &destRsspAdd);
	}
	if (1 == retVal)
	{
		retVal = RsrLnkDelete(rsspSrcAdd, bsdAdd, destRsspAdd, gpRsrUnitInfo);
	}
#endif

	return retVal;
}

/*
* 功能：RSSP协议删除链路
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     1：删除链路成功
*     0：删除链路失败
*/
UINT8 RsspDelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPI
	retVal = Rssp2LnkDelete(destType, destId, logId, pUnifyInfo->GlobalVar->gpRsspUnitInfo);
#endif

	return retVal;
}

/*
* 功能：RSSP2协议删除链路
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     1：删除链路成功
*     0：删除链路失败
*/
UINT8 Rssp2DelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPII
	retVal = RsspIILnkDelete(destType, destId, logId, pUnifyInfo->GlobalVar->gpRssp2UnitInfo);
#endif

	return retVal;
}

/*
* 功能：FSFB协议删除链路
*
* 参数：
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
* 返回值：
*     1：删除链路成功
*     0：删除链路失败
*/
UINT8 FsfbDelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_FSFB

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpFsfbUnitInfo))
	{
		retVal = FsfbProtclDeleteLnk(destType, destId, pUnifyInfo->GlobalVar->gpFsfbUnitInfo);
	}

#endif

	return retVal;
}

/********************************************刷新链路相关函数*********************************************/
/*
* 功能：SFP刷新链路状态
*
* 参数：
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
*				 struc_Unify_Info *pSfpUnitInfo SFP统一结构体
* 返回值：
*     0：刷新链路失败
*     1：刷新链路成功
*/
UINT8 SfpReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_SFP
	if (NULL != pUnifyInfo->GlobalVar->gpsfpUnitInfo)
	{
		retVal = ProtclFreshLnkMngr(slen_input, pLnkStatusData, pUnifyInfo->GlobalVar->gpsfpUnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}

/*
* 功能：RP刷新链路状态
*
* 参数：
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值：
*     0：刷新链路失败
*     1：刷新链路成功
*/
UINT8 RpReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RP
	if (NULL != pUnifyInfo->GlobalVar->gpRedunUnitInfo)
	{
		retVal = RpRefresh(slen_input, pLnkStatusData, pUnifyInfo->GlobalVar->gpRedunUnitInfo);
	}
	else
	{
		retVal = 1;

	}
#endif

	return retVal;
}

/*
* 功能：RSR刷新链路状态
*
* 参数：
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值：
*     0：刷新链路失败
*     1：刷新链路成功
*/
UINT8 RsrReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSR
	if (NULL != pUnifyInfo->GlobalVar->gpRsrUnitInfo)
	{
		retVal = RsrRefresh(slen_input, pLnkStatusData, pUnifyInfo->GlobalVar->gpRsrUnitInfo);
	}
	else
	{
		retVal = 1;

	}
#endif

	return retVal;
}

/*
* 功能：RSSP刷新链路状态
*
* 参数：
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值：
*     0：刷新链路失败
*     1：刷新链路成功
*/
UINT8 RsspReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPI
	if (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo)
	{
		retVal = Rssp2Refresh(slen_input, pLnkStatusData, pUnifyInfo->GlobalVar->gpRsspUnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}

/*
* 功能：RSSP2刷新链路状态
*
* 参数：
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值：
*     0：刷新链路失败
*     1：刷新链路成功
*/
UINT8 Rssp2ReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPII
	if (NULL != pUnifyInfo->GlobalVar->gpRssp2UnitInfo)
	{
		retVal = RsspIIRefresh(slen_input, pLnkStatusData, pUnifyInfo->GlobalVar->gpRssp2UnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}


/*
* 功能：FSFB刷新链路状态
*
* 参数：
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值：
*     0：刷新链路失败
*     1：刷新链路成功
*/
UINT8 FsfbReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_FSFB

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpFsfbUnitInfo))
	{
		retVal = FsfbProtclRefreshLnk(pLnkStatusData, pUnifyInfo->GlobalVar->gpFsfbUnitInfo);
	}

#endif

	return retVal;
}

/********************************************销毁空间相关函数*********************************************/
/*
* SFP协议销毁协议模块
* 参数：struc_Unify_Info *pSfpUnitInfo SFP统一结构体
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 SfpFreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_SFP
	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpsfpUnitInfo))
	{
		retVal = ProtclFreeSpace(pUnifyInfo->GlobalVar->gpsfpUnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}

/*
* RP协议销毁协议模块
* 参数：
*    无
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 RpFreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RP
	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRedunUnitInfo))
	{
		retVal = RpFreeSpace(pUnifyInfo->GlobalVar->gpRedunUnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}

/*
* RSR协议销毁协议模块
* 参数：
*    无
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 RsrFreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSR
	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsrUnitInfo))
	{
		retVal = RsrFreeSpace(pUnifyInfo->GlobalVar->gpRsrUnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}


/*
* RSSP协议销毁协议模块
* 参数：
*    无
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 RsspFreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPI

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo))
	{
		retVal = Rssp2FreeSpace(pUnifyInfo->GlobalVar->gpRsspUnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}
/*
* RSSP2协议销毁协议模块
* 参数：
*    无
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 Rssp2FreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_RSSPII

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRssp2UnitInfo))
	{
		retVal = RsspIIFreeSpace(pUnifyInfo->GlobalVar->gpRssp2UnitInfo);
	}
	else
	{
		retVal = 1;
	}
#endif

	return retVal;
}

/*
* FSFB协议销毁协议模块
* 参数：
*    无
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 FsfbFreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 0;

#ifdef USE_FSFB

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpFsfbUnitInfo))
	{
		retVal = FsfbProtclFreeSpace(pUnifyInfo->GlobalVar->gpFsfbUnitInfo);
	}

#endif

	return retVal;
}


/*
* 销毁全局变量的空间
* 参数：
*    ProtclConfigInfoStru* pUnifyInfo 适配层结构体
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 FreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 retVal = 1;
	UINT8 i = 0;/* 循环使用 */

	if ((NULL!= pUnifyInfo)&&(NULL != pUnifyInfo->GlobalVar)&&(NULL != pUnifyInfo->GlobalVar->RcvData))
	{
		for (i = 1; i < (PROTCL_MGECFG_MAXCNT + 1); i++)
		{
			if ((NULL != pUnifyInfo->GlobalVar->RcvData[i].pData))
			{
				free((void*)pUnifyInfo->GlobalVar->RcvData[i].pData);
				pUnifyInfo->GlobalVar->RcvData[i].pData = NULL;
			}
		}
		free((void*)pUnifyInfo->GlobalVar->RcvData);
		pUnifyInfo->GlobalVar->RcvData = NULL;


		for (i = 0; i < (PROTCL_MGECFG_MAXCNT + 1); i++)
		{
			if (NULL != pUnifyInfo->GlobalVar->protclSndCfgStru[i].pProtclAppStru)
			{
				free((void*)pUnifyInfo->GlobalVar->protclSndCfgStru[i].pProtclAppStru);
				pUnifyInfo->GlobalVar->protclSndCfgStru[i].pProtclAppStru = NULL;
			}
		}
		free(pUnifyInfo->GlobalVar->RcvData);
		pUnifyInfo->GlobalVar->RcvData = NULL;
		free(pUnifyInfo->GlobalVar);
	}


	return retVal;
}



/***********Getting**&**Setting****封装******************************************************/

/***********异常防护函数**************************************************************/
/** 零函数
* 主要用于函数指针清零。
* 防止异常情况使用空的函数指针而导致的不良后果。
**************************************************/
UINT8  protcl_nul_Init(UINT8* nul_FileName, UINT32 inDataLen, UINT8 inData[], UINT8 nul_Num, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8  retval = 1;

	return retval;
}

UINT8  protcl_nul_Snd(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen)
{
	UINT8  retval = 1;

	return retval;
}

UINT8  protcl_nul_Rcv(QueueStruct* PQueueStru, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8  retval = 1;

	return retval;
}


UINT8  protcl_nul_Lnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8  retval = 1;

	return retval;
}

void protcl_nul_NetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
	if ((NULL != pUnifyInfo) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum))
	{
		(*redRecvPkgNum) = 0u;
		(*blueRecvPkgNum) = 0u;
	}
}

UINT8  protcl_nul_DelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8  retval = 1;

	return retval;
}


UINT8  protcl_nul_ReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8  retval = 1;

	return retval;
}


UINT8  protcl_nul_End(ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8  retval = 1;

	return retval;
}

/*******************************************************************************************************
* 功能描述: 	   返回所有协议及协议适配层丢包或非宕机错误码信息
* 输入参数: 		应用获取错误编码的数组vProtclErrorInfo
* 输入输出参数: 	vProtclErrorInfo
* 输出参数: 		返回vProtclErrorInfo错误信息码
* 全局变量: 	  无
* 返回值:			无
*******************************************************************************************************/

UINT8 GetProtclErrorInfo(UINT32 vProtclErrorInfo[])
{
	UINT8 index = 0, retval = 0;

#ifdef USE_RP
	vProtclErrorInfo[index++] = GetRpErrorInfo();
#endif
#ifdef USE_RSR
	vProtclErrorInfo[index++] = GetRsrErrorInfo();
#endif

	retval = 1;
	return retval;
}


/*******************************************************************************************************
* 功能描述: 	   所有协议及协议适配层丢包和非宕机错误码复位
* 输入参数: 		应用获取错误编码的数组vProtclErrorInfo
* 输入输出参数: 	vProtclErrorInfo
* 输出参数: 		vProtclErrorInfo
* 全局变量: 	  无
* 返回值:			无
*******************************************************************************************************/

UINT8 ReSetProtclErrorInfo(UINT32 vProtclErrorInfo[])
{
	UINT8 ret = 1;
#ifdef USE_RP
	ReSetRpErrorInfo();
#endif
#ifdef USE_RSR
	ReSetRsrErrorInfo();
#endif
	CommonMemSet(vProtclErrorInfo, sizeof(UINT32) * PRTCL_ERR_INFO_LEN, 0, sizeof(UINT32) * PRTCL_ERR_INFO_LEN);
	return ret;
}
/*******************************************************************************************************
* 功能描述: 	   设置RSSP协议无发送应用数据周期数接口函数
* 输入参数: 		无
* 输入输出参数: 	无
* 输出参数: 		gRsspErrorInfo
* 全局变量: 	  gRsspErrorInfo
* 返回值:			1:成功
*******************************************************************************************************/

UINT8 SetRsspConfigData(UINT8 vNoSendDataNum, ProtclConfigInfoStru* pUnifyInfo)
{
	UINT8 ret = 1;

#ifdef USE_RSSPI
	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo))
	{
		SetRsspNoSendAppData(vNoSendDataNum, pUnifyInfo->GlobalVar->gpRsspUnitInfo);
	}

#endif

	return ret;
}

/*
*函数名:	TypeIdToDevName
*功能描述:	类型ID转成设备名称
*输入参数:	UINT8 type	类型
*			UINT8 id	ID
*输入出参:	无
*输出参数:	UINT16 *pDevName	设备名称
*返回值:	无
*/
static void TypeIdToDevName(UINT8 type, UINT8 id, UINT16* pDevName)
{
	UINT16 devName = 0U;
	devName = type;
	*pDevName = ((UINT16)(devName << 8) & 0xFF00) | (id & 0x00FF);
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
void GetNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
	UINT16 locTypeId = 0u; /*本方类型ID*/
	UINT16 oppTypeId = 0u; /*对方类型ID*/
	UINT8 protclType = 0u; /*协议类型*/
	UINT8 isGetProtclType = 0u; /*是否获取到协议类型*/
	F_NETPKGNUM fNetsPkgNum = NULL; /*各网收包数量函数*/

	if ((NULL != pUnifyInfo) && (NULL != pUnifyInfo->GlobalVar) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum)) /*参数检查*/
	{
		(*redRecvPkgNum) = 0u;
		(*blueRecvPkgNum) = 0u;
		TypeIdToDevName(pUnifyInfo->LocalType, pUnifyInfo->LocalID, &locTypeId);
		TypeIdToDevName(dstType, dstId, &oppTypeId);
		isGetProtclType = DsuProtclGetProtclType(locTypeId, oppTypeId, &protclType, &pUnifyInfo->GlobalVar->DsuProtclBookIpInfo);

		if ((1u == isGetProtclType) && (PROTCL_MGECFG_MAXCNT >= protclType) && (0u != protclType)) /*获取到协议类型*/
		{
			fNetsPkgNum = protclCfgStruBuff[protclType].fNetsRecvPkgNum;
			if (NULL != fNetsPkgNum)
			{
				fNetsPkgNum(pUnifyInfo, dstType, dstId, dstLogicId, redRecvPkgNum, blueRecvPkgNum);
			}
		}
	}
}

/*
*功能描述:	获取SFP各网收到包数
*输入参数:	ProtclConfigInfoStru* pUnifyInfo 适配层变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
static void GetSfpNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
#ifdef USE_SFP
	if ((NULL != pUnifyInfo) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpsfpUnitInfo)) /*参数检查*/
	{		
		ProtclGetNetsRecvPkgNum(pUnifyInfo->GlobalVar->gpsfpUnitInfo, dstType, dstId, redRecvPkgNum, blueRecvPkgNum);
	}
#endif
}

/*
*功能描述:	获取RSSPI各网收到包数
*输入参数:	ProtclConfigInfoStru* pUnifyInfo 适配层变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
static void GetRsspiNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
#ifdef USE_RSSPI
	if ((NULL != pUnifyInfo) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRsspUnitInfo)) /*参数检查*/
	{
		RsspiGetNetsRecvPkgNum(pUnifyInfo->GlobalVar->gpRsspUnitInfo, dstType, dstId, dstLogicId, redRecvPkgNum, blueRecvPkgNum);
	}
#endif
}

/*
*功能描述:	获取RSSPII各网收到包数
*输入参数:	ProtclConfigInfoStru* pUnifyInfo 适配层变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
static void GetRsspiiNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
#ifdef USE_RSSPII
	if ((NULL != pUnifyInfo) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpRssp2UnitInfo)) /*参数检查*/
	{
		RsspiiGetNetsRecvPkgNum(pUnifyInfo->GlobalVar->gpRssp2UnitInfo, dstType, dstId, dstLogicId, redRecvPkgNum, blueRecvPkgNum);
	}
#endif
}

/*
*功能描述:	获取FSFB各网收到包数
*输入参数:	ProtclConfigInfoStru* pUnifyInfo 适配层变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
static void GetFsfbNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
#ifdef USE_FSFB
	if ((NULL != pUnifyInfo) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum) && (NULL != pUnifyInfo->GlobalVar) && (NULL != pUnifyInfo->GlobalVar->gpFsfbUnitInfo)) /*参数检查*/
	{
		FsfbProtclGetNetsRecvPkgNum(pUnifyInfo->GlobalVar->gpFsfbUnitInfo, dstType, dstId, redRecvPkgNum, blueRecvPkgNum);
	}
#endif
}

/*
*功能描述:	初始化单帧应用到协议的数据结构体变量--只能初始化时使用
*输入参数:	const UINT16 outputSize 输出应用数长度
*			const UINT16 inputSize 输入应用数长度
*输入出参:	无
*输出参数:	无
*返回值:	PROTCL_APP_TO_STRU* protclAppToStru 应用到协议的数据结构体
*/
static PROTCL_APP_TO_STRU* NewProtclAppToStru(const UINT16 outputSize, const UINT16 inputSize)
{
	PROTCL_APP_TO_STRU* protclAppToStru = NULL; /*返回值*/

	protclAppToStru = (PROTCL_APP_TO_STRU*)malloc(sizeof(PROTCL_APP_TO_STRU));
	if (NULL != protclAppToStru)
	{
		CommonMemSet(protclAppToStru, sizeof(PROTCL_APP_TO_STRU), 0u, sizeof(PROTCL_APP_TO_STRU));

		protclAppToStru->pAppData = NewAppData(&protclAppToStru->appDataMaxLen, outputSize, inputSize);
		if (NULL == protclAppToStru->pAppData)
		{
			protclAppToStru = NULL;
		}
	}

	return protclAppToStru;
}


/*
*功能描述:	生成单帧应用到协议的数据数组--只能初始化时使用
*输入参数:	const UINT16 outputSize 输出应用数长度
*			constUINT16 inputSize 输入应用数长度
*输入出参:	UINT16* appDataMaxLen 应用到协议的数据最大长度
*输出参数:	无
*返回值:	UINT8* appData 应用到协议的数据数组
*/
static UINT8* NewAppData(UINT16* appDataMaxLen, const UINT16 outputSize, const UINT16 inputSize)
{
	UINT8* appData = NULL; /*返回值*/
	UINT32 mallocLen = 0u; /*分配空间长度*/

	if (NULL != appDataMaxLen)
	{
		*appDataMaxLen = (outputSize > inputSize) ? outputSize : inputSize;
		*appDataMaxLen += 44u; /*做输入时再加各协议里最长包头*/
		mallocLen = sizeof(UINT8) * (*appDataMaxLen);
		appData = (UINT8*)malloc(mallocLen);
		if (NULL != appData)
		{
			CommonMemSet(appData, mallocLen, 0u, mallocLen);
		}
		else
		{
			appData = NULL;
		}
	}

	return appData;
}
