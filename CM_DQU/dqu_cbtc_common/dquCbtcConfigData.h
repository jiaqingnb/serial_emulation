
/************************************************************************
*
* 文件名   ：
* 版权说明 ：  北京交控科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2011.12.27
* 作者     ：  研发中心软件部
* 功能描述 ：  系统配置数据结构体定义
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/

#ifndef DQU_CBTC_CONFIG_DATA
#define DQU_CBTC_CONFIG_DATA

#include "CommonTypes.h"
#include "dquDataTypeDefine.h"

/*为解决ats显示上下行问题新增一个字段---严广学2012-12-17*/
#define LINK_LOGIC_DIR_DOWN 0x55u	/*下行(link逻辑方向为线路下行方向)*/
#define LINK_LOGIC_DIR_UP 0xaau		/*上行(link逻辑方向为线路上行方向)*/

#define DQU_CBTC_VER1	66u			/*产品编号*/
#define DQU_CBTC_VER2	17u			/*软件模块编号*/
#define DQU_CBTC_VER3	0u			/*软件模块主版本编号*/
#define DQU_CBTC_VER4	2u			/*软件模块子版本编号*/

#ifdef __cplusplus
extern "C" {
#endif

/*============================================配置数据结构体结构体===================================================*/
/*列车车型配置信息结构体*/
typedef struct {
	UINT16  TrainName;    /*列车名称*/
	UINT8   TrainType;    /*列车车型*/
} CBTC_TRAIN_TYPE_CONGIG_STRU;

/*列车性能参数配置结构体*/
typedef struct {
	UINT8 TrainType;    /*列车车型*/
	UINT16 TrainLength; /*列车长度*/
	UINT8 TrainMaxTracAcc;	/*列车最大牵引加速度*/
	UINT16 WorstGrade;	    /*线路最大下坡坡度*/
} CBTC_TRAIN_INFO_STRU;

/*各子系统公用数据配置结构体*/
typedef struct {
	UINT16  MaxLineLimitSpeed;/*线路最高限速,此处数据库中存放的是UINT8的km/h数据，需要在这里转换成cmps单位的数据*/
	UINT8	MaxTsrSpeed;/*临时限速最高限速*/
	UINT8	TimeZoneDifference;/*NTP校时时区差.格林威治时间与本地时间的时区差，单位：时区，应用使用时应乘以3600转换为秒。*/
	UINT16	MABackDistance;/*安全防护距离.考虑到最不利情况情况下（最大潜在退行距离和线路最大坡度等）移动授权考虑的安全防护距离*/
	UINT32	VobcZcCommunicationErrorTime;/*VOBCZC通信故障判断时间*/
	UINT16	MaxTsrNum;/*一次设置临时限速的最大个数*/
	UINT16	MaxSectionOfTsr;/*一个临时限速报文中包含的最大逻辑区段数量*/
	UINT32	DsuZcCommunicationErrorTime;/*DSUZC通信故障判断时间*/
	UINT32	ZcZcCommunicationErrorTime;/*ZCZC通信故障判断时间*/
	UINT32	ZcCiCommunicationErrorTime;/*ZCCI通信故障判断时间*/
	UINT32	DsuAtsCommunicationErrorTime;/*DSUATS通信故障判断时间*/
	UINT32	ZcAtsCommunicationErrorTime;/*ZCATS通信故障判断时间*/
	UINT32	VobcCiCommunicationErrorTime;/*VOBCCI通信故障判断时间*/
	UINT32	VobcAtsCommunicationErrorTime;/*VOBCATS通信故障判断时间*/
	UINT16  MaxOverlapLength;/*系统定义最大保护区段长度（cm）*/
	UINT32   CICICommunicationErrorTime; /*CI-CI通信故障判断时间(ms)*/
	UINT32  CiVobcCommunicationErrorTime;/*CI-VOBC通信故障判断时间*/
	UINT32	CiZcCommunicationErrorTime; /*CI-ZC通信故障判断时间*/
	UINT32	CiPsdCommunicationErrorTime; /*CI-ZC通信故障判断时间*/
										 /*为解决ats显示上下行问题新增一个字段---严广学2012-12-17*/
	UINT8 LinkLogicDirUpAndDown;/*link逻辑方向上下行*/

	UINT16 SpanLineNum; /*有效跨线线路数量，互联互通需求新增， add by qxt 20160811*/
	UINT8 LineID[4];    /*线路编号，互联互通需求新增， add by qxt 20160811*/
	UINT8 OverlapLineLogDirUpAndDown[4];   /*重叠区内各线路的系统定义逻辑方向与上下行方向的关系，互联互通需求新增， add by qxt 20160811*/
	UINT16	MALengthForATOParking; /*满足精准停车条件的MA延伸长度，针对合库数据结构V16.0.0，系统配置数据各子系统公用数据增加字段“满足ATO精准停车条件的MA长度”，add by my 20170220*/
	UINT16  ZeroSpeedThreshold;		/*零速判断门限(cm/s)，依据互联互通数据结构V12.0.0添加，add by lmy 20180314*/
	UINT32  StartDistanceOfMSToNZ;		/*预告磁钢到中性区起点距离 MS=MagneticSteel  NZ=NeutralZone，依据互联互通数据结构V13.0.0添加，add by sds 20180614*/
	UINT8   CompatibleOfHLHTFAO;		/*互联互通FAO接口兼容字段，依据互联互通数据结构V13.0.0添加，add by sds 20180614*/
	UINT8   ValueOfLineDynamicTest;		/*正线是否动态测试字段，依据互联互通数据结构V13.0.0添加，add by sds 20180625*/
	UINT8   IsVOBCHandleObatacles;     /*是否由VOBC处理MA障碍物,依据互联互通数据结构V14.0.0添加，add by qxt 20180719*/
	UINT16  MaxTrainNumOfAwake;  /*允许同时进行动态测试(唤醒)的列车最大数量。,依据互联互通数据结构V14.0.0添加，add by qxt 20180719*/
	UINT16  TypeOfProject;  /*项目标识(区分工程接口),依据互联互通数据结构V15.0.0添加，add by sds 20181031*/
	UINT8  VersionOfZCToZC;  /*ZC-ZC接口协议版本号，add by sds 2019-1-21*/
	UINT8  VersionOfZCToVOBC;  /*ZC-VOBC接口协议版本号，add by sds 2019-1-21*/
	UINT8  VersionOfZCToDSU;  /*ZC-DSU接口协议版本号，add by sds 2019-1-21*/
	UINT8  VersionOfZCToCI;  /*ZC-CI接口协议版本号，add by sds 2019-1-21*/
	UINT8  VersionOfZCToATS;  /*ZC-ATS接口协议版本号，add by sds 2019-1-21*/
	UINT8  VersionOfVOBCToATS;  /*VOBC-ATS接口协议版本号，add by sds 2019-1-21*/
	UINT8  VersionOfVOBCToCI;  /*VOBC-CI接口协议版本号，add by sds 2019-1-21*/
							   /*车车专用 add by sds 2019-6-13*/
	UINT16	MaxSectionOfSpeed;/*一个临时限速报文中包含的最大限速区段数量*/
	UINT32	VobcVobcCommunicationErrorTime;/*VOBC-VOBC通信故障判断时间（ms）*/
	UINT32	VobcOcCommunicationErrorTime;/*VOBC-OC通信故障判断时间（ms）*/
	UINT32	VobcItsCommunicationErrorTime;/*VOBC-ITS通信故障判断时间（ms）*/
	UINT32	TmcItsCommunicationErrorTime;/*TMC-ATS通信故障判断时间（ms）*/
	UINT32	TmcOcCommunicationErrorTime;/*TMC-OC通信故障判断时间（ms）*/
	UINT32	ItsOcCommunicationErrorTime;/*ATS-OC通信故障判断时间（ms）*/
	UINT32	OcPsdCommunicationErrorTime;/*OC-PSD通信故障判断时间（ms）*/
	UINT32  VobcTslCommunicationErrorTime;/*VOBC-TSL通信故障判断时间（ms） by wyd 20210731*/
	UINT32  OcTslCommunicationErrorTime;/*OC-TSL通信故障判断时间（ms） by wyd 20210731*/
	UINT16  OppositeTrackDistance;    /*对向行驶双车间隔最小安全距离（cm）*/
	UINT16  StationTrackDistance;     /*站台追踪时，列车间隔距离(cm)*/
	UINT32  MaxMALength;              /*线路最差情况下紧急制动距离（单位m）*/
	UINT32  LineMinTrainLen;              /*线路最小可能车长（cm）*/
	UINT8  MainLineDynTestCfg;              /*正线是否支持动态测试*/
	UINT8  DynamicTestCfg;              /*动态测试开关*/
	UINT8  RemoteRMCfg;				      /*远程RM功能开关*/
	UINT32  RecoveryResDelay;             /*列车降级资源回收倒计时(ms)*/
									  /*车车专用-结束*/
} CBTC_CONFIG_DATA_STRU;


/*车辆段配置信息结构体*/
typedef struct {
	UINT8 DepotID;		/*车辆段编号*/
	UINT16 DepotCIID;	/*车辆段所属联锁ID*/
	UINT8 DepotDir;		/*车辆段出库方向*/
} CBTC_DEPOT_INFO_STRU;

/*CBTC配置数据信息*/
typedef struct CBTC_ConfigDataStruct
{
	CBTC_TRAIN_TYPE_CONGIG_STRU  cbtcTrainTypeConfig[200];    /*列车车型配置数据结构体数组，初始化函数完成填充*/
	UINT32 TrainTypeConfigLen;								 /*数组长度*/

	CBTC_TRAIN_INFO_STRU   cbtcTrainInfo[10];                 /*列车性能参数配置结构体数组，初始化函数完成填充*/
	UINT32   TrainInfoLen;									 /*数组长度*/

	CBTC_DEPOT_INFO_STRU   cbtcDepotInfo[10];                 /*车辆段配置信息结构体数组，初始化函数完成填充*/
	UINT32   DepotConfigLen;								/*数组长度*/

	CBTC_CONFIG_DATA_STRU  cbtcConfigData;                   /*各子系统公用数据配置结构体数组，初始化函数完成填充*/

}CBTC_CFG_DATA_STRU;

/*系统配置数据版本号获取*/
UINT32 GetDquCbtcVersion(void);

/*设置CBTC配置数据信息*/
UINT8 dquSetInitCbtcConfigInfoRef(CBTC_CFG_DATA_STRU *pCbtcConfigDataStru);

/*系统配置数据初始化*/
UINT8 dquCbtcConfigDataInit(CHAR* FSName);

/*系统配置数据初始化扩展接口*/
UINT8 dquCbtcConfigDataInitExp(UINT8 * pDataBuf, CBTC_CFG_DATA_STRU *cbtcConfigDataStru, UINT8 mode, UINT16 timeSlice);

/*编号对照表初始化完毕，执行编号对照表内部公共变量的清理工作*/
UINT8 dfsuCbtcConfigInitFinally(void);

/*设置当前查询函数据源*/
UINT8 dquSetCurCbtcCfgInfoQueryDataSource(CBTC_CFG_DATA_STRU *pCbtcConfigDataStru);

/*根据列车名称查询列出车型*/
UINT8 dquGetTrainType(UINT16 TrainName, UINT8 *pTrainType);

/*跟据列车车型获得列车信息*/
UINT8 dquGetCbtcTrainInfo(UINT8 TrainType, CBTC_TRAIN_INFO_STRU *pCbtcTrainInfo);

/*获取各子系统公用数据配置表*/
UINT8 dquGetCbtcConfigData(CBTC_CONFIG_DATA_STRU* pCbtcConfigDataStru);

/*获得车辆段配置信息 */
UINT8 dquGetCbtcDepotInfo(CBTC_DEPOT_INFO_STRU *pCbtcDepotInfo, UINT16 *Length);
/*============================================设备IP配置表结构体===================================================*/

/*源自FAO由15改为30，BY LJ,20170109*/
#define INIT_MAX_ZCDSU_COMM_OBJ_NUM_FAO 30u				/*zcdsu通信对象最大数量*/
/*20160120hxq*/
#define DSU_LEN_IP_ADDR  4u

#define DSU_MAX_DEST_NUM        10u						/*最大目标系个数*/
#define NTP_PORT                123u					/*NTP校时使用端口*/

/*结构体内部数据类型总和*/
#define INIT_IP_INFO_STRU_SIZE 30u 
#define INIT_COMM_PROTCL_STRU_SIZE 3u
#define INIT_CCOV_COMM_INFO_STRU_SIZE 12u
#define INIT_ZCDSU_COMM_INFO_STRU_SIZE_FAO (34u+30u)
#define INIT_GATEWAY_INFO_STRU_SIZE 16u
#define INIT_CI_ATS_IP_INFO_STRU_SIZE 30u
#define INIT_NET_PORT_INFO_STRU_SIZE 28u

typedef struct
{
	UINT8 RedIp[DSU_LEN_IP_ADDR]; /*红网IP地址*/
	UINT16 PortRed;   /*红网端口号*/
	UINT8 BlueIp[DSU_LEN_IP_ADDR]; /*蓝网IP地址*/
	UINT16 PortBlue;  /*蓝网端口号*/
} PROTCL_IP_PORT_STRU;

typedef struct {
	PROTCL_IP_PORT_STRU DstIpInfo[DSU_MAX_DEST_NUM];
	UINT8 dstNum;/*IP配置*/
} PROTCL_IP_INFO_STRU;

/*ip地址结构体*/
typedef struct
{
	UINT16 DevName;        /*设备标识符*/

	UINT32 HlhtID;		   /*互联互通ID*/

	UINT8 IpAddrBlue[DSU_LEN_IP_ADDR];      /*蓝网ip地址*/
	UINT8 IpAddrRed[DSU_LEN_IP_ADDR];       /*红网ip地址*/

	UINT16 SfpBluePort;    /*Sfp本地蓝网端口号*/
	UINT16 SfpRedPort;     /*Sfp本地红网端口号*/

	UINT16 RpBluePort;     /*Rp本地蓝网端口号*/
	UINT16 RpRedPort;     /*Rp本地红网端口号*/

	UINT16 RsspBluePort;     /*Rssp本地蓝网端口号*/
	UINT16 RsspRedPort;     /*Rssp本地红网端口号*/

	UINT16 DeviceSoureBluePort;		/* 设备源蓝网端口号 */
	UINT16 DeviceSoureRedPort;		/* 设备源红网端口号 */

}DSU_IP_INFO_STRU;

/*设备通信协议表*/
typedef struct
{
	UINT8 EmitterType;	/*发送设备类型*/
	UINT8 ReceptorType;	/*接受设备类型*/
	UINT8 CommuType;	/*所使用的通信协议，1为Sfp，2为Rp，3为Rsr，4为RSSPI,9为RSSPII*/

}DSU_COMM_PROTCL_INFO_STRU;

/*CCOV通信关系配置表信息*/
typedef struct
{
	UINT16 DeviceName_VOBC;	/*2	VOBC设备类型+设备ID*/
	UINT16 	DeviceName_WGB;	/*2	WGB设备类型+设备ID*/
	UINT16 	DeviceName_TimeServer;	/*2	信号系统时钟服务器设备类型+设备ID*/
	UINT16 	DeviceName_NMS;	/*2	NMS设备类型+设备ID*/
	UINT16 	NMS_BluePort;	/*2	CCOV与NMS通信蓝网端口*/
	UINT16 	NMS_RedPort;	/*2	CCOV与NMS通信红网端口*/

}DSU_CCOV_COMM_INFO_STRU;

/*地面通信关系配置表信息*/
typedef struct
{
	UINT16 LocalDevName;    /*本设备编号	2	包含设备类型与ID*/
	UINT16 CommObjNum;	    /*通信对象数量	2	暂定最大数量为15*/
	UINT16 CommObjBuf[INIT_MAX_ZCDSU_COMM_OBJ_NUM_FAO];	/*通信对象1编号	2	包含设备类型与ID*/
}DSU_ZCDSU_COMM_INFO_STRU;

/*设备网关表信息*/
typedef struct
{
	UINT16  Index;	                                   /*2	编号*/
	UINT16   DeviceType;	                           /*1	设备类型*/
	UINT8   IpAddrGatewayBlue[DSU_LEN_IP_ADDR];      /*4	设备蓝网网关*/
	UINT8   IpAddrGatewayRed[DSU_LEN_IP_ADDR];       /*4	设备红网网关*/
	UINT8   IpAddrSubnetMask[DSU_LEN_IP_ADDR];       /*4	子网掩码*/

}DSU_GATEWAY_INFO_STRU;

/*CI_ATS网设备IP地址信息*/
typedef struct
{
	UINT16 DevName;        /*设备标识符*/

	UINT8 IpAddrBlue[DSU_LEN_IP_ADDR];             /*蓝网ip地址*/
	UINT8 BlueSubNetMask[DSU_LEN_IP_ADDR];         /*蓝网子网掩码*/

	UINT8 IpAddrRed[DSU_LEN_IP_ADDR];             /*红网ip地址*/
	UINT8 RedSubNetMask[DSU_LEN_IP_ADDR];         /*红网子网掩码*/

	UINT16 SfpBluePort;    /*Sfp本地蓝网端口号*/
	UINT16 SfpRedPort;     /*Sfp本地红网端口号*/

	UINT16 RpBluePort;     /*Rp本地蓝网端口号*/
	UINT16 RpRedPort;     /*Rp本地红网端口号*/

	UINT16 RsspBluePort;     /*Rssp本地蓝网端口号*/
	UINT16 RsspRedPort;     /*Rssp本地红网端口号*/

}DSU_CI_ATS_IP_INFO_STRU;

/*端口表信息*/
typedef struct
{
	UINT16 Index;					/*编号*/
	UINT32 EmitterType;				/*源设备类型（或互联互通ID）*/
	UINT32 ReceptorType;			/*目的设备类型（或互联互通ID）*/
	UINT8 EmitterFactory;			/*源厂商编号*/
	UINT8 ReceptorFactory;			/*目的厂商编号*/

	UINT16 Tc1RedEmitterPort;		/*TC1红网发送端口号*/
	UINT16 Tc1RedReceptorPort;		/*TC1红网接收端口号*/
	UINT16 Tc1BlueEmitterPort;		/*TC1蓝网发送端口号*/
	UINT16 Tc1BlueReceptorPort;		/*TC1蓝网接收端口号*/
	UINT16 Tc2RedEmitterPort;		/*TC2红网发送端口号*/
	UINT16 Tc2RedReceptorPort;		/*TC2红网接收端口号*/
	UINT16 Tc2BlueEmitterPort;		/*TC2蓝网发送端口号*/
	UINT16 Tc2BlueReceptorPort;		/*TC2蓝网接收端口号*/
}DSU_PORT_INFO_STRU;


typedef struct
{
	/*设备IP地址*/
	UINT16 IpInfoStruLen;
	DSU_IP_INFO_STRU *pIpInfoStru;

	/*设备通信协议表*/
	UINT16 CommProtclStruLen;
	DSU_COMM_PROTCL_INFO_STRU *pCommProtclStru;

	/*CCOV通信关系配置表*/
	UINT16 CcovCommInfoLen;
	DSU_CCOV_COMM_INFO_STRU* pCcovCommInfoStru;

	/*地面设备通信关系配置表*/
	UINT16 ZcdsuCommInfoLen;
	DSU_ZCDSU_COMM_INFO_STRU* pZcdsuCommInfoStru;

	/*设备网关表*/
	UINT16 GatewayInfoLen;
	DSU_GATEWAY_INFO_STRU* pGatewayInfoStru;

	/*CI_ATS网设备IP地址*/
	UINT16 CIAtsIpInfoLen;
	DSU_CI_ATS_IP_INFO_STRU* pCIAtsIpInfoStru;

	/*CI_ATS网设备IP地址*/
	UINT16 PortInfoLen;
	DSU_PORT_INFO_STRU* pPortInfoStru;

}DSU_IP_CONFIG_INFO_STRU;

/*设置设备IP配置数据信息*/
UINT8 dquSetInitIpConfigInfoRef(DSU_IP_CONFIG_INFO_STRU *pIPConfigDataStru);

/*设备IP配置数据初始化*/
UINT8 dquIpConfigDataInit(UINT8* FSName);

/*设置设备IP配置数据查询函数据源*/
UINT8 dquSetCurIPCfgInfoQueryDataSource(DSU_IP_CONFIG_INFO_STRU *pIPConfigDataStru);

/*查询设备的通信数量和通信对象*/
UINT8 CheckCommObjectInfo(UINT16 wLocalID, UINT8 chLocalType, UINT32 * wObjectNum, UINT16 *pwObjectInfo);

/*查询设备IP*/
UINT8 CheckIPInfoOfObject(UINT16 wLocalID, UINT8 chLocalType, UINT32 * RedIPInfo, UINT32 * BlueIPInfo);

/*根据红网的IP地址和端口号获取设备名称*/
UINT8 GetRedNetDevNameAcordIpAndPort(UINT8* ipAdd, UINT16 port, UINT16* devName);

/*根据蓝网的IP地址和端口号获取设备名称*/
UINT8 GetBlueNetDevNameAcordIpAndPort(UINT8* ipAdd, UINT16 port, UINT16* devName);


#ifdef __cplusplus
}
#endif

#else
#endif
