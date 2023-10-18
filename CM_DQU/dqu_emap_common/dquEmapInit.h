/************************************************************************
*
* 文件名   ：  dquEmapInit.h
* 版权说明 ：  北京交控科技有限公司
* 版本号   ：  1.0
* 创建时间 ：
* 作者     ：  软件部
* 功能描述 ：  电子地图初始化定义
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/
#ifndef  DQU_EMAP_INIT_H
#define  DQU_EMAP_INIT_H

#include "string.h"
#include "Convert.h"
#include "dfsDataRead.h"
#include "dsuVar.h"
#include "dquStructInit.h"
#include "dquQueryExp.h"
#include "dquDataTypeDefine.h"
#include "CommonMemory.h"

#define  INIT_ERR_NUM   100u			/*依据互联互通数据结构V15.0.0添加表单后需要将此值增大，由50改为100，便于后期扩展，modify by sds 20181116*/

extern UINT8  initErrNo[INIT_ERR_NUM];	/*各数据初始化失败故障号数组*/

#ifndef DEBUG_GET_DATA_FAIL
#define DEBUG_GET_DATA_FAIL    0x01u  /*获取数据失败*/
#endif

#ifndef DEBUG_DATA_LENTH_FAIL
#define DEBUG_DATA_LENTH_FAIL  0x02u  /*数据长度有误*/
#endif

#ifndef DEBUG_CALC_FAIL
#define DEBUG_CALC_FAIL        0x04u  /*计算过程错误*/
#endif

#define DQU_EMAP_VER1	66u				/*产品编号*/
#define DQU_EMAP_VER2	17u				/*软件模块编号*/
#define DQU_EMAP_VER3	0u				/*软件模块主版本编号*/
#define DQU_EMAP_VER4	0u				/*软件模块子版本编号*/

/*新增各表每行字节数的判断 wyd 20230509*/
#define DQU_ROW_LEN_LINK			36U			/*Link表每行字节数*/
#define DQU_ROW_LEN_POINT			24U			/*道岔表每行字节数*/
#define DQU_ROW_LEN_SIGNAL			20U			/*信号机表每行字节数*/
#define DQU_ROW_LEN_BALISE			468U		/*应答器表每行字节数*/
#define DQU_ROW_LEN_AXLE			14U			/*计轴器表每行字节数*/
#define DQU_ROW_LEN_SCREEN			9U			/*屏蔽门表每行字节数*/
#define DQU_ROW_LEN_ESB				4U			/*紧急停车按钮表每行字节数*/
#define DQU_ROW_LEN_STATION			42U			/*停车区域表每行字节数*/
#define DQU_ROW_LEN_STOPPOINT		34U			/*停车点表每行字节数*/
#define DQU_ROW_LEN_AXLESGMT		132U		/*计轴区段表每行字节数*/
#define DQU_ROW_LEN_LOGICSGMT		14U			/*逻辑区段表每行字节数*/
#define DQU_ROW_LEN_ROUTE			126U		/*进路表每行字节数*/
#define DQU_ROW_LEN_PROTSGMT		12U			/*保护区段表每行字节数*/
#define DQU_ROW_LEN_BLOCACCESS		24U			/*点式接近区段表每行字节数*/
#define DQU_ROW_LEN_CBTCACCESS		44U			/*CBTC接近区段表每行字节数*/
#define DQU_ROW_LEN_GRADE			34U			/*坡度表每行字节数*/
#define DQU_ROW_LEN_STRESPEED		16U			/*静态限速表每行字节数*/
#define DQU_ROW_LEN_RUNLEVEL		44U			/*站间运行等级表每行字节数*/
#define DQU_ROW_LEN_EXITROUTE		60U			/*退出CBTC区域表每行字节数*/
#define DQU_ROW_LEN_TRAINLINE		16U			/*非CBTC区域表每行字节数*/
#define DQU_ROW_LEN_AR				16U			/*无人折返表每行字节数*/
#define DQU_ROW_LEN_CI				10U			/*CI表每行字节数*/
#define DQU_ROW_LEN_ATS				8U			/*ATS表每行字节数*/
#define DQU_ROW_LEN_CONRESPEED		4U			/*线路统一限速表每行字节数*/
#define DQU_ROW_LEN_CONGRADE		4U			/*线路统一坡度表每行字节数*/
#define DQU_ROW_LEN_PHYSGMT			2U			/*物理区段表每行字节数*/
#define DQU_ROW_LEN_FG				9U			/*防淹门表每行字节数*/
#define DQU_ROW_LEN_SPKS			206U		/*SPKS表每行字节数*/
#define DQU_ROW_LEN_DEPOTGATE		14U			/*车库门表每行字节数*/
#define DQU_ROW_LEN_TUNNEL			0U			/*隧道表每行字节数*/
#define DQU_ROW_LEN_POWERLESS		0U			/*分相区表每行字节数*/
#define DQU_ROW_LEN_CARSTOP			0U			/*车挡表每行字节数*/
#define DQU_ROW_LEN_EVACUATE		67U			/*疏散侧表每行字节数*/

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************
*函数功能：电子地图模块版本号获取
*参数说明：供其它系统调用查询函数版本号，防止集成错误。
*入口参数：无
*出口参数：无
*返回值：UINT32表示的版本号
*********************************************/
UINT32 GetDquEmapVersion(void);

/*初始化电子地图*/
UINT8 dfsEmapInit(CHAR * FSName);

/****************************************
*函数功能：初始化电子地图扩展接口
*参数说明：pDataBuf,传入数据块，
*	   dsuEmapStru,外部实体电子地图存储结构体指针，
*	   dsuLimitStcLinkIdx,外部实体静态限速link结构体指针，
*	   mode,处理模式，1上电一次加载，2运行时多次处理
*	   timeSlice，执行的时间片（单位：毫秒）
*返回值：  0失败，1成功，2分步处理未完成
*****************************************/
UINT8 dfsEmapInitExp(UINT8 * pDataBuf, DSU_EMAP_STRU *dsuEmapStru, DSU_STC_LIMIT_LINKIDX_STRU *dsuLimitStcLinkIdx, UINT8 mode, UINT16 timeSlice);


/*电子地图初始化完毕，执行电子地图内部公共变量的清理工作*/
UINT8 dfsEmapInitFinally(void);

/************************************************************
*函数功能：计算计轴区段增加项
*参数说明：无
*返回值：0失败，1成功
*
计轴区段结构体需增加项

UINT16  wOrgnRelatPointId;					计轴区段起点关联道岔编号
UINT16  wOrgnMainAxleSgmtId;					计轴区段起点连接定位计轴区段编号
UINT16  wOrgnSideAxleSgmtId;					计轴区段起点连接反位计轴区段编号
UINT16  wTmnlRelatPointId;					计轴区段终点关联道岔编号
UINT16  wTmnlMainAxleSgmtId;					计轴区段终点连接定位计轴区段编号
UINT16  wTmnlSideAxleSgmtId;					计轴区段终点连接反位计轴区段编号
UINT16  wRelatAxleSgmtNum;					关联计轴区段数目
UINT16  wRelatAxleSgmtId[3];					关联计轴区段编号
UINT16  wManageZC;							所属ZC区域编号
UINT16  wManagePhysicalCI;					所属物理CI区域编号
UINT32 dwOrgnLinkOfst;							起点所处link偏移量
UINT32 dwTmnlLinkOfst;							终点所处link偏移量
UINT16  wMiddleLinkId;							计轴区段中间包含link编号
************************************/
UINT8 AxleSgmtStru(void);

/************************************************************
*函数功能：计算逻辑区段增加项
*参数说明：无
*返回值：0失败，1成功
*   UINT16  wMiddleLinkId;						逻辑区段中间包含link编号
UINT16  wMainOrgnLogicSgmtId;				所属计轴区段内前向逻辑区段的编号
UINT16  wMainTmnlLogicSgmtId;				所属计轴区段内后向逻辑区段的编号
************************************/
UINT8 LogicSgmtStru(void);

/*********************************************
*函数功能：进路表数据结构
*参数说明：无
*返回值：0，失败；1，成功
UINT16  wIncludeAxleSgmtNum;				进路包含的计轴区段数目
UINT16  wIncludeAxleSgmtId[8];				包含的计轴区段编号
UINT16  wAxleSgmtLockStatus[8];				包含的计轴区段锁闭状态

UINT16  wObsNum;							进路包含障碍物数目
UINT16  wObsType[8];							障碍物类型
UINT16  wObsID[8];								障碍物ID
UINT16	wObsNeedLockStatus[8];					障碍物需要联锁状态
*********************************************/
UINT8 RouteStru(void);

/*********************************************
*函数功能：完成保护区段添加项
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 ProtectLink(void);

/*********************************************
*函数功能：计算坡度
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 Grade(void);

/*********************************************
*函数功能：计算默认行车序列表各项
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 DefaultRouteStru(void);
#if 0
/*********************************************
*函数功能：根据接近区段ID，获取接近区段包含的道岔及状态
*参数说明：
输入参数：AccessLinkId 接近区段ID
输出参数：RelatePointNum 道岔个数
RelatePointId[] 道岔ID
RelatePointSta[] 道岔状态
*返回值 ： 0 失败，1成功
*********************************************/
UINT8 GetPointInfoFromAccessLink(UINT16 AccessLinkId, UINT16 Dirction, UINT16 * RelatePointNum, UINT16  RelatePointId[], UINT16 RelatePointSta[]);
#endif
/**************************************
函数功能：根据link序列获得计轴区段编号序列
输入参数：linkId 序列首地址，LinkNum link个数
输出参数：AxleSgmtId 计轴区段编号数组地址
AxleSgmtNum 数组长度
返回值：0失败，1成功
***************************************/
UINT8 GetAxleSemtIdFromLinkId(UINT16* linkId, UINT16 LinkNum, UINT16 *AxleSgmtId, UINT16 * AxleSgmtNum);

/*********************************************
*函数功能：根据进路link序列，计算障碍物信息
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 GetObstacleFromRoute(DSU_ROUTE_STRU*  pRouteStru);

/*********************************************
*函数功能：计算停车点信息
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 CalculateStopPoint(void);

/**********************************************
函数功能：初始化[任务数组中占位的表]
输入参数：无
返回值:1成功
**********************************************/
UINT8 initNopCalcTask(void);

/**********************************************
函数功能：初始化[任务数组中占位的表]
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:1成功
**********************************************/
UINT8 initNop(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[link数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initLink(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[道岔数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPoint(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[信号机数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initSignal(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[应答器数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initBalise(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[计轴器数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initAxle(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[屏蔽门数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initScreen(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[紧急停车按钮数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initEmergStop(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[停车区域数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStation(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[停车点数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStopPoint(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[计轴区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initAxleSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[逻辑区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initLogicSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[进路数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initRoute(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[保护区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initProtectLink(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[点式接近区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initBlocAccessSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);


/**********************************************
函数功能：初始化[CBTC接近区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initCBTCAccessLink(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[坡度信息数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initGrade(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[静态限速信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStaticResSpeed(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[站间运营级]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initRunLevel(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[退出CBTC区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initExitRoute(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[非CBTC区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initTrainLine(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[自动折返]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initAR(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[CI信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initCI(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[ATS信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initATS(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[统一限速信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initConResSpeed(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[统一坡度信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initConGrade(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[物理区段信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPhysicalSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[防淹门信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initFloodGate(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[SPKS开关信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initSpksButton(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[车库门信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initDepotGate(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[隧道信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initTunnel(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[分相区表信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPowerlessZone(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**********************************************
函数功能：初始化[车档表信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initCarStop(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);

/**************************************************************************
* 功能描述:		初始化疏散侧表
* 输入参数:		UINT8 *pDataAddr, 数据内存地址
				UINT32 DataSize,  数据表数据总字节数
				UINT16 rowCount,  数据表数据总行数
* 输入输出参数:	无
* 输出参数:		无
* 全局变量:
* 返回值:		2未处理完，1成功，0失败
* 修改记录:
* 1、依据互联互通数据结构V28.0.0.1添加，add by hzj 2023-3-1
**************************************************************************/
UINT8 initEvacuationSide(UINT8* pDataAddr, UINT32 dataSize, UINT16 rowCount);

/***
函数功能：获取故障码
输入参数：无
输出参数：errRecord故障码数组，errRecordLen数组长度
返回值：1成功
***/
UINT8 dquGetErrNo(UINT8* errRecord, UINT32 * errRecordLen);

/************************************************
函数功能：静态限速信息对应的插入以Link为索引的表中，
用于优化查询当前LINK相关限速索引使用。
输入参数：
@linkId:当前静态限速表索引编号对应所在LINK编号，即pCurrentStaticResSpeedStru->wLinkId
@limitStcSpdId:当前静态限速表索引编号，即pCurrentStaticResSpeedStru->wId
输出参数：无
返回值：1成功 0失败
************************************************/
UINT8 InsertToLimitStcLinkIdx(UINT16 linkId, UINT16 limitStcSpdId);

/*
函数功能：根据道岔表和多开道岔表获取所有实体道岔
参数说明：无
返回值：1成功， 0失败
author:单轨项目 qxt 20170626
*/
UINT8 GetPhyiscalSwitches(void);

/*
函数功能：建立Link和其他设备间的映射关系
参数说明：无
返回值：1成功， 0失败
author: qxt 20170816
*/
UINT8 GetDeviceInfoInLink(void);

/*************************************************
函数功能：建立Link和坡度间的映射关系
坡度ID（即坡度索引）对应的插入以Link为索引的表中，
用于优化查询当前LINK相关坡度索引。
参数说明：无
返回值：2表示当前没有处理完，需要再次调用此函数； 1表示成功； 0表示失败
author: lmy 20180122
*************************************************/
UINT8 GetGradeInfoCrossLink(void);

/*
函数功能：计算SPKS包含的Link序列
参数说明：无
返回值：2表示当前没有处理完，需要再次调用此函数； 1表示成功； 0表示失败
author: qxt 20180716
*/
UINT8 CalcSPKSIncLinks(void);

/*计算物理区段包含的link序列*/
UINT8 CalcPhySgmtIncLinks(void);

/*计算非CBTC区域包含的道岔*/
UINT8 CalcNonCbtcAreaIncPoints(void);

/*
函数功能：删除数组中重复的数，内部使用函数
参数说明：输入参数：input 输入数组；len 输入数组大小；
输出参数:去重后的数组
返回值：去重后数组的长度
author: qxt 20180716
修改：yt 2021年9月11日 将声明挪到对应.h，因为外部有引用
*/
UINT8 RemoveRep(UINT16 input[], UINT8 len, UINT16 output[]);

#ifdef __cplusplus
}
#endif
#endif 
