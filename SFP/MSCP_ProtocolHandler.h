/****************************************************************************************************
* 文件名   :  MSCP_ProtocolHandler.h
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2020.02.17
* 作者     :  lsn
* 功能描述 : 平台对输入数据输出数据进行协议处理头文件
* 使用注意 :
* 修改记录 :
**************************************************************************************************/
#ifndef MSCP_PROTOCOL_HANDLER_H_
#define MSCP_PROTOCOL_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CM_Types.h"
#include "CM_Memory.h"
#include "CommonMemory.h"
#include "CommonQueue.h"
#include "Convert.h"

#include "sfpplatform.h"

#define PLFM_TRANS_DATA_TEST    (0u)  //BTM数据是否需要解析FFFE及RSSP-1协议
#define SEND_SFP_ANSWER 1u /* 各个执行板卡回复主机板的为SFP应答标志*/
#define NOT_SEND_SFP_ANSWER 2 /* 各个执行板卡回复主机板的不为SFP应答标志*/
#define SEND_SFP_ANSWER_LEN 70 /* 过完SFP协议对主机板发送应答的长度*/
extern struc_Unify_Info sfpStru;/* SFP协议用到的结构体*/

#if defined(CCS_570)
extern UINT8 SFPFollowData[1024*5];/* 用于存放主备同步的SFP协议的链路数据*/
#endif

extern UINT32 g_TimeCounter; /*用SFP协议 计数*/
extern struc_Unify_Info sfpStru;
UINT8 QueueCopyAdd(QueueStruct* p_stpQueueDest , QueueStruct* p_stpQueueSrc);
ENUM_CM_RETURN SFP_Protocol_INIT(void);
void Send_Sfp_Befor_Hander(struc_Unify_Info * pBefor_Sfp_Stru);
void Send_Sfp_After_Hander(struc_Unify_Info * pAfter_Sfp_Stru);
void Recv_SFP_Before_Handler(struc_Unify_Info * pBefore_Sfp_Stru);
void Recv_SFP_After_Handler(struc_Unify_Info * pAfter_Sfp_Stru);
UINT8 QueueCopyAdd(QueueStruct* p_stpQueueDest , QueueStruct* p_stpQueueSrc);
ENUM_CM_RETURN SFP_Protocol_Pack(UINT8 * pSendbuff,UINT16 SendBuffLen,UINT16 DataTypeId,QueueStruct * pBefor_Sfp_Q);



#ifdef __cplusplus
}
#endif

#endif
