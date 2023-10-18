/** ******************************************************
*                                                                                                            
* 文件名  ： RsspWl.c   
* 版权说明： 北京交控科技有限公司
* 版本号  ： 1.0
* 创建时间： 2013.2.5
* 作者    ： 车载及协议部
* 功能描述： Rssp层协议实现  
* 使用注意： 
*   
* 修改记录：   
*   更新时间	更新者	更新理由
* ------------  ------  ---------------
*  2009.12.07	楼宇伟	初版作成
* 2013.02.07  王佩佩  增加函数名称以及相对应的RSSP结构体
********************************************************/ 
#include <stdlib.h>
#include "CommonMemory.h"
#include "RsspIFWl.h"
#include "RsspStructWl.h"
#include "RsspCommonWl.h"
#include "Convert.h"
#include "RsspRecordDelareWl.h"
#include "CommonRecord.h"
#include "RsspLnkNodeMgrWl.h"
#include "dsuRsspFunc.h"

#ifdef __cplusplus
extern "C" {
#endif


/*--------------------------------------------------------------------
 * RSSP层内部使用函数申明 Start
 *--------------------------------------------------------------------*/

static UINT8 CheckRsspStruct_WL(const RSSP_INFO_STRU *pRsspStruct);	

/*--------------------------------------------------------------------
 * RSSP层内部使用函数申明 End
 *--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
 * RSSP层对外接口函数申明 Start
 *--------------------------------------------------------------------*/
/***********************************************************************
 * 方法名   : Rssp2Receive
 * 功能描述 : 对红蓝网输入数据进行RSSP层的安全处理，将安全数据交付应用。
 * 输入参数 : 
 *	参数名		类型		输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体
 *											输入队列：pRsspStruct->OutnetQueueA				
 *													 pRsspStruct->OutnetQueueB
 *											输出队列：pRsspStruct->DataToApp
 * 
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/

 /*
 *函数名:	Rssp2Receive
 *功能描述:	RSSPI输入处理
 *输入参数:	无
 *输入出参:	RSSP_INFO_STRU *pRsspStruct RSSPI信息
 *输出参数:	无
 *返回值:	1:成功;0:失败
 */
UINT8 Rssp2Receive(RSSP_INFO_STRU* pRsspStruct)
{
    UINT8 rtn = 0; /*返回值*/

    if ((NULL != pRsspStruct) && (NULL != pRsspStruct->pDsuRsspInfoStruWL) && (NULL != pRsspStruct->RecordArray))	/*指针非空检查*/
    {
        ShortToChar(0, pRsspStruct->RecordArray);	/*记录清空*/
        PrintRecArrayByte(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, RSSP_CYCLE_START);	/*RSSPI日志开始标识*/
        PrintRecArray4ByteB(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, *pRsspStruct->CycleNum);	/*周期号*/
        PrintRecArrayByte(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, pRsspStruct->LocalType);	/*本方类型*/
        PrintRecArrayByte(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, pRsspStruct->LocalID);	/*本方ID*/
        PrintRecArray2ByteB(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, pRsspStruct->LocalLogicId);	/*本方逻辑ID*/
        PrintRecArray2ByteB(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, pRsspStruct->pDsuRsspInfoStruWL->LocRsspiAddr);	/*本方RSSPI地址*/

        rtn = RsspReceive_WL(pRsspStruct);
    }

    return rtn;
}

/***********************************************************************
 * 方法名   : Rssp2Output
 * 功能描述 : 对应用输出数据进行RSSP层的安全处理并输出。
 * 输入参数 : 
 *	参数名		类型			输入输出	描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体
 *											输入队列：pRsspStruct->OutputDataQueue
 *											输出队列：pRsspStruct->AppArray
 *													 pRsspStruct->VarArray
 * 
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 Rssp2Output(RSSP_INFO_STRU *pRsspStruct)
{
	UINT8 ret = 0;
	UINT16 LocalName = 0;		/* 本方标识符 */

	if(pRsspStruct == NULL)
	{
		return ret;
	}
	else
	{
		/* 什么不做 */
	}
	TypeId2DevName_WL(pRsspStruct->LocalType, pRsspStruct->LocalID, &LocalName);

	/* RSSP层输出处理 */
	ret = RsspOutput_WL(&pRsspStruct->OutputDataQueue, pRsspStruct->VarArray, pRsspStruct);
	if(0 == ret)
	{
		/* 记录 */
		return ret;
	}
	else
	{
		/* 什么不做 */
	}
   /*wangpeipei 20120223 Mod S */

	ret = 1;
	return ret;
}

/*
*函数名:	Rssp2Init
*功能描述:	RSSPI协议初始化
*输入参数:	UINT8 *pFsName	FS文件
*输入出参:	RSSP_INFO_STRU *pRsspStruct	RSSPI信息
*输出参数:	无
*返回值:	1:成功;0失败
*/
UINT8 Rssp2Init(UINT8* pFsName, UINT32 inParamLen, UINT8 inParam[], RSSP_INFO_STRU* pRsspStruct)
{
	UINT8 rtn = 0;	/*返回值*/
	UINT16 locTypeId = 0;	/*本方类型ID*/
    UINT16 localType = 0U;
	if ((NULL != pFsName) && (NULL != pRsspStruct))	/*传入参数非空*/
	{
		rtn = CheckRsspStruct_WL(pRsspStruct);	/*检查参数*/
		if (1 == rtn)
		{
			pRsspStruct->pDsuRsspInfoStruWL = (DSU_RSSP_INFO_STRU_WL*)malloc(sizeof(DSU_RSSP_INFO_STRU_WL));	/*为RSSPI配置信息申请空间*/
			if (NULL != pRsspStruct->pDsuRsspInfoStruWL)
			{
				CommonMemSet(pRsspStruct->pDsuRsspInfoStruWL, sizeof(DSU_RSSP_INFO_STRU_WL), 0, sizeof(DSU_RSSP_INFO_STRU_WL));
                localType = pRsspStruct->LocalType;
                locTypeId = ((UINT16)(localType << 8) & 0xFF00) | (pRsspStruct->LocalID & 0x00FF);
                rtn = DsuRsspInitWl(pFsName, inParamLen, inParam, locTypeId, pRsspStruct->LocalLogicId, pRsspStruct->pDsuRsspInfoStruWL);
				if (1 == rtn)
				{
					rtn = RsspInit_WL(pRsspStruct);
				}
			}
		}
	}

	return rtn;
}

/***********************************************************************
 * 方法名   : RsrFreeSpace
 * 功能描述 : 通过该函数，释放RSR层开辟的空间。
 * 输入参数 : 
 *	参数名		类型			输入输出	描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体，需进行初始值设置。
 * 返回值   : UINT8  0： 错误返回 1：正常返回
 ***********************************************************************/
UINT8 Rssp2FreeSpace(RSSP_INFO_STRU *pRsspStruct)
{
	UINT8 ret = 0;

	if(pRsspStruct == NULL)
	{
		ret=0;
	}
	else
	{
		/* RSSP层空间释放 */
        ret = RsspFreeSpace_WL(pRsspStruct);
		
		/* 队列数组空间释放 */
			
		/* RSSP层输出模块输出数组释放 */
        if (NULL != pRsspStruct->AppArray)
        {
            free(pRsspStruct->AppArray);
            pRsspStruct->AppArray = NULL;
        }

		/* RSR层输出模块输出中间变量数组空间释放 */
		if( NULL != pRsspStruct->VarArray)
		{
			free(pRsspStruct->VarArray);
			pRsspStruct->VarArray = NULL;
		}
		else
		{
			/* 什么不做 */
		}

		/* 协议输入给应用的数据队列长度 */	
		QueueElementFree(&pRsspStruct->DataToApp);

		/* 应用给协议输出的数据队列长度 */
		QueueElementFree(&pRsspStruct->OutputDataQueue);

		/* 内网接受队列长度 */
		/* 红网队列 */
		QueueElementFree(&pRsspStruct->OutnetQueueA);

		/* 蓝网队列 */
		QueueElementFree(&pRsspStruct->OutnetQueueB);

		 /*  记录数组  ：释放 */
        if (NULL != pRsspStruct->RecordArray)
        {
            free(pRsspStruct->RecordArray);
            pRsspStruct->RecordArray = NULL;
        }

	}
	return ret;
}
/***********************************************************************
 * 方法名   : Rssp2LnkStatus
 * 功能描述 : 得到某个安全接受节点的在本周期得通信状态：
 * 					 存在某个安全接受节点，则返回该安全节点的通信状态
 * 					 不存在某个安全接收节点，则返回错误
 * 输入参数 : 
 *	参数名			类型			输入输出		描述
 *  -------------------------------------------------------------
 *	destType  		UINT8			IN  			目标类型
 *  destId			UINT8			IN			    目标ID
 *  logicId			UINT16			IN			    目标逻辑ID
 *  pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体，需进行初始值设置。
 * 返回值   : UINT8  	 	 0：不存在该条链路，返回错误;
 *										0x17：链路在数据传输正常状态DATA
 *	  								0x2b：链路在无法传输数据状态HALT
 ***********************************************************************/
UINT8 Rssp2LnkStatus(UINT8 destType,UINT8 destId, UINT16 logicId, RSSP_INFO_STRU *pRsspStruct)
{
	UINT8 ret = 0;

	/* ADD 20130802 */
	if(pRsspStruct == NULL)
	{
		ret = 0;
		return ret;
	}
	/* RSPP 链路处理 */
	ret = RsspLnkStatus_WL(destType, destId, logicId, pRsspStruct);
	return ret;
}

/***********************************************************************
 * 方法名   : RsrLnkDelete
 * 功能描述 : 通过该函数，应用可以删除某个发送节点。当应用不再需要对某个对象发送数据时，利用该函数删除相应的发送节点。
 * 输入参数 : 
 *	参数名			类型		输入输出		描述
 *  --------------------------------------------------------------
 *	destType  		UINT8			IN  			目标类型
 *  destId			UINT8			IN			    目标ID
 *  pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体，需进行初始值设置。
 * 返回值   : UINT8  0：错误返回  1：正常返回
 ***********************************************************************/

 /*
 *函数名:	Rssp2LnkDelete
 *功能描述:	通过对方类型ID+对方逻辑ID删除链路
 *输入参数:	UINT8 dstType	对方类型
 *			UINT8 dstId	对方ID
 *			UINT16 dstLogicId	对方逻辑ID
 *			RSSP_INFO_STRU *pRsspStruct	RSSPI配置信息
 *输入出参:	无
 *输出参数:	无
 *返回值:	1:成功;0;失败
 */
UINT8 Rssp2LnkDelete(UINT8 dstType, UINT8 dstId, UINT16 dstLogicId, RSSP_INFO_STRU *pRsspStruct)
{
	UINT8 rtn = 0;	/*返回值*/
	UINT32 oppId = 0;	/*对方唯一ID*/
    UINT16 dstName = 0U;

	if (NULL != pRsspStruct)	/*输入参数非空*/
	{
#ifdef RSSP_NO_HLHT
        TypeId2DevName_WL(dstType, dstId, &dstName);	/*获取目的RSSPI唯一ID*/
        oppId = dstName;

#else
        oppId = DquToHlhtId(dstType, dstId, dstLogicId, pRsspStruct->pDsuRsspInfoStruWL);	/*获取目的RSSPI唯一ID*/
#endif

		PrintRecArrayByte(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, 0x77);
		PrintRecArray4ByteB(pRsspStruct->RecordArray, pRsspStruct->RecordArraySize, oppId);

		/*删除RSSP层链路*/
		rtn = RsspLnkDelete_WL(oppId, pRsspStruct);
	}

	return rtn;
}

/***********************************************************************
 * 方法名   : Rssp2Refresh
 * 功能描述 : 应用使用该函数来重新对本机RSSP层的中间变量重新赋值。
 * 输入参数 : 
 *	参数名					类型		输入输出		描述
 *  --------------------------------------------------------------
 *	sLen_Input  			UINT16			IN			中间数组的大小
 *	Input_Data_LnkMes		UINT8			IN			中间变量数组
 * 
 * 返回值   : UINT8  0：错误返回  1：正常返回
 ***********************************************************************/
UINT8 Rssp2Refresh(UINT16 sLen_Input, UINT8* Input_Data_LnkMes, RSSP_INFO_STRU* pRsspStruct)
{
    UINT8 ret = 0;

    ret = RsspRefresh_WL(sLen_Input, Input_Data_LnkMes, pRsspStruct); /*sLen_Input Input_Data_LnkMes 链路整数倍*/

    return ret;
}

/***********************************************************************
 * 方法名   : CheckRsspStruct_WL
 * 功能描述 : 对统一结构体的各个项目进行检查
 * 输入参数 : 
 *	参数名			类型					输入输出	描述
 *  --------------------------------------------------------------
 *	pRsspStruct  	const RSSP_INFO_STRU*	IN			RSSP层统一结构体
 * 
 * 返回值   : UINT8  0：错误返回  1：正常返回
 ***********************************************************************/
static UINT8 CheckRsspStruct_WL(const RSSP_INFO_STRU *pRsspStruct)
{
	UINT8 ret = 0;

	/* 统一结构体指针不为空 */
	if(pRsspStruct == NULL)
	{
		return ret;
	}

	/* 非零项目检查 */
	if( (pRsspStruct->MaxNumLink == 0) || (pRsspStruct->MaxNumPerCycle == 0))
	{
		return ret;
	}
	
	/* 最大输入应用数据 */
    if (0 == pRsspStruct->InputSize)
    {
        return ret;
    }

	/* 最大输出应用数据 */
    if (0 == pRsspStruct->OutputSize)
    {
        return ret;
    }

	/* 周期号 */
	if( pRsspStruct->CycleNum == NULL)
	{
		return ret;
	}

	ret = 1;
	return ret;
}

/*******************************************************************************************************
* 功能描述: 	   设置RSSP协议无发送应用数据周期数
* 输入参数: 		pNoSendAppData 无发送次收
*					pRsspStruct	RSSPI结构
* 输入输出参数: 	无
* 输出参数: 		无
* 返回值:			1:成功
*******************************************************************************************************/
UINT8 SetRsspNoSendAppData(UINT8 pNoSendAppData, RSSP_INFO_STRU *pRsspStruct)
{
	UINT8 ret = 1;
	pRsspStruct->NoSendAppData = pNoSendAppData;
	return ret;
}

/*
*功能描述:	获取RSSPI各网收到包数
*输入参数:	RSSP_INFO_STRU* rsspi rsspi变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
void RsspiGetNetsRecvPkgNum(RSSP_INFO_STRU* rsspi, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum)
{
    UINT32 oppHlhtId = 0u; /*对方互联互通ID*/
    UINT8 isGet = 0u; /*是否获取到*/
    UINT16 locAddr = 0u; /*本方地址*/
    UINT16 oppAddr = 0u; /*对方地址*/

    if ((NULL != rsspi) && (NULL != rsspi->NetsRecvPkgNums) && (NULL != rsspi->pDsuRsspInfoStruWL) && (NULL != redRecvPkgNum) && (NULL != blueRecvPkgNum)) /*参数检查*/
    {
        oppHlhtId = DquToHlhtId(dstType, dstId, dstLogicId, rsspi->pDsuRsspInfoStruWL);
        isGet = dsuRsspRGetAddrWl(oppHlhtId, rsspi->pDsuRsspInfoStruWL->LocEmitterName, &locAddr, &oppAddr, rsspi->pDsuRsspInfoStruWL);
        if (1u == isGet)
        {
            GetNetsRecvPkgNum_WL(rsspi, oppAddr, redRecvPkgNum, blueRecvPkgNum);
        }
    }
}

/*
*函数名:	RsspiGetSwapData
*功能描述:	获取输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据(2字节长度+N个[4字节SVC])
*           const UINT16 swapDataMaxLen 交互数据最大长度
*           RSSP_INFO_STRU* rsspi SFP结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiGetSwapData(UINT8 swapData[], const UINT16 swapDataMaxLen, RSSP_INFO_STRU* rsspi)
{
    UINT16 swapDataLen = 0u; /*交互数据位置*/
    UINT32 allLen = 0u; /*总长度*/
    UINT8* outAppData = NULL; /*输出应用数据*/
    UINT32 outAppIdx = 0u; /*输出应用数据位置*/
    UINT16 pkgLen = 0u; /*包长度*/
    UINT16 svcPos = 0u; /*SVC在appArray中位置*/
    UINT8 isBreak = 0u; /*是否退出遍历*/
    UINT8* record = NULL; /*日志数组*/
    UINT16 recordSize = 0u; /*日志数组长度*/

    if ((NULL != swapData) && (NULL != rsspi) && (2u < swapDataMaxLen)) /*非空检查*/
    {
        record = rsspi->RecordArray;
        recordSize = rsspi->RecordArraySize;
        PrintRecArrayByte(record, recordSize, GET_SVC);

        ShortToChar(0u, &swapData[0]); /*长度清零*/
        swapDataLen += 2u;

#ifdef DOUBLE_CPU_INTERACTIVE_SVC
        /*AppArray格式:4字节总长度+N个[2字节长度+1字节协议类型+2字节目的地址+2字节预留+网络数据]*/
        allLen = LongFromChar(&rsspi->AppArray[0]);
        if (0u < allLen) /*有输出*/
        {
            outAppData = &rsspi->AppArray[4]; /*应用数据开始地址*/
            while (outAppIdx < allLen) /*遍历输出应用数据*/
            {
                pkgLen = 2u + ShortFromChar(&outAppData[outAppIdx]);
                if ((outAppIdx + pkgLen) <= allLen) /*当前输出应用位置加长度小于等于输出应用长度,合法*/
                {
                    if ((swapDataLen + 4u) <= swapDataMaxLen) /*有空间*/
                    {
                        if ((RSSP_RSD_MSG_WL == outAppData[outAppIdx + 8u]) && (pkgLen >= 7u + RSSP_FIXED_LEN_WL))/*RSD帧*/
                        {
                            svcPos = (0u == rsspi->cpuId) ? 19u : 23u; /*当前CPU是0,SVC所在下标是19,否则是23*/
                        }
                        else if ((RSSP_SSE_MSG_WL == outAppData[outAppIdx + 8u]) && ((7u + RSSP_SSE_LEN_WL) == pkgLen)) /*SSE帧*/
                        {
                            svcPos = (0u == rsspi->cpuId) ? 17u : 21u; /*当前CPU是0,SVC所在下标是17,否则是21*/
                        }
                        else if ((RSSP_SSR_MSG_WL == outAppData[outAppIdx + 8u]) && ((7u + RSSP_SSR_LEN_WL) == pkgLen)) /*SSR帧*/
                        {
                            svcPos = (0u == rsspi->cpuId) ? 21u : 25u; /*当前CPU是0,SVC所在下标是21,否则是25*/
                        }
                        else /*错误帧类型*/
                        {
                            PrintRecArrayByte(record, recordSize, FRAME_TYPE_ERR);
                            PrintRecArrayByte(record, recordSize, outAppData[outAppIdx + 8u]);
                            isBreak = 1u;
                        }
                        if (0u == isBreak) /*帧类型正常*/
                        {
                            CommonMemCpy(&swapData[swapDataLen], (UINT32)4u, &outAppData[outAppIdx + svcPos], 4u); /*复制SVC*/
                            swapDataLen += 4u; /*累加交互数据长度*/
                            outAppIdx += pkgLen; /*偏移下标*/
                        }
                    }
                    else /*无空间*/
                    {
                        PrintRecArrayByte(record, recordSize, 4u);
                        PrintRecArray2ByteB(record, recordSize, swapDataLen);
                        PrintRecArray2ByteB(record, recordSize, swapDataMaxLen);

                        isBreak = 1u;
                    }
                }
                else /*包长度非法*/
                {
                    PrintRecArrayByte(record, recordSize, PKG_LEN_ERR);
                    PrintRecArray2ByteB(record, recordSize, pkgLen);
                    PrintRecArray4ByteB(record, recordSize, outAppIdx);
                    PrintRecArray4ByteB(record, recordSize, allLen);

                    isBreak = 1u;
                }
                if (1u == isBreak) /*退出遍历*/
                {
                    break;
                }
            }
            if (0u == isBreak) /*遍历完成*/
            {
                ShortToChar(swapDataLen - 2u, &swapData[0]); /*设置长度*/
            }
        }
        else
        {
            PrintRecArrayByte(record, recordSize, NO_OUT_APP);
            PrintRecArrayByte(record, recordSize, 0u);
        }
#endif
    }
}

/*
*函数名:	RsspiSetSwapData
*功能描述:	设置输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据(2字节长度+N个[4字节SVC])
*			RSSP_INFO_STRU* rsspi SFP结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiSetSwapData(UINT8 swapData[], RSSP_INFO_STRU* rsspi)
{
    /*PrtclSndBuff格式:4字节总长度+N个[2字节长度+1字节协议类型+2字节目的地址+2字节预留+网络数据]*/
    RsspiSetSwapDataForOut(swapData, &rsspi->AppArray[4], rsspi);
}

/*
*函数名:	RsspiSetSwapDataForOut
*功能描述:	设置输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据(2字节长度+N个[4字节SVC])
*           UINT8 outData[] 指定输出数据
*			RSSP_INFO_STRU* rsspi SFP结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiSetSwapDataForOut(UINT8 swapData[], UINT8 outData[], RSSP_INFO_STRU* rsspi)
{
    UINT16 swapDataLen = 0u; /*交互CRC长度*/
    UINT32 allLen = 0u; /*总长度*/
    UINT8 dstNum = 0u; /*目的地址CRC数量*/
    UINT8 i = 0u; /*用于循环*/
    UINT32 outAppIdx = 0u; /*输出应用数据位置*/
    UINT8* outAppData = NULL; /*输出应用数据*/
    UINT16 pkgLen = 0u; /*包长度*/
    UINT16 svcPos = 0u; /*SVC在appArray中位置*/
    UINT8 isBreak = 0u; /*是否退出遍历*/
    UINT16 crc16 = 0u; /*CRC16值*/
    UINT8* record = NULL; /*日志数组*/
    UINT16 recordSize = 0u; /*日志数组长度*/

    if ((NULL != swapData) && (NULL != outData) && (NULL != rsspi)) /*参数检查*/
    {
        record = rsspi->RecordArray;
        recordSize = rsspi->RecordArraySize;
        PrintRecArrayByte(record, recordSize, SET_SVC);

#ifdef DOUBLE_CPU_INTERACTIVE_SVC
        swapDataLen = ShortFromChar(&swapData[0]);
        if ((0u < swapDataLen) && (0u == (swapDataLen % 4u))) /*长度合法*/
        {
            /*PrtclSndBuff格式:4字节总长度+N个[2字节长度+1字节协议类型+2字节目的地址+2字节预留+网络数据]*/
            allLen = LongFromChar(&rsspi->AppArray[0]);
            if (0u < allLen) /*有输出*/
            {
                outAppData = &outData[0];
                dstNum = swapDataLen / 4u;
                for (i = 0u; i < dstNum; i++)
                {
                    pkgLen = 2u + ShortFromChar(&outAppData[outAppIdx]);
                    if ((outAppIdx + pkgLen) <= allLen) /*包长度大于等于(7字节头+23字节帧格式),且当前输出应用位置加长度小于等于输出应用长度,合法*/
                    {
                        if ((RSSP_RSD_MSG_WL == outAppData[outAppIdx + 8u]) && (pkgLen >= 7u + RSSP_FIXED_LEN_WL))/*RSD帧*/
                        {
                            svcPos = (0u == rsspi->cpuId) ? 23u : 19u; /*当前CPU是0,SVC所在下标是23,否则是19*/
                        }
                        else if ((RSSP_SSE_MSG_WL == outAppData[outAppIdx + 8u]) && ((7u + RSSP_SSE_LEN_WL) == pkgLen)) /*SSE帧*/
                        {
                            svcPos = (0u == rsspi->cpuId) ? 21u : 17u; /*当前CPU是0,SVC所在下标是21,否则是17*/
                        }
                        else if ((RSSP_SSR_MSG_WL == outAppData[outAppIdx + 8u]) && ((7u + RSSP_SSR_LEN_WL) == pkgLen)) /*SSR帧*/
                        {
                            svcPos = (0u == rsspi->cpuId) ? 25u : 21u; /*当前CPU是0,SVC所在下标是25,否则是21*/
                        }
                        else /*错误帧类型*/
                        {
                            PrintRecArrayByte(record, recordSize, FRAME_TYPE_ERR);
                            PrintRecArrayByte(record, recordSize, outAppData[outAppIdx + 8u]);

                            isBreak = 1u;
                        }

                        if (0u == isBreak) /*帧类型正常*/
                        {
                            CommonMemCpy(&outAppData[outAppIdx + svcPos], (UINT32)4u, &swapData[2u + 4u * i], (UINT32)4u); /*设置SVC*/

                            /*重新计算CRC16拷贝到outAppData*/
                            crc16 = ComputeMsgCRC16_WL(&outAppData[outAppIdx + 7u], pkgLen - 9u);
                            ShortToCharLE(crc16, &outAppData[outAppIdx + pkgLen - 2u]);
                        }

                        outAppIdx += pkgLen;
                    }
                    else /*包长度非法*/
                    {
                        PrintRecArrayByte(record, recordSize, PKG_LEN_ERR);
                        PrintRecArray2ByteB(record, recordSize, pkgLen);
                        PrintRecArray4ByteB(record, recordSize, outAppIdx);
                        PrintRecArray4ByteB(record, recordSize, allLen);

                        isBreak = 1u;
                    }
                    if (1u == isBreak) /*退出遍历*/
                    {
                        break;
                    }
                }
            }
            else
            {
                PrintRecArrayByte(record, recordSize, NO_OUT_APP);
                PrintRecArrayByte(record, recordSize, 0u);
            }
        }
        else
        {
            PrintRecArrayByte(record, recordSize, SET_SVC_DATA_ERR);
            PrintRecArray2ByteB(record, recordSize, swapDataLen);
        }
#endif
    }
}

/*
*函数名:	RsspiDelAllLink
*功能描述:	RSSPI删除所有链路
*输入参数:	RSSP_INFO_STRU* rsspi RSSPI结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiDelAllLink(RSSP_INFO_STRU* rsspi)
{
    UINT8 i = 0u; /*用于循环*/

    if ((NULL != rsspi) && (NULL != rsspi->LnkNodeArr)) /*参数检查*/
    {
        for (i = 0; i < rsspi->LnkNodeNum; i++) /*遍历所有链路*/
        {
            RsspMemSet_WL((UINT8*)&rsspi->LnkNodeArr[i], sizeof(Lnk_Info_Node_Stru_WL), 0u); /*链路信息清零*/
            rsspi->LnkNodeArr[i].NodeStatus = Replace_WL; /*节点状态设置可覆盖*/
        }

        if (NULL != rsspi->RecordArray) /*记录日志*/
        {
            PrintRecArray2ByteB(rsspi->RecordArray, rsspi->RecordArraySize, (UINT16)0xF0F0u);
        }
    }
}

/*--------------------------------------------------------------------
 * RSSP层外部使用函数定义 End
 *--------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
