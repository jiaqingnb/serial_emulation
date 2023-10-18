/********************************************************
*                                                                                                            
* 文件名  ： RsspStructWl.h   
* 版权说明： 北京瑞安时代科技有限责任公司 
* 版本号  ： 1.0
* 创建时间： 2009.11.25
* 作者    ： 车载及协议部
* 功能描述： Rssp层中使用的结构体定义
* 使用注意： 
*   
* 修改记录：   
*     2011.12.06  王佩佩  修改通信节点信息结构体。删除动态ID，增加Rssp源地址、Rssp目的地址、Bsd地址。
*
********************************************************/ 

#ifndef RSSP_STRUCT_WL_H
#define RSSP_STRUCT_WL_H

#include "CommonTypes.h"
#include "RsspCommonWl.h"
#include "RsspTmpMsgLnkCommonWl.h"

#ifdef __cplusplus
extern "C" {
#endif


/* RSSP协议版本号定义 */
#define RSSPCODE_VER_PRODUCT_NO 88/*产品编号*/
#define RSSPCODE_VER_MODULE_NO  20/*软件模块编号*/
#define RSSPCODE_VER_MAIN_VER   20/*软件模块主版本号*/
#define RSSPCODE_VER_SUB_VER    3 /*软件模块子版本号*/

/*--------------------------------------------------------------------
 * RSSP层内部使用结构体申明 Start
 *--------------------------------------------------------------------*/
	   
/* 链路的状态 */
typedef enum 
{
	Halt_WL		= 0x00,		/* 中断	重新发送n次之sse报文之后的状态（n暂时设为4） 或者初始化状态（IDLE） */
	SendSSE_WL	= 0x01,  	/* 发送SSE		接受节点 */
	WaitSSR_WL	= 0x02,  	/* 等待SSR		接受节点 */
	RecvData_WL	= 0x04,  	/* 接受数据	接受节点 */
	SendSSR_WL	= 0x20,  	/* 发送SSR		发送节点 */
	SendData_WL	= 0x40  	/* 发送数据	发送节点 */
} ENUM_RSSP_LINK_STATUS_WL;

/* 节点状态枚举型 */
typedef enum
{
	Replace_WL 		= 0x01,	/* 可覆盖状态 该节点停止发送数据或者是初始化状态 */
	DisbaledReplace_WL = 0x02  /* 不可覆盖状态，该节点还在继续发送数据 */
} ENUM_RSSP_NODE_STATUS_WL;

/* 设备节点基本信息结构体 */
typedef struct
{
	UINT32	SIDe[2];					/* 接受SID */
	UINT32	SIDr[2];					/* 发送SID */
	UINT32	SINIT[2];					/* SINIT */
	UINT32	DATAVER[2];					/* DATAVER */
	UINT8		NumDataVer[2];			/* Data 版本号 */
	UINT8		DLost;					/* 最大无数据周期数 */
	UINT8		SSDelay;				/* 规定时间（SSDelay） */
	UINT8		AntiDelayCount;			/* 定期同步周期数 */
	UINT8		Tout;					/* BSD报文超时 */
	UINT8		MaxLost;				/* 最大丢失报文数 */
	UINT32	PREC_SINIT_K[2][DSURSR_MAX_K_NUM_WL];			/* 离线参数PREC_SINIT? */
	UINT32	PREC_FIRSTSINIT[2];			/* 离线参数PREC_FIRSTSINIT */
	UINT32	TSD_POST_RXDADA_K[2][DSURSR_MAX_K_NUM_WL];	/* 离线参数 BSD解码固定值 */
} NodeParamInfoStru_WL;

/* RSSP链路基本信息结构体 */
typedef struct
{
	UINT8	LocalLogId;				/* 本方动态ID */
	UINT8	DestLogId;				/* 对方动态ID */
	UINT8	RecvLinkStatus;			/* 链路接受状态 */
	UINT8	SendLinkStatus;			/* 链路发送状态 */
	UINT8	NoRecvDataCycleNum;		/* 无数据周期数 */
	UINT8	NoAppDataCycleNum;		/* 无发送应用数据周期数 */
	UINT16	DestBsdAdd;				/* BSD地址 */
	UINT16	RsspSrcAddr;			/* RSSP源地址 */
	UINT16	RsspDestAddr;			/* RSSP目的地址 */
	TimeStampStru_WL	TimeStmp;		/* 时间戳信息 */
	NodeParamInfoStru_WL	LocalNode;	/* 本方接受节点 */
} RsspLinkInfoStru_WL;

/* 通信节点结构体 */
typedef struct
{
	UINT32 CycleNum;				/*当前链路周期号*/
	UINT8 ExeCycle;					/*是否为执行周期*/
	UINT8 HasRsdBeforeSseOrSsr;					/*在生成SSE或者SSR之前是否有生成RSD*/
	UINT8 KeepSendSseOrHaltCycleNum;	/*保持SendSSE_WL或Halt_WL状态周期数*/

	UINT32	DestDevName;			/*对方唯一ID*/
	UINT16  DestTypeId;				/*对方类型ID*/
	UINT16	DestLogicId;			/*对方逻辑ID*/

	UINT8	NodeStatus;					/* 节点覆盖状态 */
	RsspLinkInfoStru_WL	RsspLinInfo;	/* RSSP链路基本信息 */	
} Lnk_Info_Node_Stru_WL;

/*收到各网包数*/
typedef struct
{
	UINT16 DstAddr;
	UINT16 NetsPkgNum[2];
}RsppiNetsRecvPkgNum;

/*--------------------------------------------------------------------
 * RSSP层内部使用结构体申明 End
 *--------------------------------------------------------------------*/
typedef struct
{
    UINT8 cpuId;                    /* cpuID,小型化双CPU交互SVC*/
	UINT8 LocalID;					/* 设备ID，由应用按文档在初始化时填写 */
	UINT8 LocalType;				/* 设备type，由应用按文档在初始化时填写 */
	UINT16 LocalLogicId;			/*本方逻辑ID*/
	UINT32 *CycleNum;				/* 周期号，由应用按文档在每周期开始时填写 */
	UINT8 MaxNumLink;				/* RSSP层同时通信的最大对象数，由应用按文档在初始化时填写 */
	UINT8 MaxNumPerCycle;			/* RSSP层一周期允许每个对象的最大有效数据，由应用按文档在初始化时填写 */
	UINT16 InputSize;				/* RSSP层接受数据中最大一帧的应用数据长度，由应用按文档在初始化时填写 */
	UINT16 OutputSize;				/* RSSP层发送数据中最大一帧的应用数据长度，由应用按文档在初始化时填写 */
	UINT8 *RecordArray;				/* RSSP层记录数组 */
	UINT16 RecordArraySize;			/* RSSP层记录数组大小 */
	UINT8 *AppArray;				/* 输出模块的出口1，表示外网输出数据 */
	UINT32 AppArraySize;			/* RSSP层输出模块的出口数组长度 */
	UINT8 *VarArray;				/* RSSP层输出模块的出口2，表示中间变量 */
	UINT16 VarArraySize;			/* 输出模块的出口数组长度 */
	QueueStruct DataToApp;			/* RSSP层协议输入给应用的数据队列 */
	UINT32 DataToAppSize;			/* 协议输入给应用的数据队列长度 */
	QueueStruct OutputDataQueue;	/* RSSP层应用给协议输出的数据队列 */
	UINT32 OutputDataQueueSize;		/* 应用给协议输出的数据队列长度 */
	QueueStruct OutnetQueueA;		/* RSSP层内网接收队列A(内网->冗余红网队列) */
	QueueStruct OutnetQueueB;		/* RSSP层内网接收队列B(内网->冗余蓝网队列) */
	UINT32 OutnetQueueSize;			/* 内网接收队列长度，由协议初始化时填写 */
     
    /* 由于单机多对象临时增加的变量 */
	UINT16 FrameDataMaxLen; /*单帧数据最大长度*/
	UINT8* FrameData; /*帧数据*/
	RsspStackElement_WL* StackOfDataLink;
	Lnk_Info_Node_Stru_WL* LnkNodeArr;			/* 通信节点数组 */
	UINT8 LnkNodeNum;							/* 通信节点数组长度 */
	DSU_RSSP_INFO_STRU_WL *pDsuRsspInfoStruWL;	/*RSSP配置信息*/
	UINT8 NoSendAppData;						/*输出时无应用给协议应用数据周期数*/
	UINT16 NetsRecvPkgNumsLen; /*各网收到包数数组长度*/
	RsppiNetsRecvPkgNum* NetsRecvPkgNums; /*各网收到包数数组*/
	UINT8* gpSendSingleOutMsg; /*发送单帧数据*/
} RSSP_INFO_STRU;
 
#ifdef __cplusplus
}
#endif


#endif

