/************************************************************************
* 文件名      ：    dquTslConfigQuery.c
* 版权说明    ：
* 版本号      ：    1.0
* 创建时间    ：    2009.09.27
* 作者        ：
* 功能描述    ：    dsu查询函数文件.
* 使用注意    ：
* 修改记录    ：
************************************************************************/
#include "dquTslConfigQuery.h"
#include "CommonMemory.h"

static UINT8 qds_IsInitTslConfigStruPtr = DQU_TSLCONFIG_INIT_FALSE;
static DSU_TSLCONFIG_LEN_STRU* qds_dsuTslConfigLenStru = NULL;            /*全局变量，记录读取的DSU数据中的各类型数据的数量*/
static DSU_STATIC_TSLCONFIG_STRU* qds_dsuStaticTslConfigStru = NULL;      /*存放数据库中每种结构的头指针*/
static DSU_TSLCONFIG_INDEX_STRU* qds_dsuTslConfigIndexStru = NULL;        /*各设备结构体索引*/

/**********************************************************************
* 函数功能：	设置当前数据源
* 输入参数：	DSU_TSLCONFIG_STRU* pDsuTslConfigStru
* 输出参数：	无
* 返回值:		1成功,0失败
***********************************************************************/
UINT8 dquSetCurTslConfigQueryDataSource(DSU_TSLCONFIG_STRU* pDsuTslConfigStru)
{
	UINT8 chReturnValue = 0u;             /*用于函数返回值*/
	if (NULL != pDsuTslConfigStru)
	{
		qds_dsuTslConfigIndexStru = pDsuTslConfigStru->dsuTslConfigIndexStru;        /*各设备结构体索引*/
		qds_dsuTslConfigLenStru = pDsuTslConfigStru->dsuTslConfigLenStru;            /*全局变量，记录读取的DSU数据中的各类型数据的数量*/
		qds_dsuStaticTslConfigStru = pDsuTslConfigStru->dsuStaticTslConfigStru;        /*存放数据库中每种结构的头指针*/

		if ((NULL != qds_dsuTslConfigIndexStru) &&
			(NULL != qds_dsuTslConfigLenStru) &&
			(NULL != qds_dsuStaticTslConfigStru))
		{
			chReturnValue = 1u;
			qds_IsInitTslConfigStruPtr = DQU_TSLCONFIG_INIT_TRUE;
		}
		else
		{
			chReturnValue = 0u;
			qds_IsInitTslConfigStruPtr = DQU_TSLCONFIG_INIT_FALSE;
		}

	}
	else
	{
		chReturnValue = 0;
	}

	return chReturnValue;
}

/**********************************************************************
* 函数功能：	查询单个TSL信息
* 输入参数：	UINT16 id
* 输出参数：	DSU_TSL_CONFIG_STRU* pTSLConfig
* 返回值:		1成功,0失败
*修改记录： 1.YF0069-2195 白盒问题  by xly 20221108
***********************************************************************/
UINT8 dquTslInfoById(IN UINT16 id, OUT DSU_TSL_CONFIG_STRU* pTSLConfig)
{
	UINT8 ret = 0u;
	int i = 0;
	DSU_TSL_CONFIG_STRU* tempTslConfig = NULL;
	DSU_TSLCONFIG_LEN_STRU* pTempTslConfigLenStru = NULL;
	DSU_STATIC_TSLCONFIG_STRU* pTempStaticTslConfigStru = NULL;

	if ((NULL != pTSLConfig) && (0x00u != id))
	{
		pTempStaticTslConfigStru = g_dsuTslConfigStru->dsuStaticTslConfigStru;
		if (NULL == pTempStaticTslConfigStru)
		{
			ret = 0x00u;
		}
		else
		{
			tempTslConfig = pTempStaticTslConfigStru->pTslConfigStru;
			if (NULL == tempTslConfig)
			{
				ret = 0x00u;
			}
			else
			{
				pTempTslConfigLenStru = g_dsuTslConfigStru->dsuTslConfigLenStru;
				if (NULL == pTempTslConfigLenStru)
				{
					ret = 0x00u;
				}
				else
				{
					for (i = 0u; i < pTempTslConfigLenStru->wTslConfigNumLen; i++)
					{
						if (tempTslConfig->tslId == id)
						{
							CommonMemCpy(pTSLConfig, sizeof(DSU_TSL_CONFIG_STRU), tempTslConfig, sizeof(DSU_TSL_CONFIG_STRU));
							ret = 0x01u;
							break;
						}
						tempTslConfig++;
					}
				}
			}
		}
	}
	else
	{
		ret = 0x00u;
	}

	return ret;
}
