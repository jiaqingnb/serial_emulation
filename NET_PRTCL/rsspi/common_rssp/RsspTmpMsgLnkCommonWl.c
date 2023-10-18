/********************************************************
*
* 文件名  ： RsspTmpMsgLnkCommonWl.c
* 版权说明： 北京瑞安时代科技有限责任公司
* 版本号  ： 1.0
* 创建时间： 2009.11.25
* 作者    ： 车载及协议部
* 功能描述： 临时报文链表操作处理
* 使用注意：
*
* 修改记录：
*   更新时间	更新者	更新理由
* ------------  ------  ---------------
*	2009.12.07	楼宇伟	初版作成
*	2009.12.15	楼宇伟	单元测试Bug对应
*   2013.02.22  王佩佩  修改GetSortedMsgFromQue函数。删除角色检查
********************************************************/
#include <stdlib.h>
#include "CommonMemory.h"
#include "RsspCommonWl.h"
#include "RsspTmpMsgLnkCommonWl.h"
#include "RsspRecordDelareWl.h"
#include "CommonRecord.h"

#ifdef __cplusplus
extern "C" {
#endif


	/*--------------------------------------------------------------------
	 * RSSP层内部接口函数申明 Start
	 *--------------------------------------------------------------------*/
	 /***********************************************************************
	  * 方法名   : GetSortedMsgFromQue_WL
	  * 功能描述 : 对红蓝网输入数据进行RSR层的安全处理，将安全数据交付应用。
	  * 输入参数 :
	  *	参数名			类型			输入输出	描述
	  *  --------------------------------------------------------------
	  *	pMsgQue			QueueStruct*	IN			信号层到Rssp层的输入队列
	  *	MaxLen			UINT16			IN			最大数据
	  *	RecordArray		UINT8*			INOUT		记录数组
	  *  RecordArraySize	UINT16
	  *
	  * 返回值   : DataNode_WL*  临时报文链表表头
	  ***********************************************************************/
	DataNode_WL* GetSortedMsgFromQue_WL(QueueStruct *RpToRs, UINT16 MaxLen, UINT8 *RecordArray, UINT16 RecordSize, UINT8 *pTmpDataBuff, RsspStackElement_WL *pStackOfDataLink)
	{
		UINT8 ret = 0;
		UINT16 MsgLen = 0;			/* 消息长度 */
		DataNode_WL *pHead = NULL;		/* 表头 */
		DataNode_WL *pCur = NULL;		/* 当前作成的节点 */
		const UINT8 QUE_FIXED_LEN = 8;								/* 队列节点固定部分长度 */
		const UINT8 SENDER_TYPE_QUE_POS = 2;						/* 信号层队列保存对方唯一ID的位置 */
		const UINT8 SENDER_LOG_ID_QUE_POS = SENDER_TYPE_QUE_POS + 4;	/* 信号层队列保存发送方动态Id的位置 */
		const UINT8 RECV_LOG_ID_QUE_POS = SENDER_LOG_ID_QUE_POS + 1;	/* 信号层队列保存本方动态Id的位置 */
		const UINT8 LOGIC_ID_QUE_POS = RECV_LOG_ID_QUE_POS + 1;			/* 信号层队列保存本方动态Id的位置 */
		const UINT8 RSSP_DATA_QUE_POS = LOGIC_ID_QUE_POS + 2;		/* 信号层队列保存应用数据位置 */

		if ((NULL != RpToRs)&& (NULL != RecordArray) && (NULL != pTmpDataBuff) && (NULL != pStackOfDataLink))
		{
			/* 队列长度>2 */
			while (2 <= QueueStatus(RpToRs))
			{
				/*   信号层→RSSP层队列节点格式
				 * ----------------------------------------------------------------------
				 * 长度 | 发送方Type | 发送方ID | 发送方动态Id | 本方动态Id | RSSP消息体 |
				 * 2字节|   1字节    | 1 字节   |     1 字节   |    1字节   |    n       |
				 * -----------------------------------------------------------------------
				 */
				QueueScan(2, pTmpDataBuff, RpToRs);	/* 扫描两个字节的长度 */
				MsgLen = ShortFromChar(pTmpDataBuff);
				if ((MsgLen + 2) <= MaxLen)
				{
					ret = QueueRead(MsgLen + 2, (QueueElement*)pTmpDataBuff, RpToRs);
					if (ret == 0)
					{
						/* 读队列失败则队列剩下的数据大小小于MsgLen+2，则记录 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
						PrintFiles(RecordArray, RecordSize, "W:Write Que Err\n");	/* 读队列失败 */
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
						PrintRecArrayByte(RecordArray, RecordSize, INPUT_READ_ERR);	/* 读队列失败 */
#endif
						break;
					}

					/* RSSP报文 作成临时报文节点插入链表 */
					ret = IsRedunRcvMsg((UINT16)(MsgLen - QUE_FIXED_LEN), &pTmpDataBuff[QUE_FIXED_LEN + 2]);
					/* 是否为RSSP报文，然后判断发送类型是否合法 */
					if (1 == ret)
					{
						/* 从堆栈里弹出一个临时报文节点，将其地址保存到pCur */
						ret = StackPopWl(&pStackOfDataLink->stack, (STACK_DATA_TYPE_WL*)&pCur);
						if (1 == ret)
						{
							/* 读取成功 */
							/*对方唯一ID*/
							pCur->DevName = LongFromChar(&pTmpDataBuff[SENDER_TYPE_QUE_POS]);

							/* 发送方动态ID 设定 TmpDataBuff[4] */
							pCur->DestLogId = pTmpDataBuff[SENDER_LOG_ID_QUE_POS];

							/* 接收方动态ID TmpDataBuff[5]*/
							pCur->LocalLogId = pTmpDataBuff[RECV_LOG_ID_QUE_POS];

							/*对方逻辑ID*/
							pCur->DestLogicId = ShortFromChar(&pTmpDataBuff[LOGIC_ID_QUE_POS]);

							/* RSSP消息设定 （除去2位CRC）*/
							pCur->DataLen = MsgLen - QUE_FIXED_LEN - 2;
							RsspMemCopy_WL(&pTmpDataBuff[RSSP_DATA_QUE_POS], pCur->Data, pCur->DataLen);

							/* 报文计数器设定 */      /* 测试Bug 874 对应 */
							pCur->TCr = LongFromCharLE(&pCur->Data[RSSP_TC_POS_WL]);

							/* 插入临时报文链表 */
							pHead = InsTmpMsgLnk_WL(pHead, pCur);
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
							PrintFiles(RecordArray, RecordSize, "IN:\n");
							PrintFiles(RecordArray, RecordSize, "DestType:");					/* 本帧发送方类型 */
							PrintFileHex(RecordArray, RecordSize, &pTmpDataBuff[SENDER_TYPE_QUE_POS], 1);
							PrintFiles(RecordArray, RecordSize, "\tDestId:");					/* 本帧发送方ID */
							PrintFileHex(RecordArray, RecordSize, &pTmpDataBuff[SENDER_ID_QUE_POS], 1);
							PrintFiles(RecordArray, RecordSize, "\tDestLogId:");				/* 本帧发送方动态ID */
							PrintFileHex(RecordArray, RecordSize, &pTmpDataBuff[SENDER_LOG_ID_QUE_POS], 1);
							PrintFiles(RecordArray, RecordSize, "\tLocLogId:");				/* 本帧接受方动态ID */
							PrintFileHex(RecordArray, RecordSize, &pTmpDataBuff[RECV_LOG_ID_QUE_POS], 1);
							PrintFiles(RecordArray, RecordSize, "\tMsgType:");				/* 本帧消息类型 */
							PrintFileHex(RecordArray, RecordSize, &pCur->Data[RSSP_MSG_TYPE_POS_WL], 1);
							PrintFiles(RecordArray, RecordSize, "\tSrcAddr:");				/* 本帧消息源地址 */
							PrintFileU16Hex(RecordArray, RecordSize, ShortFromCharLE(&pCur->Data[RSSP_SRC_ADD_POS_WL]));
							PrintFiles(RecordArray, RecordSize, "\tDestAddr:");				/* 本帧消息目的地址 */
							PrintFileU16Hex(RecordArray, RecordSize, ShortFromCharLE(&pCur->Data[RSSP_DEST_ADD_POS_WL]));
							PrintFiles(RecordArray, RecordSize, "\tTC:");						/* 本帧消息计数器 */
							PrintFileU32Hex(RecordArray, RecordSize, pCur->TCr);
							PrintFiles(RecordArray, RecordSize, "\n");
#endif 
						}
						else
						{
							/* 读取失败，堆栈空闲节点不足 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
							PrintFiles(RecordArray, RecordSize, "W:Link POP Err.");	/* 弹栈出错 */
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
							PrintRecArrayByte(RecordArray, RecordSize, INPUT_STACK_NO);	/* 弹栈出错 */
#endif
							break;
						}
					}
					else /* 非RSSP消息 */
					{
						/* 记录：消息类型不对 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
						PrintFiles(RecordArray, RecordSize, "W:Not Rssp MSg.\n");	/* 非RSSP消息 */
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
						PrintRecArrayByte(RecordArray, RecordSize, INPUT_PKG_INVALID);	/* 非RSSP消息 */
#endif
					}
				}
				else
				{
					/* 最大长度超过 最大数据量 丢弃数据 */
					QueueElementDiscard(MsgLen, RpToRs);
					/* 处理下一帧数据 */
#ifdef RSSP_RECORD_ERR_ASCII	/* 字符串格式 */
					PrintFiles(RecordArray, RecordSize, "W:Queue Err.\n");	/* 队列不合法 */
#endif 
#ifdef RSSP_RECORD_SIMPLIFY_BIN	/* 二进制 */
					PrintRecArrayByte(RecordArray, RecordSize, INPUT_LEN_ERR);	/* 队列不合法 */
#endif		
				}
			}
		}
		return  pHead;
	}

	/***********************************************************************
	 * 方法名   : InsTmpMsgLnk_WL
	 * 功能描述 : 插入链表一个节点。标识符，序列号顺序排序。
	 * 输入参数 :
	 *	参数名		类型				输入输出		描述
	 *  --------------------------------------------------------------
	 *	pHead		DataNode_WL*		IN			表头
	 *	pDataNode	DataNode_WL*		IN			插入的节点
	 *
	 * 返回值   : DataNode*  临时报文链表表头
	 ***********************************************************************/
	DataNode_WL* InsTmpMsgLnk_WL(DataNode_WL *pHead, DataNode_WL *pNode)
	{
		DataNode_WL *pLast = NULL, *pCur = NULL;

		pCur = pHead;

		/* 插入节点指针为空，直接返回 表头 */   /* 测试Bug 847 对应 */
		if (NULL == pNode)
		{
			return pHead;
		}

		while (pCur != NULL)
		{
			/* 当前节点的标识符大于插入节点的 或者标识符相等 并且 计数器大于插入 那么跳出循环 */
			if ((pCur->DevName > pNode->DevName) ||
				((pCur->DevName == pNode->DevName) && (pCur->TCr >= pNode->TCr)))
			{
				break;
			}
			else
			{
				/* 下一个节点 */
				pLast = pCur;
				pCur = pCur->pNextNode;

			}
		}

		/* 插入节点 */
		if (NULL == pLast)
		{
			/* 表头 */
			pNode->pNextNode = pCur;
			pHead = pNode;
		}
		else
		{
			/* 插入到pLast后面 */
			pLast->pNextNode = pNode;
			pNode->pNextNode = pCur;
		}

		return pHead;		/* 返回表头 */
	}

	/***********************************************************************
	 * 方法名   : DelTmpMsgLnkNode_WL
	 * 功能描述 : 删除对应的节点。
	 * 输入参数 :
	 *	参数名		类型				输入输出		描述
	 *  --------------------------------------------------------------
	 *	pCur		DataNode_WL*		IN			删除节点
	 *	pLast		DataNodev*		IN			删除前节点
	 *	pHead		DataNode_WL**		INOUT		表头指针的指针，最为返回的表头指针
	 *
	 * 返回值   : UINT8   1: 正常		0:
	 ***********************************************************************/
	UINT8 DelTmpMsgLnkNode_WL(DataNode_WL *pCur, DataNode_WL *pLast, DataNode_WL **pHead, RsspStackElement_WL *pStackOfDataLink)
	{
		UINT8 ret = 1;
		/* 当前节点不为表头 */
		if (pCur != *pHead)
		{
			pLast->pNextNode = pCur->pNextNode;
		}
		else
		{
			/* 当前节点为表头 */
			*pHead = pCur->pNextNode;
		}

		/* 清空pCur指向节点的内容 */
		pCur->DataLen = 0;
		pCur->DestLogId = 0;
		pCur->DevName = 0;
		pCur->DestTypeId = 0;
		pCur->LocalLogId = 0;
		pCur->DestLogicId = 0;
		pCur->TCr = 0;
		pCur->pNextNode = 0;

		/* 压入空闲堆栈 */
		if (0 == StackPushWl(&pStackOfDataLink->stack, (STACK_DATA_TYPE_WL)pCur))
		{
			/* 压入堆栈失败 */
			ret = 0;
		}
		else
		{
			ret = 1;
		}

		return ret;
	}

	/***********************************************************************
	 * 方法名   : InitMsgNodeStack_WL
	 * 功能描述 : 初始化临时报文堆栈空间。
	 * 输入参数 :
	 *	参数名		类型			输入输出		描述
	 *  --------------------------------------------------------------
	 *	NodeNum				UINT16					IN		最大报文数。
	 *	MaxData				UINT16					IN		报文数据最大大小
	 *	pStackOfDataLink	RsspStackElement_WL**	INOUT	堆栈元素
	 *
	 * 返回值   : UINT8  1：正常返回  0：错误返回
	 ***********************************************************************/
	UINT8 InitMsgNodeStack_WL(UINT16 NodeNum, UINT16 MaxData, RsspStackElement_WL **pStackOfDataLink)
	{
		UINT8 ret = 0;
		UINT16 index = 0;
		UINT16	NodeLen = 0;
		DataNode_WL* pTmp = NULL;

		NodeLen = sizeof(DataNode_WL);

		/* ADD 20130802 */
		if ((0 == NodeNum) || (0 == MaxData) || (NULL == pStackOfDataLink))
		{
			return ret;
		}
		/* 临时报文堆栈结构体分配空间 */
		*pStackOfDataLink = (RsspStackElement_WL*)malloc(sizeof(RsspStackElement_WL));
		if (NULL == *pStackOfDataLink)
		{
			return ret;
		}
		else
		{
			CommonMemSet(*pStackOfDataLink, sizeof(RsspStackElement_WL), 0, sizeof(RsspStackElement_WL));
		}

		/* 堆栈初始化 */
		ret = StackInitialWl(&(*pStackOfDataLink)->stack, NodeNum);
		if (0 == ret)
		{
			/* 处理失败 释放已分配空间 */
			DestoryMsgNodeStack_WL(pStackOfDataLink);

			/* 堆栈初始化失败 */
			return ret;
		}

		/* 报文空间分配 */
		(*pStackOfDataLink)->pFirstNode = (DataNode_WL *)malloc(NodeNum * NodeLen);
		if (NULL == (*pStackOfDataLink)->pFirstNode)
		{
			/* 处理失败 释放已分配空间 */
			DestoryMsgNodeStack_WL(pStackOfDataLink);

			/* 内存分配失败 */
			ret = 0;
			return ret;
		}
		else
		{
			CommonMemSet((*pStackOfDataLink)->pFirstNode, NodeNum * NodeLen, 0, NodeNum * NodeLen);
		}

		/* 临时报文节点压入堆栈 */
		for (index = 0; index < NodeNum; index++)
		{
			pTmp = &(*pStackOfDataLink)->pFirstNode[index];
			pTmp->Data = (UINT8 *)malloc(MaxData);			/* 实际报文报文区 */
			if (NULL == pTmp->Data)
			{
				/* 处理失败 释放已分配空间 */
				DestoryMsgNodeStack_WL(pStackOfDataLink);
				ret = 0;
				return ret;
			}
			else
			{
				/* 什么不做 */
				CommonMemSet(pTmp->Data, MaxData, 0, MaxData);
			}

			/* 压入栈 */
			ret = StackPushWl(&(*pStackOfDataLink)->stack, (STACK_DATA_TYPE_WL)pTmp);
			if (0 == ret)
			{
				/* 处理失败 释放已分配空间 */
				DestoryMsgNodeStack_WL(pStackOfDataLink);
				return ret;
			}
			else
			{
				/* 什么不做 */
			}
		}

		return ret;
	}


	/***********************************************************************
	 * 方法名   : DestoryMsgNodeStack_WL
	 * 功能描述 : 释放临时报文堆栈空间
	 * 输入参数 :
	 *	参数名				类型				输入输出		描述
	 *  -------------------------------------------------------------
	 *	无
	 *
	 * 返回值   : UINT8   1：   正常返回
	 ***********************************************************************/
	UINT8 DestoryMsgNodeStack_WL(RsspStackElement_WL **pStackOfDataLink)
	{
		UINT8 ret = 1;
		DataNode_WL *pNode = NULL;

		/* 全局临时报文堆栈为空，正常返回 */
		if (NULL == *pStackOfDataLink)
		{
			return ret;
		}

		while (StackStatusWl(&(*pStackOfDataLink)->stack) > 0)
		{
			/* 已经保证了堆栈里肯定有空闲节点，因而处理不会出错 */
			ret = StackPopWl(&(*pStackOfDataLink)->stack, (STACK_DATA_TYPE_WL*)&pNode);
			if (pNode->Data != NULL)
			{
				free(pNode->Data);
				pNode->Data = NULL;
			}
		}

		/* 释放堆栈 */
		ret = StackFreeWl(&(*pStackOfDataLink)->stack);

		/* 释放临时报文队列节点缓存区 */
		if ((*pStackOfDataLink)->pFirstNode != NULL)
		{
			free((*pStackOfDataLink)->pFirstNode);
			(*pStackOfDataLink)->pFirstNode = NULL;
		}

		/* 释放全局的指针pRsspStru->StackOfDataLink指向的空间 */
		free((*pStackOfDataLink));
		(*pStackOfDataLink) = NULL;

		return ret;
	}

	/***********************************************************************
	 * 方法名   : IsRedunRcvMsg
	 * 功能描述 : 检查报文应用类型和消息类型是否为RSSP报文，以及对报文的进行CRC验证。
	 * 输入参数 :
	 *	参数名			类型				输入输出		描述
	 *  --------------------------------------------------------------
	 *	MsgLen			const UINT16	in			报文长度
	 *	pData			const UINT8		in			报文，（包含16位的CRC）
	 *
	 * 返回值   : UINT8 1：RSSP报文		0:非RSSP报文
	 ***********************************************************************/
	UINT8 IsRedunRcvMsg(UINT16 MsgLen, const UINT8 pData[])
	{
		UINT8 ret = 1;
		UINT16 calcCrc = 0, msgCrc = 0;

		/* 报文长度 小于最小RSSP报文长度（SSE报文长度） */
		if (MsgLen < RSSP_SSE_LEN_WL)
		{
			ret = 0;
		}
		else
		{
			/*   RSSP消息格式
			 *  --------------------------------------------------------
			 *         Head						|	Body	| 16位 CRC	|
			 *	---------------------------------------------------------
			 *	应用域|消息类型|源地址|目标地址	|			|			|
			 *  ---------------------------------------------------------
			 *   1字节|	 1字节 | 2字节|	2字节   |     n     |  2 字节   |
			 *  ---------------------------------------------------------
			 */
			calcCrc = ComputeMsgCRC16_WL(pData, (UINT16)(MsgLen - (UINT16)(CRC_16_LEN_WL)));
			msgCrc = ShortFromCharLE(pData + MsgLen - CRC_16_LEN_WL);

			/* CRC正常检查 */
			if (calcCrc != msgCrc)
			{
				ret = 0;
			}
			else
			{
				/* Application Category = 01 */
				/* Message Type = 80 or 90 or 91 */
				if ((RSSP_APP_CATEGORY_WL != pData[RSSP_APP_CTGRY_POS_WL])
					|| ((RSSP_SSE_MSG_WL != pData[RSSP_MSG_TYPE_POS_WL])
						&& (RSSP_SSR_MSG_WL != pData[RSSP_MSG_TYPE_POS_WL])
						&& (RSSP_RSD_MSG_WL != pData[RSSP_MSG_TYPE_POS_WL])
						&& (RSSP_RSD_MSG_WL_B != pData[RSSP_MSG_TYPE_POS_WL])))
				{
					ret = 0;
				}
				else
				{
					ret = 1;
				}
			}
		}
		return ret;
	}

	/*--------------------------------------------------------------------
	 * RSSP层内部接口函数申明 End
	 *--------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif



