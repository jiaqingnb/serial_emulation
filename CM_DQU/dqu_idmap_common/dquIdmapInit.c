/************************************************************************
*
* 文件名   ：  dquIdmapInit.c
* 版权说明 ：  北京交控科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2016.03.21
* 作者     ：  软件部
* 功能描述 ：  编号对照表初始化处理过程
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/
#ifdef ZCAPP_VXWORKS_CODECONTROL
#include <vxWorks.h>
#endif /*End of ZCAPP_VXWORKS_CODECONTROL*/

#include <string.h>
#include "dquIdmapInit.h"
#include "dsuIdmapStruct.h"
#include "dquCbtcType.h"

UINT8	dfsIdmapInitFlag = 0u;
UINT8	initIdMapErrNo[INIT_IDMAP_ERR_NUM] = { 0u };	/*各数据初始化失败故障号数组*/

static dfsDataHead g_DFsDH = { 0 };						/*FS文件的数据头结构体*/
static UINT8*  g_pDataAddr = NULL;						/*FS单元数据地址*/
static UINT32  g_DataSize = 0u;							/*FS单元数据大小（字节数）*/
static UINT16  g_RowCount = 0u;							/*表数据行数*/

static UINT16	g_wCurInitStruProg = 0u;				/*当前结构体初始化进程，分步加载时使用*/
static UINT16   g_wCurMainIndex = 0u;					/*当前主处理索引，分步加载时使用*/
static UINT8	g_IdmapInitMode = 0x01u;				/*编号对照表初始化模式*/
static UINT8	g_IsIdmapAllocMemory = 0x01u;			/*如果调用文件访问接口上电初始化，分配内存*/
static UINT8  IdmapInitFunStepCtrl(UINT16 wStep);		/*电子地图初始化分步控制函数声明*/

/*初始化任务登记表*/
/*把ATS编号对照表是否读取从FALSE改成TRUE，wyd 20211007*/
static INIT_TASK_TAB_STRU g_struInitTaskTab[] =
{
	{ DQU_QFUN_FASLE,	IDMAP_ZC_ID,		initZCNum,			5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_CI_ID,		initCINum,			5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_ATS_ID,		initATSNum,			5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_SIGNAL_ID,	initSignalNum,		5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_TRACKSEC_ID,	initTrackSecNum,	5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_SWITCH_ID,	initSwitchNum,		5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_STATION_ID,	initStationNum,		5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_EMERGBTN_ID,	initEmergBtnNum,	5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_BALISE_ID,	initBaliseNum,		5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_PSD_ID,	    initPSDoorNum,		5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_VOBC_ID,		initVobcNum,		5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_FPD_ID,   	initFPDoorNum,		5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_DSU_ID,   	initDsuNum,			5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_LOGICSEC_ID,  initLogicSecNum,	5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_AOMSEC_ID,    initAomSecNum,		5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_SPKSSEC_ID,   initSpksSecNum,		5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_GODSEC_ID,    initGodSecNum,		5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_STASEC_ID,    initStaSecNum,		5u, 1u, 0u },
	{ DQU_QFUN_FASLE,	IDMAP_PHYSEC_ID,    initPhySecNum,		5u, 1u, 0u },
	{ DQU_QFUN_TRUE,	IDMAP_TSL_ID,       initTslNum,			5u, 1u, 0u },
};

/*初始化任务登记表项目数量*/
static const UINT16 MAX_INITTASK_COUNT = sizeof(g_struInitTaskTab) / sizeof(INIT_TASK_TAB_STRU);

/*********************************************
*函数功能：编号对照表初始化
*参数说明：FSName文件指针
*返回值：0，失败；1，成功
*********************************************/
UINT8 dfsIdmapInit(CHAR * FSName)
{
	UINT16	wIndex = 0u;
	UINT8   funcRtn = 0u;
	UINT8	rtnValue = 0u;
	UINT32	DataID = 0u;/*数据类型标识*/
	UINT8	*pDataAddr = NULL;
	UINT32	DataSize = 0u;
	UINT16	rowCount = 0u;

	/*初始化错误信息记录*/
	CommonMemSet(initIdMapErrNo, INIT_IDMAP_ERR_NUM, 0u, INIT_IDMAP_ERR_NUM);

	if ((NULL != g_dsuIdmapStru) && (NULL != FSName))
	{
		/*初始化一次就行了*/
		if (1u == dfsIdmapInitFlag)
		{
			rtnValue = 1u;
			return rtnValue;
		}

		g_IdmapInitMode = 0x01u;/*上电加载模式*/

		g_IsIdmapAllocMemory = 0x01u;/*默认本接口分配内存*/

		/*动态申请索引空间*/
		g_dsuIdmapStru->dsuIdmapLenStru = (DSU_IDMAP_LEN_STRU*)MyNew(sizeof(DSU_IDMAP_LEN_STRU));

		/*动态申请空间失败*/
		if (NULL != g_dsuIdmapStru->dsuIdmapLenStru)
		{
			/*初始化为0xFF*/
			CommonMemSet(g_dsuIdmapStru->dsuIdmapLenStru, sizeof(DSU_IDMAP_LEN_STRU), 0x0u, sizeof(DSU_IDMAP_LEN_STRU));
		}
		else
		{
			rtnValue = 0u;

		}

		/*动态申请索引空间*/
		g_dsuIdmapStru->dsuStaticIdmapStru = (DSU_STATIC_IDMAP_STRU*)MyNew(sizeof(DSU_STATIC_IDMAP_STRU));

		/*动态申请空间失败*/
		if (NULL != g_dsuIdmapStru->dsuStaticIdmapStru)
		{
			/*初始化为0xFF*/
			CommonMemSet(g_dsuIdmapStru->dsuStaticIdmapStru, sizeof(DSU_STATIC_IDMAP_STRU), 0x0u, sizeof(DSU_STATIC_IDMAP_STRU));
		}
		else
		{
			rtnValue = 0u;
		}

		/*动态申请索引空间*/
		g_dsuIdmapStru->dsuIdmapIndexStru = (DSU_IDMAP_INDEX_STRU*)MyNew(sizeof(DSU_IDMAP_INDEX_STRU));

		/*动态申请空间失败*/
		if (NULL != g_dsuIdmapStru->dsuIdmapIndexStru)
		{
			/*初始化为0xFF*/
			CommonMemSet(g_dsuIdmapStru->dsuIdmapIndexStru, sizeof(DSU_IDMAP_INDEX_STRU), 0x0u, sizeof(DSU_IDMAP_INDEX_STRU));
		}
		else
		{
			rtnValue = 0u;
		}

		/*索引初始化归零*/
		wIndex = 0u;
		do /*解析数据到相应的结构体*/
		{
			if (DQU_QFUN_TRUE == g_struInitTaskTab[wIndex].bIsEffective)/*判断这个初始化函数是否执行*/
			{
				/*获取配置数据*/
				DataID = g_struInitTaskTab[wIndex].DataTypeId;
				funcRtn = dquGetConfigDataNew(FSName, IDMAP_DATA_TYPE, DataID, &pDataAddr, &DataSize, &rowCount);
				if (1u != funcRtn)
				{
					/*函数调用失败*/
					initIdMapErrNo[DataID] |= DEBUG_GET_DATA_FAIL;
					rtnValue = 0u;
					return rtnValue;
				}
				funcRtn = g_struInitTaskTab[wIndex].pExeFun(pDataAddr, DataSize, rowCount);
				if (0u == funcRtn)
				{
					rtnValue = 0u;
					return rtnValue;
				}
				else
				{
					/*继续执行，do nothing*/
				}
			}

			wIndex++;
		} while (wIndex<MAX_INITTASK_COUNT);

		dquSetCurIdMapQueryDataSource(g_dsuIdmapStru);
		/*函数执行标志设为1*/
		dfsIdmapInitFlag = 1u;
		rtnValue = 1u;
	}
	else
	{
		/*函数执行标志设为0*/
		dfsIdmapInitFlag = 0u;
		rtnValue = 0u;
	}
	return rtnValue;
}

/****************************************
*函数功能：初始化编号对照表扩展接口
*参数说明： pDataBuf,传入数据块，
*			dsuIdmapStru,外部实体编号对照表存储结构体指针
*			mode,处理模式，1上电一次加载，2运行时多次处理
*			timeSlice，执行的时间片（单位：毫秒）
*返回值：  0失败，1成功，2分步处理未完成
*修改：by yt 2021年9月11日 删除冗余变量，修改白盒U32赋值给U16问题
*****************************************/
UINT8 dfsIdmapInitExp(UINT8 * pDataBuf, DSU_IDMAP_STRU *dsuIdmapStru, UINT8 mode, UINT16 timeSlice)
{
	UINT32 DataID = 0u;
	UINT8 rtnValue = 0u;
	UINT32 wFreqTimes = 0u;
	UINT8 bExeState = DQU_QFUN_FASLE;
	UINT32 dwOnceTime, dwUsedTime = 0u;
	UINT32 dwCurTimePoint = 0u, dwBeginTimePoint = 0u, dwLastTimePoint = 0u;

	UINT8 funcRtn = 0u;

	if (0u == g_wCurMainIndex)/*主索引变量进入执行态的时候，就不能改变处理模式*/
	{
		g_IdmapInitMode = mode;
		g_IsIdmapAllocMemory = 0x00;/*默认本接口不分配内存*/
									/*根据VOBC需求在此处接口关联指针*/
		if ((NULL != dsuIdmapStru) && (NULL != pDataBuf))
		{
			g_dsuIdmapStru = dsuIdmapStru;
		}
		else
		{
			rtnValue = 0u;
			return rtnValue;
		}
	}


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
			{
				/*解析FS头数据失败*/
				rtnValue = 0u;
			}
			else /*处理成功*/
			{
				g_wCurMainIndex = 1;
				rtnValue = 2u;
			}

			break;
		case 1u:
			/*获取配置数据*/
			DataID = g_struInitTaskTab[g_wCurInitStruProg].DataTypeId;
			funcRtn = dquGetConfigDataExpNew(pDataBuf, &g_DFsDH, IDMAP_DATA_TYPE, DataID, &g_pDataAddr, &g_DataSize, &g_RowCount);
			if (1u != funcRtn)
			{
				/*函数调用失败*/
				initIdMapErrNo[DataID] |= DEBUG_GET_DATA_FAIL;
				rtnValue = 0u;
			}
			else /*处理成功*/
			{
				g_wCurMainIndex = 2;
				rtnValue = 2u;
			}

			break;
		case 2u:
			funcRtn = g_struInitTaskTab[g_wCurInitStruProg].pExeFun(g_pDataAddr, g_DataSize, g_RowCount);
			if (1 == funcRtn)/*完成本次完整的处理*/
			{
				do
				{
					g_wCurInitStruProg++;/*初始化结构体进行下一个阶段*/
										 /*越过无效处理函数*/
				} while ((DQU_QFUN_FASLE == g_struInitTaskTab[g_wCurInitStruProg].bIsEffective) && (g_wCurInitStruProg<MAX_INITTASK_COUNT));
				/*还有的初始化处理未完成*/
				if (g_wCurInitStruProg<MAX_INITTASK_COUNT)
				{
					g_wCurMainIndex = 1;
				}
				else /*if (g_wCurInitStruProg >=MAX_INITTASK_COUNT) 这个条件状态下，完成了所有结构体初始化处理*/
				{
					g_wCurMainIndex = 3;
				}
				rtnValue = 2u;
			}
			else if (2 == funcRtn)/*分步处理未完成*/
			{
				g_struInitTaskTab[g_wCurInitStruProg].wCurProcStage++;
				rtnValue = 2u;

			}
			else/*处理出现异常*/
			{
				rtnValue = 0u;
			}

			break;
		case 3u:
			rtnValue = 1u;

			break;
		default:
			rtnValue = 0u;
			break;

		}

		/*异常退出*/
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
				if (0u < dwOnceTime)
				{
					wFreqTimes = (timeSlice - dwUsedTime) / dwOnceTime;
				}
				else /*如果单次执行时间小于1ms，则继续执行*/
				{
					wFreqTimes = 1u;
				}

				/*执行标志*/
				if (1u <= wFreqTimes)
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

	return rtnValue;
}


/*编号对照表初始化完毕，执行编号对照表内部公共变量的清理工作*/
UINT8 dfsIdmapInitFinally()
{
	UINT16 wIndex = 0u;
	UINT8 bRetVal = 0u;

	g_wCurInitStruProg = 0u;/*当前结构体初始化进程，分步加载时使用*/
	g_wCurMainIndex = 0u; /*当前主处理索引，分步加载时使用*/
	g_IdmapInitMode = 0x01u; /*电子地图初始化模式*/

	/*将数据初始化状态归零*/
	for (wIndex = 0u; wIndex < MAX_INITTASK_COUNT; wIndex++)
	{
		g_struInitTaskTab[wIndex].wCurProcStage = 0x01u;
		g_struInitTaskTab[wIndex].wCurInitPos = 0x0u;
	}

	/*初始化错误信息记录*/
	CommonMemSet(initIdMapErrNo, INIT_IDMAP_ERR_NUM, 0u, INIT_IDMAP_ERR_NUM);

	bRetVal = 1u;
	return bRetVal;
}

/*电子地图初始化分步控制函数*/
static UINT8  IdmapInitFunStepCtrl(UINT16 wStep)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;

	if (0x01u == g_IdmapInitMode)	/*上电加载处理*/
	{
		bIsEffective = DQU_QFUN_TRUE;
	}
	else /*分步加载*/
	{
		if (wStep == g_struInitTaskTab[g_wCurInitStruProg].wCurProcStage)
		{
			bIsEffective = DQU_QFUN_TRUE;
		}
		else if ((0x03u == wStep) && (0x03u < g_struInitTaskTab[g_wCurInitStruProg].wCurProcStage))
		{
			bIsEffective = DQU_QFUN_TRUE;
		}
		else
		{
			/*do nothing*/
		}
	}

	return bIsEffective;

}

/*编号对照表初始化循环步长分步切割处理
*修改：by yt 2021年9月11日 修改白盒U32赋值U16问题
*/
static UINT8 IdmapInitLoopStepLenCtrl(UINT32 dwIndex)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;

	if (0x01u == g_IdmapInitMode)	/*上电加载处理*/
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


/**********************************************
函数功能：初始化[ZC]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initZCNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_ZC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_ZC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wZCNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_ZC == rowDataLen))
			{
				/*索引数组长度计算*/
				ZCNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->ZCNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuZCNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->ZCNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuZCNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuZCNumIndex, dsuIdmapIndexStru->ZCNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->ZCNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pZCNumStru = (DSU_ZC_NUM_STRU *)MyNew(sizeof(DSU_ZC_NUM_STRU) * dsuIdmapLenStru->wZCNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pZCNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pZCNumStru, sizeof(DSU_ZC_NUM_STRU) * dsuIdmapLenStru->wZCNumLen, 0xFFu, sizeof(DSU_ZC_NUM_STRU) * dsuIdmapLenStru->wZCNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_CI_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wZCNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_CI_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->ZCNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wZCNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pZCNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			ZCNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuZCNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wZCNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[CI]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initCINum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_CI_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_CI_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;


	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wCINumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_CI == rowDataLen))
			{
				/*索引数组长度计算*/
				CINumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->CINUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuCINumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->CINUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuCINumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuCINumIndex, dsuIdmapIndexStru->CINUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->CINUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pCINumStru = (DSU_CI_NUM_STRU *)MyNew(sizeof(DSU_CI_NUM_STRU) * dsuIdmapLenStru->wCINumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pCINumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pCINumStru, sizeof(DSU_CI_NUM_STRU) * dsuIdmapLenStru->wCINumLen, 0xFFu, sizeof(DSU_CI_NUM_STRU) * dsuIdmapLenStru->wCINumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_CI_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wCINumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_CI_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->CINUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wCINumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pCINumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			CINumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuCINumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wCINumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[ATS]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initATSNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_ATS_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_ATS_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wATSNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_ATS == rowDataLen))
			{
				/*索引数组长度计算*/
				ATSNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->ATSNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuATSNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->ATSNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuATSNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuATSNumIndex, dsuIdmapIndexStru->ATSNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->ATSNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pATSNumStru = (DSU_ATS_NUM_STRU *)MyNew(sizeof(DSU_ATS_NUM_STRU) * dsuIdmapLenStru->wATSNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pATSNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pATSNumStru, sizeof(DSU_ATS_NUM_STRU) * dsuIdmapLenStru->wATSNumLen, 0xFFu, sizeof(DSU_ATS_NUM_STRU) * dsuIdmapLenStru->wATSNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_ATS_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wATSNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_ATS_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->ATSNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wATSNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pATSNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			ATSNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuATSNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wATSNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[信号机]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initSignalNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_SIGNAL_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_SIGNAL_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wSignalNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_SIGNAL == rowDataLen))
			{
				/*索引数组长度计算*/
				SignalNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->SIGNALNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuSignalNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->SIGNALNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuSignalNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuSignalNumIndex, dsuIdmapIndexStru->SIGNALNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->SIGNALNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pSignalNumStru = (DSU_SIGNAL_NUM_STRU *)MyNew(sizeof(DSU_SIGNAL_NUM_STRU) * dsuIdmapLenStru->wSignalNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pSignalNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pSignalNumStru, sizeof(DSU_SIGNAL_NUM_STRU) * dsuIdmapLenStru->wSignalNumLen, 0xFFu, sizeof(DSU_SIGNAL_NUM_STRU) * dsuIdmapLenStru->wSignalNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_SIGNAL_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wSignalNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_SIGNAL_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->SIGNALNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wSignalNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pSignalNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			SignalNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuSignalNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wSignalNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[轨道区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initTrackSecNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_TRACKSEC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_TRACKSEC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wTrackSecNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_TRACKSEC == rowDataLen))
			{
				/*索引数组长度计算*/
				TrackSecNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->TRACKSECNUMINDEXSIZE = stru.wLinkId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuTrackSecNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->TRACKSECNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuTrackSecNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuTrackSecNumIndex, dsuIdmapIndexStru->TRACKSECNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->TRACKSECNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pTrackSecNumStru = (DSU_TRACKSEC_NUM_STRU *)MyNew(sizeof(DSU_TRACKSEC_NUM_STRU) * dsuIdmapLenStru->wTrackSecNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pTrackSecNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pTrackSecNumStru, sizeof(DSU_TRACKSEC_NUM_STRU) * dsuIdmapLenStru->wTrackSecNumLen, 0xFFu, sizeof(DSU_TRACKSEC_NUM_STRU) * dsuIdmapLenStru->wTrackSecNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_TRACKSEC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wTrackSecNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_TRACKSEC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->TRACKSECNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wTrackSecNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pTrackSecNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			TrackSecNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuTrackSecNumIndex[pCurrentStru->wLinkId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wTrackSecNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[道岔]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initSwitchNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_SWITCH_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_SWITCH_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wSwitchNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_SWITCH == rowDataLen))
			{
				/*索引数组长度计算*/
				SwitchNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->SWITCHNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuSwitchNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->SWITCHNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuSwitchNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuSwitchNumIndex, dsuIdmapIndexStru->SWITCHNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->SWITCHNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pSwitchNumStru = (DSU_SWITCH_NUM_STRU *)MyNew(sizeof(DSU_SWITCH_NUM_STRU) * dsuIdmapLenStru->wSwitchNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pSwitchNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pSwitchNumStru, sizeof(DSU_SWITCH_NUM_STRU) * dsuIdmapLenStru->wSwitchNumLen, 0xFFu, sizeof(DSU_SWITCH_NUM_STRU) * dsuIdmapLenStru->wSwitchNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_SWITCH_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wSwitchNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_SWITCH_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->SWITCHNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wSwitchNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pSwitchNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			SwitchNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuSwitchNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wSwitchNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[逻辑站台]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStationNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_STATION_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_STATION_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wStationNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_STATION == rowDataLen))
			{
				/*索引数组长度计算*/
				StationNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->STATIONNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuStationNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->STATIONNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuStationNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuStationNumIndex, dsuIdmapIndexStru->STATIONNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->STATIONNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pStationNumStru = (DSU_STATION_NUM_STRU *)MyNew(sizeof(DSU_STATION_NUM_STRU) * dsuIdmapLenStru->wStationNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pStationNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pStationNumStru, sizeof(DSU_STATION_NUM_STRU) * dsuIdmapLenStru->wStationNumLen, 0xFFu, sizeof(DSU_STATION_NUM_STRU) * dsuIdmapLenStru->wStationNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_STATION_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wStationNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_STATION_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->STATIONNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wStationNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pStationNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			StationNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuStationNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wStationNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[紧急关闭按钮]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initEmergBtnNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_EMERGBTN_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_EMERGBTN_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wEmergBtnNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_EMERGBTN == rowDataLen))
			{
				/*索引数组长度计算*/
				EmergBtnNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->EMERGBTNNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuEmergBtnIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->EMERGBTNNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuEmergBtnIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuEmergBtnIndex, dsuIdmapIndexStru->EMERGBTNNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->EMERGBTNNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pEmergBtnNumStru = (DSU_EMERGBTN_NUM_STRU *)MyNew(sizeof(DSU_EMERGBTN_NUM_STRU) * dsuIdmapLenStru->wEmergBtnNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pEmergBtnNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pEmergBtnNumStru, sizeof(DSU_EMERGBTN_NUM_STRU) * dsuIdmapLenStru->wEmergBtnNumLen, 0xFFu, sizeof(DSU_EMERGBTN_NUM_STRU) * dsuIdmapLenStru->wEmergBtnNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_EMERGBTN_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wEmergBtnNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_EMERGBTN_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->EMERGBTNNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wEmergBtnNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pEmergBtnNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			EmergBtnNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuEmergBtnIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wEmergBtnNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[应答器]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initBaliseNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_BALISE_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_BALISE_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wBaliseNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_BALISE == rowDataLen))
			{
				/*索引数组长度计算*/
				BaliseNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->BALISENUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuBaliseNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->BALISENUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuBaliseNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuBaliseNumIndex, dsuIdmapIndexStru->BALISENUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->BALISENUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pBaliseNumStru = (DSU_BALISE_NUM_STRU *)MyNew(sizeof(DSU_BALISE_NUM_STRU) * dsuIdmapLenStru->wBaliseNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pBaliseNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pBaliseNumStru, sizeof(DSU_BALISE_NUM_STRU) * dsuIdmapLenStru->wBaliseNumLen, 0xFFu, sizeof(DSU_BALISE_NUM_STRU) * dsuIdmapLenStru->wBaliseNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_BALISE_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wBaliseNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_BALISE_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->BALISENUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wBaliseNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pBaliseNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			BaliseNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuBaliseNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wBaliseNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[屏蔽门]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPSDoorNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_PSD_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_PSD_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wPSDoorNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_PSD == rowDataLen))
			{
				/*索引数组长度计算*/
				PSDoorNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->PSDNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuPSDoorNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->PSDNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuPSDoorNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuPSDoorNumIndex, dsuIdmapIndexStru->PSDNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->PSDNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pPSDNumStru = (DSU_PSD_NUM_STRU *)MyNew(sizeof(DSU_PSD_NUM_STRU) * dsuIdmapLenStru->wPSDoorNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pPSDNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pPSDNumStru, sizeof(DSU_PSD_NUM_STRU) * dsuIdmapLenStru->wPSDoorNumLen, 0xFFu, sizeof(DSU_PSD_NUM_STRU) * dsuIdmapLenStru->wPSDoorNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_PSD_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wPSDoorNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_PSD_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->PSDNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wPSDoorNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pPSDNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			PSDoorNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuPSDoorNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wPSDoorNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[VOBC]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initVobcNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_VOBC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_VOBC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wVobcNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_VOBC == rowDataLen))
			{
				/*索引数组长度计算*/
				VOBCNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->VOBCNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuVobcNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->VOBCNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuVobcNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuVobcNumIndex, dsuIdmapIndexStru->VOBCNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->VOBCNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pVobcNumStru = (DSU_VOBC_NUM_STRU *)MyNew(sizeof(DSU_VOBC_NUM_STRU) * dsuIdmapLenStru->wVobcNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pVobcNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pVobcNumStru, sizeof(DSU_VOBC_NUM_STRU) * dsuIdmapLenStru->wVobcNumLen, 0xFFu, sizeof(DSU_VOBC_NUM_STRU) * dsuIdmapLenStru->wVobcNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_VOBC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wVobcNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_VOBC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->VOBCNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wVobcNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pVobcNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			VOBCNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuVobcNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wVobcNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[防淹门]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initFPDoorNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_FPD_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_FPD_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wFPDoorNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_FPD == rowDataLen))
			{
				/*索引数组长度计算*/
				FPDoorNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->FPDNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuFPDoorNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->FPDNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuFPDoorNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuFPDoorNumIndex, dsuIdmapIndexStru->FPDNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->FPDNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pFPDoorNumStru = (DSU_FPD_NUM_STRU *)MyNew(sizeof(DSU_FPD_NUM_STRU) * dsuIdmapLenStru->wFPDoorNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pFPDoorNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pFPDoorNumStru, sizeof(DSU_FPD_NUM_STRU) * dsuIdmapLenStru->wFPDoorNumLen, 0xFFu, sizeof(DSU_FPD_NUM_STRU) * dsuIdmapLenStru->wFPDoorNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_FPD_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wFPDoorNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_FPD_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->FPDNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wFPDoorNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pFPDoorNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			FPDoorNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuFPDoorNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wFPDoorNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[DSU]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initDsuNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_DSU_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_DSU_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wDsuNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_DSU == rowDataLen))
			{
				/*索引数组长度计算*/
				DsuNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->DSUNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuDsuNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->DSUNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuDsuNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuDsuNumIndex, dsuIdmapIndexStru->DSUNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->DSUNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pDsuNumStru = (DSU_DSU_NUM_STRU *)MyNew(sizeof(DSU_DSU_NUM_STRU) * dsuIdmapLenStru->wDsuNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pDsuNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pDsuNumStru, sizeof(DSU_DSU_NUM_STRU) * dsuIdmapLenStru->wDsuNumLen, 0xFFu, sizeof(DSU_DSU_NUM_STRU) * dsuIdmapLenStru->wDsuNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_DSU_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wDsuNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_DSU_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->DSUNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wDsuNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pDsuNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			DsuNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuDsuNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wDsuNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[逻辑区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initLogicSecNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_LOGICSEC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_LOGICSEC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wLogicSecNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_DSU == rowDataLen))
			{
				/*索引数组长度计算*/
				LogicSecNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->LOGICSECNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuLogicSecNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->LOGICSECNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuLogicSecNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuLogicSecNumIndex, dsuIdmapIndexStru->LOGICSECNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->LOGICSECNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pLogicSecNumStru = (DSU_LOGICSEC_NUM_STRU *)MyNew(sizeof(DSU_LOGICSEC_NUM_STRU) * dsuIdmapLenStru->wLogicSecNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pLogicSecNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pLogicSecNumStru, sizeof(DSU_LOGICSEC_NUM_STRU) * dsuIdmapLenStru->wLogicSecNumLen, 0xFFu, sizeof(DSU_LOGICSEC_NUM_STRU) * dsuIdmapLenStru->wLogicSecNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_LOGICSEC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wLogicSecNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_LOGICSEC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->LOGICSECNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wLogicSecNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pLogicSecNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			LogicSecNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuLogicSecNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wLogicSecNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[AOM]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initAomSecNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_AOMSEC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_AOMSEC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wAomSecNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_AOM == rowDataLen))
			{
				/*索引数组长度计算*/
				AomSecNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->AOMSECNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuAomSecNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->AOMSECNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuAomSecNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuAomSecNumIndex, dsuIdmapIndexStru->AOMSECNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->AOMSECNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pAomSecNumStru = (DSU_AOMSEC_NUM_STRU *)MyNew(sizeof(DSU_AOMSEC_NUM_STRU) * dsuIdmapLenStru->wAomSecNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pAomSecNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pAomSecNumStru, sizeof(DSU_AOMSEC_NUM_STRU) * dsuIdmapLenStru->wAomSecNumLen, 0xFFu, sizeof(DSU_AOMSEC_NUM_STRU) * dsuIdmapLenStru->wAomSecNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_AOMSEC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wAomSecNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_AOMSEC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->AOMSECNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wAomSecNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pAomSecNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			AomSecNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuAomSecNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wAomSecNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[SPKS]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initSpksSecNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_SPKSSEC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_SPKSSEC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wSpksSecNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_SPKS == rowDataLen))
			{
				/*索引数组长度计算*/
				SpksSecNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->SPKSSECNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuSpksSecNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->SPKSSECNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuSpksSecNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuSpksSecNumIndex, dsuIdmapIndexStru->SPKSSECNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->SPKSSECNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pSpksSecNumStru = (DSU_SPKSSEC_NUM_STRU *)MyNew(sizeof(DSU_SPKSSEC_NUM_STRU) * dsuIdmapLenStru->wSpksSecNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pSpksSecNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pSpksSecNumStru, sizeof(DSU_SPKSSEC_NUM_STRU) * dsuIdmapLenStru->wSpksSecNumLen, 0xFFu, sizeof(DSU_SPKSSEC_NUM_STRU) * dsuIdmapLenStru->wSpksSecNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_SPKSSEC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wSpksSecNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_SPKSSEC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->SPKSSECNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wSpksSecNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pSpksSecNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			SpksSecNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuSpksSecNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wSpksSecNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[车库门]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initGodSecNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_GODSEC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_GODSEC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wGodSecNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_GOD == rowDataLen))
			{
				/*索引数组长度计算*/
				GodSecNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->GODSECNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuGodSecNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->GODSECNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuGodSecNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuGodSecNumIndex, dsuIdmapIndexStru->GODSECNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->GODSECNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pGodSecNumStru = (DSU_GODSEC_NUM_STRU *)MyNew(sizeof(DSU_GODSEC_NUM_STRU) * dsuIdmapLenStru->wGodSecNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pGodSecNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pGodSecNumStru, sizeof(DSU_GODSEC_NUM_STRU) * dsuIdmapLenStru->wGodSecNumLen, 0xFFu, sizeof(DSU_GODSEC_NUM_STRU) * dsuIdmapLenStru->wGodSecNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_GODSEC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wGodSecNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_GODSEC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->GODSECNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wGodSecNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pGodSecNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			GodSecNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuGodSecNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wGodSecNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[车站]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initStaSecNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_STASEC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_STASEC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wStaSecNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_STA == rowDataLen))
			{
				/*索引数组长度计算*/
				StaSecNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->STASECNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuStaSecNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->STASECNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuStaSecNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuStaSecNumIndex, dsuIdmapIndexStru->STASECNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->STASECNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pStaSecNumStru = (DSU_STASEC_NUM_STRU *)MyNew(sizeof(DSU_STASEC_NUM_STRU) * dsuIdmapLenStru->wStaSecNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pStaSecNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pStaSecNumStru, sizeof(DSU_STASEC_NUM_STRU) * dsuIdmapLenStru->wStaSecNumLen, 0xFFu, sizeof(DSU_STASEC_NUM_STRU) * dsuIdmapLenStru->wStaSecNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_STASEC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wStaSecNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_STASEC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->STASECNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wStaSecNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pStaSecNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			StaSecNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuStaSecNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wStaSecNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/**********************************************
函数功能：初始化[物理区段]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initPhySecNum(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	DSU_PHYSEC_NUM_STRU *pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_PHYSEC_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU *dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU *dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU *dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wPhySecNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_PHY == rowDataLen))
			{
				/*索引数组长度计算*/
				PhySecNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->PHYSECNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuPhySecNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->PHYSECNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuPhySecNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuPhySecNumIndex, dsuIdmapIndexStru->PHYSECNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->PHYSECNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pPhySecNumStru = (DSU_PHYSEC_NUM_STRU *)MyNew(sizeof(DSU_PHYSEC_NUM_STRU) * dsuIdmapLenStru->wPhySecNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pPhySecNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pPhySecNumStru, sizeof(DSU_PHYSEC_NUM_STRU) * dsuIdmapLenStru->wPhySecNumLen, 0xFFu, sizeof(DSU_PHYSEC_NUM_STRU) * dsuIdmapLenStru->wPhySecNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_PHYSEC_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wPhySecNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_PHYSEC_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->PHYSECNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wPhySecNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pPhySecNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			PhySecNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuPhySecNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wPhySecNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}
/**********************************************
函数功能：初始化[TSL]:计算索引
输入参数：pDataAddr:表数据首地址
dataSize:数据字节长度
rowCount:表数据行数
输出参数：无
返回值:2处理中,1成功,0失败
**********************************************/
UINT8 initTslNum(UINT8* pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	UINT8 rtnValue = 2u;
	UINT8* pData = NULL;
	DSU_TSL_NUM_STRU* pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_TSL_NUM_STRU stru = { 0 };   			/*结构体临时指针变量*/
	UINT16 j = 0u;
	static UINT32 rowDataLen = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;

	/*编号对照表数据*/
	DSU_IDMAP_INDEX_STRU* dsuIdmapIndexStru = NULL;
	DSU_STATIC_IDMAP_STRU* dsuStaticIdmapStru = NULL;
	DSU_IDMAP_LEN_STRU* dsuIdmapLenStru = NULL;

	if ((NULL != g_dsuIdmapStru) && (NULL != pDataAddr))
	{
		dsuIdmapIndexStru = g_dsuIdmapStru->dsuIdmapIndexStru;
		dsuStaticIdmapStru = g_dsuIdmapStru->dsuStaticIdmapStru;
		dsuIdmapLenStru = g_dsuIdmapStru->dsuIdmapLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x01u)))
	{
		dsuIdmapLenStru->wTSLNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;
			if ((0U == dataSize % rowCount) && (DQU_ROW_LEN_IDMAP_TSL == rowDataLen))
			{
				/*索引数组长度计算*/
				TSLNumStru_init(pDataAddr + dataSize - rowDataLen, &stru);

				/*索引个数为最大ID + 1*/
				dsuIdmapIndexStru->TSLNUMINDEXSIZE = stru.wIndexId;
			}
			else
			{
				/*数据长度不匹配，返回失败*/
				rtnValue = 0U;
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsIdmapAllocMemory)
		{
			/*动态申请索引空间*/
			dsuIdmapIndexStru->dsuTSLNumIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuIdmapIndexStru->TSLNUMINDEXSIZE + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuIdmapIndexStru->dsuTSLNumIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuIdmapIndexStru->dsuTSLNumIndex, dsuIdmapIndexStru->TSLNUMINDEXSIZE * 2u + 2u, 0xFFu, dsuIdmapIndexStru->TSLNUMINDEXSIZE * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticIdmapStru->pTSLNumStru = (DSU_TSL_NUM_STRU*)MyNew(sizeof(DSU_TSL_NUM_STRU) * dsuIdmapLenStru->wTSLNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticIdmapStru->pTSLNumStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticIdmapStru->pTSLNumStru, sizeof(DSU_TSL_NUM_STRU) * dsuIdmapLenStru->wTSLNumLen, 0xFFu, sizeof(DSU_TSL_NUM_STRU) * dsuIdmapLenStru->wTSLNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetIdmapDataStruArrayLen(IDMAP_TSL_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuIdmapLenStru->wTSLNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetIdmapDataIndexArrayLen(IDMAP_TSL_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuIdmapIndexStru->TSLNUMINDEXSIZE >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == IdmapInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuIdmapLenStru->wTSLNumLen; j++)
		{
			pCurrentStru = dsuStaticIdmapStru->pTSLNumStru + j;

			pData = pDataAddr + rowDataLen * j;
			/*填充数据结构体*/
			TSLNumStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuIdmapIndexStru->dsuTSLNumIndex[pCurrentStru->wIndexId] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == IdmapInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuIdmapLenStru->wTSLNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}



/*-----------------------------------------------结构体解析函数------------------------------------------------*/

/**************************************
函数功能：ZC编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pZCNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void ZCNumStru_init(UINT8 * pDataAdd, DSU_ZC_NUM_STRU* pZCNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pZCNumStru))
	{
		pData = pDataAdd;

		pZCNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pZCNumStru->dwZCDevId = LongFromChar(pData);  /*ZC设备编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：CI编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pCINumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void CINumStru_init(UINT8 * pDataAdd, DSU_CI_NUM_STRU* pCINumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pCINumStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pCINumStru->wIndexId = ShortFromChar(pData);
		pData += 2u;

		/*CI设备编号*/
		pCINumStru->dwCIDevId = LongFromChar(pData);
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：ATS编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pATSNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void ATSNumStru_init(UINT8 * pDataAdd, DSU_ATS_NUM_STRU* pATSNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pATSNumStru))
	{
		pData = pDataAdd;
		pATSNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pATSNumStru->dwATSDevId = LongFromChar(pData); /*ATS设备编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：信号机编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pSignalNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
******************************************/
void SignalNumStru_init(UINT8 * pDataAdd, DSU_SIGNAL_NUM_STRU* pSignalNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pSignalNumStru))
	{
		pData = pDataAdd;
		pSignalNumStru->wIndexId = ShortFromChar(pData); /*索引编号*/
		pData += 2u;

		pSignalNumStru->dwSigDevId = LongFromChar(pData); /*信号机设备编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：计轴区段表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pAxleSecNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void TrackSecNumStru_init(UINT8 * pDataAdd, DSU_TRACKSEC_NUM_STRU* pAxleSecNumStru)
{
	UINT8 index = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pAxleSecNumStru))
	{
		pData = pDataAdd;
#if 0
		pAxleSecNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pAxleSecNumStru->dwTrackDevId = LongFromChar(pData);  /*轨道区段设备编号*/
		pData += 4u;

		pAxleSecNumStru->wIncLinkCount = ShortFromChar(pData); / *包含link数量* /
			pData += 2u;

		/ *包含link编号1~5 * /
		for (index = 0u; index < 5u; index++)
		{
			pAxleSecNumStru->wIncLinkId[index] = ShortFromChar(pData);
			pData += 2u;
		}

#endif

		pAxleSecNumStru->wLinkId = ShortFromChar(pData);  /*LINK索引编号*/
		pData += 2u;

		pAxleSecNumStru->wTrackNum = ShortFromChar(pData); /*轨道区段数量*/
		pData += 2u;

		for (index = 0u; index < 15u; index++)
		{
			pAxleSecNumStru->dwTrackSecId[index] = LongFromChar(pData);  /*轨道区段编号*/
			pData += 4u;

			pAxleSecNumStru->dwTrackSecLen[index] = LongFromChar(pData);	/*轨道区段长度*/
			pData += 4u;
		}
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：道岔编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pSwitchNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void SwitchNumStru_init(UINT8 * pDataAdd, DSU_SWITCH_NUM_STRU* pSwitchNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pSwitchNumStru))
	{
		pData = pDataAdd;
		pSwitchNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pSwitchNumStru->dwSwiDevId = LongFromChar(pData);   /*道岔设备编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：站台编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pStationNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void StationNumStru_init(UINT8 * pDataAdd, DSU_STATION_NUM_STRU* pStationNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pStationNumStru))
	{
		pData = pDataAdd;
		pStationNumStru->wIndexId = ShortFromChar(pData);   /*索引编号*/
		pData += 2u;

		pStationNumStru->dwStationId = LongFromChar(pData);  /*站台编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：紧急关闭按钮编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pEmergBtnNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void EmergBtnNumStru_init(UINT8 * pDataAdd, DSU_EMERGBTN_NUM_STRU* pEmergBtnNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pEmergBtnNumStru))
	{
		pData = pDataAdd;
		pEmergBtnNumStru->wIndexId = ShortFromChar(pData);    /*索引编号*/
		pData += 2u;

		pEmergBtnNumStru->dwEmergBtnId = LongFromChar(pData);  /*紧急关闭按钮编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：应答器编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pBaliseNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void BaliseNumStru_init(UINT8 * pDataAdd, DSU_BALISE_NUM_STRU* pBaliseNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pBaliseNumStru))
	{
		pData = pDataAdd;
		pBaliseNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pBaliseNumStru->wBaliseId = ShortFromChar(pData);  /*应答器编号*/
		pData += 2u;

		pBaliseNumStru->wLindId = ShortFromChar(pData); /*所属线路编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：屏蔽门编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pPSDoorNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void PSDoorNumStru_init(UINT8 * pDataAdd, DSU_PSD_NUM_STRU* pPSDoorNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pPSDoorNumStru))
	{
		pData = pDataAdd;
		pPSDoorNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pPSDoorNumStru->dwPSDoorId = LongFromChar(pData); /*屏蔽门编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能： VOBC编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pVOBCNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void VOBCNumStru_init(UINT8 * pDataAdd, DSU_VOBC_NUM_STRU* pVOBCNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pVOBCNumStru))
	{
		pData = pDataAdd;
		pVOBCNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pVOBCNumStru->dwVobcId[0] = LongFromChar(pData);   /*VOBC编号1*/
		pData += 4u;

		pVOBCNumStru->dwVobcId[1] = LongFromChar(pData);   /*VOBC编号2*/
		pData += 4u;
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：防淹门编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pFPDoorNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void FPDoorNumStru_init(UINT8 * pDataAdd, DSU_FPD_NUM_STRU* pFPDoorNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pFPDoorNumStru))
	{
		pData = pDataAdd;
		pFPDoorNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pFPDoorNumStru->dwFPDoorId = LongFromChar(pData);  /*防淹门编号*/
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：DSU编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pDSUNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void DsuNumStru_init(UINT8 * pDataAdd, DSU_DSU_NUM_STRU* pDSUNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pDSUNumStru))
	{
		pData = pDataAdd;
		pDSUNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pDSUNumStru->dwDsuId = LongFromChar(pData);  /*DSU编号*/
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：逻辑区段编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pLogicSecNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void LogicSecNumStru_init(UINT8 * pDataAdd, DSU_LOGICSEC_NUM_STRU* pLogicSecNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pLogicSecNumStru))
	{
		pData = pDataAdd;
		pLogicSecNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pLogicSecNumStru->dwLogicSecId = LongFromChar(pData);  /*逻辑区段编号 */
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：AOM编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pAomSecNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void AomSecNumStru_init(UINT8 * pDataAdd, DSU_AOMSEC_NUM_STRU* pAomSecNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pAomSecNumStru))
	{
		pData = pDataAdd;
		pAomSecNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pAomSecNumStru->dwAomSecId[0] = LongFromChar(pData);  /*AOM编号1 */
		pData += 4u;

		pAomSecNumStru->dwAomSecId[1] = LongFromChar(pData);  /*AOM编号2 */
		pData += 4u;
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：SPKS编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pSpksSecNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void SpksSecNumStru_init(UINT8 * pDataAdd, DSU_SPKSSEC_NUM_STRU* pSpksSecNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pSpksSecNumStru))
	{
		pData = pDataAdd;
		pSpksSecNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pSpksSecNumStru->dwSpksSecId = LongFromChar(pData);  /*SPKS编号 */
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：车库门编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pGodSecNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void GodSecNumStru_init(UINT8 * pDataAdd, DSU_GODSEC_NUM_STRU* pGodSecNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pGodSecNumStru))
	{
		pData = pDataAdd;
		pGodSecNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pGodSecNumStru->dwGodSecId = LongFromChar(pData);  /*车库门编号 */
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：车站编号表结构体初始化函数  by slm 2019-1-15
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pStaSecNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void StaSecNumStru_init(UINT8 * pDataAdd, DSU_STASEC_NUM_STRU* pStaSecNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pStaSecNumStru))
	{
		pData = pDataAdd;
		pStaSecNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pStaSecNumStru->dwStaSecId = LongFromChar(pData);  /*车站编号 */
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：物理区段编号表结构体初始化函数  by slm 2019-1-16
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pStaSecNumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void PhySecNumStru_init(UINT8 * pDataAdd, DSU_PHYSEC_NUM_STRU* pPhySecNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pPhySecNumStru))
	{
		pData = pDataAdd;
		pPhySecNumStru->wIndexId = ShortFromChar(pData);  /*索引编号*/
		pData += 2u;

		pPhySecNumStru->dwPhySecId = LongFromChar(pData);  /*物理区段编号 */
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：TSL编号表结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pCINumStru 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
void TSLNumStru_init(UINT8* pDataAdd, DSU_TSL_NUM_STRU* pTSLNumStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pTSLNumStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pTSLNumStru->wIndexId = ShortFromChar(pData);
		pData += 2u;

		/*TSL设备无线通信编号*/
		pTSLNumStru->dwTslWirelessCommId = LongFromChar(pData);
		pData += 4u;

		/*TSL设备有线通信编号*/
		pTSLNumStru->dwTslWireCommId = LongFromChar(pData);
	}
	else
	{
		/* do nothing */
	}
}


/************************************************
函数功能：获取故障码
输入参数：无
输出参数：errRecord故障码数组，errRecordLen数组长度
返回值：1成功
************************************************/
UINT8 dquGetIdMapErrNo(UINT8* errRecord, UINT32 * errRecordLen)
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

		for (i = 0u; i < INIT_IDMAP_ERR_NUM; i++)
		{
			errRecord[Ofst] = initIdMapErrNo[i];
			Ofst++;
		}

		*errRecordLen = Ofst;
		rtnValue = 1u;
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}
