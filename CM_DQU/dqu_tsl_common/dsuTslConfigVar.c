/************************************************************************
*
* 文件名   ：  dsuTslConfigVar.c
* 版权说明 ：  交控科技股份有限公司
* 版本号   ：  1.0
* 创建时间 ：  2016.03.20
* 作者     ：  研发中心
* 功能描述 ：   dsu变量处理过程
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/
#define TSLCONFIG_VAR_DEFINE

#include "dsuTslConfigVar.h"
#include "dfsDataRead.h"

DSU_TSLCONFIG_STRU	*g_dsuTslConfigStru = NULL;

static UINT16 g_dsuTslConfigDataArrayLen[DFS_DATA_INDEX_NUM] = { 0 };/*用于存储TSL配置数据表中各项数据项数组的个数*/
static UINT16 g_dsuTslConfigIndexArrayLen[DFS_DATA_INDEX_NUM] = { 0 };/*用于存储TSL配置数据表中各项索引数组的个数*/

/**********************************************************************
* 函数功能：	设置TSL配置数据表结构体指针
* 输入参数：	DSU_TSLCONFIG_STRU* pDsuTslConfigStru
* 输出参数：	无
* 返回值:		1成功,0失败
***********************************************************************/
UINT8 dquSetInitTslConfigRef(DSU_TSLCONFIG_STRU *pDsuTslConfigStru)
{
	UINT8 bRetVal = 0u;
	if (NULL != pDsuTslConfigStru)
	{
		g_dsuTslConfigStru = pDsuTslConfigStru;
		bRetVal = 1u;
	}
	else
	{
		bRetVal = 0u;
	}
	return bRetVal;
}

/**********************************************************************
* 函数功能：	设置TSL配置数据表对应数据项数组的个数
* 输入参数：	UINT16 wDataID, UINT16 wStruDataLen
* 输出参数：	无
* 返回值:		1成功,0失败
***********************************************************************/
UINT8 dquSetTslConfigDataStruArrayLen(UINT16 wDataID, UINT16 wStruDataLen)
{
	UINT8 bRetVal = 0u;
	if (wDataID < DFS_DATA_INDEX_NUM)
	{
		g_dsuTslConfigDataArrayLen[wDataID] = wStruDataLen;
		bRetVal = 1u;
	}
	else
	{
		bRetVal = 0u;
	}

	return bRetVal;
}

/**********************************************************************
* 函数功能：	设置TSL配置数据表中对应索引数组的个数
* 输入参数：	UINT16 wDataID, UINT16 wDataIndexLen
* 输出参数：	无
* 返回值:		1成功,0失败
***********************************************************************/
UINT8 dquSetTslConfigDataIndexArrayLen(UINT16 wDataID, UINT16 wDataIndexLen)
{
	UINT8 bRetVal = 0u;
	if (wDataID < DFS_DATA_INDEX_NUM)
	{
		g_dsuTslConfigIndexArrayLen[wDataID] = wDataIndexLen;
		bRetVal = 1u;
	}
	else
	{
		bRetVal = 0u;
	}

	return bRetVal;
}

/**********************************************************************
* 函数功能：	获取TSL配置数据表对应数据项数组的个数
* 输入参数：	UINT16 wDataID, UINT16 *pStruDataLe
* 输出参数：	无
* 返回值:		1成功,0失败
***********************************************************************/
UINT8 dquGetTslConfigDataStruArrayLen(UINT16 wDataID, UINT16 *pStruDataLen)
{
	UINT8 bRetVal = 0u;
	if ((wDataID < DFS_DATA_INDEX_NUM) && (NULL != pStruDataLen))
	{
		*pStruDataLen = g_dsuTslConfigDataArrayLen[wDataID];
		bRetVal = 1u;
	}
	else
	{
		bRetVal = 0u;
	}

	return bRetVal;
}

/**********************************************************************
* 函数功能：	设置TSL配置数据表中对应索引数组的个数
* 输入参数：	UINT16 wDataID
* 输出参数：	UINT16 *pDataIndexLen
* 返回值:		1成功,0失败
***********************************************************************/
UINT8 dquGetTslConfigDataIndexArrayLen(UINT16 wDataID, UINT16 *pDataIndexLen)
{
	UINT8 bRetVal = 0u;
	if ((wDataID < DFS_DATA_INDEX_NUM) && (NULL != pDataIndexLen))
	{
		*pDataIndexLen = g_dsuTslConfigIndexArrayLen[wDataID];
		bRetVal = 1u;
	}
	else
	{
		bRetVal = 0u;
	}

	return bRetVal;
}
