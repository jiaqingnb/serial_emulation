/********************************************************                                                                                                       
* 文件名  ： RsspInitWl.h   
* 版权说明： 
* 版本号  ： 1.0
* 创建时间： 2013.3.1
* 作者    ： 车载及协议部
* 功能描述： RSSP层初始化以及内存释放。
* 使用注意：调用RsspInit函数后，必须有相对应的调用RsspFreeSpace
*   
* 修改记录：   
*   时间		修改人	理由
* ------------  ------  ---------------
*	2013.3.1	楼宇伟	初版作成
********************************************************/ 

#include "stdlib.h"
#include "CommonMemory.h"
#include "RsspIFWl.h"
#include "RsspStructWl.h"
#include "RsspCommonWl.h"
#include "RsspLnkNodeMgrWl.h"
#include "RsspTmpMsgLnkCommonWl.h"
#include "dsuRsspFunc.h"


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************
 * 方法名   : RsspInit_WL
 * 功能描述 : 通过该函数，实现对RSRSP的初始化函数。根据应用设定的基本值，计算各队列的大小。
 * 输入参数 : 
 *	参数名				类型							输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT			RSSP层统一结构体，需进行初始值设置。
 * 
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
	UINT8 RsspInit_WL(RSSP_INFO_STRU* pRsspStruct)
	{
		UINT8 rtn = 0; /*返回值*/
		UINT32 mallocLen = 0u; /*分配空间长度*/

		if (NULL != pRsspStruct) /*参数非空判断*/
		{
			/*记录数组分配空间*/
			pRsspStruct->RecordArray = (UINT8*)malloc(pRsspStruct->RecordArraySize + 2);
			if (NULL != pRsspStruct->RecordArray)
			{
				CommonMemSet(pRsspStruct->RecordArray, sizeof(UINT8) * (pRsspStruct->RecordArraySize + 2), 0, sizeof(UINT8) * (pRsspStruct->RecordArraySize + 2));

				/*输出协议给应用数组AppArray分配空间*/
				pRsspStruct->AppArraySize = (pRsspStruct->OutputSize + RSSP_FIXED_LEN_WL + 14 + 26) * (pRsspStruct->MaxNumLink) + 2;
				pRsspStruct->AppArray = (UINT8*)malloc(pRsspStruct->AppArraySize);
				if (NULL != pRsspStruct->AppArray)
				{
					CommonMemSet(pRsspStruct->AppArray, sizeof(UINT8) * pRsspStruct->AppArraySize, 0, sizeof(UINT8) * pRsspStruct->AppArraySize);

					/*链路数组分配空间*/
					pRsspStruct->VarArraySize = RSSP_VAR_NODE_LEN_WL * pRsspStruct->MaxNumLink + 32 + 14 * (pRsspStruct->MaxNumLink + 2) + 32;
					pRsspStruct->VarArray = (UINT8*)malloc(pRsspStruct->VarArraySize);
					if (NULL != pRsspStruct->VarArray)
					{
						CommonMemSet(pRsspStruct->VarArray, sizeof(UINT8) * pRsspStruct->VarArraySize, 0, sizeof(UINT8) * pRsspStruct->VarArraySize);

						/*数据帧分配空间*/
						pRsspStruct->FrameDataMaxLen = 2 + 8 + RSSP_FIXED_LEN_WL + 32 + (pRsspStruct->InputSize > pRsspStruct->OutputSize ? pRsspStruct->InputSize : pRsspStruct->OutputSize);
						pRsspStruct->FrameData = (UINT8*)malloc(pRsspStruct->FrameDataMaxLen);
						if (NULL != pRsspStruct->FrameData)
						{
							CommonMemSet(pRsspStruct->FrameData, sizeof(UINT8) * pRsspStruct->FrameDataMaxLen, 0, sizeof(UINT8) * pRsspStruct->FrameDataMaxLen);

							/*数据堆栈链表分配空间*/
							rtn = InitMsgNodeStack_WL(pRsspStruct->MaxNumLink * (pRsspStruct->MaxNumPerCycle + 2) * 2, pRsspStruct->FrameDataMaxLen, &pRsspStruct->StackOfDataLink);
							if (1 == rtn) /*链路节点分配空间*/
							{
								rtn = InitLnkNodeArr_WL(pRsspStruct->MaxNumLink, pRsspStruct);
							}
							if (1 == rtn) /*输入应用给协议数据队列分配空间*/
							{
								rtn = 0; /*默认返回0*/
								pRsspStruct->OutnetQueueSize = (pRsspStruct->OutputSize + 8) * pRsspStruct->MaxNumLink * (pRsspStruct->MaxNumPerCycle + 2);
								rtn = QueueInitial(&pRsspStruct->OutnetQueueA, pRsspStruct->OutnetQueueSize);
								if (1 == rtn)
								{
									rtn = QueueInitial(&pRsspStruct->OutnetQueueB, pRsspStruct->OutnetQueueSize);
								}
							}
							if (1 == rtn) /*输入协议给应用数据队列分配空间*/
							{
								pRsspStruct->DataToAppSize = (pRsspStruct->InputSize + 6) * pRsspStruct->MaxNumLink * (pRsspStruct->MaxNumPerCycle + 2);
								rtn = QueueInitial(&pRsspStruct->DataToApp, pRsspStruct->DataToAppSize);
							}
							if (1 == rtn) /*输出应用给协议数据队列分配空间*/
							{
								pRsspStruct->OutputDataQueueSize = (pRsspStruct->OutputSize + 8) * pRsspStruct->MaxNumLink * (pRsspStruct->MaxNumPerCycle + 2);
								rtn = QueueInitial(&pRsspStruct->OutputDataQueue, pRsspStruct->OutputDataQueueSize);
							}
							if (1 == rtn) /*输出应用给协议数据队列分配空间*/
							{
								rtn = 0; /*默认返回0*/
								/*无应用发送数据个数默认值为3*/
								pRsspStruct->NoSendAppData = NONE_SEND_APP_DATA_WL;

								pRsspStruct->NetsRecvPkgNums = (RsppiNetsRecvPkgNum*)malloc(sizeof(RsppiNetsRecvPkgNum) * pRsspStruct->MaxNumLink);
								if (NULL != pRsspStruct->NetsRecvPkgNums)
								{
									CommonMemSet(pRsspStruct->NetsRecvPkgNums, sizeof(RsppiNetsRecvPkgNum) * pRsspStruct->MaxNumLink, 0, sizeof(RsppiNetsRecvPkgNum) * pRsspStruct->MaxNumLink);

									mallocLen = sizeof(UINT8) * ((pRsspStruct->OutputSize > pRsspStruct->InputSize) ? pRsspStruct->OutputSize : pRsspStruct->InputSize);
									mallocLen += 32u;
									pRsspStruct->gpSendSingleOutMsg = (UINT8*)malloc(mallocLen);
									if (NULL != pRsspStruct->gpSendSingleOutMsg)
									{
										CommonMemSet(pRsspStruct->gpSendSingleOutMsg, mallocLen, 0u, mallocLen);

										rtn = 1; /*初始化正常*/
									}

								}

							}
						}
					}
				}
			}
		}

		return rtn;
	}

/***********************************************************************
 * 方法名   : RsspFreeSpace_WL
 * 功能描述 : 通过该函数，释放RSSP层开辟的空间。
 * 输入参数 : 
 *	参数名		类型			输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT			RSSP层统一结构体，需进行初始值设置。
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 RsspFreeSpace_WL(RSSP_INFO_STRU *pRsspStruct)
{
	UINT8 ret = 1;

	/* 全局通信节点数组空间释放 */
	DestoryLnkNodeArr_WL(pRsspStruct);

	/* 临时报文堆栈空间释放:不会失败  */
	ret = DestoryMsgNodeStack_WL(&pRsspStruct->StackOfDataLink);

	/* 临时缓存区 */
	if (NULL != pRsspStruct->FrameData)
	{
		free(pRsspStruct->FrameData);
		pRsspStruct->FrameData = NULL;
	}
	else
	{
		/* 什么不做 */
	}
#ifndef RSSP_NO_HLHT
	/*释放RSSP配置信息*/
	if (NULL != pRsspStruct->pDsuRsspInfoStruWL)
	{
		FreeDsuRsspi(&pRsspStruct->pDsuRsspInfoStruWL);
	}
#endif
	return ret;
}

#ifdef __cplusplus
}
#endif
