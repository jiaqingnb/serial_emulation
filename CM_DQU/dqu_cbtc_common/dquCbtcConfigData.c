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
#ifdef ZCAPP_VXWORKS_CODECONTROL
#include <vxWorks.h>
#endif /*End of ZCAPP_VXWORKS_CODECONTROL*/

#include "dquCbtcConfigData.h"
#include "dfsDataRead.h"
#include "CbtcVersion.h"
#include "dquCbtcType.h"
#include "CommonMemory.h" 

#define DQU_ROW_LEN_TRAINTYPE	3U		/*列车车型配置表每行长度*/
#define DQU_ROW_LEN_TRAININFO	6U		/*列车性能参数表每行长度*/
#define DQU_ROW_LEN_CBTCCFG		47U		/*各子系统公用数据表每行长度*/
#define DQU_ROW_LEN_DEPOT		4U		/*车辆段配置信息表每行长度*/

static CBTC_CFG_DATA_STRU *qds_dsuCbtcConfigDataStru = NULL;			/*CBTC配置信息用查询数据源*/
static CBTC_CFG_DATA_STRU *g_pCbtcConfigDataStru = NULL;				/*指向初始化用数据库的指针*/

static DSU_IP_CONFIG_INFO_STRU *g_pIPConfigDataStru = NULL;				/*指向设备IP配置数据库指针*/
static const DSU_IP_CONFIG_INFO_STRU *qds_dsuIPConfigDataStru = NULL;	/*设备IP配置信息查询数据源*/

static dfsDataHead g_DFsDH = { 0 };										/*FS文件的数据头结构体*/
static UINT8*  g_pDataAddr = NULL;										/*FS单元数据地址*/
static UINT32  g_DataSize = 0u;											/*FS单元数据大小（字节数）*/
static UINT16  g_RowCount = 0u;											/*表数据行数*/

static UINT16	g_wCurInitStruProg = 0u;								/*当前结构体初始化进程，分步加载时使用*/
static UINT16   g_wCurMainIndex = 0u;									/*当前主处理索引，分步加载时使用*/

UINT16 g_MaExtendLength = 0u;											/*满足精确停车条件的MA延伸长度*/

/*----------内部函数声明开始--------------*/

/*解析列车车型配置数据1*/
static UINT8 dfsInitTrainModelConfigData(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);
/*列车性能参数配置结构体2*/
static UINT8 dfsInitTrainPerformConfigData(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);
/*占位函数*/
static UINT8 dfsInitNull(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);
/*各子系统公用数据配置结构体3-CC*/
static UINT8 dfsInitSubSystemCommonConfigData_CC(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);
/*车辆段配置数据结构体4-FAO ，互联互通与此版本一至*/
static UINT8 dfsInitTrainDepotConfigData(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount);
/*根据版本配置数据处理过程函数*/
static UINT8 initConfigCbtcInfoTaskTab(void);

/*----------内部函数声明结束--------------*/

/*初始化任务登记表*/
static INIT_TASK_TAB_STRU g_struInitTaskTab[] =
{
	{ DQU_QFUN_TRUE, CBTC_TRAIN_TYPE_CONFIG_ID,		dfsInitTrainModelConfigData,			5u, 1u, 0u },
	{ DQU_QFUN_TRUE, CBTC_TRAIN_INFO_ID,			dfsInitTrainPerformConfigData,			5u, 1u, 0u },
	{ DQU_QFUN_TRUE, CBTC_CONFIG_DATA_ID,			dfsInitNull,							5u, 1u, 0u },
	{ DQU_QFUN_TRUE, CBTC_CONFIG_DATA_ID,			dfsInitNull,							5u, 1u, 0u },
	{ DQU_QFUN_TRUE, CBTC_CONFIG_DATA_ID,			dfsInitNull,							5u, 1u, 0u },
	{ DQU_QFUN_TRUE, CBTC_CONFIG_DATA_ID,			dfsInitNull,							5u, 1u, 0u },
	{ DQU_QFUN_TRUE, CBTC_CONFIG_DATA_ID,			dfsInitSubSystemCommonConfigData_CC,	5u, 1u, 0u },
	{ DQU_QFUN_TRUE, CBTC_CONFIG_DEPOT_ID,			dfsInitTrainDepotConfigData,			5u, 1u, 0u },
};
/*初始化任务登记表项目数量*/
static const UINT16 MAX_INITTASK_COUNT = sizeof(g_struInitTaskTab) / sizeof(INIT_TASK_TAB_STRU);

/*********************************************
*函数功能：系统配置数据版本号获取
*参数说明：供其它系统调用查询函数版本号，防止集成错误。
*入口参数：无
*出口参数：无
*返回值：UINT32表示的版本号
*********************************************/
UINT32 GetDquCbtcVersion(void)
{
	UINT32 dquCbtcVer = 0U;
	dquCbtcVer = VerToU32SoftwareModule(DQU_CBTC_VER1, DQU_CBTC_VER2, DQU_CBTC_VER3, DQU_CBTC_VER4);
	return dquCbtcVer;
}

/*设置CBTC配置数据信息*/
UINT8 dquSetInitCbtcConfigInfoRef(CBTC_CFG_DATA_STRU *pCbtcConfigDataStru)
{
	UINT8 bRetVal = 0u;
	if (NULL != pCbtcConfigDataStru)
	{
		g_pCbtcConfigDataStru = pCbtcConfigDataStru;
		bRetVal = (UINT8)1;
	}
	else
	{
		bRetVal = (UINT8)0;
	}

	return bRetVal;
}

/*根据版本配置数据处理过程函数*/
/*备注：依据互联互通数据结构V12.0.0修改，modify by lmy 20180312*/
static UINT8 initConfigCbtcInfoTaskTab(void)
{
	UINT8 rtnValue = 0U;
	UINT8 cbtcVersionType = 0U;

	cbtcVersionType = GetCbtcSysType();
	if (DQU_CBTC_CPK == cbtcVersionType)/*合库功能配置*/
	{
		g_struInitTaskTab[3].bIsEffective = DQU_QFUN_FASLE;/*合库默认不执行，各子系统公用数据配置结构体3-FAO*/
		g_struInitTaskTab[4].bIsEffective = DQU_QFUN_FASLE;/*合库默认不执行，各子系统公用数据配置结构体3-HLT*/
		g_struInitTaskTab[5].bIsEffective = DQU_QFUN_FASLE;/*合库默认不执行，各子系统公用数据配置结构体3-DG*/
		g_struInitTaskTab[6].bIsEffective = DQU_QFUN_FASLE;/*合库默认不执行，各子系统公用数据配置结构体3-CC*/
		g_struInitTaskTab[7].bIsEffective = DQU_QFUN_FASLE;/*合库默认不执行，车辆段配置*/
		rtnValue = (UINT8)1;
	}
	else if (DQU_CBTC_FAO == cbtcVersionType) /*FAO功能配置*/
	{
		g_struInitTaskTab[2].bIsEffective = DQU_QFUN_FASLE;/*FAO默认不执行，各子系统公用数据配置结构体3-CPK*/
		g_struInitTaskTab[4].bIsEffective = DQU_QFUN_FASLE;/*FAO默认不执行，各子系统公用数据配置结构体3-HLT*/
		g_struInitTaskTab[5].bIsEffective = DQU_QFUN_FASLE;/*FAO默认不执行，各子系统公用数据配置结构体3-DG*/
		g_struInitTaskTab[6].bIsEffective = DQU_QFUN_FASLE;/*FAO默认不执行，各子系统公用数据配置结构体3-CC*/
		rtnValue = (UINT8)1;
	}
	else if (DQU_CBTC_HLT == cbtcVersionType) /*互联互通功能配置*/
	{
		g_struInitTaskTab[2].bIsEffective = DQU_QFUN_FASLE;/*互联互通默认不执行，各子系统公用数据配置结构体3-CPK*/
		g_struInitTaskTab[3].bIsEffective = DQU_QFUN_FASLE;/*互联互通默认不执行，各子系统公用数据配置结构体3-FAO*/
		g_struInitTaskTab[5].bIsEffective = DQU_QFUN_FASLE;/*互联互通默认不执行，各子系统公用数据配置结构体3-DG*/
		g_struInitTaskTab[6].bIsEffective = DQU_QFUN_FASLE;/*FAO默认不执行，各子系统公用数据配置结构体3-CC*/
		rtnValue = (UINT8)1;
	}
	else if (DQU_CBTC_DG == cbtcVersionType)/*单轨功能配置*/
	{
		g_struInitTaskTab[2].bIsEffective = DQU_QFUN_FASLE;/*DG默认不执行，各子系统公用数据配置结构体3-CPK*/
		g_struInitTaskTab[3].bIsEffective = DQU_QFUN_FASLE;/*DG默认不执行，各子系统公用数据配置结构体3-FAO*/
		g_struInitTaskTab[4].bIsEffective = DQU_QFUN_FASLE;/*DG默认不执行，各子系统公用数据配置结构体3-HLT*/
		g_struInitTaskTab[6].bIsEffective = DQU_QFUN_FASLE;/*FAO默认不执行，各子系统公用数据配置结构体3-CC*/
		rtnValue = (UINT8)1;
	}
	else if (DQU_CBTC_CC == cbtcVersionType)/*车车功能配置*/
	{
		g_struInitTaskTab[2].bIsEffective = DQU_QFUN_FASLE;/*DG默认不执行，各子系统公用数据配置结构体3-CPK*/
		g_struInitTaskTab[3].bIsEffective = DQU_QFUN_FASLE;/*DG默认不执行，各子系统公用数据配置结构体3-FAO*/
		g_struInitTaskTab[4].bIsEffective = DQU_QFUN_FASLE;/*DG默认不执行，各子系统公用数据配置结构体3-HLT*/
		g_struInitTaskTab[5].bIsEffective = DQU_QFUN_FASLE;/*FAO默认不执行，各子系统公用数据配置结构体3-DG*/
		rtnValue = (UINT8)1;
	}
	else /*未出现的配置版本值，可能未配置标志位*/
	{
		rtnValue = (UINT8)0;
	}

	return rtnValue;
}

/*系统配置数据初始化*/
UINT8 dquCbtcConfigDataInit(CHAR* FSName)
{
	UINT16	wIndex = 0U;
	UINT8   funcRtn = 0U;
	UINT8	rtnValue = 0U;
	UINT32	DataID = 0U;/*数据类型标识*/
	UINT8	*pDataAddr = NULL;
	UINT32	DataSize = 0U;
	UINT16	rowCount = 0U;

	if ((NULL != g_pCbtcConfigDataStru) && (NULL != FSName))
	{

		/*读取FS文件,并验证文件*/
		if ((UINT8)1 != dataRead(FSName))
		{
			/*数据读取不成功*/
			fileErrNo |= ERR_FILE_READ;
			rtnValue = (UINT8)0;
			return rtnValue;
		}
		else
		{
			/*根据版本配置数据处理过程函数*/
			funcRtn = initConfigCbtcInfoTaskTab();
			if ((UINT8)0 == funcRtn)
			{
				rtnValue = (UINT8)0;
				return rtnValue;
			}
		}

		/*索引初始化归零*/
		wIndex = (UINT16)0;
		do /*解析数据到相应的结构体*/
		{
			if (DQU_QFUN_TRUE == g_struInitTaskTab[wIndex].bIsEffective)/*判断这个初始化函数是否执行*/
			{
				/*获取配置数据*/
				DataID = g_struInitTaskTab[wIndex].DataTypeId;
				funcRtn = dquGetConfigDataNew(FSName, CBTC_CONFIGDATA_TYPE, DataID, &pDataAddr, &DataSize, &rowCount);
				if ((UINT8)1 != funcRtn)
				{
					/*读取数据失败*/
					rtnValue = (UINT8)0;
					return rtnValue;
				}
				funcRtn = g_struInitTaskTab[wIndex].pExeFun(pDataAddr, DataSize, rowCount);
				if ((UINT8)0 == funcRtn)
				{
					rtnValue = (UINT8)0;
					return rtnValue;
				}
				else/*继续执行*/
				{
				}
			}

			wIndex++;
		} while (wIndex<MAX_INITTASK_COUNT);

		/*默认调用此接口自动关联查询数据源*/
		funcRtn = dquSetCurCbtcCfgInfoQueryDataSource(g_pCbtcConfigDataStru);

		if ((UINT8)0 == funcRtn)
		{
			rtnValue = (UINT8)0;
			return rtnValue;
		}
		else/*继续执行*/
		{
			rtnValue = (UINT8)1;
		}
	}
	else
	{
		rtnValue = (UINT8)0;
	}

	return rtnValue;

}

/******************************************
*函数功能：系统配置数据初始化扩展接口
*参数说明： pDataBuf,传入数据块，
*			cbtcConfigDataStru,外部实体系统配置数据存储结构体指针
*			mode,处理模式，1上电一次加载，2运行时多次处理
*			timeSlice，执行的时间片（单位：毫秒）
*返回值：  0失败，1成功，2分步处理未完成
*修改：by yt 2021年9月11日 删除冗余变量，修改白盒u32赋值给U16问题
*****************************************/
UINT8 dquCbtcConfigDataInitExp(UINT8 *pDataBuf, CBTC_CFG_DATA_STRU *cbtcConfigDataStru, UINT8 mode, UINT16 timeSlice)
{
	UINT32 DataID = 0u;
	UINT8 rtnValue = 0U;
	UINT32 dwFreqTimes = 0U;
	UINT8 bExeState = DQU_QFUN_FASLE;
	UINT32 dwOnceTime = 0u, dwUsedTime = 0U;
	UINT32 dwCurTimePoint = 0u, dwBeginTimePoint = 0U;

	UINT8 funcRtn = 0u;

	if ((NULL != pDataBuf) && (NULL != cbtcConfigDataStru))
	{
		if ((UINT16)0 == g_wCurMainIndex)/*主索引变量进入执行态的时候，就不能改变处理模式*/
		{
			g_pCbtcConfigDataStru = cbtcConfigDataStru;
			if (NULL == g_pCbtcConfigDataStru)
			{
				/*全局结构体指针空间未分配*/
				rtnValue = 0;
				return rtnValue;
			}
		}

		do /*上电加载持续运行保持控制*/
		{
			/*获取进入开始时间*/
			dwBeginTimePoint = dquGetCurrentTimeByMS();
			switch (g_wCurMainIndex)/*处理流程控制*/
			{
			case 0:
				/*解析FS文件数据头、数据索引信息*/
				funcRtn = dquAnalyzeFsDataHead(pDataBuf, &g_DFsDH);
				if ((UINT8)1 != funcRtn)
				{
					/*读取数据失败*/
					rtnValue = (UINT8)0;
				}
				else
				{
					/*根据版本配置数据处理过程函数*/
					funcRtn = initConfigCbtcInfoTaskTab();
					if ((UINT8)0 == funcRtn)
					{
						rtnValue = (UINT8)0;
					}
					else
					{
						g_wCurMainIndex = (UINT16)1;
						rtnValue = (UINT8)2;
					}
				}
				break;
			case 1:
				/*获取配置数据*/
				DataID = g_struInitTaskTab[g_wCurInitStruProg].DataTypeId;
				funcRtn = dquGetConfigDataExpNew(pDataBuf, &g_DFsDH, CBTC_CONFIGDATA_TYPE, DataID, &g_pDataAddr, &g_DataSize, &g_RowCount);
				if ((UINT8)1 != funcRtn)
				{
					/*读取数据失败*/
					rtnValue = (UINT8)0;
				}
				else/*获取数据成功*/
				{
					g_wCurMainIndex = (UINT16)2;
					rtnValue = (UINT8)2;
				}
				break;
			case 2:
				funcRtn = g_struInitTaskTab[g_wCurInitStruProg].pExeFun(g_pDataAddr, g_DataSize, g_RowCount);
				if (1U == funcRtn)/*完成本次完整的处理*/
				{
					do
					{
						g_wCurInitStruProg++;/*初始化结构体进行下一个阶段*/
											 /*越过无效处理函数*/
					} while ((DQU_QFUN_FASLE == g_struInitTaskTab[g_wCurInitStruProg].bIsEffective) && (g_wCurInitStruProg<MAX_INITTASK_COUNT));
					/*还有的初始化处理未完成*/
					if (g_wCurInitStruProg<MAX_INITTASK_COUNT)
					{
						g_wCurMainIndex = (UINT16)1;
					}
					else /*if (g_wCurInitStruProg >=MAX_INITTASK_COUNT) 这个条件状态下，完成了所有结构体初始化处理*/
					{
						g_wCurMainIndex = (UINT16)3;
					}
					rtnValue = (UINT8)2;
				}
				else/*处理出现异常*/
				{
					rtnValue = (UINT8)0;
				}

				break;
			case 3:
				rtnValue = (UINT8)1;
				break;
			default:
				rtnValue = (UINT8)0;
				break;

			}
			/*处理完毕或异常退出*/
			if ((UINT8)2 != rtnValue)
			{
				break;
			}

			/*时间统计及计算处理*/
			if (0x02U == mode)
			{
				/*获取当前时间*/
				dwCurTimePoint = dquGetCurrentTimeByMS();
				/*计算已经是使用过的时间*/
				dwOnceTime = dwCurTimePoint - dwBeginTimePoint;
				dwUsedTime += dwOnceTime;
				/*使用的时间小于时间片*/
				if ((dwUsedTime < (UINT32)timeSlice) && (dwOnceTime > 0U))
				{
					/*剩下的处理时间够再执行一次处理的*/
					dwFreqTimes = (timeSlice - dwUsedTime) / dwOnceTime;
					if (dwFreqTimes >= 1U)
					{
						bExeState = DQU_QFUN_TRUE;
					}
					else/*剩余时间不够下一次执行*/
					{
						bExeState = DQU_QFUN_FASLE;
					}
				}
				else/*超时退出*/
				{
					bExeState = DQU_QFUN_FASLE;
				}
			}

			/*处理模式为上电加载时一直到执行完,或执行状态为完毕*/
		} while ((0x01U == mode) || (DQU_QFUN_TRUE == bExeState));

	}
	else
	{
		rtnValue = (UINT8)0;
	}

	return rtnValue;
}

/*编号对照表初始化完毕，执行编号对照表内部公共变量的清理工作
*修改：by yt 2021年9月11日 删除冗余变量
*/
UINT8 dfsuCbtcConfigInitFinally(void)
{
	UINT8 bRetVal = 0U;

	g_wCurInitStruProg = (UINT16)0;/*当前结构体初始化进程，分步加载时使用*/
	g_wCurMainIndex = (UINT16)0; /*当前主处理索引，分步加载时使用*/

	bRetVal = (UINT8)1;
	return bRetVal;
}


/*---------------数据解析处理函数-------------------------*/

/**********************************************
函数功能：初始化[列车车型配置表]
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:1成功,0失败
**********************************************/
static UINT8 dfsInitTrainModelConfigData(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8  rtnValue = 0u;
	UINT32 i = 0u;
	static UINT32 rowDataLen = 0u;
	UINT8 *pData = pDataAddr;

	if (NULL != pDataAddr)
	{
		g_pCbtcConfigDataStru->TrainTypeConfigLen = rowCount;

		if (0U < rowCount)
		{
			rowDataLen = dataSize / rowCount;

			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_TRAINTYPE == rowDataLen))
			{
				for (i = 0U; i<rowCount; i++)
				{
					pData = pDataAddr + rowDataLen*i;

					g_pCbtcConfigDataStru->cbtcTrainTypeConfig[i].TrainName = ShortFromChar(pData);
					pData += 2u;

					g_pCbtcConfigDataStru->cbtcTrainTypeConfig[i].TrainType = *pData;
					pData += 1u;
				}

				rtnValue = 1U;
			}
			else
			{
				rtnValue = 0U;
			}
		}
		else
		{
			rtnValue = 1U;
		}
	}
	else
	{
		rtnValue = 0U;
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[列车性能参数]
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:1成功,0失败
**********************************************/
static UINT8 dfsInitTrainPerformConfigData(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8  rtnValue = 0U;
	UINT32 i = 0U;
	UINT32 rowDataLen = 0u;
	UINT8 *pData = pDataAddr;

	if ((NULL != pDataAddr) && (NULL != g_pCbtcConfigDataStru))
	{
		g_pCbtcConfigDataStru->TrainInfoLen = rowCount;

		if (0U < rowCount)
		{
			rowDataLen = dataSize / rowCount;

			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_TRAININFO == rowDataLen))
			{
				for (i = 0U; i<rowCount; i++)
				{
					pData = pDataAddr + rowDataLen*i;

					/*列车车型*/
					g_pCbtcConfigDataStru->cbtcTrainInfo[i].TrainType = *pData;
					pData += 1u;
					/*列车长度*/
					g_pCbtcConfigDataStru->cbtcTrainInfo[i].TrainLength = ShortFromChar(pData);
					pData += 2u;
					/*列车最大牵引加速度*/
					g_pCbtcConfigDataStru->cbtcTrainInfo[i].TrainMaxTracAcc = *pData;
					pData += 1u;
					/*线路最大下坡坡度*/
					g_pCbtcConfigDataStru->cbtcTrainInfo[i].WorstGrade = ShortFromChar(pData);
					pData += 2u;
				}

				rtnValue = 1u;
			}
			else
			{
				rtnValue = 0u;
			}
		}
		else
		{
			rtnValue = 1u;
		}
	}
	else
	{
		rtnValue = 0u;
	}

	return rtnValue;
}

/*占位函数*/
UINT8 dfsInitNull(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	return 0U;
}

/**********************************************
函数功能：初始化[各子系统公用数据配置]-CC(要求有且仅有1行数据)，新增，add by sds 2019-6-13
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:1成功,0失败
修改记录:  1、删除了数据解耦后取消的部分数据，by lyk 20220307
**********************************************/
static UINT8 dfsInitSubSystemCommonConfigData_CC(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 0u;

	if ((NULL != pDataAddr) && (1u == rowCount) && (DQU_ROW_LEN_CBTCCFG == dataSize))
	{
		/*填充数据结构体*/
		/*线路最高限速*/
		g_pCbtcConfigDataStru->cbtcConfigData.MaxLineLimitSpeed = *pDataAddr;
		pDataAddr += 1u;

		/*临时限速最高限速*/
		g_pCbtcConfigDataStru->cbtcConfigData.MaxTsrSpeed = *pDataAddr;
		pDataAddr += 1u;

		/*NTP校时时区差*/
		g_pCbtcConfigDataStru->cbtcConfigData.TimeZoneDifference = *pDataAddr;
		pDataAddr += 1u;

		/*区间同向行驶双车间隔最小安全距离（cm）*/
		g_pCbtcConfigDataStru->cbtcConfigData.MABackDistance = ShortFromChar(pDataAddr);
		pDataAddr += 2u;

		/*一次设置临时限速的最大个数*/
		g_pCbtcConfigDataStru->cbtcConfigData.MaxTsrNum = ShortFromChar(pDataAddr);
		pDataAddr += 2u;

		/*一个临时限速报文中包含的最大限速区段数量*/
		g_pCbtcConfigDataStru->cbtcConfigData.MaxSectionOfSpeed = ShortFromChar(pDataAddr);
		pDataAddr += 2u;

		/*VOBC-VOBC通信故障判断时间（ms）*/
		g_pCbtcConfigDataStru->cbtcConfigData.VobcVobcCommunicationErrorTime = LongFromChar(pDataAddr);
		pDataAddr += 4u;

		/*VOBC-OC通信故障判断时间*/
		g_pCbtcConfigDataStru->cbtcConfigData.VobcOcCommunicationErrorTime = LongFromChar(pDataAddr);
		pDataAddr += 4u;

		/*VOBC-ITS通信故障判断时间*/
		g_pCbtcConfigDataStru->cbtcConfigData.VobcItsCommunicationErrorTime = LongFromChar(pDataAddr);
		pDataAddr += 4u;

		/*VOBC-TSL通信故障判断时间 by wyd 20210731*/
		g_pCbtcConfigDataStru->cbtcConfigData.VobcTslCommunicationErrorTime = LongFromChar(pDataAddr);
		pDataAddr += 4u;

		/*满足精准停车条件的MA延伸长度*/
		g_pCbtcConfigDataStru->cbtcConfigData.MALengthForATOParking = ShortFromChar(pDataAddr);
		g_MaExtendLength = g_pCbtcConfigDataStru->cbtcConfigData.MALengthForATOParking;
		pDataAddr += 2u;

		/*本线系统定义逻辑方向与上下行方向的关系*/
		g_pCbtcConfigDataStru->cbtcConfigData.LinkLogicDirUpAndDown = *pDataAddr;
		pDataAddr += 1u;

		/*对向行驶双车间隔最小安全距离*/
		g_pCbtcConfigDataStru->cbtcConfigData.OppositeTrackDistance = ShortFromChar(pDataAddr);
		pDataAddr += 2u;

		/*站台追踪时，列车间隔距离*/
		g_pCbtcConfigDataStru->cbtcConfigData.StationTrackDistance = ShortFromChar(pDataAddr);
		pDataAddr += 2u;

		/*线路最差情况下紧急制动距离（单位m）*/
		g_pCbtcConfigDataStru->cbtcConfigData.MaxMALength = LongFromChar(pDataAddr);
		pDataAddr += 4u;

		/*线路最小可能车长*/
		g_pCbtcConfigDataStru->cbtcConfigData.LineMinTrainLen = LongFromChar(pDataAddr);
		pDataAddr += 4u;

		/*正线是否支持动态测试*/
		g_pCbtcConfigDataStru->cbtcConfigData.MainLineDynTestCfg = *pDataAddr;
		pDataAddr += 1u;

		/*动态测试开关*/
		g_pCbtcConfigDataStru->cbtcConfigData.DynamicTestCfg = *pDataAddr;
		pDataAddr += 1u;

		/*远程RM功能开关*/
		g_pCbtcConfigDataStru->cbtcConfigData.RemoteRMCfg = *pDataAddr;
		pDataAddr += 1u;

		/*列车降级资源回收倒计时(ms)*/
		g_pCbtcConfigDataStru->cbtcConfigData.RecoveryResDelay = LongFromChar(pDataAddr);
		pDataAddr += 4u;

		rtnValue = 1u;
	}
	else
	{
		rtnValue = 0u;
	}

	return rtnValue;
}
/**********************************************
函数功能：初始化[车辆段配置信息]
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:1成功,0失败
**********************************************/
static UINT8 dfsInitTrainDepotConfigData(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8  rtnValue = 0u;
	UINT32 i = 0u;
	UINT32 rowDataLen = 0u;
	UINT8 *pData = pDataAddr;

	if ((NULL != pDataAddr) && (NULL != g_pCbtcConfigDataStru))
	{
		g_pCbtcConfigDataStru->DepotConfigLen = rowCount;

		if (0u < rowCount)
		{
			rowDataLen = dataSize / rowCount;

			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_DEPOT == rowDataLen))
			{
				for (i = 0u; i < rowCount; i++)
				{
					pData = pDataAddr + rowDataLen*i;

					/*车辆段编号*/
					g_pCbtcConfigDataStru->cbtcDepotInfo[i].DepotID = *pData;
					pData += 1u;
					/*车辆段所属联锁ID*/
					g_pCbtcConfigDataStru->cbtcDepotInfo[i].DepotCIID = ShortFromChar(pData);
					pData += 2u;
					/*车辆段出库方向*/
					g_pCbtcConfigDataStru->cbtcDepotInfo[i].DepotDir = *pData;
					pData += 1u;
				}

				rtnValue = 1u;
			}
			else
			{
				rtnValue = 0u;
			}
		}
		else
		{
			rtnValue = 1u;
		}
	}
	else
	{
		rtnValue = 0u;
	}

	return rtnValue;
}

/*-----------------------------数据查询接口函数-----------------------*/

/*设置当前查询函数据源*/
UINT8 dquSetCurCbtcCfgInfoQueryDataSource(CBTC_CFG_DATA_STRU *pCbtcConfigDataStru)
{
	UINT8 bRetVal = 0u;
	if (NULL != pCbtcConfigDataStru)
	{
		qds_dsuCbtcConfigDataStru = pCbtcConfigDataStru;
		bRetVal = 1u;
	}
	else
	{
		bRetVal = 0u;
	}

	return bRetVal;
}

/***********************************************
*函数功能：根据列车名称查询列出车型
*输入参数：TrainName列车名
*输出参数：列车车型
*返回值：0失败1成功
**********************************************/
UINT8 dquGetTrainType(UINT16 TrainName, UINT8* pTrainType)
{
	UINT32 i = 0U;
	UINT8 rtnValue = 0U;

	if ((NULL != qds_dsuCbtcConfigDataStru) && (NULL != pTrainType))
	{
		for (i = 0U; i < qds_dsuCbtcConfigDataStru->TrainTypeConfigLen; i++)
		{
			if (qds_dsuCbtcConfigDataStru->cbtcTrainTypeConfig[i].TrainName == TrainName)
			{
				/*找到车型*/
				*pTrainType = qds_dsuCbtcConfigDataStru->cbtcTrainTypeConfig[i].TrainType;
				rtnValue = 1U;
				return rtnValue;
			}
		}
		/*没找到跳出循环*/
		rtnValue = 0U;
	}
	else
	{
		rtnValue = 0U;
	}

	return rtnValue;
}

/*******************************************
*函数功能：跟据列车车型获得列车性能信息
*输入参数：TrainType列车车型
*输出参数：pCbtcTrainInfo列车性能信息结构体
*返回值： 0失败 1成功
*******************************************/
UINT8 dquGetCbtcTrainInfo(UINT8 TrainType, CBTC_TRAIN_INFO_STRU *pCbtcTrainInfo)
{
	UINT32 i = 0U;
	UINT8 rtnValue = 0U;

	if ((NULL != qds_dsuCbtcConfigDataStru) && (NULL != pCbtcTrainInfo))
	{
		for (i = 0U; i < qds_dsuCbtcConfigDataStru->TrainInfoLen; i++)
		{
			if (TrainType == qds_dsuCbtcConfigDataStru->cbtcTrainInfo[i].TrainType)
			{
				*pCbtcTrainInfo = qds_dsuCbtcConfigDataStru->cbtcTrainInfo[i];
				rtnValue = 1U;
				return rtnValue;
			}
			/*没找到跳出循环*/
			rtnValue = 0U;
		}
	}
	else
	{
		rtnValue = 0;
	}

	return rtnValue;
}

/*获取各子系统公用数据配置表*/
UINT8 dquGetCbtcConfigData(CBTC_CONFIG_DATA_STRU* pCbtcConfigDataStru)
{
	UINT8 rtnValue = 0U;

	if ((NULL != qds_dsuCbtcConfigDataStru) && (NULL != pCbtcConfigDataStru))
	{
		*pCbtcConfigDataStru = qds_dsuCbtcConfigDataStru->cbtcConfigData;
		rtnValue = 1U;
	}
	else
	{
		rtnValue = 0U;
	}
	return rtnValue;
}

/*********************************************
*函数功能：获得车辆段配置信息
*输入参数：无
*输出参数：pCbtcDepotInfo车辆段配置信息
*			Length配置信息数量
*返回值： 0失败 1成功
***********************************************/
UINT8 dquGetCbtcDepotInfo(CBTC_DEPOT_INFO_STRU *pCbtcDepotInfo, UINT16 *Length)
{
	UINT32 i = 0u;
	UINT8 rtnValue = 0u;

	if ((NULL != qds_dsuCbtcConfigDataStru) && (NULL != pCbtcDepotInfo) && (NULL != Length))
	{
		if ((0u == qds_dsuCbtcConfigDataStru->DepotConfigLen) || (10u < qds_dsuCbtcConfigDataStru->DepotConfigLen))
		{
			/*没找到跳出循环*/
			rtnValue = 0u;
			return rtnValue;
		}
		else
		{
			for (i = 0u; i < qds_dsuCbtcConfigDataStru->DepotConfigLen; i++)
			{
				*(pCbtcDepotInfo + i) = qds_dsuCbtcConfigDataStru->cbtcDepotInfo[i];
				(*Length)++;
			}
		}
		rtnValue = 1u;
	}
	else
	{
		rtnValue = 0U;
	}

	return rtnValue;
}

/*设置设备IP配置数据信息*/
UINT8 dquSetInitIpConfigInfoRef(DSU_IP_CONFIG_INFO_STRU *pIPConfigDataStru)
{
	UINT8 chReturnValue = 0u; /*返回值*/

	if (NULL != pIPConfigDataStru)
	{
		g_pIPConfigDataStru = pIPConfigDataStru;
		chReturnValue = 1u;
	}
	else
	{
		/*数据源错误*/
	}

	return chReturnValue;
}

/*=====================================================================
功能描述：IP配置表初始化函数，用于检查数据正确性，并将数据结构化。
输入参数：
UINT8* FSName， 文件名或者指针地址
输出参数：
返 回 值：
1，成功
0，失败，数据有错误
========================================================================*/
UINT8 dquIpConfigDataInit(UINT8 *FSName)
{
	UINT8 chReturnValue = 1u; /*返回值*/
	UINT8 tempRet = 0u;
	UINT8* pData = NULL;       /*用于读取数据的指针*/
	UINT32 dataLen = 0u;        /*数据长度*/
	UINT32 dataOff = 0u;        /*数据偏移量*/
	UINT32 i = 0u;
	UINT32 j = 0u;
	UINT8 tmpDsuMaxZcDsuCommObjNum = 0u;/*临时记录ZCDSU的目标个数*/

										/*有效性检查*/
	if ((NULL == FSName) || (NULL == g_pIPConfigDataStru))
	{
		chReturnValue = 0u;
	}
	else
	{
		/*初始化ip地址结构体表*/
		/*读取数据*/
		tempRet = dquGetConfigData((CHAR*)FSName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_IP_ID, &pData, &dataLen);
		if (1u != tempRet)
		{
			/*数据读取失败*/
			chReturnValue = 0u;
		}
		else
		{
			/*继续执行*/
		}

		g_pIPConfigDataStru->IpInfoStruLen = (UINT16)(dataLen / INIT_IP_INFO_STRU_SIZE);  /*计算表格中数据的个数*/
		g_pIPConfigDataStru->pIpInfoStru = (DSU_IP_INFO_STRU*)malloc(sizeof(DSU_IP_INFO_STRU)*(g_pIPConfigDataStru->IpInfoStruLen)); /*为结构体申请空间*/
		if (NULL == g_pIPConfigDataStru->pIpInfoStru)
		{
			/*空间分配失败*/
			chReturnValue = 0U;
		}
		else
		{
			/*继续执行*/
			CommonMemSet(g_pIPConfigDataStru->pIpInfoStru, sizeof(DSU_IP_INFO_STRU)*(g_pIPConfigDataStru->IpInfoStruLen), 0, sizeof(DSU_IP_INFO_STRU)*(g_pIPConfigDataStru->IpInfoStruLen));
			dataOff = 0u;
			/*通过循环将数据解析到结构体变量中，顺序与结构体定义相对应*/
			for (i = 0u; i < g_pIPConfigDataStru->IpInfoStruLen; i++)
			{
				g_pIPConfigDataStru->pIpInfoStru[i].DevName = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pIpInfoStru[i].HlhtID = LongFromChar(pData + dataOff);
				dataOff += 4u;

				CommonMemCpy(g_pIPConfigDataStru->pIpInfoStru[i].IpAddrBlue, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				CommonMemCpy(g_pIPConfigDataStru->pIpInfoStru[i].IpAddrRed, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				g_pIPConfigDataStru->pIpInfoStru[i].SfpBluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
				g_pIPConfigDataStru->pIpInfoStru[i].SfpRedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pIpInfoStru[i].RpBluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
				g_pIPConfigDataStru->pIpInfoStru[i].RpRedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pIpInfoStru[i].RsspBluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
				g_pIPConfigDataStru->pIpInfoStru[i].RsspRedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pIpInfoStru[i].DeviceSoureBluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
				g_pIPConfigDataStru->pIpInfoStru[i].DeviceSoureRedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
			}

			if (dataOff != dataLen)
			{
				/*数据长度不是解析数据的整数倍，说明有异常*/
				chReturnValue = 0u;
			}
			else
			{
				/*继续执行*/
			}
		}

		/*初始化设备类型与通信协议关系表*/
		/*读取数据*/
		tempRet = dquGetConfigData((CHAR*)FSName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_COMM_PROTCL_ID, &pData, &dataLen);
		if (1u != tempRet)
		{
			/*数据读取失败*/
			chReturnValue = 0u;
		}
		else
		{
			/*继续执行*/
		}

		g_pIPConfigDataStru->CommProtclStruLen = (UINT16)(dataLen / INIT_COMM_PROTCL_STRU_SIZE);  /*计算表格中数据的个数*/
		g_pIPConfigDataStru->pCommProtclStru = (DSU_COMM_PROTCL_INFO_STRU*)malloc(sizeof(DSU_COMM_PROTCL_INFO_STRU)*(g_pIPConfigDataStru->CommProtclStruLen)); /*为结构体申请空间*/
		if (NULL == g_pIPConfigDataStru->pCommProtclStru)
		{
			/*空间分配失败*/
			chReturnValue = 0U;
		}
		else
		{
			/*继续执行*/
			CommonMemSet(g_pIPConfigDataStru->pCommProtclStru, sizeof(DSU_COMM_PROTCL_INFO_STRU)*(g_pIPConfigDataStru->CommProtclStruLen), 0, sizeof(DSU_COMM_PROTCL_INFO_STRU)*(g_pIPConfigDataStru->CommProtclStruLen));
			dataOff = 0U;
			/*通过循环将数据解析到结构体变量中，顺序与结构体定义相对应*/
			for (i = 0U; i < g_pIPConfigDataStru->CommProtclStruLen; i++)
			{
				g_pIPConfigDataStru->pCommProtclStru[i].EmitterType = pData[dataOff];
				dataOff++;

				g_pIPConfigDataStru->pCommProtclStru[i].ReceptorType = pData[dataOff];
				dataOff++;

				g_pIPConfigDataStru->pCommProtclStru[i].CommuType = pData[dataOff];
				dataOff++;
			}

			if (dataOff != dataLen)
			{
				/*数据长度不是解析数据的整数倍，说明有异常*/
				chReturnValue = 0U;
			}
			else
			{
				/*继续执行*/
			}
		}

		/*初始化CCOV通信关系配置表信息*/
		/*读取数据*/
		tempRet = dquGetConfigData((CHAR*)FSName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_CCOV_COMM_ID, &pData, &dataLen);
		if (1U != tempRet)
		{
			/*数据读取失败*/
			chReturnValue = 0U;
		}
		else
		{
			/*继续执行*/
		}

		g_pIPConfigDataStru->CcovCommInfoLen = (UINT16)(dataLen / INIT_CCOV_COMM_INFO_STRU_SIZE);  /*计算表格中数据的个数*/
		g_pIPConfigDataStru->pCcovCommInfoStru = (DSU_CCOV_COMM_INFO_STRU*)malloc(sizeof(DSU_CCOV_COMM_INFO_STRU)*(g_pIPConfigDataStru->CcovCommInfoLen)); /*为结构体申请空间*/
		if (NULL == g_pIPConfigDataStru->pCcovCommInfoStru)
		{
			/*空间分配失败*/
			chReturnValue = 0;
		}
		else
		{
			/*继续执行*/
			CommonMemSet(g_pIPConfigDataStru->pCcovCommInfoStru, sizeof(DSU_CCOV_COMM_INFO_STRU)*(g_pIPConfigDataStru->CcovCommInfoLen), 0, sizeof(DSU_CCOV_COMM_INFO_STRU)*(g_pIPConfigDataStru->CcovCommInfoLen));
			dataOff = 0U;
			/*通过循环将数据解析到结构体变量中，顺序与结构体定义相对应*/
			for (i = 0U; i < g_pIPConfigDataStru->CcovCommInfoLen; i++)
			{
				g_pIPConfigDataStru->pCcovCommInfoStru[i].DeviceName_VOBC = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pCcovCommInfoStru[i].DeviceName_WGB = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pCcovCommInfoStru[i].DeviceName_TimeServer = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pCcovCommInfoStru[i].DeviceName_NMS = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pCcovCommInfoStru[i].NMS_BluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pCcovCommInfoStru[i].NMS_RedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
			}

			if (dataOff != dataLen)
			{
				/*数据长度不是解析数据的整数倍，说明有异常*/
				chReturnValue = 0U;
			}
			else
			{
				/*继续执行*/
			}
		}

		/*初始化ZCDSU通信关系配置表信息*/
		/*读取数据*/
		tempRet = dquGetConfigData((CHAR*)FSName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_ZCDSU_COMM_ID, &pData, &dataLen);
		if (1U != tempRet)
		{
			/*数据读取失败*/
			chReturnValue = 0u;
		}
		else
		{
			/*继续执行*/
		}

		g_pIPConfigDataStru->ZcdsuCommInfoLen = (UINT16)(dataLen / INIT_ZCDSU_COMM_INFO_STRU_SIZE_FAO);  /*计算表格中数据的个数*/

		g_pIPConfigDataStru->pZcdsuCommInfoStru = (DSU_ZCDSU_COMM_INFO_STRU*)malloc(sizeof(DSU_ZCDSU_COMM_INFO_STRU)*(g_pIPConfigDataStru->ZcdsuCommInfoLen)); /*为结构体申请空间*/
		if (NULL == g_pIPConfigDataStru->pZcdsuCommInfoStru)
		{
			/*空间分配失败*/
			chReturnValue = 0U;
		}
		else
		{
			/*继续执行*/
			CommonMemSet(g_pIPConfigDataStru->pZcdsuCommInfoStru, sizeof(DSU_ZCDSU_COMM_INFO_STRU)*(g_pIPConfigDataStru->ZcdsuCommInfoLen), 0, sizeof(DSU_ZCDSU_COMM_INFO_STRU)*(g_pIPConfigDataStru->ZcdsuCommInfoLen));
			dataOff = 0U;

			/*计算表格中数据的个数*/
			tmpDsuMaxZcDsuCommObjNum = INIT_MAX_ZCDSU_COMM_OBJ_NUM_FAO;  

			/*通过循环将数据解析到结构体变量中，顺序与结构体定义相对应*/
			for (i = 0U; i < g_pIPConfigDataStru->ZcdsuCommInfoLen; i++)
			{
				g_pIPConfigDataStru->pZcdsuCommInfoStru[i].LocalDevName = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				g_pIPConfigDataStru->pZcdsuCommInfoStru[i].CommObjNum = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				for (j = 0U; j < tmpDsuMaxZcDsuCommObjNum; j++)
				{
					g_pIPConfigDataStru->pZcdsuCommInfoStru[i].CommObjBuf[j] = ShortFromChar(pData + dataOff);
					dataOff += 2u;
				}

			}
			if (dataOff != dataLen)
			{
				/*数据长度不是解析数据的整数倍，说明有异常*/
				chReturnValue = 0U;
			}
			else
			{
				/*继续执行*/
			}
		}



		/*hxq,初始化设备网关表信息*/
		/*读取数据*/
		tempRet = dquGetConfigData((CHAR*)FSName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_DATA_IP_GATEWAY_ID, &pData, &dataLen);
		if (1U != tempRet)
		{
			/*数据读取失败*/
			chReturnValue = 0u;
		}
		else
		{
			/*继续执行*/
		}

		g_pIPConfigDataStru->GatewayInfoLen = (UINT16)(dataLen / INIT_GATEWAY_INFO_STRU_SIZE);  /*计算表格中数据的个数*/
		g_pIPConfigDataStru->pGatewayInfoStru = (DSU_GATEWAY_INFO_STRU*)malloc(sizeof(DSU_GATEWAY_INFO_STRU)*(g_pIPConfigDataStru->GatewayInfoLen)); /*为结构体申请空间*/
		if (NULL == g_pIPConfigDataStru->pGatewayInfoStru)
		{
			/*空间分配失败*/
			chReturnValue = 0U;
		}
		else
		{
			/*继续执行*/
			CommonMemSet(g_pIPConfigDataStru->pGatewayInfoStru, sizeof(DSU_GATEWAY_INFO_STRU)*(g_pIPConfigDataStru->GatewayInfoLen), 0, sizeof(DSU_GATEWAY_INFO_STRU)*(g_pIPConfigDataStru->GatewayInfoLen));
			dataOff = 0U;
			/*通过循环将数据解析到结构体变量中，顺序与结构体定义相对应*/
			for (i = 0U; i < g_pIPConfigDataStru->GatewayInfoLen; i++)
			{
				/*编号 2*/
				dataOff += 2u;

				/*设备类型 1*/
				g_pIPConfigDataStru->pGatewayInfoStru[i].DeviceType = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*设备红网网关 4*/
				CommonMemCpy(g_pIPConfigDataStru->pGatewayInfoStru[i].IpAddrGatewayRed, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				/*设备蓝网网关 4*/
				CommonMemCpy(g_pIPConfigDataStru->pGatewayInfoStru[i].IpAddrGatewayBlue, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				/*子网掩码 4*/
				CommonMemCpy(g_pIPConfigDataStru->pGatewayInfoStru[i].IpAddrSubnetMask, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;
			}

			if (dataOff != dataLen)
			{
				/*数据长度不是解析数据的整数倍，说明有异常*/
				chReturnValue = 0U;
			}
			else
			{
				/*do nothing*/
			}
		}

		/*hxq,初始化CI_ATS网设备IP地址信息*/
		/*读取数据*/
		tempRet = dquGetConfigData((CHAR*)FSName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_CI_ATS_DEVICE_IP_ADDR, &pData, &dataLen);
		if (1u != tempRet)
		{
			/*数据读取失败*/
			chReturnValue = 0u;
		}
		else
		{
			/*继续执行*/
		}

		g_pIPConfigDataStru->CIAtsIpInfoLen = (UINT16)(dataLen / INIT_CI_ATS_IP_INFO_STRU_SIZE);  /*计算表格中数据的个数*/
		g_pIPConfigDataStru->pCIAtsIpInfoStru = (DSU_CI_ATS_IP_INFO_STRU*)malloc(sizeof(DSU_CI_ATS_IP_INFO_STRU)*(g_pIPConfigDataStru->CIAtsIpInfoLen)); /*为结构体申请空间*/
		if (NULL == g_pIPConfigDataStru->pGatewayInfoStru)
		{
			/*空间分配失败*/
			chReturnValue = 0U;
		}
		else
		{
			/*继续执行*/
			CommonMemSet(g_pIPConfigDataStru->pCIAtsIpInfoStru, sizeof(DSU_CI_ATS_IP_INFO_STRU)*(g_pIPConfigDataStru->CIAtsIpInfoLen), 0, sizeof(DSU_CI_ATS_IP_INFO_STRU)*(g_pIPConfigDataStru->CIAtsIpInfoLen));
			dataOff = 0U;
			/*通过循环将数据解析到结构体变量中，顺序与结构体定义相对应*/
			for (i = 0U; i < g_pIPConfigDataStru->CIAtsIpInfoLen; i++)
			{
				/*编号 2*/
				g_pIPConfigDataStru->pCIAtsIpInfoStru[i].DevName = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*蓝网ip地址 4*/
				CommonMemCpy(g_pIPConfigDataStru->pCIAtsIpInfoStru[i].IpAddrBlue, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				/*蓝网子网掩码 4*/
				CommonMemCpy(g_pIPConfigDataStru->pCIAtsIpInfoStru[i].BlueSubNetMask, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				/*红网ip地址 4*/
				CommonMemCpy(g_pIPConfigDataStru->pCIAtsIpInfoStru[i].IpAddrRed, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				/*红网子网掩码 4*/
				CommonMemCpy(g_pIPConfigDataStru->pCIAtsIpInfoStru[i].RedSubNetMask, DSU_LEN_IP_ADDR, pData + dataOff, DSU_LEN_IP_ADDR);
				dataOff += DSU_LEN_IP_ADDR;

				/*SFP蓝红网端口 4*/
				g_pIPConfigDataStru->pCIAtsIpInfoStru[i].SfpBluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
				g_pIPConfigDataStru->pCIAtsIpInfoStru[i].SfpRedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*RP蓝红网端口 4*/
				g_pIPConfigDataStru->pCIAtsIpInfoStru[i].RpBluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
				g_pIPConfigDataStru->pCIAtsIpInfoStru[i].RpRedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*RSSP蓝红网端口 4*/
				g_pIPConfigDataStru->pCIAtsIpInfoStru[i].RsspBluePort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
				g_pIPConfigDataStru->pCIAtsIpInfoStru[i].RsspRedPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
			}

			if (dataOff != dataLen)
			{
				/*数据长度不是解析数据的整数倍，说明有异常*/
				chReturnValue = 0U;
			}
			else
			{

			}
		}

		/*hxq,初始化端口表信息*/
		/*读取数据*/
		tempRet = dquGetConfigData((CHAR*)FSName, DSU_DEVICE_IP_CONFIG_TYPE, DSU_NET_PORT, &pData, &dataLen);
		if (1u != tempRet)
		{
			/*数据读取失败*/
			chReturnValue = 0;
		}
		else
		{
			/*继续执行*/
		}

		g_pIPConfigDataStru->PortInfoLen = (UINT16)(dataLen / INIT_NET_PORT_INFO_STRU_SIZE);  /*计算表格中数据的个数*/
		g_pIPConfigDataStru->pPortInfoStru = (DSU_PORT_INFO_STRU*)malloc(sizeof(DSU_PORT_INFO_STRU)*(g_pIPConfigDataStru->PortInfoLen)); /*为结构体申请空间*/
		if (NULL == g_pIPConfigDataStru->pGatewayInfoStru)
		{
			/*空间分配失败*/
			chReturnValue = 0U;
		}
		else
		{
			/*继续执行*/
			CommonMemSet(g_pIPConfigDataStru->pPortInfoStru, sizeof(DSU_PORT_INFO_STRU)*(g_pIPConfigDataStru->PortInfoLen), 0, sizeof(DSU_PORT_INFO_STRU)*(g_pIPConfigDataStru->PortInfoLen));
			dataOff = 0u;
			/*通过循环将数据解析到结构体变量中，顺序与结构体定义相对应*/
			for (i = 0u; i < g_pIPConfigDataStru->PortInfoLen; i++)
			{
				/*编号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Index = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*发送设备标识符 4*/
				g_pIPConfigDataStru->pPortInfoStru[i].EmitterType = LongFromChar(pData + dataOff);
				dataOff += 4u;

				/*接受设备标识符 4*/
				g_pIPConfigDataStru->pPortInfoStru[i].ReceptorType = LongFromChar(pData + dataOff);
				dataOff += 4u;

				/*发送设备厂商编号 1*/
				g_pIPConfigDataStru->pPortInfoStru[i].EmitterFactory = *(pData + dataOff);
				dataOff += 1U;

				/*接收设备厂商编号 1*/
				g_pIPConfigDataStru->pPortInfoStru[i].ReceptorFactory = *(pData + dataOff);
				dataOff += 1U;

				/*红网发送端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc1RedEmitterPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*红网接收端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc1RedReceptorPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*蓝网发送端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc1BlueEmitterPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*蓝网接收端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc1BlueReceptorPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*红网发送端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc2RedEmitterPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*红网接收端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc2RedReceptorPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*蓝网发送端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc2BlueEmitterPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;

				/*蓝网接收端口号 2*/
				g_pIPConfigDataStru->pPortInfoStru[i].Tc2BlueReceptorPort = ShortFromChar(pData + dataOff);
				dataOff += 2u;
			}

			if (dataOff != dataLen)
			{
				/*数据长度不是解析数据的整数倍，说明有异常*/
				chReturnValue = 0u;
			}
			else
			{
				/*do nothing*/
			}
		}

		if (1u == chReturnValue)
		{
			/*设置设备IP配置数据信息查询数据源*/
			chReturnValue = dquSetCurIPCfgInfoQueryDataSource(g_pIPConfigDataStru);
		}
		else
		{
			/*初始化失败，不设置*/
		}
	}

	/*继续执行*/
	return chReturnValue;
}


/*设置设备IP配置数据查询函数据源*/
UINT8 dquSetCurIPCfgInfoQueryDataSource(DSU_IP_CONFIG_INFO_STRU *pIPConfigDataStru)
{
	UINT8 chReturnValue = 0u; /*返回值*/

	if (NULL != pIPConfigDataStru)
	{
		qds_dsuIPConfigDataStru = pIPConfigDataStru;
		chReturnValue = 1u;
	}
	else
	{
		/*数据源错误*/
	}

	return chReturnValue;
}


/*=======================================================================================================
功能描述：查询设备的通信数量和通信对象。
输入参数：
UINT16 wLocalID，本设备id
UINT8 chLocalType，设备类型
输出参数：
UINT32 * wObjectNum, 通信对象数量
UINT16 *pwObjectInfo,通信对象编号信息，包括ID信息和类型信息
返 回 值：
查询成功，返回1， 	查询失败，返回0。
===========================================================================================================*/
UINT8 CheckCommObjectInfo(UINT16 wLocalID, UINT8 chLocalType, UINT32 * wObjectNum, UINT16 *pwObjectInfo)
{
	UINT16 localName = 0u;  /*根据本地设备id和type计算出来的设备名称*/
	UINT16 i = 0u;
	UINT8 bRet = 0u;

	if ((0xffu < wLocalID) || (NULL == wObjectNum) || (NULL == pwObjectInfo))
	{
		/*设定的数据类型不能满足要求，严重错误*/
		bRet = 0u;
	}
	else
	{
		/*根据设备type和设备id计算设备名称*/
		localName = (((UINT16)chLocalType) << 8u) + (wLocalID & 0xffu);

		/*循环查找设备*/
		for (i = 0u; i < qds_dsuIPConfigDataStru->ZcdsuCommInfoLen; i++)
		{
			if (qds_dsuIPConfigDataStru->pZcdsuCommInfoStru[i].LocalDevName == localName)
			{
				/*找到设备，返回输出参数*/
				*wObjectNum = qds_dsuIPConfigDataStru->pZcdsuCommInfoStru[i].CommObjNum;
				CommonMemCpy(pwObjectInfo, (*wObjectNum) * 2, qds_dsuIPConfigDataStru->pZcdsuCommInfoStru[i].CommObjBuf, (*wObjectNum) * 2);

				/*函数返回成功*/
				break;

			}
		}

		/*循环结束，判断是否找到设备*/
		if (i == qds_dsuIPConfigDataStru->ZcdsuCommInfoLen)
		{
			/*没有找到设备，返回失败*/
			bRet = 0u;
		}
		else
		{
			/*找到设备，返回成功*/
			bRet = 1u;
		}
	}

	return bRet;
}



/*=======================================================================================================
功能描述：查询设备IP
输入参数：
UINT16 wLocalID，设备id
UINT8 chLocalType，设备类型
输出参数：
UINT32 * RedIPInfo，红网ip
UINT32 * BlueIPInfo，蓝网ip
返 回 值：
查询成功，返回1， 	查询失败，返回0。
===========================================================================================================*/

UINT8 CheckIPInfoOfObject(UINT16 wLocalID, UINT8 chLocalType, UINT32 * RedIPInfo, UINT32 * BlueIPInfo)
{
	UINT8 chReturnValue = 0;/*函数返回值*/
	UINT16 i = 0u;/*循环用*/
	UINT16 tempDevName = 0u; /*临时保存devName*/

	if ((0xffu < wLocalID) || (NULL == RedIPInfo) || (NULL == BlueIPInfo))
	{
		/*设定的数据类型不能满足要求，严重错误*/
		chReturnValue = 0u;
	}
	else
	{
		/*计算dev名称*/
		tempDevName = (UINT16)(((((UINT16)chLocalType) << 8) + wLocalID) & 0xffff);

		/*循环查找设备ip地址*/
		for (i = 0u; i < qds_dsuIPConfigDataStru->IpInfoStruLen; i++)
		{
			if (qds_dsuIPConfigDataStru->pIpInfoStru[i].DevName == tempDevName)
			{
				/*已找到当前设备*/
				*RedIPInfo = LongFromChar(qds_dsuIPConfigDataStru->pIpInfoStru[i].IpAddrRed);
				*BlueIPInfo = LongFromChar(qds_dsuIPConfigDataStru->pIpInfoStru[i].IpAddrBlue);
				break;
			}
			else
			{
				/*继续查找*/
			}
		}

		if (i == qds_dsuIPConfigDataStru->IpInfoStruLen)
		{
			/*未找到设备*/
			chReturnValue = 0u;
		}
		else
		{
			/*已找到设备*/
			chReturnValue = 1u;
		}
	}

	return chReturnValue;
}


/*=======================================================================================================
功能描述：根据红网的IP地址和端口号获取设备名称
输入参数：
UINT16 port 端口号
UINT8* ipAdd  IP地址
输出参数：
devName 设备名称  UINT16*
返 回 值：
1:正常返回 0：错误返回
=======================================================================================================*/
UINT8 GetRedNetDevNameAcordIpAndPort(UINT8* ipAdd, UINT16 port, UINT16* devName)
{
	UINT8 retnVal = 0u;
	UINT16 i = 0u;/* 循环使用 */
	UINT8 flag = 0u;
	UINT16 ipInfoLen = 0u;
	DSU_IP_INFO_STRU *pIpInfoStru = NULL;

	if ((NULL == qds_dsuIPConfigDataStru) || (NULL == qds_dsuIPConfigDataStru->pIpInfoStru) || (NULL == ipAdd) || (NULL == devName))
	{
		/*指针为空，返回错误*/
	}
	else
	{
		ipInfoLen = qds_dsuIPConfigDataStru->IpInfoStruLen;
		pIpInfoStru = qds_dsuIPConfigDataStru->pIpInfoStru;

		for (i = 0u; i < ipInfoLen; i++)
		{
			if ((ipAdd[0] == pIpInfoStru[i].IpAddrRed[0]) && (ipAdd[1] == pIpInfoStru[i].IpAddrRed[1]) &&
				(ipAdd[2] == pIpInfoStru[i].IpAddrRed[2]) && (ipAdd[3] == pIpInfoStru[i].IpAddrRed[3]))
			{
				flag = 1u;
			}
			else
			{
				flag = 0u;
			}
			if ((1u == flag) && ((port == pIpInfoStru[i].RpRedPort) || (port == pIpInfoStru[i].RsspRedPort) ||
				(port == pIpInfoStru[i].SfpRedPort)))
			{
				*devName = pIpInfoStru[i].DevName;
				retnVal = 1u;
				break;
			}
			else
			{
				retnVal = 0u;
			}
		}
	}

	return retnVal;
}

/*=======================================================================================================
功能描述：根据蓝网的IP地址和端口号获取设备名称
输入参数：
UINT16 port 端口号
UINT8* ipAdd  IP地址
输出参数：
devName 设备名称  UINT16*
返 回 值：
1:正常返回 0：错误返回
=======================================================================================================*/
UINT8 GetBlueNetDevNameAcordIpAndPort(UINT8* ipAdd, UINT16 port, UINT16* devName)
{
	UINT8 retnVal = 0u;
	UINT16 i = 0u;/* 循环使用 */
	UINT8 flag = 0u;
	UINT16 ipInfoLen = 0u;
	DSU_IP_INFO_STRU *pIpInfoStru = NULL;

	if ((NULL == qds_dsuIPConfigDataStru) || (NULL == qds_dsuIPConfigDataStru->pIpInfoStru) || (NULL == ipAdd) || (NULL == devName))
	{
		/*指针为空，返回错误*/
	}
	else
	{
		ipInfoLen = qds_dsuIPConfigDataStru->IpInfoStruLen;
		pIpInfoStru = qds_dsuIPConfigDataStru->pIpInfoStru;

		for (i = 0u; i < ipInfoLen; i++)
		{
			if ((ipAdd[0] == pIpInfoStru[i].IpAddrBlue[0]) && (ipAdd[1] == pIpInfoStru[i].IpAddrBlue[1]) &&
				(ipAdd[2] == pIpInfoStru[i].IpAddrBlue[2]) && (ipAdd[3] == pIpInfoStru[i].IpAddrBlue[3]))
			{
				flag = 1u;
			}
			else
			{
				flag = 0u;
			}
			if ((1u == flag) && ((port == pIpInfoStru[i].RpBluePort) || (port == pIpInfoStru[i].RsspBluePort) ||
				(port == pIpInfoStru[i].SfpBluePort)))
			{
				*devName = pIpInfoStru[i].DevName;
				retnVal = 1u;
				break;
			}
			else
			{
				retnVal = 0u;
			}
		}
	}

	return retnVal;
}
