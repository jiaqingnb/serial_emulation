/********************************************************                                                                                                            
* 文件名  ： RsspReciveWl.c   
* 版权说明： 北京瑞安时代科技有限责任公司 
* 版本号  ： 1.0
* 创建时间： 2009.11.25
* 作者    ： 车载及协议部
* 功能描述： Rssp层对外输入实现 
* 使用注意： 
*		  必须在RsspInit函数调用才能，调用本文件内的函数。
* 修改记录：   
* 修改记录：   
*   时间		修改人	理由
* ------------  ------  ---------------
*	2009.12.07	楼宇伟	初版作成
*	2009.12.16	楼宇伟	记录追加
*	2010.01.14	楼宇伟	与卡斯柯调试后，本方除去定期同步功能
*	2010.01.23	楼宇伟	集成测试缺陷对应，变为Halt态时，同时发送SSE次数清零
*	2010.05.10	楼宇伟	输入模块的记录中追加RSSP周期开始标识以及周期号
*   2011.12.06  王佩佩  修改AnalysisSSEnqMsg函数。
*   2011.12.06  王佩佩  修改AnalysisBsdMsg函数。
*   2011.12.06  王佩佩  修改AnalysisSSRepMsg函数。
*   2011.12.06  王佩佩  增加CheckAddr函数。检查报文信息与通信节点信息是否一致。
*   2013.02.16  王佩佩  修改WriteRsToAppQue函数
*   2013.03.01  王佩佩  将AnalysisBsdMsg修改为AnalysisRsdMsg,将BSD报文修改成RSd报文
*   2013.03.01  王佩佩  将AnalysisBsdMsg修改为AnalysisRsdMsg函数
********************************************************/ 

#include "RsspIFWl.h" 
#include "RsspStructWl.h"
#include "RsspCommonWl.h"
#include "RsspLnkNodeMgrWl.h"
#include "RsspTmpMsgLnkCommonWl.h"
#include "RsspRecordDelareWl.h"
#include "CommonRecord.h"
#include "dsuRsspFunc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------
 * RSSP层输入模块内部使用函数申明 Start
 *--------------------------------------------------------------------*/
 static UINT8 WriteRsToAppQue_WL(DataNode_WL *pHead,QueueStruct* RsToApp,UINT8 *RecordArray, UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru);
 static UINT8 AnalysisSSEnqMsg_WL(const DataNode_WL* pNode,UINT8 *RecordArray, UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru);
 static UINT8 AnalysisSSRepMsg_WL(const DataNode_WL* pNode,UINT8 *RecordArray, UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru);
 static UINT8 AnalysisRsdMsg_WL(DataNode_WL* pNode, UINT8 *RecordArray, UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru);
 static UINT8 UptLnkNodeInfoBySSE_WL(const DataNode_WL *pDataNode, Lnk_Info_Node_Stru_WL *pLnkNode);
 static UINT8 UptLnkNodeInfoBySSR_WL(const DataNode_WL *pDataNode, Lnk_Info_Node_Stru_WL *pLnkNode, UINT8 *RecordArray, UINT16 RecordArraySize);
 static UINT8 CheckAddr_WL(const DataNode_WL *pDataNode, const Lnk_Info_Node_Stru_WL *pLnkNode);

 static UINT8 CheckRsdMsg_WL(const UINT8 *pBsdMsg,Lnk_Info_Node_Stru_WL *pLnkNode, RSSP_INFO_STRU *pRsspStru);

 static void RecvLnkStatusMng_WL(RSSP_INFO_STRU *pRsspStru);

 static void ReadNetData(QueueStruct* pInputQue, DataNode_WL** pHeadNode, RSSP_INFO_STRU* rssp, const UINT8 netIdx);
 static void ClearNetsRecvPkgNum(RSSP_INFO_STRU* rsspi);
 static void SetNetsRecvPkgNum(RSSP_INFO_STRU* rsspi, const UINT16 dstAddr, const UINT8 netIdx);

 /*
*函数名:	RsspReceive_WL
*功能描述:	RSSPI输入处理
*输入参数:	RSSP_INFO_STRU* rssp RSSPI结构
*输入出参:	无
*输出参数:	无
*返回值:	是否成功,0:不成功,1:成功
*/
 UINT8 RsspReceive_WL(RSSP_INFO_STRU* rssp)
 {
	 UINT8 rtn = 0; /*返回值*/
	 DataNode_WL* headNode = NULL; /*头节点*/
	 DataNode_WL* curNode = NULL; /*当前节点*/
	 DataNode_WL* lastNode = NULL; /*上节点*/
	 DataNode_WL* nexNode = NULL; /*下节点*/
	 UINT8 isDelNode = 0; /*是否删除节点*/
	 UINT8 frameType = 0; /*数据帧类型*/
	 UINT8 proFun = 0; /*处理函数结果*/

	 if ((NULL != rssp) && (NULL != rssp->RecordArray) && (NULL != rssp->StackOfDataLink)) /*参数非空检查*/
	 {
		 QueueClear(&rssp->DataToApp); /*清理协议给应用数据队列*/

		 ClearNetsRecvPkgNum(rssp);

		 PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RED_NET_DATA);	/*记录日志*/
		 ReadNetData(&rssp->OutnetQueueA, &headNode, rssp, 0); /*读取红网队列数据,到输入数据堆栈中*/
		 PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, BLUE_NET_DATA); /*记录日志*/
		 ReadNetData(&rssp->OutnetQueueB, &headNode, rssp, 1); /*读取蓝网队列数据,到输入数据堆栈中*/

		 PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RSSP_INPUT);	/*记录日志*/

		 if (NULL != headNode) /*有数据*/
		 {
			 curNode = headNode; /*从头节点开始*/
			 while (NULL != curNode) /*遍历链表处理*/
			 {
				 isDelNode = 0; /*默认不删除*/
				 frameType = curNode->Data[RSSP_MSG_TYPE_POS_WL]; /*数据帧类型*/
				 if (RSSP_SSE_MSG_WL == frameType) /*SSE帧*/
				 {
					 AnalysisSSEnqMsg_WL(curNode, rssp->RecordArray, rssp->RecordArraySize, rssp);
					 isDelNode = 1; /*删除节点*/
				 }
				 else if (RSSP_SSR_MSG_WL == frameType) /*SSR帧*/
				 {
					 AnalysisSSRepMsg_WL(curNode, rssp->RecordArray, rssp->RecordArraySize, rssp);
					 isDelNode = 1; /*删除节点*/
				 }
				 else /*RSD帧*/
				 {
					 proFun = AnalysisRsdMsg_WL(curNode, rssp->RecordArray, rssp->RecordArraySize, rssp);
					 if (0 == proFun) /*处理失败*/
					 {
						 isDelNode = 1; /*删除节点*/
					 }
				 }

				 nexNode = curNode->pNextNode; /*存下节点,删除当前节点会清当前节点的下节点指向*/
				 if (1 == isDelNode) /*是删除节点*/
				 {
					 proFun = DelTmpMsgLnkNode_WL(curNode, lastNode, &headNode, rssp->StackOfDataLink);
					 if (0 == proFun)
					 {
						 PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, INPUT_DEL_NODE_ERR);	/*记录日志*/
					 }
				 }
				 else /*不删除节点*/
				 {
					 lastNode = curNode; /*更新上节点*/
				 }
				 curNode = nexNode; /*准备处理下节点*/
			 }

			 proFun = WriteRsToAppQue_WL(headNode, &rssp->DataToApp, rssp->RecordArray, rssp->RecordArraySize, rssp);
			 if (0 == proFun)
			 {
				 PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, INPUT_WRITE_ERR);	/*记录日志*/
			 }
		 }

		 RecvLnkStatusMng_WL(rssp);

		 rtn = 1;
	 }

	 return rtn;
 }
  
 /*--------------------------------------------------------------------
 * RSSP层输入模块内部使用函数实现 Start
 *--------------------------------------------------------------------*/
/***********************************************************************
 * 方法名   : AnalysisSSEnqMsg_WL
 * 功能描述 : 本函数对收到的SSE消息进行解帧处理，取得相关信息。
 * 输入参数 : 
 *	参数名	 类型				输入输出	描述
 *  --------------------------------------------------------------
 *	pNode	 const DataNode_WL*	IN			信号层到Rssp层的输入队列
 *	RecordArray		UINT8*			INOUT		记录数组
 *  RecordArraySize	UINT16			IN			记录数组大小 
 * 

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 AnalysisSSEnqMsg_WL(const DataNode_WL* pNode,UINT8 *RecordArray, UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru)
{
	UINT8 ret = 0;
	Lnk_Info_Node_Stru_WL *retLnkNodePtr = NULL;
	/* wangpeipei 20111201 ADD*/
	UINT16 SrcAddr = 0;
	UINT16 DesAddr = 0;
	/* wangpeipei 20111201 ADD*/
	/* 报文长度 == 18（不含CRC）*/
	if((RSSP_SSE_LEN_WL -CRC_16_LEN_WL) == pNode->DataLen)
	{
		/* wangpeipei 20111201 ADD S */
		DesAddr = ShortFromCharLE(&pNode->Data[RSSP_SRC_ADD_POS_WL]);
	    SrcAddr = ShortFromCharLE(&pNode->Data[RSSP_DEST_ADD_POS_WL]);
		retLnkNodePtr = SearchLnkSse_SsrNode_WL(SrcAddr,DesAddr, pRsspStru);
        /* wangpeipei 20111201 ADD E */
		/* 判断通信节点是否存在 */
		if( NULL != retLnkNodePtr)	
		{
			/* 通信节点存在，那么检查报文的信息是否和该通信节点的一致 */
            /* wangpeipei 20111201 ADD S */
			ret = CheckAddr_WL(pNode,retLnkNodePtr);
			/* wangpeipei 20111201 ADD E */
			if(0 == ret)
			{
				/* 不一致 ，记录 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
				PrintFiles(RecordArray,RecordArraySize,"W:NOT Node Msg.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
				PrintRecArrayByte(RecordArray, RecordArraySize, CHK_SSE_ADDR_ERR);
				PrintRecArray2ByteB(RecordArray, RecordArraySize, DesAddr);

#endif
			}
			else	/* 检查通过 */
			{
				/* 更新链路基本信息结构体 */
				ret = UptLnkNodeInfoBySSE_WL(pNode,retLnkNodePtr);

				PrintRecArray2ByteB(RecordArray, RecordArraySize, DesAddr);
				PrintRecArrayByte(RecordArray, RecordArraySize, pNode->Data[RSSP_MSG_TYPE_POS_WL]);
				PrintRecArray4ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pNode->Data[RSSP_TC_POS_WL]));
			}		
		}
		else
		{
			/* 记录 通信节点不存在 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
			PrintFiles(RecordArray,RecordArraySize,"W:Node Not Exist.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
			PrintRecArrayByte(RecordArray, RecordArraySize, INPUT_SSE_NO_USE_NODE);
			PrintRecArray2ByteB(RecordArray, RecordArraySize, DesAddr);
#endif
		}
	}
	else	/* 长度不正 */
	{
		/* 记录 正常返回 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
		PrintFiles(RecordArray,RecordArraySize,"W:Msg Len Err.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
		PrintRecArrayByte(RecordArray, RecordArraySize, SSE_LEN_ERR);

#endif
	}

	ret = 1;
	return ret;
}

/***********************************************************************
 * 方法名   : AnalysisSSRepMsg_WL
 * 功能描述 : 本函数对收到的SSR消息进行解帧处理，取得相关信息。
 * 输入参数 : 
 *	参数名	 类型				输入输出	描述
 *  --------------------------------------------------------------
 *	pNode	 const DataNode_WL*	IN			信号层到Rssp层的输入队列
 *	RecordArray		UINT8*			INOUT		记录数组
 *  RecordArraySize	UINT16			IN			记录数组大小
 *

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 AnalysisSSRepMsg_WL(const DataNode_WL* pNode,UINT8 *RecordArray,UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru)
{
	UINT8 ret = 0;
	Lnk_Info_Node_Stru_WL *retLnkNodePtr = NULL;
	/* wangpeipei 20111201 ADD*/
	UINT16 SrcAddr = 0;
	UINT16 DesAddr = 0;
	/* wangpeipei 20111201 ADD*/
	/* 报文长度 == 23（不含CRC）*/
	if((RSSP_SSR_LEN_WL-CRC_16_LEN_WL) == pNode->DataLen)
	{
		/* wangpeipei 20111201 ADD */
		DesAddr = ShortFromCharLE(&pNode->Data[RSSP_SRC_ADD_POS_WL]);
	    SrcAddr = ShortFromCharLE(&pNode->Data[RSSP_DEST_ADD_POS_WL]);
		retLnkNodePtr = SearchLnkSse_SsrNode_WL(SrcAddr,DesAddr,pRsspStru);
        /* wangpeipei 20111201 ADD */
		/* 判断通信节点是否存在 */
		if( NULL != retLnkNodePtr)	
		{
			/* 通信节点存在，那么检查报文的信息是否和该通信节点的一致 */
			/* wangpeipei 20111201 ADD S */
			ret = CheckAddr_WL(pNode,retLnkNodePtr);
			/* wangpeipei 20111201 ADD E */
			if(0 == ret)
			{
				/* 不一致 ，记录 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
				PrintFiles(RecordArray,RecordArraySize,"W:NOT Node Msg.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
				PrintRecArrayByte(RecordArray, RecordArraySize, CHK_SSR_ADDR_ERR);
				PrintRecArray2ByteB(RecordArray, RecordArraySize, DesAddr);
#endif
			}
			else	/* 检查通过 */
			{
				/* 更新链路基本信息结构体 */
				ret = UptLnkNodeInfoBySSR_WL(pNode, retLnkNodePtr, RecordArray, RecordArraySize);
				if( 0 == ret)
				{
					/* 更新出错，说明不是对应的SSR报文 那么记录 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
					PrintFiles(RecordArray,RecordArraySize,"W:SSR Msg Err.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
					PrintRecArrayByte(RecordArray, RecordArraySize, UPD_SSR_ERR);
#endif
				}
			}		
		}
		else
		{
			/* 记录 通信节点不存在 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
			PrintFiles(RecordArray,RecordArraySize,"W:NOT Node Msg.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
			PrintRecArrayByte(RecordArray,RecordArraySize, INPUT_SSR_NO_USE_NODE);
			PrintRecArray2ByteB(RecordArray, RecordArraySize, DesAddr);
#endif
		}
	}
	else	/* 长度不正 */
	{
		/* 记录 正常返回 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
		PrintFiles(RecordArray,RecordArraySize,"W:Msg Len Err.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
		PrintRecArrayByte(RecordArray, RecordArraySize, SSR_LEN_ERR);

#endif
	}

	ret = 1;
	return ret;
}

/***********************************************************************
 * 方法名   : AnalysisRsdMsg_WL
 * 功能描述 : 本函数对收到的RSD消息进行解帧处理。
 * 输入参数 : 
 *	参数名			类型			输入输出	描述
 *  --------------------------------------------------------------
 *	pNode			DataNode_WL*	IN			信号层到Rssp层的输入队列
 *	RecordArray		UINT8*			INOUT		记录数组
 *  RecordArraySize	UINT16			IN			记录数组大小

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 AnalysisRsdMsg_WL(DataNode_WL* pNode,UINT8 *RecordArray, UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru)
{
	UINT8 ret = 0;					/* 返回值 */
	UINT16 LocalDevName = 0;		/* 本方标识符 */
	UINT16 RsspMsgLen = 0;			/* RSSP消息体长度 */
	Lnk_Info_Node_Stru_WL *retLnkNodePtr = NULL;
	/* wangpeipei 20111201 ADD*/
	UINT16 srcAddr = 0;
	UINT16 bsdAddr = 0;
	/* wangpeipei 20111201 ADD*/
	
	/* 本方标识符*/
	TypeId2DevName_WL(pRsspStru->LocalType, pRsspStru->LocalID,&LocalDevName);
	/* 消息体长度 */
	RsspMsgLen = ShortFromCharLE(&pNode->Data[RSSP_DATA_SIZE_POS_WL]);	

	/* RSSP报文长度 >= RSD固定长度(20,不含CRC) 
	 *  && 消息体长度 == RSSP报文长度-RSD固定长度  + 2个 CRCM长度
	 */
	if((RSSP_DATA_POS_WL <= pNode->DataLen)
		&&((pNode->DataLen - RSSP_CRCM_1_POS_WL)== RsspMsgLen))
	{	
        /* wangpeipei 20111201 ADD */
		srcAddr= ShortFromCharLE(&pNode->Data[RSSP_SRC_ADD_POS_WL]);
	        bsdAddr =  ShortFromCharLE(&pNode->Data[RSSP_DEST_ADD_POS_WL]);
		retLnkNodePtr = SearchLnkBsdNode_WL(srcAddr,bsdAddr, pRsspStru);
        /* wangpeipei 20111201 ADD */

		/* 通信节点数组内通信关系存在判断 */
		if( NULL != retLnkNodePtr)		/* 通信节点存在 */
		{			
			/* 检查RSD报文信息是否和通信节点一致 */
			/* wangpeipei 20111206 ADD S */
			ret = CheckAddr_WL(pNode,retLnkNodePtr);
			/* wangpeipei 20111206 ADD E */
			if(1 == ret)	/* 验证通过 */
			{					
				/* 验证RSD信息是否合法 */
				ret = CheckRsdMsg_WL(pNode->Data,retLnkNodePtr, pRsspStru);
				if(1 == ret)
				{
					/* 验证通过 */
					pNode->DestTypeId = retLnkNodePtr->DestTypeId;	/*设置数据节点的对方类型ID*/
					/* 上次收到报文本地时钟设定为当前周期号 */
					retLnkNodePtr->RsspLinInfo.TimeStmp.LastLocalRecTC = retLnkNodePtr->RsspLinInfo.TimeStmp.TC;
					/* 无数据周期数设定为0 */
					retLnkNodePtr->RsspLinInfo.NoRecvDataCycleNum = 0;

					/* 处理成功 */
					ret = 1;
				}
				else
				{
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
					PrintFiles(RecordArray,RecordArraySize,"W:BSD Invalid.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
					PrintRecArrayByte(RecordArray, RecordArraySize, CHK_RSD_ERR);
					PrintRecArray2ByteB(RecordArray, RecordArraySize, srcAddr);
#endif
					/* 验证失败 */
					ret = 0;
				}
			}
			else
			{
				/* 不正常 记录 ,错误返回 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
			PrintFiles(RecordArray,RecordArraySize,"W:NOT Node Msg.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
			PrintRecArrayByte(RecordArray, RecordArraySize, CHK_RSD_ADDR_ERR);
			PrintRecArray2ByteB(RecordArray, RecordArraySize, srcAddr);
#endif
				ret = 0;
			}			
		}
		else	/* 通信节点不存在 if( NULL != retLnkNodePtr)	 */
		{
			/* 不存在，则取出可覆盖节点机进行覆盖 */
			/* wangpeipei 20111206 ADD S */
            retLnkNodePtr = GetReplaceNode_WL(pRsspStru);

			/* wangpeipei 20111206 ADD E */
			if( NULL != retLnkNodePtr)	/* 覆盖节点存在 */
			{
				ret = recvReplaceLnkNode_WL(LocalDevName, pNode, retLnkNodePtr, pRsspStru->pDsuRsspInfoStruWL);
				if( 0 == ret)
				{
					/* 记录Warning */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
			PrintFiles(RecordArray,RecordArraySize,"W:Rplace Node Err.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
			PrintRecArrayByte(RecordArray, RecordArraySize, INPUT_NEW_NODE_ERR);
			PrintRecArray2ByteB(RecordArray, RecordArraySize, srcAddr);
#endif
				}
				else
				{
					retLnkNodePtr->CycleNum = *pRsspStru->CycleNum;	/*应用周期给协议*/
#ifdef RSSP_NO_HLHT
					pNode->DestTypeId = (UINT16)retLnkNodePtr->DestDevName;
#else
					pNode->DestTypeId = DquToDevTypeId(retLnkNodePtr->DestDevName, pRsspStru->pDsuRsspInfoStruWL);	/*对方设备类型ID*/
#endif
					retLnkNodePtr->DestTypeId = pNode->DestTypeId;	/*对方类型ID*/
					retLnkNodePtr->DestLogicId = pNode->DestLogicId;	/*对方逻辑ID*/
					AdjustLnkCycle(pRsspStru->LocalType, retLnkNodePtr->CycleNum, retLnkNodePtr);
					if (1 == retLnkNodePtr->ExeCycle)	/*此链路是执行周期则执行*/
					{
							/*   链路刷新计数器由应用填写20160906  S   */
						retLnkNodePtr->RsspLinInfo.TimeStmp.TC = retLnkNodePtr->CycleNum;
						/*   链路刷新计数器由应用填写20160906  E   */
					}

					PrintRecArrayByte(RecordArray, RecordArraySize, INPUT_NEW_NODE);
					PrintRecArray2ByteB(RecordArray, RecordArraySize, srcAddr);
				}
				/* 错误返回，以并在上面处理删除该节点 */
				ret = 0;
			}
			else	/* 覆盖节点不存在 */
			{
				ret = 0;	/* 错误返回，丢弃报文 */
				/* 记录 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
				PrintFiles(RecordArray,RecordArraySize,"W:No Rplace Node.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
				PrintRecArrayByte(RecordArray, RecordArraySize, NO_NODE);
				PrintRecArray2ByteB(RecordArray, RecordArraySize, srcAddr);

#endif
			}
	
		}  /* else */
	}
	else	/* 长度不合法 记录 */
	{
		/* 记录，错误返回 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
		PrintFiles(RecordArray,RecordArraySize,"W:Msg Len Err.\n");	
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
		PrintRecArrayByte(RecordArray,RecordArraySize, INPUT_RSD_LEN_ERR);

#endif
		ret = 0;
	}

	return ret;
}
/***********************************************************************
 * 方法名   : WriteRsToAppQue_WL
 * 功能描述 : 对解码成功的RSD报文的应用数据写入输出队列。
 *				如果有多帧数据只取最新的报文数据。
 * 输入参数 : 
 *	参数名			类型			输入输出	描述
 *  --------------------------------------------------------------
 *	pHead			DataNode_WL*		IN			冗余处理到Rssp层的输入队列
 *  RsToApp			QueueStruct*	INOUT		RSSP到应用的输出队列
 *	RecordArray		UINT8*			INOUT		记录数组
 *  RecordArraySize	UINT16			IN			记录数组大小

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 WriteRsToAppQue_WL(DataNode_WL *pHead,QueueStruct* RsToApp,UINT8 *RecordArray, UINT16 RecordArraySize, RSSP_INFO_STRU *pRsspStru)
{
	UINT8 ret = 0;
	DataNode_WL *pCur = NULL,*pNext = NULL;
	UINT16 SrcAddr = 0, DestAddr = 0;							/* 源地址 目标地址 */
	UINT16 AppDataLen = 0;	
	UINT32  destName = 0;
	/* 应用数据长度 */
	const UINT8 LEN_OUT_QUE_POS = 0;							/* 输出对列数据长度保存位置 */
	/* wangpeipei Del 2013.02.16 S */
	/*const UINT8 SRC_ADDR_OUT_QUE_POS = LEN_OUT_QUE_POS+2;	*/	/* 输出对列对方源地址保存位置 */
	/*const UINT8 DEST_ADDR_OUT_QUE_POS = SRC_ADDR_OUT_QUE_POS+2;*/	/* 输出对列对方BSD地址保存位置 */
	/*const UINT8 DATA_OUT_QUE_POS = DEST_ADDR_OUT_QUE_POS+2;*/		/* 输出对列数据保存位置 */
    /* wangpeipei Del 2013.02.16 E */

	/* wangpeipei ADD 2013.02.16 S */
	const UINT8 SRC_TYPE_OUT_QUE_POS = LEN_OUT_QUE_POS+2;			/*输出对方类型ID保存位置 */
	const UINT8 DST_LOGIC_ID_QUE_POS = SRC_TYPE_OUT_QUE_POS +2;		/* 输出对方逻辑ID保存位置 */
	const UINT8 DATA_OUT_QUE_POS = DST_LOGIC_ID_QUE_POS + 2;		/* 输出对列数据保存位置 */
    /* wangpeipei ADD 2013.02.16 E */
	pCur = pHead;
	/* 当前节点不为空 */
	while(pCur != NULL)
	{
		/* 如果当前节点与下一个节点的标识符相等，则把该报文的数据丢弃 */
		pNext = pCur->pNextNode;
		if( (NULL != pNext) && (pNext->DevName == pCur->DevName) )
		{
			/* 说明该标识符对应的发送方还有最新报文，本节点为旧数据，不处理 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
	PrintFiles(RecordArray,RecordArraySize,"W:OLD MSG.\n");	/* 非最新报文丢弃 */
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
	PrintRecArrayByte(RecordArray, RecordArraySize, RSD_DUP);	 /* 非最新报文丢弃 */
	PrintRecArray4ByteB(RecordArray, RecordArraySize, pCur->DevName);
	PrintRecArray4ByteB(RecordArray, RecordArraySize, pCur->TCr);
#endif
		}
		else
		{
			/* 安全数据长度 不包含 2个CRCM长度 */
			AppDataLen = ShortFromCharLE(&pCur->Data[RSSP_DATA_SIZE_POS_WL]) -2*CRCM_LEN_WL;
			SrcAddr = ShortFromCharLE(&pCur->Data[RSSP_SRC_ADD_POS_WL]);		/* RSSP源地址 */
			DestAddr = ShortFromCharLE(&pCur->Data[RSSP_DEST_ADD_POS_WL]);		/* RSSP目标地址 */
			/* 写入队列 
			 * -------------------------------------------------
			 *  数据长度 | 对方Type | 对方ID| 应用数据 |
			 *   2字节   |  1字节   | 1 字节|   n      |
			 * -------------------------------------------------
			 */
			ret = dsuRsspRGetName_WL(SrcAddr, DestAddr, &destName, pRsspStru->pDsuRsspInfoStruWL);
			
			ShortToChar((UINT16)(AppDataLen+4),&pRsspStru->FrameData[LEN_OUT_QUE_POS]);	/* 数据长度 安全数据长度 + 4(源地址，目标地址) */
			ShortToChar(pCur->DestTypeId, &pRsspStru->FrameData[SRC_TYPE_OUT_QUE_POS]);	/*源类型ID*/
			ShortToChar(pCur->DestLogicId, &pRsspStru->FrameData[DST_LOGIC_ID_QUE_POS]);	/*源逻辑ID*/

			/* 拷贝应用数据 */
			RsspMemCopy_WL(&pCur->Data[RSSP_DATA_POS_WL],&pRsspStru->FrameData[DATA_OUT_QUE_POS], AppDataLen);

			/* 写入队列 */
			ret = QueueWrite(2+AppDataLen+4,pRsspStru->FrameData,RsToApp);
			if(1 == ret)
			{	/* 写入成功 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
				PrintFiles(RecordArray,RecordArraySize,"Out:\n");
				PrintFiles(RecordArray,RecordArraySize,"MsgLen：");	
				PrintFileU16Hex(RecordArray,RecordArraySize,AppDataLen);	/* 纯应用数据长度 */
				PrintFiles(RecordArray,RecordArraySize,"\tSrcAddr：");	
				PrintFileU16Hex(RecordArray,RecordArraySize,SrcAddr);		/* 源地址 */
				PrintFiles(RecordArray,RecordArraySize,"\tDestAddr：");	
				PrintFileU16Hex(RecordArray,RecordArraySize,DestAddr);		/* 目标地址 */
				PrintFiles(RecordArray,RecordArraySize,"\n");	
#endif 
			}
			else
			{
				/* 队列写入失败 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
				PrintFiles(RecordArray,RecordArraySize,"W:Queue Write Err.\n");	
#endif
#ifdef RSSP_RECORD_SIMPLIFY_BIN
				PrintRecArrayByte(RecordArray, RecordArraySize, RSD_APP_WRITE_ERR);
#endif
			}

			PrintRecArray2ByteB(RecordArray, RecordArraySize, SrcAddr);
			PrintRecArrayByte(RecordArray, RecordArraySize, pCur->Data[RSSP_MSG_TYPE_POS_WL]);
			PrintRecArray4ByteB(RecordArray, RecordArraySize, LongFromCharLE(&pCur->Data[RSSP_TC_POS_WL]));
			PrintRecArray2ByteB(RecordArray, RecordArraySize, AppDataLen);
		}

		/* 删除当前报文节点:pCur实际为表头，因而删除pCur后，表头即为pNext */
		ret = DelTmpMsgLnkNode_WL(pCur, NULL, &pCur, pRsspStru->StackOfDataLink);
		if(0 == ret )
		{
			/* 记录 Warning，实际不应该会发生这样的错误 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
	PrintFiles(RecordArray,RecordArraySize,"W:Delete Msg Lnk Error.");		
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN
	PrintRecArrayByte(RecordArray, RecordArraySize, RSD_DEL_DATA_NODE_ERR);	 /* 临时报文删除失败 */

#endif
		}
		else
		{
			/* 继续处理下一个节点 */
		}
	}

	ret = 1;
	return ret;
}

/***********************************************************************
 * 方法名   : RecvLnkStatusMng_WL
 * 功能描述 : 根据通信节点的信息进行状态变化。
 * 输入参数 : 
 *	参数名		类型				输入输出	描述
 *  --------------------------------------------------------------
 *	pBsdMsg		const DataNod_WLe*		IN			Bsd报文
 *  pLnkNode	Lnk_Info_Node_Stru_WL 	INOUT		RSSP到应用的输出队列
 * 

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static void RecvLnkStatusMng_WL(RSSP_INFO_STRU *pRsspStru)
{
	UINT8 index = 0;
	UINT8 LnkNodeNum = 0;	/* 节点数 */
	UINT8 RecvStatus = 0;	/* 状态 */
	UINT32 LastLocalRecTC = 0;		/* 上次收到报文本地周期号 */
	UINT32 NoRecDataTc = 0;
	UINT8 delLnkRtn = 0; /*删除链路结果*/

#if 0
	INT32 AntiDelayTC = 0;
#endif
	Lnk_Info_Node_Stru_WL * LnkNodeArr = NULL;
	UINT32 destDevName = 0;	/*对方设备唯一ID*/

	/* 取得全局通信节点数组 */
	GetLnkNodeArr_WL(&LnkNodeArr,&LnkNodeNum,pRsspStru);
	for (index = 0; index < LnkNodeNum; index++)
	{
		if (1 == LnkNodeArr[index].ExeCycle)	/*此链路是执行周期则执行*/
		{
			/* 接受状态为 等待SSR 或者 接受数据 */
			RecvStatus = LnkNodeArr[index].RsspLinInfo.RecvLinkStatus;
			if ((WaitSSR_WL == RecvStatus) || (RecvData_WL == RecvStatus))
			{
				LastLocalRecTC = LnkNodeArr[index].RsspLinInfo.TimeStmp.LastLocalRecTC;
				/* 本周内有没有收到数据判断 */
				if (LnkNodeArr[index].RsspLinInfo.TimeStmp.TC != LastLocalRecTC)
				{
					/* 没有收到数据周期数+1 */
					LnkNodeArr[index].RsspLinInfo.NoRecvDataCycleNum += 1;
					NoRecDataTc = LnkNodeArr[index].RsspLinInfo.NoRecvDataCycleNum;
					/* 等待SSR */
					if (WaitSSR_WL == RecvStatus)
					{
						/* 等待SSR超时 */
						if (LnkNodeArr[index].RsspLinInfo.LocalNode.SSDelay < NoRecDataTc)
						{
							/* 发送SSE次数超时 */
							if (LnkNodeArr[index].RsspLinInfo.TimeStmp.NumSendSse >= SEND_SSE_NUM_WL)
							{
								/* 状态设置为HALT 发送SSE次数清零 */
								LnkNodeArr[index].RsspLinInfo.RecvLinkStatus = Halt_WL;
								/*无数据周期数 清零*/
								LnkNodeArr[index].RsspLinInfo.NoRecvDataCycleNum = 0;
								LnkNodeArr[index].RsspLinInfo.TimeStmp.NumSendSse = 0;
							}
							else	/* 发送SSE次数不超时 继续发送SSE */
							{
								LnkNodeArr[index].RsspLinInfo.RecvLinkStatus = SendSSE_WL;
								LnkNodeArr[index].RsspLinInfo.TimeStmp.NumSendSse++;
							}
						}
						else
						{
							/* 什么不做 */
						}
					}
					else	/* 接受数据状态 */
					{
						/* 定期同步 超时检查 */
#if 0	/* 屏蔽定期同步功能 2010.1.14 */
						AntiDelayTC = CheckTimeStamp_WL(LnkNodeArr[index].RsspLinInfo.TimeStmp.TC, LnkNodeArr[index].RsspLinInfo.TimeStmp.SsrTc, LnkNodeArr[index].RsspLinInfo.LocalNode.AntiDelayCount);
						if (AntiDelayTC < 0)
						{
							/* 将接受状态设定为发送SSE */
							LnkNodeArr[index].RsspLinInfo.RecvLinkStatus = SendSSE_WL;
						}
						else
						{
							/* 什么不做 */
						}
#endif
						if (NoRecDataTc > LnkNodeArr[index].RsspLinInfo.LocalNode.DLost)
						{
							/* 将接受状态设定为HALT */
							LnkNodeArr[index].RsspLinInfo.RecvLinkStatus = Halt_WL;
							/*无数据周期数 清零*/
							LnkNodeArr[index].RsspLinInfo.NoRecvDataCycleNum = 0;
						}
						else
						{
							/* 什么不做 */
						}
					}
				}
				else	/* 本周期有对应接受数据 */
				{
					/* 什么不做 */
				}
			}
			else	/* 等待SSR 和 接受数据以外状态 */
			{
				/* 什么不做 */
			}
			/*链路可用时检查*/
			if (DisbaledReplace_WL == LnkNodeArr[index].NodeStatus)
			{
				/*发送状态为中断 且 接收状态为中断或准备发SSE*/
				if ((Halt_WL == LnkNodeArr[index].RsspLinInfo.SendLinkStatus)
					&& ((Halt_WL == LnkNodeArr[index].RsspLinInfo.RecvLinkStatus) || (SendSSE_WL == LnkNodeArr[index].RsspLinInfo.RecvLinkStatus)))
				{
					/*在保持状态,计数累加*/
					LnkNodeArr[index].KeepSendSseOrHaltCycleNum++;
					/*保持状态达到 最大无数据周期数时 清除链路*/
					if (LnkNodeArr[index].KeepSendSseOrHaltCycleNum > LnkNodeArr[index].RsspLinInfo.LocalNode.DLost)
					{
						/*对方设备标识*/
						destDevName = LnkNodeArr[index].DestDevName;
						/*删除安全层链路*/
						delLnkRtn = RsspLnkDelete_WL(destDevName, pRsspStru);
						if (0 == delLnkRtn)
						{
							PrintRecArrayByte(pRsspStru->RecordArray, pRsspStru->RecordArraySize, INPUT_LNK_STATUS_ERR);	/*记录日志*/
							PrintRecArray4ByteB(pRsspStru->RecordArray, pRsspStru->RecordArraySize, destDevName);
						}
						/*计数清零*/
						LnkNodeArr[index].KeepSendSseOrHaltCycleNum = 0;
					}
				}
				else
				{
					/*计数清零*/
					LnkNodeArr[index].KeepSendSseOrHaltCycleNum = 0;
				}
			}
		}
		else
		{
			/* 什么不做 */
		}
	}	
}

/***********************************************************************
 * 方法名   : CheckAddr_WL
 * 功能描述 : 检查报文信息与通信节点信息是否一致。
 * 输入参数 : 
 *	参数名		类型						输入输出	描述
 *  --------------------------------------------------------------
 *	pDataNode	DataNode_WL*					IN			报文节点
 *  pLnkNode	const Lnk_Info_Node_Stru_WL 	INOUT		RSSP到应用的输出队列
 * 

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 CheckAddr_WL(const DataNode_WL *pDataNode, const Lnk_Info_Node_Stru_WL *pLnkNode)
{
	UINT8 ret = 0;						/* 初始值：处理不成功 */

	/* 设备名称 == 通信节点信息.链路情报.源地址 */
	if( (pDataNode->DevName == pLnkNode->DestDevName) && (pDataNode->LocalLogId == pLnkNode->RsspLinInfo.LocalLogId)
		&& (pDataNode->DestLogId == pLnkNode->RsspLinInfo.DestLogId))
	/* wangpeipei 20111201 ADD*/
	{
		/* 检查通过 */
		ret = 1;
	}
	else
	{
		/* 检查失败，返回值 ret = 0 */
	}
	return ret;
}

/***********************************************************************
 * 方法名   : UptLnkNodeInfoBySSE_WL
 * 功能描述 : 根据SSE报文信息更新通信节点信息。
 * 输入参数 : 
 *	参数名		类型				输入输出	描述
 *  --------------------------------------------------------------
 *	pDataNode	const DataNode_WL*		IN			报文节点
 *  pLnkNode	Lnk_Info_Node_Stru_WL 	INOUT		RSSP到应用的输出队列
 * 

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 UptLnkNodeInfoBySSE_WL(const DataNode_WL *pDataNode, Lnk_Info_Node_Stru_WL *pLnkNode)
{
	const UINT8 *Msg = NULL;	/* RSSP报文指针 */
	RsspLinkInfoStru_WL *pRsspLnkInfo = NULL;

	Msg = pDataNode->Data;
	pRsspLnkInfo = &pLnkNode->RsspLinInfo;

	/* 链路发送节点的状态设置为 发送SSR */
	pRsspLnkInfo->SendLinkStatus = SendSSR_WL;

	/* 设定收到SSE报文里的时钟 */ 
	pRsspLnkInfo->TimeStmp.RecvSseTCe = 
		LongFromCharLE(&Msg[RSSP_TC_POS_WL]);

	/* SEQENQ_1取得 设定 */
	pRsspLnkInfo->TimeStmp.SeqEng[0] = LongFromCharLE(&Msg[RSSP_SSE_ENQ_1_POS_WL]);

	/* SEQENQ_2取得 设定 */
	pRsspLnkInfo->TimeStmp.SeqEng[1] = LongFromCharLE(&Msg[RSSP_SSE_ENQ_2_POS_WL]);
			   	
	return (UINT8)1;
}

/***********************************************************************
 * 方法名   : UptLnkNodeInfoBySSR_WL
 * 功能描述 : 根据SSR报文信息更新通信节点信息。
 * 输入参数 : 
 *	参数名		类型				输入输出	描述
 *  --------------------------------------------------------------
 *	pDataNode	const DataNode_WL*		IN			报文节点
 *  pLnkNode	Lnk_Info_Node_Stru_WL 	INOUT		RSSP到应用的输出队列
 * 

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 UptLnkNodeInfoBySSR_WL(const DataNode_WL *pDataNode, Lnk_Info_Node_Stru_WL *pLnkNode, UINT8 *RecordArray, UINT16 RecordArraySize)
{
	UINT8 ret = 1;				/* 初始值：正常返回 */
	UINT32 SseTCe = 0;			/* SSR报文里面的 请求SSE的TC */
	UINT32 SeqIni[2] = {0};		/* SSR报文的SEQ_INI 值 */
	const UINT8 *Msg = NULL;	/* RSSP报文指针 */
	RsspLinkInfoStru_WL *pRsspLnkInfo = NULL;	/* 链路基本情报 */
	TimeStampStru_WL *pTmStmp = NULL;			/* 时间结构体指针 */

	Msg = pDataNode->Data;
	pRsspLnkInfo = &pLnkNode->RsspLinInfo;
	pTmStmp = &pRsspLnkInfo->TimeStmp;

	/* 通信链路的接受状态为 等待SSR */
	if(WaitSSR_WL == pRsspLnkInfo->RecvLinkStatus )
	{
		SseTCe = LongFromCharLE(&Msg[RSSP_SSR_TCENQ_POS_WL]);		/* SSR报文的 SSE请求计数器号 */
		
		/* 判断是否为对应 SSR */
		if( pTmStmp->SseTC == SseTCe)
		{
			/* 如果是对应的SSR报文，则更新链路基本情报信息 */

			pTmStmp->SsrTc = pLnkNode->RsspLinInfo.TimeStmp.TC;				/* 上次同步周期号更新 ： 当前周期号 */

			SeqIni[0] = LongFromCharLE(&Msg[RSSP_SSR_INI_1_POS_WL]);	/* 信号道 SEQINI_1 值 */
			SeqIni[1] = LongFromCharLE(&Msg[RSSP_SSR_INI_2_POS_WL]);	/* 信号道 SEQINI_2 值 */

			/* 计算初始SINITr AND [SIDr ^ Tr(n)]保存到pTmStmp->NowSINIT_ADD_SID_XOR_Tn里面 */
			ret = CalcFirstSinitAndSidXorTn_WL(pRsspLnkInfo->LocalNode.PREC_FIRSTSINIT,SeqIni,
				pTmStmp->SseTn,pTmStmp->NowSINIT_ADD_SID_XOR_Tn);

			pTmStmp->NumSendSse = 0;							/* 发送SSE 次数设定为 0 */
			pTmStmp->LastLocalRecTC = pLnkNode->RsspLinInfo.TimeStmp.TC;	/* 上次收到报文的本地时钟 */
			pTmStmp->LastRecvTCr = pTmStmp->NowRecvTCr;			/* 上次接受报文时钟设为 当前收到报文时钟 预留 */
			pTmStmp->NowRecvTCr = LongFromCharLE(&Msg[RSSP_TC_POS_WL]);	/* 当前收到报文时钟 = 报文计数器 */

			pRsspLnkInfo->RecvLinkStatus = RecvData_WL;			/* 链路接受状态 = 接受数据 */
			pRsspLnkInfo->NoRecvDataCycleNum = 0;				/* 无数据周期数	*/
		}
		else	/* 不是对应的SSR 报文 */
		{
			/* 错误返回 */
			ret	= 0;
			PrintRecArrayByte(RecordArray, RecordArraySize, SSR_SSE_SN_ERR);
			PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->SseTC);
		}
		
		PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&Msg[RSSP_SRC_ADD_POS_WL]));
		PrintRecArrayByte(RecordArray, RecordArraySize, Msg[RSSP_MSG_TYPE_POS_WL]);
		PrintRecArray4ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&Msg[RSSP_TC_POS_WL]));
		PrintRecArray4ByteB(RecordArray, RecordArraySize, SseTCe);
	}
	else
	{
		/* 错误返回 0 */
		ret = 0;

		PrintRecArrayByte(RecordArray, RecordArraySize, SSR_RCV_STATUS_ERR);
		PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&Msg[RSSP_SRC_ADD_POS_WL]));
	}
	
	
	return ret;
}

/***********************************************************************
 * 方法名   : CheckRsdMsg_WL
 * 功能描述 : 验证RSD报文的正确性。
 * 输入参数 : 
 *	参数名		类型				输入输出	描述
 *  --------------------------------------------------------------
 *	pBsdMsg		const UINT8 *		IN			Bsd报文
 *  pLnkNode	Lnk_Info_Node_Stru_WL 	INOUT		RSSP到应用的输出队列
 * 

 * 返回值   : UINT8  1：正常返回  
 ***********************************************************************/
static UINT8 CheckRsdMsg_WL(const UINT8 *pBsdMsg,Lnk_Info_Node_Stru_WL *pLnkNode, RSSP_INFO_STRU *pRsspStru)
{
	UINT8 ret = 1;					/* 初始化为正常返回 */
	UINT8 tmpret = 0;
	INT32 DelayTCr = 0;				/* 丢包数 */
	INT32 Tout = 0;					/* 距离上次收到报文的时间差 */
	UINT32 TCr = 0;					/* 报文时钟 */
	UINT32 SidXorTn[2] = {0};		/* SID XOR TN ：BSD报文计算出来 */
	TimeStampStru_WL *pTmStmp = &pLnkNode->RsspLinInfo.TimeStmp;				/* 时间戳结构体 */
	const NodeParamInfoStru_WL *pNodeParam = &pLnkNode->RsspLinInfo.LocalNode;	/* 节点基本信息结构体 */
	UINT8 *RecordArray = pRsspStru->RecordArray;
	UINT16 RecordArraySize = pRsspStru->RecordArraySize;
  
	/* 通信节点接受状态为 接收数据状态 */
	if( RecvData_WL == pLnkNode->RsspLinInfo.RecvLinkStatus)
	{
		/* 报文时钟 */
		TCr = LongFromCharLE(&pBsdMsg[RSSP_TC_POS_WL]);
		/* 
		 * 如果和上次报文的时钟一致(即同一周期内同时受到BSD报文和SSR报文)，则丢弃该报文 
		 * 否则 检查丢包数是否超时
		 */
		if( TCr != pTmStmp->NowRecvTCr)	
		{
			DelayTCr = CheckTimeStamp_WL(TCr,pTmStmp->NowRecvTCr,pNodeParam->MaxLost);
		}
		else
		{
			PrintRecArrayByte(RecordArray, RecordArraySize, INPUT_TC_EQ);
			PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pBsdMsg[RSSP_SRC_ADD_POS_WL]));
			PrintRecArray4ByteB(RecordArray, RecordArraySize, TCr);
			return 0;
		}
		
		if(-1 == DelayTCr)
		{/*序列号超限，触发SSE*/
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
		PrintFiles(RecordArray,RecordArraySize,"DelayTCr > MaxLost\n");
#endif
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
		PrintRecArrayByte(RecordArray, RecordArraySize, OVER_MAX_LOST);
		PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pBsdMsg[RSSP_SRC_ADD_POS_WL]));
		PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->NowRecvTCr);
		PrintRecArray4ByteB(RecordArray, RecordArraySize, TCr);
#endif
			/* 丢包数超过 最大的丢包数 重发SSE进行同步 */
			pLnkNode->RsspLinInfo.RecvLinkStatus = SendSSE_WL;
			ret = 0;
		}	
		else if(-2 == DelayTCr)
		{/*旧数据，丢弃*/
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
			PrintRecArrayByte(RecordArray, RecordArraySize, OLD_DATA);
			PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pBsdMsg[RSSP_SRC_ADD_POS_WL]));
			PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->NowRecvTCr);
			PrintRecArray4ByteB(RecordArray, RecordArraySize, TCr);
#endif
			ret = 0;
		}
		else
		{
			/* 报文超时 检查 */
			if (pTmStmp->LastLocalRecTC != pTmStmp->TC)
			{
				Tout = CheckTimeStamp_WL(pTmStmp->TC,
					pTmStmp->LastLocalRecTC, pNodeParam->Tout);
			}
			else
			{
				/* 说明本周期已经收到一帧旧数据，不需要在做超时检查 */
				Tout = 0;
			}
			if (Tout >= 0)		/* 合法时间内收到报文 */
			{
				/* 从RSD报文里取得SID XOR TN ：CRCM[chan] ^ CRC[chan] ^ SYS_CHK_WD[chan] */
				tmpret = GetSidXorTn_WL(pBsdMsg, SidXorTn);
				/* 检验CRCM的有效性 */
				ret = CheckCRCM_WL(SidXorTn, pTmStmp->NowSINIT_ADD_SID_XOR_Tn,
					(UINT32)DelayTCr, pNodeParam->TSD_POST_RXDADA_K);

				if ((1U == ret) && (1U == tmpret))	/* 检查成功 则更新相应信息 */
				{
					/* 更新时间戳结构体 : pTmStmpStru非空，返回值肯定为1 */
					ret = UpdtTmStmpStru_WL(SidXorTn, TCr, DelayTCr, pNodeParam->PREC_SINIT_K, pTmStmp);
					ret = 1; /* 正常处理 */
			}
				else
				{
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
					PrintFiles(RecordArray, RecordArraySize, "W:CRCM Error.\n");
#endif
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
					PrintRecArrayByte(RecordArray, RecordArraySize, SVC_ERR);
					PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pBsdMsg[RSSP_SRC_ADD_POS_WL]));
					PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->NowRecvTCr);
					PrintRecArray4ByteB(RecordArray, RecordArraySize, TCr);
					PrintRecArray4ByteB(RecordArray, RecordArraySize, SidXorTn[0]);
					PrintRecArray4ByteB(RecordArray, RecordArraySize, SidXorTn[1]);
					PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->NowSINIT_ADD_SID_XOR_Tn[0]);
					PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->NowSINIT_ADD_SID_XOR_Tn[1]);
#endif
					/* 检查失败 错误返回 */
					ret = 0;
				}
		}
			else	/* 超时 */
			{
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
				PrintFiles(RecordArray, RecordArraySize, "W:Time Out.\n");
#endif
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
				PrintRecArrayByte(RecordArray, RecordArraySize, OVER_MAX_TIME);
				PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pBsdMsg[RSSP_SRC_ADD_POS_WL]));
				PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->LastLocalRecTC);
				PrintRecArray4ByteB(RecordArray, RecordArraySize, pTmStmp->TC);
#endif
				ret = 0;
			}
	}
	}
	else if( Halt_WL == pLnkNode->RsspLinInfo.RecvLinkStatus)
	{
		pLnkNode->RsspLinInfo.RecvLinkStatus = SendSSE_WL;	/* 状态设置为发送SSE */
		ret = 0;

		PrintRecArrayByte(RecordArray, RecordArraySize, RSD_HALT_SSE);
		PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pBsdMsg[RSSP_SRC_ADD_POS_WL]));
	}
	else	/* 其他状态丢弃数据 */
	{
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
		PrintFiles(RecordArray,RecordArraySize,"W:RecvStatus() not RecvData or Halt.Discard Bsd Msg.");
		PrintFileHex(RecordArray,RecordArraySize,&pLnkNode->RsspLinInfo.RecvLinkStatus,1);
		PrintFiles(RecordArray,RecordArraySize,"\n");
#endif
		ret =0;

		PrintRecArrayByte(RecordArray, RecordArraySize, RSD_RCV_HALT_ERR);
		PrintRecArray2ByteB(RecordArray, RecordArraySize, ShortFromCharLE(&pBsdMsg[RSSP_SRC_ADD_POS_WL]));
	}

	return ret;
}

/*
*函数名:	ReadNetData
*功能描述:	读取红或蓝网队列数据,从小到大排序并过滤掉重复序号数据,到输入数据堆栈链表中
*输入参数:	QueueStruct *pInputQue	单网输入数据队列
*			RSSP_INFO_STRU* rssp RSSPI结构
*输入出参:	DataNode_WL **pHeadNode 头节点
*输出参数:	无
*返回值:	无
*/
static void ReadNetData(QueueStruct* pInputQue, DataNode_WL** pHeadNode, RSSP_INFO_STRU* rssp, const UINT8 netIdx)
{
	const UINT8 oneLen = 1u;	/*长度值1*/
	const UINT8 twoLen = 2u;	/*长度值2*/
	UINT32 queLen = 0u;	/*队列长度*/
	UINT16 pkgDataLen = 0u;	/*单包数据长度*/
	UINT8 idx = 0u;	/*数据下标*/
	UINT8 crcValid = 0u;	/*CRC16是否有效*/
	UINT16 dstLogicId = 0u;	/*对方逻辑ID*/
	UINT8 frameType = 0u;	/*帧类型*/
	UINT16 srcAddr = 0u;	/*源地址*/
	UINT16 dstAddr = 0u;	/*目的地址*/
	UINT32 sn = 0u;	/*序号*/
	DataNode_WL* pLastNode = NULL;	/*上一节点*/
	DataNode_WL* pCurNode = NULL;	/*当前节点*/
	DataNode_WL* pNewNode = NULL;	/*新节点*/
	UINT8 isCover = 0u;	/*是否覆盖节点*/
	UINT8 isPop = 0u;	/*是否能弹出节点*/
	UINT8 isAddFull = 0u; /*堆栈满时是否添加*/
	UINT8 isInBreak = 0u; /*是否退出内层循环*/

	if ((NULL != pInputQue) && (NULL != pHeadNode) && (NULL != rssp) && (NULL != rssp->StackOfDataLink) && (NULL != rssp->pDsuRsspInfoStruWL) && (NULL != rssp->FrameData) && (NULL != rssp->RecordArray)) /*非空检查*/
	{
		queLen = QueueStatus(pInputQue);	/*队列中数据长度*/
		PrintRecArray4ByteB(rssp->RecordArray, rssp->RecordArraySize, queLen);	/*记录冗余层数据长度*/
		/*输入队列数据中有数据,进行处理*/
		while (queLen > twoLen)
		{
			QueueScan(twoLen, rssp->FrameData, pInputQue); /*扫长度字节到数组中*/
			pkgDataLen = ShortFromChar(rssp->FrameData); /*表示的此单包数据长度*/
			if ((twoLen + pkgDataLen) <= queLen) /*表示的此单包数据长度正常*/
			{
				if ((twoLen + RSSP_HEAD_LEN_WL + TC_LEN_WL + CRC_16_LEN_WL) < pkgDataLen) /*表示帧长度正常*/
				{
					if ((twoLen + pkgDataLen) <= rssp->FrameDataMaxLen)	/*单包数据长度可写入堆栈单包数据缓存中*/
					{
						QueueRead(twoLen + pkgDataLen, rssp->FrameData, pInputQue); /*读出整个单包数据,2字节长度+RSSPI帧格式*/

						idx = twoLen; /*跳过2字节长度*/
						dstLogicId = ShortFromChar(&rssp->FrameData[idx]);
						idx += twoLen; /*2字节逻辑ID*/
						idx += oneLen; /*跳过1字节域*/
						frameType = rssp->FrameData[idx]; /*1字节帧类型*/
						idx += oneLen;
						srcAddr = ShortFromCharLE(&rssp->FrameData[idx]); /*源地址*/
						idx += twoLen;
						dstAddr = ShortFromCharLE(&rssp->FrameData[idx]); /*目的地址*/
						idx += twoLen;
						sn = LongFromCharLE(&rssp->FrameData[idx]); /*序号*/

						SetNetsRecvPkgNum(rssp, srcAddr, netIdx);

						PrintRecArray2ByteB(rssp->RecordArray, rssp->RecordArraySize, srcAddr); /*记录源地址*/
						PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, frameType); /*记录帧类型*/
						PrintRecArray4ByteB(rssp->RecordArray, rssp->RecordArraySize, sn); /*记录序号*/

						if (dstAddr == rssp->pDsuRsspInfoStruWL->LocRsspiAddr)	/*是发给本方的数据*/
						{
							crcValid = IsRedunRcvMsg(pkgDataLen - twoLen, &rssp->FrameData[twoLen + twoLen]); /*检查单包数据报文*/
							if (1u == crcValid) /*CRC有效*/
							{
								pLastNode = NULL; /*清空上一节点*/
								pCurNode = (*pHeadNode); /*当前节点从头节点开始*/
								isCover = 0u; /*不覆盖节点*/
								isPop = 0u; /*未弹出节点*/
								isAddFull = 0u; /*不添加*/

								isInBreak = 0u; /*默认值*/
								while (NULL != pCurNode) /*遍历*/
								{
									if ((srcAddr == pCurNode->SrcAddr) && (sn == pCurNode->TCr) && (frameType == pCurNode->FrameType)) /*相同值节点,覆盖节点*/
									{
										isCover = 1u; /*覆盖*/
										isInBreak = 1u; /*是退出循环*/
									}
									else /*按源地址,序号,帧类型 从小到大排序*/
									{
										if ((srcAddr < pCurNode->SrcAddr)	/*比较源地址*/
											|| ((srcAddr == pCurNode->SrcAddr) && (sn < pCurNode->TCr))	/*再比较序号*/
											|| ((srcAddr == pCurNode->SrcAddr) && (sn == pCurNode->TCr) && (frameType < pCurNode->FrameType)) /*再比较帧类型*/
											)
										{
											isInBreak = 1u; /*是退出循环*/
										}
									}

									if (0 == isInBreak) /*继续遍历*/
									{
										pLastNode = pCurNode;
										pCurNode = pCurNode->pNextNode;

									}
									else /*退出遍历*/
									{
										break;
									}
								}

								if (1u == isCover)	/*覆盖节点*/
								{
									pNewNode = pCurNode;
								}
								else /*插入节点*/
								{
									isPop = StackPopWl(&rssp->StackOfDataLink->stack, (STACK_DATA_TYPE_WL*)&pNewNode);
									if (1u == isPop)
									{
										if (NULL != pCurNode)	/*新节点插入当前节点前*/
										{
											pNewNode->pNextNode = pCurNode;
										}
										if (NULL == pLastNode)	/*新节点作为头节点*/
										{
											*pHeadNode = pNewNode;
										}
										else /*上一节点指向新节点*/
										{
											pLastNode->pNextNode = pNewNode;
										}
									}
									else /*堆栈已满*/
									{
										if ((NULL != (*pHeadNode)) && (NULL != pLastNode)) /*链表有多于一个节点数据,用之前头节点做为新节点添加*/
										{
											pNewNode = (*pHeadNode);
											(*pHeadNode) = (*pHeadNode)->pNextNode;
											pLastNode->pNextNode = pNewNode;
											pNewNode->pNextNode = pCurNode;
											isAddFull = 1u;
										}

										PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RP_INPUT_STACK_NO);	/*记录日志-堆栈无空间*/
										PrintRecArray4ByteB(rssp->RecordArray, rssp->RecordArraySize, queLen);	/*数据长度*/
									}
								}
								if ((1u == isCover) || (1u == isPop) || (1u == isAddFull))	/*未退出循环,设置值*/
								{
									pNewNode->FrameType = frameType; /*帧类型*/
									pNewNode->SrcAddr = srcAddr; /*RSSPI的源地址*/
									dsuRsspRGetName_WL(srcAddr, dstAddr, &pNewNode->DevName, rssp->pDsuRsspInfoStruWL); /*对方唯一ID*/
#ifdef RSSP_NO_HLHT
									pNewNode->DestLogId = pNewNode->DevName & 0xff;
#else
									pNewNode->DestLogId = DquToDevId(pNewNode->DevName, rssp->pDsuRsspInfoStruWL); /*对方设备ID*/
#endif
									pNewNode->LocalLogId = rssp->LocalID; /*本方设备ID*/
									pNewNode->DestLogicId = dstLogicId; /*对方逻辑ID*/
									pNewNode->DataLen = pkgDataLen - twoLen - twoLen; /*去掉最后2字节CRC长度,pkgDataLen里包含2字节逻辑ID*/
									pNewNode->TCr = sn; /*序号*/
									RsspMemCopy_WL(&rssp->FrameData[twoLen + twoLen], pNewNode->Data, pkgDataLen - twoLen); /*复制数据*/
								}
							}
							else  /*CRC无效*/
							{
								PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RP_INPUT_PKG_INVALID);	/*记录标志*/
							}
						}
						else /*不是发给本方的数据*/
						{
							PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RP_INPUT_PKG_ADDR_ERR);	/*记录标志*/
						}
					}
					else /*单包数据长度不可写入堆栈单包数据缓存中*/
					{
						QueueElementDiscard(twoLen + pkgDataLen, pInputQue); /*从队列中清除此单包数据*/
						PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RP_OVER_MAX_INPUT_PKG_LEN); /*记录标志*/
					}
				}
				else /*表示帧长度错误*/
				{
					QueueElementDiscard(queLen, pInputQue); /*清空队列*/
					PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RP_INPUT_PKG_LEN_ERR); /*记录标志*/
				}
			}
			else /*表示的此单包数据长度错误*/
			{
				QueueElementDiscard(queLen, pInputQue); /*清空队列*/
				PrintRecArrayByte(rssp->RecordArray, rssp->RecordArraySize, RP_INPUT_LEN_ERR); /*记录标志*/
			}
			queLen = QueueStatus(pInputQue); /*队列中剩余数据长度*/

		}
	}
}

/*
*功能描述:	清理各网收包数
*输入参数:	struc_Unify_Info* sfp sfp变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
static void ClearNetsRecvPkgNum(RSSP_INFO_STRU* rsspi)
{
	UINT16 i = 0u; /*用于循环*/

	if ((NULL != rsspi) && (NULL != rsspi->NetsRecvPkgNums)) /*参数检查*/
	{
		for (i = 0; i < rsspi->NetsRecvPkgNumsLen; i++)
		{
			RsspMemSet_WL((UINT8*)rsspi->NetsRecvPkgNums[i].NetsPkgNum, 2 * sizeof(UINT16), 0u);
		}
	}
}

/*
*功能描述:	设置各网收包数
*输入参数:	RSSP_INFO_STRU* rsspi rsspi变量
*           const UINT16 dstAddr 目的地址
*           const UINT8 netIdx 网下标
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
static void SetNetsRecvPkgNum(RSSP_INFO_STRU* rsspi, const UINT16 dstAddr, const UINT8 netIdx)
{
	UINT16 i = 0u; /*用于循环*/

	if ((NULL != rsspi) && (NULL != rsspi->NetsRecvPkgNums) && ((0u == netIdx) || (1u == netIdx))) /*参数检查*/
	{
		for (i = 0; i < rsspi->NetsRecvPkgNumsLen; i++)
		{
			if (dstAddr == rsspi->NetsRecvPkgNums[i].DstAddr) /*找到*/
			{
				rsspi->NetsRecvPkgNums[i].NetsPkgNum[netIdx]++;
				break;
			}
		}
		if ((i == rsspi->NetsRecvPkgNumsLen) && (rsspi->NetsRecvPkgNumsLen < rsspi->MaxNumLink)) /*未找到,添加一设备*/
		{
			rsspi->NetsRecvPkgNums[rsspi->NetsRecvPkgNumsLen].DstAddr = dstAddr;
			RsspMemSet_WL((UINT8*)rsspi->NetsRecvPkgNums[i].NetsPkgNum, 2 * sizeof(UINT16), 0u);
			rsspi->NetsRecvPkgNums[rsspi->NetsRecvPkgNumsLen].NetsPkgNum[netIdx] = 1;
			rsspi->NetsRecvPkgNumsLen++;
		}
	}
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
void GetNetsRecvPkgNum_WL(RSSP_INFO_STRU* rsspi, const UINT16 dstAddr, UINT16* redPkgNum, UINT16* bluePkgNum)
{
	UINT16 i = 0u; /*用于循环*/

	if ((NULL != rsspi) && (NULL != rsspi->NetsRecvPkgNums) && (NULL != redPkgNum) && (NULL != bluePkgNum)) /*参数检查*/
	{
		for (i = 0; i < rsspi->NetsRecvPkgNumsLen; i++)
		{
			if (dstAddr == rsspi->NetsRecvPkgNums[i].DstAddr)
			{
				(*redPkgNum) = rsspi->NetsRecvPkgNums[i].NetsPkgNum[0];
				(*bluePkgNum) = rsspi->NetsRecvPkgNums[i].NetsPkgNum[1];
				break;
			}
		}
	}
}

#ifdef __cplusplus
}
#endif



