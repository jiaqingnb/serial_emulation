/************************************************************************
*
* 文件名   ：  dsuRsspFunc.h
* 版权说明 ：  北京交控科技有限公司 
* 版本号   ：  1.0
* 创建时间 ：  2011.12.30
* 作者     ：  研发中心软件部
* 功能描述 ：  安全通信协议rsr查询函数  
* 使用注意 ： 
* 修改记录 ：  
* 20130222  王佩佩 删除通信角色的宏
************************************************************************/

#ifndef DSU_RSSP_FUNC_H
#define DSU_RSSP_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dsuRsspDataStru.h"
#include "RsspConfigWl.h"
/* RSSP查询函数版本号定义 */
#define DQU_RSSP_VER_PRODUCT_NO 88/*产品编号*/
#define DQU_RSSP_VER_MODULE_NO  19/*软件模块编号*/
#define DQU_RSSP_VER_MAIN_VER   20/*软件模块主版本号*/
#define DQU_RSSP_VER_SUB_VER    3 /*软件模块子版本号*/
	/*RsspSGetLnkInfo函数使用的结构体*/
	typedef struct {
		UINT8 LocalLogId; /* 表示报文源动态ID */
		UINT32 DestName;/*表示报文目标标识*/
		UINT8 DestLogId; /* 表示报文的目标动态ID */
		UINT16  SseAddr; /*表示SSE_SSR_Destination_Addres，见数据库*/
		UINT32 SID_NUMBER_E[2];/* 见数据库，数组下标0和1分别对应数据库中的1和2通道,该值为发送方SID*/
		UINT32 SID_NUMBER_R[2];/* 见数据库，数组下标0和1分别对应数据库中的1和2通道,该值为接受方SID*/
		UINT32 SINIT_NUMBER[2];/* 见数据库，数组下标0和1分别对应数据库中的1和2通道*/
		UINT8 DATANUM[2];/*见数据库，数组下标0和1分别对应数据库中的1和2通道*/
		UINT32 DATAVER_1_Number[2]; /*见数据库，数组下标0和1分别对应数据库中的1和2通道*/
	} DSU_RSSP_LINK_SINFO_STRU_WL;

	/*RsspRGetLnkInfo函数使用的结构体*/
	typedef struct {
		UINT8 LocalLogId; /* 表示报文源动态ID */
		UINT32 DestName;/*表示报文目标标识*/
		UINT8 DestLogId; /* 表示报文的目标动态ID */
		UINT16  SseAddr; /*表示SSE_SSR_Destination_Addres，见数据库*/
		UINT32 SID_NUMBER_E[2];/* 见数据库，数组下标0和1分别对应数据库中的1和2通道,该值为发送方SID*/
		UINT32 SID_NUMBER_R[2];/* 见数据库，数组下标0和1分别对应数据库中的1和2通道,该值为接受方SID*/
		UINT32 PREC_FIRSTSINIT[2]; /*表示首个SINIT*/
		UINT32 POST_RXDADA_K[2][DSURSR_MAX_K_NUM_WL];  /*表示BSD解码固定值*/
		UINT32 PREC_SINIT_K[2][DSURSR_MAX_K_NUM_WL];  /*表示SINIT值*/
		UINT8 MaxLost;/*LOST报文数*/
		UINT8 DLost;/*长时间无周期数*/
		UINT8 WSsrOut;/*等待SSR超时*/
		UINT8 AntiDelayCount;/*定期同步周期数*/
		UINT8 TOut;  /*报文超时*/
	} DSU_RSSP_LINK_RINFO_STRU_WL;

	/*
	*函数名:	DsuRsspInitWl
	*功能描述:	RSSPI协议查询函数初始化
	*输入参数:	UINT8 *pFsName	FS文件
	*			UINT16 localTypeId	本方类型ID
	*			UINT16 localLogicId	本方逻辑ID
	*输入出参:	DSU_RSSP_INFO_STRU_WL *pDsuRsspInfo	RSSPI配置信息
	*输出参数:	无
	*返回值:	1:成功;0失败
	*/
	UINT8 DsuRsspInitWl(UINT8* pFsName, UINT32 inDataLen, UINT8 inData[], UINT16 localTypeId, UINT16 localLogicId, DSU_RSSP_INFO_STRU_WL* pDsuRsspInfo);
	/*
	*函数名:	dsuSetCommInfo_WL
	*功能描述:	根据本方对方设备类型设置作为接收方时配置参数
	*输入参数:	UINT8 localType	本方类型
	*			UINT8 DestType	对方类型
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	DSU_RSSP_LINK_RINFO_STRU_WL *pRsspLnkRInfo	接收方时配置参数
	*返回值:	1:成功;0失败
	*/
	UINT8 dsuSetCommInfo_WL(DSU_RSSP_LINK_RINFO_STRU_WL* pRsspLnkRInfo, UINT8 localType, UINT8 DestType, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL);

	/*
	*函数名:	dsuRsspGetDevTypeWl
	*功能描述:	获取设备类型
	*输入参数:	UINT16 DevName	设备名称
	*输入出参:	无
	*输出参数:	无
	*返回值:	设备类型
	*/
	UINT8 dsuRsspGetDevTypeWl(UINT16 DevName);

	/*
	*函数名:   dsuRsspRGetLnkInfoWl
	*功能描述:  用于查询RSSP层的链路相关信息（接收节点信息）的映射数据
	*输入参数:  UINT16 RsspAddr Rssp地址
	*           UINT16 SrcAddr  Src地址
	*           DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL   RSSPI配置信息
	*输入出参:  无
	*输出参数:  DSU_RSSP_LINK_RINFO_STRU_WL *pRsspLnkRInfo  接收方时配置参数
	*返回值:   1:成功;0失败
	*/
	UINT8 dsuRsspRGetLnkInfoWl(UINT16 RsspAddr, UINT16 SrcAddr, DSU_RSSP_LINK_RINFO_STRU_WL* pRsspLnkRInfo, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL);

	/*
	*函数名:	dsuRsspSGetLnkInfoWl
	*功能描述:	用于查询RSSP层的链路相关信息（发送节点信息）的映射数据
	*输入参数:	UINT16 RsspAddr	源地址
	*			UINT16 BsdAddr	目的地址
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	DSU_RSSP_LINK_SINFO_STRU_WL* pRsspLnkSInfo,	发送方时配置参数
	*返回值:	1:成功;0失败
	*/
	UINT8 dsuRsspSGetLnkInfoWl(UINT16 RsspAddr, UINT16 BsdAddr, DSU_RSSP_LINK_SINFO_STRU_WL* pRsspLnkSInfo, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL);

	/*
	*函数名:	dsuRsspRGetAddrWl
	*功能描述:	根据本方设备名称和对方设备名称，查询RSSP链路表，获取源地址、目标地址
	*输入参数:	UINT16 destDevName	目的设备名称
	*			UINT16 locDevName	本方设备名称
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	UINT16* rsspSrcAddr	源地址
	*			UINT16* rsspDestAddr	目的地址
	*返回值:	1:成功;0失败
	*/
	UINT8 dsuRsspRGetAddrWl(UINT32 destDevName, UINT32 locDevName, UINT16* rsspSrcAddr, UINT16* rsspDestAddr, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL);

	/*
	*函数名:	dsuRsspRGetName_WL
	*功能描述:	根据源地址、目标地址，查询RSSP链路表，获取本方设备名称和对方设备名称
	*输入参数:	UINT16 rsspSrcAddr	源地址
	*			UINT16 rsspDestAddr	目的地址
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	UINT16* destDevName	目的设备名称
	*返回值:	1:成功;0失败
	*/
	UINT8 dsuRsspRGetName_WL(UINT16 rsspSrcAddr, UINT16 rsspDestAddr, UINT32* destDevName, DSU_RSSP_INFO_STRU_WL* pDsuRsrInfoStru_WL);
#ifndef RSSP_NO_HLHT
	/*
	*函数名:	DquToHlhtId
	*功能描述:	根据设备ID,逻辑ID转成HLHTID
	*输入参数:	UINT8 devType	设备类型
	*			UINT8 devId		设备ID
	*			UINT16 logicId	逻辑ID
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	无
	*返回值:	HLHTID
	*/
	UINT32 DquToHlhtId(UINT8 devType, UINT8 devId, UINT16 logicId, DSU_RSSP_INFO_STRU_WL *pDsuRssp);

	/*
	*函数名:	DquTypeIdToHlhtId
	*功能描述:	根据设备ID,逻辑ID转成HLHTID
	*输入参数:	UINT16 devTypeId	设备类型ID
	*			UINT16 logicId	逻辑ID
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	无
	*返回值:	HLHTID
	*/
	UINT32 DquTypeIdToHlhtId(UINT16 devTypeId, UINT16 logicId, DSU_RSSP_INFO_STRU_WL *pDsuRssp);

	/*
	*函数名:	DquToDevTypeId
	*功能描述:	根据HLHTID转成设备类型ID
	*输入参数:	UINT32 hlhtid	HLHTID
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	无
	*返回值:	设备类型ID
	*/
	UINT16 DquToDevTypeId(UINT32 hlhtid, DSU_RSSP_INFO_STRU_WL *pDsuRssp);

	/*
	*函数名:	DquToDevType
	*功能描述:	根据HLHTID转成设备类型
	*输入参数:	UINT32 hlhtid	HLHTID
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	无
	*返回值:	设备类型
	*/
	UINT8 DquToDevType(UINT32 hlhtid, DSU_RSSP_INFO_STRU_WL *pDsuRssp);

	/*
	*函数名:	DquToDevId
	*功能描述:	根据HLHTID转成设备ID
	*输入参数:	UINT32 hlhtid	HLHTID
	*			DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	无
	*返回值:	设备ID
	*/
	UINT8 DquToDevId(UINT32 hlhtid, DSU_RSSP_INFO_STRU_WL *pDsuRssp);

	/*
	*函数名:	FreeDsuRsspi
	*功能描述:	释放RSSPI查询函数使用的空间
	*输入参数:	DSU_RSSP_INFO_STRU_WL *pDsuRsrInfoStru_WL	RSSPI配置信息
	*输入出参:	无
	*输出参数:	无
	*返回值:	无
	*/
	void FreeDsuRsspi(DSU_RSSP_INFO_STRU_WL **pDsuRsspInfo);
#endif
#ifdef __cplusplus
}
#endif

#endif


