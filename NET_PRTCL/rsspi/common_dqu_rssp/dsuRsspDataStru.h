/*******************************************************************************
*
* 文件名  ：dsuRsspDataStru.h
* 版权说明：北京瑞安时代科技有限责任公司
* 版本号  ：1.0  
* 创建时间：2009.11.20
* 作者    ：车载及协议部
* 功能描述：与RSR协议相关的dsu查询结构体定义，内部使用
* 使用注意：无
* 修改记录：无
**	2013.02.15	王佩佩	删除DSU_RSSP_LNK_INFO_STRU，增加DSU_RSSP_LNK_INFO_STRU_WL
**	2013.02.15	王佩佩	删除DSU_RSSP_INFO_STRU，增加DSU_RSSP_INFO_STRU_WL
*******************************************************************************/

#ifndef DSU_RSSP_DATA_STRU_H
#define DSU_RSSP_DATA_STRU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CommonTypes.h"

#define	DSURSR_MAX_K_NUM_WL	10	/*离散参数SINIT RXDADA数量*/

	/*RSSP链接信息表*/
	typedef struct
	{
		UINT16 Id;	/*序号*/
		UINT32 EmitterName;	/*本方类型ID*/
		UINT32 ReceptorName;	/*对方类型ID*/
		UINT16 ChannelNumber;	/*通道号*/
		UINT32 SID_NUMBER_E;	/*本方SID*/
		UINT32 SINIT_NUMBER;	/*本方SINIT*/
		UINT8 DATANUM;	/*版本号*/
		UINT32 DATAVER_1_Number;	/*本方DATAVER*/
		UINT16 RsspAddr;	/*源地址*/
		UINT16 BsdAddr;	/*目的地址BSD*/
		UINT16 SseAddr;	/*目的地址*/
		UINT32 SID_NUMBER_R;	/*对方SID*/
		UINT32 PREC_FIRSTSINIT;	/*首个SINIT*/
		UINT32 PREC_SINIT_K[DSURSR_MAX_K_NUM_WL];	/*离线SINIT*/
		UINT32 POST_RXDADA_K[DSURSR_MAX_K_NUM_WL];	/*离线RXDADA*/
	}DSU_RSSP_LNK_INFO_STRU_WL;

	/*设备通信属性表*/
	typedef struct
	{
		UINT16 EmitterType;	/*发送方类型*/
		UINT16 ReceptorType;	/*接收方类型*/
		UINT8 MaxLost;	/*最大丢包数*/
		UINT8 DLost;	/*连续收不到数据周期数*/
		UINT8 WSsrOut;           /*等待回复SSR周期数*/
		UINT8 AntiDelayCount;    /*定期同步周期数-未用*/
		UINT8 TOut;              /*报文超时周期数*/
		UINT8 MsgId;             /*消息ID-未用*/
		UINT8 IftVer;            /*接口版本号,未用*/
	}DSU_DEVICE_COMM_INFO_STRU_WL;

	/*设备对应的HLHTID列表*/
	typedef struct
	{
		UINT16	DevId;	/*设备ID*/
		UINT32	*HlhtIds;	/*hlhtid列表*/
	} DSU_DEV_HLHTID;

	/*RSSPI配置信息*/
	typedef struct
	{
		UINT32 LocEmitterName;	/*配置的本设备唯一ID*/
		UINT16 LocRsspiAddr;	/*配置的对应本设备唯一ID的RSSPI地址*/

		UINT16 RsspInfoStruLen;	/*RSSP链接信息表行数*/
		DSU_RSSP_LNK_INFO_STRU_WL *pRsspLnkInfoStru;	/*RSSP链接信息表*/

		UINT16 DeviceCommStruLen;	/*设备通信属性表行数*/
		DSU_DEVICE_COMM_INFO_STRU_WL	*pDevCommInfoStru;	/*设备通信属性表*/
		
		UINT16	ZcRowNum;	/*ZC编号对照表行数*/
		DSU_DEV_HLHTID *ZcHlhtids;	/*ZC编号对照表*/

		UINT16	CiRowNum;	/*CI编号对照表行数*/
		DSU_DEV_HLHTID *CiHlhtids;	/*CI编号对照表*/

		UINT16	AtsRowNum;	/*ATS编号对照表行数*/
		DSU_DEV_HLHTID *AtsHlhtids;	/*ATS编号对照表*/

		UINT16	VobcRowNum;	/*VOBC编号对照表行数*/
		DSU_DEV_HLHTID *VobcHlhtids;	/*VOBC编号对照表*/

		UINT16	DsuRowNum;	/*DSU编号对照表行数*/
		DSU_DEV_HLHTID *DsuHlhtids;	/*DSU编号对照表*/

		UINT16	AomRowNum;	/*AOM编号对照表行数*/
		DSU_DEV_HLHTID *AomHlhtids;	/*AOM编号对照表*/

		UINT16	TdtRowNum;	/*TDT编号对照表行数*/
		DSU_DEV_HLHTID *TdtHlhtids;	/*TDT编号对照表*/
	}DSU_RSSP_INFO_STRU_WL;

#ifdef __cplusplus
}
#endif

#endif

