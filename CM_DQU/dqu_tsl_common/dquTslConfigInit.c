/************************************************************************
*
* 文件名   ：  dquTslConfigInit.c
* 版权说明 ：  交控科技股份有限公司
* 版本号   ：  1.0
* 创建时间 ：  2021年4月27日
* 作者     ：  软件部
* 功能描述 ：  TSL配置数据表初始化处理过程
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/
#include <string.h>
#include "dquTslConfigInit.h"
#include "dsuTslConfigStruct.h"
#include "dquCbtcType.h"

UINT8	dfsTslConfigInitFlag = 0u;
UINT8	initTslConfigErrNo[INIT_TSLCONFIG_ERR_NUM] = { 0u };	/*各数据初始化失败故障号数组*/

static dfsDataHead g_DFsDH = { 0 };								/*FS文件的数据头结构体*/
static UINT8*  g_pDataAddr = NULL;								/*FS单元数据地址*/
static UINT32  g_DataSize = 0u;									/*FS单元数据大小（字节数）*/
static UINT16  g_RowCount = 0u;									/*表数据行数*/

static UINT16	g_wCurInitStruProg = 0u;						/*当前结构体初始化进程，分步加载时使用*/
static UINT16   g_wCurMainIndex = 0u;							/*当前主处理索引，分步加载时使用*/
static UINT8	g_TslConfigInitMode = 0x01u;					/*TSL配置数据表初始化模式*/
static UINT8	g_IsTslConfigAllocMemory = 0x01u;				/*如果调用文件访问接口上电初始化，分配内存*/
static UINT8	TslConfigInitFunStepCtrl(UINT16 wStep);			/*电子地图初始化分步控制函数声明*/

/*初始化任务登记表*/
static INIT_TASK_TAB_STRU g_struInitTaskTab[] = {
	{ DQU_QFUN_TRUE, TSL_CONFIG_DATA_ID,initTslConfig,5u, 1u, 0u }
};

/*初始化任务登记表项目数量*/
static const UINT16 MAX_INITTASK_COUNT = sizeof(g_struInitTaskTab) / sizeof(INIT_TASK_TAB_STRU);

/*********************************************
*函数功能：TSL配置数据表初始化
*参数说明：FSName文件指针
*返回值：0，失败；1，成功
*********************************************/
UINT8 dfsTslConfigInit(CHAR * FSName)
{
	UINT16	wIndex = 0u;
	UINT8   funcRtn = 0u;
	UINT8	rtnValue = 0u;
	UINT32	DataID = 0u;/*数据类型标识*/
	UINT8	*pDataAddr = NULL;
	UINT32	DataSize = 0u;
	UINT16	rowCount = 0u;

	/*初始化错误信息记录*/
	CommonMemSet(initTslConfigErrNo, INIT_TSLCONFIG_ERR_NUM, 0u, INIT_TSLCONFIG_ERR_NUM);

	if ((NULL != g_dsuTslConfigStru) && (NULL != FSName))
	{
		/*初始化一次就行了*/
		if (1u == dfsTslConfigInitFlag)
		{
			rtnValue = 1u;
			return rtnValue;
		}

		g_TslConfigInitMode = 0x01u;/*上电加载模式*/

		g_IsTslConfigAllocMemory = 0x01u;/*默认本接口分配内存*/

										/*动态申请索引空间*/
		g_dsuTslConfigStru->dsuTslConfigLenStru = (DSU_TSLCONFIG_LEN_STRU*)MyNew(sizeof(DSU_TSLCONFIG_LEN_STRU));

		/*动态申请空间失败*/
		if (NULL != g_dsuTslConfigStru->dsuTslConfigLenStru)
		{
			/*初始化为0xFF*/
			CommonMemSet(g_dsuTslConfigStru->dsuTslConfigLenStru, sizeof(DSU_TSLCONFIG_LEN_STRU), 0x00u, sizeof(DSU_TSLCONFIG_LEN_STRU));
		}
		else
		{
			rtnValue = 0u;
		}

		/*动态申请索引空间*/
		g_dsuTslConfigStru->dsuStaticTslConfigStru = (DSU_STATIC_TSLCONFIG_STRU*)MyNew(sizeof(DSU_STATIC_TSLCONFIG_STRU));

		/*动态申请空间失败*/
		if (NULL != g_dsuTslConfigStru->dsuStaticTslConfigStru)
		{
			/*初始化为0xFF*/
			CommonMemSet(g_dsuTslConfigStru->dsuStaticTslConfigStru, sizeof(DSU_STATIC_TSLCONFIG_STRU), 0x00u, sizeof(DSU_STATIC_TSLCONFIG_STRU));
		}
		else
		{
			rtnValue = 0u;

		}

		/*动态申请索引空间*/
		g_dsuTslConfigStru->dsuTslConfigIndexStru = (DSU_TSLCONFIG_INDEX_STRU*)MyNew(sizeof(DSU_TSLCONFIG_INDEX_STRU));

		/*动态申请空间失败*/
		if (NULL != g_dsuTslConfigStru->dsuTslConfigIndexStru)
		{
			/*初始化为0xFF*/
			CommonMemSet(g_dsuTslConfigStru->dsuTslConfigIndexStru, sizeof(DSU_TSLCONFIG_INDEX_STRU), 0x00u, sizeof(DSU_TSLCONFIG_INDEX_STRU));
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
				funcRtn = dquGetConfigDataNew(FSName, TSL_CONFIG_DATA_TYPE, DataID, &pDataAddr, &DataSize, &rowCount);
				if (1u != funcRtn)
				{
					/*函数调用失败*/
					initTslConfigErrNo[DataID] |= DEBUG_GET_DATA_FAIL;
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
					/*继续执行*/
				}
			}

			wIndex++;
		} while (wIndex < MAX_INITTASK_COUNT);

		dquSetCurTslConfigQueryDataSource(g_dsuTslConfigStru);
		/*函数执行标志设为1*/
		dfsTslConfigInitFlag = 1u;
		rtnValue = 1u;
	}
	else
	{
		/*函数执行标志设为0*/
		dfsTslConfigInitFlag = 0u;
		rtnValue = 0u;
	}
	return rtnValue;
}

/****************************************
*函数功能：初始化TSL配置数据表扩展接口
*参数说明： pDataBuf,传入数据块，
*			dsuTslConfigStru,外部实体TSL配置数据表存储结构体指针
*			mode,处理模式，1上电一次加载，2运行时多次处理
*			timeSlice，执行的时间片（单位：毫秒）
*返回值：  0失败，1成功，2分步处理未完成
*****************************************/
UINT8 dfsTslConfigInitExp(UINT8 * pDataBuf, DSU_TSLCONFIG_STRU *dsuTslConfigStru, UINT8 mode, UINT32 timeSlice)
{
	UINT8  bExeState = DQU_QFUN_FASLE;
	UINT8  rtnValue = 0u;
	UINT8  funcRtn = 0u;
	UINT16 wFreqTimes = 0u;
	UINT32 DataID = 0u;
	UINT32 dwOnceTime = 0u;
	UINT32 dwUsedTime = 0u;
	UINT32 dwCurTimePoint = 0u;
	UINT32 dwBeginTimePoint = 0u;
	UINT32 dwLastTimePoint = 0u;

	if (0u == g_wCurMainIndex)/*主索引变量进入执行态的时候，就不能改变处理模式*/
	{
		g_TslConfigInitMode = mode;
		g_IsTslConfigAllocMemory = 0x00u;/*默认本接口不分配内存*/
		/*根据VOBC需求在此处接口关联指针*/
		if ((NULL != dsuTslConfigStru) && (NULL != pDataBuf))
		{
			g_dsuTslConfigStru = dsuTslConfigStru;
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
			if (funcRtn != 1)
			{
				/*解析FS头数据失败*/
				rtnValue = 0;
			}
			else /*处理成功*/
			{
				g_wCurMainIndex = 1;
				rtnValue = 2;
			}

			break;
		case 1u:
			/*获取配置数据*/
			DataID = g_struInitTaskTab[g_wCurInitStruProg].DataTypeId;
			funcRtn = dquGetConfigDataExpNew(pDataBuf, &g_DFsDH, TSL_CONFIG_DATA_TYPE, DataID, &g_pDataAddr, &g_DataSize, &g_RowCount);
			if (1u != funcRtn)
			{
				/*函数调用失败*/
				initTslConfigErrNo[DataID] |= DEBUG_GET_DATA_FAIL;
				rtnValue = 0u;
			}
			else /*处理成功*/
			{
				g_wCurMainIndex = 2u;
				rtnValue = 2u;
			}

			break;
		case 2u:
			funcRtn = g_struInitTaskTab[g_wCurInitStruProg].pExeFun(g_pDataAddr, g_DataSize, g_RowCount);
			if (1u == funcRtn)/*完成本次完整的处理*/
			{
				do
				{
					g_wCurInitStruProg++;/*初始化结构体进行下一个阶段*/
					/*越过无效处理函数*/
				} while ((DQU_QFUN_FASLE == g_struInitTaskTab[g_wCurInitStruProg].bIsEffective) && (g_wCurInitStruProg < MAX_INITTASK_COUNT));
				/*还有的初始化处理未完成*/
				if (g_wCurInitStruProg < MAX_INITTASK_COUNT)
				{
					g_wCurMainIndex = 1u;
				}
				else /*if (g_wCurInitStruProg >=MAX_INITTASK_COUNT) 这个条件状态下，完成了所有结构体初始化处理*/
				{
					g_wCurMainIndex = 3u;
				}
				rtnValue = 2u;
			}
			else if (2u == funcRtn)/*分步处理未完成*/
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
					wFreqTimes = (UINT16)((timeSlice - dwUsedTime) / dwOnceTime);
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

/*********************************************************************
* 函数功能：	TSL配置数据表初始化完毕，
执行TSL配置数据表内部公共变量的清理工作
* 参数说明：	FSName文件指针
* 返回值：		0，失败；1，成功
**********************************************************************/
UINT8 dfsTslConfigInitFinally(void)
{
	UINT16 wIndex = 0u;
	UINT8 bRetVal = 0u;

	g_wCurInitStruProg = 0u;		/*当前结构体初始化进程，分步加载时使用*/
	g_wCurMainIndex = 0u;			/*当前主处理索引，分步加载时使用*/
	g_TslConfigInitMode = 0x01u;	/*初始化模式*/

	/*将数据初始化状态归零*/
	for (wIndex = 0u; wIndex < MAX_INITTASK_COUNT; wIndex++)
	{
		g_struInitTaskTab[wIndex].wCurProcStage = 0x01u;
		g_struInitTaskTab[wIndex].wCurInitPos = 0x0u;
	}

	/*初始化错误信息记录*/
	CommonMemSet(initTslConfigErrNo, INIT_TSLCONFIG_ERR_NUM, 0u, INIT_TSLCONFIG_ERR_NUM);

	bRetVal = 1u;
	return bRetVal;
}

/*********************************************************************
* 函数功能：	TSL配置初始化分步控制函数
* 参数说明：	UINT16 wStep 运行阶段
* 返回值：		0，失败；1，成功
**********************************************************************/
static UINT8  TslConfigInitFunStepCtrl(UINT16 wStep)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;

	if (0x01u == g_TslConfigInitMode)	/*上电加载处理*/
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
			/*不处理*/
		}
	}

	return bIsEffective;
}

/*********************************************************************
* 函数功能：	TSL配置数据表初始化循环步长分步切割处理
* 参数说明：	UINT16 wStep 运行阶段
* 返回值：		0，失败；1，成功
**********************************************************************/
static UINT8 TslConfigInitLoopStepLenCtrl(UINT16 wIndex)
{
	UINT8 bIsEffective = DQU_QFUN_FASLE;

	if (0x01u == g_TslConfigInitMode)	/*上电加载处理*/
	{
		bIsEffective = DQU_QFUN_FASLE;
	}
	else /*分步加载*/
	{
		if (g_struInitTaskTab[g_wCurInitStruProg].wStepLen == wIndex - g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos + 1u)
		{
			g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos = wIndex + 1u;
			bIsEffective = DQU_QFUN_TRUE;
		}
	}

	return bIsEffective;
}


/**********************************************************************
* 函数功能：	初始化TSL配置数据表
* 输入参数：	pDataAddr:表数据首地址
* 				dataSize:数据字节长度
* 				rowCount:表数据行数
* 输出参数：	无
* 返回值:		2处理中,1成功,0失败
***********************************************************************/
UINT8 initTslConfig(UINT8 *pDataAddr, UINT32 dataSize, UINT16 rowCount)
{
	static UINT32 rowDataLen = 0u;
	UINT8 rtnValue = 2u;
	UINT8 *pData = NULL;
	UINT16 j = 0u;
	UINT16 wStruDataLen = 0u;
	UINT16 wDataIndexLen = 0u;
	DSU_TSL_CONFIG_STRU* pCurrentStru = NULL; /*结构体临时指针变量*/
	DSU_TSL_CONFIG_STRU stru = { 0 };   		/*结构体临时指针变量*/

	/*TSL配置数据表数据*/
	DSU_TSLCONFIG_INDEX_STRU *dsuTslConfigIndexStru = NULL;
	DSU_STATIC_TSLCONFIG_STRU *dsuStaticTslConfigStru = NULL;
	DSU_TSLCONFIG_LEN_STRU *dsuTslConfigLenStru = NULL;

	if ((NULL != g_dsuTslConfigStru) && (NULL != pDataAddr))
	{
		dsuTslConfigIndexStru = g_dsuTslConfigStru->dsuTslConfigIndexStru;
		dsuStaticTslConfigStru = g_dsuTslConfigStru->dsuStaticTslConfigStru;
		dsuTslConfigLenStru = g_dsuTslConfigStru->dsuTslConfigLenStru;
	}
	else
	{
		/*数据指针为空*/
		rtnValue = 0u;
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == TslConfigInitFunStepCtrl(0x01u)))
	{
		dsuTslConfigLenStru->wTslConfigNumLen = rowCount;

		if (0u == rowCount)
		{
			rtnValue = 1u;
		}
		else
		{
			rowDataLen = dataSize / rowCount;

			/*索引数组长度计算*/
			PoseStru_init(pDataAddr + dataSize - rowDataLen, &stru);

			/*索引个数为最大ID + 1*/
			dsuTslConfigIndexStru->TSLCONFIGINDEXNUM = stru.index;
		}
	}


	if ((2u == rtnValue) && (DQU_QFUN_TRUE == TslConfigInitFunStepCtrl(0x02u)))
	{
		if (0x01u == g_IsTslConfigAllocMemory)
		{
			/*动态申请索引空间*/
			dsuTslConfigIndexStru->dsuTslConfigIndex = (UINT16*)MyNew(sizeof(UINT16) * dsuTslConfigIndexStru->TSLCONFIGINDEXNUM + 2u);

			/*动态申请空间失败*/
			if (NULL == dsuTslConfigIndexStru->dsuTslConfigIndex)
			{
				rtnValue = 0u;
			}
			else
			{
				/*初始化为0xFF*/
				CommonMemSet(dsuTslConfigIndexStru->dsuTslConfigIndex, dsuTslConfigIndexStru->TSLCONFIGINDEXNUM * 2u + 2u, 0xFFu, dsuTslConfigIndexStru->TSLCONFIGINDEXNUM * 2u + 2u);
			}

			if (2u == rtnValue)
			{
				/*动态申请空间*/
				dsuStaticTslConfigStru->pTslConfigStru = (DSU_TSL_CONFIG_STRU *)MyNew(sizeof(DSU_TSL_CONFIG_STRU) * dsuTslConfigLenStru->wTslConfigNumLen);

				/*动态申请空间失败*/
				if (NULL == dsuStaticTslConfigStru->pTslConfigStru)
				{
					rtnValue = 0u;
				}
				else
				{
					/*内存先设为无效*/
					CommonMemSet(dsuStaticTslConfigStru->pTslConfigStru, sizeof(DSU_TSL_CONFIG_STRU) * dsuTslConfigLenStru->wTslConfigNumLen, 0xFFu, sizeof(DSU_TSL_CONFIG_STRU) * dsuTslConfigLenStru->wTslConfigNumLen);
				}
			}
		}
		else/*内存空间是由外部产品来分配的*/
		{
			/*检测对应数据项数组的个数是否超出内存分配范围*/
			dquGetTslConfigDataStruArrayLen(TSL_CONFIG_DATA_ID, &wStruDataLen);

			if (((0u != wStruDataLen) && (dsuTslConfigLenStru->wTslConfigNumLen >= wStruDataLen)) || (0u == wStruDataLen))
			{
				rtnValue = 0u;
			}
			else
			{
				/*检测对应索引数组的个数是否超出内存分配范围*/
				dquGetTslConfigDataIndexArrayLen(TSL_CONFIG_DATA_ID, &wDataIndexLen);
				if (((0u != wDataIndexLen) && (dsuTslConfigIndexStru->TSLCONFIGINDEXNUM >= wDataIndexLen)) || (0u == wDataIndexLen))
				{
					rtnValue = 0u;
				}
			}
		}
	}

	if ((2u == rtnValue) && (DQU_QFUN_TRUE == TslConfigInitFunStepCtrl(0x03u)))
	{
		/*解析数据到结构体数组 */
		for (j = g_struInitTaskTab[g_wCurInitStruProg].wCurInitPos; j < dsuTslConfigLenStru->wTslConfigNumLen; j++)
		{
			pCurrentStru = dsuStaticTslConfigStru->pTslConfigStru + j;

			pData = pDataAddr + rowDataLen*j;
			/*填充数据结构体*/
			PoseStru_init(pData, pCurrentStru);

			/*完成对应填充索引*/
			dsuTslConfigIndexStru->dsuTslConfigIndex[pCurrentStru->index] = j;
			/*循环分步切割处理*/
			if (DQU_QFUN_TRUE == TslConfigInitLoopStepLenCtrl(j))
			{
				break;
			}
		}

		if (j == dsuTslConfigLenStru->wTslConfigNumLen)
		{
			rtnValue = 1u;
		}
	}

	return rtnValue;
}

/*********************************************************************
* 函数功能：	TSL配置数据表结构体初始化函数
* 参数说明：	[in] pDataAdd 数据地址（二进制文件）
*				[out] pTslConfigStru 被初始化的结构体指针
* 返回值：		无
* 备注：		结构内容已经完善
**********************************************************************/
void PoseStru_init(UINT8 * pDataAdd, DSU_TSL_CONFIG_STRU* pTslConfigStru)
{
	UINT8 *pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pTslConfigStru))
	{
		pData = pDataAdd;

		/*TSL索引编号*/
		pTslConfigStru->index = ShortFromChar(pData);
		pData += 2u;

		/*TSL_ID*/
		pTslConfigStru->tslId = ShortFromChar(pData);
		pData += 2u;

		/*所处link编号*/
		pTslConfigStru->linkId = ShortFromChar(pData);
		pData += 2u;

		/*所处link偏移量*/
		pTslConfigStru->off = LongFromChar(pData);
		pData += 4u;

		/*TSL-IVOC共用数据校验信息*/
		pTslConfigStru->tslIvocValidation = LongFromChar(pData);
		pData += 4u;

		/*TSL关联的道岔ID*/
		pTslConfigStru->tslSwitchID = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/******************************************************************
函数功能：		获取故障码
输入参数：		无
输出参数：		errRecord故障码数组，errRecordLen数组长度
返回值：		1成功
*******************************************************************/
UINT8 dquGetTslConfigErrNo(UINT8* errRecord, UINT32 * errRecordLen)
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

		for (i = 0u; i < INIT_TSLCONFIG_ERR_NUM; i++)
		{
			errRecord[Ofst] = initTslConfigErrNo[i];
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
