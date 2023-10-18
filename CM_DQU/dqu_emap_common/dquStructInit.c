/************************************************************************
*
* 文件名   ：  dquStructInit.c
* 版权说明 ：  北京交控科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2009.11.21
* 作者     ：  软件部
* 功能描述 ：  数据查询函数结构处理过程
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/

#include <stdio.h>
#include "dquStructInit.h"
#include "dquCbtcType.h"


static void ROUTE_STRU_init_HLT(UINT8 * pDataAdd, DSU_ROUTE_STRU * pRouteStru);
static void POINT_STRU_init_DFT(UINT8 *pDataAdd, DSU_POINT_STRU *pPointStru);
static void POINT_STRU_init_CC(UINT8 *pDataAdd, DSU_POINT_STRU *pPointStru);
#if 0
/**************************************
函数功能：link结构体初始化函数
参数说明：[in] pData 数据地址（二进制文件）
[out] pLinkStrc 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
***************************/
static void LINK_STRU_init_DFT(UINT8 * pDataAdd, DSU_LINK_STRU* pLinkStrc)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pLinkStrc))
	{
		pData = pDataAdd;

		/*link编号*/
		pLinkStrc->wId = ShortFromChar(pData);
		pData += 2u;

		/*link长度（cm）*/
		pLinkStrc->dwLength = LongFromChar(pData);
		pData += 4u;

		/*Link所处线路的上下行信息*/   /*新增---严广学*/
		pLinkStrc->wLinkLine = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点类型*/
		pLinkStrc->wOrgnPointType = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点编号*/
		pLinkStrc->wOrgnPointId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻正线link编号*/
		pLinkStrc->wOrgnJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻侧线link编号*/
		pLinkStrc->wOrgnJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点类型*/
		pLinkStrc->wTmnlPointType = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点编号*/
		pLinkStrc->wTmnlPointId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻正线link编号*/
		pLinkStrc->wTmnlJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻侧线link编号*/
		pLinkStrc->wTmnlJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*所属ZC区域编号*/
		pLinkStrc->wManageZC = ShortFromChar(pData);
		pData += 2u;

		/*所属ATS区域编号*/
		pLinkStrc->wManageATS = ShortFromChar(pData);
		pData += 2u;

		/*所属逻辑CI区域编号*/
		pLinkStrc->wManageLogicCI = ShortFromChar(pData);
		pData += 2u;

		/*所属物理CI区域编号*/   /*----确定长度为2个字节---*/
		pLinkStrc->wManagePhysicalCI = ShortFromChar(pData);
		pData += 2u;

		/*Link限速信息属性*/
		pLinkStrc->wLinkResSpeed = ShortFromChar(pData);
		pData += 2u;

		/*Link坡度信息属性*/
		pLinkStrc->wGrade = ShortFromChar(pData);
		pData += 2u;

		/*Link所属车站区域ID:2013年8月13日,由车载提出删除;
		pLinkStrc->wStationArea = ShortFromChar(pData);
		pData += 2u;*/

		pLinkStrc->wLogicDir = pLinkStrc->wLinkLine;

	}

}

static void LINK_STRU_init_HLT(UINT8 *pDataAdd, DSU_LINK_STRU *pLinkStrc)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pLinkStrc))
	{
		pData = pDataAdd;
		/*link编号*/
		pLinkStrc->wId = ShortFromChar(pData);
		pData += 2u;

		/*link长度（cm）*/
		pLinkStrc->dwLength = LongFromChar(pData);
		pData += 4u;

		/*Link所处线路的上下行信息*/   /*新增---严广学*/
		pLinkStrc->wLinkLine = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点类型*/
		pLinkStrc->wOrgnPointType = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点编号*/
		pLinkStrc->wOrgnPointId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻正线link编号*/
		pLinkStrc->wOrgnJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻侧线link编号*/
		pLinkStrc->wOrgnJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点类型*/
		pLinkStrc->wTmnlPointType = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点编号*/
		pLinkStrc->wTmnlPointId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻正线link编号*/
		pLinkStrc->wTmnlJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻侧线link编号*/
		pLinkStrc->wTmnlJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*所属ZC区域编号*/
		pLinkStrc->wManageZC = ShortFromChar(pData);
		pData += 2u;

		/*所属ATS区域编号*/
		pLinkStrc->wManageStation = ShortFromChar(pData);
		pData += 2u;

		/*所属逻辑CI区域编号*/
		pLinkStrc->wManageLogicCI = ShortFromChar(pData);
		pData += 2u;

		/*所属物理CI区域编号*/   /*----确定长度为2个字节---*/
		pLinkStrc->wManagePhysicalCI = ShortFromChar(pData);
		pData += 2u;

		/*Link限速信息属性*/
		pLinkStrc->wLinkResSpeed = ShortFromChar(pData);
		pData += 2u;

		/*Link坡度信息属性*/
		pLinkStrc->wGrade = ShortFromChar(pData);
		pData += 2u;

		/*Link所属车站区域ID:2013年8月13日,由车载提出删除;
		pLinkStrc->wStationArea = ShortFromChar(pData);
		pData += 2u;*/

		pLinkStrc->wLogicDir = pLinkStrc->wLinkLine;


		pLinkStrc->wManageATS = ShortFromChar(pData);
		pData += 2u;

		pLinkStrc->wLinkLogicDirUpAndDown = *pData;       /*互联互通需求新增，系统定义逻辑方向与上下行方向的关系 add by qxt 20160811*/
		pData++;

		pLinkStrc->wLogicDirChanged = *pData;            /*逻辑方向变化点，解决灯泡线问题， add by qxt 20170816*/
		pData++;

		pLinkStrc->wTunAttribute = *pData;				/*Link隧道属性，依据互联互通数据结构V12.0.0添加，add by lmy 20180312*/
		pData++;

	}
	else
	{
		/* do nothing */
	}
}

static void LINK_STRU_init_DG(UINT8 * pDataAdd, DSU_LINK_STRU* pLinkStrc)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pLinkStrc))
	{
		pData = pDataAdd;

		/*link编号*/
		pLinkStrc->wId = ShortFromChar(pData);
		pData += 2u;

		/*link长度（cm）*/
		pLinkStrc->dwLength = LongFromChar(pData);
		pData += 4u;

		/*Link所处线路的上下行信息*/   /*新增---严广学*/
		pLinkStrc->wLinkLine = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点类型*/
		pLinkStrc->wOrgnPointType = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点编号*/
		pLinkStrc->wOrgnPointId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻正线link编号*/
		pLinkStrc->wOrgnJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻侧线link编号*/
		pLinkStrc->wOrgnJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点类型*/
		pLinkStrc->wTmnlPointType = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点编号*/
		pLinkStrc->wTmnlPointId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻正线link编号*/
		pLinkStrc->wTmnlJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻侧线link编号*/
		pLinkStrc->wTmnlJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*所属ZC区域编号*/
		pLinkStrc->wManageZC = ShortFromChar(pData);
		pData += 2u;

		/*所属ATS区域编号*/
		pLinkStrc->wManageStation = ShortFromChar(pData);
		pData += 2u;

		/*所属逻辑CI区域编号*/
		pLinkStrc->wManageLogicCI = ShortFromChar(pData);
		pData += 2u;

		/*所属物理CI区域编号*/   /*----确定长度为2个字节---*/
		pLinkStrc->wManagePhysicalCI = ShortFromChar(pData);
		pData += 2u;

		/*Link限速信息属性*/
		pLinkStrc->wLinkResSpeed = ShortFromChar(pData);
		pData += 2u;

		/*Link坡度信息属性*/
		pLinkStrc->wGrade = ShortFromChar(pData);
		pData += 2u;

		/*Link所属车站区域ID:2013年8月13日,由车载提出删除;
		pLinkStrc->wStationArea = ShortFromChar(pData);
		pData += 2u;*/

		pLinkStrc->wLogicDir = pLinkStrc->wLinkLine;


		pLinkStrc->wManageATS = ShortFromChar(pData);
		pData += 2u;

		pLinkStrc->wLinkLogicDirUpAndDown = *pData;       /*互联互通需求新增，系统定义逻辑方向与上下行方向的关系 add by qxt 20160811*/
		pData++;
	}
}
#endif

/**************************************
函数功能：link结构体初始化函数-车车 add by sds 2019-6-13
参数说明：[in] pData 数据地址（二进制文件）
[out] pLinkStrc 被初始化的结构体指针
返回值：无
备注：结构内容已经完善
数据解耦修改部分结构体数据 by lyk 20220307
***************************/
static void LINK_STRU_init_CC(UINT8 * pDataAdd, DSU_LINK_STRU* pLinkStrc)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pLinkStrc))
	{
		pData = pDataAdd;

		/*link编号*/
		pLinkStrc->wId = ShortFromChar(pData);
		pData += 2u;

		/*link长度（cm）*/
		pLinkStrc->dwLength = LongFromChar(pData);
		pData += 4u;

		/*Link所处线路的上下行信息*/
		pLinkStrc->wLinkLine = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点类型*/
		pLinkStrc->wOrgnPointType = ShortFromChar(pData);
		pData += 2u;

		/*link起点端点编号*/
		pLinkStrc->wOrgnPointId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻正线link编号*/
		pLinkStrc->wOrgnJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link起点相连接的相邻侧线link编号*/
		pLinkStrc->wOrgnJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点类型*/
		pLinkStrc->wTmnlPointType = ShortFromChar(pData);
		pData += 2u;

		/*link终点端点编号*/
		pLinkStrc->wTmnlPointId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻正线link编号*/
		pLinkStrc->wTmnlJointMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*link终点相连接的相邻侧线link编号*/
		pLinkStrc->wTmnlJointSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*所属物理TIOC区域编号*/
		pLinkStrc->wManageTIOC = ShortFromChar(pData);
		pData += 2u;

		/*Link限速信息属性*/
		pLinkStrc->wLinkResSpeed = ShortFromChar(pData);
		pData += 2u;

		/*Link坡度信息属性*/
		pLinkStrc->wGrade = ShortFromChar(pData);
		pData += 2u;

		/*所属ITS区域编号*/
		pLinkStrc->wManageITS = ShortFromChar(pData);
		pData += 2u;

		/*系统定义逻辑方向与上下行方向的关系*/
		pLinkStrc->wLinkLogicDirUpAndDown = *pData;
		pData += 1u;

		/*link 逻辑方向变化点*/
		pLinkStrc->wLogicDirChanged = *pData;
		pData += 1u;

		/*Link隧道属性*/
		pLinkStrc->wTunAttribute = *pData;
		pData += 1u;

		/*link 邻线属性*/
		pLinkStrc->wLinkNeighborType = *pData;
		pData += 1u;
	}
}

/*调用接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
/*删除互联互通和车车以外的接口 by lyk 20220307*/
void LINK_STRU_init(UINT8 *pDataAdd, DSU_LINK_STRU *pLinkStrc)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pLinkStrc))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
#if 0
			LINK_STRU_init_HLT(pDataAdd, pLinkStrc);
#endif
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			LINK_STRU_init_CC(pDataAdd, pLinkStrc);
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：道岔数据结构体初始化函数--车车
参数说明：[in] pData 数据地址（二进制文件）
[out]pPointStru 被初始化的结构体指针
返回值：无
修改记录：数据解耦修改部分结构体数据 by lyk 20220307
***************************/
static void POINT_STRU_init_CC(UINT8 *pDataAdd, DSU_POINT_STRU *pPointStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pPointStru))
	{
		pData = pDataAdd;

		/*道岔编号*/
		pPointStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*所处正线link编号*/
		pPointStru->wMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*所处正线link偏移量(cm)*/
		pPointStru->dwMainLkOfst = LongFromChar(pData);
		pData += 4u;

		/*所处侧线link编号*/
		pPointStru->wSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*所处侧线link偏移量(cm)*/
		pPointStru->dwSideLkOfst = LongFromChar(pData);
		pData += 4u;

		/*所处汇合link编号*/
		pPointStru->wConfluentLkId = ShortFromChar(pData);
		pData += 2u;

		/*所处汇合link偏移量(cm)*/
		pPointStru->dwConfluentLkOfst = LongFromChar(pData);
		pData += 4u;

		/*道岔反位静态限制速度(cm/s)*/
		pPointStru->wSideResSpeed = ShortFromChar(pData);
		pData += 2u;

		/*道岔后方防护长度*/
		pPointStru->wRearSwitchProtecDis = ShortFromChar(pData);
		pData += 2u;
	}
}

/**************************************
函数功能：道岔数据结构体初始化函数
参数说明：[in] pData 数据地址（二进制文件）
[out]pPointStru 被初始化的结构体指针
返回值：无
备注：结构需增加
UINT16  wManageZC;				   所属ZC区域编号
UINT16  wManagePhysicalCI;		   所属物理CI区域编号
UINT16  wManageLogicCI;			   所属逻辑CI区域编号
修改说明：根据合库数据结构V19.0.0需求修改，20180208 lmy
***************************/
static void POINT_STRU_init_DFT(UINT8 *pDataAdd, DSU_POINT_STRU *pPointStru)
{
#if 0
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pPointStru))
	{
		pData = pDataAdd;

		/*道岔名称*/
		for (i = 0u; i < 16u; i++)
		{
			pPointStru->szCaption[i] = *pData;
			pData++;
		}

		/*联动道岔编号*/
		pPointStru->DPointOtherId = ShortFromChar(pData);
		pData += 2u;

		/*道岔点公里标*/
		pPointStru->dwPointKilo = LongFromChar(pData);
		pData += 4u;

		/*所处正线link编号*/
		pPointStru->wMainLkId = ShortFromChar(pData);
		pData += 2u;

		/*所处正线link偏移量(cm)*/
		pPointStru->dwMainLkOfst = LongFromChar(pData);
		pData += 4u;

		/*所处侧线link编号*/
		pPointStru->wSideLkId = ShortFromChar(pData);
		pData += 2u;

		/*所处侧线link偏移量(cm)*/
		pPointStru->dwSideLkOfst = LongFromChar(pData);
		pData += 4u;

		/*所处汇合link编号*/
		pPointStru->wConfluentLkId = ShortFromChar(pData);
		pData += 2u;

		/*所处汇合link偏移量(cm)*/
		pPointStru->dwConfluentLkOfst = LongFromChar(pData);
		pData += 4u;

		/*道岔反位静态限制速度(cm/s)*/
		pPointStru->wSideResSpeed = ShortFromChar(pData);
		pData += 2u;
	}
#endif

}

/**************************************
函数功能：道岔数据结构体初始化函数
参数说明：[in] pData 数据地址（二进制文件）
[out]pPointStru 被初始化的结构体指针
返回值：无
修改说明：根据车车数据结构V1.0.0需求修改，2019-7-10 by sds
删除互联互通和车车以外的接口 by lyk 20220307
***************************/
void POINT_STRU_init(UINT8 *pDataAdd, DSU_POINT_STRU *pPointStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pPointStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
			POINT_STRU_init_DFT(pDataAdd, pPointStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			POINT_STRU_init_CC(pDataAdd, pPointStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
}

/**************************************
函数功能：信号机数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pSignalStru 被初始化的结构体指针
返回值：无
备注：需增加项目：
UINT16  wManageZC;					所属ZC区域编号
UINT16  wManagePhysicalCI;		所属物理CI区域编号
UINT16  wManageLogicCI;			所属逻辑CI区域编号
修改说明：删除互联互通和车车以外的接口 by lyk 20220307
***************************/
void SIGNAL_STRU_init(UINT8 * pDataAdd, DSU_SIGNAL_STRU * pSignalStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pSignalStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
			SIGNAL_STRU_init_DFT(pDataAdd, pSignalStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			SIGNAL_STRU_init_CC(pDataAdd, pSignalStru);
		}
		else
		{
			/*分支不可达*/
		}
	}
}

/**************************************
函数功能：DFT信号机数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pSignalStru 被初始化的结构体指针
返回值：无
备注：需增加项目：
UINT16  wManageZC;					所属ZC区域编号
UINT16  wManagePhysicalCI;		所属物理CI区域编号
UINT16  wManageLogicCI;			所属逻辑CI区域编号
***************************/
void SIGNAL_STRU_init_DFT(UINT8 * pDataAdd, DSU_SIGNAL_STRU * pSignalStru)
{
#if 0
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pSignalStru))
	{
		pData = pDataAdd;

		/*编号*/
		pSignalStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*信号机名称*/

		for (i = 0u; i < 12u; i++)
		{
			pSignalStru->szCaption[i] = *pData;
			pData += 1u;
		}

		/*信号机类型*/
		pSignalStru->wType = ShortFromChar(pData);
		pData += 2u;

		/*信号机属性*/
		pSignalStru->wAttribute = ShortFromChar(pData);
		pData += 2u;

		/*信号机所处link编号*/
		pSignalStru->wLkId = ShortFromChar(pData);
		pData += 2u;

		/*信号机所处link偏移量(cm)*/
		pSignalStru->dwLkOfst = LongFromChar(pData);
		pData += 4u;

		/*信号机防护点所处link编号*/
		pSignalStru->wProtectLinkId = ShortFromChar(pData);
		pData += 2u;

		/*信号机防护点所处link偏移量(cm)*/
		pSignalStru->dwProtectLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*信号机防护方向(16进制)*/
		pSignalStru->wProtectDir = ShortFromChar(pData);
		pData += 2u;

		/*开口信号机标志*/

		pSignalStru->wPassSignal = ShortFromChar(pData);
		pData += 2u;

		/*信号机判断闯信号功能标志*/
		pSignalStru->wEnableSignal = ShortFromChar(pData);
		pData += 2u;

		/*信号机亮灭功能标志*/
		pSignalStru->wShineSignal = ShortFromChar(pData);
		pData += 2u;

		/*信号机灯列*/
		pSignalStru->dwSignalDengLie = LongFromChar(pData);
		pData += 4u;

		/*信号机灯位封闭*/
		pSignalStru->sDengWeiFengBi = *pData;
		pData++;

		/*关联主信号机*/
		pSignalStru->wMainSignalId = *pData;
		pData++;
	}
#endif
}

/**************************************
函数功能：CC信号机数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pSignalStru 被初始化的结构体指针
返回值：无
备注：需增加项目：
UINT16  wManageZC;					所属ZC区域编号
UINT16  wManagePhysicalCI;		所属物理CI区域编号
UINT16  wManageLogicCI;			所属逻辑CI区域编号
数据解耦修改部分结构体数据 by lyk 20220307
***************************/
void SIGNAL_STRU_init_CC(UINT8 * pDataAdd, DSU_SIGNAL_STRU * pSignalStru)
{
	UINT8 * pData = NULL;

	if ((NULL == pDataAdd) || (NULL == pSignalStru))
	{
		/*do nothing*/
	}
	else
	{
		pData = pDataAdd;

		/*编号*/
		pSignalStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*信号机属性*/
		pSignalStru->wAttribute = ShortFromChar(pData);
		pData += 2u;

		/*信号机真实位置link编号*/
		pSignalStru->wPositionLinkId = ShortFromChar(pData);
		pData += 2u;

		/*信号机真实位置link偏移量(cm)*/
		pSignalStru->dwPositionLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*信号机防护方向(16进制)*/
		pSignalStru->wProtectDir = ShortFromChar(pData);
		pData += 2u;

		/*信号机防护link编号*/
		pSignalStru->wProtectLinkId = ShortFromChar(pData);
		pData += 2u;

		/*信号机防护link偏移量(cm)*/
		pSignalStru->dwProtectLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*信号机关联主信号机ID*/
		pSignalStru->wMainSignalId = ShortFromChar(pData);
		pData += 2u;
	}
}
#if 0
/**************************************
函数功能：应答器数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pBaliseStru 被初始化的结构体指针
返回值：无
备注：该结构体无需改动
数据解耦修改部分应答器个数等数据 by lyk 20220307
***************************/
/*默认版本相邻应答器个数为16个*/
static void BALISE_STRU_init_DFT(UINT8 * pDataAdd, DSU_BALISE_STRU * pBaliseStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pBaliseStru))
	{
		pData = pDataAdd;

		/*编号*/
		pBaliseStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*应答器ID*/
		pBaliseStru->dwBaliseId = LongFromChar(pData);
		pData += 4u;

		/*应答器所处link编号*/
		pBaliseStru->wLkId = ShortFromChar(pData);
		pData += 2u;

		/*应答器所处link偏移量(cm)*/
		pBaliseStru->dwLkOfst = LongFromChar(pData);
		pData += 4u;

		/*应答器类型*/
		pBaliseStru->wBaliseType = ShortFromChar(pData);
		pData += 2u;

		/*在所处link逻辑方向上起点的相邻的应答器数量*/
		pBaliseStru->wOrgnBaliseNum = ShortFromChar(pData);
		pData += 2u;


		for (i = 0u; i < 16u; i++)
		{
			/*在所处link逻辑方向上起点相邻的应答器的编号*/
			pBaliseStru->dwOrgnBaliseId[i] = LongFromChar(pData);
			pData += 4u;

			/*沿线路的起点方向，与本应答器相邻且对列车有效（可直接到达）的应答器间路径上，列车经过本应答器再经过该相邻应答器时的方向*/
			pBaliseStru->wPassOrgnBaliseDir[i] = ShortFromChar(pData);
			pData += 2u;

			/*在所处link逻辑方向上起点相邻的应答器的距离*/
			pBaliseStru->dwDistanceToOrgnBalise[i] = LongFromChar(pData);
			pData += 4u;

			/*起点相邻应答器关联道岔编号*/
			pBaliseStru->wOrgnRelatePointID[i] = ShortFromChar(pData);
			pData += 2u;

			/*起点相邻应答器关联道岔状态*/
			pBaliseStru->wOrgnRelatePointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*在所处link逻辑方向上终点的相邻的应答器数量*/
		pBaliseStru->wTmnlBaliseNum = ShortFromChar(pData);
		pData += 2u;

		/*在所处link逻辑方向上终点相邻的应答器的编号*/
		for (i = 0u; i < 16u; i++)
		{
			pBaliseStru->dwTmnlBaliseId[i] = LongFromChar(pData);
			pData += 4u;

			/*沿线路的终点方向，与本应答器相邻且对列车有效（可直接到达）的应答器间路径上，列车经过本应答器再经过该相邻应答器时的方向*/
			pBaliseStru->wPassTmnlBaliseDir[i] = ShortFromChar(pData);
			pData += 2u;

			/*在所处link逻辑方向上终点相邻的应答器的距离*/
			pBaliseStru->dwDistanceToTmnlBalise[i] = LongFromChar(pData);
			pData += 4u;

			/*终点相邻应答器关联道岔编号*/
			pBaliseStru->wTmnlRelatePointID[i] = ShortFromChar(pData);
			pData += 2u;

			/*终点相邻应答器关联道岔状态*/

			pBaliseStru->wTmnlRelatePointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*地图版本信息*/
		pBaliseStru->wEmapVersion = ShortFromChar(pData);
		pData += 2u;
	}
}

/*互联互通版本相邻应答器个数增加到是16个*/
static void BALISE_STRU_init_HLT(UINT8 * pDataAdd, DSU_BALISE_STRU * pBaliseStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pBaliseStru))
	{
		pData = pDataAdd;
		/*编号*/
		pBaliseStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*应答器ID*/
		pBaliseStru->dwBaliseId = LongFromChar(pData);
		pData += 4u;

		/*应答器名称*/
		for (i = 0u; i < 8; i++)
		{
			pBaliseStru->szCaption[i] = *pData;
			pData += 1u;
		}

		/*应答器所处link编号*/
		pBaliseStru->wLkId = ShortFromChar(pData);
		pData += 2u;

		/*应答器所处link偏移量(cm)*/
		pBaliseStru->dwLkOfst = LongFromChar(pData);
		pData += 4u;

		/*与应答器关联的信号机编号*/
		pBaliseStru->wRelateSignalID = ShortFromChar(pData);
		pData += 2u;

		/*应答器作用方向()*/
		pBaliseStru->wBaliseDir = ShortFromChar(pData);
		pData += 2u;

		/*应答器类型*/
		pBaliseStru->wBaliseType = ShortFromChar(pData);
		pData += 2u;

		/*在所处link逻辑方向上起点的相邻的应答器数量*/
		pBaliseStru->wOrgnBaliseNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 16u; i++)
		{
			/*在所处link逻辑方向上起点相邻的应答器的编号*/
			pBaliseStru->dwOrgnBaliseId[i] = LongFromChar(pData);
			pData += 4u;

			/*沿线路的起点方向，与本应答器相邻且对列车有效（可直接到达）的应答器间路径上，列车经过本应答器再经过该相邻应答器时的方向*/
			pBaliseStru->wPassOrgnBaliseDir[i] = ShortFromChar(pData);
			pData += 2u;

			/*在所处link逻辑方向上起点相邻的应答器的距离*/
			pBaliseStru->dwDistanceToOrgnBalise[i] = LongFromChar(pData);
			pData += 4u;

			/*起点相邻应答器关联道岔编号*/
			pBaliseStru->wOrgnRelatePointID[i] = ShortFromChar(pData);
			pData += 2u;

			/*起点相邻应答器关联道岔状态*/
			pBaliseStru->wOrgnRelatePointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*在所处link逻辑方向上终点的相邻的应答器数量*/
		pBaliseStru->wTmnlBaliseNum = ShortFromChar(pData);
		pData += 2u;

		/*在所处link逻辑方向上终点相邻的应答器的编号*/
		for (i = 0u; i < 16u; i++)
		{
			pBaliseStru->dwTmnlBaliseId[i] = LongFromChar(pData);
			pData += 4u;

			/*沿线路的终点方向，与本应答器相邻且对列车有效（可直接到达）的应答器间路径上，列车经过本应答器再经过该相邻应答器时的方向*/
			pBaliseStru->wPassTmnlBaliseDir[i] = ShortFromChar(pData);
			pData += 2u;

			/*在所处link逻辑方向上终点相邻的应答器的距离*/
			pBaliseStru->dwDistanceToTmnlBalise[i] = LongFromChar(pData);
			pData += 4u;

			/*终点相邻应答器关联道岔编号*/
			pBaliseStru->wTmnlRelatePointID[i] = ShortFromChar(pData);
			pData += 2u;

			/*终点相邻应答器关联道岔状态*/

			pBaliseStru->wTmnlRelatePointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*LEU编号*/
		pBaliseStru->wLeuId = ShortFromChar(pData);
		pData += 2u;

		/*电子地图版本信息---根据互联互通需求增加 by hgj 20150507*/
		pBaliseStru->wEmapVersion = ShortFromChar(pData);
		pData += 2u;
	}
}
#endif

/**************************************
函数功能：应答器数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pAxleStru 被初始化的结构体指针
返回值：无
修改说明：根据车车数据结构V1.0.0需求增加，2019-7-10 sds
数据解耦修改部分数据 by lyk 20220307
***************************/
static void BALISE_STRU_init_CC(UINT8 * pDataAdd, DSU_BALISE_STRU * pBaliseStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pBaliseStru))
	{
		pData = pDataAdd;

		/*编号*/
		pBaliseStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*应答器ID*/
		pBaliseStru->dwBaliseId = LongFromChar(pData);
		pData += 4u;


		/*应答器所处link编号*/
		pBaliseStru->wLkId = ShortFromChar(pData);
		pData += 2u;

		/*应答器所处link偏移量(cm)*/
		pBaliseStru->dwLkOfst = LongFromChar(pData);
		pData += 4u;

		/*应答器类型*/
		pBaliseStru->wBaliseType = ShortFromChar(pData);
		pData += 2u;

		/*在所处link逻辑方向上起点的相邻的应答器数量*/
		pBaliseStru->wOrgnBaliseNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 16u; i++)
		{
			/*在所处link逻辑方向上起点相邻的应答器的编号*/
			pBaliseStru->dwOrgnBaliseId[i] = LongFromChar(pData);
			pData += 4u;

			/*沿线路的起点方向，与本应答器相邻且对列车有效（可直接到达）的应答器间路径上，列车经过本应答器再经过该相邻应答器时的方向*/
			pBaliseStru->wPassOrgnBaliseDir[i] = ShortFromChar(pData);
			pData += 2u;

			/*在所处link逻辑方向上起点相邻的应答器的距离*/
			pBaliseStru->dwDistanceToOrgnBalise[i] = LongFromChar(pData);
			pData += 4u;

			/*起点相邻应答器关联道岔编号*/
			pBaliseStru->wOrgnRelatePointID[i] = ShortFromChar(pData);
			pData += 2u;

			/*起点相邻应答器关联道岔状态*/
			pBaliseStru->wOrgnRelatePointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*在所处link逻辑方向上终点的相邻的应答器数量*/
		pBaliseStru->wTmnlBaliseNum = ShortFromChar(pData);
		pData += 2u;

		/*在所处link逻辑方向上终点相邻的应答器的编号*/
		for (i = 0u; i < 16u; i++)
		{
			pBaliseStru->dwTmnlBaliseId[i] = LongFromChar(pData);
			pData += 4u;

			/*沿线路的终点方向，与本应答器相邻且对列车有效（可直接到达）的应答器间路径上，列车经过本应答器再经过该相邻应答器时的方向*/
			pBaliseStru->wPassTmnlBaliseDir[i] = ShortFromChar(pData);
			pData += 2u;

			/*在所处link逻辑方向上终点相邻的应答器的距离*/
			pBaliseStru->dwDistanceToTmnlBalise[i] = LongFromChar(pData);
			pData += 4u;

			/*终点相邻应答器关联道岔编号*/
			pBaliseStru->wTmnlRelatePointID[i] = ShortFromChar(pData);
			pData += 2u;

			/*终点相邻应答器关联道岔状态*/
			pBaliseStru->wTmnlRelatePointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}
#if 0
		/*新增LEU编号*/
		pBaliseStru->wLeuId = ShortFromChar(pData);
		pData += 2u;
#endif
		/*电子地图版本信息---根据互联互通需求增加 by hgj 20150507*/
		pBaliseStru->wEmapVersion = ShortFromChar(pData);
		pData += 2u;
	}
}

/*调用接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
/*删除互联互通和车车以外的接口 by lyk 20220307*/
void BALISE_STRU_init(UINT8 *pDataAdd, DSU_BALISE_STRU *pBaliseStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pBaliseStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
#if 0
			BALISE_STRU_init_HLT(pDataAdd, pBaliseStru);/*互联互通的处理*/
#endif
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			BALISE_STRU_init_CC(pDataAdd, pBaliseStru);/*车车的处理*/
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：计轴器数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pAxleStru 被初始化的结构体指针
返回值：无
备注：该结构体无需改动
修改说明：根据合库数据结构V19.0.0需求修改，20180208 lmy
数据解耦修改部分数据 by lyk 20220307
***************************/
void AXLE_STRU_init(UINT8 *pDataAdd, DSU_AXLE_STRU *pAxleStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pAxleStru))
	{
		pData = pDataAdd;

		/*编号*/
		pAxleStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*计轴类型信息*/
		pAxleStru->wType = ShortFromChar(pData);
		pData += 2u;

		/*计轴器公里标*/
		pAxleStru->dwAxleKilo = LongFromChar(pData);
		pData += 4u;

		/*计轴器Lnk*/
		pAxleStru->wLinkId = ShortFromChar(pData);
		pData += 2u;

		/*计轴器Off*/
		pAxleStru->wLnkOff = LongFromChar(pData);
		pData += 4u;
	}

}

/**************************************
函数功能：安全屏蔽门数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pScreenStru 被初始化的结构体指针
返回值：无
备注：1、结构体需增加项： wManageLogicCI;所属逻辑CI区域编号 数据解耦修改部分数据 by lyk 20220307
2、此站台小屏蔽门的数量 ADD by lpw 20220520
***************************/
void SCREEN_STRU_init(UINT8 * pDataAdd, DSU_SCREEN_STRU * pScreenStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pScreenStru))
	{
		pData = pDataAdd;

		pScreenStru->wId = ShortFromChar(pData);
		pData += 2u;

		pScreenStru->wRelatLkId = ShortFromChar(pData);
		pData += 2u;

		pScreenStru->wwRelatLkOrient = ShortFromChar(pData);
		pData += 2u;

		pScreenStru->wVobcCiID = ShortFromChar(pData);
		pData += 2u;

		pScreenStru->wLittlePSDNUM = *pData;
		pData += 1u;
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：紧急停车按钮数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pEmergStopStru 被初始化的结构体指针
返回值：无
备注：结构体需增加项：
UINT16  wManageLogicCI;						所属逻辑CI区域编号
数据解耦修改部分数据 by lyk 20220307
***************************/
void EMERG_STOP_STRU_init(UINT8 * pDataAdd, DSU_EMERG_STOP_STRU * pEmergStopStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pEmergStopStru))
	{
		pData = pDataAdd;

		pEmergStopStru->wId = ShortFromChar(pData);
		pData += 2u;

		pEmergStopStru->wRelatLkId = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

static void STATION_STRU_init_HLT(UINT8 * pDataAdd, DSU_STATION_STRU * pStationStru)
{
#if 0
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pStationStru))
	{
		pData = pDataAdd;
		pStationStru->wId = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 20u; i++)
		{
			pStationStru->szCaption[i] = *pData;
			pData += 1u;
		}
		/*目的地编码*/
		for (i = 0u; i < 4u; i++)
		{
			pStationStru->bDesId[i] = *pData;
			pData += 1u;
		}

		pStationStru->wLinkId = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wAttribute = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wMinParkTime = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wMaxParkTime = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wParkTime = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wScreenNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 2u; i++)
		{
			pStationStru->wScreenId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		pStationStru->wEmergStopNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 2u; i++)
		{
			pStationStru->wEmergStopId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		pStationStru->wStopPointNum = ShortFromChar(pData);
		pData += 2u;
		/*包含的停车点,互联互通为四个*/
		for (i = 0u; i < 4u; i++)
		{
			pStationStru->wStopPointID[i] = ShortFromChar(pData);
			pData += 2u;
		}

		pStationStru->wDoorOpenMode = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wDoorOpenTime = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wDoorCloseMode = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wDoorCloseTime = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wParkSteadyTime = ShortFromChar(pData);
		pData += 2u;

		pStationStru->wStationID = ShortFromChar(pData); /*2013年8月13日,由车载提出增加*/
		pData += 2u;

		pStationStru->wLuggageOpenMode = ShortFromChar(pData);	/*行李开门方式，依据互联互通数据结构V12.0.0添加，add by lmy 20180312*/
		pData += 2u;

		pStationStru->wLuggageCloseMode = ShortFromChar(pData);	/*行李关门方式，依据互联互通数据结构V12.0.0添加，add by lmy 20180312*/
		pData += 2u;

		pStationStru->wCrashResSpeed = ShortFromChar(pData);	/*允许碰撞限速值，依据互联互通数据结构V12.0.0添加，add by lmy 20180312*/
		pData += 2u;

		pStationStru->wATSStationId = ShortFromChar(pData);	    /*ATS停车区域ID*/
		pData += 2u;
	}
#endif
}

/***********************************************************************************
函数功能：停车区域信息数据结构体初始化函数-车车系统使用 add by sds 2019-6-13
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pStationStru 被初始化的结构体指针
返回值：无
备注： 数据解耦修改部分数据 by lyk 20220307
***********************************************************************************/
static void STATION_STRU_init_CC(UINT8 *pDataAdd, DSU_STATION_STRU *pStationStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pStationStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pStationStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*目的地编码*/
		for (i = 0u; i < 4u; i++)
		{
			pStationStru->bDesId[i] = *pData;
			pData += 1u;
		}

		/*停车区域Link编号*/
		pStationStru->wLinkId = ShortFromChar(pData);
		pData += 2u;

		/*停车区域属性*/
		pStationStru->wAttribute = LongFromChar(pData);
		pData += 4u;

		/*站台默认停站时间*/
		pStationStru->wParkTime = ShortFromChar(pData);
		pData += 2u;

		/*屏蔽门数量*/
		pStationStru->wScreenNum = ShortFromChar(pData);
		pData += 2u;

		/*屏蔽门编号*/
		for (i = 0u; i < 2u; i++)
		{
			pStationStru->wScreenId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*紧急停车按钮数量*/
		pStationStru->wEmergStopNum = ShortFromChar(pData);
		pData += 2u;

		/*紧急停车按钮编号*/
		for (i = 0u; i < 2u; i++)
		{
			pStationStru->wEmergStopId[i] = ShortFromChar(pData);
			pData += 2u;
		}


		/*站台开门方式*/
		pStationStru->wDoorOpenMode = ShortFromChar(pData);
		pData += 2u;

		/*站台开门时间间隔*/
		pStationStru->wDoorOpenTime = ShortFromChar(pData);
		pData += 2u;

		/*站台关门方式*/
		pStationStru->wDoorCloseMode = ShortFromChar(pData);
		pData += 2u;

		/*站台关门时间间隔*/
		pStationStru->wDoorCloseTime = ShortFromChar(pData);
		pData += 2u;

		/*行李开门方式*/
		pStationStru->wLuggageOpenMode = ShortFromChar(pData);
		pData += 2u;

		/*行李关门方式*/
		pStationStru->wLuggageCloseMode = ShortFromChar(pData);
		pData += 2u;

		/*允许碰撞限速值*/
		pStationStru->wCrashResSpeed = ShortFromChar(pData);
		pData += 2u;

		/*ITS停车区域编号*/
		pStationStru->wITSStationId = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/*调用接口*/
/*备注：依据互联互通数据结构V12.0.0修改，modify by lmy 20180312*/
/*删除互联互通和车车以外的接口 by lyk 20220307*/
void STATION_STRU_init(UINT8 *pDataAdd, DSU_STATION_STRU *pStationStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pStationStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
			STATION_STRU_init_HLT(pDataAdd, pStationStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			STATION_STRU_init_CC(pDataAdd, pStationStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：停车点数据结构体初始化函数--车车系统专用
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pStoppointStru 被初始化的结构体指针
返回值：无
修改记录： 增加四字段 add by lpw 20220520
***************************/
static void STOPPOINT_STRU_init_CC(UINT8 * pDataAdd, DSU_STOPPOINT_STRU * pStoppointStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pStoppointStru))
	{
		pData = pDataAdd;

		pStoppointStru->wId = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wAttribute = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->wDir = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wLinkId = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->dwLinkOfst = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->wOverLapId = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->dwATOParkArea = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->dwATPParkArea = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->wStationID = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wFirstPSDID = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wLastPSDID = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wFirstDOORID = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wLastDOORID = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：停车点数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pStoppointStru 被初始化的结构体指针
返回值：无
备注：结构体需增加项
UINT16  wMinStopTime;					最小停站时间
UINT16  wMaxStopTime;					最大停站时间
UINT16  wStopTime;						默认停站时间

***************************/
static void STOPPOINT_STRU_init_DFT(UINT8 * pDataAdd, DSU_STOPPOINT_STRU * pStoppointStru)
{
#if 0
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pStoppointStru))
	{
		pData = pDataAdd;

		pStoppointStru->wId = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wAttribute = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->wDir = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wLinkId = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->dwLinkOfst = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->wOverLapId = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->dwATOParkArea = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->dwATPParkArea = LongFromChar(pData);
		pData += 4u;

		pStoppointStru->wStationID = ShortFromChar(pData);
		pData += 2u;

		pStoppointStru->wStationDoorID = *pData;
		pData += 1u;
	}
	else
	{
		/* do nothing */
	}
#endif
}

/*备注：依据互联互通数据结构V12.0.0修改，modify by lmy 20180312*/
/**************************************
函数功能：停车点数据结构体初始化函数-因增加车车系统 修改函数功能
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pStoppointStru 被初始化的结构体指针
返回值：无
备注：结构体需增加项
UINT16  wMinStopTime;					最小停站时间
UINT16  wMaxStopTime;					最大停站时间
UINT16  wStopTime;						默认停站时间

***************************/
void STOPPOINT_STRU_init(UINT8 * pDataAdd, DSU_STOPPOINT_STRU * pStoppointStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pStoppointStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if ((DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_CPK == cbtcVersionType)
			|| (DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType))
		{
			STOPPOINT_STRU_init_DFT(pDataAdd, pStoppointStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			STOPPOINT_STRU_init_CC(pDataAdd, pStoppointStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：计轴区段数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pAxleSgmtStru 被初始化的结构体指针
返回值：无
备注：结构体需增加项
UINT16  wMiddleLinkId;							计轴区段中间包含link编号
UINT32 dwOrgnLinkOfst;							起点所处link偏移量
UINT32 dwTmnlLinkOfst;							终点所处link偏移量
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
数据解耦修改部分数据 by lyk 20220307
***************************/
void AXLE_SGMT_STRU_init_DFT(UINT8 * pDataAdd, DSU_AXLE_SGMT_STRU * pAxleSgmtStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pAxleSgmtStru))
	{
		pData = pDataAdd;

		/*编号*/
		pAxleSgmtStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*起点计轴器编号*/
		pAxleSgmtStru->wOrgnAxleId = ShortFromChar(pData);
		pData += 2u;

		/*终点计轴器编号*/
		pAxleSgmtStru->wTmnlAxleId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link编号*/
		pAxleSgmtStru->wOrgnLkId = ShortFromChar(pData);
		pData += 2u;

		/*终点所处link编号*/
		pAxleSgmtStru->wTmnlLkId = ShortFromChar(pData);
		pData += 2u;

		/*计轴区段包含的逻辑区段的个数*/
		pAxleSgmtStru->wIncludeLogicSgmtNum = ShortFromChar(pData);
		pData += 2u;

		/*计轴区段包含的逻辑区段的编号*/
		for (i = 0u; i < 50u; i++)
		{
			pAxleSgmtStru->wLogicSgmtId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*关联道岔数目*/
		pAxleSgmtStru->wRelatPointNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 4u; i++)
		{
			/*包含道岔编号*/
			pAxleSgmtStru->wRelatPointId[i] = ShortFromChar(pData);
			pData += 2u;
			/*包含道岔状态*/
			pAxleSgmtStru->wRelatPointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*关联物理区段编号*/
		pAxleSgmtStru->wRelatePhysicalSgmtId = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：计轴区段数据结构体初始化函数_车车数据结构
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pAxleSgmtStru 被初始化的结构体指针
返回值：无
备注：结构体需增加项
UINT16  wMiddleLinkId;							计轴区段中间包含link编号
UINT32 dwOrgnLinkOfst;							起点所处link偏移量
UINT32 dwTmnlLinkOfst;							终点所处link偏移量
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
数据解耦修改部分数据 by lyk 20220307
***************************/
void AXLE_SGMT_STRU_init_CC(UINT8 * pDataAdd, DSU_AXLE_SGMT_STRU * pAxleSgmtStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pAxleSgmtStru))
	{
		pData = pDataAdd;

		/*编号*/
		pAxleSgmtStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*起点计轴器编号*/
		pAxleSgmtStru->wOrgnAxleId = ShortFromChar(pData);
		pData += 2u;

		/*终点计轴器编号*/
		pAxleSgmtStru->wTmnlAxleId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link编号*/
		pAxleSgmtStru->wOrgnLkId = ShortFromChar(pData);
		pData += 2u;

		/*终点所处link编号*/
		pAxleSgmtStru->wTmnlLkId = ShortFromChar(pData);
		pData += 2u;

		/*包含逻辑区段数目*/
		pAxleSgmtStru->wIncludeLogicSgmtNum = ShortFromChar(pData);
		pData += 2u;

		/*逻辑区段编号*/
		for (i = 0u; i < 50u; i++)
		{
			pAxleSgmtStru->wLogicSgmtId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*关联道岔数目*/
		pAxleSgmtStru->wRelatPointNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 4u; i++)
		{
			/*包含道岔编号*/
			pAxleSgmtStru->wRelatPointId[i] = ShortFromChar(pData);
			pData += 2u;
			/*包含道岔状态*/
			pAxleSgmtStru->wRelatPointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*对应物理区段*/
		pAxleSgmtStru->wRelatePhysicalSgmtId = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：计轴区段数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pAxleSgmtStru 被初始化的结构体指针
返回值：无
修改记录：删除互联互通和车车以外的接口 by lyk 20220307
***************************/
void AXLE_SGMT_STRU_init(UINT8 * pDataAdd, DSU_AXLE_SGMT_STRU * pAxleSgmtStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pAxleSgmtStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
			AXLE_SGMT_STRU_init_DFT(pDataAdd, pAxleSgmtStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			AXLE_SGMT_STRU_init_CC(pDataAdd, pAxleSgmtStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}

}

/**************************************
函数功能：进路表数据结构体初始化函数-互联互通
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pRouteStru 被初始化的结构体指针
返回值：无
修改说明：
①适配数据结构变化，进路包含的点式接近区段、CBTC接近区段、点式触发区段、CBTC触发区段数量均可配置为0-30的动态范围 by sds 2019-5-15
***************************/
static void ROUTE_STRU_init_HLT(UINT8 * pDataAdd, DSU_ROUTE_STRU * pRouteStru)
{
#if 0
	UINT8 * pData = pDataAdd;
	UINT8 i = 0u;
	static UINT8 runCount = 0;/*执行该函数的次数 用于判定是否为第一次进入*/
	static UINT8 maxBlocAccessLinkNum = MAX_BLOC_ACCESS_LINK_NUM;/*进路包含的实际点式接近区段最大数量*/
	static UINT8 maxCBTCAccessLinkNum = MAX_BLOC_ACCESS_LINK_NUM;/*进路包含的实际CBTC接近区段最大数量*/
	static UINT8 maxBlocTRGAreLinkNum = MAX_BLOC_ACCESS_LINK_NUM;/*进路包含的实际点式触发区段最大数量*/

																 /*进路ID*/
	pRouteStru->wId = ShortFromChar(pData);
	pData += 2u;

	/*进路名称*/
	for (i = 0u; i<24; i++)
	{
		pRouteStru->szCaption[i] = *pData;
		pData += 1u;
	}

	/*进路性质*/
	pRouteStru->wRouteType = ShortFromChar(pData);
	pData += 2u;

	/*起始信号机编号*/
	pRouteStru->wStartSignalId = ShortFromChar(pData);
	pData += 2u;

	/*终止信号机编号*/
	pRouteStru->wEndSignalId = ShortFromChar(pData);
	pData += 2u;

	/*起点所处link编号*/		/*新增---严广学*/
	pRouteStru->wOrgnLkId = ShortFromChar(pData);
	pData += 2u;

	/*终点所处link编号*/		/*新增---严广学*/
	pRouteStru->wTmnlLkId = ShortFromChar(pData);
	pData += 2u;

	/*包含道岔数目*/
	pRouteStru->wIncludePointNum = ShortFromChar(pData);
	pData += 2u;


	for (i = 0u; i<16; i++)
	{
		/*道岔编号*/
		pRouteStru->wPointId[i] = ShortFromChar(pData);
		pData += 2u;

		/*道岔状态*/
		pRouteStru->wPointStatus[i] = ShortFromChar(pData);
		pData += 2u;
	}

	/*保护区段数目*/
	pRouteStru->wProtectLinkNum = ShortFromChar(pData);
	pData += 2u;


	for (i = 0u; i<4; i++)
	{
		/*保护区段编号*/
		pRouteStru->wProtectLinkId[i] = ShortFromChar(pData);
		pData += 2u;

		/*保护区段关联道岔编号*/
		pRouteStru->wProtectLinkRelatePointId[i] = ShortFromChar(pData);
		pData += 2u;

		/*保护区段关联道岔状态*/
		pRouteStru->wProtectLinkRelatePointStatus[i] = ShortFromChar(pData);
		pData += 2u;
	}


	/* 程序中"点式接近区段数目"、"CBTC接近区段数目"、"点式触发区段数量"、"cbtc触发区段数量" 为固定值,
	导致一旦数据表中数据增加就要修改代码,为优化此问题修改结构体添充逻辑, by sds  2019-5-16 */

	/*判断是否第一次执行  第一次执行时计算包含的区段数量   后续进入时复用第一次计算的成果*/
	if (0 == runCount)
	{
		/* 获取点式接近区段数目 */
		pRouteStru->wBlogAccessLinkNum = ShortFromChar(pData);
		maxBlocAccessLinkNum = ShortFromChar(pData);
		pData += 2u;

		/* 根据获取到的点式接近区段数目进行数据填充 */
		for (i = 0u; i<pRouteStru->wBlogAccessLinkNum; i++)
		{
			pRouteStru->wBlogAccessLinkId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*获取CBTC接近区段数目*/
		for (i = 0u; i<MAX_BLOC_ACCESS_LINK_NUM; i++)
		{
			pRouteStru->wCBTCAccessLinkNum = ShortFromChar(pData);
			pData += 2u;

			/* 不断读取数据，丢弃无效值，直到读取到正确的值时，退出 */
			if ((pRouteStru->wCBTCAccessLinkNum != DSU_NULL_16)
				&& (pRouteStru->wCBTCAccessLinkNum <= MAX_CBTC_ACCESS_LINK_NUM))
			{
				/*进路的CBTC接近区段有效数量赋值*/
				maxCBTCAccessLinkNum = pRouteStru->wCBTCAccessLinkNum;
				break;
			}
			else
			{
				/*进路的点式接近区段总数量+1*/
				maxBlocAccessLinkNum++;
			}
		}

		/* 根据获取到的CBTC接近区段数目进行数据填充 */
		for (i = 0u; i<pRouteStru->wCBTCAccessLinkNum; i++)
		{
			pRouteStru->wCBTCAccessLinkId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*所属CI区域编号*/
		for (i = 0u; i<MAX_CBTC_ACCESS_LINK_NUM; i++)
		{
			pRouteStru->wManageCI = ShortFromChar(pData);
			pData += 2u;

			/* 不断读取数据，丢弃无效值，直到读取到正确的值时，退出 */
			if (pRouteStru->wManageCI != DSU_NULL_16)
			{
				break;
			}
			else
			{
				/*进路的CBTC接近区段总数量+1*/
				maxCBTCAccessLinkNum++;
			}
		}

		/*进路属性*/
		pRouteStru->wAttribute = ShortFromChar(pData);
		pData += 2u;


		/*点式触发区段数量	*/
		pRouteStru->wBlocTrgAreaNum = ShortFromChar(pData);
		maxBlocTRGAreLinkNum = ShortFromChar(pData);
		pData += 2u;

		/*点式触发区段ID*/
		for (i = 0u; i<pRouteStru->wBlocTrgAreaNum; i++)
		{
			pRouteStru->wBlocTrgAreaId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*cbtc触发区段数量	*/
		for (i = 0u; i<MAX_BLOC_TRG_AREA_NUM; i++)
		{
			pRouteStru->wCbtcTrgAreaNum = ShortFromChar(pData);
			pData += 2u;

			/* 不断读取数据，丢弃无效值，直到读取到正确的值时，退出 */
			if ((pRouteStru->wCbtcTrgAreaNum != DSU_NULL_16)
				&& (pRouteStru->wCbtcTrgAreaNum <= MAX_CBTC_TRG_AREA_NUM))
			{
				break;
			}
			else
			{
				/*进路的点式触发区段总数量+1*/
				maxBlocTRGAreLinkNum++;
			}
		}

		/*cbtc触发区段ID*/
		for (i = 0u; i<pRouteStru->wCbtcTrgAreaNum; i++)
		{
			pRouteStru->wCbtcTrgAreaId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*执行次数+1*/
		runCount++;
	}
	else
	{

		/* 获取点式接近区段数目 */
		pRouteStru->wBlogAccessLinkNum = ShortFromChar(pData);
		pData += 2u;

		/* 根据获取到的点式接近区段数目进行数据填充 */
		for (i = 0u; i<maxBlocAccessLinkNum; i++)
		{
			pRouteStru->wBlogAccessLinkId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*获取CBTC接近区段数目*/
		pRouteStru->wCBTCAccessLinkNum = ShortFromChar(pData);
		pData += 2u;

		/* 根据获取到的CBTC接近区段数目进行数据填充 */
		for (i = 0u; i<maxCBTCAccessLinkNum; i++)
		{
			pRouteStru->wCBTCAccessLinkId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*所属CI区域编号*/
		pRouteStru->wManageCI = ShortFromChar(pData);
		pData += 2u;

		/*进路属性*/
		pRouteStru->wAttribute = ShortFromChar(pData);
		pData += 2u;


		/*点式触发区段数量	*/
		pRouteStru->wBlocTrgAreaNum = ShortFromChar(pData);
		pData += 2u;

		/*点式触发区段ID*/
		for (i = 0u; i<maxBlocTRGAreLinkNum; i++)
		{
			pRouteStru->wBlocTrgAreaId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*cbtc触发区段数量	*/
		pRouteStru->wCbtcTrgAreaNum = ShortFromChar(pData);
		pData += 2u;

		/*cbtc触发区段ID-依据实际数量进行数据获取*/
		for (i = 0u; i<pRouteStru->wCbtcTrgAreaNum; i++)
		{
			pRouteStru->wCbtcTrgAreaId[i] = ShortFromChar(pData);
			pData += 2u;
		}

	}
#endif
}
/**************************************
函数功能：进路表数据结构体初始化函数-车车
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pRouteStru 被初始化的结构体指针
返回值：无
修改说明：创建，wyd 20220214
数据解耦修改部分数据 by lyk 20220307
数据结构增加点式接近区段数据读取 by xly  20230802
***************************/
static void ROUTE_STRU_init_CC(UINT8 * pDataAdd, DSU_ROUTE_STRU * pRouteStru)
{
	UINT8 * pData = NULL;
	UINT8 i = 0u;

	if ((NULL == pDataAdd) || (NULL == pRouteStru))
	{
		/*do nothing*/
	}
	else
	{
		pData = pDataAdd;

		/*进路ID*/
		pRouteStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*进路性质*/
		pRouteStru->wRouteType = ShortFromChar(pData);
		pData += 2u;

		/*起始信号机编号*/
		pRouteStru->wStartSignalId = ShortFromChar(pData);
		pData += 2u;

		/*终止信号机编号*/
		pRouteStru->wEndSignalId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link编号*/		/*新增---严广学*/
		pRouteStru->wOrgnLkId = ShortFromChar(pData);
		pData += 2u;

		/*终点所处link编号*/		/*新增---严广学*/
		pRouteStru->wTmnlLkId = ShortFromChar(pData);
		pData += 2u;

		/*包含道岔数目*/
		pRouteStru->wIncludePointNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 16u; i++)
		{
			/*道岔编号*/
			pRouteStru->wPointId[i] = ShortFromChar(pData);
			pData += 2u;

			/*道岔状态*/
			pRouteStru->wPointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*保护区段数目*/
		pRouteStru->wProtectLinkNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 4u; i++)
		{
			/*保护区段编号*/
			pRouteStru->wProtectLinkId[i] = ShortFromChar(pData);
			pData += 2u;

			/*保护区段关联道岔编号*/
			pRouteStru->wProtectLinkRelatePointId[i] = ShortFromChar(pData);
			pData += 2u;

			/*保护区段关联道岔状态*/
			pRouteStru->wProtectLinkRelatePointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*点式接近区段数目*/
		pRouteStru->wBlocAccessLinkNum = ShortFromChar(pData);
		pData += 2u;

		if (MAX_BLOC_ACCESS_LINK_NUM >= pRouteStru->wBlocAccessLinkNum)
		{
			for (i = 0u; i < pRouteStru->wBlocAccessLinkNum; i++)
			{
				/*接近区段编号*/
				pRouteStru->wBlocAccessLinkIdBuff[i] = ShortFromChar(pData);
				pData += 2u;
			}
		}
		else
		{
			/*数组越界防护，导向没有接近区段，无安全问题*/
		}
		/*跳过多配的65535*/
		for (; i < MAX_BLOC_ACCESS_LINK_NUM; i++)
		{
			if (0xFFFFU == ShortFromChar(pData))
			{
				pData += 2u;
			}
			else
			{
				break;
			}
		}

		/*接近区段数目*/
		pRouteStru->wAccessLinkNum = ShortFromChar(pData);
		pData += 2u;

		if (MAX_CBTC_ACCESS_LINK_NUM >= pRouteStru->wAccessLinkNum)
		{
			for (i = 0u; i < pRouteStru->wAccessLinkNum; i++)
			{
				/*接近区段编号*/
				pRouteStru->wAccessLinkIdBuff[i] = ShortFromChar(pData);
				pData += 2u;
			}
		}
		else
		{
			/*数组越界防护，导向没有接近区段，无安全问题*/
		}

		/*跳过多配的65535*/
		for (; i < MAX_CBTC_ACCESS_LINK_NUM; i++)
		{
			if (0xFFFFU == ShortFromChar(pData))
			{
				pData += 2u;
			}
			else
			{
				break;
			}
		}
	}
}

/*调用接口
备注：
1. 增加对单轨项目标识的处理，modify by qxt 20170726
2. 修改互联互通项目数据结构进路表处理逻辑，兼容对进路内包含的点式/CBTC接近区段，点式/CBTC触发区段动态列数的处理，m by sds 2019-5-16
3. 删除互联互通和车车以外的接口 by lyk 20220307*/
void ROUTE_STRU_init(UINT8 *pDataAdd, DSU_ROUTE_STRU *pRouteStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL == pDataAdd) || (NULL == pRouteStru))
	{
		/*do nothing*/
	}
	else
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
			ROUTE_STRU_init_HLT(pDataAdd, pRouteStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			ROUTE_STRU_init_CC(pDataAdd, pRouteStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
}

/**************************************
函数功能：逻辑区段数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pProtectLinkStru被初始化的结构体指针
返回值：无
备注：结构体需增加项：
UINT16  wRouteId;							进路编号
UINT16  wIncludeAxleSgmtId;					包含的计轴区段编号
UINT16  wObsNum;							包含障碍物数目
UINT16  wObsType;							障碍物类型
数据解耦修改部分数据 by lyk 20220307
***************************/

void LOGIC_SGMT_STRU_init(UINT8 * pDataAdd, DSU_LOGIC_SGMT_STRU * pLogicSgmtStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pLogicSgmtStru))
	{
		pData = pDataAdd;

		/*编号*/
		pLogicSgmtStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link编号*/
		pLogicSgmtStru->wOrgnLkId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link偏移量(cm)*/
		pLogicSgmtStru->dwOrgnLkOfst = LongFromChar(pData);
		pData += 4u;

		/*终点所处link编号*/
		pLogicSgmtStru->wTmnlLkId = ShortFromChar(pData);
		pData += 2u;

		/*终点所处link偏移量(cm)*/
		pLogicSgmtStru->dwTmnlLkOfst = LongFromChar(pData);
		pData += 4u;
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：点式接近区段信息数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pCBTCAccessLinkStru被初始化的结构体指针
返回值：无
备注：结构体无需更改
数据解耦修改部分数据 by xly 20230802
***************************/
void BLOC_ACCESS_LINK_STRU_init(UINT8 *pDataAdd, DSU_BLOC_ACCESS_LINK_STRU *pBLOCAccessLinkStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pBLOCAccessLinkStru))
	{
		pData = pDataAdd;
		/*编号*/
		pBLOCAccessLinkStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*包含的逻辑区段数目*/
		pBLOCAccessLinkStru->wIncludeAxleSgmtNum = ShortFromChar(pData);
		pData += 2u;

		/*包含的逻辑区段编号*/
		for (i = 0u; i < 10u; i++)
		{
			pBLOCAccessLinkStru->wIncludeAxleSgmtId[i] = ShortFromChar(pData);
			pData += 2u;
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：CBTC接近区段信息数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out] pCBTCAccessLinkStru被初始化的结构体指针
返回值：无
备注：结构体无需更改
数据解耦修改部分数据 by lyk 20220307
***************************/
void CBTC_ACCESS_LINK_STRU_init(UINT8 *pDataAdd, DSU_CBTC_ACCESS_LINK_STRU *pCBTCAccessLinkStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pCBTCAccessLinkStru))
	{
		pData = pDataAdd;
		/*编号*/
		pCBTCAccessLinkStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*包含的逻辑区段数目*/
		pCBTCAccessLinkStru->wIncludeLogicSgmtNum = ShortFromChar(pData);
		pData += 2u;

		/*包含的逻辑区段编号*/
		for (i = 0u; i < 20u; i++)
		{
			pCBTCAccessLinkStru->wIncludeLogicSgmtId[i] = ShortFromChar(pData);
			pData += 2u;
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：坡度数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pGradeStru被初始化的结构体指针
返回值：无
备注：结构体需增加：
UINT32  dwGradeLength;							坡度长度
UINT16  wIncludeLinkId[3];							坡度线路中间包含link编号


***************************/
void GRADE_STRU_init(UINT8 * pDataAdd, DSU_GRADE_STRU * pGradeStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pGradeStru))
	{
		pData = pDataAdd;
		/*编号*/
		pGradeStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*坡度起点所处link编号*/
		pGradeStru->wOrgnLinkId = ShortFromChar(pData);
		pData += 2u;

		/*坡度起点所处link偏移量*/
		pGradeStru->dwOrgnLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*坡度终点所处link编号*/
		pGradeStru->wTmnlLinkId = ShortFromChar(pData);
		pData += 2u;

		/*坡度终点所处link偏移量*/
		pGradeStru->dwTmnlLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*起点关联道岔编号*/
		pGradeStru->wOrgnRelatePointId = ShortFromChar(pData);
		pData += 2u;

		/*起点正线坡度编号*/
		pGradeStru->wOrgnJointMainGradeId = ShortFromChar(pData);
		pData += 2u;

		/*起点侧线坡度编号*/
		pGradeStru->wOrgnJointSideGradeId = ShortFromChar(pData);
		pData += 2u;

		/*终点关联道岔编号*/
		pGradeStru->wTmnlRelatePointId = ShortFromChar(pData);
		pData += 2u;

		/*终点正线坡度编号*/
		pGradeStru->wTmnlJointMainGradeId = ShortFromChar(pData);
		pData += 2u;

		/*终点侧线坡度编号*/
		pGradeStru->wTmnlJointSideGradeId = ShortFromChar(pData);
		pData += 2u;

		/*坡度值*/
		pGradeStru->wGradValue = ShortFromChar(pData);
		pData += 2u;

		/*坡段相对于线路逻辑方向的倾斜方向*/
		pGradeStru->wInclineDir = ShortFromChar(pData);
		pData += 2u;

		/*竖曲线半径*/		/*新增---严广学*/
		pGradeStru->dwRadius = LongFromChar(pData);
		pData += 4u;
	}
	else
	{
		/* do nothing */
	}
}
/**************************************
函数功能：线路静态限速数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pStaticResSpeedStru被初始化的结构体指针
返回值：无
备注：结构体无更改

***************************/
void STATIC_RES_SPEED_STRU_init(UINT8 * pDataAdd, DSU_STATIC_RES_SPEED_STRU * pStaticResSpeedStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pStaticResSpeedStru))
	{
		pData = pDataAdd;
		/*编号*/
		pStaticResSpeedStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*该限速区域所处link编号*/
		pStaticResSpeedStru->wLinkId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link偏移量(cm)*/
		pStaticResSpeedStru->dwOrgnLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*终点所处link偏移量(cm)*/
		pStaticResSpeedStru->dwTmnlLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*关联道岔编号*/
		pStaticResSpeedStru->wRelatePointId = ShortFromChar(pData);
		pData += 2u;

		/*静态限速值*/
		pStaticResSpeedStru->wResSpeed = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}
/**************************************
函数功能：默认行车序列数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pDefaultrouteStru被初始化的结构体指针
返回值：无
备注：结构体增加项：
UINT16  wCurrentPlatformId;				当前站台编号
UINT16  wCurrentPlatformFlag;			当前站台标志位

***************************/
void DEFAULTROUTE_STRU_init(UINT8 * pDataAdd, DSU_DEFAULTROUTE_STRU * pDefaultrouteStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pDefaultrouteStru))
	{
		pData = pDataAdd;
		/*编号*/
		pDefaultrouteStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*link编号*/
		pDefaultrouteStru->wLinkId = ShortFromChar(pData);
		pData += 2u;

		/*运行方向*/
		pDefaultrouteStru->wLinkDir = ShortFromChar(pData);
		pData += 2u;

		/*前方站台编号*/
		pDefaultrouteStru->wNextPlatformId = ShortFromChar(pData);
		pData += 2u;

		/*目标站台编号*/
		pDefaultrouteStru->wDestinationId = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}
/**************************************
函数功能：站间运营级别数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pRunLevelStru被初始化的结构体指针
返回值：无
备注：结构体无增加项：


***************************/
static void RUN_LEVEL_STRU_init_CPK(UINT8 * pDataAdd, DSU_RUN_LEVEL_STRU * pRunLevelStru)
{
#if 0
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pRunLevelStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pRunLevelStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*起点目的地编号*/
		pRunLevelStru->wOrgnDesId = ShortFromChar(pData);
		pData += 2u;

		/*终点目的地编号*/
		pRunLevelStru->wTmnlDesId = ShortFromChar(pData);
		pData += 2u;

		/*运行级别分级数目*/		/*新增---严广学*/
		pRunLevelStru->wRunLevelNum = ShortFromChar(pData);
		pData += 2u;


		for (i = 0u; i < 10; i++)
		{
			/*运营级别编号*/
			pRunLevelStru->wRunLevelID[i] = ShortFromChar(pData);
			pData += 2u;

			/*该运营级别时间*/
			pRunLevelStru->wRunTime[i] = ShortFromChar(pData);
			pData += 2u;

			/*该运营级别速度*/
			pRunLevelStru->wRunSpeed[i] = ShortFromChar(pData);
			pData += 2u;
		}
	}
	else
	{
		/* do nothing */
	}
#endif
}

/**************************************
函数功能：站间运营级别数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pRunLevelStru被初始化的结构体指针
返回值：无
备注：FAO和HLT共用
***************************/
static void RUN_LEVEL_STRU_init_DFT(UINT8 *pDataAdd, DSU_RUN_LEVEL_STRU *pRunLevelStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pRunLevelStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pRunLevelStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*起点目的地编号*/
		pRunLevelStru->wOrgnDesId = ShortFromChar(pData);
		pData += 2u;

		/*终点目的地编号*/
		pRunLevelStru->wTmnlDesId = ShortFromChar(pData);
		pData += 2u;

		/*运行级别分级数目*/		/*新增---严广学*/
		pRunLevelStru->wRunLevelNum = ShortFromChar(pData);
		pData += 2u;


		for (i = 0u; i < 6; i++)
		{
			/*运营级别编号*/
			pRunLevelStru->wRunLevelID[i] = ShortFromChar(pData);
			pData += 2u;

			/*该运营级别时间*/
			pRunLevelStru->wRunTime[i] = ShortFromChar(pData);
			pData += 2u;

			/*该运营级别速度*/
			pRunLevelStru->wRunSpeed[i] = ShortFromChar(pData);
			pData += 2u;
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：站间运营级别数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pRunLevelStru被初始化的结构体指针
返回值：无
备注：调用接口
增加对单轨项目标识的处理，modify by qxt 20170726
***************************/
void RUN_LEVEL_STRU_init(UINT8 *pDataAdd, DSU_RUN_LEVEL_STRU *pRunLevelStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pRunLevelStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if ((DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			RUN_LEVEL_STRU_init_DFT(pDataAdd, pRunLevelStru);
		}
		else if (DQU_CBTC_CPK == cbtcVersionType)
		{
			RUN_LEVEL_STRU_init_CPK(pDataAdd, pRunLevelStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}
}

/**************************************
函数功能：退出CBTC区域数据结构体初始化函数(原退出线路区域)
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pExitRouteStru被初始化的结构体指针
返回值：无
备注：结构体无增加项：


***************************/
void EXIT_ROUTE_STRU_init(UINT8 * pDataAdd, DSU_EXIT_ROUTE_STRU * pExitRouteStru)
{
	UINT8 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pExitRouteStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pExitRouteStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link编号*/
		pExitRouteStru->wOrgnLinkId = ShortFromChar(pData);
		pData += 2u;

		/*起点link偏移量(cm)*/
		pExitRouteStru->dwOrgnLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*终点所处link编号*/
		pExitRouteStru->wTmnlLinkId = ShortFromChar(pData);
		pData += 2u;

		/*终点link偏移量(cm)*/
		pExitRouteStru->dwTmnlLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*退出CBTC区域时列车运行方向*/
		pExitRouteStru->wDIR = ShortFromChar(pData);
		pData += 2u;

		/*退出CBTC区域属性*/
		pExitRouteStru->wType = ShortFromChar(pData);
		pData += 2u;

		/*包含道岔数量*/
		pExitRouteStru->wIncludePointNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 10U; i++)
		{
			/*包含道岔id*/
			pExitRouteStru->wPointId[i] = ShortFromChar(pData);
			pData += 2u;

			/*包含道岔状态*/
			pExitRouteStru->wPointStatus[i] = ShortFromChar(pData);
			pData += 2u;
		}
	}
	else
	{
		/* do nothing */
	}
}
/**************************************
函数功能：非CBTC区域数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pTrainLineStru被初始化的结构体指针
返回值：无
备注：结构体无增加项：

***************************/
void TRAIN_LINE_STRU_init(UINT8 * pDataAdd, DSU_TRAIN_LINE_STRU * pTrainLineStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pTrainLineStru))
	{
		pData = pDataAdd;
		/*编号*/
		pTrainLineStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link编号*/
		pTrainLineStru->wOrgnLinkId = ShortFromChar(pData);
		pData += 2u;

		/*起点link偏移量(cm)*/
		pTrainLineStru->dwOrgnLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*终点所处link编号*/
		pTrainLineStru->wTmnlLinkId = ShortFromChar(pData);
		pData += 2u;

		/*终点link偏移量(cm)*/
		pTrainLineStru->dwTmnlLinkOfst = LongFromChar(pData);
		pData += 4u;

		/*非CBTC区域列车运行方向*/
		pTrainLineStru->wDir = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}
/**************************************
函数功能：无人折返表数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pARStru被初始化的结构体指针
返回值：无
备注：结构体由原来的无人折返进入和无人折返退出表合并而成
数据解耦修改部分数据 by lyk 20220307
***************************/
void AR_STRU_init(UINT8 * pDataAdd, DSU_AR_STRU * pARStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pARStru))
	{
		pData = pDataAdd;

		/*索引编号*/
		pARStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*无人折返出发停车区域编号*/
		pARStru->wStartPlartformId = ShortFromChar(pData);
		pData += 2u;

		/*进入点对应停车点编号*/
		pARStru->wARInStopPointId = ShortFromChar(pData);
		pData += 2u;

		/*换端点对应停车点编号*/
		pARStru->wRevertPointId = ShortFromChar(pData);
		pData += 2u;

		/*退出停车区域编号*/
		pARStru->wEndPlartformId = ShortFromChar(pData);
		pData += 2u;

		/*退出点对应停车点编号*/
		pARStru->wAROutStopPointId = ShortFromChar(pData);
		pData += 2u;

		/*无人折返灯ID*/
		pARStru->wARLampID = ShortFromChar(pData);
		pData += 2u;

		/*无人折返按钮D*/
		pARStru->wARButtonId = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}

}

/**************************************
函数功能：CI信息数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pCIStru被初始化的结构体指针
返回值：无
备注：结构体无更改

***************************/
void CI_STRU_init_DFT(UINT8 * pDataAdd, DSU_CI_STRU * pCIStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pCIStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pCIStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*CI设备ID*/
		pCIStru->wID = ShortFromChar(pData);
		pData += 2u;

		/*联锁属性*/
		pCIStru->wType = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

void CI_STRU_init_HLT(UINT8 * pDataAdd, DSU_CI_STRU * pCIStru)
{
#if 0
	UINT16 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pCIStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pCIStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*CI设备ID*/
		pCIStru->wID = ShortFromChar(pData);
		pData += 2u;

		/*联锁属性*/
		pCIStru->wType = ShortFromChar(pData);
		pData += 2u;

		/*CI-VOBC共用数据校验信息*/
		pCIStru->dwVersionOfCIToVOBC = LongFromChar(pData);
		pData += 4u;

		/*CI-ZC共用数据校验信息*/
		pCIStru->dwVersionOfCIToZC = LongFromChar(pData);
		pData += 4u;

		/*CI-ATS共用数据校验信息*/
		pCIStru->dwVersionOfCIToATS = LongFromChar(pData);
		pData += 4u;

		/*相邻CI数量*/
		pCIStru->dwAdjoinCICount = *pData;
		pData += 1u;

		for (i = 0u; i < 6u; i++)
		{
			/*相邻CI[1-6]的ID*/
			pCIStru->dwAdjoinCIId[i] = ShortFromChar(pData);
			pData += 2u;

			/*与相邻CI[1-6]的共用数据校验信息*/
			pCIStru->dwAdjoinCIShareCheckData[i] = LongFromChar(pData);
			pData += 4u;

		}
	}
	else
	{
		/* do nothing */
	}
#endif
}

/**************************************
函数功能：CI信息数据结构体初始化函数--车车使用
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pCIStru被初始化的结构体指针
返回值：无
备注：结构体无更改
新建 by hekuan 20220307
***************************/
void CI_STRU_init_CC(UINT8 * pDataAdd, DSU_CI_STRU * pCIStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pCIStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pCIStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*TIOC设备ID*/
		pCIStru->wID = ShortFromChar(pData);
		pData += 2u;

		/*联锁属性*/
		pCIStru->wType = ShortFromChar(pData);
		pData += 2u;

		/*TIOC-IVOC共用数据校验信息*/
		pCIStru->dwVersionOfCIToVOBC = LongFromChar(pData);
		pData += 4u;
	}
	else
	{
		/* do nothing */
	}
}

/*调用接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
/*删除互联互通和车车以外的接口 by lyk 20220307*/
void CI_STRU_init(UINT8 *pDataAdd, DSU_CI_STRU *pCIStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pCIStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
			CI_STRU_init_HLT(pDataAdd, pCIStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			CI_STRU_init_CC(pDataAdd, pCIStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：ATS信息数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pATSStru被初始化的结构体指针
返回值：无
备注：结构体无更改

***************************/
void ATS_STRU_init_DFT(UINT8 * pDataAdd, DSU_ATS_STRU * pATSStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pATSStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pATSStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*ATS设备ID*/
		pATSStru->wID = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

void ATS_STRU_init_CC(UINT8 * pDataAdd, DSU_ATS_STRU * pATSStru)
{
	UINT16 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pATSStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pATSStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*ATS设备ID*/
		pATSStru->wID = ShortFromChar(pData);
		pData += 2u;

		/*ATS-VOBC共用数据校验信息*/
		pATSStru->dwVersionOfATSToVOBC = LongFromChar(pData);
		pData += 4u;

	}
	else
	{
		/* do nothing */
	}
}

void ATS_STRU_init_HLT(UINT8 * pDataAdd, DSU_ATS_STRU * pATSStru)
{
	UINT16 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pATSStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pATSStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*ATS设备ID*/
		pATSStru->wID = ShortFromChar(pData);
		pData += 2u;

		/*ATS-VOBC共用数据校验信息*/
		pATSStru->dwVersionOfATSToVOBC = LongFromChar(pData);
		pData += 4u;

		/*ATS-DSU共用数据校验信息*/
		pATSStru->dwVersionOfATSToDSU = LongFromChar(pData);
		pData += 4u;

		/*相邻ATS数量*/
		pATSStru->dwAdjoinATSCount = *pData;
		pData += 1u;

		for (i = 0u; i < 6u; i++)
		{
			/*相邻ATS[1-6]的ID*/
			pATSStru->dwAdjoinATSId[i] = ShortFromChar(pData);
			pData += 2u;

			/*与相邻ATS[1-6]的共用数据校验信息*/
			pATSStru->dwAdjoinATSShareCheckData[i] = LongFromChar(pData);
			pData += 4u;

		}
	}
	else
	{
		/* do nothing */
	}
}


/*调用接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
void ATS_STRU_init(UINT8 * pDataAdd, DSU_ATS_STRU * pATSStru)
{
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDataAdd) && (NULL != pATSStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if ((DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_CPK == cbtcVersionType))
		{
			ATS_STRU_init_DFT(pDataAdd, pATSStru);
		}
		else if ((DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType))
		{
			ATS_STRU_init_HLT(pDataAdd, pATSStru);
		}
		else if (DQU_CBTC_CC == cbtcVersionType)
		{
			ATS_STRU_init_CC(pDataAdd, pATSStru);
		}
		else
		{
			/*不可达分支*/
		}
	}
	else
	{
		/* do nothing */
	}
}


/**************************************
函数功能：保护区段信息数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pProtectLinkStru被初始化的结构体指针
返回值：无
备注：结构体需增加：
UINT16  wRouteId;							进路编号
UINT16  wIncludeAxleSgmtId;			包含的计轴区段编号（更改为数组）
UINT16  wObsNum;							包含障碍物数目
UINT16  wObsType;							障碍物类型

***************************/
void PROTECT_LINK_STRU_init(UINT8 * pDataAdd, DSU_PROTECT_LINK_STRU * pProtectLinkStru)
{
	UINT32 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pProtectLinkStru))
	{
		pData = pDataAdd;
		/*编号*/
		pProtectLinkStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*包含的计轴区段数目*/		/*新增---严广学*/
		pProtectLinkStru->wIncludeAxleSgmtNum = ShortFromChar(pData);
		pData += 2u;

		for (i = 0u; i < 4u; i++)
		{
			/*包含的计轴区段编号*/		/*新增---严广学*/  /*这个字段原来不是数组*/
			pProtectLinkStru->wIncludeAxleSgmtId[i] = ShortFromChar(pData);
			pData += 2u;
		}
	}
	else
	{
		/* do nothing */
	}

}

/*线路统一限速信息表*/
void CONRESSPEED_STRU_init(UINT8 * pDataAdd, DSU_CONRESSPEED_STRU * pConRESSpeedStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pConRESSpeedStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pConRESSpeedStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*限速值*/
		pConRESSpeedStru->wSpeed = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}

}

/*线路统一坡度信息表*/
void CONGRADE_STRU_init(UINT8 * pDataAdd, DSU_CONGRADE_STRU * pConGradeStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pConGradeStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pConGradeStru->wIndex = ShortFromChar(pData);
		pData += 2u;

		/*坡度值*/
		pConGradeStru->wGrade = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}



/*物理区段信息表*/
/*数据解耦修改部分数据 by lyk 20220307*/
/*YF0069-2195  删除未使用变量 by xly 20221130*/
void PHYSICAL_SGMT_STRU_init(UINT8 * pDataAdd, DSU_PHYSICAL_SGMT_STRU * pPhysicalSgmtStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pPhysicalSgmtStru))
	{
		pData = pDataAdd;
		/*ID*/
		pPhysicalSgmtStru->wId = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/*防淹门表,2014.7.18*/
void DSU_FLOOD_GATE_STRU_init(UINT8 * pDataAdd, DSU_FLOOD_GATE_STRU * pFloodGateStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pFloodGateStru))
	{
		pData = pDataAdd;
		/*编号*/
		pFloodGateStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*Link号*/
		pFloodGateStru->wFloodGateLinkID = ShortFromChar(pData);
		pData += 2u;

		/*偏移量*/
		pFloodGateStru->dwFloodGateLinkoffset = LongFromChar(pData);
		pData += 4u;
		/*防护方向*/
		pFloodGateStru->wDir = *pData;
		pData += 1u;
	}
	else
	{
		/* do nothing */
	}
}

/*SPKS开关表*/
void DSU_SPKS_BUTTON_STRU_init(UINT8 *pDataAdd, DSU_SPKS_BUTTON_STRU *pSpksButtonStru)
{
	UINT16 i = 0u;
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pSpksButtonStru))
	{
		pData = pDataAdd;
		/*编号*/
		pSpksButtonStru->wId = ShortFromChar(pData);
		pData += 2u;

		pSpksButtonStru->wPhysicalSgmtNum = ShortFromChar(pData);
		pData += 2u;

		/*物理区段ID*/
		for (i = 0u; i < 100u; i++)
		{
			pSpksButtonStru->wRelatePhysicalSgmtId[i] = ShortFromChar(pData);
			pData += 2u;
		}

		/*所属CI区域编号*/
		pSpksButtonStru->wManageCI = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/*车库门表*/
/*数据解耦修改部分数据 by lyk 20220307*/
void DEPOT_GATE_STRU_init(UINT8 *pDataAdd, DSU_DEPOT_GATE_STRU *pDepotGateStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pDepotGateStru))
	{
		pData = pDataAdd;

		/*车库门编号*/
		pDepotGateStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*车库门所在Link*/
		pDepotGateStru->wDepotGateLinkID = ShortFromChar(pData);
		pData += 2u;

		/*Link偏移量*/
		pDepotGateStru->dwDepotGateLinkoffset = LongFromChar(pData);
		pData += 4u;

		/*车库门对应A库线物理区段编号*/
		pDepotGateStru->wAPhysicalSgmtId = ShortFromChar(pData);
		pData += 2u;

		/*车库门对应B库线物理区段编号*/
		pDepotGateStru->wBPhysicalSgmtId = ShortFromChar(pData);
		pData += 2u;

		/*车库门属性*/
		pDepotGateStru->sDepotAttribute = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/*隧道表*/
void TUNNEL_STRU_init(UINT8 *pDataAdd, DSU_TUNNEL_STRU *pTunnelStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pTunnelStru))
	{
		pData = pDataAdd;
		/*索引编号*/
		pTunnelStru->wId = ShortFromChar(pData);
		pData += 2u;

		/*隧道所处link编号*/
		pTunnelStru->wTunLinkId = ShortFromChar(pData);
		pData += 2u;

		/*起点所处link偏移量*/
		pTunnelStru->dwBeginTunLinkOffset = LongFromChar(pData);
		pData += 4u;

		/*起点所处link偏移量*/
		pTunnelStru->dwEndTunLinkOffset = LongFromChar(pData);
		pData += 4u;
	}
	else
	{
		/* do nothing */
	}
}

/**********************************************************
函数功能：分相区表信息数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pPowerlessZoneStru被初始化的结构体指针
返回值：无
备注：依据互联互通数据结构V12.0.0添加，add by lmy 20180312
***********************************************************/
void POWERLESS_ZONE_STRU_init(UINT8 *pDataAdd, DSU_POWERLESS_ZONE_STRU *pPowerlessZoneStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pPowerlessZoneStru))
	{
		pData = pDataAdd;
		/*编号*/
		pPowerlessZoneStru->wId = ShortFromChar(pData);
		pData += 2u;
		/*中性区起点Link编号*/
		pPowerlessZoneStru->wOrgnLinkId = ShortFromChar(pData);
		pData += 2u;
		/*中性区起点Link偏移量*/
		pPowerlessZoneStru->dwOrgnLinkOfst = LongFromChar(pData);
		pData += 4u;
		/*中性区终点Link编号*/
		pPowerlessZoneStru->wTmnlLinkId = ShortFromChar(pData);
		pData += 2u;
		/*中性区终点Link偏移量*/
		pPowerlessZoneStru->dwTmnlLinkOfst = LongFromChar(pData);
		pData += 4u;
		/*防护方向*/
		pPowerlessZoneStru->wProtectDir = *pData;
		pData++;
		/*分相区通过入口速度（km/h）*/
		pPowerlessZoneStru->wEntrySpeed = ShortFromChar(pData);
		pData += 2u;
	}
	else
	{
		/* do nothing */
	}
}

/**********************************************************
函数功能：车档表信息数据结构体初始化函数
参数说明：[in] pDataAdd 数据地址（二进制文件）
[out]pCarStopStru被初始化的结构体指针
返回值：无
备注：依据互联互通数据结构V15.0.0添加，add by sds 20181031
***********************************************************/
void CARSTOP_STRU_init(UINT8 *pDataAdd, DSU_CARSTOP_STRU *pCarStopStru)
{
	UINT8 * pData = NULL;

	if ((NULL != pDataAdd) && (NULL != pCarStopStru))
	{
		pData = pDataAdd;
		/*编号*/
		pCarStopStru->carsId = ShortFromChar(pData);
		pData += 2u;

		/*所属Link编号*/
		pCarStopStru->carsLinkId = ShortFromChar(pData);
		pData += 2u;

		/*所属Link偏移量*/
		pCarStopStru->carsLinkOfst = LongFromChar(pData);
		pData += 4u;
	}
	else
	{
		/* do nothing */
	}
}

/**************************************************************************
* 功能描述:		疏散侧表信息数据结构体初始化函数
* 输入参数:		UINT8 *pDataAdd 数据地址（二进制文件）
* 输入输出参数:	无
* 输出参数:		DSU_EVACUATION_SIDE_STRU *pEvasideStru 被初始化的结构体指针
* 全局变量:		无
* 返回值:		无
* 修改记录:
* 1、增加疏散侧表结构体初始化 m by hzj 2023-3-1
**************************************************************************/
void EVASIDE_STRU_init(UINT8* pDataAdd, DSU_EVACUATION_SIDE_STRU* pEvasideStru)
{
	UINT8* pData = NULL;
	UINT16 loopIndex = 0U;

	/*空指针防护*/
	if ((NULL != pDataAdd) && (NULL != pEvasideStru))
	{
		pData = pDataAdd;

		/*索引编号*/
		pEvasideStru->wId = ShortFromChar(pData);
		pData += 2;

		/*解锁区域起点Link编号*/
		pEvasideStru->wOrgnLinkId = ShortFromChar(pData);
		pData += 2;

		/*所属Link偏移量*/
		pEvasideStru->dwOrgnLinkOfst = LongFromChar(pData);
		pData += 4;

		/*解锁区域起点 查询终点逻辑方向*/
		pEvasideStru->wOrgnLinkDir = *pData;
		pData++;

		/*解锁区域终点 Link编号*/
		pEvasideStru->wTmnlLinkId = ShortFromChar(pData);
		pData += 2;

		/*解锁区域终点Link偏移量*/
		pEvasideStru->dwTmnlLinkOfst = LongFromChar(pData);
		pData += 4;
			
		/*解锁区域终点查询起点逻辑方向*/
		pEvasideStru->wTmnlLinkDir = *pData;
		pData++;

		/*客室门紧急解锁开门方式*/
		pEvasideStru->wModeOfOpenDoor = *pData;
		pData++;

		/*解锁区域所属物理CI区域数量*/
		pEvasideStru->wCINum = *pData;
		pData++;

		/*解锁区域所属物理CI区域编号*/
		for (loopIndex = 0U; loopIndex < 2U; loopIndex++)
		{
			pEvasideStru->wCIId[loopIndex] = *pData;
			pData++;
		}

		/*疏散侧的上下行*/
		pEvasideStru->wLinkLine = *pData;
		pData++;

		/*解锁区域总长度*/
		pEvasideStru->wEvacuationLenth = LongFromChar(pData);
		pData += 4;

		/*起点到终点包含link数目（包含起点和终点）*/
		pEvasideStru->wLinkNum = ShortFromChar(pData);
		pData += 2;

		/*包含的link编号*/
		for (loopIndex = 0U; loopIndex < 20U; loopIndex++)
		{
			pEvasideStru->wLinkId[loopIndex] = ShortFromChar(pData);
			pData += 2;
		}		
	}
	else
	{
		/*空指针,不做任何处理*/
	}
}
