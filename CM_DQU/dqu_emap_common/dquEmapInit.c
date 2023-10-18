/************************************************************************
*
* 文件名   ：  dquEmapInit.c
* 版权说明 ：  北京交控科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2011.12.05
* 作者     ：  软件部
* 功能描述 ：  电子地图初始化处理过程
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/
#ifdef ZCAPP_VXWORKS_CODECONTROL
#include <vxWorks.h>
#endif /*End of ZCAPP_VXWORKS_CODECONTROL*/

#include <string.h>
#include "dquEmapInit.h"
#include "dsuStruct.h"
#include "CbtcVersion.h"
#include "dquCbtcType.h"
#include "dquQuery.h"

UINT8	dfsEmapInitFlag = 0u;
UINT8	initErrNo[INIT_ERR_NUM];      /*各数据初始化失败故障号数组*/

static dfsDataHead g_DFsDH;			/*FS文件的数据头结构体*/
static UINT8*  g_pDataAddr;         /*FS单元数据地址*/
static UINT32  g_DataSize;          /*FS单元数据大小（字节数）*/
static UINT16  g_RowCount;			/*表数据行数*/

static UINT16	g_wCurInitStruProg = 0;/*当前结构体初始化进程，分步加载时使用*/
static UINT16	g_wCurCalcStruProg = 0;/*当前结构体计算处理进程，分步加载时使用*/
static UINT16   g_wCurMainIndex = 0;  /*当前主处理索引，分步加载时使用*/
static UINT8	g_EmapInitMode = 0x01; /*电子地图初始化模式*/
static UINT8	g_IsEmapAllocMemory = 0x01;  /*如果调用文件访问接口上电初始化，分配内存*/
extern UINT16	g_MaExtendLength;		/*满足精确停车条件的MA延伸长度，从系统配置数据获取*/

/*----------内部函数声明开始--------------*/
/*道岔表需增加wManageZC，wManagePhysicalCI，wManageLogicCI，通过LINK来获取*/
UINT8 CalcSwitchStruExpItem(void);
/*信号机表需增加wManageZC，wManagePhysicalCI，wManageLogicCI，通过LINK来获取*/
UINT8 CalcSignalStruExpItem(void);
/*安全屏蔽门表需增加wManageLogicCI，通过LINK来获取*/
UINT8 CalcPSDoorStruExpItem(void);
/*紧急停车按钮需增加wManageLogicCI，通过LINK来获取*/
UINT8 CalcEmergStopBtnStruExpItem(void);
/*停车区域需增加dwStatTmnlLinkOfst，wManagePhysicalC，wManageLogicCI，通过LINK来获取*/
UINT8 CalcStopStationStruExpItem(void);
/*处理无人折返进入数据处理*/
UINT8 ARInStru(void);
/*处理无人折返退出数据处理*/
UINT8 AROutStru(void);


/*----------内部函数声明结束--------------*/

/*初始化任务行为调度表*/
static INIT_TASK_TAB_STRU g_struInitTaskTab[] =
{
	{ DQU_QFUN_TRUE, EMAP_NOP_0,				initNop,			5u, 1u, 0u },	/*为使数据表序号与该数据表在数组中所占位置相同，补全数据 ID  sds --0 */		
	{ DQU_QFUN_TRUE, EMAP_LINK_ID,				initLink,			5u, 1u, 0u },	/*link数据ID--1	*/										
	{ DQU_QFUN_TRUE, EMAP_POINT_ID,				initPoint,			5u, 1u, 0u },	/*道岔数据ID--2	*/							
	{ DQU_QFUN_TRUE, EMAP_SIGNAL_ID,			initSignal,			5u, 1u, 0u },	/*信号机数据ID--3*/						
	{ DQU_QFUN_TRUE, EMAP_BALISE_ID,			initBalise,			5u, 1u, 0u },	/*应答器数据ID--4*/						
	{ DQU_QFUN_TRUE, EMAP_AXLE_ID,				initAxle,			5u, 1u, 0u },	/*计轴器数据ID--5*/							
	{ DQU_QFUN_TRUE, EMAP_SCREEN_ID,			initScreen,			5u, 1u, 0u },	/*安全屏蔽门数据ID--6*/						
	{ DQU_QFUN_TRUE, EMAP_EMERG_STOP_BUTTON_ID,	initEmergStop,		5u, 1u, 0u },	/*紧急停车按钮数据ID--7*/						
	{ DQU_QFUN_TRUE, EMAP_STATION_ID,			initStation,		5u, 1u, 0u },	/*停车区域数据ID--8	*/						
	{ DQU_QFUN_TRUE, EMAP_STOPPOINT_ID,			initStopPoint,		5u, 1u, 0u },	/*停车点数据ID--9*/							
	{ DQU_QFUN_TRUE, EMAP_AXLE_SGMT_ID,			initAxleSgmt,		5u, 1u, 0u },	/*计轴区段数据ID--10*/	
	{ DQU_QFUN_TRUE, EMAP_LOGIC_SGMT_ID,		initLogicSgmt,		5u, 1u, 0u },	/*逻辑区段 --11*/
	{ DQU_QFUN_TRUE, EMAP_ROUTE_ID,				initRoute,			5u, 1u, 0u },	/*进路数据ID--12*/							
	{ DQU_QFUN_TRUE, EMAP_PROTECT_LINK_ID,		initProtectLink,	5u, 1u, 0u },	/*保护区段数据ID--13*/						
	{ DQU_QFUN_TRUE, EMAP_BLOC_ACCESS_LINK_ID,	initBlocAccessSgmt,	5u, 1u, 0u },	/*点式接近区段数据ID--14*/
	{ DQU_QFUN_TRUE, EMAP_CBTC_ACCESS_LINK_ID,	initCBTCAccessLink,	5u, 1u, 0u },	/*CBTC接近区段数据ID--15*/	                
	{ DQU_QFUN_TRUE, EMAP_GRADE_ID,				initGrade,			5u, 1u, 0u },	/*坡度数据ID--16*/							
	{ DQU_QFUN_TRUE, EMAP_STATIC_RES_SPEED_ID,	initStaticResSpeed,	5u, 1u, 0u },	/*静态限速数据ID--17*/						
	{ DQU_QFUN_TRUE, EMAP_DEFAULT_ROUTE_ID,		initNop,			5u, 1u, 0u },	/*默认行车序列数据ID--18*/
	{ DQU_QFUN_TRUE, EMAP_RUN_LEVEL_ID,			initRunLevel,		5u, 1u, 0u },	/*站间运营级别数据ID--19*/					
	{ DQU_QFUN_TRUE, EMAP_EXIT_ROUTE_ID,		initExitRoute,		5u, 1u, 0u },	/*退出CBTC区域数据ID--20*/	                
	{ DQU_QFUN_TRUE, EMAP_TRAIN_LINE_ID,		initTrainLine,		5u, 1u, 0u },	/*非CBTC区域数据ID--21*/					
	{ DQU_QFUN_TRUE, EMAP_AR_ID,				initAR,				5u, 1u, 0u },	/*自动折返数据ID--22*/						
	{ DQU_QFUN_TRUE, EMAP_ZC_DIVID_POINT_ID,	initNop,			5u, 1u, 0u },	/*ZC分界点数据ID--23*/
	{ DQU_QFUN_TRUE, EMAP_ZC_COM_ZONE_ID,		initNop,			5u, 1u, 0u },	/*ZC共管区域数据ID--24*/
	{ DQU_QFUN_TRUE, EMAP_CI_ID,				initCI,				5u, 1u, 0u },	/*CI数据ID--25*/							
	{ DQU_QFUN_TRUE, EMAP_ATS_ID,				initATS,			5u, 1u, 0u },	/*ATS数据ID--26*/							
	{ DQU_QFUN_TRUE, EMAP_CONRESSPEED_ID,		initConResSpeed,	5u, 1u, 0u },	/*统一限速数据ID--27*/						
	{ DQU_QFUN_TRUE, EMAP_CONGRADE_ID,			initConGrade,		5u, 1u, 0u },	/*统一坡度数据ID--28*/						
	{ DQU_QFUN_TRUE, EMAP_PHYSICAL_SGMT_ID,		initPhysicalSgmt,	5u, 1u, 0u },	/*物理区段数据ID--29*/						
	{ DQU_QFUN_TRUE, EMAP_BLOC_TRG_AREA_ID,		initNop,			5u, 1u, 0u },	/*点式触发区段数据ID--30*/
	{ DQU_QFUN_TRUE, EMAP_CBTC_TRG_AREA_ID,		initNop,			5u, 1u, 0u },	/*CBTC触发区段数据ID--31*/
	{ DQU_QFUN_TRUE, EMAP_PLAT_FORM_ID,			initNop,			5u, 1u, 0u },	/*站台数据ID--32*/
	{ DQU_QFUN_TRUE, EMAP_COME_GO_ID,			initNop,			5u, 1u, 0u },	/*往复进路表--33*/
	{ DQU_QFUN_TRUE, EMAP_FLOOD_GATE_ID,		initFloodGate,		5u, 1u, 0u },	/*防淹门--34*/								
	{ DQU_QFUN_TRUE, EMAP_SPKS_BUTTON_ID,		initSpksButton,		5u, 1u, 0u },	/*SPKS按钮--35*/							
	{ DQU_QFUN_TRUE, EMAP_DEPOT_GATE_ID,		initDepotGate,		5u, 1u, 0u },	/*车库门--36*/								
	{ DQU_QFUN_TRUE, EMAP_TUNNEL_ID,			initTunnel,			5u, 1u, 0u },	/*隧道--37*/								
	{ DQU_QFUN_TRUE, EMAP_ZC_ID,				initNop,			5u, 1u, 0u },	/*ZC数据ID--38*/
	{ DQU_QFUN_TRUE, EMAP_MULTI_POINT_ID,		initNop,			5u, 1u, 0u },	/*多开道岔数据ID--39*/
	{ DQU_QFUN_TRUE, EMAP_NOP_40,				initNop,			5u, 1u, 0u },	/*补全数据 ID  sds--40*/	
	{ DQU_QFUN_TRUE, EMAP_POWERLESSZONE_ID,		initPowerlessZone,	5u, 1u, 0u },	/*依据互联互通数据结构V12.0.0添加--41*/		
	{ DQU_QFUN_TRUE, EMAP_NOP_42,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --42*/
	{ DQU_QFUN_TRUE, EMAP_NOP_43,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --43*/
	{ DQU_QFUN_TRUE, EMAP_NOP_44,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --44*/
	{ DQU_QFUN_TRUE, EMAP_OC_ID,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --45*/
	{ DQU_QFUN_TRUE, EMAP_ITS_ID,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --46*/
	{ DQU_QFUN_TRUE, EMAP_NOP_47,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --47*/					
	{ DQU_QFUN_TRUE, EMAP_NOP_48,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --48*/					
	{ DQU_QFUN_TRUE, EMAP_NOP_49,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --49*/					
	{ DQU_QFUN_TRUE, EMAP_NOP_50,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --50*/					
	{ DQU_QFUN_TRUE, EMAP_CARSTOP_ID,			initCarStop,		5u, 1u, 0u },	/*依据互联互通数据结构V15.0.0添加--51*/	
	{ DQU_QFUN_TRUE, EMAP_NOP_52,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --52*/
	{ DQU_QFUN_TRUE, EMAP_NOP_53,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --53*/							
	{ DQU_QFUN_TRUE, EMAP_NOP_54,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --54*/							
	{ DQU_QFUN_TRUE, EMAP_NOP_55,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --55*/							
	{ DQU_QFUN_TRUE, EMAP_NOP_56,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --56*/							
	{ DQU_QFUN_TRUE, EMAP_NOP_57,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --57*/							
	{ DQU_QFUN_TRUE, EMAP_EVACUATION_ID,		initEvacuationSide,	5u, 1u, 0u },	/*依据互联互通数据结构V28.0.0.1添加 --58 */						
	{ DQU_QFUN_TRUE, EMAP_NOP_59,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --59*/							
	{ DQU_QFUN_TRUE, EMAP_NOP_60,				initNop,			5u, 1u, 0u },	/*补全数据 ID sds --60*/							 							
};

/*初始化任务登记表项目数量*/
static const UINT16 MAX_INITTASK_COUNT = sizeof(g_struInitTaskTab) / sizeof(INIT_TASK_TAB_STRU);


/*计算任务行为调度表*/
static CALC_TASK_TAB_STRU g_struCalcTaskTab[] =
{
	{ DQU_QFUN_TRUE, TASK_TAB_NOP_0					, initNopCalcTask,				5u, 1u, 0u, 0u },/*0--无*/
	{ DQU_QFUN_TRUE, TASK_TAB_SWITCH_STRU			, CalcSwitchStruExpItem,		5u, 1u, 0u, 0u },/*1--道岔*/
	{ DQU_QFUN_TRUE, TASK_TAB_SIGNAL_STRU			, CalcSignalStruExpItem,		5u, 1u, 0u, 0u },/*2--信号机*/
	{ DQU_QFUN_TRUE, TASK_TAB_PSD_STRU				, CalcPSDoorStruExpItem,		5u, 1u, 0u, 0u },/*3--屏蔽门*/
	{ DQU_QFUN_TRUE, TASK_TAB_ESB_STRU				, CalcEmergStopBtnStruExpItem,	5u, 1u, 0u, 0u },/*4--紧急停车按钮*/
	{ DQU_QFUN_TRUE, TASK_TAB_STOPSTATION_STRU		, CalcStopStationStruExpItem,	5u, 1u, 0u, 0u },/*5--停车区域*/
	{ DQU_QFUN_TRUE, TASK_TAB_AXLE_SGMT_STRU		, AxleSgmtStru,					5u, 1u, 0u, 0u },/*6--计轴区段*/
	{ DQU_QFUN_TRUE, TASK_TAB_STOPPOINT_STRU		, CalculateStopPoint,		    5u, 1u, 0u, 0u },/*7--停车点*/
	{ DQU_QFUN_TRUE, TASK_TAB_AR_IN_STRU			, ARInStru,						5u, 1u, 0u, 0u },/*8--AR IN*/
	{ DQU_QFUN_TRUE, TASK_TAB_AR_OUT_STRU			, AROutStru,					5u, 1u, 0u, 0u },/*9--AR OUT*/
	{ DQU_QFUN_TRUE, TASK_TAB_LOGIC_SGMT_STRU		, LogicSgmtStru,				5u, 1u, 0u, 0u },/*10--逻辑区段*/
	{ DQU_QFUN_TRUE, TASK_TAB_ROUTE_STRU			, RouteStru,					5u, 1u, 0u, 0u },/*11--进路*/
	{ DQU_QFUN_TRUE, TASK_TAB_PROTECT_LINK_STRU		, ProtectLink,					5u, 1u, 0u, 0u },/*12--保护区段*/
	{ DQU_QFUN_TRUE, TASK_TAB_GRADE_STRU			, Grade,						5u, 1u, 0u, 0u },/*13--坡度*/
	{ DQU_QFUN_TRUE, TASK_TAB_DEFAULT_ROUTE_STRU	, DefaultRouteStru,				5u, 1u, 0u, 0u },/*14--默认行车序列*/
	{ DQU_QFUN_TRUE, TASK_TAB_DEVICE_INFO_IN_LINK	, GetDeviceInfoInLink,			5u, 1u, 0u, 0u },/*15--建立Link和其他设备间的映射关系*/
	{ DQU_QFUN_TRUE, TASK_TAB_GRADE_INFO_CROSS_LINK	, GetGradeInfoCrossLink,		5u, 1u, 0u, 0u },/*16--建立Link和坡度间的映射关系*/
	{ DQU_QFUN_TRUE, TASK_TAB_SPKS_IN_LINK			, CalcSPKSIncLinks,				5u, 1u, 0u, 0u },/*17--计算SPKS包含的Link序列*/
	{ DQU_QFUN_TRUE, TASK_TAB_PHYSGMT_STRU			, CalcPhySgmtIncLinks,			5u, 1u, 0u, 0u },/*18--计算物理区段包含的Link序列*/
	{ DQU_QFUN_TRUE, TASK_TAB_NONCBTC_IN_POINT		, CalcNonCbtcAreaIncPoints,		5u, 1u, 0u, 0u },/*19--计算非CBTC区域包含的道岔*/
};
/*计算任务基表项目数量*/
static const UINT16 MAX_CALCTASK_COUNT = sizeof(g_struCalcTaskTab) / sizeof(CALC_TASK_TAB_STRU);

/*根据版本配置数据处理过程函数*/
/*备注：依据互联互通数据结构V12.0.0修改，modify by lmy 20180312*/
/*数据解耦修改，modify by lyk 20220307*/
static UINT8 initConfigEmapTaskTab(void)
{
	UINT8 rtnValue = 0u;
	UINT8 cbtcVersionType = 0u;

	cbtcVersionType = GetCbtcSysType();

	/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID 因为是空表，所以设置不进行读取 sds*/

	g_struInitTaskTab[EMAP_NOP_0].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_40].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_42].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_43].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_44].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_47].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_48].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_49].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_50].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_52].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_53].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_54].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_55].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_56].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_57].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_59].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/
	g_struInitTaskTab[EMAP_NOP_60].bIsEffective = DQU_QFUN_FASLE;/*为补全数据表占位的空表*/

	if (DQU_CBTC_CPK == cbtcVersionType)/*以前版本合库不需要处理的数据表*/
	{
		g_struInitTaskTab[EMAP_SPKS_BUTTON_ID].bIsEffective = DQU_QFUN_FASLE;/*SPKS表*/
		g_struInitTaskTab[EMAP_DEPOT_GATE_ID].bIsEffective = DQU_QFUN_FASLE;/*车库门表*/
		g_struInitTaskTab[EMAP_TUNNEL_ID].bIsEffective = DQU_QFUN_FASLE;/*隧道表*/
		g_struInitTaskTab[EMAP_ZC_ID].bIsEffective = DQU_QFUN_FASLE;/*ZC集中区表*/
		g_struInitTaskTab[EMAP_MULTI_POINT_ID].bIsEffective = DQU_QFUN_FASLE;/*多开道岔表*/
		g_struInitTaskTab[EMAP_POWERLESSZONE_ID].bIsEffective = DQU_QFUN_FASLE;/*分相区表*/
		g_struInitTaskTab[EMAP_CARSTOP_ID].bIsEffective = DQU_QFUN_FASLE;/*车档表*/

		g_struCalcTaskTab[MAX_CALCTASK_COUNT - 1u].bIsEffective = DQU_QFUN_FASLE;/*不需要计算SPKS包含的Link序列*/

		rtnValue = 1u;
	}
	else if (DQU_CBTC_FAO == cbtcVersionType)/*FAO不需要处理的数据表*/
	{
		g_struInitTaskTab[EMAP_TUNNEL_ID].bIsEffective = DQU_QFUN_FASLE;/*隧道表*/
		g_struInitTaskTab[EMAP_ZC_ID].bIsEffective = DQU_QFUN_FASLE;/*ZC集中区表*/
		g_struInitTaskTab[EMAP_MULTI_POINT_ID].bIsEffective = DQU_QFUN_FASLE;/*多开道岔表*/
		g_struInitTaskTab[EMAP_POWERLESSZONE_ID].bIsEffective = DQU_QFUN_FASLE;/*分相区表*/
		g_struInitTaskTab[EMAP_CARSTOP_ID].bIsEffective = DQU_QFUN_FASLE;/*车档表*/

		g_struCalcTaskTab[MAX_CALCTASK_COUNT - 3u].bIsEffective = DQU_QFUN_FASLE;/*不需要建立Link和其他设备间的映射关系*/
		g_struCalcTaskTab[MAX_CALCTASK_COUNT - 2u].bIsEffective = DQU_QFUN_FASLE;/*不需要建立Link与坡度索引间的映射关系*/
		g_struCalcTaskTab[MAX_CALCTASK_COUNT - 1u].bIsEffective = DQU_QFUN_FASLE;/*不需要计算SPKS包含的Link序列*/


		rtnValue = 1u;
	}
	else if (DQU_CBTC_HLT == cbtcVersionType) /*互联互通不需要处理的数据表*/
	{
		g_struInitTaskTab[EMAP_ZC_DIVID_POINT_ID].bIsEffective = DQU_QFUN_FASLE;/*ZC分界点表*/
		g_struInitTaskTab[EMAP_ZC_COM_ZONE_ID].bIsEffective = DQU_QFUN_FASLE;/*ZC共管区域表*/
		g_struInitTaskTab[EMAP_MULTI_POINT_ID].bIsEffective = DQU_QFUN_FASLE;/*多开道岔表*/

		g_struInitTaskTab[EMAP_OC_ID].bIsEffective = DQU_QFUN_FASLE;/*OC表*/
		g_struInitTaskTab[EMAP_ITS_ID].bIsEffective = DQU_QFUN_FASLE;/*ITS表*/

		rtnValue = 1u;
	}
	else if (DQU_CBTC_DG == cbtcVersionType)/*单轨不需要处理的数据表*/
	{
		g_struInitTaskTab[EMAP_POWERLESSZONE_ID].bIsEffective = DQU_QFUN_FASLE;/*分相区表*/
		g_struInitTaskTab[EMAP_CARSTOP_ID].bIsEffective = DQU_QFUN_FASLE;/*车档表*/

		g_struCalcTaskTab[MAX_CALCTASK_COUNT - 2u].bIsEffective = DQU_QFUN_FASLE;/*不需要建立Link与坡度索引间的映射关系*/
		g_struCalcTaskTab[MAX_CALCTASK_COUNT - 1u].bIsEffective = DQU_QFUN_FASLE;/*不需要计算SPKS包含的Link序列*/
		rtnValue = 1u;
	}
	else if (DQU_CBTC_CC == cbtcVersionType) /*车车系统中不需要处理的数据表*/
	{
		g_struInitTaskTab[EMAP_BLOC_ACCESS_LINK_ID].bIsEffective = DQU_QFUN_FASLE;/*点式接近区段表*/
		g_struInitTaskTab[EMAP_DEFAULT_ROUTE_ID].bIsEffective = DQU_QFUN_FASLE;/*默认行车序列数据ID表*/
		g_struInitTaskTab[EMAP_ZC_DIVID_POINT_ID].bIsEffective = DQU_QFUN_FASLE;/*ZC分界点表*/
		g_struInitTaskTab[EMAP_ZC_COM_ZONE_ID].bIsEffective = DQU_QFUN_FASLE;/*ZC共管区域表*/
		g_struInitTaskTab[EMAP_BLOC_TRG_AREA_ID].bIsEffective = DQU_QFUN_FASLE;/*点式触发区段数据表*/
		g_struInitTaskTab[EMAP_CBTC_TRG_AREA_ID].bIsEffective = DQU_QFUN_FASLE;/*CBTC触发区段数据表*/
		g_struInitTaskTab[EMAP_PLAT_FORM_ID].bIsEffective = DQU_QFUN_FASLE;/*站台数据表*/
		g_struInitTaskTab[EMAP_COME_GO_ID].bIsEffective = DQU_QFUN_FASLE;/*往复进路表*/
		g_struInitTaskTab[EMAP_TUNNEL_ID].bIsEffective = DQU_QFUN_FASLE;/*隧道表*/
		g_struInitTaskTab[EMAP_ZC_ID].bIsEffective = DQU_QFUN_FASLE;/*ZC数据表*/
		g_struInitTaskTab[EMAP_MULTI_POINT_ID].bIsEffective = DQU_QFUN_FASLE;/*多开道岔数据表*/
		g_struInitTaskTab[EMAP_POWERLESSZONE_ID].bIsEffective = DQU_QFUN_FASLE;/*分相区表表*/
		g_struInitTaskTab[EMAP_CARSTOP_ID].bIsEffective = DQU_QFUN_FASLE;/*车档表*/
		g_struInitTaskTab[EMAP_OC_ID].bIsEffective = DQU_QFUN_FASLE;/*OC表*/
		g_struCalcTaskTab[TASK_TAB_DEFAULT_ROUTE_STRU].bIsEffective = DQU_QFUN_FASLE;		/*默认行车序列动态初始化*/
		g_struInitTaskTab[EMAP_ITS_ID].bIsEffective = DQU_QFUN_FASLE;/*ITSID表*/
		rtnValue = 1u;

	}
	else /*未出现的配置版本值，可能未配置标志位*/
	{
		rtnValue = 0u;
	}

	return rtnValue;

}

/*********************************************
*函数功能：电子地图模块版本号获取
*参数说明：供其它系统调用查询函数版本号，防止集成错误。
*入口参数：无
*出口参数：无
*返回值：UINT32表示的版本号
*********************************************/
UINT32 GetDquEmapVersion(void)
{
	UINT32 dquEmapVer = 0u;
	dquEmapVer = VerToU32SoftwareModule(DQU_EMAP_VER1, DQU_EMAP_VER2, DQU_EMAP_VER3, DQU_EMAP_VER4);
	return dquEmapVer;
}


/*********************************************
*函数功能：电子地图初始化
*参数说明：FSName文件指针
*返回值：0，失败；1，成功
*********************************************/
UINT8 dfsEmapInit(CHAR * FSName)
{
	UINT16	wIndex = 0u;
	UINT8   funcRtn = 0u;
	UINT8	rtnValue = 1u;
	UINT32	DataID = 0u;/*数据类型标识*/
	UINT8	*pDataAddr = NULL;
	UINT32	DataSize = 0u;
	UINT16  rowCount = 0u;

	if (NULL != FSName)
	{
		/*初始化错误信息记录*/
		CommonMemSet(initErrNo, INIT_ERR_NUM, 0u, INIT_ERR_NUM);

		/*初始化一次就行了*/
		if (1u == dfsEmapInitFlag)
		{
			rtnValue = 1u;
		}
		else
		{
			g_EmapInitMode = 0x01u;/*上电加载模式*/

			g_IsEmapAllocMemory = 0x01u;/*默认本接口分配内存*/

			/*检测指针是否为空*/
			if (NULL == g_dsuEmapStru)
			{
				/*指针为空退出*/
				rtnValue = 0u;
			}
			else
			{
				/*读取FS文件,并验证文件*/
				if (1u != dataRead(FSName))
				{
					/*数据读取不成功*/
					fileErrNo |= ERR_FILE_READ;
					rtnValue = 0u;
				}
				else
				{
					/*根据版本配置数据处理过程函数*/
					funcRtn = initConfigEmapTaskTab();
					if (0u == funcRtn)
					{
						rtnValue = 0u;
					}
				}

				/*初步分配内存空间*/
				if ((1u == rtnValue) && (0x01u == g_IsEmapAllocMemory))
				{
					g_dsuEmapStru->dsuDataLenStru = (DSU_DATA_LEN_STRU*)MyNew(sizeof(DSU_DATA_LEN_STRU));
					/*动态申请空间失败*/
					if (NULL == g_dsuEmapStru->dsuDataLenStru)
					{
						rtnValue = 0u;
					}
					else
					{
						/*初始化为0x0*/
						CommonMemSet(g_dsuEmapStru->dsuDataLenStru, sizeof(DSU_DATA_LEN_STRU), 0x0u, sizeof(DSU_DATA_LEN_STRU));
					}

					if (1u == rtnValue)
					{
						g_dsuEmapStru->dsuStaticHeadStru = (DSU_STATIC_HEAD_STRU *)MyNew(sizeof(DSU_STATIC_HEAD_STRU));
						/*动态申请空间失败*/
						if (NULL == g_dsuEmapStru->dsuStaticHeadStru)
						{
							rtnValue = 0u;
						}
						else
						{
							/*初始化为0x0*/
							CommonMemSet(g_dsuEmapStru->dsuStaticHeadStru, sizeof(DSU_STATIC_HEAD_STRU), 0x0u, sizeof(DSU_STATIC_HEAD_STRU));
						}
					}

					if (1u == rtnValue)
					{
						g_dsuEmapStru->dsuEmapIndexStru = (DSU_EMAP_INDEX_STRU*)MyNew(sizeof(DSU_EMAP_INDEX_STRU));
						/*动态申请空间失败*/
						if (NULL == g_dsuEmapStru->dsuEmapIndexStru)
						{
							rtnValue = 0u;
						}
						else
						{
							/*初始化为0x0*/
							CommonMemSet(g_dsuEmapStru->dsuEmapIndexStru, sizeof(DSU_EMAP_INDEX_STRU), 0x0u, sizeof(DSU_EMAP_INDEX_STRU));
						}
					}
				}

				if (1u == rtnValue)
				{
					/*索引初始化归零*/
					wIndex = 0u;
					do /*解析数据到相应的结构体*/
					{
						if (DQU_QFUN_TRUE == g_struInitTaskTab[wIndex].bIsEffective)/*判断这个初始化函数是否执行*/
						{
							/*获取配置数据*/
							DataID = g_struInitTaskTab[wIndex].DataTypeId;
							funcRtn = dquGetConfigDataNew(FSName, EMAP_DATA_TYPE, DataID, &pDataAddr, &DataSize, &rowCount);
							if (1u != funcRtn)
							{
								/*函数调用失败*/
								initErrNo[DataID] |= DEBUG_GET_DATA_FAIL;
								rtnValue = 0u;
								break;
							}

							if (1u == rtnValue)
							{
								funcRtn = g_struInitTaskTab[wIndex].pExeFun(pDataAddr, DataSize, rowCount);
								if (0u == funcRtn)
								{
									rtnValue = 0u;
									break;
								}
							}
						}

						wIndex++;
					} while (wIndex<MAX_INITTASK_COUNT);
				}

				/*接下来将数据结构还原为原来的数据结构*/
				if (1u == rtnValue)
				{
					wIndex = 0u;/*索引初始化归零*/
					do /***数据计算并填写结构体扩展变量****/
					{
						if (DQU_QFUN_TRUE == g_struCalcTaskTab[wIndex].bIsEffective)/*判断这个初始化函数是否执行*/
						{
							funcRtn = g_struCalcTaskTab[wIndex].pExeFun();
							if (0u == funcRtn)
							{
								rtnValue = 0u;
								break;
							}
						}

						wIndex++;
					} while (wIndex<MAX_CALCTASK_COUNT);
				}

				if (1u == rtnValue)
				{
					/*默认调用此接口自动关联查询数据源*/
					(void)dquSetCurrentQueryDataSource(g_dsuEmapStru, g_dsuEmapExtendStru, *g_ppDsuLimitStcLinkIdx);
					/*函数执行标志设为1*/
					dfsEmapInitFlag = 1u;
				}
			}
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/************************************************************
*函数功能：初始化电子地图扩展接口
*参数说明：
*		pDataBuf,传入数据块，
*		dsuEmapStru,外部实体电子地图存储结构体指针，
*		dsuLimitStcLinkIdx,外部实体静态限速link结构体指针，
*		mode,处理模式，1上电一次加载，2运行时多次处理
*		timeSlice,执行时间片
*返回值：0失败，1成功，2分步处理未完成
*修改：by yt 2021年9月11日 删除冗余变量，修改白盒U32赋值给U16问题
*************************************************************/
UINT8 dfsEmapInitExp(UINT8 *pDataBuf, DSU_EMAP_STRU *dsuEmapStru, DSU_STC_LIMIT_LINKIDX_STRU *dsuLimitStcLinkIdx, UINT8 mode, UINT16 timeSlice)
{
	UINT32 DataID = 0u;
	UINT8 rtnValue = 2u;
	UINT32 wFreqTimes = 0u;
	UINT8 bExeState = DQU_QFUN_FASLE;
	UINT32 dwOnceTime, dwUsedTime = 0u;
	UINT32 dwCurTimePoint = 0u;
	UINT32 dwBeginTimePoint = 0u;
	UINT32 dwLastTimePoint = 0u;

	UINT8 funcRtn = 0u;

	if ((NULL != pDataBuf) && (NULL != dsuEmapStru) && (NULL != dsuLimitStcLinkIdx))
	{
		if (0u == g_wCurMainIndex)/*主索引变量进入执行态的时候，就不能改变处理模式*/
		{
			g_EmapInitMode = mode;/*根据参数初始化全局模式控制变量*/

			g_IsEmapAllocMemory = 0x00u;/*默认本接口不分配内存*/

			g_dsuEmapStru = dsuEmapStru;/*将内部全局的电子地图指针关联外部定义结构体变量地址*/
			g_ppDsuLimitStcLinkIdx = &dsuLimitStcLinkIdx;/*关联外部的静态限速Link变量地址*/
														 
			/*检测电子地图数据结构体指针是否为空,由于静态限速Link的不是所有版本都需要初始化所有不做检测*/
			if (NULL == g_dsuEmapStru)
			{
				/*指针为空退出*/
				rtnValue = 0u;
			}
		}

		if (2u == rtnValue)
		{
			/*获取进入开始时间*/
			dwBeginTimePoint = dquGetCurrentTimeByMS();
			dwLastTimePoint = dwBeginTimePoint;/*记录历史时间点*/
			do /*上电加载持续运行保持控制*/
			{
				/*获取进入开始时间
				dwBeginTimePoint=dquGetCurrentTimeByMS();*/
				switch (g_wCurMainIndex)/*处理流程控制*/
				{
				case 0u:
					/*解析FS文件数据头、数据索引信息*/
					funcRtn = dquAnalyzeFsDataHead(pDataBuf, &g_DFsDH);
					if (1u != funcRtn)
					{	/*解析FS头数据失败*/
						rtnValue = 0u;
					}
					else/*处理成功*/
					{
						/*根据版本配置数据处理过程函数*/
						funcRtn = initConfigEmapTaskTab();
						if (0u == funcRtn)
						{
							rtnValue = 0u;
						}
						else
						{
							g_wCurMainIndex = 1u;
						}
					}
					break;
				case 1u:
					if (DQU_QFUN_TRUE == g_struInitTaskTab[g_wCurInitStruProg].bIsEffective)/*判断这个初始化函数是否需要执行*/
					{
						/*获取配置数据*/
						DataID = g_struInitTaskTab[g_wCurInitStruProg].DataTypeId;
						funcRtn = dquGetConfigDataExpNew(pDataBuf, &g_DFsDH, EMAP_DATA_TYPE, DataID, &g_pDataAddr, &g_DataSize, &g_RowCount);
						if (1u != funcRtn)
						{
							/*函数调用失败*/
							initErrNo[DataID] |= DEBUG_GET_DATA_FAIL;
							rtnValue = 0u;
						}
						else/*处理成功*/
						{
							g_wCurMainIndex = 2u;
						}
					}
					else
					{
						g_wCurInitStruProg++;/*初始化结构体进行下一个阶段*/
					}

					break;
				case 2:
					funcRtn = g_struInitTaskTab[g_wCurInitStruProg].pExeFun(g_pDataAddr, g_DataSize, g_RowCount);
					if (1u == funcRtn)/*完成本次完整的处理*/
					{
						do
						{
							g_wCurInitStruProg++;/*初始化结构体进行下一个阶段*/
												 /*越过无效处理函数*/
						} while ((g_wCurInitStruProg < MAX_INITTASK_COUNT) && (DQU_QFUN_FASLE == g_struInitTaskTab[g_wCurInitStruProg].bIsEffective));

						/*还有的初始化处理未完成*/
						if (g_wCurInitStruProg < MAX_INITTASK_COUNT)
						{
							g_wCurMainIndex = 1u;
						}
						else /*if (g_wCurInitStruProg >=MAX_INITTASK_COUNT) 这个条件状态下，完成了所有结构体初始化处理，进入计算阶段*/
						{
							g_wCurMainIndex = 3u;
						}
					}
					else if (2u == funcRtn)/*函数体内分步处理未完成*/
					{
						g_struInitTaskTab[g_wCurInitStruProg].wCurProcStage++;
					}
					else/*处理出现异常*/
					{
						rtnValue = 0u;
					}

					break;
				case 3u:/*计算处理*/
					funcRtn = g_struCalcTaskTab[g_wCurCalcStruProg].pExeFun();
					if (1u == funcRtn)/*完成本次完整的处理*/
					{
						do 
						{
							/*初始化结构体进行下一个阶段*/
							g_wCurCalcStruProg++;

						} while ((g_wCurCalcStruProg < MAX_CALCTASK_COUNT) && (DQU_QFUN_FASLE == g_struCalcTaskTab[g_wCurCalcStruProg].bIsEffective));

						/*还有的计算处理未完成*/
						if (g_wCurCalcStruProg < MAX_CALCTASK_COUNT)
						{
							g_wCurMainIndex = 3u;
						}
						else /*if (g_wCurCalcStruProg >=MAX_CALCTASK_COUNT) 这个条件状态下，完成了所有结构体初始化处理，进入计算阶段*/
						{
							g_wCurMainIndex = 4u;
						}
					}
					/*当前函数仍然在分步处理过程中*/
					else if (2u == funcRtn)
					{
						/*当前函数的执行阶段中不包含For循环体或循环体执行结束*/
						if (0u == g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop)
						{
							g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos = 0u;/*当函数包含多个循环体时，前一个For循环体结束，为下一个循环体准备初值*/
							g_struCalcTaskTab[g_wCurCalcStruProg].wCurProcStage++;/*切换函数中的下一段处理过程*/
						}
						else/*函数处理阶段在循环执行体中未完成*/
						{
							/*什么也不做*/
						}
					}
					else/*处理出现异常*/
					{
						rtnValue = 0u;
					}

					break;
				case 4:/*所有流程执行完毕并成功*/
					rtnValue = 1u;
					break;
				default:
					rtnValue = 0u;
					break;
				}

				/*异常出错退出,或所有处理执行完毕退出*/
				if (2u != rtnValue)
				{
					break;
				}

				/*时间统计及计算处理*/
				if (0x02u == mode)
				{
					/*获取当前时间*/
					dwCurTimePoint = dquGetCurrentTimeByMS();

					/*计算剩余时间 */
					dwUsedTime = dwCurTimePoint - dwBeginTimePoint;
					/*计算单次执行时间 */
					dwOnceTime = dwCurTimePoint - dwLastTimePoint;
					/*更新历史时间点 */
					dwLastTimePoint = dwCurTimePoint;

					/*使用的时间小于时间片 */
					if (dwUsedTime < timeSlice)
					{
						/*剩下的处理时间够再执行一次处理的*/
						if (0 < dwOnceTime)
						{
							wFreqTimes = (timeSlice - dwUsedTime) / dwOnceTime;
						}
						else /*如果单次执行时间小于1ms，则继续执行*/
						{
							wFreqTimes = 1u;
						}

						/*执行标志*/
						if (wFreqTimes >= 1)
						{
							bExeState = DQU_QFUN_TRUE;
						}
						else /*剩余时间不够下一次执行*/
						{
							bExeState = DQU_QFUN_FASLE;
						}
					}
					else/*超时退出，对于一次处理时间小于1毫秒的默认也退出，*/
					{	/* 防止出现进行多次小于1毫秒处理累积起来已经超时的情况*/

						bExeState = DQU_QFUN_FASLE;
					}
				}

				/*处理模式为上电加载时一直到执行完,或执行状态为完毕*/
			} while ((0x01u == mode) || (DQU_QFUN_TRUE == bExeState));

		}
	}
	else
	{
		rtnValue = 0u;
	}

	return rtnValue;
}

/*电子地图初始化完毕，执行电子地图内部公共变量的清理工作*/
UINT8 dfsEmapInitFinally(void)
{
	UINT16 wIndex = 0u;
	UINT8 bRetVal = 0u;

	g_wCurInitStruProg = 0u;/*当前结构体初始化进程，分步加载时使用*/
	g_wCurCalcStruProg = 0u;/*当前结构体计算处理进程，分步加载时使用*/
	g_wCurMainIndex = 0u; /*当前主处理索引，分步加载时使用*/
	g_EmapInitMode = 0x01u; /*电子地图初始化模式*/

						   /*将数据初始化状态归零*/
	for (wIndex = 0u; wIndex < MAX_INITTASK_COUNT; wIndex++)
	{
		g_struInitTaskTab[wIndex].wCurProcStage = 0x01u;
		g_struInitTaskTab[wIndex].wCurInitPos = 0x0u;
	}
	/*将数据计算处理状态归零*/
	for (wIndex = 0u; wIndex<MAX_CALCTASK_COUNT; wIndex++)
	{
		g_struCalcTaskTab[wIndex].wCurProcStage = 0x01u;
		g_struCalcTaskTab[wIndex].wCurInitPos = 0x0u;
		g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
	}

	/*初始化错误信息记录*/
	CommonMemSet(initErrNo, INIT_ERR_NUM, 0u, INIT_ERR_NUM);

	bRetVal = 1u;
	return bRetVal;
}


/*---------------------电子地图运行控制函数-------------------------*/

/*电子地图初始化分步控制函数*/
static UINT8  EmapInitFunStepCtrl(UINT16 wStep)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;

	if (0x01u == g_EmapInitMode)	/*上电加载处理*/
	{
		bIsEffective = DQU_QFUN_TRUE;
	}
	else /*分步加载*/
	{
		if (wStep == g_struInitTaskTab[g_wCurInitStruProg].wCurProcStage)
		{
			bIsEffective = DQU_QFUN_TRUE;
		}
		else if ((0x03u == wStep) && (0x03 < g_struInitTaskTab[g_wCurInitStruProg].wCurProcStage))
		{
			bIsEffective = DQU_QFUN_TRUE;
		}
		else
		{
			bIsEffective = DQU_QFUN_FASLE;
		}
	}

	return bIsEffective;

}


/*电子地图初始化循环步长分步切割处理
*修改：by yt 2021年9月11日 修改白盒U32赋值给U16问题
*/
static UINT8 EmapInitLoopStepLenCtrl(UINT32 dwIndex)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;

	if (0x01u == g_EmapInitMode)	/*上电加载处理*/
	{
		bIsEffective = DQU_QFUN_FASLE;
	}
	else /*分步加载*/
	{
		if (g_struInitTaskTab[g_wCurInitStruProg].wStepLen == dwIndex - g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos + 1u)
		{
			g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos = (UINT16)(dwIndex + 1u);
			bIsEffective = DQU_QFUN_TRUE;
		}
	}

	return bIsEffective;
}

/*电子地图数据计算分步控制函数*/
static UINT8 EmapCalcFunStepCtrl(UINT16 wStep)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;
	if (0x01u == g_EmapInitMode)	/*上电加载处理*/
	{
		bIsEffective = DQU_QFUN_TRUE;
	}
	else /*分步加载*/
	{
		if (wStep == g_struCalcTaskTab[g_wCurCalcStruProg].wCurProcStage)
		{
			bIsEffective = DQU_QFUN_TRUE;
		}
	}

	return bIsEffective;
}

/*电子地图计算循环步长分步切割处理
*修改：by yt 2021年9月11日 修改白盒U32赋值给U16问题
*/
static UINT8 EmapCalcLoopStepLenCtrl(UINT32 dwIndex)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;

	if (0x01u == g_EmapInitMode)	/*上电加载处理*/
	{
		bIsEffective = DQU_QFUN_FASLE;
	}
	else /*分步加载*/
	{
		if (g_struCalcTaskTab[g_wCurCalcStruProg].wStepLen == dwIndex - g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos + 1u)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos = (UINT16)(dwIndex + 1u);
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 1u;
			bIsEffective = DQU_QFUN_TRUE;
		}
	}

	return bIsEffective;
}



/*-------------------------电子地图结构体计算函数--------------------------------------*/

/*道岔表需增加wManageZC，wManagePhysicalCI，wManageLogicCI，通过LINK来获取*/
UINT8 CalcSwitchStruExpItem(void)
{
	UINT16 wIndex = 0u; /*循环变量*/
	UINT8 rtnValue = 2u;
	DSU_POINT_STRU*    pTempPointStru = NULL;	/*道岔数据结构体临时指针变量*/
	DSU_LINK_STRU *    pLinkStru = NULL;		/*LINK结构体临时指针变量,在计算是用到*/

	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	UINT16	*dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		if (NULL == dsuEmapIndexStru)
		{
			rtnValue = 0u;
			return rtnValue;
		}
		dsuLinkIndex = dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	if (DQU_QFUN_TRUE == EmapCalcFunStepCtrl(0x02u))
	{
		for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wPointLen; wIndex++)
		{
			pTempPointStru = dsuStaticHeadStru->pPointStru + wIndex;
			/*通过索引找到link信息*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempPointStru->wMainLkId];

			/*完成增加项计算*/
			pTempPointStru->wManageZC = pLinkStru->wManageZC;
			pTempPointStru->wManageLogicCI = pLinkStru->wManageLogicCI;
			pTempPointStru->wManagePhysicalCI = pLinkStru->wManagePhysicalCI;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == EmapCalcLoopStepLenCtrl(wIndex))
			{
				rtnValue = 2u;
				break;
			}
		}
		if (wIndex == dsuDataLenStru->wPointLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*	信号机表
*	需增加wManageZC，wManagePhysicalCI，wManageLogicCI，通过LINK来获取
*	修改记录：增加信号机关联进路ID，适应代码规范。wyd 20220214
*				修复YF0069-2125，进路不完全的的缺陷。kuan.he 20220718
*				在link上添加信号机信息 kuan.he 20230515
*/
UINT8 CalcSignalStruExpItem(void)
{
	UINT16 wIndex = 0u; /*循环变量*/
	UINT8 rtnValue = 1u;
	DSU_SIGNAL_STRU *  pTempSignalStru = NULL;	/*信号机结构体临时指针变量*/
	DSU_SIGNAL_STRU *  pTempSignalStru2 = NULL;	/*信号机结构体临时指针变量2*/
	DSU_LINK_STRU *    pLinkStru = NULL;		/*LINK结构体临时指针变量,在计算是用到*/
	DSU_ROUTE_STRU *pRouteStru = NULL;			/*进路结构体临时指针变量*/
	DSU_ROUTE_STRU *pRouteChkStru = NULL;		/*进路结构体临时指针变量，检查重复进路用*/
	UINT16 RouteIndex = 0u;						/*遍历进路表循环变量*/
	UINT16 RouteChkIndex = 0u;					/*遍历进路表循环变量，检查重复进路用*/
	UINT16 PointIndex = 0u;						/*遍历道岔循环变量*/
	UINT8  IsDuplicateRoute = DSU_FALSE;		/*重复进路标志*/
	UINT8  FunRtn = 0u;							/*函数返回值承接变量*/

	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	UINT16  *dsuSignalIndex = NULL;
	UINT16	*dsuLinkIndex = NULL;
	UINT16	*dsuRouteIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		if (NULL == dsuEmapIndexStru)
		{
			rtnValue = 0u;
		}
		else
		{
			dsuSignalIndex = dsuEmapIndexStru->dsuSignalIndex;
			dsuLinkIndex = dsuEmapIndexStru->dsuLinkIndex;
			dsuRouteIndex = dsuEmapIndexStru->dsuRouteIndex;
			dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
			dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;

			FunRtn = EmapCalcFunStepCtrl(0x02u);
			if (DQU_QFUN_TRUE == FunRtn)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wSignalLen; wIndex++)
				{
					pTempSignalStru = dsuStaticHeadStru->pSignalStru + wIndex;
					/*通过索引找到link信息*/
					pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempSignalStru->wProtectLinkId];

					/*完成增加项计算*/
					pTempSignalStru->wManageZC = pLinkStru->wManageZC;
					pTempSignalStru->wManageLogicCI = pLinkStru->wManageLogicCI;
					pTempSignalStru->wManagePhysicalCI = pLinkStru->wManagePhysicalCI;

					/*在link上添加信号机信息*/
					if (0xFFu == pLinkStru->EmapSameDirSignalNum)
					{
						/*初始化link表中的数量*/
						pLinkStru->EmapSameDirSignalNum = 0u;
					}
					else
					{
						/*do nothing*/
					}

					if (0xFFu == pLinkStru->EmapConvDirSignalNum)
					{
						/*初始化link表中的数量*/
						pLinkStru->EmapConvDirSignalNum = 0u;
					}
					else
					{
						/*do nothing*/
					}

					/*填入信号机信息，针对一个link上的信号机，10个位置完全够用*/
					if (EMAP_SAME_DIR == (UINT8)pTempSignalStru->wProtectDir)
					{
						/*和电子地图同方向*/
						pLinkStru->EmapSameDirSignalIdBuff[pLinkStru->EmapSameDirSignalNum] = pTempSignalStru->wId;
						pLinkStru->EmapSameDirSignalNum++;
					}
					else
					{
						/*和电子地图反方向*/
						pLinkStru->EmapConvDirSignalIdBuff[pLinkStru->EmapConvDirSignalNum] = pTempSignalStru->wId;
						pLinkStru->EmapConvDirSignalNum++;
					}

					/*增加信号机关联进路ID，wyd 20220214*/
					/*初始化绿灯对应进路为0xFFFF，黄灯对应进路数量为0*/
					pTempSignalStru->wGreenRouteId = DSU_NULL_16;
					pTempSignalStru->wYellowRouteNum = 0u;

					/*调车信号机、分段信号机不保存关联进路ID*/
					if ((EMAP_SIGNAL_TYPE_DC == (pTempSignalStru->wAttribute & EMAP_SIGNAL_TYPE_DC)) 
						|| (EMAP_SIGNAL_TYPE_HBDC == (pTempSignalStru->wAttribute & EMAP_SIGNAL_TYPE_HBDC))
						|| (EMAP_SIGNAL_TYPE_FENDUAN == (pTempSignalStru->wAttribute & EMAP_SIGNAL_TYPE_FENDUAN)))
					{
						/*do nothing*/
					}
					else
					{
						/*遍历进路表*/
						for (RouteIndex = 0u; RouteIndex < dsuDataLenStru->wRouteLen; RouteIndex++)
						{
							/*指针指向当前进路，将进路重复标识预置为FALSE*/
							pRouteStru = dsuStaticHeadStru->pRouteStru + RouteIndex;
							IsDuplicateRoute = DSU_FALSE;

							/*需要判断一下进路终点是否为分段信号机*/
							pTempSignalStru2 = dsuStaticHeadStru->pSignalStru + dsuSignalIndex[pRouteStru->wEndSignalId];

							/*调车进路、尾端是分段信号机的进路不需要遍历*/
							if ((EMAP_SHUNT_ROUTE == (pRouteStru->wRouteType & EMAP_SHUNT_ROUTE)) || (EMAP_SIGNAL_TYPE_FENDUAN == (pTempSignalStru2->wAttribute & EMAP_SIGNAL_TYPE_FENDUAN)))
							{
								/*do nothing*/
							}
							else
							{
								/*先检查前面有没有起点、终点、道岔开向均相同的进路，如有则认为该进路重复，不往信号机结构里添加*/
								if (1u <= RouteIndex)
								{
									/*这里用--而不用++是因为一般重复的两条进路在数据里都是相邻配置，从后往前遍历可以减少循环次数*/
									for (RouteChkIndex = RouteIndex; RouteChkIndex > 0u; RouteChkIndex--)
									{
										pRouteChkStru = dsuStaticHeadStru->pRouteStru + (RouteChkIndex - 1u);
										if ((pRouteChkStru->wStartSignalId == pRouteStru->wStartSignalId)
											&& (pRouteChkStru->wEndSignalId == pRouteStru->wEndSignalId)
											&& (pRouteChkStru->wIncludePointNum == pRouteStru->wIncludePointNum))
										{
											/*始端、终端信号机相同，道岔数量相同，遍历道岔位置*/
											for (PointIndex = 0u; PointIndex < pRouteChkStru->wIncludePointNum; PointIndex++)
											{
												if (pRouteChkStru->wPointStatus[PointIndex] == pRouteStru->wPointStatus[PointIndex])
												{
													/*位置相同，继续遍历*/
												}
												else
												{
													/*位置不同，跳出*/
													break;
												}
											}
											if (PointIndex < pRouteChkStru->wIncludePointNum)
											{
												/*跳出了，有位置不同的道岔，进路不重复*/
											}
											else
											{
												/*道岔位置也相同，进路是重复的*/
												IsDuplicateRoute = DSU_TRUE;
												break;
											}
										}
										else
										{
											/*进路不重复*/
										}
									}
								}
								else
								{
									/*DO NOTHING*/
								}

								if ((pRouteStru->wStartSignalId == pTempSignalStru->wId) && (DSU_FALSE == IsDuplicateRoute))
								{
									/*当前进路始端信号机ID与当前信号机ID相同，且不与已经遍历过的进路重复，继续判断进路内是否有侧向道岔*/
									for (PointIndex = 0u; PointIndex < pRouteStru->wIncludePointNum; PointIndex++)
									{
										if (EMAP_POINT_STATUS_MAIN == pRouteStru->wPointStatus[PointIndex])
										{
											/*遍历到进路内的直向道岔，DO NOTHING*/
										}
										else /*if (EMAP_POINT_STATUS_SIDE == pRouteStru->wPointStatus[PointIndex])*/
										{
											/*遍历到进路内的侧向道岔，跳出遍历*/
											break;
										}
									}

									if (PointIndex == pRouteStru->wIncludePointNum)
									{
										/*遍历完所有道岔没有提前跳出，说明全部道岔都在直向*/
										if ((0u == pTempSignalStru->wGreenRouteId) || (DSU_NULL_16 == pTempSignalStru->wGreenRouteId))
										{
											/*对应直向进路ID没被赋过值，在这里赋值*/
											pTempSignalStru->wGreenRouteId = pRouteStru->wId;
										}
										else
										{
											/*对应直向进路ID已被赋过值，说明一个信号机对应了两条直向进路，不符合原则，报错*/
											rtnValue = 0u;
											break;
										}
									}
									else
									{
										/*没有遍历完所有道岔就跳出了，说明有道岔在侧向*/
										if (MAX_YELLOW_ROUTE_NUM > pTempSignalStru->wYellowRouteNum)
										{
											/*黄灯进路数量未达到最大值，赋值*/
											pTempSignalStru->wYellowRouteId[pTempSignalStru->wYellowRouteNum] = pRouteStru->wId;
											pTempSignalStru->wYellowRouteNum += 1u;
										}
										else
										{
											/*黄灯进路数量达到或超过最大值了，报错*/
											rtnValue = 0u;
											break;
										}
									}
								}
								else
								{
									/*继续遍历*/
								}
							}
						}
					}

					/*循环分步切割处理*/
					FunRtn = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FunRtn)
					{
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wSignalLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				}
			}

			/*为关联信号机匹配进路*/
			FunRtn = EmapCalcFunStepCtrl(0x03u);
			if (DQU_QFUN_TRUE == FunRtn)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wSignalLen; wIndex++)
				{
					pTempSignalStru = dsuStaticHeadStru->pSignalStru + wIndex;
					if (0xffffu != pTempSignalStru->wMainSignalId)
					{
						/*信号机是复示信号机，找到对应的主信号机，把关联进路ID拷贝过来*/
						pTempSignalStru2 = dsuStaticHeadStru->pSignalStru + dsuSignalIndex[pTempSignalStru->wMainSignalId];
						/*增加检查：如果复示信号机和主信号机的防护防向不同，数据初始化不通过。wyd 20220302*/
						if (pTempSignalStru->wProtectDir != pTempSignalStru2->wProtectDir)
						{
							rtnValue = 0u;
							break;
						}
						else
						{
							pTempSignalStru->wGreenRouteId = pTempSignalStru2->wGreenRouteId;
							pTempSignalStru->wYellowRouteNum = pTempSignalStru2->wYellowRouteNum;
							CommonMemCpy(&pTempSignalStru->wYellowRouteId[0], MAX_YELLOW_ROUTE_NUM * sizeof(UINT16), &pTempSignalStru2->wYellowRouteId[0], MAX_YELLOW_ROUTE_NUM * sizeof(UINT16));
						}
					}
					else
					{
						/*DO NOTHING*/
					}

					/*将所有的进路id记录*/
					if ((0u != pTempSignalStru->wGreenRouteId) && (0xFFFF != pTempSignalStru->wGreenRouteId))
					{
						/*将绿灯信号机对应的进路id记录*/
						pTempSignalStru->wAllRouteNum = 1u;
						pTempSignalStru->wAllRouteIdBuff[0] = pTempSignalStru->wGreenRouteId;
					}
					else
					{
						/*信号机没有绿灯时，do nothing*/
						pTempSignalStru->wAllRouteNum = 0u;
					}

					/*将黄灯信号机对应的进路id记录*/
					if ((0u != pTempSignalStru->wYellowRouteNum) && (0xFFu != pTempSignalStru->wYellowRouteNum))
					{
						/*修复YF0069-2125，可能没有绿灯进路，黄灯进路需要按次序存储 kuan.he	20220718*/
						CommonMemCpy(&pTempSignalStru->wAllRouteIdBuff[pTempSignalStru->wAllRouteNum], pTempSignalStru->wYellowRouteNum * sizeof(UINT16), &pTempSignalStru->wYellowRouteId[0], pTempSignalStru->wYellowRouteNum * sizeof(UINT16));
						pTempSignalStru->wAllRouteNum += pTempSignalStru->wYellowRouteNum;
					}
					else
					{
						/*do nothing*/
					}

					/*循环分步切割处理*/
					FunRtn = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FunRtn)
					{
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wSignalLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				}
			}
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}

	return rtnValue;
}

/*安全屏蔽门表需增加wManageLogicCI，通过LINK来获取*/
UINT8 CalcPSDoorStruExpItem(void)
{
	UINT16 wIndex = 0u; /*循环变量*/
	UINT8 rtnValue = 2u;
	DSU_SCREEN_STRU*   pTempScreenStru = NULL;        /*安全屏蔽门表*/
	DSU_LINK_STRU *    pLinkStru = NULL;   /*LINK结构体临时指针变量,在计算是用到*/

										   /*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	UINT16	*dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		if (NULL == dsuEmapIndexStru)
		{
			rtnValue = 0u;
			return rtnValue;
		}
		dsuLinkIndex = dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	if (DQU_QFUN_TRUE == EmapCalcFunStepCtrl(0x02u))
	{

		for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wScreenLen; wIndex++)
		{
			pTempScreenStru = dsuStaticHeadStru->pScreenStru + wIndex;
			/*通过索引找到link信息*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempScreenStru->wRelatLkId];
			/*完成增加项计算*/
			pTempScreenStru->wManageLogicCI = pLinkStru->wManageLogicCI;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == EmapCalcLoopStepLenCtrl(wIndex))
			{
				rtnValue = 2u;
				break;
			}
		}
		if (wIndex == dsuDataLenStru->wScreenLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*紧急停车按钮需增加wManageLogicCI，通过LINK来获取*/
UINT8 CalcEmergStopBtnStruExpItem(void)
{
	UINT16 wIndex = 0u; /*循环变量*/
	UINT8 rtnValue = 2u;
	DSU_EMERG_STOP_STRU*   pTempEmergStopStru = NULL;   /*紧急停车按钮*/
	DSU_LINK_STRU *    pLinkStru = NULL;   /*LINK结构体临时指针变量,在计算是用到*/

										   /*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	UINT16	*dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		if (NULL == dsuEmapIndexStru)
		{
			rtnValue = 0u;
			return rtnValue;
		}
		dsuLinkIndex = dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	if (DQU_QFUN_TRUE == EmapCalcFunStepCtrl(0x02u))
	{

		for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wEmergStopLen; wIndex++)
		{
			pTempEmergStopStru = dsuStaticHeadStru->pEmergStopStru + wIndex;
			/*通过索引找到link信息*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempEmergStopStru->wRelatLkId];
			/*完成增加项计算*/
			pTempEmergStopStru->wManageLogicCI = pLinkStru->wManageLogicCI;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == EmapCalcLoopStepLenCtrl(wIndex))
			{
				rtnValue = 2u;
				break;
			}
		}
		if (wIndex == dsuDataLenStru->wEmergStopLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*停车区域需增加dwStatTmnlLinkOfst，wManagePhysicalC，wManageLogicCI，通过LINK来获取*/
UINT8 CalcStopStationStruExpItem(void)
{
	UINT16 wIndex = 0u; /*循环变量*/
	UINT8 rtnValue = 2u;
	DSU_STATION_STRU*   pTempStationStru = NULL;    /*停车区域信息结构*/		/*原来叫---站台表*/
	DSU_LINK_STRU *    pLinkStru = NULL;   /*LINK结构体临时指针变量,在计算是用到*/

										   /*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	UINT16	*dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		if (NULL == dsuEmapIndexStru)
		{
			rtnValue = 0u;
			return rtnValue;
		}
		dsuLinkIndex = dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	if (DQU_QFUN_TRUE == EmapCalcFunStepCtrl(0x02u))
	{
		for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wStationLen; wIndex++)
		{
			pTempStationStru = dsuStaticHeadStru->pStationStru + wIndex;
			/*通过索引找到link信息*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempStationStru->wLinkId];

			/*完成增加项计算*/
			pTempStationStru->dwStatTmnlLinkOfst = pLinkStru->dwLength;
			pTempStationStru->wManageLogicCI = pLinkStru->wManageLogicCI;
			pTempStationStru->wManagePhysicalCI = pLinkStru->wManagePhysicalCI;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == EmapCalcLoopStepLenCtrl(wIndex))
			{
				rtnValue = 2u;
				break;
			}
		}
		if (wIndex == dsuDataLenStru->wStationLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*处理无人折返进入数据处理*/
/*修改记录：YF0069-2197 修改白盒问题   by xly  20221108*/
UINT8 ARInStru(void)
{
	UINT8	rtnValue = 2u;
	UINT16	wIndex = 0u;
	DSU_AR_IN_STRU*  pTempARInStru = NULL;/*无人折返进入表*/			/*合并到无人折返表*/
	DSU_AR_STRU*  pTempARStru = NULL;/*无人折返表*/    /*这个表由无人折返进入和无人折返退出表合并*/  /*新增---严广学*/
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	/*电子地图数据*/
	UINT16	*dsuStopPointIndex = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	UINT8 FuncRtn1 = 0u;
	/*分拣数据*/
	if (NULL != g_dsuEmapStru)
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStopPointIndex = dsuEmapIndexStru->dsuStopPointIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	if (0u != dsuDataLenStru->wARLen)
	{
		if (0u == dsuDataLenStru->wARInLen)
		{
			/*长度=自动折返长度*/
			dsuDataLenStru->wARInLen = dsuDataLenStru->wARLen;
			/*索引个数为自动折返个数*/
			dsuEmapIndexStru->ARINNUM = dsuEmapIndexStru->ARNUM;
		}

		if (DQU_QFUN_TRUE == EmapCalcFunStepCtrl(0x01u))
		{
			if (0x01u == g_IsEmapAllocMemory)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pARInStru = (DSU_AR_IN_STRU *)MyNew(sizeof(DSU_AR_IN_STRU) * dsuDataLenStru->wARInLen);
				if (NULL == dsuStaticHeadStru->pARInStru)
				{
					/*申请空间失败*/
					rtnValue = 0u;
					return rtnValue;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pARInStru, sizeof(DSU_AR_IN_STRU) * dsuDataLenStru->wARInLen, 0xFFu, sizeof(DSU_AR_IN_STRU) * dsuDataLenStru->wARInLen);
				}

				/*动态申请索引空间*/
				dsuEmapIndexStru->dsuARInIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->ARINNUM + 2u);
				/*动态申请空间失败*/
				if (NULL == dsuEmapIndexStru->dsuARInIndex)
				{
					rtnValue = 0u;
					return rtnValue;
				}
				else
				{
					/*初始化为0xFF*/
					CommonMemSet(dsuEmapIndexStru->dsuARInIndex, dsuEmapIndexStru->ARINNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->ARINNUM * 2u + 2u);
				}
				rtnValue = 2u;
			}
			else/*内存空间是由外部产品来分配的*/
			{
				rtnValue = 2u;

				/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_LINK_ID, &wStruDataLen);

				if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wARInLen >= wStruDataLen)) || (0u == wStruDataLen)))
				{
					rtnValue = 0u;
					return rtnValue;
				}

				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_LINK_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->ARINNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
					return rtnValue;
				}
			}

		}
		FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wARInLen; wIndex++)
			{
				pTempARInStru = dsuStaticHeadStru->pARInStru + wIndex;
				pTempARStru = dsuStaticHeadStru->pARStru + wIndex;

				/*计算自动折返进入表*/
				pTempARInStru->wId = pTempARStru->wId;
				/*所属站台ID*/
				pTempARInStru->wPlatformId = pTempARStru->wStartPlartformId;
				/*停车点所处Link编号*/
				pTempARInStru->wStopPointLinkID = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wARInStopPointId])->wLinkId;
				pTempARInStru->dwStopPointLinkOffset = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wARInStopPointId])->dwLinkOfst;
				pTempARInStru->wDir = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wARInStopPointId])->wDir;
				pTempARInStru->dwParkArea = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wARInStopPointId])->dwATPParkArea;
				pTempARInStru->wRevertPointLinkID = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wRevertPointId])->wLinkId;
				pTempARInStru->dwRevertPointLinkOffset = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wRevertPointId])->dwLinkOfst;
				pTempARInStru->dwRevertParkArea = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wRevertPointId])->dwATPParkArea;
				pTempARInStru->wARLampID = pTempARStru->wARLampID;
				pTempARInStru->wARButtonID = pTempARStru->wARButtonId;
				/*初始化索引*/
				dsuEmapIndexStru->dsuARInIndex[pTempARInStru->wId] = wIndex;
				/*循环分步切割处理*/
				FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
				if (DQU_QFUN_TRUE == FuncRtn1)
				{
					rtnValue = 2u;
					break;
				}

			}
			if (wIndex == dsuDataLenStru->wARInLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue = 1u;
			}
		}

	}
	else
	{
		rtnValue = 1u;
		/*长度=自动折返长度*/
		dsuDataLenStru->wARInLen = 0u;
		/*索引个数为自动折返个数*/
		dsuEmapIndexStru->ARINNUM = 0u;
	}

	return rtnValue;
}

/*处理无人折返退出数据处理*/
UINT8 AROutStru(void)
{
	UINT8	rtnValue = 2u;
	UINT16	wIndex = 0u;
	DSU_AR_OUT_STRU*  pTempAROutStru = NULL;/*无人折返退出表*/			/*合并到无人折返表*/
	DSU_AR_STRU*  pTempARStru = NULL;/*无人折返表*/    /*这个表由无人折返进入和无人折返退出表合并*/  /*新增---严广学*/
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*电子地图数据*/
	UINT16	*dsuStopPointIndex = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	UINT8 FuncRtn1 = 0u;
	/*分拣数据*/
	if (NULL != g_dsuEmapStru)
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStopPointIndex = dsuEmapIndexStru->dsuStopPointIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	if (0u != dsuDataLenStru->wARLen)
	{
		if (0u == dsuDataLenStru->wAROutLen)
		{

			/*长度=自动折返长度*/
			dsuDataLenStru->wAROutLen = dsuDataLenStru->wARLen;
			/*索引个数为自动折返个数*/
			dsuEmapIndexStru->AROUTNUM = dsuEmapIndexStru->ARNUM;
		}
		FuncRtn1 = EmapCalcFunStepCtrl(0x01u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			if (0x01u == g_IsEmapAllocMemory)
			{
				dsuStaticHeadStru->pAROutStru = (DSU_AR_OUT_STRU *)MyNew(sizeof(DSU_AR_OUT_STRU) * dsuDataLenStru->wAROutLen);
				if (NULL == dsuStaticHeadStru->pAROutStru)
				{
					/*申请空间失败*/
					rtnValue = 0u;
					return rtnValue;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pAROutStru, sizeof(DSU_AR_OUT_STRU) * dsuDataLenStru->wAROutLen, 0xFFu, sizeof(DSU_AR_OUT_STRU) * dsuDataLenStru->wAROutLen);
				}
				/*动态申请索引空间*/
				dsuEmapIndexStru->dsuAROutIndex = (UINT16*)MyNew(sizeof(UINT16) * (dsuEmapIndexStru->AROUTNUM + 1u));
				/*动态申请空间失败*/
				if (NULL == dsuEmapIndexStru->dsuAROutIndex)
				{
					rtnValue = 0u;
					return rtnValue;
				}
				else
				{
					/*初始化为0xFF*/
					CommonMemSet(dsuEmapIndexStru->dsuAROutIndex, sizeof(UINT16) * (dsuEmapIndexStru->AROUTNUM + 1u), 0xFFu, sizeof(UINT16) * (dsuEmapIndexStru->AROUTNUM + 1u));
				}
				rtnValue = 2u;
			}
			else/*内存空间是由外部产品来分配的*/
			{
				rtnValue = 2u;

				/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_LINK_ID, &wStruDataLen);

				if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wAROutLen >= wStruDataLen)) || (0u == wStruDataLen)))
				{
					rtnValue = 0u;
					return rtnValue;
				}

				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_LINK_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->AROUTNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
					return rtnValue;
				}
			}
		}
		FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex < dsuDataLenStru->wAROutLen; wIndex++)
			{
				pTempAROutStru = dsuStaticHeadStru->pAROutStru + wIndex;
				pTempARStru = dsuStaticHeadStru->pARStru + wIndex;

				/*计算自动折返退出表*/
				pTempAROutStru->wId = pTempARStru->wId;
				pTempAROutStru->wPlatformId = pTempARStru->wEndPlartformId;
				pTempAROutStru->wStopPointLinkID = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wAROutStopPointId])->wLinkId;
				pTempAROutStru->dwStopPointLinkOffset = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wAROutStopPointId])->dwLinkOfst;
				pTempAROutStru->dwParkArea = (dsuStaticHeadStru->pStopPointStru + dsuStopPointIndex[pTempARStru->wAROutStopPointId])->dwATPParkArea;
				/*初始化索引*/
				dsuEmapIndexStru->dsuAROutIndex[pTempAROutStru->wId] = wIndex;
				/*循环分步切割处理*/
				FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
				if (DQU_QFUN_TRUE == FuncRtn1)
				{
					rtnValue = 2u;
					break;
				}
			}

			if (wIndex == dsuDataLenStru->wAROutLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue = 1u;
			}
		}
	}
	else
	{
		rtnValue = 1u;
		/*长度=自动折返长度*/
		dsuDataLenStru->wAROutLen = 0u;
		/*索引个数为自动折返个数*/
		dsuEmapIndexStru->AROUTNUM = 0u;
	}

	return rtnValue;
}

/************************************************
auther: qxt
函数功能：判断数组中是否存在某元素，
用于计轴区段结构体相关联道岔计算中。
输入参数：
@inputArray:输入的数组
@arrayLength:数组长度
@element:所要查找的元素
输出参数：无
返回值： 0返回错误 1存在 2不存在
************************************************/
UINT8 DetecteElementInArray(UINT16 inputArray[], UINT16 arrayLength, UINT16 element)
{
	UINT8 rtnValue = 2u;
	UINT16 index;

	if (NULL != inputArray)
	{
		for (index = 0u; index<arrayLength; index++)
		{
			if (inputArray[index] == element)
			{
				rtnValue = 1u;
				return rtnValue;
			}
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/************************************************
auther: qxt
函数功能：判断两数组中是否有相同的元素，
用于计轴区段结构体相关联道岔计算中。
输入参数：
@array1:输入的数组1
@arrayLength1:数组1长度
@array2:输入的数组2
@arrayLength2:数组2长度
输出参数：无
返回值： 0返回错误 1存在 2不存在
修改记录： 修改YF0069-2195 白盒问题，判断时常量在左侧   by  xly  20221130
************************************************/
UINT8 DetecteSameElemInTwoArray(UINT16 array1[], UINT8 arrayLength1, UINT16 array2[], UINT8 arrayLength2)
{
	UINT8 rtnValue = 2u;
	UINT8 i = 0u, j = 0u;

	if ((NULL != array1) && (NULL != array2))
	{
		for (i = 0u; i<arrayLength1; i++)
		{
			for (j = 0u; j<arrayLength2; j++)
			{
				if ((array1[i] == array2[j]) && (0xffffu != array1[i] ))
				{
					rtnValue = 1u;
					return rtnValue;
				}
			}
		}
	}
	else
	{
		rtnValue = 0u;
	}

	return rtnValue;

}

/*互联互通项目使用*/
static UINT8 AxleSgmtStru_HLT(void)
{
	UINT16  linkId1 = 0u, linkId2 = 0u, linkId3 = 0u, linkId4 = 0u;			/*计轴区段计算用到的临时变量*/
	DQU_POINT_STATUS_STRU  pointStatusStru[4u] = { 0 };						/*道岔状态结构体数组*/
	UINT16 i = 0u, j = 0u, k = 0u, m = 0u, n = 0u, l = 0u;					/*循环变量*/
	DSU_AXLE_SGMT_STRU *pAxleSgmtStru[16] = { NULL };						/*计算计轴区段各项时用到，该数组表示所计算计轴区段相连接的所有计轴区段，目前上限值设为16*/
	UINT8  funcCallResult = 0u;												/*所调用的各函数的返回结构*/
	DSU_AXLE_SGMT_STRU *pTempAxleSgmtStru = NULL, *pCurAxleSgmtStru = NULL;	/*计轴区段表*/
	DSU_LINK_STRU *pLinkStru = NULL;										/*LINK结构体临时指针变量,在计算时用到*/
	UINT16 AxleSgmtCount = 0u;
	UINT8 rtnValue = 2u;
	UINT8 PointCount = 0u;													/*计轴区段相关联道岔数目 add by qxt 20161104*/
	UINT8 chFlag = 1u;														/*执行失败标记*/
	UINT8 chTempCount = 0u;

	/*电子地图数据*/
	UINT16	*dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	UINT8 FuncRtn1 = 0u;
	UINT8 FuncRtn2 = 0u;
	if (NULL != g_dsuEmapStru)
	{
		dsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	/**********
	计算wManagePhysicalCI,wManageZC,dwOrgnLinkOfst,dwTmnlLinkOfst
	*****/
	FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = 0u; i < dsuDataLenStru->wAxleSgmtLen; i++)
		{
			pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;
			/*通过索引找到link信息(取终点LINK)*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];

			/*完成增加项wManagePhysicalCI,wManageZC计算*/
			pTempAxleSgmtStru->wManageZC = pLinkStru->wManageZC;
			pTempAxleSgmtStru->wManagePhysicalCI = pLinkStru->wManagePhysicalCI;

			/*完成增加项dwOrgnLinkOfst,dwTmnlLinkOfst计算*/
			pTempAxleSgmtStru->dwOrgnLinkOfst = 0;
			pTempAxleSgmtStru->dwTmnlLinkOfst = pLinkStru->dwLength;
		}
		rtnValue = 2u;
	}
	/**********
	计算完毕wManagePhysicalCI,wManageZC,dwOrgnLinkOfst,dwTmnlLinkOfst
	*****/

	/****
	计算wMiddleLinkId
	****/
	FuncRtn1 = EmapCalcFunStepCtrl(0x03);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = 0u; i < dsuDataLenStru->wAxleSgmtLen; i++)
		{
			pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;

			/*150608 初始化中间Link数组为0xFFFF*/
			for (m = 0u; m < MIDDLE_LINK_MAX_NUM; m++)
			{
				pTempAxleSgmtStru->wMiddleLinkId[m] = 0xFFFFu;
			}

			/*包含道岔大于3,出错退出*//*3-->4 by qxt 20160915*/
			if (4u < pTempAxleSgmtStru->wRelatPointNum)
			{
				rtnValue = 0u;
				return rtnValue;
			}
			else
			{
				/*继续往下执行*/
			}

			/*起始link和末端link为同一link，wMiddleLinkId为0xFFFF*/
			if (pTempAxleSgmtStru->wOrgnLkId == pTempAxleSgmtStru->wTmnlLkId)
			{
				pTempAxleSgmtStru->wMiddleLinkId[0u] = 0xFFFFu;/*150608*/
			}
			else
			{
				/*不在同一link*/
				for (j = 0u; j < pTempAxleSgmtStru->wRelatPointNum; j++)
				{
					/*初始化道岔状态结构体*/
					pointStatusStru[j].PointId = pTempAxleSgmtStru->wRelatPointId[j];
					pointStatusStru[j].PointStatus = (UCHAR)pTempAxleSgmtStru->wRelatPointStatus[j];
				}


				funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, pTempAxleSgmtStru->wOrgnLkId, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId1);

				/*函数调用失败，退出*/
				if (1u != funcCallResult)
				{
					rtnValue = 0u;
					return rtnValue;
				}
				else
				{
					/*继续往下执行*/
				}

				/*起始link和末端link相邻，wMiddleLinkId为0xFFFF*/
				if (pTempAxleSgmtStru->wTmnlLkId == linkId1)
				{
					pTempAxleSgmtStru->wMiddleLinkId[0] = 0xFFFFu;/*150608*/
				}
				else
				{
					funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId1, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId2);

					/*函数调用失败，退出*/
					if (1u != funcCallResult)
					{
						rtnValue = 0u;
						return rtnValue;
					}
					else
					{
						/*继续往下执行*/
					}

					/*中间link为linkId1*/
					if (pTempAxleSgmtStru->wTmnlLkId == linkId2)
					{
						pTempAxleSgmtStru->wMiddleLinkId[0u] = linkId1;
					}
					else
					{
						/*150608 当有linkId2也不为该计轴区段的终点Link时，继续寻找下一个Link*/
						funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId2, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId3);

						/*函数调用失败，退出*/
						if (1u != funcCallResult)
						{
							rtnValue = 0u;
							return rtnValue;
						}
						else
						{
							/*继续往下执行*/
						}

						if (pTempAxleSgmtStru->wTmnlLkId == linkId3)
						{
							pTempAxleSgmtStru->wMiddleLinkId[0u] = linkId1;
							pTempAxleSgmtStru->wMiddleLinkId[1u] = linkId2;
						}
						else
						{
							/*支持计轴区段包含道岔数目为4 add by qxt 20160915*/
							/*当有linkId3也不为该计轴区段的终点Link时，继续寻找下一个Link*/
							funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId3, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId4);

							/*函数调用失败，退出*/
							if (1u != funcCallResult)
							{
								rtnValue = 0u;
								return rtnValue;
							}
							else
							{
								/*继续往下执行*/
							}

							if (pTempAxleSgmtStru->wTmnlLkId == linkId4)
							{
								pTempAxleSgmtStru->wMiddleLinkId[0u] = linkId1;
								pTempAxleSgmtStru->wMiddleLinkId[1u] = linkId2;
								pTempAxleSgmtStru->wMiddleLinkId[2u] = linkId3;
							}
							else
							{
								/*该计轴区段超过5个link,查询函数不支持这种情况*/
								rtnValue = 0u;
								return rtnValue;
							}

						}
					}
				}
			}
		} /*end  forloop*/
		rtnValue = 2u;
	}
	/****
	计算完毕  wMiddleLinkId
	****/

	/****
	计算	UINT16  wRelatAxleSgmtNum;			关联计轴区段数目
	UINT16  wRelatAxleSgmtId[4];				150608 关联计轴区段编号
	****/
	FuncRtn1 = EmapCalcFunStepCtrl(0x04u);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		/*先把关联计轴数目设为0*/
		for (k = 0u; k < dsuDataLenStru->wAxleSgmtLen; k++)
		{
			pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + k;
			pTempAxleSgmtStru->wRelatAxleSgmtNum = 0u;
		}
		rtnValue = 2u;
	}
	FuncRtn2 = DetecteSameElemInTwoArray(pTempAxleSgmtStru->wMiddleLinkId, MIDDLE_LINK_MAX_NUM, pCurAxleSgmtStru->wMiddleLinkId, MIDDLE_LINK_MAX_NUM);
	FuncRtn1 = EmapCalcFunStepCtrl(0x05u);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wAxleSgmtLen; i++)
		{
			/*当前待计算计轴区段*/
			pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;
			for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
			{
				/*临时计轴区段，不同于当前计轴区段*/
				if (j == i)
				{
					/*无操作，继续循环*/
				}
				else
				{
					pCurAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + j;
					/*当前计轴区段起始Link同临时计轴区段起始Link相同，
					当前计轴区段关联计轴区段为临时计轴*/
					if (pTempAxleSgmtStru->wOrgnLkId == pCurAxleSgmtStru->wOrgnLkId)
					{
						/*支持相关联计轴区段数目为16*/
						if (15u < pTempAxleSgmtStru->wRelatAxleSgmtNum)    /*wRelatAxleSgmtNum被当做数组下标*/
						{
							/*计轴区段数量错误*/
							rtnValue = 0u;
							return rtnValue;
						}
						else
						{
							pTempAxleSgmtStru->wRelatAxleSgmtId[pTempAxleSgmtStru->wRelatAxleSgmtNum] = pCurAxleSgmtStru->wId;
							pTempAxleSgmtStru->wRelatAxleSgmtNum++;
						}
					}
					/*当前计轴区段终端Link同临时计轴区段终端Link相同，
					当前计轴区段关联计轴区段为临时计轴*/
					else if (pTempAxleSgmtStru->wTmnlLkId == pCurAxleSgmtStru->wTmnlLkId)
					{
						/*支持相关联计轴区段数目为16*/
						if (15u < pTempAxleSgmtStru->wRelatAxleSgmtNum)
						{
							/*计轴区段数量错误*/
							rtnValue = 0u;
							return rtnValue;
						}
						else
						{
							pTempAxleSgmtStru->wRelatAxleSgmtId[pTempAxleSgmtStru->wRelatAxleSgmtNum] = pCurAxleSgmtStru->wId;
							pTempAxleSgmtStru->wRelatAxleSgmtNum++;
						}
					}
					/*当前计轴区段中间link存在且中间Link同临时计轴区段中间Link有相同的，当前计轴区段关联计轴区段为临时计轴*/

					else if (1u == FuncRtn2)
					{
						if (15u < pTempAxleSgmtStru->wRelatAxleSgmtNum)
						{
							/*计轴区段数量错误*/
							rtnValue = 0u;
							return rtnValue;
						}
						else
						{
							pTempAxleSgmtStru->wRelatAxleSgmtId[pTempAxleSgmtStru->wRelatAxleSgmtNum] = pCurAxleSgmtStru->wId;
							pTempAxleSgmtStru->wRelatAxleSgmtNum++;
						}

					}

					else
					{
						/*其他情况无操作，继续循环*/
					}
				}

			}
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			/*循环切分检测处理*/
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}
		}
		if (i == dsuDataLenStru->wAxleSgmtLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 2u;
		}
	}
	/****
	计算完毕	UINT16  wRelatAxleSgmtNum;					关联计轴区段数目
	UINT16  wRelatAxleSgmtId[4];				关联计轴区段编号
	****/

	/***
	计算相关联道岔wOrgnRelatPointId
	******/
	FuncRtn1 = EmapCalcFunStepCtrl(0x06);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wAxleSgmtLen; i++)
		{
			/*计数器还得设为0*/
			AxleSgmtCount = 0u;
			/*如果当前计轴区段的（起始计轴）wOrgnAxleId是另外其他计轴区段的（终点计轴）wTmnlAxleId，
			说明，当前计轴区段前必有关联道岔  pAxleSgmtStru[]为所有关联计轴区段*/

			/*新增link逻辑方向变化点：如果当前计轴区段起始link的起点为逻辑方向变化点，则需寻找其他计轴的起点计轴*/
			pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;

			/*判断起始link的起点是否逻辑方向变化点*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wOrgnLkId];

			if (0xaau == pLinkStru->wLogicDirChanged)
			{
				for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
				{
					/*临时计轴区段，不同于当前计轴区段*/
					if (j == i)
					{
						/*无操作，继续循环*/
					}
					else
					{
						/*起点计轴相等*/
						if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wOrgnAxleId == pTempAxleSgmtStru->wOrgnAxleId)
						{
							/*临时计轴区段不能是当前计轴区段的相关联计轴区段*/
							chTempCount = 0u;
							for (k = 0u; k<pTempAxleSgmtStru->wRelatAxleSgmtNum; k++)
							{
								if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wId == pTempAxleSgmtStru->wRelatAxleSgmtId[k])
								{
									chTempCount++;
									break;
								}
							}
							if (0u == chTempCount)
							{
								/*保存该指针，并计数*/
								pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
								AxleSgmtCount++;
							}

						}
						else
						{

						}
					}
				}
				/*计算当前计轴区段关联道岔*/
				/*一个计轴区段前相邻16个以上的计轴区段，出错！*/
				if (16 < AxleSgmtCount)
				{
					rtnValue = 0u;
					chFlag = 0u;
				}
				else
				{
					/*计算
					计轴区段起点连接计轴区段编号
					*/

					for (k = 0u; k < AxleSgmtCount; k++)
					{
						pTempAxleSgmtStru->wOrgnAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
					}

					/*	计轴区段起点连接计轴区段编号计算完毕*/


					/**计算计轴区段起点关联道岔***/

					/*连接计轴区段个数两个以上时才有关联道岔*/
					if (1u < AxleSgmtCount)
					{
						/*相关联道岔数目初始为0*/
						PointCount = 0u;
						/*循环相连接联计轴区段求关联道岔*/
						for (n = 0u; n < AxleSgmtCount; n++)
						{
							/*判断各个计轴区段的始端link的终端相邻侧线link是否有效*/
							pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wOrgnLkId];
							/*如果计轴区段起始link的终端相邻侧线Link为有效，关联道岔*/
							if (0xFFFF != pLinkStru->wTmnlJointSideLkId)
							{
								/*关联道岔没有被保存过*/
								FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wTmnlPointId);
								if (2u == FuncRtn1)
								{

									if (15u > PointCount)
									{
										pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
										PointCount++;
									}
									else
									{
										rtnValue = 0u;
										chFlag = 0u;
									}
								}
							}

							/*判断中间各link的终端相邻侧线Link是否为有效*/
							for (l = 0u; l < MIDDLE_LINK_MAX_NUM; l++)
							{
								if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
								{
									pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

									/*如果wMiddleLinkId的终端相邻侧线Link为有效，关联道岔*/
									if (0xFFFFu != pLinkStru->wTmnlJointSideLkId)
									{
										/*关联道岔没有被保存过*/
										FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wTmnlPointId);
										if (2u == FuncRtn1)
										{
											if (15u > PointCount)
											{
												pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
												PointCount++;
											}
											else
											{
												rtnValue = 0u;
												chFlag = 0u;
												break;
											}
										}
									}
								}
							}/*循环中间link end*/
						}/*循环相连接联计轴区段end*/
					}
				}
			}
			else/*不是起点逻辑方向变化点*/
			{
				for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
				{
					if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wTmnlAxleId == pTempAxleSgmtStru->wOrgnAxleId)
					{
						/*保存该指针，并计数*/
						pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
						AxleSgmtCount++;
					}
					else
					{

					}
				}
				/*计算当前计轴区段关联道岔*/
				/*一个计轴区段前相邻16个以上的计轴区段，出错！*/
				if (16u < AxleSgmtCount)
				{
					rtnValue = 0u;
					chFlag = 0u;
				}
				else
				{
					/*计算
					计轴区段起点连接计轴区段编号
					*/
					for (k = 0u; k < AxleSgmtCount; k++)
					{
						pTempAxleSgmtStru->wOrgnAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
					}

					/*	计轴区段起点连接计轴区段编号计算完毕*/

					/**计算计轴区段起点关联道岔***/
					/*连接计轴区段个数两个以上时才有关联道岔*/
					if (1u < AxleSgmtCount)
					{
						PointCount = 0u;/*相关联道岔数目初始为0*/
						/*循环相连接联计轴区段求关联道岔*/
						for (n = 0u; n < AxleSgmtCount; n++)
						{

							/*判断各个计轴区段的终端link的始端相邻侧线link是否有效*/
							pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wTmnlLkId];
							/*如果计轴区段起始link的始端相邻侧线Link为有效，关联道岔*/
							if (0xFFFFu != pLinkStru->wOrgnJointSideLkId)
							{
								/*关联道岔没有被保存过*/
								FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wOrgnPointId);
								if (2u == FuncRtn1)
								{

									if (15u > PointCount)
									{
										pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
										PointCount++;
									}
									else
									{
										rtnValue = 0u;
										chFlag = 0u;
									}
								}
							}

							/*判断中间各link的始端相邻侧线link是否有效*/
							for (l = 0u; l < MIDDLE_LINK_MAX_NUM; l++)
							{
								if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
								{
									pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

									/*如果wMiddleLinkId的始端相邻侧线Link为有效，关联道岔*/
									if (0xFFFFu != pLinkStru->wOrgnJointSideLkId)
									{
										/*关联道岔没有被保存过*/
										FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wOrgnPointId);
										if (2u == FuncRtn1)
										{
											if (15u > PointCount)
											{
												pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
												PointCount++;
											}
											else
											{
												rtnValue = 0u;
												chFlag = 0u;
												break;
											}
										}
									}
								}
							}/*循环中间link end*/
						}/*循环相连接联计轴区段end*/
					}
				}
			}/*else 不是逻辑方向变化点 end*/

			/**
			计轴区段起点关联道岔
			计算完毕***/
			
			/*循环切分检测处理*/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}
		}/*最外层for循环end*/

		if (i == dsuDataLenStru->wAxleSgmtLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 2u;
		}
	}

	/*开始增加项wTmnlRelatPointId计算 150608*/
	FuncRtn1 = EmapCalcFunStepCtrl(0x07u);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wAxleSgmtLen; i++)
		{
			/*计数器还得设为0*/
			AxleSgmtCount = 0u;
			/*如果当前计轴区段的（终点计轴）wTmnlAxleId是另外其他计轴区段的（起始计轴）wOrgnAxleId，
			说明，当前计轴区段后必有关联道岔*/


			/*新增link逻辑方向变化点：如果当前计轴区段终点link的终点为逻辑方向变化点，则需寻找其他计轴的终点计轴*/
			pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;


			/*判断终端link的终点是否逻辑方向变化点*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];
			if (0x55u == pLinkStru->wLogicDirChanged)
			{
				for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
				{
					/*临时计轴区段，不同于当前计轴区段*/
					if (j == i)
					{
						/*无操作，继续循环*/
					}
					else
					{
						if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wTmnlAxleId == pTempAxleSgmtStru->wTmnlAxleId)
						{
							/*临时计轴区段不能是当前计轴区段的相关联计轴区段*/
							chTempCount = 0u;
							for (k = 0u; k < pTempAxleSgmtStru->wRelatAxleSgmtNum; k++)
							{
								if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wId == pTempAxleSgmtStru->wRelatAxleSgmtId[k])
								{
									chTempCount++;
									break;
								}
							}
							if (0u == chTempCount)
							{
								/*保存该指针，并计数*/
								pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
								AxleSgmtCount++;
							}
						}
						else
						{
							/*do nothing*/
						}
					}
				}

				/*计算第i个计轴区段关联道岔*/ /*还在for循环中*/
				/*一个计轴区段后相邻16个以上的计轴区段，出错！*/
				if (16u < AxleSgmtCount)
				{
					rtnValue = 0u;
					chFlag = 0u;
				}
				else
				{
					/*计算
					计轴区段终点连接计轴区段编号
					*/

					for (k = 0u; k < AxleSgmtCount; k++)
					{
						pTempAxleSgmtStru->wTmnlAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
					}
					/****
					计轴区段终点连接计轴区段编号
					计算完毕
					***/

					/****计算终点关联道岔******/

					/*连接计轴区段个数两个以上时才有关联道岔*/
					if (1u < AxleSgmtCount)
					{
						/*相关联道岔计数器设为0*/
						PointCount = 0u;
						/*循环关联计轴区段求关联道岔*/
						for (n = 0u; n < AxleSgmtCount; n++)
						{
							/*判断各个计轴区段的终端link的始端相邻侧线link是否有效*/
							pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wTmnlLkId];
							/*如果计轴区段终端link的始端相邻侧线Link为有效，关联道岔*/
							if (0xFFFF != pLinkStru->wOrgnJointSideLkId)
							{
								/*关联道岔没有被保存过*/
								FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wOrgnPointId);
								if (2u == FuncRtn1)
								{
									if (15u > PointCount)
									{
										pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
										PointCount++;
									}
									else
									{
										rtnValue = 0u;
										chFlag = 0u;
									}
								}
							}

							/*判断中间各link的始端相邻侧线link是否有效*/
							for (l = 0u; l<MIDDLE_LINK_MAX_NUM; l++)
							{
								if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
								{
									pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

									/*如果wMiddleLinkId的始端相邻侧线Link为有效，关联道岔*/
									if (0xFFFFu != pLinkStru->wOrgnJointSideLkId)
									{
										/*关联道岔没有被保存过*/
										FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wOrgnPointId);
										if (2u == FuncRtn1)
										{
											if (15u > PointCount)
											{
												pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
												PointCount++;
											}
											else
											{
												rtnValue = 0u;
												chFlag = 0u;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else/*非逻辑方向变化点*/
			{
				for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
				{
					if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wOrgnAxleId == pTempAxleSgmtStru->wTmnlAxleId)
					{
						/*保存该指针，并计数*/
						pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
						AxleSgmtCount++;
					}
					else
					{
						/*do nothing*/
					}
				}

				/*计算第i个计轴区段关联道岔*/ /*还在for循环中*/
				/*一个计轴区段后相邻16个以上的计轴区段，出错！*/
				if (16u < AxleSgmtCount)
				{
					rtnValue = 0u;
					chFlag = 0u;
				}
				else
				{
					/*计算
					计轴区段终点连接计轴区段编号
					*/

					for (k = 0u; k < AxleSgmtCount; k++)
					{
						pTempAxleSgmtStru->wTmnlAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
					}
					/****
					计轴区段终点连接计轴区段编号
					计算完毕
					***/

					/****计算终点关联道岔******/

					/*连接计轴区段个数两个以上时才有关联道岔*/
					if (1u < AxleSgmtCount)
					{
						PointCount = 0u;/*相关联道岔数目初始为0*/
									   /*循环关联计轴区段求关联道岔*/
						for (n = 0u; n < AxleSgmtCount; n++)
						{
							/*判断各个计轴区段的起始link的终端相邻侧线link是否有效*/
							pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wOrgnLkId];
							/*如果计轴区段起始link的终端相邻侧线Link为有效，关联道岔*/
							if (0xFFFFu != pLinkStru->wTmnlJointSideLkId)
							{
								/*关联道岔没有被保存过*/
								FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wTmnlPointId);
								if (2u == FuncRtn1)
								{
									if (15u > PointCount)
									{
										pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
										PointCount++;
									}
									else
									{
										rtnValue = 0u;
										chFlag = 0u;
									}

								}
							}

							/*判断中间link的终端相邻侧线link是否有效*/
							for (l = 0u; l < MIDDLE_LINK_MAX_NUM; l++)
							{
								if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
								{
									pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

									/*如果wMiddleLinkId的始端相邻侧线Link为有效，关联道岔*/
									if (0xFFFF != pLinkStru->wTmnlJointSideLkId)
									{
										/*关联道岔没有被保存过*/
										FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wTmnlPointId);
										if (2u == FuncRtn1)
										{
											if (15u > PointCount)
											{
												pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
												PointCount++;
											}
											else
											{
												rtnValue = 0u;
												chFlag = 0u;
												break;
											}
										}
									}
								}
							}
						}
					}
				}
			}/*else end*/

			/****计算完毕  终点关联道岔******/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}
		}
		if ((i == dsuDataLenStru->wAxleSgmtLen) && (0u != chFlag))
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}
	return rtnValue;
}

/*车车项目使用 wyd 20220214*/
static UINT8 AxleSgmtStru_CC(void)
{
	UINT16 linkId1 = 0u;
	UINT16 linkId2 = 0u;
	UINT16 linkId3 = 0u;
	UINT16 linkId4 = 0u;				/*计轴区段计算用到的临时变量 150608添加临时变量linkId3*//*add linkId4 支持计轴区段包含道岔数目为4 modify by qxt 20160915*/

	DQU_POINT_STATUS_STRU  pointStatusStru[4u] = { 0 };	/*道岔状态结构体数组*//*3-->4 支持一个计轴区段包含4个道岔 modify by qxt 20160915 */
	UINT16 i = 0u;						/*循环变量*/
	UINT16 j = 0u;						/*循环变量*/
	UINT16 m = 0u;						/*循环变量*/
	UINT16 n = 0u;						/*循环变量*/
	UINT16 l = 0u;						/*循环变量*/
	UINT16 k = 0u;						/*循环变量*/
	DSU_AXLE_SGMT_STRU *pAxleSgmtStru[16] = { NULL };	/*计算计轴区段各项时用到，该数组表示所计算计轴区段相连接的所有计轴区段，目前上限值设为16 add by qxt 20160915*/
	UINT8  funcCallResult = 0u;							/*所调用的各函数的返回结构*/
	DSU_AXLE_SGMT_STRU *pTempAxleSgmtStru = NULL;
	DSU_AXLE_SGMT_STRU *pCurAxleSgmtStru = NULL;		/*计轴区段表*/
	DSU_LINK_STRU *pLinkStru = NULL;	/*LINK结构体临时指针变量,在计算时用到 150608添加pLinkStru1用于临时变量*//*添加pLinkStru2用于临时变量 add by qxt 20160917*/
	UINT16 AxleSgmtCount = 0u;
	UINT8 rtnValue = 2u;
	UINT8 PointCount = 0u;				/*计轴区段相关联道岔数目 add by qxt 20161104*/
	UINT8 chFlag = 1u;					/*执行失败标记*/
	UINT8 chTempCount = 0u;
	UINT8 FunRtn = 0u;
	UINT32 tempLength = 0u;
	DSU_STATIC_RES_SPEED_STRU *pStaticResSpeedStru = NULL;
	DSU_CONRESSPEED_STRU *pConResSpeedStru = NULL;

	/*电子地图数据*/
	UINT16	*dsuLinkIndex = NULL;
	UINT16  *dsuStaticResSpeedIndex = NULL;
	UINT16  *dsuConResSpeedIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	UINT8 FuncRtn1 = 0u;
	UINT8 FuncRtn2 = 0u;
	if (NULL != g_dsuEmapStru)
	{
		dsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticResSpeedIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuStaticResSpeedIndex;
		dsuConResSpeedIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuConResSpeedIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;

		/**********
		计算wManagePhysicalCI,wManageZC,dwOrgnLinkOfst,dwTmnlLinkOfst
		*****/
		funcCallResult = EmapCalcFunStepCtrl(0x02u);
		if (DQU_QFUN_TRUE == funcCallResult)
		{
			for (i = 0u; i < dsuDataLenStru->wAxleSgmtLen; i++)
			{
				pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;
				/*通过索引找到link信息(取终点LINK)*/
				pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];

				/*完成增加项wManagePhysicalCI,wManageZC计算*/
				pTempAxleSgmtStru->wManageZC = pLinkStru->wManageZC;
				pTempAxleSgmtStru->wManagePhysicalCI = pLinkStru->wManagePhysicalCI;

				/*完成增加项dwOrgnLinkOfst,dwTmnlLinkOfst计算*/
				pTempAxleSgmtStru->dwOrgnLinkOfst = 0u;
				pTempAxleSgmtStru->dwTmnlLinkOfst = pLinkStru->dwLength;
			}
		}
		/**********
		计算完毕wManagePhysicalCI,wManageZC,dwOrgnLinkOfst,dwTmnlLinkOfst
		*****/

		/****
		计算wMiddleLinkId
		增加计轴区段长度dwLength动态初始化，wyd 20220215
		****/
		funcCallResult = EmapCalcFunStepCtrl(0x03u);
		if (DQU_QFUN_TRUE == funcCallResult)
		{
			for (i = 0u; i < dsuDataLenStru->wAxleSgmtLen; i++)
			{
				pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;
				pTempAxleSgmtStru->dwLength = DSU_NULL_32;
				tempLength = DSU_NULL_32;
				/*150608 初始化中间Link数组为0xFFFF*/
				for (m = 0u; m < MIDDLE_LINK_MAX_NUM; m++)
				{
					pTempAxleSgmtStru->wMiddleLinkId[m] = 0xFFFFu;
				}

				/*包含道岔大于3,出错退出*//*3-->4 by qxt 20160915*/
				if (4u < pTempAxleSgmtStru->wRelatPointNum)
				{
					rtnValue = 0u;
					break;
				}
				else
				{
					/*继续往下执行*/
				}

				/*起始link和末端link为同一link，wMiddleLinkId为0xFFFF*/
				if (pTempAxleSgmtStru->wOrgnLkId == pTempAxleSgmtStru->wTmnlLkId)
				{
					tempLength = LinkLengthExp(g_dsuEmapStru, pTempAxleSgmtStru->wOrgnLkId);
					pTempAxleSgmtStru->wMiddleLinkId[0u] = 0xFFFFu;/*150608*/
				}
				else
				{
					/*不在同一link*/
					tempLength = LinkLengthExp(g_dsuEmapStru, pTempAxleSgmtStru->wOrgnLkId) + LinkLengthExp(g_dsuEmapStru, pTempAxleSgmtStru->wTmnlLkId);
					for (j = 0u; j < pTempAxleSgmtStru->wRelatPointNum; j++)
					{
						/*初始化道岔状态结构体*/
						pointStatusStru[j].PointId = pTempAxleSgmtStru->wRelatPointId[j];
						pointStatusStru[j].PointStatus = (UCHAR)pTempAxleSgmtStru->wRelatPointStatus[j];
					}

					funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, pTempAxleSgmtStru->wOrgnLkId, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId1);
					/*函数调用失败，退出*/
					if (1u != funcCallResult)
					{
						rtnValue = 0u;
						break;
					}
					else
					{
						/*继续往下执行*/
					}

					/*起始link和末端link相邻，wMiddleLinkId为0xFFFF*/
					if (pTempAxleSgmtStru->wTmnlLkId == linkId1)
					{
						pTempAxleSgmtStru->wMiddleLinkId[0u] = 0xFFFFu;/*150608*/
					}
					else
					{
						funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId1, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId2);
						/*函数调用失败，退出*/
						if (1u != funcCallResult)
						{
							rtnValue = 0u;
							break;
						}
						else
						{
							/*继续往下执行*/
						}

						/*中间link为linkId1*/
						if (pTempAxleSgmtStru->wTmnlLkId == linkId2)
						{
							tempLength += LinkLengthExp(g_dsuEmapStru, linkId1);
							pTempAxleSgmtStru->wMiddleLinkId[0u] = linkId1;
						}
						else
						{
							/*150608 当有linkId2也不为该计轴区段的终点Link时，继续寻找下一个Link*/
							funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId2, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId3);
							/*函数调用失败，退出*/
							if (1u != funcCallResult)
							{
								rtnValue = 0u;
								break;
							}
							else
							{
								/*继续往下执行*/
							}

							if (pTempAxleSgmtStru->wTmnlLkId == linkId3)
							{
								tempLength += LinkLengthExp(g_dsuEmapStru, linkId1);
								tempLength += LinkLengthExp(g_dsuEmapStru, linkId2);
								pTempAxleSgmtStru->wMiddleLinkId[0] = linkId1;
								pTempAxleSgmtStru->wMiddleLinkId[1] = linkId2;
							}
							else
							{
								/*支持计轴区段包含道岔数目为4 add by qxt 20160915*/
								/*当有linkId3也不为该计轴区段的终点Link时，继续寻找下一个Link*/
								funcCallResult = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId3, EMAP_SAME_DIR, (UCHAR)pTempAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId4);
								/*函数调用失败，退出*/
								if (1u != funcCallResult)
								{
									rtnValue = 0u;
									break;
								}
								else
								{
									/*继续往下执行*/
								}

								if (pTempAxleSgmtStru->wTmnlLkId == linkId4)
								{
									tempLength += LinkLengthExp(g_dsuEmapStru, linkId1);
									tempLength += LinkLengthExp(g_dsuEmapStru, linkId2);
									tempLength += LinkLengthExp(g_dsuEmapStru, linkId3);
									pTempAxleSgmtStru->wMiddleLinkId[0] = linkId1;
									pTempAxleSgmtStru->wMiddleLinkId[1] = linkId2;
									pTempAxleSgmtStru->wMiddleLinkId[2] = linkId3;
								}
								else
								{
									/*该计轴区段超过5个link,查询函数不支持这种情况*/
									rtnValue = 0u;
									break;
								}

							}
						}
					}
				}
				pTempAxleSgmtStru->dwLength = tempLength;

			} /*end  forloop*/
		}
		/****
		计算完毕  wMiddleLinkId
		****/

		/****
		计算	UINT16  wRelatAxleSgmtNum;			关联计轴区段数目
		UINT16  wRelatAxleSgmtId[4];				150608 关联计轴区段编号
		****/
		FuncRtn1 = EmapCalcFunStepCtrl(0x04);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			/*先把关联计轴数目设为0*/
			for (k = 0u; k < dsuDataLenStru->wAxleSgmtLen; k++)
			{
				pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + k;
				pTempAxleSgmtStru->wRelatAxleSgmtNum = 0u;
			}
			rtnValue = 2u;
		}
		FuncRtn1 = EmapCalcFunStepCtrl(0x05u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wAxleSgmtLen; i++)
			{
				/*当前待计算计轴区段*/
				pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;
				for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
				{
					/*临时计轴区段，不同于当前计轴区段*/
					if (j == i)
					{
						/*无操作，继续循环*/
					}
					else
					{
						pCurAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + j;
						FuncRtn2 = DetecteSameElemInTwoArray(pTempAxleSgmtStru->wMiddleLinkId, MIDDLE_LINK_MAX_NUM, pCurAxleSgmtStru->wMiddleLinkId, MIDDLE_LINK_MAX_NUM);
						/*当前计轴区段起始Link同临时计轴区段起始Link相同，
						当前计轴区段关联计轴区段为临时计轴*/
						if (pTempAxleSgmtStru->wOrgnLkId == pCurAxleSgmtStru->wOrgnLkId)
						{
							/*支持相关联计轴区段数目为16*/
							if (15u < pTempAxleSgmtStru->wRelatAxleSgmtNum)    /*wRelatAxleSgmtNum被当做数组下标*/
							{
								/*计轴区段数量错误*/
								rtnValue = 0u;
								return rtnValue;
							}
							else
							{
								pTempAxleSgmtStru->wRelatAxleSgmtId[pTempAxleSgmtStru->wRelatAxleSgmtNum] = pCurAxleSgmtStru->wId;
								pTempAxleSgmtStru->wRelatAxleSgmtNum++;
							}
						}
						/*当前计轴区段终端Link同临时计轴区段终端Link相同，
						当前计轴区段关联计轴区段为临时计轴*/
						else if (pTempAxleSgmtStru->wTmnlLkId == pCurAxleSgmtStru->wTmnlLkId)
						{
							/*支持相关联计轴区段数目为16*/
							if (15u < pTempAxleSgmtStru->wRelatAxleSgmtNum)
							{
								/*计轴区段数量错误*/
								rtnValue = 0u;
								return rtnValue;
							}
							else
							{
								pTempAxleSgmtStru->wRelatAxleSgmtId[pTempAxleSgmtStru->wRelatAxleSgmtNum] = pCurAxleSgmtStru->wId;
								pTempAxleSgmtStru->wRelatAxleSgmtNum++;
							}
						}
						/*当前计轴区段中间link存在且中间Link同临时计轴区段中间Link有相同的，当前计轴区段关联计轴区段为临时计轴*/
						else if (1u == FuncRtn2)
						{
							if (15u < pTempAxleSgmtStru->wRelatAxleSgmtNum)
							{
								/*计轴区段数量错误*/
								rtnValue = 0u;
								return rtnValue;
							}
							else
							{
								pTempAxleSgmtStru->wRelatAxleSgmtId[pTempAxleSgmtStru->wRelatAxleSgmtNum] = pCurAxleSgmtStru->wId;
								pTempAxleSgmtStru->wRelatAxleSgmtNum++;
							}
						}
						else
						{
							/*其他情况无操作，继续循环*/
						}
					}
				}

				/*循环切分检测处理*/
				FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == FuncRtn1)
				{
					rtnValue = 2u;
					break;
				}
			}
			if (i == dsuDataLenStru->wAxleSgmtLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue = 2u;
			}
		}
		/****
		计算完毕	UINT16  wRelatAxleSgmtNum;					关联计轴区段数目
		UINT16  wRelatAxleSgmtId[4];				关联计轴区段编号
		****/

		/***
		计算相关联道岔wOrgnRelatPointId
		******/
		FuncRtn1 = EmapCalcFunStepCtrl(0x06u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wAxleSgmtLen; i++)
			{
				/*计数器还得设为0*/
				AxleSgmtCount = 0u;
				/*如果当前计轴区段的（起始计轴）wOrgnAxleId是另外其他计轴区段的（终点计轴）wTmnlAxleId，
				说明，当前计轴区段前必有关联道岔  pAxleSgmtStru[]为所有关联计轴区段*/

				/*新增link逻辑方向变化点：如果当前计轴区段起始link的起点为逻辑方向变化点，则需寻找其他计轴的起点计轴*/
				pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;

				/*判断起始link的起点是否逻辑方向变化点*/
				pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wOrgnLkId];

				if (0xaau == pLinkStru->wLogicDirChanged)
				{
					for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
					{
						/*临时计轴区段，不同于当前计轴区段*/
						if (j == i)
						{
							/*无操作，继续循环*/
						}
						else
						{
							/*起点计轴相等*/
							if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wOrgnAxleId == pTempAxleSgmtStru->wOrgnAxleId)
							{
								/*临时计轴区段不能是当前计轴区段的相关联计轴区段*/
								chTempCount = 0u;
								for (k = 0u; k<pTempAxleSgmtStru->wRelatAxleSgmtNum; k++)
								{
									if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wId == pTempAxleSgmtStru->wRelatAxleSgmtId[k])
									{
										chTempCount++;
										break;
									}
								}
								if (0u == chTempCount)
								{
									/*保存该指针，并计数*/
									pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
									AxleSgmtCount++;
								}
							}
							else
							{
								/*do nothing*/
							}
						}
					}
					/*计算当前计轴区段关联道岔*/
					/*一个计轴区段前相邻16个以上的计轴区段，出错！*/
					if (16u < AxleSgmtCount)
					{
						rtnValue = 0u;
						chFlag = 0u;
					}
					else
					{
						/*计算
						计轴区段起点连接计轴区段编号
						*/
						for (k = 0u; k < AxleSgmtCount; k++)
						{
							pTempAxleSgmtStru->wOrgnAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
						}

						/*	计轴区段起点连接计轴区段编号计算完毕*/

						/**计算计轴区段起点关联道岔***/
						/*连接计轴区段个数两个以上时才有关联道岔*/
						if (1u < AxleSgmtCount)
						{
							/*相关联道岔数目初始为0*/
							PointCount = 0u;
							/*循环相连接联计轴区段求关联道岔*/
							for (n = 0u; n < AxleSgmtCount; n++)
							{
								/*判断各个计轴区段的始端link的终端相邻侧线link是否有效*/
								pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wOrgnLkId];
								/*如果计轴区段起始link的终端相邻侧线Link为有效，关联道岔*/
								if (0xFFFFu != pLinkStru->wTmnlJointSideLkId)
								{
									/*关联道岔没有被保存过*/
									FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wTmnlPointId);
									if (2u == FuncRtn1)
									{

										if (15u > PointCount)
										{
											pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
											PointCount++;
										}
										else
										{
											rtnValue = 0u;
											chFlag = 0u;
										}
									}
								}

								/*判断中间各link的终端相邻侧线Link是否为有效*/
								for (l = 0u; l < MIDDLE_LINK_MAX_NUM; l++)
								{
									if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
									{
										pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

										/*如果wMiddleLinkId的终端相邻侧线Link为有效，关联道岔*/
										if (0xFFFFu != pLinkStru->wTmnlJointSideLkId)
										{
											/*关联道岔没有被保存过*/
											FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wTmnlPointId);
											if (2u == FuncRtn1)
											{
												if (15u > PointCount)
												{
													pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
													PointCount++;
												}
												else
												{
													rtnValue = 0u;
													chFlag = 0u;
													break;
												}
											}
										}
									}
								}/*循环中间link end*/
							}/*循环相连接联计轴区段end*/
						}
					}
				}
				else/*不是起点逻辑方向变化点*/
				{
					for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
					{
						if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wTmnlAxleId == pTempAxleSgmtStru->wOrgnAxleId)
						{
							/*保存该指针，并计数*/
							pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
							AxleSgmtCount++;
						}
						else
						{
							/*do nothing*/
						}
					}
					/*计算当前计轴区段关联道岔*/
					/*一个计轴区段前相邻16个以上的计轴区段，出错！*/
					if (16u < AxleSgmtCount)
					{
						rtnValue = 0u;
						chFlag = 0u;
					}
					else
					{
						/*计算
						计轴区段起点连接计轴区段编号
						*/

						for (k = 0u; k < AxleSgmtCount; k++)
						{
							pTempAxleSgmtStru->wOrgnAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
						}

						/*	计轴区段起点连接计轴区段编号计算完毕*/
						/**计算计轴区段起点关联道岔***/

						/*连接计轴区段个数两个以上时才有关联道岔*/
						if (1u < AxleSgmtCount)
						{
							/*相关联道岔数目初始为0*/
							PointCount = 0u;
							/*循环相连接联计轴区段求关联道岔*/
							for (n = 0u; n < AxleSgmtCount; n++)
							{

								/*判断各个计轴区段的终端link的始端相邻侧线link是否有效*/
								pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wTmnlLkId];
								/*如果计轴区段起始link的始端相邻侧线Link为有效，关联道岔*/
								if (0xFFFFu != pLinkStru->wOrgnJointSideLkId)
								{
									/*关联道岔没有被保存过*/
									FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wOrgnPointId);
									if (2u == FuncRtn1)
									{

										if (15u > PointCount)
										{
											pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
											PointCount++;
										}
										else
										{
											rtnValue = 0u;
											chFlag = 0u;
										}
									}
								}

								/*判断中间各link的始端相邻侧线link是否有效*/
								for (l = 0u; l < MIDDLE_LINK_MAX_NUM; l++)
								{
									if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
									{
										pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

										/*如果wMiddleLinkId的始端相邻侧线Link为有效，关联道岔*/
										if (0xFFFFu != pLinkStru->wOrgnJointSideLkId)
										{
											/*关联道岔没有被保存过*/
											FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wOrgnRelatPointId, PointCount, pLinkStru->wOrgnPointId);
											if (2u == FuncRtn1)
											{
												if (15u > PointCount)
												{
													pTempAxleSgmtStru->wOrgnRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
													PointCount++;
												}
												else
												{
													rtnValue = 0u;
													chFlag = 0u;
													break;
												}
											}
										}
									}
								}/*循环中间link end*/
							}/*循环相连接联计轴区段end*/
						}
					}
				}/*else 不是逻辑方向变化点 end*/

				/**
				计轴区段起点关联道岔
				计算完毕***/
				
				/*循环切分检测处理*/
				FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == FuncRtn1)
				{
					rtnValue = 2u;
					break;
				}
			}/*最外层for循环end*/

			if (i == dsuDataLenStru->wAxleSgmtLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue = 2u;
			}
		}

		/*开始增加项wTmnlRelatPointId计算 150608*/
		FuncRtn1 = EmapCalcFunStepCtrl(0x07u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wAxleSgmtLen; i++)
			{
				/*计数器还得设为0*/
				AxleSgmtCount = 0u;
				/*如果当前计轴区段的（终点计轴）wTmnlAxleId是另外其他计轴区段的（起始计轴）wOrgnAxleId，
				说明，当前计轴区段后必有关联道岔*/

				/*新增link逻辑方向变化点：如果当前计轴区段终点link的终点为逻辑方向变化点，则需寻找其他计轴的终点计轴*/
				pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;

				/*判断终端link的终点是否逻辑方向变化点*/
				pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];
				if (0x55u == pLinkStru->wLogicDirChanged)
				{
					for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
					{

						/*临时计轴区段，不同于当前计轴区段*/
						if (j == i)
						{
							/*无操作，继续循环*/
						}
						else
						{
							if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wTmnlAxleId == pTempAxleSgmtStru->wTmnlAxleId)
							{
								/*临时计轴区段不能是当前计轴区段的相关联计轴区段*/
								chTempCount = 0u;
								for (k = 0u; k < pTempAxleSgmtStru->wRelatAxleSgmtNum; k++)
								{
									if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wId == pTempAxleSgmtStru->wRelatAxleSgmtId[k])
									{
										chTempCount++;
										break;
									}
								}
								if (0u == chTempCount)
								{
									/*保存该指针，并计数*/
									pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
									AxleSgmtCount++;
								}
							}
							else
							{
								/*do nothing*/
							}
						}
					}

					/*计算第i个计轴区段关联道岔*/ /*还在for循环中*/
					/*一个计轴区段后相邻16个以上的计轴区段，出错！*/
					if (16u < AxleSgmtCount)
					{
						rtnValue = 0u;
						chFlag = 0u;
					}
					else
					{
						/*计算
						计轴区段终点连接计轴区段编号
						*/
						for (k = 0u; k < AxleSgmtCount; k++)
						{
							pTempAxleSgmtStru->wTmnlAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
						}
						/****
						计轴区段终点连接计轴区段编号
						计算完毕
						***/

						/****计算终点关联道岔******/

						/*连接计轴区段个数两个以上时才有关联道岔*/
						if (1u < AxleSgmtCount)
						{
							/*相关联道岔计数器设为0*/
							PointCount = 0u;
							/*循环关联计轴区段求关联道岔*/
							for (n = 0u; n < AxleSgmtCount; n++)
							{
								/*判断各个计轴区段的终端link的始端相邻侧线link是否有效*/
								pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wTmnlLkId];
								/*如果计轴区段终端link的始端相邻侧线Link为有效，关联道岔*/
								if (0xFFFFu != pLinkStru->wOrgnJointSideLkId)
								{
									/*关联道岔没有被保存过*/
									FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wOrgnPointId);
									if (2u == FuncRtn1)
									{
										if (15u > PointCount)
										{
											pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
											PointCount++;
										}
										else
										{
											rtnValue = 0u;
											chFlag = 0u;
										}
									}
								}

								/*判断中间各link的始端相邻侧线link是否有效*/
								for (l = 0u; l < MIDDLE_LINK_MAX_NUM; l++)
								{
									if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
									{
										pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

										/*如果wMiddleLinkId的始端相邻侧线Link为有效，关联道岔*/
										if (0xFFFFu != pLinkStru->wOrgnJointSideLkId)
										{
											/*关联道岔没有被保存过*/
											FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wOrgnPointId);
											if (2u == FuncRtn1)
											{
												if (15u > PointCount)
												{
													pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wOrgnPointId;
													PointCount++;
												}
												else
												{
													rtnValue = 0u;
													chFlag = 0u;
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				else/*非逻辑方向变化点*/
				{
					for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
					{
						if ((dsuStaticHeadStru->pAxleSgmtStru + j)->wOrgnAxleId == pTempAxleSgmtStru->wTmnlAxleId)
						{
							/*保存该指针，并计数*/
							pAxleSgmtStru[AxleSgmtCount] = dsuStaticHeadStru->pAxleSgmtStru + j;
							AxleSgmtCount++;
						}
						else
						{
							/*do nothing*/
						}
					}

					/*计算第i个计轴区段关联道岔*/ /*还在for循环中*/
									  /*一个计轴区段后相邻16个以上的计轴区段，出错！*/
					if (16u < AxleSgmtCount)
					{
						rtnValue = 0u;
						chFlag = 0u;
					}
					else
					{
						/*计算
						计轴区段终点连接计轴区段编号
						*/
						for (k = 0u; k < AxleSgmtCount; k++)
						{
							pTempAxleSgmtStru->wTmnlAxleSgmtId[k] = pAxleSgmtStru[k]->wId;
						}
						/****
						计轴区段终点连接计轴区段编号
						计算完毕
						***/

						/****计算终点关联道岔******/

						/*连接计轴区段个数两个以上时才有关联道岔*/
						if (1u < AxleSgmtCount)
						{
							/*相关联道岔数目初始为0*/
							PointCount = 0u;
							/*循环关联计轴区段求关联道岔*/
							for (n = 0u; n < AxleSgmtCount; n++)
							{
								/*判断各个计轴区段的起始link的终端相邻侧线link是否有效*/
								pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wOrgnLkId];
								/*如果计轴区段起始link的终端相邻侧线Link为有效，关联道岔*/
								if (0xFFFFu != pLinkStru->wTmnlJointSideLkId)
								{
									/*关联道岔没有被保存过*/
									FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wTmnlPointId);
									if (2u == FuncRtn1)
									{
										if (15u > PointCount)
										{
											pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
											PointCount++;
										}
										else
										{
											rtnValue = 0u;
											chFlag = 0u;
										}
									}
								}

								/*判断中间link的终端相邻侧线link是否有效*/
								for (l = 0u; l < MIDDLE_LINK_MAX_NUM; l++)
								{
									if (0xFFFFu != pAxleSgmtStru[n]->wMiddleLinkId[l])/*存在中间link*/
									{
										pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pAxleSgmtStru[n]->wMiddleLinkId[l]];

										/*如果wMiddleLinkId的始端相邻侧线Link为有效，关联道岔*/
										if (0xFFFFu != pLinkStru->wTmnlJointSideLkId)
										{
											/*关联道岔没有被保存过*/
											FuncRtn1 = DetecteElementInArray(pTempAxleSgmtStru->wTmnlRelatPointId, PointCount, pLinkStru->wTmnlPointId);
											if (2u == FuncRtn1)
											{
												if (15u > PointCount)
												{
													pTempAxleSgmtStru->wTmnlRelatPointId[PointCount] = pLinkStru->wTmnlPointId;
													PointCount++;
												}
												else
												{
													rtnValue = 0u;
													chFlag = 0u;
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}/*else end*/

				/****计算完毕  终点关联道岔******/
				FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == FuncRtn1)
				{
					rtnValue = 2u;
					break;
				}
			}
			if ((i == dsuDataLenStru->wAxleSgmtLen) && (0u != chFlag))
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue = 1u;
			}
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}

	return rtnValue;
}


/*访问接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
/*移除互联互通、车车以外的接口 by lyk 20220307*/
UINT8 AxleSgmtStru(void)
{
	UINT8 rtnValue = 0u;
	UINT8 cbtcVersionType = 0u;
	cbtcVersionType = GetCbtcSysType();

	if (DQU_CBTC_HLT == cbtcVersionType)
	{
		rtnValue = AxleSgmtStru_HLT();
	}
	else if (DQU_CBTC_CC == cbtcVersionType)
	{
		rtnValue = AxleSgmtStru_CC();
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/*逻辑区段动态计算, copy from HLT kuan.he 20220308*/
static UINT8 LogicSgmtStru_CC(void)
{
	UINT8 RtnNo = 0u;	/*初始化为失败*/
	UINT8 CallFuncRtn = 0u;
	UINT16 i = 0u, j = 0u, k = 0u, n = 0u, LgcSgmtIdIndex = 0xFFFFu;	/*循环变量*/
	UINT8 breakTag = DSU_FALSE;											/*跳出循环标志位*/
	DQU_POINT_STATUS_STRU  pointStatusStru[4u] = { 0 };					/*道岔状态结构体数组*//*3-->4 支持一个计轴区段包含4个道岔 modify by qxt 20160915 */
	UINT16 linkId1 = 0u, linkId2 = 0u, linkId3 = 0u, linkId4 = 0u;		/*计轴区段计算用到的临时变量*//*add linkId4 支持计轴区段包含道岔数目为4 modify by qxt 20160915*/
	UINT8 FuncRtn1 = 0u;
	UINT8 OrgnDirChangedTag = 0xFFu;	/*起点为link逻辑方向变化点标志*/
	UINT8 TmnlDirChangedTag = 0xFFu;	/*终点为link逻辑方向变化点标志*/

	/*电子地图指针*/
	DSU_STATIC_HEAD_STRU *pDsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *pDsuDataLenStru = NULL;
	DSU_LINK_STRU *pDsuLinkStru = NULL;										/*link表*/
	UINT16 *pDsuLinkIndex = NULL, *pDsuLogicSgmtIndex = NULL;				/*link索引*/
	DSU_LOGIC_SGMT_STRU *pDsuLogicSgmtStru, *pDsuLogicSgmtStruPrev = NULL;	/*逻辑区段表*/
	DSU_AXLE_SGMT_STRU  *pDsuAxleSgmtStru = NULL;							/*计轴区段表*/
	UINT16 *pDsuAxleSgmtIndex = NULL;										/*计轴区段索引*/

	/*指针防空*/
	if (NULL != g_dsuEmapStru)
	{
		/*表头索引*/
		pDsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;

		/*结构体数量指针*/
		pDsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;

		/*link表索引*/
		pDsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;

		/*计轴区段表指针*/
		pDsuAxleSgmtStru = pDsuStaticHeadStru->pAxleSgmtStru;
		pDsuAxleSgmtIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuAxleSgmtIndex;

		/*逻辑区段索引*/
		pDsuLogicSgmtIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex;
		FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			/*1、计算逻辑区段对应的计轴区段*/
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < pDsuDataLenStru->wLogicSgmtLen; i++)
			{
				/*获取当前逻辑区段*/
				pDsuLogicSgmtStru = pDsuStaticHeadStru->pLogicSgmtStru + i;
				pDsuLogicSgmtStru->wAxleSgmtNum = 0U;
				/*遍历计轴区段表*/
				for (j = 0u; j < pDsuDataLenStru->wAxleSgmtLen; j++)
				{
					/*拿到计轴区段结构体指针*/
					pDsuAxleSgmtStru = pDsuStaticHeadStru->pAxleSgmtStru + j;

					/*找到计轴器表中的包含逻辑区段*/
					for (k = 0u; k < pDsuAxleSgmtStru->wIncludeLogicSgmtNum; k++)
					{
						if (pDsuLogicSgmtStru->wId == pDsuAxleSgmtStru->wLogicSgmtId[k])
						{
							if (MAX_ADJACENT_LOGICSGMT_NUM > pDsuLogicSgmtStru->wAxleSgmtNum)
							{
								pDsuLogicSgmtStru->wAxleSgmtId[pDsuLogicSgmtStru->wAxleSgmtNum] = pDsuAxleSgmtStru->wId;
								pDsuLogicSgmtStru->wAxleSgmtNum += 1U;
							}
							else
							{
								/*数组越界防护*/
								RtnNo = 0u;
							}
							break;
						}
						else
						{
							/*do nothing*/
						}
					}
				}

				if (0U == pDsuLogicSgmtStru->wAxleSgmtNum)
				{
					/*逻辑区段没有对应的计轴区段，报错*/
					RtnNo = 0U;
				}
				else
				{
					/*do nothing*/
				}
			}
			/*end  forloop*/
			if (i == pDsuDataLenStru->wLogicSgmtLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				RtnNo = 1u;
			}

			/*2、计算中间Link和相邻计轴区段*/
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < pDsuDataLenStru->wLogicSgmtLen; i++)
			{
				pDsuLogicSgmtStru = pDsuStaticHeadStru->pLogicSgmtStru + i;
				/*计算middlelink可以任取一个所属计轴区段*/
				pDsuAxleSgmtStru = pDsuStaticHeadStru->pAxleSgmtStru + pDsuAxleSgmtIndex[pDsuLogicSgmtStru->wAxleSgmtId[0]];

				/*150608 初始化中间Link数组为0xFFFF*/
				for (n = 0u; n < MIDDLE_LINK_MAX_NUM; n++)
				{
					pDsuLogicSgmtStru->wMiddleLinkId[n] = 0xFFFFu;
				}

				/*起始link和末端link为同一link，wMiddleLinkId为0xFFFF*/
				if (pDsuLogicSgmtStru->wOrgnLkId == pDsuLogicSgmtStru->wTmnlLkId)
				{
					pDsuLogicSgmtStru->wMiddleLinkId[0] = 0xFFFFu;
				}
				else
				{
					/*不在同一link*/
					for (j = 0u; j < pDsuAxleSgmtStru->wRelatPointNum; j++)
					{
						pointStatusStru[j].PointId = pDsuAxleSgmtStru->wRelatPointId[j];
						pointStatusStru[j].PointStatus = (UCHAR)pDsuAxleSgmtStru->wRelatPointStatus[j];
					}

					CallFuncRtn = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, pDsuLogicSgmtStru->wOrgnLkId, EMAP_SAME_DIR, (UCHAR)pDsuAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId1);
					if (1u == CallFuncRtn)
					{
						/*起始link和末端link相邻，wMiddleLinkId为0xFFFF*/
						if (pDsuLogicSgmtStru->wTmnlLkId == linkId1)
						{
							pDsuLogicSgmtStru->wMiddleLinkId[0] = 0xFFFFu;
						}
						else
						{
							CallFuncRtn = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId1, EMAP_SAME_DIR, (UCHAR)pDsuAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId2);
							/*dsuGetAdjacentLinkID执行失败，退出*/
							if (1u != CallFuncRtn)
							{
								RtnNo = 0u;
								break;
							}
							else
							{
								/*继续向下执行*/
							}

							if (pDsuLogicSgmtStru->wTmnlLkId == linkId2)
							{
								pDsuLogicSgmtStru->wMiddleLinkId[0] = linkId1;
							}
							else
							{
								CallFuncRtn = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId2, EMAP_SAME_DIR, (UCHAR)pDsuAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId3);
								/*dsuGetAdjacentLinkID执行失败，退出*/
								if (1u != CallFuncRtn)
								{
									RtnNo = 0u;
									break;
								}
								else
								{
									/*继续向下执行*/
								}

								if (pDsuLogicSgmtStru->wTmnlLkId == linkId3)
								{
									pDsuLogicSgmtStru->wMiddleLinkId[0u] = linkId1;
									pDsuLogicSgmtStru->wMiddleLinkId[1u] = linkId2;
								}
								else
								{
									/*支持计轴区段包含道岔数目为4 add by qxt 20160915*/
									/*当有linkId3也不为该计轴区段的终点Link时，继续寻找下一个Link*/
									CallFuncRtn = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, linkId3, EMAP_SAME_DIR, (UCHAR)pDsuAxleSgmtStru->wRelatPointNum, pointStatusStru, &linkId4);

									/*函数调用失败，退出*/
									if (1u != CallFuncRtn)
									{
										RtnNo = 0u;
										break;
									}
									else
									{
										/*继续往下执行*/
									}

									if (pDsuLogicSgmtStru->wTmnlLkId == linkId4)
									{
										pDsuLogicSgmtStru->wMiddleLinkId[0u] = linkId1;
										pDsuLogicSgmtStru->wMiddleLinkId[1u] = linkId2;
										pDsuLogicSgmtStru->wMiddleLinkId[2u] = linkId3;
									}
									else
									{
										/*该逻辑区段超过5个link,查询函数不支持这种情况*/
										RtnNo = 0u;
										break;
									}
								}
							}
						}
					}
					else
					{
						/*函数调用失败，退出*/
						RtnNo = 0u;
						break;
					}
				}

				/*计算
				wMainOrgnLogicSgmtId;				所属计轴区段内前向逻辑区段的编号
				wMainTmnlLogicSgmtId;				所属计轴区段内后向逻辑区段的编号
				*/
				/*先设为无效*/
				LgcSgmtIdIndex = 0xFFFFu;
				for (k = 0u; k < pDsuAxleSgmtStru->wIncludeLogicSgmtNum; k++)
				{
					if (pDsuLogicSgmtStru->wId == pDsuAxleSgmtStru->wLogicSgmtId[k])
					{
						LgcSgmtIdIndex = k;
					}
				}

				/*逻辑区段编号在计轴区段中没找到，数据出错*/
				if (LgcSgmtIdIndex == 0xFFFF)
				{
					RtnNo = 0u;
					break;
				}
				/*当计轴区段只有一个逻辑区段时，没有前向逻辑区段和后向逻辑区段*/
				else if ((0u == LgcSgmtIdIndex) && (LgcSgmtIdIndex == (pDsuAxleSgmtStru->wIncludeLogicSgmtNum - 1u)))
				{
					pDsuLogicSgmtStru->wMainOrgnLogicSgmtId = 0xFFFFu;
					pDsuLogicSgmtStru->wMainTmnlLogicSgmtId = 0xFFFFu;
				}
				/*当前逻辑区段处于计轴区段的第一个逻辑区段时，没有前向逻辑区段*/
				else if (0u == LgcSgmtIdIndex)
				{
					pDsuLogicSgmtStru->wMainOrgnLogicSgmtId = 0xFFFFu;
					pDsuLogicSgmtStru->wMainTmnlLogicSgmtId = pDsuAxleSgmtStru->wLogicSgmtId[LgcSgmtIdIndex + 1u];
				}
				/*当前逻辑区段处于计轴区段的最后一个逻辑区段时，没有后向逻辑区段*/
				else if (LgcSgmtIdIndex == (pDsuAxleSgmtStru->wIncludeLogicSgmtNum - 1u))
				{
					pDsuLogicSgmtStru->wMainOrgnLogicSgmtId = pDsuAxleSgmtStru->wLogicSgmtId[LgcSgmtIdIndex - 1u];
					pDsuLogicSgmtStru->wMainTmnlLogicSgmtId = 0xFFFFu;
				}
				/*当前逻辑区段处于计轴区段的中间逻辑区段时段*/
				else
				{
					pDsuLogicSgmtStru->wMainOrgnLogicSgmtId = pDsuAxleSgmtStru->wLogicSgmtId[LgcSgmtIdIndex - 1u];
					pDsuLogicSgmtStru->wMainTmnlLogicSgmtId = pDsuAxleSgmtStru->wLogicSgmtId[LgcSgmtIdIndex + 1u];
				}

				/*循环分步切割处理*/
				FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == FuncRtn1)
				{
					RtnNo = 2u;
					break;
				}

			}
			/*end  forloop*/
			if (i == pDsuDataLenStru->wLogicSgmtLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				RtnNo = 1u;
			}

			/*3、计算逻辑区段的相邻逻辑区段*/
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < pDsuDataLenStru->wLogicSgmtLen; i++)
			{
				/*获取当前逻辑区段*/
				pDsuLogicSgmtStru = pDsuStaticHeadStru->pLogicSgmtStru + i;
				CallFuncRtn = dsuGetAdjacentLogicSgmtImp(g_dsuEmapStru, pDsuLogicSgmtStru->wId, EMAP_CONVER_DIR, 
					&pDsuLogicSgmtStru->wOrgnLogicSgmtNum, pDsuLogicSgmtStru->wOrgnLogicSgmtId, &OrgnDirChangedTag);
				CallFuncRtn &= dsuGetAdjacentLogicSgmtImp(g_dsuEmapStru, pDsuLogicSgmtStru->wId, EMAP_SAME_DIR, 
					&pDsuLogicSgmtStru->wTmnlLogicSgmtNum, pDsuLogicSgmtStru->wTmnlLogicSgmtId, &TmnlDirChangedTag);
				if (1u == CallFuncRtn)
				{
					if (EMAP_DIRCHANGE_ORGN2ORGN == OrgnDirChangedTag)
					{
						pDsuLogicSgmtStru->wDirChangedTag = EMAP_DIRCHANGE_ORGN2ORGN;
					}
					else if (EMAP_DIRCHANGE_TMNL2TMNL == TmnlDirChangedTag)
					{
						pDsuLogicSgmtStru->wDirChangedTag = EMAP_DIRCHANGE_TMNL2TMNL;
					}
					else
					{
						pDsuLogicSgmtStru->wDirChangedTag = 0xFFu;
					}
				}
				else
				{
					RtnNo = 0u;
					break;
				}
			}
			/*end  forloop*/
			if (i == pDsuDataLenStru->wLogicSgmtLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				RtnNo = 1u;
			}

			/*将逻辑区段写入对应的结构体*/
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < pDsuDataLenStru->wLogicSgmtLen; i++)
			{
				pDsuLogicSgmtStru = pDsuStaticHeadStru->pLogicSgmtStru + i;
				pDsuLinkStru = pDsuStaticHeadStru->pLinkStru + pDsuLinkIndex[pDsuLogicSgmtStru->wOrgnLkId];

				/*写入起点*/
				pDsuLinkStru->LogicSgmtIdBuff[pDsuLinkStru->LogicSgmtNum] = pDsuLogicSgmtStru->wId;
				pDsuLinkStru->LogicSgmtNum++;

				/*如果存在中间link，一并写入*/
				j = 0u;	/*重置循环变量*/
				while ((0u != pDsuLogicSgmtStru->wMiddleLinkId[j]) && (0xFFFFu != pDsuLogicSgmtStru->wMiddleLinkId[j]) && (MIDDLE_LINK_MAX_NUM > j))
				{
					/*写入中间link*/
					pDsuLinkStru = pDsuStaticHeadStru->pLinkStru + pDsuLinkIndex[pDsuLogicSgmtStru->wMiddleLinkId[j]];
					pDsuLinkStru->LogicSgmtIdBuff[pDsuLinkStru->LogicSgmtNum] = pDsuLogicSgmtStru->wId;
					pDsuLinkStru->LogicSgmtNum++;
					j++;
				}

				/*写入终点*/
				if (pDsuLogicSgmtStru->wOrgnLkId != pDsuLogicSgmtStru->wTmnlLkId)
				{
					pDsuLinkStru = pDsuStaticHeadStru->pLinkStru + pDsuLinkIndex[pDsuLogicSgmtStru->wTmnlLkId];
					pDsuLinkStru->LogicSgmtIdBuff[pDsuLinkStru->LogicSgmtNum] = pDsuLogicSgmtStru->wId;
					pDsuLinkStru->LogicSgmtNum++;
				}
				else
				{
					/*起点与终点link相同，不写入终点*/
				}
			}

		}
		else
		{
			/*初始步骤不正确,等待*/
			RtnNo = 2u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		RtnNo = 0u;
	}

	return RtnNo;
}

/*访问接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
UINT8 LogicSgmtStru(void)
{
	UINT8 rtnValue = 0u;
	UINT8 cbtcVersionType = 0u;
	cbtcVersionType = GetCbtcSysType();

	if (DQU_CBTC_CC == cbtcVersionType)
	{
		/*逻辑区段动态处理*/
		rtnValue = LogicSgmtStru_CC();
	}
	else
	{
		rtnValue = 0u;
	}

	return rtnValue;
}

/*********************************************
*函数功能：将逻辑区段的属性上添加所属进路
*参数说明：无
*返回值：0，失败；1，成功
*修改记录：创建，kuan.he 20230515
备注：			
*********************************************/
UINT8 LogicSgmtAddRouteId_CC(void)
{
	UINT8 RtnNo = 1u;
	UINT16 i = 0u;
	UINT16 j = 0u;
	DSU_ROUTE_STRU* pCurrRouteStru = NULL;							/*进路信息结构体指针*/
	DSU_LOGIC_SGMT_STRU* pCurrLogicSgmtStru = NULL;						/*逻辑区段信息结构体指针*/

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru))
	{
		for (i = 0; i < g_dsuEmapStru->dsuDataLenStru->wRouteLen; i++)
		{
			/*遍历进路表*/
			pCurrRouteStru = g_dsuEmapStru->dsuStaticHeadStru->pRouteStru + i;

			if (NULL != pCurrRouteStru)
			{
				/*遍历进路中的逻辑区段*/
				for (j = 0; j < pCurrRouteStru->wIncludeLogicSgmtNum; j++)
				{
					/*直接访问对应的逻辑区段*/
					pCurrLogicSgmtStru = g_dsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + g_dsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pCurrRouteStru->wLogicSgmtId[j]];

					if (NULL != pCurrLogicSgmtStru)
					{
						/*将所属进路id添加到对应的逻辑区段中*/
						if (0xFFu == pCurrLogicSgmtStru->RouteNum)
						{
							/*初始化进路id数量*/
							pCurrLogicSgmtStru->RouteNum = 0u;
						}
						else
						{
							/*do nothing*/
						}

						/*检查是否越界*/
						if (pCurrLogicSgmtStru->RouteNum >= (sizeof(pCurrLogicSgmtStru->RouteIdBuff) / sizeof(UINT16)))
						{
							/*已满*/
							RtnNo &= 0u;
							break;
						}
						else
						{
							pCurrLogicSgmtStru->RouteIdBuff[pCurrLogicSgmtStru->RouteNum] = pCurrRouteStru->wId;
							pCurrLogicSgmtStru->RouteNum++;
						}
						
					}
					else
					{
						/*指针为空*/
						RtnNo &= 0u;
						break;
					}
				}
			}
			else
			{
				/*指针为空*/
				RtnNo &= 0u;
				break;
			}
		}
	}
	else
	{
		/*指针为空*/
		RtnNo &= 0u;
	}


	return RtnNo;
}

/*********************************************
*函数功能：进路表数据结构
*参数说明：无
*返回值：0，失败；1，成功
*修改记录：创建，wyd 20220214
备注：内部调用函数，车车使用
修改记录:
	1. 新增在逻辑区段信息中添加所属进路信息 kuan,he 20230515
*********************************************/
static UINT8 RouteStru_CC(void)
{
	UINT16 i = 0u;												/*循环变量*/
	UINT16 j = 0u;												/*循环变量*/
	UINT16 k = 0u;												/*循环变量*/
	UINT16 m = 0u;												/*循环变量*/
	UINT16 LogicSgmtIndex = 0;									/*循环变量*/
	DSU_ROUTE_STRU* pRouteStru = NULL;							/*进路指针*/
	UINT8 funcRtr = 0u;											/*函数调用返回值*/
	DQU_POINT_STATUS_STRU PointStatus[16u] = { 0u };			/*道岔位置*/
	UINT16 linkIndexSame = 0u;									/*link数组下标索引（同向查找）*/
	UINT16 linkIndexConvert = 0u;								/*link数组下标索引（反向查找）*/
	UINT16 tempLinkIdSame[DSU_MAX_LINK_BETWEEN] = { 0u };		/*中间link数组（同向查找）*/
	UINT16 tempLinkIdConvert[DSU_MAX_LINK_BETWEEN] = { 0u };	/*中间link数组（反向查找）*/
	UINT8 Dirction = 0u;										/*进路方向，为起点信号机防护方向，临时变量*/
	UINT8 rtnValue = 2u;										/*返回值*/
	LOD_STRU StartPointSameDir = { 0 };							/*查找的正方向起点*/
	LOD_STRU StartPointConvertDir = { 0 };						/*查找的反方向起点*/
	LOD_STRU EndPoint = { 0 };									/*查找的终点*/
	UINT8 FunRtnSame = 0u;										/*正方向查找结果*/
	UINT8 FunRtnConvert = 0u;									/*反方向查找结果*/
	DSU_LINK_STRU *pLinkStru = NULL;							/*Link指针*/
	DSU_PROTECT_LINK_STRU *pProtectLinkStru = NULL;				/*保护区段指针*/
	DSU_AXLE_SGMT_STRU *pAxleSgmtStru = NULL;					/*计轴区段指针*/
	DSU_SIGNAL_STRU *pDsuSignalStruStart = NULL;
	DSU_SIGNAL_STRU *pDsuSignalStruEnd = NULL;
	UINT32 ShortestLength = 0u;									/*最短保护区段长度*/
	UINT32 tempLength = 0u;										/*长度临时变量*/
	UINT32 tempLength2 = 0u;									/*长度临时变量*/
	UINT8  FlagPointMain = DSU_FALSE;							/*区段内有定位道岔标志*/
	UINT8  FlagFindAllPointSide = DSU_FALSE;					/*找到了道岔全为反位的保护区段标志*/
	UINT8 tempPointNum = 0u;									/*道岔都在反位的保护区段内道岔数量*/
	DQU_POINT_STATUS_STRU tempPointInfo[16u] = { 0 };			/*道岔都在反位的保护区段内道岔信息*/
	LOD_STRU RouteEndLoc = { 0 };								/*进路终点*/
	LOD_STRU MaEndLoc = { 0 };									/*MA终点*/
	DQU_POINT_LOC_STRU tempPointLoc = { 0 };					/*道岔位置信息*/
	DSU_LOGIC_SGMT_STRU *pLogicSgmtStru = NULL;					/*逻辑区段指针*/
	UINT32 tempOffset = 0u;										/*偏移量临时变量*/
	UINT16 ResultLogicSgmtNum = 0u;								/*所求逻辑区段数量*/
	UINT16 ResultLogicSgmtId[50u] = { 0 };						/*所求逻辑区段序列*/
	UINT16 ResultLogicSgmtDir[50u] = { 0 };						/*所求逻辑区段方向序列*/
	UINT8 FindLogicSgmtFlag = DSU_FALSE;						/*找到逻辑区段标志*/
	LOD_STRU tempStartLoc = { 0 };								/*进路起点临时变量*/
	LOD_STRU tempEndLoc = { 0 };								/*进路终点临时变量*/
	DQU_POINT_STATUS_STRU tempPointStatus[16] = { 0 };			/*进路内道岔状态临时变量*/
	UINT16 wLinkId = 0u;										/*link ID*/

	/*电子地图数据*/
	UINT16 *dsuLinkIndex = NULL;
	UINT16 *dsuSignalIndex = NULL;
	UINT16 *dsuProtectLinkIndex = NULL;
	UINT16 *dsuAxleSgmtLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru))
	{
		dsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuSignalIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuSignalIndex;
		dsuProtectLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuProtectLinkIndex;
		dsuAxleSgmtLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuAxleSgmtIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;

		/*********
		1、计算link序列及长度
		**********/
		funcRtr = EmapCalcFunStepCtrl(0x03u);
		if (DQU_QFUN_TRUE == funcRtr)
		{
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wRouteLen; i++)
			{
				pRouteStru = dsuStaticHeadStru->pRouteStru + i;


				if (pRouteStru->wOrgnLkId == pRouteStru->wTmnlLkId)
				{
					/*起点、终点在同一Link*/
					pRouteStru->wIncludeLinkNum = 1u;
					pRouteStru->wLinkId[0u] = pRouteStru->wOrgnLkId;
				}
				else
				{
					StartPointSameDir.Lnk = pRouteStru->wOrgnLkId;
					StartPointSameDir.Off = 0u;
					StartPointSameDir.Dir = EMAP_SAME_DIR;

					StartPointConvertDir.Lnk = pRouteStru->wOrgnLkId;
					StartPointConvertDir.Off = 0u;
					StartPointConvertDir.Dir = EMAP_CONVER_DIR;

					EndPoint.Lnk = pRouteStru->wTmnlLkId;
					EndPoint.Off = 0u;
					EndPoint.Dir = EMAP_SAME_DIR;

					for (j = 0u; j < 16u; j++)
					{
						PointStatus[j].PointId = pRouteStru->wPointId[j];
						PointStatus[j].PointStatus = (UCHAR)pRouteStru->wPointStatus[j];
					}

					/*按照起点的正方向查找*/
					FunRtnSame = dsuGetLinkBtwLocExp(g_dsuEmapStru, &StartPointSameDir, &EndPoint, 16u, PointStatus, &tempLinkIdSame[0], &linkIndexSame);
					/*按照起点的反方向查找*/
					FunRtnConvert = dsuGetLinkBtwLocExp(g_dsuEmapStru, &StartPointConvertDir, &EndPoint, 16u, PointStatus, &tempLinkIdConvert[0], &linkIndexConvert);

					if (1u == FunRtnSame)
					{
						/*按照起点的正方向查找成功*/
						pRouteStru->wLinkId[0u] = pRouteStru->wOrgnLkId;
						CommonMemCpy(&pRouteStru->wLinkId[1u], linkIndexSame * sizeof(UINT16), &tempLinkIdSame[0u], linkIndexSame * sizeof(UINT16));
						pRouteStru->wLinkId[linkIndexSame + 1u] = pRouteStru->wTmnlLkId;
						pRouteStru->wIncludeLinkNum = linkIndexSame + 2u;
					}
					else if (1u == FunRtnConvert)
					{
						/*按照起点的反方向查找成功*/
						pRouteStru->wLinkId[0u] = pRouteStru->wOrgnLkId;
						CommonMemCpy(&pRouteStru->wLinkId[1u], linkIndexConvert * sizeof(UINT16), &tempLinkIdConvert[0u], linkIndexConvert * sizeof(UINT16));
						pRouteStru->wLinkId[linkIndexConvert + 1u] = pRouteStru->wTmnlLkId;
						pRouteStru->wIncludeLinkNum = linkIndexConvert + 2u;
					}
					else
					{
						/*函数调用出错*/
						rtnValue = 0u;
						break;
					}
				}

				/*计算进路长度，wyd 20220216*/
				/*先将进路长度置0，再遍历Link累加*/
				pRouteStru->dwLength = 0u;
				for (j = 0u; j < pRouteStru->wIncludeLinkNum; j++)
				{
					pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pRouteStru->wLinkId[j]];
					pRouteStru->dwLength += pLinkStru->dwLength;
				}

				/*查询进路起点*/
				pDsuSignalStruStart = dsuStaticHeadStru->pSignalStru + dsuSignalIndex[pRouteStru->wStartSignalId];
				pDsuSignalStruEnd = dsuStaticHeadStru->pSignalStru + dsuSignalIndex[pRouteStru->wEndSignalId];
				tempStartLoc.Lnk = pDsuSignalStruStart->wProtectLinkId;
				tempStartLoc.Off = pDsuSignalStruStart->dwProtectLinkOfst;
				tempStartLoc.Dir = (UINT8)pDsuSignalStruStart->wProtectDir;
				tempEndLoc.Lnk = pDsuSignalStruEnd->wProtectLinkId;
				tempEndLoc.Off = pDsuSignalStruEnd->dwProtectLinkOfst;
				tempEndLoc.Dir = (UINT8)pDsuSignalStruEnd->wProtectDir;

				if (tempStartLoc.Lnk == pRouteStru->wOrgnLkId)
				{
					/*始端信号机的防护点就在第一个link，无需再处理*/
					pRouteStru->wStartLocOfst = tempStartLoc.Off;
					pRouteStru->wStartLocDir = tempStartLoc.Dir;
				}
				else
				{
					/*始端信号机的防护点不在第一个link*/
					funcRtr = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, tempStartLoc.Lnk, tempStartLoc.Dir, 16u, PointStatus, &wLinkId);
					if (1u == funcRtr)
					{
						if (wLinkId == pRouteStru->wOrgnLkId)
						{
							pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[tempStartLoc.Lnk];
							if (EMAP_SAME_DIR == tempStartLoc.Dir)
							{
								/*防护点必在link最大偏移量*/
								if (LinkLengthExp(g_dsuEmapStru, tempStartLoc.Lnk) == tempStartLoc.Off)
								{
									if (EMAP_DIRCHANGE_TMNL2TMNL == pLinkStru->wLogicDirChanged)
									{
										/*进路起点是方向变化点*/
										pRouteStru->wStartLocOfst = LinkLengthExp(g_dsuEmapStru, wLinkId);
										pRouteStru->wStartLocDir = EMAP_CONVER_DIR;
									}
									else
									{
										/*进路起点不是方向变化点*/
										pRouteStru->wStartLocOfst = 0u;
										pRouteStru->wStartLocDir = EMAP_SAME_DIR;
									}
								}
								else
								{
									/*函数调用出错*/
									rtnValue = 0u;
									break;
								}
							}
							else if (EMAP_CONVER_DIR == tempStartLoc.Dir)
							{
								/*防护点必在0偏移量*/
								if (0u == tempStartLoc.Off)
								{
									if (EMAP_DIRCHANGE_ORGN2ORGN == pLinkStru->wLogicDirChanged)
									{
										/*进路起点是方向变化点*/
										pRouteStru->wStartLocOfst = 0u;
										pRouteStru->wStartLocDir = EMAP_SAME_DIR;
									}
									else
									{
										/*进路起点不是方向变化点*/
										pRouteStru->wStartLocOfst = LinkLengthExp(g_dsuEmapStru, wLinkId);
										pRouteStru->wStartLocDir = EMAP_CONVER_DIR;
									}
								}
								else
								{
									/*函数调用出错*/
									rtnValue = 0u;
									break;
								}
							}
							else
							{
								/*函数调用出错*/
								rtnValue = 0u;
								break;
							}
						}
						else
						{
							/*函数调用出错*/
							rtnValue = 0u;
							break;
						}
					}
					else
					{
						/*函数调用出错*/
						rtnValue = 0u;
						break;
					}
				}

				if (tempEndLoc.Lnk == pRouteStru->wTmnlLkId)
				{
					/*终端信号机的防护点就在第一个link，无需再处理*/
					pRouteStru->wEndLocOfst = tempEndLoc.Off;
					pRouteStru->wEndLocDir = tempEndLoc.Dir;
				}
				else
				{
					/*终端信号机的防护点不在最后一个link*/
					funcRtr = dsuGetAdjacentLinkIDExp(g_dsuEmapStru, tempEndLoc.Lnk, 
						((EMAP_SAME_DIR == tempEndLoc.Dir) ? EMAP_CONVER_DIR : EMAP_SAME_DIR), 16u, PointStatus, &wLinkId);
					if (1u == funcRtr)
					{
						if (wLinkId == pRouteStru->wTmnlLkId)
						{
							pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[tempEndLoc.Lnk];
							if (EMAP_SAME_DIR == tempEndLoc.Dir)
							{
								/*防护点必在0偏移量*/
								if (0u == tempEndLoc.Off)
								{
									if (EMAP_DIRCHANGE_ORGN2ORGN == pLinkStru->wLogicDirChanged)
									{
										/*进路终点是方向变化点*/
										pRouteStru->wEndLocOfst = 0u;
										pRouteStru->wEndLocDir = EMAP_CONVER_DIR;
									}
									else
									{
										/*进路终点不是方向变化点*/
										pRouteStru->wEndLocOfst = LinkLengthExp(g_dsuEmapStru, wLinkId);
										pRouteStru->wEndLocDir = EMAP_SAME_DIR;
									}
								}
								else
								{
									/*函数调用出错*/
									rtnValue = 0u;
									break;
								}
							}
							else if (EMAP_CONVER_DIR == tempEndLoc.Dir)
							{
								/*防护点必在link最大偏移量*/
								if (LinkLengthExp(g_dsuEmapStru, tempEndLoc.Lnk) == tempEndLoc.Off)
								{
									if (EMAP_DIRCHANGE_TMNL2TMNL == pLinkStru->wLogicDirChanged)
									{
										/*进路终点是方向变化点*/
										pRouteStru->wEndLocOfst = LinkLengthExp(g_dsuEmapStru, wLinkId);
										pRouteStru->wEndLocDir = EMAP_SAME_DIR;
									}
									else
									{
										/*进路终点不是方向变化点*/
										pRouteStru->wEndLocOfst = 0u;
										pRouteStru->wEndLocDir = EMAP_CONVER_DIR;
									}
								}
								else
								{
									/*函数调用出错*/
									rtnValue = 0u;
									break;
								}
							}
							else
							{
								/*函数调用出错*/
								rtnValue = 0u;
								break;
							}
						}
						else
						{
							/*函数调用出错*/
							rtnValue = 0u;
							break;
						}
					}
					else
					{
						/*函数调用出错*/
						rtnValue = 0u;
						break;
					}
				}

				/*循环分步切割处理*/
				funcRtr = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == funcRtr)
				{
					rtnValue &= 2u;
					break;
				}
			}
			if (i == dsuDataLenStru->wRouteLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue &= 2u;
			}
		}
		/**********
		link序列及长度计算完毕
		*********/


		/*********
		2、计算逻辑区段序列及长度 wyd 20220310
		**********/
		funcRtr = EmapCalcFunStepCtrl(0x04u);
		if (DQU_QFUN_TRUE == funcRtr)
		{
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wRouteLen; i++)
			{
				ResultLogicSgmtNum = 0u;
				(void)CommonMemSet(ResultLogicSgmtId, 50u * sizeof(UINT16), 0x00u, 50u * sizeof(UINT16));
				(void)CommonMemSet(ResultLogicSgmtDir, 50u * sizeof(UINT8), 0x00u, 50u * sizeof(UINT8));
				(void)CommonMemSet(tempPointStatus, sizeof(DQU_POINT_STATUS_STRU), 0x00u, sizeof(DQU_POINT_STATUS_STRU));
				pRouteStru = dsuStaticHeadStru->pRouteStru + i;
				pRouteStru->wIncludeLogicSgmtNum = 0u;

				for (j = 0U; j < pRouteStru->wIncludePointNum; j++)
				{
					tempPointStatus[j].PointId = pRouteStru->wPointId[j];
					tempPointStatus[j].PointStatus = (UINT8)pRouteStru->wPointStatus[j];
				}

				tempStartLoc.Lnk = pRouteStru->wOrgnLkId;
				tempStartLoc.Off = pRouteStru->wStartLocOfst;
				tempStartLoc.Dir = pRouteStru->wStartLocDir;
				tempEndLoc.Lnk = pRouteStru->wTmnlLkId;
				tempEndLoc.Off = pRouteStru->wEndLocOfst;
				tempEndLoc.Dir = pRouteStru->wEndLocDir;
				funcRtr = dsuLogicSgmtCalcuByLocExp(g_dsuEmapStru, tempStartLoc, tempEndLoc,
					(UINT8)pRouteStru->wIncludePointNum, tempPointStatus, &ResultLogicSgmtNum, ResultLogicSgmtId);
				if (1U == funcRtr)
				{
					/*输出*/
					pRouteStru->wIncludeLogicSgmtNum = ResultLogicSgmtNum;
					CommonMemCpy(&pRouteStru->wLogicSgmtId[0u], ResultLogicSgmtNum * sizeof(UINT16), &ResultLogicSgmtId[0u], ResultLogicSgmtNum * sizeof(UINT16));

					/*遍历所有的逻辑区段*/
					for (LogicSgmtIndex = 0u; LogicSgmtIndex < ResultLogicSgmtNum; LogicSgmtIndex++)
					{
						/*获取逻辑区段信息*/
						pLogicSgmtStru = g_dsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + g_dsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[ResultLogicSgmtId[LogicSgmtIndex]];
						
						/*检查进路起点到本逻辑区段起点的位置的方向*/
						pRouteStru->wLogicSgmtDir[LogicSgmtIndex] = dsuGetPosBDirByPosAExp(g_dsuEmapStru, pRouteStru->wOrgnLkId, pRouteStru->wStartLocOfst, pRouteStru->wStartLocDir, 
							pLogicSgmtStru->wOrgnLkId, pLogicSgmtStru->dwOrgnLkOfst, (UINT8)pRouteStru->wIncludePointNum, tempPointStatus);
					}
				}
				else
				{
					rtnValue = 0U;
					break;
				}
				/*循环分步切割处理*/
				funcRtr = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == funcRtr)
				{
					rtnValue &= 2u;
					break;
				}
			}
			if (i == dsuDataLenStru->wRouteLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue &= 2u;
			}
		}
		/**********
		link序列及长度计算完毕
		*********/

		/******
		3、计算用于点式MA的保护区段信息 wyd 20220215
		********/
		funcRtr = EmapCalcFunStepCtrl(0x05u);
		if (DQU_QFUN_TRUE == funcRtr)
		{
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wRouteLen; i++)
			{
				pRouteStru = dsuStaticHeadStru->pRouteStru + i;

				/*需要检查进路终点是否为方向变化点，一会儿用*/
				pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pRouteStru->wTmnlLkId];
				if (((EMAP_CONVER_DIR == pRouteStru->wEndLocDir) && (EMAP_DIRCHANGE_ORGN2ORGN == pLinkStru->wLogicDirChanged))
					|| ((EMAP_SAME_DIR == pRouteStru->wEndLocDir) && (EMAP_DIRCHANGE_TMNL2TMNL == pLinkStru->wLogicDirChanged)))
				{
					Dirction = (EMAP_SAME_DIR == pRouteStru->wEndLocDir) ? EMAP_CONVER_DIR : EMAP_SAME_DIR;/*有变化点，改变查找方向*/
				}
				else
				{
					Dirction = pRouteStru->wEndLocDir;
				}


				/*查找最短保护区段，先判断进路对应的保护区段个数*/
				if (0u == pRouteStru->wProtectLinkNum)
				{
					/*当前进路没有保护区段，最短长度置0*/
					ShortestLength = 0u;
				}
				else if (4u < pRouteStru->wProtectLinkNum)
				{
					/*保护区段数量不合法，退出*/
					rtnValue = 0u;
					break;
				}
				else
				{
					/*最短保护区段长度先赋最大值，不超过满足精确条件的MA延伸长度。wyd 20220627*/
					ShortestLength = g_MaExtendLength;
					FlagFindAllPointSide = DSU_FALSE;
					tempPointNum = 0u;
					/*遍历所有保护区段*/
					for (j = 0u; j < pRouteStru->wProtectLinkNum; j++)
					{
						pProtectLinkStru = dsuStaticHeadStru->pProtectLinkStru + dsuProtectLinkIndex[pRouteStru->wProtectLinkId[j]];

						/*保护区段内先认为没有定位道岔*/
						FlagPointMain = DSU_FALSE;
						tempLength = 0u;
						/*遍历保护区段内所有计轴区段，累加当前保护区段长度，并判断保护区段内是否包含同向定位道岔*/
						if ((0u == pProtectLinkStru->wIncludeAxleSgmtNum) || (4u < pProtectLinkStru->wIncludeAxleSgmtNum))
						{
							/*计轴区段数量不合法，退出*/
							rtnValue = 0u;
							break;
						}
						else
						{
							for (k = 0u; k < pProtectLinkStru->wIncludeAxleSgmtNum; k++)
							{
								pAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + dsuAxleSgmtLinkIndex[pProtectLinkStru->wIncludeAxleSgmtId[k]];
								/*累加长度*/
								tempLength += pAxleSgmtStru->dwLength;
								/*判断计轴区段内是否包含定位道岔并进行标记*/
								for (m = 0u; m < pAxleSgmtStru->wRelatPointNum; m++)
								{
									if (DSU_TRUE == FlagPointMain)
									{
										break;
									}
									else if (EMAP_POINT_STATUS_MAIN == pAxleSgmtStru->wRelatPointStatus[m])
									{
										/*道岔为定位，判断该道岔对于进路是分岔方向还是汇合方向*/
										funcRtr = GetPointLocFromIdExp(g_dsuEmapStru, pAxleSgmtStru->wRelatPointId[m], &tempPointLoc);
										if (1u == funcRtr)
										{
											pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[tempPointLoc.ConfluentLkId];
											/*道岔汇合link的右侧是定位link，且进路方向向右；或道岔汇合link的左侧是定位link，且进路方向向左*/
											if ((pLinkStru->wTmnlJointMainLkId == tempPointLoc.MainLkId) && (EMAP_SAME_DIR == Dirction)
												|| ((pLinkStru->wOrgnJointMainLkId == tempPointLoc.MainLkId) && (EMAP_CONVER_DIR == Dirction)))
											{
												/*道岔开向与进路方向相同*/
												FlagPointMain = DSU_TRUE;
												break;
											}
											else
											{
												/*道岔开向与进路方向不同，DO NOTHING*/
											}
										}
										else
										{
											rtnValue = 0u;
											break;
										}
									}
									else
									{
										/*DO NOTHING*/
									}
								}
							}

							if (tempLength < ShortestLength)
							{
								/*当前长度更短，更新最短保护区段长度*/
								ShortestLength = tempLength;
							}
							else
							{
								/*DO NOTHING*/
							}

							/*同开向道岔全在反位的保护区段理论上仅有一条，只记录符合条件的第一条保护区段*/
							if ((DSU_FALSE == FlagPointMain) && (DSU_FALSE == FlagFindAllPointSide))
							{
								FlagFindAllPointSide = DSU_TRUE;
								/*该保护区段内不含同开向的定位道岔，同开向道岔全在反位，MA终点基于该条保护区段进行计算*/
								/*将该条保护区段内的道岔信息记录下来，后面计算MA终点用*/
								for (k = 0u; k < pProtectLinkStru->wIncludeAxleSgmtNum; k++)
								{
									pAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + dsuAxleSgmtLinkIndex[pProtectLinkStru->wIncludeAxleSgmtId[k]];
									for (m = 0u; m < pAxleSgmtStru->wRelatPointNum; m++)
									{
										if (MAX_PROTECTLINK_POINT_NUM <= tempPointNum)
										{
											/*数组越界（理论上不会发生，除非数据配置错误）防护*/
											rtnValue = 0u;
											break;
										}
										else
										{
											/*一条保护区段内不会有重复ID的道岔，无需对数组查重*/
											tempPointInfo[tempPointNum].PointId = pAxleSgmtStru->wRelatPointId[m];
											tempPointInfo[tempPointNum].PointStatus = (UINT8)pAxleSgmtStru->wRelatPointStatus[m];
											tempPointNum += 1u;
										}
									}

									if (0u == rtnValue)
									{
										/*查询失败，跳出*/
										break;
									}
									else
									{
										/*do nothing*/
									}
								}
							}
							else
							{
								/*DO NOTHING*/
							}
						}

						if (0u == rtnValue)
						{
							/*查询失败，跳出*/
							break;
						}
						else
						{
							/*do nothing*/
						}
					}

					if (DSU_FALSE == FlagFindAllPointSide)
					{
						/*没找到同向道岔都在反位的进路，报错*/
						rtnValue = 0u;
						break;
					}
					else
					{
						/*DO NOTHING*/
					}
				}

				if (0u == rtnValue)
				{
					/*报错退出了，跳出循环*/
					break;
				}
				else
				{
					/*计算进路终点*/
					/*数组越界防护*/
					if ((0u == pRouteStru->wIncludeLinkNum) || (50u < pRouteStru->wIncludeLinkNum))
					{
						/*函数调用出错*/
						rtnValue = 0u;
						break;
					}
					else
					{
						RouteEndLoc.Lnk = pRouteStru->wLinkId[pRouteStru->wIncludeLinkNum - 1u];
						RouteEndLoc.Dir = pRouteStru->wEndLocDir;
						if (EMAP_SAME_DIR == RouteEndLoc.Dir)
						{
							RouteEndLoc.Off = LinkLengthExp(g_dsuEmapStru, RouteEndLoc.Lnk);
						}
						else if (EMAP_CONVER_DIR == RouteEndLoc.Dir)
						{
							RouteEndLoc.Off = 0u;
						}
						else
						{
							/*函数调用出错*/
							rtnValue = 0u;
							break;
						}
					}

					/*计算MA终点*/
					if (0u == pRouteStru->wProtectLinkNum)
					{
						/*没有保护区段，MA终点就是进路终点*/
						pRouteStru->wMaEndLkId = RouteEndLoc.Lnk;
						pRouteStru->dwMaEndLkOfst = RouteEndLoc.Off;
						pRouteStru->wMaEndSidePointNum = 0u;
					}
					else
					{
						/*有保护区段，根据进路终点、最短保护区段长度和所有道岔反位，计算MA终点*/
						funcRtr = dsuLODCalcuExp(g_dsuEmapStru, &RouteEndLoc, (INT32)ShortestLength, tempPointNum, &tempPointInfo[0u], &MaEndLoc);
						if (1u == funcRtr)
						{
							/*计算成功*/
							pRouteStru->wMaEndLkId = MaEndLoc.Lnk;
							pRouteStru->dwMaEndLkOfst = MaEndLoc.Off;
							pRouteStru->wMaEndSidePointNum = tempPointNum;
							if (MAX_PROTECTLINK_POINT_NUM <= tempPointNum)
							{
								/*数组越界（理论上不会发生）防护*/
								rtnValue = 0u;
								break;
							}
							else
							{
								for (j = 0u; j < tempPointNum; j++)
								{
									pRouteStru->wMaEndSidePointId[j] = tempPointInfo[j].PointId;
									pRouteStru->wMaEndSidePointStatus[j] = tempPointInfo[j].PointStatus;
								}
							}
						}
						else
						{
							/*计算失败，退出*/
							rtnValue = 0u;
							break;
						}
					}
				}

				/*循环切分检测处理*/
				funcRtr = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == funcRtr)
				{
					rtnValue &= 2u;
					break;
				}
			}
			if (i == dsuDataLenStru->wRouteLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue &= 2u;
			}
		}
		/******
		用于点式MA的保护区段信息计算完毕
		********/

		/*******增加逻辑区段所属的进路*********/
		funcRtr = LogicSgmtAddRouteId_CC();
		if (0u == funcRtr)
		{
			rtnValue = 0u;
		}
		else
		{
			/*do nothing*/
		}
		/*******增加逻辑区段所属的进路完毕*********/
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}

	return rtnValue;
}

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

备注：因互联互通增加逻辑方向变化点，计算进路包含link算法需改进（分别按照进路起点link的正向和反向搜索）。
change by qxt,20171012
移除互联互通、车车以外的接口 by lyk 20220307
*********************************************/
UINT8 RouteStru(void)
{
	UINT8 rtnValue = 0u;
	UINT8 cbtcVersionType = 0u;
	cbtcVersionType = GetCbtcSysType();

	if (DQU_CBTC_CC == cbtcVersionType)
	{
		rtnValue = RouteStru_CC();
	}
	else
	{
		rtnValue = 0u;

	}
	return rtnValue;
}

/*********************************************
*函数功能：完成保护区段添加项
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 ProtectLink(void)
{
	UINT16 i = 0u, j = 0u, k = 0u;
	DSU_PROTECT_LINK_STRU * pPretectLinkStru = NULL;
	DSU_AXLE_SGMT_STRU * pAxleSgmtStru = NULL;
	DSU_ROUTE_STRU * pRouteStru = NULL;
	UINT8 rtnValue = 2u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	UINT16	*dsuAxleSgmtIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuAxleSgmtIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuAxleSgmtIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	/***********
	计算进路编号
	********/
	FuncRtn1 = EmapCalcFunStepCtrl(0x02);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wProtectLinkLen; i++)
		{
			pPretectLinkStru = dsuStaticHeadStru->pProtectLinkStru + i;
			/*先设为无效值*/
			pPretectLinkStru->wRouteId = 0xFFFFu;
			for (j = 0u; j < dsuDataLenStru->wRouteLen; j++)
			{
				pRouteStru = dsuStaticHeadStru->pRouteStru + j;
				for (k = 0u; k < pRouteStru->wProtectLinkNum; k++)
				{
					if (pRouteStru->wProtectLinkId[k] == pPretectLinkStru->wId)
					{
						pPretectLinkStru->wRouteId = pRouteStru->wId;
					}
					else
					{
						/*什么都不做*/
					}
				}
			}
			if (0xFFFFu == pPretectLinkStru->wRouteId)
			{
				/*该值还是无效，说明没找到对应的进路,返回失败*/
				rtnValue = 0u;
				return rtnValue;
			}
			/*循环分步切割处理*/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}

		}
		if (i == dsuDataLenStru->wProtectLinkLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 2u;
		}
	}
	/***********
	进路编号计算完毕
	********/

	/***********
	计算障碍物（只考虑道岔）
	********/
	FuncRtn1 = EmapCalcFunStepCtrl(0x03u);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wProtectLinkLen; i++)
		{
			pPretectLinkStru = dsuStaticHeadStru->pProtectLinkStru + i;
			pPretectLinkStru->wObsNum = 0u;
			for (j = 0u; j < pPretectLinkStru->wIncludeAxleSgmtNum; j++)
			{
				/*取包含计轴区段中关联道岔*/
				pAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + dsuAxleSgmtIndex[pPretectLinkStru->wIncludeAxleSgmtId[j]];
				pPretectLinkStru->wObsNum += pAxleSgmtStru->wRelatPointNum;
			}
			/*道岔个数为0*/
			if (0u == pPretectLinkStru->wObsNum)
			{
				/*道岔类型无*/
				pPretectLinkStru->wObsType = 0xFFFFu;
			}
			/*道岔个数小于4*/
			else if (4u >= pPretectLinkStru->wObsNum)
			{
				/*障碍物类型为道岔*/
				pPretectLinkStru->wObsType = OBSPOINT;
			}
			else
			{
				/*道岔个数为4以上，数据错误*/
				rtnValue = 0u;
				return rtnValue;
			}

			/*循环分步切割处理*/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}

		}
		if (i == dsuDataLenStru->wProtectLinkLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}

	/*****
	障碍物信息计算完毕
	****/
	return rtnValue;
}

/*********************************************
*函数功能：计算坡度
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 Grade(void)
{
	UINT16 i = 0u, j = 0u;
	DSU_GRADE_STRU * pGradeStru = NULL;   /*计算当前坡度信息结构体指针*/
	DSU_LINK_STRU* pLinkStur = NULL;      /*link信息结构体指针临时变量*/
	UINT8 rtnValue = 2u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	UINT16	*dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	/*计算坡度中间link序列*/
	FuncRtn1 = EmapCalcFunStepCtrl(0x02);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wGradeLen; i++)
		{
			pGradeStru = dsuStaticHeadStru->pGradeStru + i;
			/*将起始link设为当前link*/
			pLinkStur = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pGradeStru->wOrgnLinkId];
			/*始端link 与末端link重合  或者相邻*/
			for (j = 0; j < GRADE_INCLUDE_LINK_NUM; j++)
			{
				if ((pLinkStur->wTmnlJointMainLkId == pGradeStru->wTmnlLinkId)
					|| (pLinkStur->wId == pGradeStru->wTmnlLinkId))
				{
					break;
				}
				else
				{
					/*移动到相邻link*/
					pLinkStur = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pLinkStur->wTmnlJointMainLkId];
					pGradeStru->wIncludeLinkId[j] = pLinkStur->wId;
				}
				/*当前相邻link是为终端link,退出*/
				if (pLinkStur->wTmnlJointMainLkId == pGradeStru->wTmnlLinkId)
				{
					break;
				}
				else
				{
					/*继续循环*/
				}
			}
			/*包含link大于10个*/
			if (j >= GRADE_INCLUDE_LINK_NUM)
			{
				rtnValue = 0u;
				return rtnValue;
			}

			/*循环分步切割处理*/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}

		}
		if (i == dsuDataLenStru->wGradeLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0;
			rtnValue = 2u;
		}

	}

	/*计算坡度长度*/
	FuncRtn1 = EmapCalcFunStepCtrl(0x03);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wGradeLen; i++)
		{
			pGradeStru = dsuStaticHeadStru->pGradeStru + i;
			if (pGradeStru->dwOrgnLinkOfst == 0)
			{
				/*坡度长度初始化为0*/
				pGradeStru->dwGradeLength = 0;
			}
			else
			{
				/*坡度长度初始化为1*/
				pGradeStru->dwGradeLength = 1;
			}

			if (pGradeStru->wOrgnLinkId == pGradeStru->wTmnlLinkId)
			{
				/*坡度长度 += 起点所在link长度 - 偏移量*/
				pGradeStru->dwGradeLength += (pGradeStru->dwTmnlLinkOfst - pGradeStru->dwOrgnLinkOfst);
			}
			else
			{
				/*将起始link设为当前link*/
				pLinkStur = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pGradeStru->wOrgnLinkId];
				/*初始化坡度长度 = 起点所在link长度 - 偏移量*/
				pGradeStru->dwGradeLength = pLinkStur->dwLength - pGradeStru->dwOrgnLinkOfst;
				for (j = 0; j < GRADE_INCLUDE_LINK_NUM; j++)
				{

					/*如果相邻link不是终点link*/
					if (pLinkStur->wTmnlJointMainLkId != pGradeStru->wTmnlLinkId)
					{
						/*找到相邻link*/
						pGradeStru->wIncludeLinkId[j] = pLinkStur->wTmnlJointMainLkId;
						/*移动到相邻的下一个link上*/
						pLinkStur = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pGradeStru->wIncludeLinkId[j]];
						/*坡度长度增加该link长度*/
						pGradeStru->dwGradeLength += pLinkStur->dwLength;
					}
					else
					{
						/*坡度长度增加末端点偏移量*/
						pGradeStru->dwGradeLength += pGradeStru->dwTmnlLinkOfst;
						break;
					}
				}

				if (j >= GRADE_INCLUDE_LINK_NUM)
				{
					/*中间link超过10个,数据有误*/
					rtnValue = 0u;
					return rtnValue;
				}
			}

			/*循环分步切割处理*/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}
		}
		if (i == dsuDataLenStru->wGradeLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0;
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*********************************************
*函数功能：计算默认行车序列表各项
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 DefaultRouteStru(void)
{
	UINT16 i = 0u, j = 0u;
	DSU_STOPPOINT_STRU*  pStopPointStru = NULL;
	DSU_DEFAULTROUTE_STRU*  pDefaultRouteStru = NULL;
	UINT8 rtnValue = 2u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}
	FuncRtn1 = EmapCalcFunStepCtrl(0x02);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wDefaultRoutLen; i++)
		{
			pDefaultRouteStru = dsuStaticHeadStru->pDefaultRoutStru + i;
			/*站台标志设为0*/
			pDefaultRouteStru->wCurrentPlatformFlag = 0u;
			for (j = 0u; j < dsuDataLenStru->wStopPointLen; j++)
			{
				pStopPointStru = dsuStaticHeadStru->pStopPointStru + j;
				/*如果当前linkId,方向与停车点表linkId,方向一致，且停车点属性为站台运营停车点*/
				if ((pStopPointStru->wLinkId == pDefaultRouteStru->wLinkId)
					&& (pStopPointStru->wDir == pDefaultRouteStru->wLinkDir)
					&& ((EMAP_OPERAT_STOP_POINT & pStopPointStru->wAttribute) == EMAP_OPERAT_STOP_POINT))
				{
					/*站台ID为停车点站台ID，站台标志设为1*/
					pDefaultRouteStru->wCurrentPlatformId = pStopPointStru->wStationID;
					pDefaultRouteStru->wCurrentPlatformFlag = 1u;
				}
				else
				{
					/*什么都不做*/
				}
			}

			/*循环分步切割处理*/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}
		}
		if (i == dsuDataLenStru->wDefaultRoutLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*********************************************
*函数功能：根据进路link序列，计算障碍物信息
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
UINT8 GetObstacleFromRoute(DSU_ROUTE_STRU*  pRouteStru)
{
	UINT16 i = 0u, j = 0u;
	DSU_SCREEN_STRU*  pScreenStru = NULL;			/*障碍物屏蔽门临时变量*/
	DSU_EMERG_STOP_STRU*  pEmergStopStru = NULL;	/*障碍物紧急停车按钮临时变量*/
	DSU_POINT_STRU*  pPointStru = NULL;				/*障碍物道岔临时变量*/
	UINT8 rtnValue = 0u;

	/*电子地图数据*/
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	/*进路障碍物个数初始化为0*/
	pRouteStru->wObsNum = 0u;

	/*计算障碍物信号机的各信息*/
	pRouteStru->wObsID[pRouteStru->wObsNum] = pRouteStru->wStartSignalId;
	pRouteStru->wObsType[pRouteStru->wObsNum] = OBSSIGNAL;
	pRouteStru->wObsNeedLockStatus[pRouteStru->wObsNum] = DSU_STATUS_SIGNAL_PASS;
	pRouteStru->wObsNum++;

	for (i = 0u; i < pRouteStru->wIncludeLinkNum; i++)
	{
		/*计算障碍物屏蔽门的各信息*/
		for (j = 0u; j < dsuDataLenStru->wScreenLen; j++)
		{
			pScreenStru = dsuStaticHeadStru->pScreenStru + j;
			if (pScreenStru->wRelatLkId == pRouteStru->wLinkId[i])
			{
				/*障碍物ID，类型，个数，状态*/
				pRouteStru->wObsID[pRouteStru->wObsNum] = pScreenStru->wId;
				pRouteStru->wObsType[pRouteStru->wObsNum] = OBSSCREEN;
				pRouteStru->wObsNeedLockStatus[pRouteStru->wObsNum] = DSU_STATUS_PSD_CLOSE;
				pRouteStru->wObsNum++;
			}
			else
			{
				/*什么都不做*/
			}
		}

		/*计算障碍物紧急停车按钮的各信息*/
		for (j = 0u; j < dsuDataLenStru->wEmergStopLen; j++)
		{
			pEmergStopStru = dsuStaticHeadStru->pEmergStopStru + j;
			if (pEmergStopStru->wRelatLkId == pRouteStru->wLinkId[i])
			{
				/*障碍物ID，类型，个数，状态*/
				pRouteStru->wObsID[pRouteStru->wObsNum] = pEmergStopStru->wId;
				pRouteStru->wObsType[pRouteStru->wObsNum] = OBSEMERAGESTOP;
				pRouteStru->wObsNeedLockStatus[pRouteStru->wObsNum] = DSU_STATUS_ESP_UNACTIVITE;
				pRouteStru->wObsNum++;
			}
			else
			{
				/*什么都不做*/
			}
		}

		/*计算障碍物道岔的各信息*/
		for (j = 0u; j < dsuDataLenStru->wPointLen; j++)
		{
			pPointStru = dsuStaticHeadStru->pPointStru + j;
			if (pPointStru->wMainLkId == pRouteStru->wLinkId[i])
			{
				/*障碍物ID，类型，个数，状态*/
				pRouteStru->wObsID[pRouteStru->wObsNum] = pPointStru->wId;
				pRouteStru->wObsType[pRouteStru->wObsNum] = OBSPOINT;
				pRouteStru->wObsNeedLockStatus[pRouteStru->wObsNum] = EMAP_POINT_STATUS_MAIN;
				pRouteStru->wObsNum++;
			}
			else if (pPointStru->wSideLkId == pRouteStru->wLinkId[i])
			{
				/*障碍物ID，类型，个数，状态*/
				pRouteStru->wObsID[pRouteStru->wObsNum] = pPointStru->wId;
				pRouteStru->wObsType[pRouteStru->wObsNum] = OBSPOINT;
				pRouteStru->wObsNeedLockStatus[pRouteStru->wObsNum] = EMAP_POINT_STATUS_SIDE;
				pRouteStru->wObsNum++;
			}
			else
			{
				/*什么都不做*/
			}
		}
	}
	rtnValue = 1u;
	return rtnValue;
}
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
UINT8 GetPointInfoFromAccessLink(UINT16 AccessLinkId, UINT16 Dirction, UINT16*  RelatePointNum, UINT16 RelatePointId[], UINT16 RelatePointSta[])
{
	DSU_CBTC_ACCESS_LINK_STRU*  pCBTCAccessLinkStru = NULL;		/*临时变量*/
	UINT16 i = 0u, j = 0u;										/*循环变量*/
	DSU_LOGIC_SGMT_STRU*  pLogicSgmtStru = NULL;				/*临时变量*/
	DSU_AXLE_SGMT_STRU*  pAxleSgmtStru = NULL;					/*临时变量--计轴区段*/
	UINT8 rtnValue = 0u;

	/*电子地图数据*/
	UINT16	*dsuAxleSgmtIndex = NULL;
	UINT16	*dsuCBTCAccessLinkIndex = NULL;
	UINT16	*dsuLogicSgmtIndex = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != RelatePointId) && (NULL != RelatePointSta))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuAxleSgmtIndex = dsuEmapIndexStru->dsuAxleSgmtIndex;
		dsuCBTCAccessLinkIndex = dsuEmapIndexStru->dsuCBTCAccessLinkIndex;
		dsuLogicSgmtIndex = dsuEmapIndexStru->dsuLogicSgmtIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针或传入参数指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	*RelatePointNum = 0u;

	if ((AccessLinkId > dsuEmapIndexStru->CBTCACCESSLINKINDEXNUM) || (0xffffu == dsuCBTCAccessLinkIndex[AccessLinkId]))
	{
		rtnValue = 0u;
		return rtnValue;
	}
	pCBTCAccessLinkStru = dsuStaticHeadStru->pCBTCAccessLinkStru + dsuCBTCAccessLinkIndex[AccessLinkId];

	/*（逻辑区段是按顺序排列的）*/
	for (i = 0u; i < pCBTCAccessLinkStru->wIncludeLogicSgmtNum; i++)
	{
		/*设当前逻辑区段为数组中第i个*/
		pLogicSgmtStru = dsuStaticHeadStru->pLogicSgmtStru + dsuLogicSgmtIndex[pCBTCAccessLinkStru->wIncludeLogicSgmtId[i]];
		/*起始link同末端link不一致，一定有道岔*/
		if ((pLogicSgmtStru->wOrgnLkId) != (pLogicSgmtStru->wTmnlLkId))
		{
			/*道岔信息在计轴区段中*/
			/*对应计轴区段信息不存在，数据错误，退出*/
			if ((0xFFFFu == pLogicSgmtStru->wAxleSgmtId)
				|| (0xFFFFu == dsuAxleSgmtIndex[pLogicSgmtStru->wAxleSgmtId]))
			{
				rtnValue = 0u;
				return rtnValue;
			}
			/*初始化所在计轴区段*/
			pAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + dsuAxleSgmtIndex[pLogicSgmtStru->wAxleSgmtId];
			for (j = 0u; j < pAxleSgmtStru->wRelatPointNum; j++)
			{
				if (*RelatePointNum >= ROUTE_ACCESS_LINK_POINT_NUM)
				{
					/*接近区段包含道岔大于ROUTE_ACCESS_LINK_POINT_NUM，数据错误，退出*/
					rtnValue = 0u;
					return rtnValue;
				}
				RelatePointId[*RelatePointNum] = pAxleSgmtStru->wRelatPointId[j];
				RelatePointSta[*RelatePointNum] = pAxleSgmtStru->wRelatPointStatus[j];
				*RelatePointNum += 1u;
			}
		}
		else
		{
			/*起始link与终点link相同，一定没有道岔，无操作，继续执行*/
		}
	}

	rtnValue = 1u;
	return rtnValue;
}
#endif
/*********************************************
函数功能：根据link序列获得计轴区段编号序列
输入参数：linkId 序列首地址，LinkNum link个数
输出参数：AxleSgmtId 计轴区段编号数组地址
AxleSgmtNum 数组长度
返回值：0失败，1成功
**********************************************/
UINT8 GetAxleSemtIdFromLinkId(UINT16* linkId, UINT16 LinkNum, UINT16 *AxleSgmtId, UINT16 * AxleSgmtNum)
{
	UINT16 i = 0u, j = 0u, k = 0u;
	DSU_AXLE_SGMT_STRU * pAxleSgmt = NULL;
	DSU_LINK_STRU * pLink = NULL;
	UINT16 AxleCount = 0u;
	UINT8 rtnValue = 0u;

	/*电子地图数据*/
	UINT16 *dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != AxleSgmtId) && (NULL != AxleSgmtNum))
	{
		dsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}

	/*计轴个数先初始化为0*/
	for (i = 0u; i < LinkNum; i++)
	{
		pLink = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[linkId[i]];

		for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
		{
			pAxleSgmt = dsuStaticHeadStru->pAxleSgmtStru + j;
			/*若当前link起点类型为计轴 并且 计轴编号在计轴区段表中能够找到*/
			if ((pLink->wOrgnPointType == EMAP_AXLE_DOT)
				&& (pAxleSgmt->wOrgnAxleId == pLink->wOrgnPointId) && (pAxleSgmt->wOrgnLkId == pLink->wId))
			{
				/*当前link终端点类型为计轴*/
				if (pLink->wTmnlPointType == EMAP_AXLE_DOT)
				{
					/*记录该计轴区段 计数器+1*/
					AxleSgmtId[AxleCount] = pAxleSgmt->wId;
					AxleCount++;

				}
				/*当前link终端点类型为道岔*/
				else if (pLink->wTmnlPointType == EMAP_POINT_DOT)
				{
					for (k = 0u; k < LinkNum; k++)
					{
						/*如果计轴终端link出现在link序列中，则记录*/
						if (pAxleSgmt->wTmnlLkId == linkId[k])
						{
							/*记录该计轴区段 计数器+1*/
							AxleSgmtId[AxleCount] = pAxleSgmt->wId;
							AxleCount++;
						}
						else
						{
							/*什么都不做*/
						}
					}
				}
				else/*当前link终端点类型为其他类型，即使是线路终点，也出错（线路终点也是道岔）*/
				{
					rtnValue = 0u;
					return rtnValue;
				}
			}
			else
			{
				/*do nothing*/
			}
		}
	}

	/*计轴区段个数为0或者大于8,失败*/
	if ((0u == AxleCount) || (MAX_AXLESGMT_NUM_IN_ROUTE < AxleCount))
	{
		rtnValue = 0u;
		return rtnValue;
	}
	else
	{
		/*记录个数*/
		*AxleSgmtNum = AxleCount;
		rtnValue = 1u;
		return rtnValue;
	}
}

/*********************************************
*函数功能：计算停车点信息
*参数说明：无
*返回值：0，失败；1，成功
*********************************************/
static UINT8 CalculateStopPoint_HLT(void)
{
	UINT16 i = 0u, j = 0u;
	DSU_STOPPOINT_STRU * pCurrentStopPointStru = NULL;
	DSU_LINK_STRU * pTempLinkStru = NULL, *pLinkStru = NULL;
	DSU_PROTECT_LINK_STRU * pTempProtectLinkStru = NULL;
	DSU_AXLE_SGMT_STRU * pTempAxleSgmtStru = NULL;
	UINT8 rtnValue = 2u;
	UINT8 FuncRtn1 = 0u;

	/*电子地图数据*/
	UINT16	*dsuLinkIndex = NULL;
	UINT16	*dsuStationIndex = NULL;
	UINT16	*dsuAxleSgmtIndex = NULL;
	UINT16	*dsuProtectLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != g_dsuEmapStru)
	{
		dsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStationIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuStationIndex;
		dsuAxleSgmtIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuAxleSgmtIndex;
		dsuProtectLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuProtectLinkIndex;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
		return rtnValue;
	}
	FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
	if (DQU_QFUN_TRUE == FuncRtn1)
	{
		/*停车点需增加wMinStopTime，wMaxStopTime，wStopTime,dwOverLapLength*/
		for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wStopPointLen; i++)
		{
			pCurrentStopPointStru = dsuStaticHeadStru->pStopPointStru + i;
			/*完成增加项计算*/
			pCurrentStopPointStru->wMaxStopTime = 0u;      /*没用到，暂时设为0*/
			pCurrentStopPointStru->wMinStopTime = 0u;      /*没用到，暂时设为0*/

														  /*如果站台号无效，设停车时间为0*/
			if (0xFFFFu == pCurrentStopPointStru->wStationID)
			{
				pCurrentStopPointStru->wStopTime = 0u;
			}
			else
			{
				/*默认停车时间放到了停车区域表（站台表）中，直接取过来*/
				pCurrentStopPointStru->wStopTime = (dsuStaticHeadStru->pStationStru + dsuStationIndex[pCurrentStopPointStru->wStationID])->wParkTime;
			}


			/*根据overLapId（保护区段）计算长度，先设为0*/
			pTempLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pCurrentStopPointStru->wLinkId];
			if (EMAP_CONVER_DIR == pCurrentStopPointStru->wDir)
			{
				pCurrentStopPointStru->dwOverLapLength = pCurrentStopPointStru->dwLinkOfst;
			}
			else
			{
				pCurrentStopPointStru->dwOverLapLength = pTempLinkStru->dwLength - pCurrentStopPointStru->dwLinkOfst;
			}
			/*取得保护区段*/
			if (0xFFFFu == pCurrentStopPointStru->wOverLapId)
			{
				/*包含区段无效，什么都不做*/
			}
			else
			{
				pTempProtectLinkStru = dsuStaticHeadStru->pProtectLinkStru + dsuProtectLinkIndex[pCurrentStopPointStru->wOverLapId];
				/*overLapId长度 = 各计轴区段包含link之和*/
				for (j = 0u; j < pTempProtectLinkStru->wIncludeAxleSgmtNum; j++)
				{
					/*取得保护区段中包含的计轴区段*/
					pTempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + dsuAxleSgmtIndex[pTempProtectLinkStru->wIncludeAxleSgmtId[j]];
					/*计轴区段只有一个link*/
					if (pTempAxleSgmtStru->wOrgnLkId == pTempAxleSgmtStru->wTmnlLkId)
					{
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wOrgnLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
					}
					/*有两个link 150608*/
					else if (0xFFFFu == pTempAxleSgmtStru->wMiddleLinkId[0u])
					{
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wOrgnLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
					}
					/*有三个link 150608*/
					else if (0xFFFFu == pTempAxleSgmtStru->wMiddleLinkId[1u])
					{
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wOrgnLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wMiddleLinkId[0u]];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
					}
					/*有四个link 150608*/
					else if (0xFFFFu == pTempAxleSgmtStru->wMiddleLinkId[2u])
					{
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wOrgnLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wMiddleLinkId[0u]];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wMiddleLinkId[1u]];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
					}
					/*有五个link 161109 add by qxt*/
					else
					{
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wOrgnLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wMiddleLinkId[0u]];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wMiddleLinkId[1u]];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wMiddleLinkId[2u]];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;
						pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pTempAxleSgmtStru->wTmnlLkId];
						pCurrentStopPointStru->dwOverLapLength += pLinkStru->dwLength;

					}
				}
			}
			/*循环分步切割处理*/
			FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				rtnValue = 2u;
				break;
			}
		}
		if (i == dsuDataLenStru->wStopPointLen)
		{
			g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
			rtnValue = 1u;
		}
	}
	return rtnValue;
}

/*访问接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
/*移除互联互通、车车以外的接口 by lyk 20220307*/
UINT8 CalculateStopPoint(void)
{
	UINT8 rtnValue = 0u;
	UINT8 cbtcVersionType = 0;
	cbtcVersionType = GetCbtcSysType();

	if ((DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
	{
		rtnValue = CalculateStopPoint_HLT();
	}
	else
	{
		rtnValue = 0u;

	}
	return rtnValue;
}

/********************************以下为数据初始化接口*********************************/

/**********************************************
函数功能：初始化[任务数组中占位的表]
输入参数：无
返回值:1成功
**********************************************/
UINT8 initNopCalcTask(void)
{
	/*直接返回成功*/
	return 1u;
}

/**********************************************
函数功能：初始化[任务数组中占位的表]
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:1成功
**********************************************/
UINT8 initNop(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	/*直接返回成功*/
	return 1u;
}

/**********************************************
函数功能：初始化[link数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initLink(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_LINK_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_LINK_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == EmapInitFunStepCtrl(0x01u)))
	{
		dsuDataLenStru->wLinkLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_LINK == rowDataLen))
			{
				/*索引数组长度计算*/
				LINK_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->LINKINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}
	
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == EmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuLinkIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->LINKINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuLinkIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuLinkIndex, dsuEmapIndexStru->LINKINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->LINKINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pLinkStru = (DSU_LINK_STRU *)MyNew(sizeof(DSU_LINK_STRU) * dsuDataLenStru->wLinkLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pLinkStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pLinkStru, sizeof(DSU_LINK_STRU) * dsuDataLenStru->wLinkLen, 0xFFu, sizeof(DSU_LINK_STRU) * dsuDataLenStru->wLinkLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_LINK_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wLinkLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_LINK_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->LINKINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wLinkLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pLinkStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			LINK_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->LINKINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}
			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuLinkIndex[pCurrentStru->wId] = j;

			/*初始化包含的逻辑区段数量，具体内容在逻辑区段初始化时填充*/
			pCurrentStru->LogicSgmtNum = 0u;

			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wLinkLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[道岔数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPoint(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_POINT_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_POINT_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wPointLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;

			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_POINT == rowDataLen))
			{
				/*索引数组长度计算*/
				POINT_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->POINTINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if (2u == rtnValue && DQU_QFUN_TRUE == FuncRtn1)
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuPointIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->POINTINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuPointIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuPointIndex, dsuEmapIndexStru->POINTINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->POINTINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pPointStru = (DSU_POINT_STRU *)MyNew(sizeof(DSU_POINT_STRU) * dsuDataLenStru->wPointLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pPointStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pPointStru, sizeof(DSU_POINT_STRU) * dsuDataLenStru->wPointLen, 0xFFu, sizeof(DSU_POINT_STRU) * dsuDataLenStru->wPointLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_POINT_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wPointLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_POINT_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->POINTINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == EmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wPointLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pPointStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			POINT_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->POINTINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuPointIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wPointLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[信号机数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initSignal(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_SIGNAL_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_SIGNAL_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01);
	if (2u == rtnValue && DQU_QFUN_TRUE == FuncRtn1)
	{
		dsuDataLenStru->wSignalLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_SIGNAL == rowDataLen))
			{
				/*索引数组长度计算*/
				SIGNAL_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->SIGNALINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuSignalIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->SIGNALINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuSignalIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuSignalIndex, dsuEmapIndexStru->SIGNALINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->SIGNALINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pSignalStru = (DSU_SIGNAL_STRU *)MyNew(sizeof(DSU_SIGNAL_STRU) * dsuDataLenStru->wSignalLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pSignalStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pSignalStru, sizeof(DSU_SIGNAL_STRU) * dsuDataLenStru->wSignalLen, 0xFFu, sizeof(DSU_SIGNAL_STRU) * dsuDataLenStru->wSignalLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_SIGNAL_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wSignalLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_SIGNAL_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->SIGNALINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wSignalLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pSignalStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			SIGNAL_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->SIGNALINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuSignalIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wSignalLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[应答器数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initBalise(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_BALISE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_BALISE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wBaliseLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_BALISE == rowDataLen))
			{
				/*索引数组长度计算*/
				BALISE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->BALISEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuBaliseIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->BALISEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuBaliseIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuBaliseIndex, dsuEmapIndexStru->BALISEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->BALISEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pBaliseStru = (DSU_BALISE_STRU *)MyNew(sizeof(DSU_BALISE_STRU) * dsuDataLenStru->wBaliseLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pBaliseStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pBaliseStru, sizeof(DSU_BALISE_STRU) * dsuDataLenStru->wBaliseLen, 0xFFu, sizeof(DSU_BALISE_STRU) * dsuDataLenStru->wBaliseLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_BALISE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wBaliseLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_BALISE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->BALISEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wBaliseLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pBaliseStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			BALISE_STRU_init(pData, pCurrentStru);


			if (dsuEmapIndexStru->BALISEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuBaliseIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wBaliseLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[计轴器数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initAxle(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_AXLE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_AXLE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wAxlecounterLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_AXLE == rowDataLen))
			{
				/*索引数组长度计算*/
				AXLE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->AXLEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuAxleIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->AXLEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuAxleIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuAxleIndex, dsuEmapIndexStru->AXLEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->AXLEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pAxleStru = (DSU_AXLE_STRU *)MyNew(sizeof(DSU_AXLE_STRU) * dsuDataLenStru->wAxlecounterLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pAxleStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pAxleStru, sizeof(DSU_AXLE_STRU) * dsuDataLenStru->wAxlecounterLen, 0xFFu, sizeof(DSU_AXLE_STRU) * dsuDataLenStru->wAxlecounterLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_AXLE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wAxlecounterLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				dquGetEmapDataIndexArrayLen(EMAP_AXLE_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuEmapIndexStru->AXLEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wAxlecounterLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pAxleStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			AXLE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->AXLEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuAxleIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wAxlecounterLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[屏蔽门数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initScreen(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_SCREEN_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_SCREEN_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wScreenLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_SCREEN == rowDataLen))
			{
				/*索引数组长度计算*/
				SCREEN_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->SCREENINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuScreenIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->SCREENINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuScreenIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuScreenIndex, dsuEmapIndexStru->SCREENINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->SCREENINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pScreenStru = (DSU_SCREEN_STRU *)MyNew(sizeof(DSU_SCREEN_STRU) * dsuDataLenStru->wScreenLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pScreenStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pScreenStru, sizeof(DSU_SCREEN_STRU) * dsuDataLenStru->wScreenLen, 0xFFu, sizeof(DSU_SCREEN_STRU) * dsuDataLenStru->wScreenLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_SCREEN_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wScreenLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				dquGetEmapDataIndexArrayLen(EMAP_SCREEN_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuEmapIndexStru->SCREENINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wScreenLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pScreenStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			SCREEN_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->SCREENINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuScreenIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wScreenLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[紧急停车按钮数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initEmergStop(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_EMERG_STOP_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_EMERG_STOP_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wEmergStopLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_ESB == rowDataLen))
			{
				/*索引数组长度计算*/
				EMERG_STOP_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->EMERGSTOPINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuEmergStopIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->EMERGSTOPINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuEmergStopIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuEmergStopIndex, dsuEmapIndexStru->EMERGSTOPINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->EMERGSTOPINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pEmergStopStru = (DSU_EMERG_STOP_STRU *)MyNew(sizeof(DSU_EMERG_STOP_STRU) * dsuDataLenStru->wEmergStopLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pEmergStopStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pEmergStopStru, sizeof(DSU_EMERG_STOP_STRU) * dsuDataLenStru->wEmergStopLen, 0xFFu, sizeof(DSU_EMERG_STOP_STRU) * dsuDataLenStru->wEmergStopLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_EMERG_STOP_BUTTON_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wEmergStopLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_EMERG_STOP_BUTTON_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->EMERGSTOPINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wEmergStopLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pEmergStopStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			EMERG_STOP_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->EMERGSTOPINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuEmergStopIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wEmergStopLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[停车区域数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStation(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_STATION_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_STATION_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wStationLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_STATION == rowDataLen))
			{
				/*索引数组长度计算*/
				STATION_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->STATIONINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuStationIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->STATIONINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuStationIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuStationIndex, dsuEmapIndexStru->STATIONINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->STATIONINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pStationStru = (DSU_STATION_STRU *)MyNew(sizeof(DSU_STATION_STRU) * dsuDataLenStru->wStationLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pStationStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pStationStru, sizeof(DSU_STATION_STRU) * dsuDataLenStru->wStationLen, 0xFFu, sizeof(DSU_STATION_STRU) * dsuDataLenStru->wStationLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_STATION_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wStationLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_STATION_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->STATIONINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wStationLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pStationStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			STATION_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->STATIONINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuStationIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wStationLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[停车点数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStopPoint(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_STOPPOINT_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_STOPPOINT_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wStopPointLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_STOPPOINT == rowDataLen))
			{
				/*索引数组长度计算*/
				STOPPOINT_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->STOPPOINTINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuStopPointIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->STOPPOINTINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuStopPointIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuStopPointIndex, dsuEmapIndexStru->STOPPOINTINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->STOPPOINTINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pStopPointStru = (DSU_STOPPOINT_STRU *)MyNew(sizeof(DSU_STOPPOINT_STRU) * dsuDataLenStru->wStopPointLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pStopPointStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pStopPointStru, sizeof(DSU_STOPPOINT_STRU) * dsuDataLenStru->wStopPointLen, 0xFFu, sizeof(DSU_STOPPOINT_STRU) * dsuDataLenStru->wStopPointLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_STOPPOINT_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wStopPointLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_STOPPOINT_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->STOPPOINTINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wStopPointLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pStopPointStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			STOPPOINT_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->STOPPOINTINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuStopPointIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wStopPointLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[计轴区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initAxleSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_AXLE_SGMT_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_AXLE_SGMT_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wAxleSgmtLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_AXLESGMT == rowDataLen))
			{
				/*索引数组长度计算*/
				AXLE_SGMT_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->AXLESGMTINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuAxleSgmtIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->AXLESGMTINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuAxleSgmtIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuAxleSgmtIndex, dsuEmapIndexStru->AXLESGMTINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->AXLESGMTINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pAxleSgmtStru = (DSU_AXLE_SGMT_STRU *)MyNew(sizeof(DSU_AXLE_SGMT_STRU) * dsuDataLenStru->wAxleSgmtLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pAxleSgmtStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pAxleSgmtStru, sizeof(DSU_AXLE_SGMT_STRU) * dsuDataLenStru->wAxleSgmtLen, 0xFFu, sizeof(DSU_AXLE_SGMT_STRU) * dsuDataLenStru->wAxleSgmtLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_AXLE_SGMT_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wAxleSgmtLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				dquGetEmapDataIndexArrayLen(EMAP_AXLE_SGMT_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuEmapIndexStru->AXLESGMTINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wAxleSgmtLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pAxleSgmtStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			AXLE_SGMT_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->AXLESGMTINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuAxleSgmtIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wAxleSgmtLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[逻辑区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initLogicSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_LOGIC_SGMT_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_LOGIC_SGMT_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wLogicSgmtLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_LOGICSGMT == rowDataLen))
			{
				/*索引数组长度计算*/
				LOGIC_SGMT_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->LOGICSGMTINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuLogicSgmtIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->LOGICSGMTINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuLogicSgmtIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuLogicSgmtIndex, dsuEmapIndexStru->LOGICSGMTINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->LOGICSGMTINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pLogicSgmtStru = (DSU_LOGIC_SGMT_STRU *)MyNew(sizeof(DSU_LOGIC_SGMT_STRU) * dsuDataLenStru->wLogicSgmtLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pLogicSgmtStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pLogicSgmtStru, sizeof(DSU_LOGIC_SGMT_STRU) * dsuDataLenStru->wLogicSgmtLen, 0xFFu, sizeof(DSU_LOGIC_SGMT_STRU) * dsuDataLenStru->wLogicSgmtLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_LOGIC_SGMT_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wLogicSgmtLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_LOGIC_SGMT_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->LOGICSGMTINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wLogicSgmtLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pLogicSgmtStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			LOGIC_SGMT_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->LOGICSGMTINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuLogicSgmtIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wLogicSgmtLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[进路数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initRoute(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_ROUTE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_ROUTE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wRouteLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if (0U == dataSize % rowCount)
			{
				/*索引数组长度计算*/
				ROUTE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->ROUTEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuRouteIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->ROUTEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuRouteIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuRouteIndex, dsuEmapIndexStru->ROUTEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->ROUTEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pRouteStru = (DSU_ROUTE_STRU *)MyNew(sizeof(DSU_ROUTE_STRU) * dsuDataLenStru->wRouteLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pRouteStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pRouteStru, sizeof(DSU_ROUTE_STRU) * dsuDataLenStru->wRouteLen, 0xFFu, sizeof(DSU_ROUTE_STRU) * dsuDataLenStru->wRouteLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_ROUTE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wRouteLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_ROUTE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->ROUTEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wRouteLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pRouteStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			ROUTE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->ROUTEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuRouteIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wRouteLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[保护区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initProtectLink(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_PROTECT_LINK_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_PROTECT_LINK_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wProtectLinkLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_PROTSGMT == rowDataLen))
			{
				/*索引数组长度计算*/
				PROTECT_LINK_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->PROTECTLINKINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuProtectLinkIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->PROTECTLINKINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuProtectLinkIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuProtectLinkIndex, dsuEmapIndexStru->PROTECTLINKINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->PROTECTLINKINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pProtectLinkStru = (DSU_PROTECT_LINK_STRU *)MyNew(sizeof(DSU_PROTECT_LINK_STRU) * dsuDataLenStru->wProtectLinkLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pProtectLinkStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pProtectLinkStru, sizeof(DSU_PROTECT_LINK_STRU) * dsuDataLenStru->wProtectLinkLen, 0xFFu, sizeof(DSU_PROTECT_LINK_STRU) * dsuDataLenStru->wProtectLinkLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_PROTECT_LINK_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wProtectLinkLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_PROTECT_LINK_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->PROTECTLINKINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wProtectLinkLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pProtectLinkStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			PROTECT_LINK_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->PROTECTLINKINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuProtectLinkIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wProtectLinkLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[点式接近区段数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initBlocAccessSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_BLOC_ACCESS_LINK_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_BLOC_ACCESS_LINK_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wBLOCAccessLinkLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_BLOCACCESS == rowDataLen))
			{
				/*索引数组长度计算*/
				BLOC_ACCESS_LINK_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->BLOCACCESSLINKINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuBLOCAccessLinkIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->BLOCACCESSLINKINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuBLOCAccessLinkIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuBLOCAccessLinkIndex, dsuEmapIndexStru->BLOCACCESSLINKINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->BLOCACCESSLINKINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pBLOCAccessLinkStru = (DSU_BLOC_ACCESS_LINK_STRU *)MyNew(sizeof(DSU_BLOC_ACCESS_LINK_STRU) * dsuDataLenStru->wBLOCAccessLinkLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pBLOCAccessLinkStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pBLOCAccessLinkStru, sizeof(DSU_PROTECT_LINK_STRU) * dsuDataLenStru->wBLOCAccessLinkLen, 0xFFu, sizeof(DSU_PROTECT_LINK_STRU) * dsuDataLenStru->wBLOCAccessLinkLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_BLOC_ACCESS_LINK_ID, &wStruDataLen);

			if ((1u == FuncRtn1) && (((0u != wStruDataLen) && (dsuDataLenStru->wBLOCAccessLinkLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_BLOC_ACCESS_LINK_ID, &wDataIndexLen);
				if ((1u == FuncRtn1) && (((0u != wDataIndexLen) && (dsuEmapIndexStru->BLOCACCESSLINKINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wBLOCAccessLinkLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pBLOCAccessLinkStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			BLOC_ACCESS_LINK_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->BLOCACCESSLINKINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuBLOCAccessLinkIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wBLOCAccessLinkLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*********************************************
*函数功能：将逻辑区段的属性上添加所属CBTC接近区段id
*参数说明：无
*返回值：0，失败；1，成功
*修改记录：创建，kuan.he
备注：内部调用函数，车车使用
*********************************************/
UINT8 LogicSgmtAddCbtcAccessSgmtId_CC(void)
{
	UINT8 RtnNo = 1u;
	UINT16 i = 0u;
	UINT16 j = 0u;
	DSU_CBTC_ACCESS_LINK_STRU* pCurrCbtcAccessSgmtStru = NULL;			/*CBTC接近区段信息结构体指针*/
	DSU_LOGIC_SGMT_STRU* pCurrLogicSgmtStru = NULL;						/*逻辑区段信息结构体指针*/

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru))
	{
		for (i = 0; i < g_dsuEmapStru->dsuDataLenStru->wCBTCAccessLinkLen; i++)
		{
			/*遍历进路表*/
			pCurrCbtcAccessSgmtStru = g_dsuEmapStru->dsuStaticHeadStru->pCBTCAccessLinkStru + i;

			if (NULL != pCurrCbtcAccessSgmtStru)
			{
				/*遍历进路中的逻辑区段*/
				for (j = 0; j < pCurrCbtcAccessSgmtStru->wIncludeLogicSgmtNum; j++)
				{
					/*直接访问对应的逻辑区段*/
					pCurrLogicSgmtStru = g_dsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + g_dsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pCurrCbtcAccessSgmtStru->wIncludeLogicSgmtId[j]];

					if (NULL != pCurrLogicSgmtStru)
					{
						/*将所属进路id添加到对应的逻辑区段中*/
						if (0xFFu == pCurrLogicSgmtStru->AccessSgmtNum)
						{
							/*初始化进路id数量*/
							pCurrLogicSgmtStru->AccessSgmtNum = 0u;
						}
						else
						{
							/*do nothing*/
						}

						if (pCurrLogicSgmtStru->AccessSgmtNum >= (sizeof(pCurrLogicSgmtStru->AccessSgmtIdBuff) / sizeof(UINT16)))
						{
							/*已满*/
							RtnNo &= 0u;
							break;
						}
						else
						{
							pCurrLogicSgmtStru->AccessSgmtIdBuff[pCurrLogicSgmtStru->AccessSgmtNum] = pCurrCbtcAccessSgmtStru->wId;
							pCurrLogicSgmtStru->AccessSgmtNum++;
						}
					}
					else
					{
						/*指针为空*/
						RtnNo &= 0u;
						break;
					}
				}
			}
			else
			{
				/*指针为空*/
				RtnNo &= 0u;
				break;
			}
		}
	}
	else
	{
		/*指针为空*/
		RtnNo &= 0u;
	}


	return RtnNo;
}

/**********************************************
函数功能：CBTC接近区段信息增加进路id
输入参数：无
输出参数：无
返回值:   1成功,0失败
修改记录:	created by  kuan,he 20230515
**********************************************/
UINT8 CbtcAccessSgmtIdAddRouteId_CC(void)
{
	UINT8 RtnNo = 1u;

	DSU_CBTC_ACCESS_LINK_STRU* pCbtcAccessSgmtStru = NULL;		/*CBTC接近区段表*/
	DSU_ROUTE_STRU* pRouteStru = NULL;							/*进路表*/

	UINT16 i = 0u, j = 0u;

	if (NULL != g_dsuEmapStru)
	{
		/*遍历进路表*/
		for ( i = 0; i < g_dsuEmapStru->dsuDataLenStru->wRouteLen; i++)
		{
			/*获取当前的进路信息*/
			pRouteStru = g_dsuEmapStru->dsuStaticHeadStru->pRouteStru + i;
			if (NULL != pRouteStru)
			{
				/*遍历进路的接近区段表*/
				for ( j = 0; j < pRouteStru->wAccessLinkNum; j++)
				{
					/*获取对应的接近区段信息*/
					pCbtcAccessSgmtStru = g_dsuEmapStru->dsuStaticHeadStru->pCBTCAccessLinkStru + g_dsuEmapStru->dsuEmapIndexStru->dsuCBTCAccessLinkIndex[pRouteStru->wAccessLinkIdBuff[j]];
					if (NULL != pCbtcAccessSgmtStru)
					{
						if (0xFFFFu == pCbtcAccessSgmtStru->BelongRouteNum)
						{
							pCbtcAccessSgmtStru->BelongRouteNum = 0u;
						}
						else
						{
							/*do nothing*/
						}

						pCbtcAccessSgmtStru->BelongRouteIdBuff[pCbtcAccessSgmtStru->BelongRouteNum] = pRouteStru->wId;
						pCbtcAccessSgmtStru->BelongRouteNum++;
					}
					else
					{
						/*指针为空*/
						RtnNo &= 0u;
					}
				}
			}
			else
			{
				/*指针为空*/
				RtnNo &= 0u;
			}
		}
		
	}
	else
	{
		/*指针为空*/
		RtnNo &= 0u;
	}

	return RtnNo;
}

/**********************************************
函数功能：初始化[CBTC接近区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
修改记录:
	1. 新增在逻辑区段信息中添加所属CBTC接近区段信息 kuan,he 20230515
**********************************************/
UINT8 initCBTCAccessLink(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_CBTC_ACCESS_LINK_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_CBTC_ACCESS_LINK_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wCBTCAccessLinkLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_CBTCACCESS == rowDataLen))
			{
				/*索引数组长度计算*/
				CBTC_ACCESS_LINK_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->CBTCACCESSLINKINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuCBTCAccessLinkIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->CBTCACCESSLINKINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuCBTCAccessLinkIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuCBTCAccessLinkIndex, dsuEmapIndexStru->CBTCACCESSLINKINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->CBTCACCESSLINKINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pCBTCAccessLinkStru = (DSU_CBTC_ACCESS_LINK_STRU *)MyNew(sizeof(DSU_CBTC_ACCESS_LINK_STRU) * dsuDataLenStru->wCBTCAccessLinkLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pCBTCAccessLinkStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pCBTCAccessLinkStru, sizeof(DSU_CBTC_ACCESS_LINK_STRU) * dsuDataLenStru->wCBTCAccessLinkLen, 0xFFu, sizeof(DSU_CBTC_ACCESS_LINK_STRU) * dsuDataLenStru->wCBTCAccessLinkLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_CBTC_ACCESS_LINK_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wCBTCAccessLinkLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_CBTC_ACCESS_LINK_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->CBTCACCESSLINKINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wCBTCAccessLinkLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pCBTCAccessLinkStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			CBTC_ACCESS_LINK_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->CBTCACCESSLINKINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuCBTCAccessLinkIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wCBTCAccessLinkLen)
		{
			rtnValue = 1u;
		}

		if (1 == rtnValue)
		{
			/*******增加逻辑区段所属的CBTC接近区段信息*********/
			rtnValue = LogicSgmtAddCbtcAccessSgmtId_CC();
			/*******增加逻辑区段所属的CBTC接近区段信息完毕*********/
		}
		else
		{
			/*do nothing*/
		}

		if (1 == rtnValue)
		{
			/*******CBTC接近区段信息增加进路id*********/
			rtnValue = CbtcAccessSgmtIdAddRouteId_CC();
			/*******CBTC接近区段信息增加进路id完毕*********/
		}
		else
		{
			/*do nothing*/
		}
		
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[坡度信息数组]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initGrade(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_GRADE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_GRADE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wGradeLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_GRADE == rowDataLen))
			{
				/*索引数组长度计算*/
				GRADE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->GRADEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuGradeIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->GRADEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuGradeIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuGradeIndex, dsuEmapIndexStru->GRADEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->GRADEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pGradeStru = (DSU_GRADE_STRU *)MyNew(sizeof(DSU_GRADE_STRU) * dsuDataLenStru->wGradeLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pGradeStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pGradeStru, sizeof(DSU_GRADE_STRU) * dsuDataLenStru->wGradeLen, 0xFFu, sizeof(DSU_GRADE_STRU) * dsuDataLenStru->wGradeLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_GRADE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wGradeLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_GRADE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->GRADEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wGradeLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pGradeStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			GRADE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->GRADEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuGradeIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wGradeLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}
#if 0
/**********************************************
函数功能：初始化[静态限速信息]:计算索引(普通处理)
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
static UINT8 initStaticResSpeed_FAO(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_STATIC_RES_SPEED_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_STATIC_RES_SPEED_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wStaticResSpeedLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_STRESPEED == rowDataLen))
			{
				/*索引数组长度计算*/
				STATIC_RES_SPEED_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->STATICRESSPEEDNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuStaticResSpeedIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->STATICRESSPEEDNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuStaticResSpeedIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuStaticResSpeedIndex, dsuEmapIndexStru->STATICRESSPEEDNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->STATICRESSPEEDNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pStaticResSpeedStru = (DSU_STATIC_RES_SPEED_STRU *)MyNew(sizeof(DSU_STATIC_RES_SPEED_STRU) * dsuDataLenStru->wStaticResSpeedLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pStaticResSpeedStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pStaticResSpeedStru, sizeof(DSU_STATIC_RES_SPEED_STRU) * dsuDataLenStru->wStaticResSpeedLen, 0xFFu, sizeof(DSU_STATIC_RES_SPEED_STRU) * dsuDataLenStru->wStaticResSpeedLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_STATIC_RES_SPEED_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wStaticResSpeedLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_STATIC_RES_SPEED_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->STATICRESSPEEDNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wStaticResSpeedLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pStaticResSpeedStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			STATIC_RES_SPEED_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->STATICRESSPEEDNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuStaticResSpeedIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wStaticResSpeedLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}
#endif
/**********************************************
函数功能：初始化[静态限速信息]:计算索引(包含附加处理)
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
static UINT8 initStaticResSpeed_CPK(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 rtnValueOfLimitStc = 2;
	UINT8 *pData = NULL;
	DSU_STATIC_RES_SPEED_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_STATIC_RES_SPEED_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wStaticResSpeedLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_STRESPEED == rowDataLen))
			{
				/*索引数组长度计算*/
				STATIC_RES_SPEED_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->STATICRESSPEEDNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuStaticResSpeedIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->STATICRESSPEEDNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuStaticResSpeedIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuStaticResSpeedIndex, dsuEmapIndexStru->STATICRESSPEEDNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->STATICRESSPEEDNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pStaticResSpeedStru = (DSU_STATIC_RES_SPEED_STRU *)MyNew(sizeof(DSU_STATIC_RES_SPEED_STRU) * dsuDataLenStru->wStaticResSpeedLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pStaticResSpeedStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pStaticResSpeedStru, sizeof(DSU_STATIC_RES_SPEED_STRU) * dsuDataLenStru->wStaticResSpeedLen, 0xFFu, sizeof(DSU_STATIC_RES_SPEED_STRU) * dsuDataLenStru->wStaticResSpeedLen);

					/* add by ywlou */
					/* 初始化静态限速查询索引表，此处用LINKINDEXNUM而没有用dsuDataLenStru->wLinkLen，
					因为dsuLimitStcLinkIdx是以linkId为下标的查找索引表，长度应为最大linkId*/
					*g_ppDsuLimitStcLinkIdx = (DSU_STC_LIMIT_LINKIDX_STRU*)MyNew(sizeof(DSU_STC_LIMIT_LINKIDX_STRU)*(dsuEmapIndexStru->LINKINDEXNUM + 1));
					if (NULL == *g_ppDsuLimitStcLinkIdx)
					{
						rtnValue = 0u;
					}
					else
					{
						CommonMemSet(*g_ppDsuLimitStcLinkIdx,
							sizeof(DSU_STC_LIMIT_LINKIDX_STRU)*(dsuEmapIndexStru->LINKINDEXNUM + 1),
							0xFFu,
							sizeof(DSU_STC_LIMIT_LINKIDX_STRU)*(dsuEmapIndexStru->LINKINDEXNUM + 1));
					}
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_STATIC_RES_SPEED_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wStaticResSpeedLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_STATIC_RES_SPEED_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->STATICRESSPEEDNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wStaticResSpeedLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pStaticResSpeedStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			STATIC_RES_SPEED_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->STATICRESSPEEDNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuStaticResSpeedIndex[pCurrentStru->wId] = j;

			/*完成Link静态限速索引表的建立*/
			rtnValueOfLimitStc = InsertToLimitStcLinkIdx(pCurrentStru->wLinkId, pCurrentStru->wId);
			if (0 == rtnValueOfLimitStc)
			{
				break;
			}
			else
			{
				/*继续执行*/
			}

			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (0 == rtnValueOfLimitStc)
		{
			rtnValue = 0u;
		}
		else if (j == dsuDataLenStru->wStaticResSpeedLen)
		{
			rtnValue = 1u;
		}
		else
		{
			/*nothing to do */
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[静态限速信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStaticResSpeed(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 0u;

	if (NULL != pDataAddr)
	{
		/*如果不给静态限速Link分配指针或开辟内存则，不能执行静态限速初始化函数*/
		if (0x01u == g_IsEmapAllocMemory)
		{
			if (NULL != g_ppDsuLimitStcLinkIdx)
			{
				rtnValue = initStaticResSpeed_CPK(pDataAddr, dataSize, rowCount);
			}
			else
			{
				rtnValue = 0u;
			}
		}
		else/*外部分配内存的*/
		{
			if (NULL != *g_ppDsuLimitStcLinkIdx)
			{
				rtnValue = initStaticResSpeed_CPK(pDataAddr, dataSize, rowCount);
			}
			else
			{
				rtnValue = 0u;
			}
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/**********************************************
函数功能：初始化[站间运营级]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initRunLevel(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_RUN_LEVEL_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_RUN_LEVEL_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;


	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wRunLevelLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_RUNLEVEL == rowDataLen))
			{
				/*索引数组长度计算*/
				RUN_LEVEL_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->RUNLEVELINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuRunLevelIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->RUNLEVELINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuRunLevelIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuRunLevelIndex, dsuEmapIndexStru->RUNLEVELINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->RUNLEVELINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pRunLevelStru = (DSU_RUN_LEVEL_STRU *)MyNew(sizeof(DSU_RUN_LEVEL_STRU) * dsuDataLenStru->wRunLevelLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pRunLevelStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pRunLevelStru, sizeof(DSU_RUN_LEVEL_STRU) * dsuDataLenStru->wRunLevelLen, 0xFFu, sizeof(DSU_RUN_LEVEL_STRU) * dsuDataLenStru->wRunLevelLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_RUN_LEVEL_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wRunLevelLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_RUN_LEVEL_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->RUNLEVELINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wRunLevelLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pRunLevelStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			RUN_LEVEL_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->RUNLEVELINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuRunLevelIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wRunLevelLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[退出CBTC区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initExitRoute(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_EXIT_ROUTE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_EXIT_ROUTE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wExitRouteLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_EXITROUTE == rowDataLen))
			{
				/*索引数组长度计算*/
				EXIT_ROUTE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->EXITROUTEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuExitRouteIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->EXITROUTEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuExitRouteIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuExitRouteIndex, dsuEmapIndexStru->EXITROUTEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->EXITROUTEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pExitRouteStru = (DSU_EXIT_ROUTE_STRU *)MyNew(sizeof(DSU_EXIT_ROUTE_STRU) * dsuDataLenStru->wExitRouteLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pExitRouteStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pExitRouteStru, sizeof(DSU_EXIT_ROUTE_STRU) * dsuDataLenStru->wExitRouteLen, 0xFFu, sizeof(DSU_EXIT_ROUTE_STRU) * dsuDataLenStru->wExitRouteLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_EXIT_ROUTE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wExitRouteLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_EXIT_ROUTE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->EXITROUTEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wExitRouteLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pExitRouteStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			EXIT_ROUTE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->EXITROUTEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuExitRouteIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wExitRouteLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[非CBTC区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initTrainLine(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_TRAIN_LINE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_TRAIN_LINE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wTrainLineLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_TRAINLINE == rowDataLen))
			{
				/*索引数组长度计算*/
				TRAIN_LINE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->TRAINLINEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuTrainLineIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->TRAINLINEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuTrainLineIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuTrainLineIndex, dsuEmapIndexStru->TRAINLINEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->TRAINLINEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pTrainLineStru = (DSU_TRAIN_LINE_STRU *)MyNew(sizeof(DSU_TRAIN_LINE_STRU) * dsuDataLenStru->wTrainLineLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pTrainLineStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pTrainLineStru, sizeof(DSU_TRAIN_LINE_STRU) * dsuDataLenStru->wTrainLineLen, 0xFFu, sizeof(DSU_TRAIN_LINE_STRU) * dsuDataLenStru->wTrainLineLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_TRAIN_LINE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wTrainLineLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_TRAIN_LINE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->TRAINLINEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wTrainLineLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pTrainLineStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			TRAIN_LINE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->TRAINLINEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuTrainLineIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wTrainLineLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[自动折返]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initAR(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_AR_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_AR_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wARLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_AR == rowDataLen))
			{
				/*索引数组长度计算*/
				AR_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->ARNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuARIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->ARNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuARIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuARIndex, dsuEmapIndexStru->ARNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->ARNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pARStru = (DSU_AR_STRU *)MyNew(sizeof(DSU_AR_STRU) * dsuDataLenStru->wARLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pARStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pARStru, sizeof(DSU_AR_STRU) * dsuDataLenStru->wARLen, 0xFFu, sizeof(DSU_AR_STRU) * dsuDataLenStru->wARLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_AR_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wARLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_AR_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->ARNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wARLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pARStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			AR_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->ARNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuARIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wARLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[CI信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initCI(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_CI_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_CI_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;


	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wCILen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_CI == rowDataLen))
			{
				/*索引数组长度计算*/
				CI_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->CIINDEXNUM = stru.wIndex;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuCIIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->CIINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuCIIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuCIIndex, dsuEmapIndexStru->CIINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->CIINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pCIStruStru = (DSU_CI_STRU *)MyNew(sizeof(DSU_CI_STRU) * dsuDataLenStru->wCILen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pCIStruStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pCIStruStru, sizeof(DSU_CI_STRU) * dsuDataLenStru->wCILen, 0xFFu, sizeof(DSU_CI_STRU) * dsuDataLenStru->wCILen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_CI_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wCILen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_CI_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->CIINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wCILen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pCIStruStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			CI_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->CIINDEXNUM < pCurrentStru->wIndex)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuCIIndex[pCurrentStru->wIndex] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wCILen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[ATS信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initATS(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_ATS_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_ATS_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wATSLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_ATS == rowDataLen))
			{
				/*索引数组长度计算*/
				ATS_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->ATSINDEXNUM = stru.wIndex;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuATSIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->ATSINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuATSIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuATSIndex, dsuEmapIndexStru->ATSINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->ATSINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pATSStruStru = (DSU_ATS_STRU *)MyNew(sizeof(DSU_ATS_STRU) * dsuDataLenStru->wATSLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pATSStruStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pATSStruStru, sizeof(DSU_ATS_STRU) * dsuDataLenStru->wATSLen, 0xFFu, sizeof(DSU_ATS_STRU) * dsuDataLenStru->wATSLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_ATS_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wATSLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_ATS_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->ATSINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wATSLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pATSStruStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			ATS_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->ATSINDEXNUM < pCurrentStru->wIndex)
			{
				rtnValue = 0u;
				break;
			}
			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuATSIndex[pCurrentStru->wIndex] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wATSLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[统一限速信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initConResSpeed(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_CONRESSPEED_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_CONRESSPEED_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wConResSpeedLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_CONRESPEED == rowDataLen))
			{
				/*索引数组长度计算*/
				CONRESSPEED_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->CONRESSPEEDINDEXNUM = stru.wIndex;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuConResSpeedIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->CONRESSPEEDINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuConResSpeedIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuConResSpeedIndex, dsuEmapIndexStru->CONRESSPEEDINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->CONRESSPEEDINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pConResSpeedStru = (DSU_CONRESSPEED_STRU *)MyNew(sizeof(DSU_CONRESSPEED_STRU) * dsuDataLenStru->wConResSpeedLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pConResSpeedStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pConResSpeedStru, sizeof(DSU_CONRESSPEED_STRU) * dsuDataLenStru->wConResSpeedLen, 0xFFu, sizeof(DSU_CONRESSPEED_STRU) * dsuDataLenStru->wConResSpeedLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_CONRESSPEED_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wConResSpeedLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_CONRESSPEED_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->CONRESSPEEDINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wConResSpeedLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pConResSpeedStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			CONRESSPEED_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->CONRESSPEEDINDEXNUM < pCurrentStru->wIndex)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuConResSpeedIndex[pCurrentStru->wIndex] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wConResSpeedLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[统一坡度信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initConGrade(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_CONGRADE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_CONGRADE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;


	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wConGradeLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_CONGRADE == rowDataLen))
			{
				/*索引数组长度计算*/
				CONGRADE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->CONGRADEINDEXNUM = stru.wIndex;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuConGradeIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->CONGRADEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuConGradeIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuConGradeIndex, dsuEmapIndexStru->CONGRADEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->CONGRADEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pConGradeStru = (DSU_CONGRADE_STRU *)MyNew(sizeof(DSU_CONGRADE_STRU) * dsuDataLenStru->wConGradeLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pConGradeStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pConGradeStru, sizeof(DSU_CONGRADE_STRU) * dsuDataLenStru->wConGradeLen, 0xFFu, sizeof(DSU_CONGRADE_STRU) * dsuDataLenStru->wConGradeLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_CONGRADE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wConGradeLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_CONGRADE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->CONGRADEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wConGradeLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pConGradeStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			CONGRADE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->CONGRADEINDEXNUM < pCurrentStru->wIndex)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuConGradeIndex[pCurrentStru->wIndex] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wConGradeLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[物理区段信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPhysicalSgmt(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_PHYSICAL_SGMT_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_PHYSICAL_SGMT_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wPhysicalSgmtLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_PHYSGMT == rowDataLen))
			{
				/*索引数组长度计算*/
				PHYSICAL_SGMT_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->PHYSICALSGMTINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuPhysicalSgmtIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->PHYSICALSGMTINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuPhysicalSgmtIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuPhysicalSgmtIndex, dsuEmapIndexStru->PHYSICALSGMTINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->PHYSICALSGMTINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pPhysicalSgmtStru = (DSU_PHYSICAL_SGMT_STRU *)MyNew(sizeof(DSU_PHYSICAL_SGMT_STRU) * dsuDataLenStru->wPhysicalSgmtLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pPhysicalSgmtStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pPhysicalSgmtStru, sizeof(DSU_PHYSICAL_SGMT_STRU) * dsuDataLenStru->wPhysicalSgmtLen, 0xFFu, sizeof(DSU_PHYSICAL_SGMT_STRU) * dsuDataLenStru->wPhysicalSgmtLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_PHYSICAL_SGMT_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wPhysicalSgmtLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_PHYSICAL_SGMT_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->PHYSICALSGMTINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wPhysicalSgmtLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pPhysicalSgmtStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			PHYSICAL_SGMT_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->PHYSICALSGMTINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuPhysicalSgmtIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wPhysicalSgmtLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[防淹门信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initFloodGate(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_FLOOD_GATE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_FLOOD_GATE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;


	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wFloodGateLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_FG == rowDataLen))
			{
				/*索引数组长度计算*/
				DSU_FLOOD_GATE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->FLOODGATEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuFloodGateIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->FLOODGATEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuFloodGateIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuFloodGateIndex, dsuEmapIndexStru->FLOODGATEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->FLOODGATEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pFloodGateStru = (DSU_FLOOD_GATE_STRU *)MyNew(sizeof(DSU_FLOOD_GATE_STRU) * dsuDataLenStru->wFloodGateLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pFloodGateStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pFloodGateStru, sizeof(DSU_FLOOD_GATE_STRU) * dsuDataLenStru->wFloodGateLen, 0xFFu, sizeof(DSU_FLOOD_GATE_STRU) * dsuDataLenStru->wFloodGateLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_FLOOD_GATE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wFloodGateLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_FLOOD_GATE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->FLOODGATEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wFloodGateLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pFloodGateStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			DSU_FLOOD_GATE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->FLOODGATEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuFloodGateIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wFloodGateLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[SPKS开关信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initSpksButton(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_SPKS_BUTTON_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_SPKS_BUTTON_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;


	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wSpksButtonLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_SPKS == rowDataLen))
			{
				/*索引数组长度计算*/
				DSU_SPKS_BUTTON_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->SPKSBUTTONINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuSpksButtonIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->SPKSBUTTONINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuSpksButtonIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuSpksButtonIndex, dsuEmapIndexStru->SPKSBUTTONINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->SPKSBUTTONINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pSpksButtonStru = (DSU_SPKS_BUTTON_STRU *)MyNew(sizeof(DSU_SPKS_BUTTON_STRU) * dsuDataLenStru->wSpksButtonLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pSpksButtonStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pSpksButtonStru, sizeof(DSU_SPKS_BUTTON_STRU) * dsuDataLenStru->wSpksButtonLen, 0xFFu, sizeof(DSU_SPKS_BUTTON_STRU) * dsuDataLenStru->wSpksButtonLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_SPKS_BUTTON_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wSpksButtonLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_SPKS_BUTTON_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->SPKSBUTTONINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wSpksButtonLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pSpksButtonStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			DSU_SPKS_BUTTON_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->SPKSBUTTONINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuSpksButtonIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wSpksButtonLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[车库门信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initDepotGate(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_DEPOT_GATE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_DEPOT_GATE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wDepotGateLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_DEPOTGATE == rowDataLen))
			{
				/*索引数组长度计算*/
				DEPOT_GATE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->DEPOTGATEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuDepotGateIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->DEPOTGATEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuDepotGateIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuDepotGateIndex, dsuEmapIndexStru->DEPOTGATEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->DEPOTGATEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pDepotGateStru = (DSU_DEPOT_GATE_STRU *)MyNew(sizeof(DSU_DEPOT_GATE_STRU) * dsuDataLenStru->wDepotGateLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pDepotGateStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pDepotGateStru, sizeof(DSU_DEPOT_GATE_STRU) * dsuDataLenStru->wDepotGateLen, 0xFFu, sizeof(DSU_DEPOT_GATE_STRU) * dsuDataLenStru->wDepotGateLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_DEPOT_GATE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wDepotGateLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_DEPOT_GATE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->DEPOTGATEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wDepotGateLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pDepotGateStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			DEPOT_GATE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->DEPOTGATEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuDepotGateIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wDepotGateLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[隧道信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initTunnel(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_TUNNEL_STRU	 *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_TUNNEL_STRU	 stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wTunnelLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_TUNNEL == rowDataLen))
			{
				/*索引数组长度计算*/
				TUNNEL_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->TUNNELINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuTunnelIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->TUNNELINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuTunnelIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuTunnelIndex, dsuEmapIndexStru->TUNNELINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->TUNNELINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pTunnelStru = (DSU_TUNNEL_STRU	 *)MyNew(sizeof(DSU_TUNNEL_STRU) * dsuDataLenStru->wTunnelLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pTunnelStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pTunnelStru, sizeof(DSU_TUNNEL_STRU) * dsuDataLenStru->wTunnelLen, 0xFFu, sizeof(DSU_TUNNEL_STRU) * dsuDataLenStru->wTunnelLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_TUNNEL_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wTunnelLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_TUNNEL_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->TUNNELINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wTunnelLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pTunnelStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			TUNNEL_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->TUNNELINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuTunnelIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wTunnelLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[分相区表信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPowerlessZone(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_POWERLESS_ZONE_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_POWERLESS_ZONE_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;


	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wPowerlessZoneLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_POWERLESS == rowDataLen))
			{
				/*索引数组长度计算*/
				POWERLESS_ZONE_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->POWERLESSZONEINDEXNUM = stru.wId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuPowerlessZoneIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->POWERLESSZONEINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuPowerlessZoneIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuPowerlessZoneIndex, dsuEmapIndexStru->POWERLESSZONEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->POWERLESSZONEINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pPowerlessZoneStru = (DSU_POWERLESS_ZONE_STRU *)MyNew(sizeof(DSU_POWERLESS_ZONE_STRU) * dsuDataLenStru->wPowerlessZoneLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pPowerlessZoneStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pPowerlessZoneStru, sizeof(DSU_POWERLESS_ZONE_STRU) * dsuDataLenStru->wPowerlessZoneLen, 0xFFu, sizeof(DSU_POWERLESS_ZONE_STRU) * dsuDataLenStru->wPowerlessZoneLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_POWERLESSZONE_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wPowerlessZoneLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				FuncRtn1 = dquGetEmapDataIndexArrayLen(EMAP_POWERLESSZONE_ID, &wDataIndexLen);
				if ((1u == FuncRtn1)&&(((0u != wDataIndexLen) && (dsuEmapIndexStru->POWERLESSZONEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen)))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wPowerlessZoneLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pPowerlessZoneStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			POWERLESS_ZONE_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->POWERLESSZONEINDEXNUM < pCurrentStru->wId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuPowerlessZoneIndex[pCurrentStru->wId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wPowerlessZoneLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[车档表信息]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initCarStop(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_CARSTOP_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_CARSTOP_STRU stru = { 0 };    			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn1 = 0u;
	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if ((NULL != g_dsuEmapStru) && (NULL != pDataAddr))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		rtnValue = 0u;
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x01u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		dsuDataLenStru->wCarStopLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_RUNLEVEL == rowDataLen))
			{
				/*索引数组长度计算*/
				CARSTOP_STRU_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuEmapIndexStru->CARSTOPINDEXNUM = stru.carsId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	FuncRtn1 = EmapInitFunStepCtrl(0x02u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		if (0x01u == g_IsEmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuEmapIndexStru->dsuCarStopIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->CARSTOPINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuEmapIndexStru->dsuCarStopIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuEmapIndexStru->dsuCarStopIndex, dsuEmapIndexStru->CARSTOPINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->CARSTOPINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticHeadStru->pCarStopStru = (DSU_CARSTOP_STRU *)MyNew(sizeof(DSU_CARSTOP_STRU) * dsuDataLenStru->wCarStopLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticHeadStru->pCarStopStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticHeadStru->pCarStopStru, sizeof(DSU_CARSTOP_STRU) * dsuDataLenStru->wCarStopLen, 0xFFu, sizeof(DSU_CARSTOP_STRU) * dsuDataLenStru->wCarStopLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
			FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_CARSTOP_ID, &wStruDataLen);

			if ((1u == FuncRtn1)&&(((0u != wStruDataLen) && (dsuDataLenStru->wCarStopLen >= wStruDataLen)) || (0u == wStruDataLen)))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
				dquGetEmapDataIndexArrayLen(EMAP_CARSTOP_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuEmapIndexStru->CARSTOPINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}
	FuncRtn1 = EmapInitFunStepCtrl(0x03u);
	if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn1))
	{
		/*解析数据到 自动折返 结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wCarStopLen; j++)
		{
			pCurrentStru = dsuStaticHeadStru->pCarStopStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			CARSTOP_STRU_init(pData, pCurrentStru);

			if (dsuEmapIndexStru->CARSTOPINDEXNUM < pCurrentStru->carsId)
			{
				rtnValue = 0u;
				break;
			}

			/*完成对应填充索引*/
			dsuEmapIndexStru->dsuCarStopIndex[pCurrentStru->carsId] = j;
			/*循环分步切割处理*/
			FuncRtn1 = EmapInitLoopStepLenCtrl(j);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				break;
			}
		}

		if (j == dsuDataLenStru->wCarStopLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

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
UINT8 initEvacuationSide(UINT8* pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;	                     /*返回值变量声明*/
	UINT8* pData = NULL;
	DSU_EVACUATION_SIDE_STRU stru = { 0 };				 /*疏散侧结构体临时变量*/
	DSU_EVACUATION_SIDE_STRU* pCurrentStru = NULL;	     /*疏散侧结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	UINT8 FuncRtn = DQU_QFUN_FASLE;
	UINT8 FuncRtn1 = DQU_QFUN_FASLE;

	/*空指针防护*/
	if (NULL != pDataAddr)
	{
		/*电子地图数据*/
		DSU_EMAP_INDEX_STRU* dsuEmapIndexStru = NULL;
		DSU_STATIC_HEAD_STRU* dsuStaticHeadStru = NULL;
		DSU_DATA_LEN_STRU* dsuDataLenStru = NULL;

		/*非空防护*/
		if (NULL != g_dsuEmapStru)
		{
			dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
			dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
			dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;

			/*函数未处理完成&当前处于第1阶段*/
			FuncRtn = EmapInitFunStepCtrl(0x01u);
			if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn))
			{
				dsuDataLenStru->wEvacuationSideLen = rowCount;

				/*行数为0，直接认为初始化成功*/
				if (0u == rowCount)
				{
					rtnValue = 1u;
				}
				else
				{
					rowDataLen = dataSize / ((UINT32)rowCount);
					if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_EVACUATE == rowDataLen))
					{
						/*索引数组长度计算*/
						EVASIDE_STRU_init((pDataAddr + dataSize - rowDataLen), &stru);

						/*索引个数为最大ID + 1*/
						dsuEmapIndexStru->EVASIDEINDEXNUM = stru.wId;
					}
					else
					{
						/*数据长度不匹配，返回失败*/
						rtnValue = 0U;
					}
				}
			}
			else
			{
				/*nothing to do*/
			}

			/*函数未处理完成&当前处于第2阶段*/
			FuncRtn = EmapInitFunStepCtrl(0x02u);
			if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn))
			{
				if (0x01u == g_IsEmapAllocMemory)
				{
					/*动态申请索引空间*/
					dsuEmapIndexStru->dsuEvacuationSideIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuEmapIndexStru->EVASIDEINDEXNUM + 2u);

					/*动态申请空间失败*/
					if (NULL == dsuEmapIndexStru->dsuEvacuationSideIndex)
					{
						rtnValue = 0u;
					}
					else
					{
						/*初始化为0xFF*/
						CommonMemSet(dsuEmapIndexStru->dsuEvacuationSideIndex, dsuEmapIndexStru->EVASIDEINDEXNUM * 2u + 2u, 0xFFu, dsuEmapIndexStru->EVASIDEINDEXNUM * 2u + 2u);

						/*动态申请空间*/
						dsuStaticHeadStru->pEvacuationSideStru = (DSU_EVACUATION_SIDE_STRU*)MyNew(sizeof(DSU_EVACUATION_SIDE_STRU) * dsuDataLenStru->wEvacuationSideLen);

						/*动态申请空间失败*/
						if (NULL == dsuStaticHeadStru->pEvacuationSideStru)
						{
							rtnValue = 0u;
						}
						else
						{
							/*内存先设为无效*/
							CommonMemSet(dsuStaticHeadStru->pEvacuationSideStru, sizeof(DSU_EVACUATION_SIDE_STRU) * dsuDataLenStru->wEvacuationSideLen, 0xFFu, sizeof(DSU_EVACUATION_SIDE_STRU) * dsuDataLenStru->wEvacuationSideLen);
						}
					}
				}
				else/*内存空间是由外部产品来分配的*/
				{
					/*检测电子地图对应数据项数组的个数是否超出内存分配范围*/
					FuncRtn1 = dquGetEmapDataStruArrayLen(EMAP_EVACUATION_ID, &wStruDataLen);

					if ((1u == FuncRtn1) && (((0u != wStruDataLen) && (dsuDataLenStru->wEvacuationSideLen >= wStruDataLen)) || (0u == wStruDataLen)))
					{
						rtnValue = 0u;
					}
					else
					{
						/*检测电子地图中对应索引数组的个数是否超出内存分配范围*/
						dquGetEmapDataIndexArrayLen(EMAP_EVACUATION_ID, &wDataIndexLen);
						if (((0u != wDataIndexLen) && (dsuEmapIndexStru->EVASIDEINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen))
						{
							rtnValue = 0u;
						}
						else
						{
							/*nothing to do*/
						}
					}
				}
			}
			else
			{
				/*nothing to do*/
			}

			/*函数未处理完成&当前处于第3阶段*/
			FuncRtn = EmapInitFunStepCtrl(0x03u);
			if ((2u == rtnValue) && (DQU_QFUN_TRUE == FuncRtn))
			{
				/*解析数据到 自动折返 结构体数组 */
				for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuDataLenStru->wEvacuationSideLen; j++)
				{
					pCurrentStru = dsuStaticHeadStru->pEvacuationSideStru + j;

					pData = pDataAddr + (rowDataLen * j);

					/*填充数据结构体*/
					EVASIDE_STRU_init(pData, pCurrentStru);

					if (dsuEmapIndexStru->EVASIDEINDEXNUM < pCurrentStru->wId)
					{
						rtnValue = 0u;
						break;
					}

					/*完成对应填充索引*/
					dsuEmapIndexStru->dsuEvacuationSideIndex[pCurrentStru->wId] = j;

					/*循环分步切割处理*/
					FuncRtn1 = EmapInitLoopStepLenCtrl(j);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						break;
					}
					else
					{
						/*nothing to do*/
					}
				}
								
				if (j == dsuDataLenStru->wEvacuationSideLen)
				{
					rtnValue = 1u;
				}
			}
			else
			{
				/*nothing to do*/
			}
		}
		else
		{
			/*电子地图数据指针为空*/
			rtnValue = 0;
		}
	}
	else
	{
		/*空指针,直接返回失败*/
		rtnValue = 0;
	}

	return rtnValue;
}

/************************************************
函数功能：获取故障码
输入参数：无
输出参数：errRecord故障码数组，errRecordLen数组长度
返回值：1成功
************************************************/
UINT8 dquGetErrNo(UINT8* errRecord, UINT32 * errRecordLen)
{
	UINT32 Ofst = 0u;
	UINT32 i = 0u;
	UINT8 rtnValue = 0u;

	if ((NULL != errRecord) && (NULL != errRecordLen))
	{
		ShortToChar(fileErrNo, &errRecord[Ofst]);
		Ofst += 2u;

		ShortToChar(dataTypeErrNo, &errRecord[Ofst]);
		Ofst += 2u;

		for (i = 0u; i < INIT_ERR_NUM; i++)
		{
			errRecord[Ofst] = initErrNo[i];
			Ofst++;
		}

		*errRecordLen = Ofst;
		rtnValue = 1u;
	}
	else
	{
		/* do nothing */
	}
	return rtnValue;
}

/************************************************
函数功能：静态限速信息对应的插入以Link为索引的表中，
用于优化查询当前LINK相关限速索引使用。
输入参数：
@linkId:当前静态限速表索引编号对应所在LINK编号，即pCurrentStaticResSpeedStru->wLinkId
@limitStcSpdId:当前静态限速表索引编号，即pCurrentStaticResSpeedStru->wId
输出参数：无
返回值：1成功 0失败
************************************************/
UINT8 InsertToLimitStcLinkIdx(UINT16 linkId, UINT16 limitStcSpdId)
{
	UINT8 ret = 0u;

	/*电子地图数据*/
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_STC_LIMIT_LINKIDX_STRU *dsuLimitStcLinkIdx = NULL;
	if ((NULL != g_dsuEmapStru) && (NULL != g_ppDsuLimitStcLinkIdx))
	{
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuLimitStcLinkIdx = *g_ppDsuLimitStcLinkIdx;
		if (NULL == dsuLimitStcLinkIdx)
		{
			ret = 0u;
			return ret;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		ret = 0u;
		return ret;
	}

	if (linkId <= dsuEmapIndexStru->LINKINDEXNUM)/*当linkId在电子地图中linkId范围内时进行如下处理*/
	{
		/*判断当前索引表中linkId是否已经有值，
		如果是初始值则表示当前linkId没有进行过赋值，
		如果不是初始值则表示当前linkId已经有关联静态限速*/
		if (UINT16_MAX == dsuLimitStcLinkIdx[linkId].linkId)
		{
			dsuLimitStcLinkIdx[linkId].linkId = linkId;
			dsuLimitStcLinkIdx[linkId].limitStcSpdInfoCount = 0u;
		}

		if (LINK_MAX_LIMIT_SPD > dsuLimitStcLinkIdx[linkId].limitStcSpdInfoCount)
		{
			/*当前linkId中含有大于1个的静态限速*/
			dsuLimitStcLinkIdx[linkId].limitStcSpdInfoIdBuf[dsuLimitStcLinkIdx[linkId].limitStcSpdInfoCount] = limitStcSpdId;/*将静态限速索引填充到对应linkId*/
			dsuLimitStcLinkIdx[linkId].limitStcSpdInfoCount++;/*对应限速索引个数累加*/
			ret = 1u;
		}
	}
	else/*linkId不再范围内，返回错误*/
	{
		ret = 0u;
	}

	return ret;
}

/*
函数功能：根据道岔表和多开道岔表获取所有实体道岔
参数说明：无
返回值：1成功， 0失败
author:qxt 20160626
*/
UINT8 GetPhyiscalSwitches(void)
{
	UINT8 chReturnValue = 0u;									/*函数返回值*/
	UINT16 i = 0u, j = 0u, k = 0u, m = 0u;						/*循环用变量*/
	UINT16 chFindPhyPointNum = 0u;								/*找到的实体道岔的数目索引*/
	UINT16 wVirtualPointNum = 0u;								/*虚拟道岔数量*/
	UINT16 wPhysicalPointNum = 0u;								/*实体道岔数量*/
	UINT16 tempPointId = 0u;									/*道岔临时变量*/
	UINT8 bVirtualPoint = 0u;									/*是否为虚拟道岔*/
	UINT16 *pVirtualPointIdx = NULL;							/*虚拟道岔编号索引*/
	UINT16 wVirPointIndex = 0u;
	DSU_MULTI_SWITCH_STRU *pMultiPointStru = NULL;				/*多开道岔临时变量结结构体*/
	DSU_POINT_STRU *pPointStru = NULL;							/*单开道岔临时变量结构体*/
	DSU_PHYSICAL_SWITCH_STRU *pTempPhysicalPointStru = NULL;

	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru) && (NULL != g_dsuEmapExtendStru))
	{
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;

		/*遍历多开道岔表，计算虚拟道岔数量*/
		for (m = 0u; m<dsuDataLenStru->wMultiPointLen; m++)
		{
			pMultiPointStru = dsuStaticHeadStru->pMultiPointStru + m;
			wVirtualPointNum += pMultiPointStru->wType - 1u;           /*虚拟道岔数量=多开道岔类型-1*/

		}
		/*计算实体道岔数量=多开道岔+单开道岔-虚拟道岔*/
		wPhysicalPointNum = dsuDataLenStru->wMultiPointLen + dsuDataLenStru->wPointLen - wVirtualPointNum;

		/*动态申请空间*/
		g_dsuEmapExtendStru->pDsuPhysicalSwitchIdx = (DSU_PHYSICAL_SWITCH_STRU*)MyNew(sizeof(DSU_PHYSICAL_SWITCH_STRU) * (wPhysicalPointNum + 1u));

		pVirtualPointIdx = (UINT16*)MyNew(sizeof(UINT16)*wVirtualPointNum + 2u);

		/*动态申请空间成功*/
		if ((NULL != g_dsuEmapExtendStru->pDsuPhysicalSwitchIdx) && (NULL != pVirtualPointIdx))
		{
			/*内存先设为无效*/
			CommonMemSet(g_dsuEmapExtendStru->pDsuPhysicalSwitchIdx, sizeof(DSU_PHYSICAL_SWITCH_STRU)*(wPhysicalPointNum + 1u), 0xFFu, sizeof(DSU_PHYSICAL_SWITCH_STRU)*(wPhysicalPointNum + 1u));

			CommonMemSet(pVirtualPointIdx, wVirtualPointNum + 2u, 0xFFu, wVirtualPointNum + 2u);

			pTempPhysicalPointStru = g_dsuEmapExtendStru->pDsuPhysicalSwitchIdx;
			/*遍历多开道岔表,保存多开实体道岔，并存储所有虚拟道岔*/
			for (i = 0u; i<dsuDataLenStru->wMultiPointLen; i++)
			{
				pMultiPointStru = dsuStaticHeadStru->pMultiPointStru + i;
				/*多开实体道岔*/
				pTempPhysicalPointStru[chFindPhyPointNum].Id = pMultiPointStru->wId;
				pTempPhysicalPointStru[chFindPhyPointNum].type = pMultiPointStru->wType;
				chFindPhyPointNum++;

				/*存储虚拟道岔,N位和R1位为同一道岔，因此数组索引从1开始*/
				for (k = 1; k<pMultiPointStru->wType; k++)
				{
					pVirtualPointIdx[wVirPointIndex] = pMultiPointStru->wSwitchIds[k];
					wVirPointIndex++;

				}
			}

			/*遍历单开道岔表，保存单开实体道岔*/
			for (j = 0u; j<dsuDataLenStru->wPointLen; j++)
			{
				bVirtualPoint = 0u;
				pPointStru = dsuStaticHeadStru->pPointStru + j;
				tempPointId = pPointStru->wId;
				/*遍历所有的虚拟道岔*/
				for (m = 0u; m < wVirtualPointNum; m++)
				{
					if (tempPointId == pVirtualPointIdx[m])/*是虚拟道岔*/
					{
						bVirtualPoint++;
					}
				}
				if ((0u == bVirtualPoint) && (chFindPhyPointNum<wPhysicalPointNum))
				{
					pTempPhysicalPointStru[chFindPhyPointNum].Id = tempPointId;
					pTempPhysicalPointStru[chFindPhyPointNum].type = DSU_SINGLE_POINT_TYPE;/*单开道岔类型*/
					chFindPhyPointNum++;
				}
			}

			/*找到所有实体道岔,进一步验证*/
			if (chFindPhyPointNum == wPhysicalPointNum)
			{

				g_dsuEmapExtendStru->PHYSICALSWITCHNUM = chFindPhyPointNum;
				chReturnValue = 1u;
			}

			/*释放指针*/
			free(pVirtualPointIdx);

			pVirtualPointIdx = NULL;

		}
		else/*内存分配失败*/
		{
			chReturnValue = 0u;
		}

	}
	return chReturnValue;
}

/*
函数功能：建立Link和其他设备间的映射关系
参数说明：无
返回值：1成功， 0失败
author: qxt 20170816
*/
UINT8 GetDeviceInfoInLink(void)
{
	UINT8 chReturnValue = 2u;							/*函数返回值*/
	UINT8 chFlag = 1u;									/*标识位*/
	UINT16 wIndex = 0u;									/*循环用变量*/
	UINT8 FuncRtn1 = 0u;

	DSU_LINK_STRU *pLinkStru = NULL;					/*Link临时变量结结构体*/
	DSU_SIGNAL_STRU *pSignalStru = NULL;				/*信号机临时变量结构体*/
	DSU_BALISE_STRU *pBaliseStru = NULL;				/*应答器临时变量结结构体*/
	DSU_SCREEN_STRU *pScreenStru = NULL;				/*安全屏蔽门临时变量结结构体*/
	DSU_EMERG_STOP_STRU *pESBStru = NULL;				/*紧急停车按钮临时变量结结构体*/
	DSU_STATION_STRU *pStationStru = NULL;				/*停车区域临时变量结结构体*/
	DSU_STOPPOINT_STRU *pStopPointStru = NULL;			/*停车点临时变量结结构体*/
	DSU_PLAT_FORM_STRU *pPlatformStru = NULL;			/*站台临时变量结结构体*/

	DSU_DEVICE_IN_LINK_STRU *pTempDeviceStru = NULL;

	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	UINT16 *dsuLinkIndex = NULL;
	UINT16 *dsuStationIndex = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru) && (NULL != g_dsuEmapExtendStru))
	{
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		dsuLinkIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStationIndex = g_dsuEmapStru->dsuEmapIndexStru->dsuStationIndex;

		/*本文件分配内存空间*/
		if (0x01u == g_IsEmapAllocMemory)
		{
			g_dsuEmapExtendStru->pDsuDeviceInLinkIdx = (DSU_DEVICE_IN_LINK_STRU*)MyNew(sizeof(DSU_DEVICE_IN_LINK_STRU) * (dsuEmapIndexStru->LINKINDEXNUM + 1u));

		}

		if (NULL != g_dsuEmapExtendStru->pDsuDeviceInLinkIdx)
		{

			/*第一阶段设置内存默认值*/
			if (DQU_QFUN_TRUE == EmapCalcFunStepCtrl(0x01u))
			{
				/*内存先设为无效*/
				CommonMemSet(g_dsuEmapExtendStru->pDsuDeviceInLinkIdx, sizeof(DSU_DEVICE_IN_LINK_STRU)*(dsuEmapIndexStru->LINKINDEXNUM + 1u), 0x0u, sizeof(DSU_DEVICE_IN_LINK_STRU)*(dsuEmapIndexStru->LINKINDEXNUM + 1u));

			}

			/*遍历link表，先插入linkId编号*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{

				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wLinkLen; wIndex++)
				{
					pLinkStru = dsuStaticHeadStru->pLinkStru + wIndex;
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pLinkStru->wId;
					pTempDeviceStru->wLinkId = pLinkStru->wId;

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wLinkLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}
			/*①遍历信号机表*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x03u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wSignalLen; wIndex++)
				{
					pSignalStru = dsuStaticHeadStru->pSignalStru + wIndex;
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pSignalStru->wProtectLinkId;

					if (pTempDeviceStru->struSigalDev.wDevCount<LINK_MAX_DEVICE_NUM)
					{
						pTempDeviceStru->struSigalDev.wDevSetId[pTempDeviceStru->struSigalDev.wDevCount++] = pSignalStru->wId;
					}
					else
					{
						chFlag = 0u;
					}

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wSignalLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}

			/*②遍历应答器表*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x04u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wBaliseLen; wIndex++)
				{
					pBaliseStru = dsuStaticHeadStru->pBaliseStru + wIndex;
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pBaliseStru->wLkId;


					if (LINK_MAX_DEVICE_NUM > pTempDeviceStru->struBaliseDev.wDevCount)
					{
						pTempDeviceStru->struBaliseDev.wDevSetId[pTempDeviceStru->struBaliseDev.wDevCount++] = pBaliseStru->wId;
					}
					else
					{
						chFlag = 0u;
					}

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wBaliseLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}



			/*③遍历安全屏蔽门表*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x05u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wScreenLen; wIndex++)
				{
					pScreenStru = dsuStaticHeadStru->pScreenStru + wIndex;
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pScreenStru->wRelatLkId;

					if (LINK_MAX_DEVICE_NUM > pTempDeviceStru->struScreenDev.wDevCount)
					{
						pTempDeviceStru->struScreenDev.wDevSetId[pTempDeviceStru->struScreenDev.wDevCount++] = pScreenStru->wId;
					}
					else
					{
						chFlag = 0u;
					}

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wScreenLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}

			/*④遍历紧急停车按钮表*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x06u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wEmergStopLen; wIndex++)
				{
					pESBStru = dsuStaticHeadStru->pEmergStopStru + wIndex;
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pESBStru->wRelatLkId;

					if (LINK_MAX_DEVICE_NUM > pTempDeviceStru->struESBDev.wDevCount)
					{
						pTempDeviceStru->struESBDev.wDevSetId[pTempDeviceStru->struESBDev.wDevCount++] = pESBStru->wId;
					}
					else
					{
						chFlag = 0u;
					}


					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wEmergStopLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}

			/*⑤遍历停车区域表*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x07u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wStationLen; wIndex++)
				{
					pStationStru = dsuStaticHeadStru->pStationStru + wIndex;
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pStationStru->wLinkId;

					if (LINK_MAX_DEVICE_NUM > pTempDeviceStru->struStationDev.wDevCount)
					{
						pTempDeviceStru->struStationDev.wDevSetId[pTempDeviceStru->struStationDev.wDevCount++] = pStationStru->wId;
					}
					else
					{
						chFlag = 0u;
					}


					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wStationLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}

			/*⑥遍历停车点表*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x08u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wStopPointLen; wIndex++)
				{
					pStopPointStru = dsuStaticHeadStru->pStopPointStru + wIndex;
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pStopPointStru->wLinkId;

					if (pTempDeviceStru->struStopPointDev.wDevCount<LINK_MAX_DEVICE_NUM)
					{
						pTempDeviceStru->struStopPointDev.wDevSetId[pTempDeviceStru->struStopPointDev.wDevCount++] = pStopPointStru->wId;
					}
					else
					{
						chFlag = 0u;
					}


					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wStopPointLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}

			/*⑦遍历站台表*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x09u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wPlatFormLen; wIndex++)
				{
					pPlatformStru = dsuStaticHeadStru->pPlatFormStru + wIndex;
					pStationStru = dsuStaticHeadStru->pStationStru + dsuStationIndex[pPlatformStru->wStationId];
					pTempDeviceStru = g_dsuEmapExtendStru->pDsuDeviceInLinkIdx;
					pTempDeviceStru = pTempDeviceStru + pStationStru->wLinkId;

					if (LINK_MAX_DEVICE_NUM > pTempDeviceStru->struPlatformDev.wDevCount)
					{
						pTempDeviceStru->struPlatformDev.wDevSetId[pTempDeviceStru->struPlatformDev.wDevCount++] = pPlatformStru->wId;
					}
					else
					{
						chFlag = 0u;
					}

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wPlatFormLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 1u;
				}
			}
			/*所有设备查找完成*/

			if (0u == chFlag)
			{
				/*查找失败*/
				chReturnValue = 0u;
			}
		}
		else
		{
			chReturnValue = 0u;
		}
	}
	else
	{
		/*电子地图相关全局指针变量为空*/
		chReturnValue = 0u;
	}
	return chReturnValue;
}

/*************************************************
函数功能：建立Link和坡度间的映射关系
坡度ID（即坡度索引）对应的插入以Link为索引的表中，
用于优化查询当前LINK相关坡度索引。
参数说明：无
返回值：2表示当前没有处理完，需要再次调用此函数； 1表示成功； 0表示失败
author: lmy 20180122
*************************************************/
UINT8 GetGradeInfoCrossLink(void)
{
	UINT8 chReturnValue = 2u;				/*函数返回值*/
	UINT8 chFlag = 1u;						/*标识位*/
	UINT16 wIndex = 0u;						/*循环用变量*/
	UINT8 i = 0u, j = 0u;					/*循环用变量*/
	UINT8 FuncRtn1 = 0u;
	DSU_LINK_STRU *pLinkStru = NULL;		/*Link临时变量结结构体*/
	DSU_GRADE_STRU *pGradeStru = NULL;		/*坡度临时变量结构体*/

	DSU_GRADE_LINKIDX_STRU *pTempGradeLinkIdexStru = NULL;

	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru) && (NULL != g_dsuEmapExtendStru))
	{
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;

		/*本文件分配内存空间*/
		if (0x01u == g_IsEmapAllocMemory)
		{
			g_dsuEmapExtendStru->pDsuGradeInLinkIdx = (DSU_GRADE_LINKIDX_STRU*)MyNew(sizeof(DSU_GRADE_LINKIDX_STRU) * (dsuEmapIndexStru->LINKINDEXNUM + 1u));

		}

		if (NULL != g_dsuEmapExtendStru->pDsuGradeInLinkIdx)
		{

			/*第一阶段设置内存默认值*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x01u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				/*内存先设为无效*/
				CommonMemSet(g_dsuEmapExtendStru->pDsuGradeInLinkIdx, sizeof(DSU_GRADE_LINKIDX_STRU)*(dsuEmapIndexStru->LINKINDEXNUM + 1u), 0x0u, sizeof(DSU_GRADE_LINKIDX_STRU)*(dsuEmapIndexStru->LINKINDEXNUM + 1u));
			}

			/*①遍历link表，先插入linkId编号*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wLinkLen; wIndex++)
				{
					pLinkStru = dsuStaticHeadStru->pLinkStru + wIndex;
					pTempGradeLinkIdexStru = g_dsuEmapExtendStru->pDsuGradeInLinkIdx;
					pTempGradeLinkIdexStru = pTempGradeLinkIdexStru + pLinkStru->wId;
					pTempGradeLinkIdexStru->wLinkId = pLinkStru->wId;

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wLinkLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}
			/*②pTempGradeLinkIdexStru的坡度编号wGradeId先设为无效值0xffff*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x03u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wLinkLen; wIndex++)
				{
					pLinkStru = dsuStaticHeadStru->pLinkStru + wIndex;
					pTempGradeLinkIdexStru = g_dsuEmapExtendStru->pDsuGradeInLinkIdx;
					pTempGradeLinkIdexStru = pTempGradeLinkIdexStru + pLinkStru->wId;

					for (i = 0u; i < LINK_MAX_GRADE_NUM; i++)
					{
						pTempGradeLinkIdexStru->wGradeId[i] = DSU_NULL_16;
					}

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wLinkLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 2u;
				}
			}
			/*③遍历坡度，建立link编号和坡度编号的映射关系*/
			FuncRtn1 = EmapCalcFunStepCtrl(0x04u);
			if (DQU_QFUN_TRUE == FuncRtn1)
			{
				for (wIndex = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; wIndex<dsuDataLenStru->wGradeLen; wIndex++)
				{
					pGradeStru = dsuStaticHeadStru->pGradeStru + wIndex;

					if (pGradeStru->wOrgnLinkId == pGradeStru->wTmnlLinkId)
					{
						pTempGradeLinkIdexStru = g_dsuEmapExtendStru->pDsuGradeInLinkIdx;
						pTempGradeLinkIdexStru = pTempGradeLinkIdexStru + pGradeStru->wOrgnLinkId;
						if (LINK_MAX_GRADE_NUM > pTempGradeLinkIdexStru->wGradeCount)
						{
							pTempGradeLinkIdexStru->wGradeId[pTempGradeLinkIdexStru->wGradeCount++] = pGradeStru->wId;
						}
						else
						{
							chFlag = 0u;
						}
					}
					else
					{
						pTempGradeLinkIdexStru = g_dsuEmapExtendStru->pDsuGradeInLinkIdx;
						pTempGradeLinkIdexStru = pTempGradeLinkIdexStru + pGradeStru->wOrgnLinkId;
						if (LINK_MAX_GRADE_NUM > pTempGradeLinkIdexStru->wGradeCount)
						{
							pTempGradeLinkIdexStru->wGradeId[pTempGradeLinkIdexStru->wGradeCount++] = pGradeStru->wId;
						}
						else
						{
							chFlag = 0u;
						}

						j = 0u;
						while (0xffffu != pGradeStru->wIncludeLinkId[j])
						{
							pTempGradeLinkIdexStru = g_dsuEmapExtendStru->pDsuGradeInLinkIdx;
							pTempGradeLinkIdexStru = pTempGradeLinkIdexStru + pGradeStru->wIncludeLinkId[j];
							if (LINK_MAX_GRADE_NUM > pTempGradeLinkIdexStru->wGradeCount)
							{
								pTempGradeLinkIdexStru->wGradeId[pTempGradeLinkIdexStru->wGradeCount++] = pGradeStru->wId;
							}
							else
							{
								chFlag = 0u;
							}
							j++;
						}

						pTempGradeLinkIdexStru = g_dsuEmapExtendStru->pDsuGradeInLinkIdx;
						pTempGradeLinkIdexStru = pTempGradeLinkIdexStru + pGradeStru->wTmnlLinkId;
						if (LINK_MAX_GRADE_NUM > pTempGradeLinkIdexStru->wGradeCount)
						{
							pTempGradeLinkIdexStru->wGradeId[pTempGradeLinkIdexStru->wGradeCount++] = pGradeStru->wId;
						}
						else
						{
							chFlag = 0u;
						}
					}

					/*循环分步切割处理*/
					FuncRtn1 = EmapCalcLoopStepLenCtrl(wIndex);
					if (DQU_QFUN_TRUE == FuncRtn1)
					{
						chReturnValue = 2u;
						break;
					}
				}
				if (wIndex == dsuDataLenStru->wGradeLen)
				{
					g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
					chReturnValue = 1u;
				}
			}

			if (0u == chFlag)
			{
				/*查找失败*/
				chReturnValue = 0u;
			}
		}
		else
		{
			/*内存空间分配失败*/
			chReturnValue = 0u;
		}
	}
	else
	{
		/*电子地图相关全局指针变量为空，返回失败*/
		chReturnValue = 0u;
	}
	return chReturnValue;
}

/*
函数功能：删除数组中重复的数，内部使用函数
参数说明：输入参数：input 输入数组；len 输入数组大小；
输出参数:去重后的数组
返回值：去重后数组的长度
author: qxt 20180716
*/
UINT8 RemoveRep(UINT16 input[], UINT8 len, UINT16 output[])
{
	UINT8 i = 0u, j = 0u;
	UINT8 flag = 1u;		/*重复标记，1代表无重复，0代表有重复*/
	UINT8 count = 0u;		/*返回值*/

	if ((NULL != input) && (NULL != output))
	{
		/*参数检查*/
		if (0u < len)
		{
			output[count++] = input[0u];/*先给数组的第一个值赋值*/
			for (i = 1u; i < len; i++)
			{
				flag = 1u;/*每次循环需将标志位重新置位*/
				for (j = 0u; j < count; j++)
				{
					if (input[i] == output[j])
					{
						/*有重复的数，将标志位置位置为0，跳出循环*/
						flag = 0u;
						break;
					}
				}
				if (1u == flag)
				{   /*无重复的数，将存入当前的数*/
					output[count++] = input[i];
				}
			}
		}
	}
	return count;
}

/*
函数功能：计算SPKS包含的Link序列
参数说明：无
返回值：2表示当前没有处理完，需要再次调用此函数； 1表示成功； 0表示失败
author: qxt 20180716
*/
UINT8 CalcSPKSIncLinks(void)
{
	UINT8 rtnValue = 2u;								/*函数返回值*/

	UINT16 i = 0u, j = 0u, k = 0u, n = 0u;				/*循环用变量*/
	UINT16 arTempLink[SPKS_INCLUDE_LINK_NUM] = { 0u };	/*临时存放LINK序列*/
	UINT16 iTempIndex = 0u;								/*临时存放LINK序列数组所对应的索引*/
	UINT8 FuncRtn1 = 0u;
	DSU_SPKS_BUTTON_STRU *pSPKSStru = NULL;				/*SPKS临时变量结结构体*/
	DSU_AXLE_SGMT_STRU *pAxleSgmtStru = NULL;			/*计轴区段临时变量结构体*/

	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuDataLenStru) && (NULL != g_dsuEmapStru->dsuStaticHeadStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru))
	{
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			/*遍历SPKS表*/
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wSpksButtonLen; i++)
			{
				pSPKSStru = dsuStaticHeadStru->pSpksButtonStru + i;
				iTempIndex = 0u;
				/*遍历包含的物理区段*/
				for (j = 0u; j < pSPKSStru->wPhysicalSgmtNum; j++)
				{
					/*遍历计轴区段表，寻找与当前物理区段所对应的计轴区段，计算当前计轴区段所包含的LINK*/
					for (k = 0u; k<dsuDataLenStru->wAxleSgmtLen; k++)
					{

						pAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + k;
						if (pAxleSgmtStru->wRelatePhysicalSgmtId == pSPKSStru->wRelatePhysicalSgmtId[j])
						{
							/*存储起点LINK*/
							arTempLink[iTempIndex++] = pAxleSgmtStru->wOrgnLkId;
							/*存储中间LINK*/
							for (n = 0u; n < MIDDLE_LINK_MAX_NUM; n++)
							{
								if (DSU_NULL_16 != pAxleSgmtStru->wMiddleLinkId[n])
								{
									arTempLink[iTempIndex++] = pAxleSgmtStru->wMiddleLinkId[n];

								}
							}
							/*存储终点LINK*/
							arTempLink[iTempIndex++] = pAxleSgmtStru->wTmnlLkId;
						}
					}

				}
				if (0u == iTempIndex)/*数组越界或未找到对应的link*/
				{
					rtnValue = 0u;
					break;
				}
				else
				{
					/*去掉重复的Link*/
					pSPKSStru->wLinkNum = RemoveRep(arTempLink, (UINT8)iTempIndex, pSPKSStru->wIncLinkId);

				}

				/*循环分步切割处理*/
				FuncRtn1 = EmapCalcLoopStepLenCtrl(i);
				if (DQU_QFUN_TRUE == FuncRtn1)
				{
					rtnValue = 2u;
					break;
				}
			}
			if (i == dsuDataLenStru->wSpksButtonLen)
			{
				g_struCalcTaskTab[g_wCurCalcStruProg].bIsInTheLoop = 0u;
				rtnValue = 1u;
			}
		}
		else
		{
			/*不做处理*/
		}
	}
	else
	{
		/*电子地图相关全局指针变量为空，返回失败*/
		rtnValue = 0u;
	}
	return rtnValue;
}

/*计算物理区段包含的link序列*/
UINT8 CalcPhySgmtIncLinks(void)
{
	UINT8 rtnValue = 0u;								/*函数返回值*/
	UINT8 TempRtnVal = 1U;
	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_PHYSICAL_SGMT_STRU *pPhySgmtStru = NULL;
	DSU_AXLE_SGMT_STRU *pAxleSgmtStru = NULL;			/*计轴区段临时变量结构体*/
	UINT8 FuncRtn1 = 0u;
	UINT16 i = 0U;
	UINT16 j = 0U;
	UINT16 n = 0U;
	UINT16 checkIndex = 0u;
	COM_DQU_OBJECT_STRU LinksStru = { 0 };

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuDataLenStru) && (NULL != g_dsuEmapStru->dsuStaticHeadStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru))
	{
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			/*遍历物理区段表*/
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wPhysicalSgmtLen; i++)
			{
				(void)CommonMemSet(&LinksStru, sizeof(COM_DQU_OBJECT_STRU), 0x00U, sizeof(COM_DQU_OBJECT_STRU));
				pPhySgmtStru = dsuStaticHeadStru->pPhysicalSgmtStru + i;
				pPhySgmtStru->wIncLinkNum = 0U;

				/*遍历计轴区段表，寻找与当前物理区段所对应的计轴区段，计算当前计轴区段所包含的LINK*/
				for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
				{
					pAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + j;
					if (pAxleSgmtStru->wRelatePhysicalSgmtId == pPhySgmtStru->wId)
					{
						/*存储起点LINK*/
						for (checkIndex = 0u; checkIndex < LinksStru.ObjNum; checkIndex++)
						{
							if (LinksStru.ObjIds[checkIndex] == pAxleSgmtStru->wOrgnLkId)
							{
								break;
							}
						}
						if (checkIndex == LinksStru.ObjNum)
						{
							LinksStru.ObjIds[LinksStru.ObjNum++] = pAxleSgmtStru->wOrgnLkId;
						}
						else
						{
							/*重复LINK，不添加*/
						}

						/*存储中间LINK*/
						for (n = 0u; n < MIDDLE_LINK_MAX_NUM; n++)
						{
							if (DSU_NULL_16 != pAxleSgmtStru->wMiddleLinkId[n])
							{
								for (checkIndex = 0u; checkIndex < LinksStru.ObjNum; checkIndex++)
								{
									if (LinksStru.ObjIds[checkIndex] == pAxleSgmtStru->wMiddleLinkId[n])
									{
										break;
									}
									else
									{
										/*do nothing*/
									}
								}

								if (checkIndex == LinksStru.ObjNum)
								{
									LinksStru.ObjIds[LinksStru.ObjNum++] = pAxleSgmtStru->wMiddleLinkId[n];
								}
								else
								{
									/*重复LINK，不添加*/
								}
							}
						}

						/*存储终点LINK*/
						for (checkIndex = 0u; checkIndex < LinksStru.ObjNum; checkIndex++)
						{
							if (LinksStru.ObjIds[checkIndex] == pAxleSgmtStru->wTmnlLkId)
							{
								break;
							}
						}
						if (checkIndex == LinksStru.ObjNum)
						{
							LinksStru.ObjIds[LinksStru.ObjNum++] = pAxleSgmtStru->wTmnlLkId;
						}
						else
						{
							/*重复LINK，不添加*/
						}
					}
					else
					{
						/*继续遍历*/
					}
				}

				if (20U < LinksStru.ObjNum)
				{
					/*数组越界防护*/
					TempRtnVal = 0U;
					break;
				}
				else
				{
					pPhySgmtStru->wIncLinkNum = (UINT8)LinksStru.ObjNum;
					(void)CommonMemCpy(pPhySgmtStru->wIncLinkId, LinksStru.ObjNum * sizeof(UINT16), LinksStru.ObjIds, LinksStru.ObjNum * sizeof(UINT16));
				}
			}
		}
		else
		{
			TempRtnVal = 0U;
		}

		if (1U == TempRtnVal)
		{
			rtnValue = 1U;
		}
		else
		{
			/*查询失败*/
		}
	}
	else
	{
		/*入参错误*/
	}

	return rtnValue;
}

/*计算非CBTC区域包含的道岔*/
UINT8 CalcNonCbtcAreaIncPoints(void)
{
	UINT8 rtnValue = 0u;								/*函数返回值*/
	UINT8 TempRtnVal = 1U;
	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_EMAP_INDEX_STRU *dsuEmapIndexStru = NULL;
	DSU_TRAIN_LINE_STRU *dsuTrainLineStru = NULL;
	UINT8 FuncRtn1 = 0u;
	UINT16 i = 0U;
	UINT8 j = 0U;
	LOD_STRU LocA = { 0 };
	LOD_STRU LocB = { 0 };
	UINT16 LinkNum = 0U;
	UINT16 LinkId[100] = { 0u };
	UINT8 wDir = 0u;
	UINT32 Disp = 0U;
	COM_DQU_OBJECT_STRU LinkStru = { 0 };
	COM_DQU_POINT_STRU PointStru = { 0 };
	UINT8 Trash = 0u;

	if ((NULL != g_dsuEmapStru) && (NULL != g_dsuEmapStru->dsuDataLenStru) && (NULL != g_dsuEmapStru->dsuStaticHeadStru) && (NULL != g_dsuEmapStru->dsuEmapIndexStru))
	{
		dsuDataLenStru = g_dsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = g_dsuEmapStru->dsuStaticHeadStru;
		dsuEmapIndexStru = g_dsuEmapStru->dsuEmapIndexStru;
		FuncRtn1 = EmapCalcFunStepCtrl(0x02u);
		if (DQU_QFUN_TRUE == FuncRtn1)
		{
			/*遍历非CBTC区域表*/
			for (i = g_struCalcTaskTab[g_wCurCalcStruProg].wCurInitPos; i < dsuDataLenStru->wTrainLineLen; i++)
			{
				dsuTrainLineStru = dsuStaticHeadStru->pTrainLineStru + i;
				LocA.Lnk = dsuTrainLineStru->wOrgnLinkId;
				LocA.Off = dsuTrainLineStru->dwOrgnLinkOfst;
				LocB.Lnk = dsuTrainLineStru->wTmnlLinkId;
				LocB.Off = dsuTrainLineStru->dwTmnlLinkOfst;
				Disp = dsuLenByPointExp(g_dsuEmapStru, &LocA, &LocB, &LinkNum, LinkId, &wDir, &Trash);
				if ((DSU_NULL_32 == Disp) || (100u < LinkNum))
				{
					/*查询失败*/
					TempRtnVal = 0U;
				}
				else
				{
					LinkStru.ObjNum = LinkNum;
					FuncRtn1 = CommonMemCpy(&LinkStru.ObjIds[0], OBJECT_NUM_MAX * sizeof(UINT16), LinkId, LinkNum * sizeof(UINT16));
					FuncRtn1 &= dsuGetPointsBetweenLinkExp(g_dsuEmapStru, &LinkStru, &PointStru);
					if (1u == FuncRtn1)
					{
						if (16u < PointStru.PointNum)
						{
							/*查询失败*/
							TempRtnVal = 0U;
						}
						else
						{
							dsuTrainLineStru->wIncludePointNum = PointStru.PointNum;
							for (j = 0u; j < dsuTrainLineStru->wIncludePointNum; j++)
							{
								dsuTrainLineStru->wPointId[j] = PointStru.PointInfo[j].PointId;
								dsuTrainLineStru->wPointStatus[j] = PointStru.PointInfo[j].PointExpectedStatus;
							}
						}
					}
					else
					{
						/*查询失败*/
						TempRtnVal = 0U;
					}
				}

				if (1u == TempRtnVal)
				{
					/*继续计算*/
				}
				else
				{
					/*退出*/
					break;
				}
			}
		}
		else
		{
			TempRtnVal = 0U;
		}

		if (1U == TempRtnVal)
		{
			rtnValue = 1U;
		}
		else
		{
			/*查询失败*/
		}
	}
	else
	{
		/*入参错误*/
	}

	return rtnValue;
}
