/*
*
* 文件名   ：  ProtclManage.h
* 版权说明 ：  北京北京交控科技有限公司 
* 版本号   ：  1.0
* 创建时间 ：  2013.09.15
* 作者     ：  王佩佩
* 功能描述 ：  协议主处理类,主要实现了周期性调用协议。
* 使用注意 ：  
* 修改记录 ：
*    时间			修改人		修改理由
*---------------------------------------------
*/
#ifndef PROTCL_MANAGE_H
#define PROTCL_MANAGE_H

#include "dquCbtcType.h"
#include "protclStruct.h"

#ifdef USE_SFP
#include "sfpplatform.h"
#endif
#ifdef USE_RP
#include "RpPlatform.h"
#endif
#ifdef USE_RSSPI
#include "RsspWl.h"
#endif
#ifdef USE_RSR
#include "Rsr.h"
#endif

#define LEN_IP_ADDR  4              /*IP地址长度*/

#define PROTCL_OUT_FONT_LEN 5  /* 接收数据输出帧头长度 */
#define VAR_NODE_LEN 400   /* 中间变量长度 */
#define RSR_RSSP_MAX_LEN (2900)	/*ZC-10辆车1450改为2900*/
#define RSSP2_MAX_LEN 1200
#define FSFB_MAX_LEN (200) /*FSFB每帧应用数据最大长度*/

#define SFP_PER_LNK_RECORD_MAX_LEN 96		/*SFP每个链路记录数据最大长度*/
#define RP_PER_LNK_RECORD_MAX_LEN 68		/*RP每个链路记录数据最大长度*/
#define RSR_PER_LNK_RECORD_MAX_LEN 194		/*RSR每个链路记录数据最大长度*/
#define RSSPI_PER_LNK_RECORD_MAX_LEN 130	/*RSSPI每个链路记录数据最大长度*/
#define RSSPII_PER_LNK_RECORD_MAX_LEN 158	/*RSSPII每个链路记录数据最大长度*/
#define FSFB_PER_LNK_RECORD_MAX_LEN 68		/*FSFB每个链路记录数据最大长度*/


#define PRTCL_ERR_INFO_LEN		10	/* 返回协议错误码数组大小 */

#ifdef __cplusplus
extern "C" {
#endif

#define	ERR_PRTCL_MANAGE_TYPE_ERR  				0x00000001	
#define	ERR_PRTCL_MANAGE_DATA_ANALYSIS_ERR		0x00000002	
	   
/*
 * 协议初始化
 *
 */
UINT8 Init(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], ProtclConfigInfoStru* pUnifyInfo);

/*
* 功能：协议发送数据
*
* 参数： 
* 输入输出参数：  ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    UINT8 1 正常，其他不正常
*/
UINT8 SendAppData(ProtclConfigInfoStru *pUnifyInfo);

/*
* 功能：协议接收数组
* 参数：
* 输入输出参数：ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    1：接收数据成功
0：失败
*/

UINT8 ReceiveAppData(ProtclConfigInfoStru* pUnifyInfo);

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
*/
UINT8 GetLnkStatus (UINT8 destType,UINT8 destId,UINT16 logId,ProtclConfigInfoStru * pUnifyInfo);

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
UINT8 DelLnkFunc(UINT8 destType,UINT8 destId,UINT16 logId,ProtclConfigInfoStru *pUnifyInfo);

/*
* 功能：刷新链路状态
*
* 参数： 
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值： 
*     0：刷新链路失败
*     1：刷新链路成功 
*/
UINT8 ReFresh(UINT8 *pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo);

/*
* 协议销毁协议模块
* 参数： 
*    ProtclConfigInfoStru *pUnifyInfo 协议结构体
* 返回值：
*    1 ：销毁成功
*    0 ：销毁失败
*/
UINT8 FreeSpace(ProtclConfigInfoStru *pUnifyInfo);

UINT8 ReceiveTcpState(UINT16 TcpStateDataLen,UINT8* pTcpStateData, ProtclConfigInfoStru *pUnifyInfo);
UINT8 SendTcpCtrl(UINT16 *pTcpCtrlDataLen,UINT8 *pTcpCtrlData, ProtclConfigInfoStru *pUnifyInfo);

UINT8 InitGetCfgInfo(UINT8* pFileName, UINT32 inDataLen, UINT8 inData[], UINT8* pEvenPrtclNumBuff, ProtclConfigInfoStru* pUnifyInfo);
/***********协议发送相关函数**************************************************************/
UINT8 AppSendInputDataAnalysis(QueueStruct *pAppQueueData,PROTCL_APP_TO_STRU *pProtclAppStru);
UINT8 sfpInputPackFms(const PROTCL_APP_TO_STRU protclAppStru, QueueStruct *pAppQueueData);
UINT8 sfpSendData(ProtclConfigInfoStru *pUnifyInfo, UINT8 devDataCount,UINT8* pSndData,UINT32* sendDataLen,UINT8 * pLnkStatusData,UINT16* lnkDataLen,UINT8* recordArray,UINT16* recordDataLen);
UINT8 RsspInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct *pAppQueueData);
UINT8 RsspSendData(ProtclConfigInfoStru *pUnifyInfo, UINT8 devDataCount,UINT8* pSndData,UINT32* sendDataLen,UINT8* pLnkStatusData,UINT16* lnkDataLen,UINT8* recordArray,UINT16* recordDataLen);

#ifdef USE_RSR
UINT8 RsrInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct *pAppQueueData, RSR_INFO_STRU *gpRsrUnitInfo);
#endif

UINT8 RsrSendData(ProtclConfigInfoStru *pUnifyInfo, UINT8 devDataCount,UINT8* pSndData,UINT32* sendDataLen,UINT8* pLnkStatusData,UINT16* lnkDataLen,UINT8* recordArray,UINT16* recordDataLen);
UINT8 RpInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct *pAppQueueData);
UINT8 RpSendData(ProtclConfigInfoStru *pUnifyInfo, UINT8 devDataCount,UINT8* pSndData,UINT32* sendDataLen,UINT8* pLnkStatusData,UINT16* lnkDataLen,UINT8* recordArray,UINT16* recordDataLen);
UINT8 RsrAndRpAppArrayPage(UINT8* pInAppArray,UINT32 appDtaLen,UINT8* pOutAppArray,UINT32* outAppLen);
UINT8 Rssp2InputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct *pAppQueueData);
UINT8 Rssp2SendData(ProtclConfigInfoStru *pUnifyInfo, UINT8 devDataCount,UINT8* pSndData,UINT32* sendDataLen,UINT8* pLnkStatusData,UINT16* lnkDataLen,UINT8* recordArray,UINT16* recordDataLen);
UINT8 FsfbInputPackFms(PROTCL_APP_TO_STRU protclAppStru, QueueStruct* pAppQueueData);
UINT8 FsfbSendData(ProtclConfigInfoStru* pUnifyInfo, UINT8 devDataCount, UINT8* pSndData, UINT32* sendDataLen, UINT8* pLnkStatusData, UINT16* lnkDataLen, UINT8* recordArray, UINT16* recordDataLen);
/***********协议接收相关函数**************************************************************/
UINT8 SfpRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru *pUnifyInfo);
UINT8 RpRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru *pUnifyInfo);
UINT8 RsrRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru *pUnifyInfo);
UINT8 RsspRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru *pUnifyInfo);
UINT8 SetDataToQue(UINT32 dataLen,UINT8 *buff,QueueStruct* q);
UINT8 AppReceiveDataAnalysis(QueueStruct *pAppQueueData,PROTCL_APP_TO_STRU *pProtclAppStru);
UINT8 readQueueDataToApp(QueueStruct *rcvQueueData, UINT8 OutNetFlag, UINT16 locDevName, PROTCL_RCV_DATA_TYPE* pDataToApp, ProtclConfigInfoStru *pUnifyInfo);
UINT8 RcvDataToAppQueue(UINT8 devType,UINT8 devId,UINT16 logId,UINT8 lifeTime,UINT16 appDatalen,UINT8* pAppData,QueueStruct* pDataToApp);
UINT8 Rssp2RcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru *pUnifyInfo);
UINT8 FsfbRcvData(QueueStruct* pOutToApp, ProtclConfigInfoStru* pUnifyInfo);
/***********协议链路相关函数***********************************************************/
UINT8 GetSfpLnkStatus (UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 GetRsrLnkStatus (UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 GetRsspLnkStatus (UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 GetRssp2LnkStatus (UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 GetFsfbLnkStatus(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo);

/*******************************************删除链路相关函数 **********************************************/

UINT8 SfpDelLnk(UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 RpDelLnk(UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 RsrDelLnk(UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 RsspDelLnk(UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 Rssp2DelLnk(UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);

UINT8 FsfbDelLnk(UINT8 destType, UINT8 destId, UINT16 logId, ProtclConfigInfoStru* pUnifyInfo);

/*********************刷新链路相关函数***********************************************/
UINT8 SfpReFreshLnk(UINT16 slen_input,UINT8 *pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo);
UINT8 RpReFreshLnk(UINT16 slen_input,UINT8 *pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo);
UINT8 RsrReFreshLnk( UINT16 slen_input,UINT8 *pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo);
UINT8 RsspReFreshLnk(UINT16 slen_input,UINT8 *pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo);
UINT8 Rssp2ReFreshLnk(UINT16 slen_input,UINT8 *pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo);
UINT8 FsfbReFreshLnk(UINT16 slen_input, UINT8* pLnkStatusData, ProtclConfigInfoStru* pUnifyInfo);

/********************************************销毁空间相关函数*********************************************/
UINT8 SfpFreeSpaceFunc(ProtclConfigInfoStru *pUnifyInfo);
UINT8 RpFreeSpaceFunc(ProtclConfigInfoStru *pUnifyInfo);
UINT8 RsrFreeSpaceFunc(ProtclConfigInfoStru *pUnifyInfo);
UINT8 RsspFreeSpaceFunc(ProtclConfigInfoStru *pUnifyInfo);
UINT8 Rssp2FreeSpaceFunc(ProtclConfigInfoStru *pUnifyInfo);
UINT8 FreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo);
UINT8 FsfbFreeSpaceFunc(ProtclConfigInfoStru* pUnifyInfo);

/***********Getting**&**Setting****封装***********************************************/

/***********异常防护函数**************************************************************/
UINT8  protcl_nul_Init(UINT8* nul_FileName, UINT32 inDataLen, UINT8 inData[], UINT8 nul_Num, ProtclConfigInfoStru* pUnifyInfo);
UINT8  protcl_nul_Snd(ProtclConfigInfoStru *pUnifyInfo, UINT8 devDataCount,UINT8* pSndData,UINT32* sendDataLen,UINT8 * pLnkStatusData,UINT16* lnkDataLen,UINT8* recordArray,UINT16* recordDataLen);
UINT8  protcl_nul_Rcv(QueueStruct* PQueueStru, ProtclConfigInfoStru *pUnifyInfo);
UINT8  protcl_nul_Lnk(UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);
void   protcl_nul_NetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);
UINT8  protcl_nul_DelLnk(UINT8 destType,UINT8 destId,UINT16 logId, ProtclConfigInfoStru *pUnifyInfo);
UINT8  protcl_nul_ReFreshLnk(UINT16 slen_input,UINT8 *pLnkStatusData, ProtclConfigInfoStru *pUnifyInfo);
UINT8  protcl_nul_End(ProtclConfigInfoStru *pUnifyInfo);

UINT8  GetProtclErrorInfo(UINT32 vProtclErrorInfo[]);
UINT8  ReSetProtclErrorInfo(UINT32 vProtclErrorInfo[]);

UINT8  SetRsspConfigData(UINT8 vNoSendDataNum, ProtclConfigInfoStru *pUnifyInfo);

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
void GetNetsRecvPkgNum(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);

#ifdef __cplusplus
}
#endif

#endif
