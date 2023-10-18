/********************************************************
*                                                                                                            
* 文件名  ： RsspTmpMsgLnkCommonWl.h   
* 版权说明： 北京瑞安时代科技有限责任公司 
* 版本号  ： 1.0
* 创建时间： 2009.11.25
* 作者    ： 车载及协议部
* 功能描述： Rssp层临时报文链表处理
* 使用注意： 
*   
* 修改记录：   
*   更新时间	更新者	更新理由
* ------------  ------  ---------------
*	2009.12.07	楼宇伟	初版作成
********************************************************/ 

#ifndef RSSP_TMP_MSG_LNK_COMMONWL_WL_H
#define RSSP_TMP_MSG_LNK_COMMONWL_WL_H

#include "CommonQueue.h"
#include "RsspCommonWl.h"

#ifdef __cplusplus
extern "C" {
#endif

	
/* 临时报文链表节点 */
typedef struct DataNodeStru_WL
{
	UINT32	DevName;	/* 设备标识 */
	UINT16  DestTypeId;	/*对方类型ID*/
	UINT32	TCr;		/* 序列号 */
	UINT8	DestLogId;	/* 对方动态ID */
	UINT8 	LocalLogId;	/* 本方动态ID */
	UINT16	DestLogicId;/*对方逻辑ID*/
	UINT8 FrameType; /*帧类型*/
	UINT16 SrcAddr; /*RSSPI的源地址*/
	UINT16	DataLen;	/* 报文长度 */
	UINT8	*Data;		/* 报文数据 */
	struct DataNodeStru_WL *pNextNode;		/* 下一个节点 */
 } DataNode_WL;

/* 临时报文堆栈结构 */
typedef struct
{	
	CommonStackWl stack;				/* 堆栈 */
	DataNode_WL *pFirstNode;			/* 堆栈空间 */
} RsspStackElement_WL;

/*--------------------------------------------------------------------
 * RSSP层内部接口函数申明 Start
 *--------------------------------------------------------------------*/
/***********************************************************************
 * 方法名   : GetSortedMsgFromQue_WL
 * 功能描述 : 对红蓝网输入数据进行RSSP层的安全处理，将安全数据交付应用。
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
DataNode_WL* GetSortedMsgFromQue_WL(QueueStruct *RpToRs,UINT16 MaxLen,UINT8 *RecordArray, UINT16 RecordSize, UINT8 *pTmpDataBuff, RsspStackElement_WL *pStackOfDataLink);

/***********************************************************************
 * 方法名   : InsTmpMsgLnk_WL
 * 功能描述 : 插入链表一个节点。标识符，序列号顺序排序。
 * 输入参数 : 
 *	参数名		类型				输入输出		描述
 *  --------------------------------------------------------------
 *	pHead		DataNode_WL*		IN			表头
 *	pNode		DataNode_WL*		IN			插入的节点
 *  	 
 * 返回值   : DataNode_WL*  NULL：异常 非NULL：临时报文链表表头
 ***********************************************************************/
DataNode_WL* InsTmpMsgLnk_WL(DataNode_WL *pHead,DataNode_WL *pNode);

/***********************************************************************
 * 方法名   : DelTmpMsgLnkNode_WL
 * 功能描述 : 删除对
 应的节点。
 * 输入参数 : 
 *	参数名		类型				输入输出		描述
 *  --------------------------------------------------------------
 *	pCur		DataNode_WL*		IN			删除节点
 *	pLast		DataNode_WL*		IN			删除前节点
 *	pHead		DataNode_WL**		INOUT		表头
 * 
 * 返回值   : DataNode_WL*  NULL：异常 非NULL：临时报文链表表头
 ***********************************************************************/
UINT8 DelTmpMsgLnkNode_WL(DataNode_WL *pCur,DataNode_WL *pLast,DataNode_WL **pHead, RsspStackElement_WL *pStackOfDataLink);

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
UINT8 InitMsgNodeStack_WL(UINT16 NodeNum,UINT16 MaxData, RsspStackElement_WL **pStackOfDataLink);


/***********************************************************************
 * 方法名   : DestoryMsgNodeStack_WL
 * 功能描述 : 释放临时报文堆栈空间
 * 输入参数 : 
 *	参数名				类型				输入输出		描述
 *  -------------------------------------------------------------
 *	无
 *
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 DestoryMsgNodeStack_WL(RsspStackElement_WL **pStackOfDataLink);

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
UINT8 IsRedunRcvMsg(UINT16 MsgLen, const UINT8 pData[]);

/*--------------------------------------------------------------------
 * RSSP层内部接口函数申明 End
 *--------------------------------------------------------------------*/
 
#ifdef __cplusplus
}
#endif


#endif

