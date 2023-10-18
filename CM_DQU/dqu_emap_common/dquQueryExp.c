/************************************************************************
* 文件名    ：  dsuQuery.c
* 版权说明  ：  北京交控科技有限公司
* 版本号  	：  1.0
* 创建时间	：	2009.09.26
* 作者  		软件部
* 功能描述	：	dsu查询函数文件
* 使用注意	：
* 修改记录	：
************************************************************************/
#ifdef ZCAPP_VXWORKS_CODECONTROL
#include <vxWorks.h>
#endif /*End of ZCAPP_VXWORKS_CODECONTROL*/

#include "dquQueryExp.h"
#include "dsuVar.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "dquCbtcType.h"
#include "CommonMemory.h"
#include "dquEmapInit.h"


/************************************************************************
功能：判断两个带方向的位置是否同向
注意:    传入的A点位置的方向必须是指向B点处的,否则函数执行失败
入口参数：
	IN const LOD_STRU *pLocA, A点位置(它携带的方向必须是指向B点的)
	IN const LOD_STRU *pLocB, B点位置
	IN UINT8 PointNum,  道岔数目
	IN const DQU_POINT_STATUS_STRU *PointInfo  道岔状态信息
出口参数：无
返回值：0-失败(未知) 0x55-同向 0xAA-反向
修订记录:
1.修复QC1235 同一link上方向不一致时，返回方向不同的缺陷，songxy 20190314
2.修复集成测试缺陷Bug-1907,add by yhl,20200114
************************************************************************/
UINT8 dquIsDirSameExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const LOD_STRU *pLocA, IN const LOD_STRU *pLocB, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *PointInfo)
{
	UINT8 isSame = 0U; /* 方向是否相同 */
	UINT8 rslt = 0U; /* 临时变量 */
	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN]; /* 两点间link序列 */
	UINT16 wLinkNumber = 0U;/* 两点间link数目 */
	UINT32 dirChangeCount = 0U;/* 方向变化点个数 */
	DSU_LINK_STRU *pTmpLink = NULL;/* 临时变量:LINK结构体指针 */
	UINT16 i = 0U;/* 循环变量 */
	UINT8 searchDir = 0U; /* 当前搜索方向(由于从A向B遍历的过程中可能经历方向变化点,所以搜索方向可能改变) */
	LOD_STRU locStart = { 0U,0U,0U };

	(void)CommonMemSet(wLinkID, sizeof(wLinkID), 0U, sizeof(wLinkID));

	if ((NULL != pLocA) && (NULL != pLocB) && (NULL != PointInfo)
		&& (NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru)
		&& (NULL != pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex)
		&& (NULL != pDsuEmapStru->dsuStaticHeadStru) && (NULL != pDsuEmapStru->dsuStaticHeadStru->pLinkStru))
	{
		rslt = 1U;
	}
	else
	{
		rslt = 0U;
	}

	if (1U == rslt)
	{
		if ((pLocA->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (DSU_NULL_16 == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLocA->Lnk]))
		{
			rslt = 0U;
		}
		else
		{
			/*无操作*/
		}

		if ((pLocB->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (DSU_NULL_16 == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLocB->Lnk]))
		{
			rslt = 0U;
		}
		else
		{
			/*无操作*/
		}
	}
	else
	{
		/*无操作*/
	}

	if (1U == rslt)
	{
		if (pLocA->Lnk == pLocB->Lnk)
		{
			if (pLocA->Dir == pLocB->Dir)
			{
				isSame = DSU_TRUE;
			}
			else
			{
				/* 认为不存在，返回失败 */
				isSame = DSU_FALSE;
			}
		}
		else
		{
			locStart.Lnk = pLocA->Lnk;
			locStart.Off = pLocA->Off;
			locStart.Dir = pLocA->Dir;

			rslt = dsuGetLinkBtwLocExp(pDsuEmapStru, &locStart, pLocB, PointNum, PointInfo, wLinkID, &wLinkNumber);
			if (1U != rslt)
			{
				locStart.Dir = (UINT8)(~(locStart.Dir));
				rslt = dsuGetLinkBtwLocExp(pDsuEmapStru, &locStart, pLocB, PointNum, PointInfo, wLinkID, &wLinkNumber);
			}
			else
			{
				/*无操作*/
			}

			if (1U == rslt)
			{
				dirChangeCount = 0U;
				/* 起点link是否有方向变化点 */
				pTmpLink = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLocA->Lnk];
				searchDir = locStart.Dir;
				if (((EMAP_SAME_DIR == searchDir) && (0x55U == pTmpLink->wLogicDirChanged))
					|| ((EMAP_CONVER_DIR == searchDir) && (0xAAU == pTmpLink->wLogicDirChanged)))
				{
					/* 起点link只检查沿搜索方向一侧的端点是否为方向变化点 */
					dirChangeCount++;
					searchDir = (UINT8)(~searchDir);
				}
				else
				{
					/*无操作*/
				}

				for (i = 0U; i < wLinkNumber; i++)
				{
					pTmpLink = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[wLinkID[i]];
					if (((EMAP_SAME_DIR == searchDir) && (0x55U == pTmpLink->wLogicDirChanged))
						|| ((EMAP_CONVER_DIR == searchDir) && (0xAAU == pTmpLink->wLogicDirChanged)))
					{
						/* 由于同一个方向变化点会在相邻两个link上配两次,所以这里只统计搜索方向上的变化点 */
						dirChangeCount++;
						searchDir = (UINT8)(~searchDir);
					}
					else
					{
						/*无操作*/
					}
				}

				if (0U == dirChangeCount % 2U)
				{
					/* 中间有偶数个方向变化点 */
					if (pLocA->Dir == pLocB->Dir)
					{
						isSame = DSU_TRUE;
					}
					else
					{
						isSame = DSU_FALSE;
					}
				}
				else
				{
					/* 中间有奇数个方向变化点 */
					if (pLocA->Dir == pLocB->Dir)
					{
						isSame = DSU_FALSE;
					}
					else
					{
						isSame = DSU_TRUE;
					}
				}
			}
			else
			{
				/*无操作*/
			}
		}
	}
	else
	{
		/*无操作*/
	}

	if (1U != rslt)
	{
		isSame = 0U;
	}
	else
	{
		/*无操作*/
	}

	return isSame;
}

/*
* 功能描述： 利用运行方向信息以及道岔状态信息，查询得到当前link沿运行方向的相邻link。
* 输入参数： UINT16 LinkId, 当前Link编号
*            UCHAR Direction, 当前方向
*            UCHAR PointNum,  道岔信息，道岔个数
*            const POINT_STRU *pPointStru, 道岔信息
* 输出参数： UINT16 *pLinkId，当前Link的相邻Link编号
* 返回值：   1,查询成功
*            0,查询失败,输入数据无效，道岔失表
*		      	 0xFF,线路终点，未找到道岔
*
*/
UINT8 dsuGetAdjacentLinkIDExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkId, const UCHAR Direction, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, UINT16 *pLinkId)
{
	UINT8 chReturnValue = 0u;					/*用于函数返回值*/
	UINT16 CurrentLinkIndex = 0u;				/*当前Link Index*/
	DSU_LINK_STRU *pCurrentLinkStru = NULL;		/*当前Link,获取本地数据使用*/
	UINT16 AdjancentLinkIndex = 0u;				/*当前Link Index*/
	DSU_LINK_STRU *pAdjancentLinkStru = NULL;	/*当前Link,获取本地数据使用*/
	UINT8 FindFlag = 0u;						/*查找标记，用于记录查找是否成功*/
	UINT8 i = 0u;								/*用于循环*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pPointStru) && (NULL != pLinkId))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断输入LinkID的有效性*/
	if ((LinkId>LINKINDEXNUM) || (0xffffu == dsuLinkIndex[LinkId]))
	{
		/*输入LinkId无效，查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断输入方向有效性*/
	if ((EMAP_SAME_DIR != Direction) && (EMAP_CONVER_DIR != Direction))
	{
		/*输入方向无效，查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*到此表示输入数据有效*/

	/*根据LinkID获取当前link结构体*/
	CurrentLinkIndex = dsuLinkIndex[LinkId];
	pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

	/*按照输入方向来分情况处理*/
	if (EMAP_CONVER_DIR == Direction)
	{
		/*Link反向查找，按照当前Link始端端点情况分情况处理*/
		if (DSU_NULL_16 == pCurrentLinkStru->wOrgnJointMainLkId)
		{
			/*线路终点，当前link无相邻link，返回*/
			chReturnValue = 0xffu;
			return chReturnValue;

		}
		else if (EMAP_AXLE_DOT == pCurrentLinkStru->wOrgnPointType)
		{
			/*计轴点*/
			/*相邻正线Link即为所求*/
			*pLinkId = pCurrentLinkStru->wOrgnJointMainLkId;

			/*查询成功，返回查询结果*/
			chReturnValue = 1u;
			return chReturnValue;

		}
		else if (EMAP_POINT_DOT == pCurrentLinkStru->wOrgnPointType)
		{
			/*当前始端端点类型是道岔点*/
			/*判断当前link是否为道岔的汇合Link*/
			if (DSU_NULL_16 == pCurrentLinkStru->wOrgnJointSideLkId)
			{
				/*当前link非汇合Link，非混合link相邻Link只有一个，道岔状态也必须正确，以下验证道岔状态是否正确*/

				/*遍历道岔信息，查找当前道岔的状态信息*/
				FindFlag = 0u;
				for (i = 0u; i<PointNum; i++)
				{
					if (pPointStru[i].PointId == pCurrentLinkStru->wOrgnPointId)
					{
						/*说明已在输入道岔信息中查找到当前Link终端处的道岔*/

						/*置查找位为TRUR*/
						FindFlag = 1u;
						break;
					}
				}

				if (0u == FindFlag)
				{
					/*没有找到当前道岔信息，当前Link相邻Link可能处于MA范围之外，返回失败*/
					chReturnValue = 0xffu;
					return chReturnValue;
				}

				/*获取相邻正线Link信息,也就是获取相邻汇合Link信息*/
				AdjancentLinkIndex = dsuLinkIndex[pCurrentLinkStru->wOrgnJointMainLkId];
				pAdjancentLinkStru = dsuStaticHeadStru->pLinkStru + AdjancentLinkIndex;

				if (((EMAP_POINT_STATUS_MAIN == pPointStru[i].PointStatus) && (LinkId == pAdjancentLinkStru->wTmnlJointMainLkId))
					|| ((EMAP_POINT_STATUS_SIDE == pPointStru[i].PointStatus) && (LinkId == pAdjancentLinkStru->wTmnlJointSideLkId)))
				{
					/*道岔状态正确有两种情况：1.相邻道岔为定位，当前Link是相邻Link的正线相邻Iink。
					2.相邻道岔为反位，当前Link是相邻Link的侧线相邻Iink。
					*/
					*pLinkId = pCurrentLinkStru->wOrgnJointMainLkId;

					/*查询成功，返回查询结果*/
					chReturnValue = 1u;
					return chReturnValue;
				}
				else
				{
					/*已找到相邻道岔，但是相邻道岔的道岔状态不对或者道岔状态失表，发生严重错误，函数返回失败*/
					chReturnValue = 0u;
					return chReturnValue;
				}

			}
			else
			{
				/*当前link为道岔的汇合Link*/
				/*遍历道岔信息，查找当前道岔的状态信息*/
				FindFlag = 0u;
				for (i = 0u; i<PointNum; i++)
				{
					if (pPointStru[i].PointId == pCurrentLinkStru->wOrgnPointId)
					{
						/*说明已在输入道岔信息中查找到当前Link终端处的道岔*/

						/*置查找位为TRUR*/
						FindFlag = 1u;
						break;
					}
				}

				if (0u == FindFlag)
				{
					/*没有找到当前道岔信息，当前Link相邻Link可能处于MA范围之外，返回失败*/
					chReturnValue = 0xffu;
					return chReturnValue;
				}
				else
				{
					/*根据道岔的实时状态信息分类讨论*/
					if (EMAP_POINT_STATUS_MAIN == pPointStru[i].PointStatus)
					{
						/*道岔状态位于定位*/
						/*起点相邻正线Link即为所求*/
						*pLinkId = pCurrentLinkStru->wOrgnJointMainLkId;

						/*查询成功，返回查询结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else if (EMAP_POINT_STATUS_SIDE == pPointStru[i].PointStatus)
					{
						/*道岔状态位于反位*/
						/*起点相邻侧线Link即为所求*/
						*pLinkId = pCurrentLinkStru->wOrgnJointSideLkId;

						/*查询成功，返回查询结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*道岔失表， POINTLOSESTATUS 失表返回值的处理*/
						chReturnValue = 0u;
						return chReturnValue;
					}
				}
			}
		}
		else
		{
			/*Link端点类型不合法*/
			chReturnValue = 0u;
			return chReturnValue;
		}

	}
	else /*if(Direction==SAMEDIR)*/
	{
		/*正向于Link方向查找*/

		/*Link正向查找，按照当前Link终端端点情况分情况处理*/
		if (DSU_NULL_16 == pCurrentLinkStru->wTmnlJointMainLkId)
		{
			/*端点类型为线路终点，返回查询结果*/
			chReturnValue = 0xffu;
			return chReturnValue;

		}
		else if (EMAP_AXLE_DOT == pCurrentLinkStru->wTmnlPointType)
		{
			/*当前Link终端端点计轴点*/
			/*相邻正线Link即为所求*/

			*pLinkId = pCurrentLinkStru->wTmnlJointMainLkId;
			chReturnValue = 1;
			return chReturnValue;

		}
		else if (EMAP_POINT_DOT == pCurrentLinkStru->wTmnlPointType)
		{
			/*当前Link终端端点道岔点*/

			/*判断当前link是否为道岔的汇合Link*/
			if (DSU_NULL_16 == pCurrentLinkStru->wTmnlJointSideLkId)
			{
				/*当前link非汇合Link，非混合link相邻Link只有一个，道岔状态也必须正确，以下验证道岔状态是否正确*/

				/*遍历道岔信息，查找当前道岔的状态信息*/
 				FindFlag = 0u;
				for (i = 0u; i<PointNum; i++)
				{
					if (pPointStru[i].PointId == pCurrentLinkStru->wTmnlPointId)
					{
						/*说明已在输入道岔信息中查找到当前Link终端处的道岔*/

						/*置查找位为TRUR*/
						FindFlag = 1u;
						break;
					}
				}

				if (0u == FindFlag)
				{
					/*没有找到当前道岔信息，当前Link相邻Link可能处于MA范围之外，返回失败*/
					chReturnValue = 0xffu;
					return chReturnValue;
				}

				/*获取相邻正线Link信息,也就是获取相邻汇合Link信息*/
				AdjancentLinkIndex = dsuLinkIndex[pCurrentLinkStru->wTmnlJointMainLkId];
				pAdjancentLinkStru = dsuStaticHeadStru->pLinkStru + AdjancentLinkIndex;

				if (((EMAP_POINT_STATUS_MAIN == pPointStru[i].PointStatus) && (LinkId == pAdjancentLinkStru->wOrgnJointMainLkId))
					|| ((EMAP_POINT_STATUS_SIDE == pPointStru[i].PointStatus) && (LinkId == pAdjancentLinkStru->wOrgnJointSideLkId)))
				{
					/*道岔状态正确有两种情况：1.相邻道岔为定位，当前Link是相邻Link的正线相邻Iink。
					2.相邻道岔为反位，当前Link是相邻Link的侧线相邻Iink。
					*/
					*pLinkId = pCurrentLinkStru->wTmnlJointMainLkId;

					/*查询成功，返回查询结果*/
					chReturnValue = 1u;
					return chReturnValue;
				}
				else
				{
					/*已找到相邻道岔，但是相邻道岔的道岔状态不对或者道岔状态失表，发生严重错误，函数返回失败*/
					chReturnValue = 0u;
					return chReturnValue;
				}
			}
			else
			{
				/*当前汇合Link，按照道岔实时状态分类处理*/

				/*查找当前道岔信息*/
				FindFlag = 0u;
				for (i = 0u; i<PointNum; i++)
				{
					if (pPointStru[i].PointId == pCurrentLinkStru->wTmnlPointId)
					{
						/*说明已在输入道岔信息中查找到当前Link终端处的道岔*/
						FindFlag = 1u;
						break;
					}
				}

				if (0u == FindFlag)
				{
					/*没有找到当前道岔信息，当前Link相邻Link可能处于MA范围之外，返回失败*/
					chReturnValue = 0xffu;
					return chReturnValue;
				}
				else
				{
					/*根据道岔实时状态，分情况处理*/
					if (EMAP_POINT_STATUS_MAIN == pPointStru[i].PointStatus)
					{
						/*当前Link是汇合Link，道岔定位，终点相邻正线Link即为所求*/

						*pLinkId = pCurrentLinkStru->wTmnlJointMainLkId;

						/*查询成功，返回查询结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else if (EMAP_POINT_STATUS_SIDE == pPointStru[i].PointStatus)
					{
						/*当前Link是汇合Link，道岔反位，终点相邻侧线Link即为所求*/

						*pLinkId = pCurrentLinkStru->wTmnlJointSideLkId;

						/*查询成功，返回查询结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*当前Link是汇合Link，失表  POINTLOSESTATUS，返回失败*/
						chReturnValue = 0u;
						return chReturnValue;
					}
				}
			}
		}
		else
		{
			/*Link端点类型不合法，函数返回失败*/
			chReturnValue = 0u;
			return chReturnValue;
		}
	}

	/*此处不可达*/

}

/*
* 功能描述： 利用运行方向信息，得到当前link沿运行方向的相邻link组。
* 输入参数： UINT16 LinkId, 当前Link编号
*            UCHAR Direction, 当前方向
* 输出参数： UINT16 *pLinkId，当前Link的相邻Link编号数组
* 返回值：   1,查询成功
*            0,查询失败,输入数据无效，道岔失表
*		     0xFF,线路终点，未找到道岔
*修改：by yt 2021年9月11日 删除冗余变量
*/
UINT8 dsuGetAdjacentLinkIDArrayExp(const DSU_EMAP_STRU* pDsuEmapStru, const UINT16 LinkId, const UCHAR Direction, UINT16* pLinkId)
{
	UINT8 chReturnValue = 0u;						/*用于函数返回值*/
	UINT16 CurrentLinkIndex = 0u;					/*当前Link Index*/
	DSU_LINK_STRU* pCurrentLinkStru = NULL;			/*当前Link,获取本地数据使用*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU* dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pLinkId))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断输入LinkID的有效性*/
	if ((LinkId > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[LinkId]))
	{
		/*输入LinkId无效，查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断输入方向有效性*/
	if ((EMAP_SAME_DIR != Direction) && (EMAP_CONVER_DIR != Direction))
	{
		/*输入方向无效，查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*到此表示输入数据有效*/

	/*根据LinkID获取当前link结构体*/
	CurrentLinkIndex = dsuLinkIndex[LinkId];
	pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

	/*按照输入方向来分情况处理*/
	if (EMAP_CONVER_DIR == Direction)
	{
		/*Link反向查找，按照当前Link始端端点情况分情况处理*/
		if (DSU_NULL_16 == pCurrentLinkStru->wOrgnJointMainLkId)
		{
			/*线路终点，当前link无相邻link，返回*/
			chReturnValue = 0xffu;
			return chReturnValue;
		}

		/*相邻正线Link加进去*/
		pLinkId[0] = pCurrentLinkStru->wOrgnJointMainLkId;

		/*相邻侧线Link加进去*/
		if (DSU_NULL_16 != pCurrentLinkStru->wOrgnJointSideLkId)
		{
			pLinkId[1] = pCurrentLinkStru->wOrgnJointSideLkId;
		}
		/*查询成功，返回查询结果*/
		chReturnValue = 1u;
		return chReturnValue;
	}
	else /*if(Direction==SAMEDIR)*/
	{
		/*正向于Link方向查找*/

		/*Link正向查找，按照当前Link终端端点情况分情况处理*/
		if (DSU_NULL_16 == pCurrentLinkStru->wTmnlJointMainLkId)
		{
			/*端点类型为线路终点，返回查询结果*/
			chReturnValue = 0xffu;
			return chReturnValue;
		}

		/*相邻正线Link加进去*/
		pLinkId[0] = pCurrentLinkStru->wTmnlJointMainLkId;

		/*相邻侧线Link加进去*/
		if (DSU_NULL_16 != pCurrentLinkStru->wTmnlJointSideLkId)
		{
			pLinkId[1] = pCurrentLinkStru->wTmnlJointSideLkId;
		}
		chReturnValue = 1u;
		return chReturnValue;

	}

	/*此处不可达*/

}

/*
* 功能描述： 功能函数，根据当前位置，和位移以及输入的道岔信息，得到新位置.
如果一个点即当前Lnk起点，又是下一个Link终点，按照离当前点pLODStru最近的Link Id返回
* 输入参数： const LOD_STRU * pLODStru, 当前位置
*            INT32 Displacement,        位移，有符号，如果符号为负表示方向取反
*            UCHAR PointNum,            道岔数量
*            const POINT_STRU * pPointStru,  道岔信息
* 输出参数： LOD_STRU *pNewLODStru       新位置
* 返回值：   1,成功
*            0,失败
*注意：合库和燕房使用
*修改：yt 2021/09/01 修改白盒相关问题，条件语句宏值放左边
*/
static UCHAR dsuLODCalcuExp_DFT(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru)
{
#if 0
	UINT8 chReturnValue = 0u;				/*用于函数返回值*/
	UINT16 CurrentLinkId = 0u;				/*用于查找过程中临时保存当前LinkID*/
	UINT16 CurrentLinkIndex = 0u;			/*当前Link Index*/
	DSU_LINK_STRU *pCurrentLinkStru = NULL;	/*用于循环查找过程中临时保存当前Link结构体*/
	UINT8 FindDir = 0u;						/*用于记录实际操作过程中的查找方向*/
	INT32 FindDisplacement = 0;				/*临时保存偏移量*/
	INT32 FindDisplacementSum = 0;			/*用于累加位移偏移量*/
	UINT16 k = 0u;							/*循环用变量*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStru) && (NULL != pPointStru) && (NULL != pNewLODStru))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断输入LinkID的有效性*/
	if ((pLODStru->Lnk>LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pLODStru->Lnk]))
	{
		/*输入LinkId无效，查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断入口数据偏移量的有效性*/
	if (pLODStru->Off > LinkLengthExp(pDsuEmapStru, pLODStru->Lnk))
	{
		/*数据无效，查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断输入方向有效性*/
	if ((EMAP_SAME_DIR != pLODStru->Dir) && (EMAP_CONVER_DIR != pLODStru->Dir))
	{
		/*输入方向无效，查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*特殊情况处理，偏移量Displacement为0*/
	if (0 == Displacement)
	{
		/*displacement如果为0，返回当前位置为所求*/
		pNewLODStru->Lnk = pLODStru->Lnk;
		pNewLODStru->Off = pLODStru->Off;
		pNewLODStru->Dir = pLODStru->Dir;

		/*查询成功，返回查询结果*/
		chReturnValue = 1u;
		return chReturnValue;
	}

	/*如果displacement小于0，则按照displacement绝对值来查找新位置，方向取反*/
	if ((EMAP_SAME_DIR == pLODStru->Dir) && (0 < Displacement))
	{
		/*正向查找*/
		FindDir = EMAP_SAME_DIR;
		FindDisplacement = Displacement;
	}
	else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (0 > Displacement))
	{
		/*正向查找*/
		FindDir = EMAP_SAME_DIR;
		FindDisplacement = -Displacement;
	}
	else if ((EMAP_SAME_DIR == pLODStru->Dir) && (0 > Displacement))
	{
		/*反向查找*/
		FindDir = EMAP_CONVER_DIR;
		FindDisplacement = -Displacement;
	}
	else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (0 < Displacement))
	{
		/*反向查找*/
		FindDir = EMAP_CONVER_DIR;
		FindDisplacement = Displacement;
	}
	else
	{
		/*不可达分支*/
	}

	/*根据LinkID从数据库中查找当前link信息*/
	CurrentLinkIndex = dsuLinkIndex[pLODStru->Lnk];
	pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;
	if (EMAP_SAME_DIR == FindDir)
	{
		/*正向查找*/

		/*根据当前位置正向累加偏移量*/
		FindDisplacementSum = pCurrentLinkStru->dwLength - pLODStru->Off;

		/*根据位移判断新位置是否在同一个Link上*/
		if (FindDisplacementSum >= FindDisplacement)
		{
			/*说明在同一个link上，累加位移得到新位置*/
			pNewLODStru->Lnk = pCurrentLinkStru->wId;
			pNewLODStru->Off = pLODStru->Off + FindDisplacement;
			pNewLODStru->Dir = pLODStru->Dir;
			chReturnValue = 1u;
			return chReturnValue;
		}
		else
		{
			for (k = 0u; k<LINKINDEXNUM; k++)
			{
				/*此处循环并不是为了要遍历每一个Link，而是对循环计数，不能超过LINKINDEXNUM*/
				if (1u == dsuGetAdjacentLinkIDExp(pDsuEmapStru, pCurrentLinkStru->wId, FindDir, PointNum, pPointStru, &CurrentLinkId))
				{
					/*pCurrentLinkStru指向相邻Link,相邻Link变为当前Link*/
					CurrentLinkIndex = dsuLinkIndex[CurrentLinkId];
					pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

					FindDisplacementSum += pCurrentLinkStru->dwLength;
					if (FindDisplacementSum >= FindDisplacement)
					{
						/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置当前link上*/
						pNewLODStru->Lnk = pCurrentLinkStru->wId;
						pNewLODStru->Off = FindDisplacement - (FindDisplacementSum - pCurrentLinkStru->dwLength);
						pNewLODStru->Dir = pLODStru->Dir;
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
					}
				}
				else
				{
					/*获取下一个link失败*/
					chReturnValue = 0u;
					return chReturnValue;
				}
			}

			/*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
			chReturnValue = 0u;
			return chReturnValue;
		}
	}
	else
	{
		/*反向查找*/

		/*累加偏移量置0*/
		FindDisplacementSum = 0;

		/*根据当前位置反向累加偏移量*/
		FindDisplacementSum = pLODStru->Off;

		if (FindDisplacementSum >= FindDisplacement)
		{
			/*说明在同一个link上*/
			pNewLODStru->Lnk = pCurrentLinkStru->wId;
			pNewLODStru->Off = pLODStru->Off - FindDisplacement;
			pNewLODStru->Dir = pLODStru->Dir;

			/*查询成功，返回查询结果*/
			chReturnValue = 1u;
			return chReturnValue;
		}
		else
		{
			for (k = 0u; k<LINKINDEXNUM; k++)
			{
				/*此处循环并不是为了要遍历每一个Link，而是对循环计数，不能超过LINKINDEXNUM*/
				if (1u == dsuGetAdjacentLinkIDExp(pDsuEmapStru, pCurrentLinkStru->wId, FindDir, PointNum, pPointStru, &CurrentLinkId))
				{
					/*pCurrentLinkStru指向相邻Link*/
					CurrentLinkIndex = dsuLinkIndex[CurrentLinkId];
					pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

					FindDisplacementSum += pCurrentLinkStru->dwLength;
					if (FindDisplacementSum >= FindDisplacement)
					{
						/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置当前link上*/
						pNewLODStru->Lnk = pCurrentLinkStru->wId;
						pNewLODStru->Off = (FindDisplacementSum - FindDisplacement);
						pNewLODStru->Dir = pLODStru->Dir;
						/*查询成功，输出查询结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
					}
				}
				else
				{
					/*获取下一个link失败*/
					chReturnValue = 0u;
					return chReturnValue;
				}
			}

			/*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
			chReturnValue = 0;
			return chReturnValue;
		}
	}
#else
	return 0U;
#endif
}

/*
* 功能描述： 功能函数，根据当前位置，和位移以及输入的道岔信息，得到新位置.
如果一个点即当前Lnk起点，又是下一个Link终点，按照离当前点pLODStru最近的Link Id返回
* 输入参数： const LOD_STRU * pLODStru, 当前位置
*            INT32 Displacement,        位移，有符号，如果符号为负表示方向取反
*            UCHAR PointNum,            道岔数量
*            const POINT_STRU * pPointStru,  道岔信息
* 输出参数： LOD_STRU *pNewLODStru       新位置
* 返回值：   1,成功
*            0,失败
*author:qxt 20170811 互联互通增加link逻辑方向变化点
*/
static UCHAR dsuLODCalcuExp_HLT(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru)
{
	UINT8 chReturnValue = 0u;				/*用于函数返回值*/
	UINT8 chFuncReturnValue = 0u;			/*用于函数返回*/
	UINT8 chFlag = 1u;						/*函数执行失败标记*/
	UINT16 wCurrentLinkId = 0u;				/*用于查找过程中临时保存当前LinkID*/
	UINT16 wCurrentLinkIndex = 0u;			/*当前Link Index*/
	DSU_LINK_STRU *pCurrentLinkStru = NULL;	/*用于循环查找过程中临时保存当前Link结构体*/
	UINT8 wFindDir = 0u;					/*用于记录实际操作过程中的查找方向*/
	INT32 dwFindDisplacement = 0;			/*临时保存偏移量*/
	INT32 dwFindDisplacementSum = 0;		/*用于累加位移偏移量*/
	UINT16 k = 0u;							/*循环用变量*/
	UINT8 tmpLODDir = 0xffu;				/*临时变量,保存入口方向信息*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != pDsuEmapStru->dsuStaticHeadStru)
		&& (NULL != pLODStru) && (NULL != pPointStru) && (NULL != pNewLODStru))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断输入LinkID的有效性*/
		if ((pLODStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODStru->Lnk]))
		{
			/*输入LinkId无效，查询失败，返回*/
			chFlag = 0u;
		}

		/*判断入口数据偏移量的有效性*/
		if (pLODStru->Off > LinkLengthExp(pDsuEmapStru, pLODStru->Lnk))
		{
			/*数据无效，查询失败，返回*/
			chFlag = 0u;
		}
		/*判断输入方向有效性*/
		if ((EMAP_SAME_DIR != pLODStru->Dir) && (EMAP_CONVER_DIR != pLODStru->Dir))
		{
			/*输入方向无效，查询失败，返回*/
			chFlag = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		chFlag = 0u;

	}

	if (0u != chFlag)
	{
		/*特殊情况处理，偏移量Displacement为0*/
		if (0 == Displacement)
		{
			/*displacement如果为0，返回当前位置为所求*/
			pNewLODStru->Lnk = pLODStru->Lnk;
			pNewLODStru->Off = pLODStru->Off;
			pNewLODStru->Dir = pLODStru->Dir;

			/*查询成功，返回查询结果*/
			chReturnValue = 1u;
		}
		else/*0!=Displacement*/
		{
			tmpLODDir = pLODStru->Dir;

			/*如果displacement小于0，则按照displacement绝对值来查找新位置，方向取反*/
			if ((EMAP_SAME_DIR == pLODStru->Dir) && (Displacement>0))
			{
				/*正向查找*/
				wFindDir = EMAP_SAME_DIR;
				dwFindDisplacement = Displacement;
			}
			else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (Displacement<0))
			{
				/*正向查找*/
				wFindDir = EMAP_SAME_DIR;
				dwFindDisplacement = -Displacement;
			}
			else if ((EMAP_SAME_DIR == pLODStru->Dir) && (Displacement<0))
			{
				/*反向查找*/
				wFindDir = EMAP_CONVER_DIR;
				dwFindDisplacement = -Displacement;
			}
			else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (Displacement>0))
			{
				/*反向查找*/
				wFindDir = EMAP_CONVER_DIR;
				dwFindDisplacement = Displacement;
			}
			else
			{
				/*不可达分支*/
			}

			/*根据LinkID从数据库中查找当前link信息*/
			wCurrentLinkIndex = dsuLinkIndex[pLODStru->Lnk];
			pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;

			/*查询的新位置在查询起点所在的Link的情况*/
			if (EMAP_SAME_DIR == wFindDir)
			{
				/*正向查找*/

				/*根据当前位置正向累加偏移量*/
				dwFindDisplacementSum = pCurrentLinkStru->dwLength - pLODStru->Off;

				/*根据位移判断新位置是否在同一个Link上*/
				if (dwFindDisplacementSum >= dwFindDisplacement)
				{
					/*说明在同一个link上，累加位移得到新位置*/
					pNewLODStru->Lnk = pCurrentLinkStru->wId;
					pNewLODStru->Off = pLODStru->Off + dwFindDisplacement;
					pNewLODStru->Dir = pLODStru->Dir;
					chReturnValue = 1u;
				}
				else
				{
					/*不作处理*/
				}
			}
			else/*FindDir=EMAP_CONVER_DIR*/
			{
				/*反向查找*/

				/*累加偏移量置0*/
				dwFindDisplacementSum = 0;

				/*根据当前位置反向累加偏移量*/
				dwFindDisplacementSum = pLODStru->Off;

				if (dwFindDisplacementSum >= dwFindDisplacement)
				{
					/*说明在同一个link上*/
					pNewLODStru->Lnk = pCurrentLinkStru->wId;
					pNewLODStru->Off = pLODStru->Off - dwFindDisplacement;
					pNewLODStru->Dir = pLODStru->Dir;

					/*查询成功，返回查询结果*/
					chReturnValue = 1u;
				}
				else
				{
					/*不作处理*/
				}
			}

			/*查询的新位置与查询起点不在同一个Link*/
			if (1u != chReturnValue)
			{
				/*此处循环并不是为了要遍历每一个Link，而是对循环计数，不能超过LINKINDEXNUM*/
				for (k = 0u; k<LINKINDEXNUM; k++)
				{
					chFuncReturnValue = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pCurrentLinkStru->wId, wFindDir, PointNum, pPointStru, &wCurrentLinkId);
					if (1u == chFuncReturnValue)
					{
						/*判断Link是否为逻辑方向变化点(查询方向为正且该link的终点为逻辑方向变化点 或 查询反方向且该link的起点为逻辑方向变化)*/
						if (((EMAP_SAME_DIR == wFindDir) && (0x55u == pCurrentLinkStru->wLogicDirChanged)) ||
							((EMAP_CONVER_DIR == wFindDir) && (0xaau == pCurrentLinkStru->wLogicDirChanged)))
						{
							wFindDir = ~wFindDir;/*有变化点，改变查找方向*/
							tmpLODDir = ~tmpLODDir;/*有变化点,改变输出位置逻辑方向*/
						}

						/*pCurrentLinkStru指向相邻Link,相邻Link变为当前Link*/
						wCurrentLinkIndex = dsuLinkIndex[wCurrentLinkId];
						pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;

						dwFindDisplacementSum += pCurrentLinkStru->dwLength;
						if (dwFindDisplacementSum >= dwFindDisplacement)/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置当前link上*/
						{

							/*最终点所在位置为查找方向反向*/
							if (EMAP_CONVER_DIR == wFindDir)
							{
								pNewLODStru->Lnk = pCurrentLinkStru->wId;
								pNewLODStru->Off = dwFindDisplacementSum - dwFindDisplacement;
								pNewLODStru->Dir = tmpLODDir;
							}
							/*最终点所在位置为查找方向正向 */
							else
							{

								pNewLODStru->Lnk = pCurrentLinkStru->wId;
								pNewLODStru->Off = dwFindDisplacement - (dwFindDisplacementSum - pCurrentLinkStru->dwLength);
								pNewLODStru->Dir = tmpLODDir;
							}
							chReturnValue = 1u;
							break;
						}
						else
						{
							/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
						}
					}
					else
					{
						/*获取下一个link失败*/
						chReturnValue = 0u;
						break;
					}
				}/*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
			}

		}/*(0!=Displacement) end*/

	}/*if (0!=chFlag) end */

	return chReturnValue;

}

/*访问接口*/
UINT8 dsuLODCalcuExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru)
{
	UINT8 rtnValue = 0u;/*函数返回值*/
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStru) && (NULL != pPointStru) && (NULL != pNewLODStru))
	{
		cbtcVersionType = GetCbtcSysType();

		if ((DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			rtnValue = dsuLODCalcuExp_HLT(pDsuEmapStru, pLODStru, Displacement, PointNum, pPointStru, pNewLODStru);
		}
		else if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType))
		{
			rtnValue = dsuLODCalcuExp_DFT(pDsuEmapStru, pLODStru, Displacement, PointNum, pPointStru, pNewLODStru);
		}
		else
		{
			rtnValue = 0u;

		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/*
* 功能描述： 功能函数，根据当前位置，和位移以及输入的道岔信息，得到新位置.
如果一个点即当前Lnk起点，又是下一个Link终点，按照离当前点pLODStru最近的Link Id返回
* 输入参数： const LOD_STRU * pLODStru, 当前位置
*            INT32 Displacement,        位移，有符号，如果符号为负表示方向取反
*            UCHAR PointNum,            道岔数量
*            const POINT_STRU * pPointStru,  道岔信息
* 输出参数： LOD_STRU *pNewLODStru       新位置
* 返回值：   1,成功
*            0,失败
* 记录:		 1.封装dsuLODCalcuExp_HLT,查找到线路尽头时返回ff，输出新位置
*/
UINT8 dsuLODCalcuNewExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru)
{
	UINT8 chReturnValue = 0u;				/*用于函数返回值*/
	UINT8 chFuncReturnValue = 0u;			/*用于函数返回*/
	UINT8 chFlag = 1u;						/*函数执行失败标记*/
	UINT16 wCurrentLinkId = 0u;				/*用于查找过程中临时保存当前LinkID*/
	UINT16 wCurrentLinkIndex = 0u;			/*当前Link Index*/
	DSU_LINK_STRU *pCurrentLinkStru = NULL;	/*用于循环查找过程中临时保存当前Link结构体*/
	UINT8 wFindDir = 0u;					/*用于记录实际操作过程中的查找方向*/
	INT32 dwFindDisplacement = 0;			/*临时保存偏移量*/
	INT32 dwFindDisplacementSum = 0;		/*用于累加位移偏移量*/
	UINT16 k = 0u;							/*循环用变量*/
	UINT8 tmpLODDir = 0xffu;				/*临时变量,保存入口方向信息*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != pDsuEmapStru->dsuStaticHeadStru)
		&& (NULL != pLODStru) && (NULL != pPointStru) && (NULL != pNewLODStru))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断输入LinkID的有效性*/
		if ((pLODStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODStru->Lnk]))
		{
			/*输入LinkId无效，查询失败，返回*/
			chFlag = 0u;
		}

		/*判断入口数据偏移量的有效性*/
		if (pLODStru->Off > LinkLengthExp(pDsuEmapStru, pLODStru->Lnk))
		{
			/*数据无效，查询失败，返回*/
			chFlag = 0u;
		}
		/*判断输入方向有效性*/
		if ((EMAP_SAME_DIR != pLODStru->Dir) && (EMAP_CONVER_DIR != pLODStru->Dir))
		{
			/*输入方向无效，查询失败，返回*/
			chFlag = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		chFlag = 0u;

	}

	if (0u != chFlag)
	{
		/*特殊情况处理，偏移量Displacement为0*/
		if (0 == Displacement)
		{
			/*displacement如果为0，返回当前位置为所求*/
			pNewLODStru->Lnk = pLODStru->Lnk;
			pNewLODStru->Off = pLODStru->Off;
			pNewLODStru->Dir = pLODStru->Dir;

			/*查询成功，返回查询结果*/
			chReturnValue = 1u;
		}
		else/*0!=Displacement*/
		{
			tmpLODDir = pLODStru->Dir;

			/*如果displacement小于0，则按照displacement绝对值来查找新位置，方向取反*/
			/*修复编码规范问题，常量在前。wyd 20220910*/
			if ((EMAP_SAME_DIR == pLODStru->Dir) && (0 < Displacement))
			{
				/*正向查找*/
				wFindDir = EMAP_SAME_DIR;
				dwFindDisplacement = Displacement;
			}
			else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (0 > Displacement))
			{
				/*正向查找*/
				wFindDir = EMAP_SAME_DIR;
				dwFindDisplacement = -Displacement;
			}
			else if ((EMAP_SAME_DIR == pLODStru->Dir) && (0 > Displacement))
			{
				/*反向查找*/
				wFindDir = EMAP_CONVER_DIR;
				dwFindDisplacement = -Displacement;
			}
			else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (0 < Displacement))
			{
				/*反向查找*/
				wFindDir = EMAP_CONVER_DIR;
				dwFindDisplacement = Displacement;
			}
			else
			{
				/*不可达分支*/
			}

			/*根据LinkID从数据库中查找当前link信息*/
			wCurrentLinkIndex = dsuLinkIndex[pLODStru->Lnk];
			pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;

			/*查询的新位置在查询起点所在的Link的情况*/
			if (EMAP_SAME_DIR == wFindDir)
			{
				/*正向查找*/

				/*根据当前位置正向累加偏移量*/
				dwFindDisplacementSum = pCurrentLinkStru->dwLength - pLODStru->Off;

				/*根据位移判断新位置是否在同一个Link上*/
				if (dwFindDisplacementSum >= dwFindDisplacement)
				{
					/*说明在同一个link上，累加位移得到新位置*/
					pNewLODStru->Lnk = pCurrentLinkStru->wId;
					pNewLODStru->Off = pLODStru->Off + dwFindDisplacement;
					pNewLODStru->Dir = pLODStru->Dir;
					chReturnValue = 1u;
				}
				else
				{
					/*不作处理*/
				}
			}
			else/*FindDir=EMAP_CONVER_DIR*/
			{
				/*反向查找*/

				/*累加偏移量置0*/
				dwFindDisplacementSum = 0;

				/*根据当前位置反向累加偏移量*/
				dwFindDisplacementSum = pLODStru->Off;

				if (dwFindDisplacementSum >= dwFindDisplacement)
				{
					/*说明在同一个link上*/
					pNewLODStru->Lnk = pCurrentLinkStru->wId;
					pNewLODStru->Off = pLODStru->Off - dwFindDisplacement;
					pNewLODStru->Dir = pLODStru->Dir;

					/*查询成功，返回查询结果*/
					chReturnValue = 1u;
				}
				else
				{
					/*不作处理*/
				}
			}

			/*查询的新位置与查询起点不在同一个Link*/
			if (1u != chReturnValue)
			{
				/*此处循环并不是为了要遍历每一个Link，而是对循环计数，不能超过LINKINDEXNUM*/
				for (k = 0u; k<LINKINDEXNUM; k++)
				{
					chFuncReturnValue = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pCurrentLinkStru->wId, wFindDir, PointNum, pPointStru, &wCurrentLinkId);
					if (1u == chFuncReturnValue)
					{
						/*判断Link是否为逻辑方向变化点(查询方向为正且该link的终点为逻辑方向变化点 或 查询反方向且该link的起点为逻辑方向变化)*/
						if (((EMAP_SAME_DIR == wFindDir) && (0x55u == pCurrentLinkStru->wLogicDirChanged)) ||
							((EMAP_CONVER_DIR == wFindDir) && (0xaau == pCurrentLinkStru->wLogicDirChanged)))
						{
							wFindDir = ~wFindDir;/*有变化点，改变查找方向*/
							tmpLODDir = ~tmpLODDir;/*有变化点,改变输出位置逻辑方向*/
						}

						/*pCurrentLinkStru指向相邻Link,相邻Link变为当前Link*/
						wCurrentLinkIndex = dsuLinkIndex[wCurrentLinkId];
						pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;

						dwFindDisplacementSum += pCurrentLinkStru->dwLength;
						if (dwFindDisplacementSum >= dwFindDisplacement)/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置当前link上*/
						{

							/*最终点所在位置为查找方向反向*/
							if (EMAP_CONVER_DIR == wFindDir)
							{
								pNewLODStru->Lnk = pCurrentLinkStru->wId;
								pNewLODStru->Off = dwFindDisplacementSum - dwFindDisplacement;
								pNewLODStru->Dir = tmpLODDir;
							}
							/*最终点所在位置为查找方向正向 */
							else
							{

								pNewLODStru->Lnk = pCurrentLinkStru->wId;
								pNewLODStru->Off = dwFindDisplacement - (dwFindDisplacementSum - pCurrentLinkStru->dwLength);
								pNewLODStru->Dir = tmpLODDir;
							}
							chReturnValue = 1u;
							break;
						}
						else
						{
							/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
						}
					}
					else if (0xffu == chFuncReturnValue)
					{
						chReturnValue = 0xffu;
						if (EMAP_CONVER_DIR == wFindDir)
						{
							pNewLODStru->Lnk = pCurrentLinkStru->wId;
							pNewLODStru->Off = 0u;
							pNewLODStru->Dir = pLODStru->Dir;
						}
						else
						{
							pNewLODStru->Lnk = pCurrentLinkStru->wId;
							pNewLODStru->Off = LinkLengthExp(pDsuEmapStru, pCurrentLinkStru->wId);
							pNewLODStru->Dir = pLODStru->Dir;
						}
						break;
					}
					else
					{
						/*获取下一个link失败*/
						chReturnValue = 0u;
						break;
					}
				}/*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
			}

		}/*(0!=Displacement) end*/

	}/*if (0!=chFlag) end */

	return chReturnValue;
}

/*
* 功能描述： 由A，B两者位置和B点方向得到两者位移。
* 输入参数： const LOD_STRU * pLODAStru, A点位置
*            const LOD_STRU * pLODBStru, B点位置
*            UINT8 PointNum,             道岔数量
*            const POINT_STRU * pPointStru, 道岔信息结构体
* 输出参数： INT32 *Displacement，       AB位移，是有符号数。符号表示方向，
AB位移以LinkB方向为参照。AB方向与LinkB方向相同，位移值取正，否则位移值取负。
* 返回值：   1,成功
*            0,失败
* 使用注意：注意此处只按照B的方向进行查找，未关注A点的方向。（合库和燕房项目使用）
*/
static UINT8 dsuDispCalcuByLOCExp_DFT(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODAStru, const LOD_STRU *pLODBStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru, INT32 *Displacement)
{
#if 0
	UINT8 chReturnValue = 0u;					/*用于函数返回值*/
	UINT8 chFunRtn = 0u;						/*调用函数返回值*/
	UINT16 CurrentLinkId = 0u;					/*用于查找过程中临时保存当前LinkID*/
	UINT16 CurrentLinkIndex = 0u;				/*查找过程中记录Link Index临时变量*/

	UINT8 AFindFlag = 2u;						/*0，查找失败。1，查找成功。2,继续查找。*/
	UINT8 BFindFlag = 2u;						/*0，查找失败。1，查找成功。2,继续查找。*/
	INT32 AFindDisplacementSum = 0;				/*A查找B过程中的位移累加值*/
	INT32 BFindDisplacementSum = 0;				/*B查找A过程中位移累加值*/
	DSU_LINK_STRU *pACurrentLinkStru = NULL;    /*A当前Link*/
	DSU_LINK_STRU *pBCurrentLinkStru = NULL;    /*B当前Link*/
	UINT16 k = 0u;								/*循环用变量*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pLODAStru) && (NULL != pPointStru) && (NULL != pLODBStru) && (NULL != Displacement))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*预置函数返回值为0*/
	*Displacement = 0;

	/*判断输入A LinkID的有效性*/
	if ((pLODAStru->Lnk > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pLODAStru->Lnk]))
	{
		/*输入LinkId无效,查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断输入B LinkID的有效性*/
	if ((pLODBStru->Lnk > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pLODBStru->Lnk]))
	{
		/*输入LinkId无效,查询失败，返回*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断入口数据linkA偏移量的有效性*/
	if (pLODAStru->Off > LinkLengthExp(pDsuEmapStru, pLODAStru->Lnk))
	{
		/*入口数据异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断入口数据linkB偏移量的有效性*/
	if (pLODBStru->Off > LinkLengthExp(pDsuEmapStru, pLODBStru->Lnk))
	{
		/*入口数据异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断linkB运行方向的正确性*/
	if ((EMAP_SAME_DIR != pLODBStru->Dir) && (EMAP_CONVER_DIR != pLODBStru->Dir))
	{
		/*入口数据异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*特殊情况处理，LinkA与LinkB处于同一Link上*/
	if (pLODAStru->Lnk == pLODBStru->Lnk)
	{
		/*A,B位于同一link，计算两点间距离信息*/
		*Displacement = pLODBStru->Off - pLODAStru->Off;

		if (EMAP_SAME_DIR == pLODBStru->Dir)
		{
			/**/
		}
		else
		{
			/*方向相反，转换位移符号*/
			*Displacement = -(*Displacement);
		}

		/*函数查询成功，返回*/
		chReturnValue = 1u;
		return 1u;
	}


	/*根据LinkID获取当前link*/
	CurrentLinkIndex = dsuLinkIndex[pLODAStru->Lnk];
	pACurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;
	CurrentLinkIndex = dsuLinkIndex[pLODBStru->Lnk];
	pBCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

	/*按照B的方向不同进行分类*/
	/*查找基本方法为A和B各自按照B的方向向前查找，如果找到对方，记录查找到的位移，返回结果*/
	if (pLODBStru->Dir == EMAP_SAME_DIR)
	{
		/*沿正向查找*/
		/*累加位移值*/
		AFindDisplacementSum = pACurrentLinkStru->dwLength - pLODAStru->Off;
		BFindDisplacementSum = pBCurrentLinkStru->dwLength - pLODBStru->Off;
		for (k = 0u; k < LINKINDEXNUM; k++)
		{
			/*此处循环是为了计数，循环次数不能超过LINKINDEXNUM*/
			if (2u == AFindFlag)
			{
				/*A向前查找一个link*/
				chFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pACurrentLinkStru->wId, EMAP_SAME_DIR, PointNum, pPointStru, &CurrentLinkId);
				if (1u == chFunRtn)
				{
					/*pACurrentLinkStru指向相邻Link*/
					CurrentLinkIndex = dsuLinkIndex[CurrentLinkId];
					pACurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

					if (pACurrentLinkStru->wId == pLODBStru->Lnk)
					{
						/*A找到了B*/

						/*查找成功*/
						AFindFlag = 1u;
						/*累加偏移量，计算结果值*/
						AFindDisplacementSum += (pLODBStru->Off);
						/*此时A沿正向找到了B，偏移量为正*/
						*Displacement = AFindDisplacementSum;

						/*查找成功，返回结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*A没有找到B，累加偏移量，继续查找*/
						AFindDisplacementSum += pACurrentLinkStru->dwLength;
					}
				}
				else if (0xffu == chFunRtn)
				{
					/*线路终点或者道岔未找到，A点停止查找*/
					AFindFlag = 0u;
				}
				else
				{
					/*A获取下一个link失败,Link失表或者输入错误*/
					chReturnValue = 0u;
					return chReturnValue;
				}
			}

			/*B向前查找一个link*/
			if (2u == BFindFlag) 
			{
				chFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pBCurrentLinkStru->wId, EMAP_SAME_DIR, PointNum, pPointStru, &CurrentLinkId);
				if (1u == chFunRtn)
				{
					/*pBCurrentLinkStru指向相邻Link*/
					CurrentLinkIndex = dsuLinkIndex[CurrentLinkId];
					pBCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

					if (pBCurrentLinkStru->wId == pLODAStru->Lnk)
					{
						/*B找到了A*/
						BFindFlag = 1u;

						/*累加偏移量，计算结果值*/
						BFindDisplacementSum += (pLODAStru->Off);

						/*此时B沿正向找到了A，偏移量为负*/
						*Displacement = -BFindDisplacementSum;

						/*查找成功，返回结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*如果B没有找到A，累加偏移量，继续查找*/
						BFindDisplacementSum += pBCurrentLinkStru->dwLength;
					}
				}
				else if (0xffu == chFunRtn)
				{
					/*线路终点或者道岔未找到，停止查找*/
					BFindFlag = 0u;
				}
				else
				{
					/*B获取下一个link失败,Link失表或者输入错误*/
					chReturnValue = 0u;
					return chReturnValue;
				}
			}

			if ((0u == AFindFlag) && (0u == BFindFlag))
			{
				/*A，B均结束查找并且没有找到对方，函数返回失败*/
				chReturnValue = 0u;
				return chReturnValue;
			}
			else
			{
				/*此时AFindFlag，BFindFlag至少有一个为2，继续查找*/
			}
		}

		/*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
		chReturnValue = 0u;
		return chReturnValue;

	}
	else /*if (pLODBStru->Dir==CONVERDIR)*/
	{
		/*A,B均沿反向向查找*/
		AFindDisplacementSum = pLODAStru->Off;
		BFindDisplacementSum = pLODBStru->Off;
		for (k = 0u; k < LINKINDEXNUM; k++)
		{
			/*此处循环是为了计数，循环次数不能超过LINKINDEXNUM*/
			if (2u == AFindFlag)
			{
				/*A向前查找一个link*/
				chFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pACurrentLinkStru->wId, EMAP_CONVER_DIR, PointNum, pPointStru, &CurrentLinkId);
				if (1u == chFunRtn)
				{
					/*pACurrentLinkStru指向相邻Link*/
					CurrentLinkIndex = dsuLinkIndex[CurrentLinkId];
					pACurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

					if (pACurrentLinkStru->wId == pLODBStru->Lnk)
					{
						/*A找到了B*/

						/*查找成功*/
						AFindFlag = 1u;
						AFindDisplacementSum += (pACurrentLinkStru->dwLength - pLODBStru->Off);

						/*此时A沿反向找到了B，偏移量为正*/
						*Displacement = AFindDisplacementSum;

						/*查找成功，返回结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*如果A没有找到B，累加偏移量，继续查找*/
						AFindDisplacementSum += pACurrentLinkStru->dwLength;
					}
				}
				else if (0xffu == chFunRtn)
				{
					/*线路终点或者道岔未找到，停止查找*/
					AFindFlag = 0u;
				}
				else/*( 0==chReturnValue )*/
				{
					/*A获取下一个link失败,Link失表或者输入错误*/
					chReturnValue = 0u;
					return chReturnValue;
				}
			}

			/*B向前查找一个link*/
			if (2u == BFindFlag)
			{
				chFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pBCurrentLinkStru->wId, EMAP_CONVER_DIR, PointNum, pPointStru, &CurrentLinkId);
				if (1u == chFunRtn)
				{
					/*pACurrentLinkStru指向相邻Link*/
					CurrentLinkIndex = dsuLinkIndex[CurrentLinkId];
					pBCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

					if (pBCurrentLinkStru->wId == pLODAStru->Lnk)
					{
						/*B找到了A*/

						/*查找成功*/
						BFindFlag = 1u;
						BFindDisplacementSum += (pBCurrentLinkStru->dwLength - pLODAStru->Off);

						/*此时B沿反向找到了A，偏移量为负*/
						*Displacement = -BFindDisplacementSum;

						/*查找成功，返回结果*/
						chReturnValue = 1u;
						return chReturnValue;
					}
					else
					{
						/*如果B没有找到A，累加偏移量，继续查找*/
						BFindDisplacementSum += pBCurrentLinkStru->dwLength;
					}
				}
				else if (0xffu == chFunRtn)
				{
					/*线路终点或者道岔未找到，停止查找*/
					BFindFlag = 0u;
				}
				else /*( 0==chReturnValue )*/
				{
					/*B获取下一个link失败,Link失表或者输入错误*/
					chReturnValue = 0u;
					return chReturnValue;
				}
			}

			if ((0u == AFindFlag) && (0u == BFindFlag))
			{
				/*A，B均结束查找并且没有找到对方，函数返回失败*/
				chReturnValue = 0u;
				return chReturnValue;
			}
			else
			{
				/*此时AFindFlag，BFindFlag至少有一个为2，继续查找*/
			}
		}

		/*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
		chReturnValue = 0u;
		return chReturnValue;
	}
#else
	return 0U;
#endif
}

/*
* 功能描述： 由A，B两者位置和A点方向得到两者位移。
* 输入参数： const LOD_STRU * pLODAStru, A点位置
*            const LOD_STRU * pLODBStru, B点位置
*            UINT8 PointNum,             道岔数量
*            const POINT_STRU * pPointStru, 道岔信息结构体
* 输出参数： INT32 *Displacement，       AB位移，是有符号数。符号表示方向，
AB位移以LinkA方向为参照。AB方向与LinkA方向相同，位移值取正，否则位移值取负。
* 返回值：   1,成功
*            0,失败
* 使用注意：注意此处只按照A的方向进行查找，未关注B点的方向。（互联互通项目使用）
* author:qxt 20170811
* 修改查找算法-cjq20230724：【新算法不关注B点的方向】。因AB点的前后关系不确定，原来是A向前查找、B也向前查找，初始都是用A点的方向，但这在灯泡线时存在问题。
*          修改为：A向前/后两个方向查找，不再使用B查找（因为B与A之间是否存在方向变化点不确定，原算法使用A的方向是有问题的）
*/
static UINT8 dsuDispCalcuByLOCExp_HLT(const DSU_EMAP_STRU* pDsuEmapStru, const LOD_STRU* pLODAStru, const LOD_STRU* pLODBStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU* pPointStru, INT32* Displacement)
{
	UINT8 chReturnValue = 0u;					/*用于函数返回值*/
	UINT8 chFunRtn = 0u;						/*调用函数返回值*/
	UINT8 chFlag = 1u;							/*函数执行失败标记*/
	UINT16 wCurrentLinkId = 0u;					/*用于查找过程中临时保存当前LinkID*/
	UINT16 wCurrentLinkIndex = 0u;				/*查找过程中记录Link Index临时变量*/

	UINT8 wAFindFlag = 2u;						/*0，查找失败。1，查找成功。2,继续查找。*/
	UINT8 wRvrAFindFlag = 2u;					/*0，查找失败。1，查找成功。2,继续查找。*/
	INT32 dwAFindDisplacementSum = 0;			/*A查找B过程中的位移累加值*/
	INT32 dwRvrAFindDisplacementSum = 0;		/*A反向查找B过程中位移累加值*/
	DSU_LINK_STRU* pACurrentLinkStru = NULL;	/*A当前Link*/
	DSU_LINK_STRU* pRvrACurrentLinkStru = NULL;	/*A反向当前Link*/
	UINT16 k = 0u;								/*循环用变量*/
	UINT8 wAFindDir = 0u;						/*用于记录实际操作A查找B过程中的查找方向,初始值为A的方向 */
	UINT8 wRvrAFindDir = 0u;					/*用于记录实际操作反向A查找B过程中的查找方向,初始值为A的反方向 */
	
	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU* dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != pDsuEmapStru->dsuStaticHeadStru)
		&& (NULL != pLODAStru) && (NULL != pPointStru) && (NULL != pLODBStru) && (NULL != Displacement))
	{
		/*预置函数返回值为0*/
		*Displacement = 0;

		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断输入A LinkID的有效性*/
		if ((pLODAStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODAStru->Lnk]))
		{
			/*输入LinkId无效,查询失败*/
			chFlag = 0u;
		}

		/*判断输入B LinkID的有效性*/
		if ((pLODBStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODBStru->Lnk]))
		{
			/*输入LinkId无效,查询失败*/
			chFlag = 0u;
		}

		/*判断入口数据linkA偏移量的有效性*/
		if (pLODAStru->Off > LinkLengthExp(pDsuEmapStru, pLODAStru->Lnk))
		{
			/*入口数据异常，查询失败*/
			chFlag = 0u;
		}

		/*判断入口数据linkB偏移量的有效性*/
		if (pLODBStru->Off > LinkLengthExp(pDsuEmapStru, pLODBStru->Lnk))
		{
			/*入口数据异常，查询失败*/
			chFlag = 0u;
		}

		/*判断linkA运行方向的正确性*/
		if ((EMAP_SAME_DIR != pLODAStru->Dir) && (EMAP_CONVER_DIR != pLODAStru->Dir))
		{
			/*入口数据异常，查询失败*/
			chFlag = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		chFlag = 0u;
	}

	/*常规防护已完成 继续执行*/
	if (0u != chFlag)
	{
		/*特殊情况处理，LinkA与LinkB处于同一Link上*/
		if (pLODAStru->Lnk == pLODBStru->Lnk)
		{
			/*A,B位于同一link，计算两点间距离信息*/
			*Displacement = pLODBStru->Off - pLODAStru->Off;

			if (EMAP_SAME_DIR == pLODAStru->Dir)
			{
				/*无需处理*/
			}
			else
			{
				/*方向相反，转换位移符号*/
				*Displacement = -(*Displacement);
			}

			/*函数查询成功*/
			chReturnValue = 1u;
		}
		else/*A、B不在同一link*/
		{
			/*根据LinkID获取当前link*/
			wCurrentLinkIndex = dsuLinkIndex[pLODAStru->Lnk];
			pACurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;
			pRvrACurrentLinkStru = pACurrentLinkStru;

			/*按照A的方向不同进行分类*/
			if (EMAP_SAME_DIR == pLODAStru->Dir)
			{
				/*沿正向查找*/
				dwAFindDisplacementSum = pACurrentLinkStru->dwLength - pLODAStru->Off;
				dwRvrAFindDisplacementSum = pLODAStru->Off;
				wRvrAFindDir = EMAP_CONVER_DIR;
			}
			else /*if (pLODAStru->Dir==CONVERDIR)*/
			{
				/*A,B均沿反向向查找*/
				dwAFindDisplacementSum = pLODAStru->Off;
				dwRvrAFindDisplacementSum = pACurrentLinkStru->dwLength - pLODAStru->Off;
				wRvrAFindDir = EMAP_SAME_DIR;
			}
			wAFindDir = pLODAStru->Dir;

			/*查找基本方法为向A的前后两个方向查，如果任意方向找到B则停止（找到尽头时此方向也停止），记录查找到的位移，返回结果*/
			for (k = 0u; k < LINKINDEXNUM; k++)
			{
				/*此处循环是为了计数，循环次数不能超过LINKINDEXNUM*/
				if (2u == wAFindFlag)
				{
					/*A向前查找一个link*/
					chFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pACurrentLinkStru->wId, wAFindDir, PointNum, pPointStru, &wCurrentLinkId);
					if (1u == chFunRtn)
					{
						/*如果该Link为逻辑方向变化点(查询方向为正且该link的终点为逻辑方向变化点 或 查询反方向且该link的起点为逻辑方向变化)*/
						/* 0x55表示link终点为逻辑方向变化点，0xaa表示link起点为逻辑方向变化点*/
						if (((EMAP_SAME_DIR == wAFindDir) && (0x55 == pACurrentLinkStru->wLogicDirChanged)) ||
							((EMAP_CONVER_DIR == wAFindDir) && (0xaa == pACurrentLinkStru->wLogicDirChanged)))
						{
							wAFindDir = (EMAP_SAME_DIR == wAFindDir) ? EMAP_CONVER_DIR : EMAP_SAME_DIR;/*有变化点，改变查找方向*/
						}

						/*pACurrentLinkStru指向相邻Link*/
						wCurrentLinkIndex = dsuLinkIndex[wCurrentLinkId];
						pACurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;

						if (pACurrentLinkStru->wId == pLODBStru->Lnk)
						{
							/*A找到了B*/

							/*查找成功*/
							wAFindFlag = 1u;
							/*累加偏移量，计算结果值*/
							if (EMAP_SAME_DIR == wAFindDir)/*最终找到B点时的查询方向为正时*/
							{
								dwAFindDisplacementSum += (pLODBStru->Off);
							}
							else/*最终找到B点时的查询方向为反时*/
							{
								dwAFindDisplacementSum += (pACurrentLinkStru->dwLength - pLODBStru->Off);
							}
							/*此时A沿A的方向找到了B，偏移量为正*/
							*Displacement = dwAFindDisplacementSum;
							chReturnValue = 1u;
							/*查找成功，退出*/
							break;

						}
						else
						{
							/*A没有找到B，累加偏移量，继续查找*/
							dwAFindDisplacementSum += pACurrentLinkStru->dwLength;
						}
					}
					else
					{
						/*A获取下一个link失败,Link失表或者输入错误*/
						wAFindFlag = 0u;

						/*本来此处应该break  但是发现还有B点未进行查找 因此此处不进行break*/
					}
				}

				/*A反向前查找一个link*/
				if (2u == wRvrAFindFlag) 
				{
					chFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pRvrACurrentLinkStru->wId, wRvrAFindDir, PointNum, pPointStru, &wCurrentLinkId);
					if (1u == chFunRtn)
					{
						/*如果该Link为逻辑方向变化点(查询方向为正且该link的终点为逻辑方向变化点 或 查询反方向且该link的起点为逻辑方向变化)*/
						/* 0x55表示link终点为逻辑方向变化点，0xaa表示link起点为逻辑方向变化点*/
						if (((EMAP_SAME_DIR == wRvrAFindDir) && (0x55 == pRvrACurrentLinkStru->wLogicDirChanged)) ||
							((EMAP_CONVER_DIR == wRvrAFindDir) && (0xaa == pRvrACurrentLinkStru->wLogicDirChanged)))
						{
							wRvrAFindDir = (EMAP_SAME_DIR == wRvrAFindDir) ? EMAP_CONVER_DIR : EMAP_SAME_DIR;/*有变化点，改变查找方向*/
						}
						/*pRvrACurrentLinkStru指向相邻Link*/
						wCurrentLinkIndex = dsuLinkIndex[wCurrentLinkId];
						pRvrACurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;

						if (pRvrACurrentLinkStru->wId == pLODBStru->Lnk)
						{
							/*反向找到了B*/
							wRvrAFindFlag = 1u;

							/*累加偏移量，计算结果值*/
							if (EMAP_SAME_DIR == wRvrAFindDir)/*最终找到B点时的查询方向为正时*/
							{
								dwRvrAFindDisplacementSum += (pLODBStru->Off);
							}
							else/*最终找到B点时的查询方向为反时*/
							{
								dwRvrAFindDisplacementSum += (pRvrACurrentLinkStru->dwLength - pLODBStru->Off);
							}

							/*此时A沿反方向找到了B，偏移量为负*/
							*Displacement = -dwRvrAFindDisplacementSum;
							chReturnValue = 1u;
							/*查找成功，退出*/
							break;
						}
						else
						{
							/*如果反向没有找到，累加偏移量，继续查找*/
							dwRvrAFindDisplacementSum += pRvrACurrentLinkStru->dwLength;
						}
					}
					else
					{
						/*A获取下一个link失败,Link失表或者输入错误*/
						wRvrAFindFlag = 0u;

						/*本来此处应该break  但是发现还有B点未进行查找 因此此处不进行break*/
					}
				}

				if ((0u == wAFindFlag) && (0u == wRvrAFindFlag))
				{
					/*A，B均结束查找并且没有找到对方，函数返回失败*/
					chReturnValue = 0u;
					break;
				}
				else
				{
					/*此时AFindFlag，BFindFlag至少有一个为2，继续查找*/
				}

			}/*for循环end*/

			 /*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
			 /*如果没找到，此时chReturnValue=0*/
		}
	}/*（0!=chReturnValue）end*/

	return chReturnValue;
}

/*
*访问接口
* 功能描述： 由A，B两者位置得到两者位移。
* 输入参数： const LOD_STRU * pLODAStru, A点位置
*            const LOD_STRU * pLODBStru, B点位置
*            UINT8 PointNum,             道岔数量
*            const POINT_STRU * pPointStru, 道岔信息结构体
* 输出参数： INT32 *Displacement，       AB位移，是有符号数。符号表示方向，
* 返回值：   1,成功
*            0,失败
* 使用注意：合库和燕房项目算法是按照B的方向查找，AB位移以LinkB方向为参照；
互联互通项目因增加逻辑方向变化点，算法改为按照A的方向查找，AB位移以LinkA方向为参照。
* author:qxt 20170811
*/
UINT8 dsuDispCalcuByLOCExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODAStru, const LOD_STRU *pLODBStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru, INT32 *Displacement)
{
	UINT8 rtnValue = 0u;/*函数返回值*/
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDsuEmapStru) && (NULL != pLODAStru) && (NULL != pPointStru) && (NULL != pLODBStru) && (NULL != Displacement))
	{
		cbtcVersionType = GetCbtcSysType();

		if ((DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			rtnValue = dsuDispCalcuByLOCExp_HLT(pDsuEmapStru, pLODAStru, pLODBStru, PointNum, pPointStru, Displacement);
		}
		else if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType))
		{
			rtnValue = dsuDispCalcuByLOCExp_DFT(pDsuEmapStru, pLODAStru, pLODBStru, PointNum, pPointStru, Displacement);
		}
		else
		{
			rtnValue = 0u;
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}


/*
* 函数功能：	此函数的功能是查询两个位置间包含的link序列。
* ****************************************************************************************************
*        1          2              3                4                    5              6
*       -----×---------*--×--------------×--------------------×-------*-----×------------
*                       S                                                 E
*        图例：  ×，Link端点。*，查询输入的位置。1，2，3...Link示意编号
*        如图所示：Link3，4为所求结果。
*        此处方向按照pLODEndStru方向来查找，
*        S沿着pLODEndStru->Dir方向查找，找到E则返回成功，找不到则返回失败。
* ****************************************************************************************************
* 入口参数：	const LOD_STRU * pLODStartStru,	始端位置
*               const LOD_STRU * pLODEndStru	终端位置
*               const UINT8 PointNum			道岔数目
*               const POINT_STRU * pPointStru	道岔信息
* 出口参数：	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN]				Link序列数组
*               UINT16 *wObjNumber                             Link个数
* 返回值：   	0,  查询失败
*               1,  查询成功
* 使用注意：    目前的做法没有包含两端的Link Id。
*               函数要求两个点之间可以形成区域，否则返回0
*               合库和燕房项目使用
*/
static UINT8 dsuGetLinkBtwLocExp_DFT(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN], UINT16 *wObjNumber)
{
#if 0
	UINT16 chFindObjNum = 0u;							/*找到的link的数目*/
	UINT16 wCurrentLinkID = 0u;							/*当前link编号*/
	UINT16 wNextLinkID = 0u;							/*相邻link编号*/
	UINT16 wFindLink[DSU_MAX_LINK_BETWEEN] = { 0u };	/*找到的link序列数组*/
	UINT16 i = 0u;										/*循环用变量*/
	UINT8 chReturnValue = 0u;							/*函数返回值*/
	UINT16 k = 0u;										/*循环用变量*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pPointStru) && (NULL != pLODEndStru) && (NULL != wLinkID) && (NULL != wObjNumber))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*Link个数返回值预置0*/
	*wObjNumber = 0u;

	/*判断入口数据Link的有效性*/
	if ((pLODStartStru->Lnk > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pLODStartStru->Lnk])
		|| (pLODEndStru->Lnk > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pLODEndStru->Lnk]))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断入口数据方向的正确性*/
	if ((EMAP_SAME_DIR != pLODEndStru->Dir) && (EMAP_CONVER_DIR != pLODEndStru->Dir))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	if (pLODStartStru->Lnk == pLODEndStru->Lnk)
	{
		/*始端与终端在同一个Link上，*/

		/*link数为0*/
		*wObjNumber = 0u;


		if (((EMAP_SAME_DIR == pLODEndStru->Dir) && (pLODEndStru->Off >= pLODStartStru->Off))
			|| ((EMAP_CONVER_DIR == pLODEndStru->Dir) && (pLODEndStru->Off <= pLODStartStru->Off)))
		{
			/*查询确保函数从起点能够到达终点*/
			/*返回查询成功，退出*/
			chReturnValue = 1u;
			return chReturnValue;
		}
		else
		{
			/*返回查询成功，退出*/
			chReturnValue = 0u;
			return chReturnValue;
		}

	}

	/*将始端link作为当前link*/
	wCurrentLinkID = pLODStartStru->Lnk;

	/*在找到的当前link不等于终端link时，持续while循环*/
	for (k = 0u; k < LINKINDEXNUM; k++)
	{
		/*此处循环是为了计数，循环次数不能超过LINKINDEXNUM*/

		/*调用功能函数“查询当前link的相邻link”，查询当前link的相邻link*/
		chReturnValue = dsuGetAdjacentLinkIDExp(pDsuEmapStru, wCurrentLinkID, pLODEndStru->Dir, PointNum, pPointStru, &wNextLinkID);

		/*通过函数返回值，判断是否正确*/
		if (1u != chReturnValue)
		{
			/*查询失败，返回查询错误*/
			chFindObjNum = 0u;
			chReturnValue = 0u;
			return chReturnValue;
		}

		/*将找到的相邻link作为新的当前link*/
		wCurrentLinkID = wNextLinkID;

		if (wCurrentLinkID != pLODEndStru->Lnk)
		{
			if (DSU_MAX_LINK_BETWEEN <= chFindObjNum)
			{
				/*缓冲区已满，返回失败，此种情况实际执行时不应该出现*/
				chFindObjNum = 0u;
				chReturnValue = 0u;
				return chReturnValue;
			}
			else
			{
				/*当前Link不等于结束Link，更新局部数组，同时将找到的link数目加一*/
				wFindLink[chFindObjNum] = wCurrentLinkID;
				chFindObjNum++;
			}
		}
		else
		{
			/*当前Link是结束Link，当前Link不写入结果中，找到终端link，退出循环*/
			break;
		}
	}

	if (k == LINKINDEXNUM)
	{
		/*循环因为k==LINKINDEXNUM结束，说明查找次数超过LINKINDEXNUM，此现象不可能发生*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*退出循环，找到终端link,输出找到的link序列*/
	for (i = 0u; i < chFindObjNum; i++)
	{
		wLinkID[i] = wFindLink[i];
	}

	/*找到的Link数量赋值*/
	*wObjNumber = chFindObjNum;

	/*返回查询成功，退出*/
	chReturnValue = 1u;
	return chReturnValue;
#else
	return 0U;
#endif
}

/*
* 函数功能：	此函数的功能是查询两个位置间包含的link序列。
* ****************************************************************************************************
*        1          2              3                4                    5              6
*       -----×---------*--×--------------×--------------------×-------*-----×------------
*                       S                                                 E
*        图例：  ×，Link端点。*，查询输入的位置。1，2，3...Link示意编号
*        如图所示：Link3，4为所求结果。
*        此处方向按照pLODStartStru方向来查找，
*        S沿着pLODStartStru->Dir方向查找，找到E则返回成功，找不到则返回失败。
* ****************************************************************************************************
* 入口参数：	const LOD_STRU * pLODStartStru,	始端位置
*               const LOD_STRU * pLODEndStru	终端位置
*               const UINT8 PointNum			道岔数目
*               const POINT_STRU * pPointStru	道岔信息
* 出口参数：	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN]				Link序列数组
*               UINT16 *wObjNumber                             Link个数
* 返回值：   	0,  查询失败
*               1,  查询成功
* 使用注意：    目前的做法没有包含两端的Link Id。
*               函数要求两个点之间可以形成区域，否则返回0
*               内部调用函数，互联互通项目使用
*  author: qxt 20170814
*/
static UINT8 dsuGetLinkBtwLocExp_HLT(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN], UINT16 *wObjNumber)
{
	UINT16 chFindObjNum = 0u;							/*找到的link的数目*/
	UINT16 wCurrentLinkID = 0u;							/*当前link编号*/
	UINT16 wNextLinkID = 0u;							/*相邻link编号*/
	UINT16 wFindLink[DSU_MAX_LINK_BETWEEN] = { 0u };	/*找到的link序列数组*/
	UINT16 i = 0u;										/*循环用变量*/
	UINT8 chReturnValue = 0u;							/*函数返回值*/
	UINT16 k = 0u;										/*循环用变量*/
	UINT16 wCurrentLinkIndex = 0u;						/*当前Link Index */
	DSU_LINK_STRU *pCurrentLinkStru = NULL;				/*用于循环查找过程中临时保存当前Link结构体*/
	UINT8 wFindDir = 0u;								/*用于记录实际操作过程中的查找方向*/
	UINT8 chFlag = 1u;									/*函数执行失败标识位*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;/*add by qxt 20161029*/

	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != pDsuEmapStru->dsuStaticHeadStru)
		&& (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != wLinkID) && (NULL != wObjNumber))
	{
		/*Link个数返回值预置0*/
		*wObjNumber = 0u;

		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;/*add by qxt 20161029*/

		/*判断入口数据Link的有效性*/
		if ((pLODStartStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODStartStru->Lnk])
			|| (pLODEndStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODEndStru->Lnk]))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			chFlag = 0u;
		}

		/*判断入口数据方向的正确性*/
		if ((EMAP_SAME_DIR != pLODStartStru->Dir) && (EMAP_CONVER_DIR != pLODStartStru->Dir))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			chFlag = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		chFlag = 0u;
	}

	if (0u != chFlag)
	{
		/*始端与终端在同一个Link上，*/
		if (pLODStartStru->Lnk == pLODEndStru->Lnk)
		{
			/*link数为0*/
			*wObjNumber = 0u;
			if (((EMAP_SAME_DIR == pLODStartStru->Dir) && (pLODEndStru->Off >= pLODStartStru->Off))
				|| ((EMAP_CONVER_DIR == pLODStartStru->Dir) && (pLODEndStru->Off <= pLODStartStru->Off)))
			{
				/*查询确保函数从起点能够到达终点*/
				/*返回查询成功，退出*/
				chReturnValue = 1u;
			}
			else
			{
				/*返回查询成功，退出*/
				chReturnValue = 0u;
			}

		}
		else/*始端与终端不在同一个Link上 start*/
		{
			/*将始端link作为当前link*/
			wCurrentLinkID = pLODStartStru->Lnk;

			/*根据LinkID从数据库中查找当前link信息*/
			wCurrentLinkIndex = dsuLinkIndex[wCurrentLinkID];
			pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;
			/*将初始查询方向设置为起点的方向 */
			wFindDir = pLODStartStru->Dir;

			/*在找到的当前link不等于终端link时，持续while循环*/ /*此处循环是为了计数，循环次数不能超过LINKINDEXNUM*/
			for (k = 0u; k < LINKINDEXNUM; k++)
			{
				/*调用功能函数“查询当前link的相邻link”，查询当前link的相邻link*/
				chReturnValue = dsuGetAdjacentLinkIDExp(pDsuEmapStru, wCurrentLinkID, wFindDir, PointNum, pPointStru, &wNextLinkID);

				/*通过函数返回值，判断是否正确*/
				if (1u != chReturnValue)
				{
					/*查询失败，返回查询错误*/
					chFindObjNum = 0u;
					chReturnValue = 0u;
					break;
				}


				/*判断该Link是否有逻辑方向变化点(查询方向为正且该link的终点为逻辑方向变化点 或 查询反方向且该link的起点为逻辑方向变化)*/
				if (((EMAP_SAME_DIR == wFindDir) && (0x55u == pCurrentLinkStru->wLogicDirChanged)) ||
					((EMAP_CONVER_DIR == wFindDir) && (0xaau == pCurrentLinkStru->wLogicDirChanged)))
				{
					wFindDir = ~wFindDir;/*有变化点，改变查找方向*/
				}

				/*将找到的相邻link作为新的当前link*/
				wCurrentLinkID = wNextLinkID;

				/*pCurrentLinkStru指向相邻Link,相邻Link变为当前Link*/
				wCurrentLinkIndex = dsuLinkIndex[wCurrentLinkID];
				pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + wCurrentLinkIndex;

				if (wCurrentLinkID != pLODEndStru->Lnk)
				{
					if (chFindObjNum >= DSU_MAX_LINK_BETWEEN)
					{
						/*缓冲区已满，返回失败，此种情况实际执行时不应该出现*/
						chFindObjNum = 0u;
						chReturnValue = 0u;
						break;
					}
					else
					{
						/*当前Link不等于结束Link，更新局部数组，同时将找到的link数目加一*/
						wFindLink[chFindObjNum] = wCurrentLinkID;
						chFindObjNum++;
					}

				}
				else
				{
					/*当前Link是结束Link，当前Link不写入结果中，找到终端link，退出循环*/
					break;
				}

			}/*for循环结束*/

			 /*查找次数超过LINKINDEXNUM*/
			if ((k != LINKINDEXNUM) && (0u != chReturnValue))
			{
				/*退出循环，找到终端link,输出找到的link序列*/
				for (i = 0u; i < chFindObjNum; i++)
				{
					wLinkID[i] = wFindLink[i];
				}

				/*找到的Link数量赋值*/
				*wObjNumber = chFindObjNum;
				/*返回查询成功*/
				chReturnValue = 1u;

			}

		}/*始端与终端不在同一个Link上 end*/

	}/*(0!=chFlag) end*/
	return chReturnValue;
}

/*访问接口*/
UINT8 dsuGetLinkBtwLocExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN], UINT16 *wObjNumber)
{
	UINT8 rtnValue = 0u;		/*函数返回值*/
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != wLinkID) && (NULL != wObjNumber))
	{
		cbtcVersionType = GetCbtcSysType();

		if ((DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			rtnValue = dsuGetLinkBtwLocExp_HLT(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, wLinkID, wObjNumber);
		}
		else if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType))
		{
			rtnValue = dsuGetLinkBtwLocExp_DFT(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, wLinkID, wObjNumber);
		}
		else
		{
			rtnValue = 0u;
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/*
* 函数功能： 此函数的功能是将判断当前位置是否在指定区域范围内。
* ****************************************************************************************************
*
*       --------------*--------------------*---------------------*-----------------
*                     S                    C                     E
*       查找算法为，计算位移DisplacementSC，DisplacementCE。如果两个位移符号相同，说明C在SE区域内
*
* ****************************************************************************************************
* 入口参数： const LOD_STRU * pLODStartStru,	区域始端位置
*            const LOD_STRU * pLODEndStru,	区域终端位置
*            const UINT8 PointNum,			道岔数目
*            const POINT_STRU * pPointStru,	道岔信息
*			 const LOD_STRU * pCurrentLODStru,当前位置
* 出口参数： UINT8 *pLocInArea         1,在指定区域
*                                     0,不在指定区域
* 返回值：   0，查询失败
*            1，查询成功
* 使用注意：1.在当前位置在区域端点位置时，暂时按如下规则判断：
*             1) 当前位置在区域起点，认为该点属于本区域；
*             2) 当前位置在区域终点，认为该点不属于本区域；
*             3) 3点重合返回在区域内。
*           2.如果待查找的起点与终点都在区域内，可以调用两次本函数，互换始端与终端位置，只要两次调用有一次在区域内，说明在点在始端与终端形成的闭区域内
*           3.如果SE区域不存在，则函数返回0失败。如果SC,或者SE不存在，则函数返回1成功，但是点C不在区域内
*  合库和燕房使用
* 修改记录：1.修改代码逻辑与注释一致：如果SC,或者SE不存在，则函数返回1成功，但是点C不在区域内。by wyd 20210919
*           2.适配灯泡线,确定S起点方向则可查询。by xh 20230802
*/
static UINT8 dsuCheckLocInAreaExp_DFT(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU * pPointStru,
	const LOD_STRU * pCurrentLODStru, UINT8 *pLocInArea)
{
	UINT8 chReturnValue = 0u;			/*函数返回值*/
	INT32 DisplacementSC = 0;			/*start -> current 位移*/
	INT32 DisplacementSE = 0;			/*start -> end 位移 */
	LOD_STRU CurrentLODStru = { 0 };	/*另设一个变量来存储当前LOD，方向取pLODEndStru方向，这样在计算位移时，
										按照统一的方向来计算位移*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != pCurrentLODStru) && (NULL != pLocInArea))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
		return chReturnValue;
	}


	/*默认置不在区域内*/
	*pLocInArea = 0u;

	/*判断入口数据当前位置link编号输入的有效性*/
	if ((pCurrentLODStru->Lnk > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pCurrentLODStru->Lnk]))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断入口数据当前位置偏移量输入的有效性*/
	if (pCurrentLODStru->Off > LinkLengthExp(pDsuEmapStru, pCurrentLODStru->Lnk))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断入口数据当前位置方向的有效性*/
	if ((EMAP_SAME_DIR != pCurrentLODStru->Dir) && (EMAP_CONVER_DIR != pCurrentLODStru->Dir))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断入口数据起点位置link编号输入的有效性*/
	if ((pLODStartStru->Lnk > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pLODStartStru->Lnk]))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断入口数据起点位置link偏移量输入的有效性*/
	if (pLODStartStru->Off > LinkLengthExp(pDsuEmapStru, pLODStartStru->Lnk))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断入口数据起点方向的有效性*/
	if ((EMAP_SAME_DIR != pLODStartStru->Dir) && (EMAP_CONVER_DIR != pLODStartStru->Dir))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断入口数据输入终点位置link编号的有效性*/
	if ((pLODEndStru->Lnk > LINKINDEXNUM) || (0xffffu == dsuLinkIndex[pLODEndStru->Lnk]))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断入口数据终点位置link偏移量输入的有效性*/
	if (pLODEndStru->Off > LinkLengthExp(pDsuEmapStru, pLODEndStru->Lnk))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0;
		return chReturnValue;
	}
	/*判断入口数据终点方向的有效性*/
	if ((EMAP_SAME_DIR != pLODEndStru->Dir) && (EMAP_CONVER_DIR != pLODEndStru->Dir))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	CurrentLODStru.Lnk = pCurrentLODStru->Lnk;
	CurrentLODStru.Off = pCurrentLODStru->Off;
	CurrentLODStru.Dir = pLODEndStru->Dir;

	/*利用位移判断3点关系*/

	/*查询DisplacementSE*/
	chReturnValue = dsuDispCalcuByLOCExp(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, &DisplacementSE);
	if (1u != chReturnValue)
	{
		/*位移查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	else
	{
		/*SE区域存在，函数返回值为1*/
		chReturnValue = 1u;
	}

	/*查询DisplacementSC*/
	chReturnValue = dsuDispCalcuByLOCExp(pDsuEmapStru, pLODStartStru, &CurrentLODStru, PointNum, pPointStru, &DisplacementSC);
	if (1u != chReturnValue)
	{
		/*位移查询失败，但返回值为1*/
		*pLocInArea = 0u;
		chReturnValue = 1u;
		return chReturnValue;
	}

	if ((0 == DisplacementSC) && (0 == DisplacementSE))
	{
		/*三点重合*/
		*pLocInArea = 1u;
		chReturnValue = 1u;
		return chReturnValue;
	}

	if (0 == DisplacementSC)
	{
		/*只与区域的起点重合*/
		*pLocInArea = 1u;
		chReturnValue = 1u;
		return chReturnValue;
	}

	if ((DisplacementSC == DisplacementSE) && (0 != DisplacementSC))
	{
		/*当前位置在区域终点*/
		*pLocInArea = 0u;
		chReturnValue = 1u;
		return chReturnValue;
	}

	if ((0 < DisplacementSC) && (0 < DisplacementSE) && (DisplacementSE > DisplacementSC))
	{
		/*判断DisplacementSC，DisplacementSE符号为正,且SE>SC,则C在SE区域内*/
		*pLocInArea = 1u;
		chReturnValue = 1u;
		return chReturnValue;
	}
	else if ((0 > DisplacementSC) && (0 > DisplacementSE) && (DisplacementSE < DisplacementSC))
	{
		/*判断DisplacementSC，DisplacementSE符号为负,且SE<SC,则C在SE区域内*/
		*pLocInArea = 1u;
		chReturnValue = 1u;
		return chReturnValue;
	}
	else
	{
		/*DisplacementSC，DisplacementSE符号不相同，或C不在区域内*/
		*pLocInArea = 0u;
		chReturnValue = 1u;
		return chReturnValue;
	}
}

/*
* 函数功能： 此函数的功能是将判断当前位置是否在指定区域范围内。
* ****************************************************************************************************
*
*       --------------*--------------------*---------------------*-----------------
*                     S                    C                     E
*       因新增Link逻辑方向变化点，查找算法改为，计算位移DisplacementCS，DisplacementCE。如果两个位移符号相异，说明C在SE区域内
*
* ****************************************************************************************************
* 入口参数： const LOD_STRU * pLODStartStru,	区域始端位置
*            const LOD_STRU * pLODEndStru,	区域终端位置
*            const UINT8 PointNum,			道岔数目
*            const POINT_STRU * pPointStru,	道岔信息
*			 const LOD_STRU * pCurrentLODStru,当前位置
* 出口参数： UINT8 *pLocInArea         1,在指定区域
*                                     0,不在指定区域
* 返回值：   0，查询失败
*            1，查询成功
* 使用注意：1.在当前位置在区域端点位置时，暂时按如下规则判断：
*             1) 当前位置在区域起点，认为该点属于本区域；
*             2) 当前位置在区域终点，认为该点不属于本区域；
*             3) 3点重合返回在区域内。
*           2.如果待查找的起点与终点都在区域内，可以调用两次本函数，互换始端与终端位置，只要两次调用有一次在区域内，说明在点在始端与终端形成的闭区域内
*           3.如果SE区域不存在，则函数返回0失败。如果SC,或者SE不存在，则函数返回1成功，但是点C不在区域内
*author:qxt 20170811
*/
static UINT8 dsuCheckLocInAreaExp_HLT(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	const LOD_STRU *pCurrentLODStru, UINT8 *pLocInArea)
{
#if 0
	UINT8 chReturnValue = 0u;			/*函数返回值*/
	UINT8 chFlag = 1u;					/*函数执行失败标识位*/
	UINT8 chRtnCS = 0u;					/*计算位移CS函数返回值*/
	UINT8 chRtnCE = 0u;					/*计算位移CE函数返回值*/
	INT32 dwDisplacementCS = 0;			/*current -> start 位移*/
	INT32 dwDisplacementCE = 0;			/*current -> end 位移 */
	INT32 dwDisplacementSE = 0;			/*start -> end 位移 */
	LOD_STRU struCurrentLOD = { 0u };	/*另设一个变量来存储当前LOD，方向取pLODStartStru方向，这样在计算位移时，
										按照统一的方向来计算位移*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;

	/*默认置不在区域内*/
	*pLocInArea = 0u;
	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru)
		&& (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != pCurrentLODStru) && (NULL != pLocInArea))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;

		/*判断入口数据当前位置link编号输入的有效性*/
		if ((pCurrentLODStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pCurrentLODStru->Lnk]))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}
		/*判断入口数据当前位置偏移量输入的有效性*/
		if (pCurrentLODStru->Off > LinkLengthExp(pDsuEmapStru, pCurrentLODStru->Lnk))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}
		/*判断入口数据当前位置方向的有效性*/
		if ((EMAP_SAME_DIR != pCurrentLODStru->Dir) && (EMAP_CONVER_DIR != pCurrentLODStru->Dir))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}

		/*判断入口数据起点位置link编号输入的有效性*/
		if ((pLODStartStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pLODStartStru->Lnk]))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}
		/*判断入口数据起点位置link偏移量输入的有效性*/
		if (pLODStartStru->Off > LinkLengthExp(pDsuEmapStru, pLODStartStru->Lnk))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}
		/*判断入口数据起点方向的有效性*/
		if ((EMAP_SAME_DIR != pLODStartStru->Dir) && (EMAP_CONVER_DIR != pLODStartStru->Dir))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}

		/*判断入口数据输入终点位置link编号的有效性*/
		if ((pLODEndStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pLODEndStru->Lnk]))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}
		/*判断入口数据终点位置link偏移量输入的有效性*/
		if (pLODEndStru->Off > LinkLengthExp(pDsuEmapStru, pLODEndStru->Lnk))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}
		/*判断入口数据终点方向的有效性*/
		if ((EMAP_SAME_DIR != pLODEndStru->Dir) && (EMAP_CONVER_DIR != pLODEndStru->Dir))
		{
			/*入口参数异常，查询失败*/
			chFlag = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		chFlag = 0u;
	}

	if (0u != chFlag)
	{
		/*利用位移判断3点关系*/
		struCurrentLOD.Lnk = pCurrentLODStru->Lnk;
		struCurrentLOD.Off = pCurrentLODStru->Off;
		struCurrentLOD.Dir = pLODStartStru->Dir;
		/*查询DisplacementSE*/
		chReturnValue = dsuDispCalcuByLOCExp(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, &dwDisplacementSE);
		if (1u != chReturnValue)
		{
			/*位移查询失败*/
			chReturnValue = 0u;
		}
		else/*SE区域存在*/
		{

			/*查询DisplacementCS*/
			chRtnCS = dsuDispCalcuByLOCExp(pDsuEmapStru, &struCurrentLOD, pLODStartStru, PointNum, pPointStru, &dwDisplacementCS);

			/*查询DisplacementCE*/
			chRtnCE = dsuDispCalcuByLOCExp(pDsuEmapStru, &struCurrentLOD, pLODEndStru, PointNum, pPointStru, &dwDisplacementCE);

			if ((1u != chRtnCS) || (1u != chRtnCE))
			{
				/*位移查询失败*/
				*pLocInArea = 0u;
				chReturnValue = 0u;
			}
			else
			{
				/*三点重合*/
				if ((0 == dwDisplacementCS) && (0 == dwDisplacementCE))
				{
					*pLocInArea = 1u;
					chReturnValue = 1u;
				}
				/*只与区域的起点重合*/
				else if (0 == dwDisplacementCS)
				{

					*pLocInArea = 1u;
					chReturnValue = 1u;
				}

				/*当前位置在区域终点*/
				else if (0 == dwDisplacementCE)
				{

					*pLocInArea = 0u;
					chReturnValue = 1u;
				}

				/*判断DisplacementCS，DisplacementCS符号是否不同 changed by qxt*/
				else if (((0 < dwDisplacementCS) && (0 > dwDisplacementCE)) || ((0 > dwDisplacementCS) && (0 < dwDisplacementCE)))
				{
					/*DisplacementCS，DisplacementCE符号不相同*/
					*pLocInArea = 1u;
					chReturnValue = 1u;
				}
				else
				{
					/*DisplacementCS，DisplacementCE符号相同，当前位置不在区域内*/
					*pLocInArea = 0u;
					chReturnValue = 1u;
				}

			}
		}/*SE区域存在 end*/
	}

	return chReturnValue;
#else
	return 0U;
#endif
}

/*访问接口*/
UINT8 dsuCheckLocInAreaExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU * pPointStru,
	const LOD_STRU * pCurrentLODStru, UINT8 *pLocInArea)
{
	UINT8 rtnValue = 0u;/*函数返回值*/
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != pCurrentLODStru) && (NULL != pLocInArea))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_HLT == cbtcVersionType)
		{
			rtnValue = dsuCheckLocInAreaExp_HLT(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, pCurrentLODStru, pLocInArea);
		}
		else if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			rtnValue = dsuCheckLocInAreaExp_DFT(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, pCurrentLODStru, pLocInArea);
		}
		else
		{
			rtnValue = 0u;
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/************************************************************************

函数功能：	此函数的功能是利用link的ID信息查询link的长度信息
入口参数：	const UINT16 wLinkID		link的ID
出口参数：	无
返回值：	UINT32 dwLinkLength	查询失败: 0xffffffff; 查询成功: 返回查找到的link长度
************************************************************************/
/*功能函数，查询link长度*/
UINT32 LinkLengthExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wLinkID)
{
	UINT32 dwLinkLength = 0u;/*函数返回值*/
	DSU_LINK_STRU *pstLinK;/*指向link信息的结构体指针*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	if (NULL != pDsuEmapStru)
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		dwLinkLength = 0xffffffffu;
		return dwLinkLength;
	}

	/*判断数据输入的有效性*/
	if ((wLinkID > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[wLinkID]))
	{
		/*无效数据输入，查询失败，返回默认值*/
		dwLinkLength = 0xffffffffu;
		return dwLinkLength;
	}
	/*利用管理函数找到link长度信息，返回*/
	pstLinK = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[wLinkID];
	dwLinkLength = pstLinK->dwLength;
	return dwLinkLength;
}

#if 0
/*
* 函数功能： 此函数的功能是将判断当前位置是否在指定区域范围内。
此函数的功能与CheckLocInArea是有区别的。CheckLocInArea对方向没有要求，假设所有涉及的方向都是正确的。
而CheckLocInAreaQuick要求从StartLOD能够到达EndLOD。
* ****************************************************************************************************
*
*       --------------*--------------------*---------------------*-----------------
*                     S                    C                     E
*       查找算法为，首先判断当前位置与输入的S，E是否在同一Link上，
*       如果在同一Link上，根据方向和位移判断是否在区域内，如果不在同一Link，判断CLinkId是否与SE之间的某一LinkId相同
*       由于两点之间的Link可以保存下来，所以对于多次调用本函数查询是否在区域内的函数效率很有帮助
*
* ****************************************************************************************************
* 入口参数： const LOD_STRU * pLODStartStru,	区域始端位置
*            const LOD_STRU * pLODEndStru,	区域终端位置
* 出口参数：	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN]				Link序列数组
*               UINT16 *wObjNumber                             Link个数
*			 const LOD_STRU * pCurrentLODStru,当前位置
* 出口参数： UINT8 *pLocInArea         1,在指定区域
*                                     0,不在指定区域
* 返回值：   0，查询失败
*            1，查询成功
* 使用注意：1.在当前位置在区域端点位置时，暂时按如下规则判断：
*             1) 当前位置在区域起点，认为该点属于本区域；
*             2) 当前位置在区域终点，认为该点不属于本区域；
*             3) 3点重合返回在区域内。
*           2.如果待查找的起点与终点都在区域内，可以调用两次本函数，互换始端与终端位置，只要两次调用有一次在区域内，说明在点在始端与终端形成的闭区域内
*           3.如果SE区域不存在，则函数返回0失败。如果SC,或者SE不存在，则函数返回1成功，但是点C不在区域内
*/
UINT8 dsuCheckLocInAreaQuickExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UINT16 wObjNumber, const UINT16 wLinkID[DSU_MAX_LINK_BETWEEN],
	const LOD_STRU * pCurrentLODStru, UINT8 *pLocInArea)
{
	UINT8 chReturnValue = 0u;	/*函数返回值*/
	UINT16 i = 0u;				/*循环用变量*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != wLinkID) && (NULL != pCurrentLODStru) && (NULL != pLocInArea))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*默认置不在区域内*/
	*pLocInArea = 0u;

	/*判断入口数据输入的有效性*/
	if ((pCurrentLODStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pCurrentLODStru->Lnk]))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断入口数据输入的有效性*/
	if ((pLODStartStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pLODStartStru->Lnk]))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}
	/*判断入口数据输入的有效性*/
	if ((pLODEndStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pLODEndStru->Lnk]))
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*三点重合，返回在区域内*/
	if ((pCurrentLODStru->Off == pLODStartStru->Off) && (pCurrentLODStru->Off == pLODEndStru->Off) &&
		(pCurrentLODStru->Lnk == pLODStartStru->Lnk) && (pCurrentLODStru->Lnk == pLODEndStru->Lnk))
	{
		*pLocInArea = 1u;
		chReturnValue = 1u;
		return chReturnValue;
	}

	/*判断C点是否在S点或者E点所在Lnk上，并且在SE区域内*/

	if (EMAP_SAME_DIR == pLODEndStru->Dir)
	{
		/*当前方向为正方向*/

		if ((pCurrentLODStru->Lnk == pLODStartStru->Lnk) && (pCurrentLODStru->Lnk == pLODEndStru->Lnk))
		{
			/*S,C,ELink相同*/
			if ((pCurrentLODStru->Off >= pLODStartStru->Off) && (pCurrentLODStru->Off<pLODEndStru->Off))
			{
				*pLocInArea = 1u;
				chReturnValue = 1u;
				return chReturnValue;
			}
			else
			{
				*pLocInArea = 0u;
				chReturnValue = 1u;
				return chReturnValue;
			}
		}

		else if (pCurrentLODStru->Lnk == pLODStartStru->Lnk)
		{
			if (pCurrentLODStru->Off >= pLODStartStru->Off)
			{
				/*当前C点Lnk与S点相同且当前C点Off大于等于S点Off，在区域内*/
				*pLocInArea = 1u;
				chReturnValue = 1u;
				return chReturnValue;
			}
			else
			{
				*pLocInArea = 0u;
				chReturnValue = 1u;
				return chReturnValue;
			}

		}
		else if (pCurrentLODStru->Lnk == pLODEndStru->Lnk)
		{
			if (pCurrentLODStru->Off<pLODEndStru->Off)
			{
				/*当前C点Lnk与E点相同且当前C点Off小于E点of，在区域内*/
				*pLocInArea = 1u;
				chReturnValue = 1u;
				return chReturnValue;
			}
			else
			{
				*pLocInArea = 0u;
				chReturnValue = 1u;
				return chReturnValue;
			}

		}
		else
		{
			/*不满足，C点与S点或E点不相同Lnk*/
		}
	}
	else if (EMAP_CONVER_DIR == pLODEndStru->Dir)
	{
		/*当前方向为反方向*/

		if ((pCurrentLODStru->Lnk == pLODStartStru->Lnk) && (pCurrentLODStru->Lnk == pLODEndStru->Lnk))
		{
			if ((pCurrentLODStru->Off <= pLODStartStru->Off) && (pCurrentLODStru->Off>pLODEndStru->Off))
			{
				/*当前C点Lnk与S点相同且在区域内*/
				*pLocInArea = 1u;
				chReturnValue = 1u;
				return chReturnValue;
			}

			else
			{
				*pLocInArea = 0u;
				chReturnValue = 1u;
				return chReturnValue;
			}
		}
		else if (pCurrentLODStru->Lnk == pLODStartStru->Lnk)
		{
			if (pCurrentLODStru->Off <= pLODStartStru->Off)
			{
				/*当前C点Lnk与S点相同且在区域内*/
				*pLocInArea = 1u;
				chReturnValue = 1u;
				return chReturnValue;
			}
			else
			{
				*pLocInArea = 0u;
				chReturnValue = 1u;
				return chReturnValue;
			}
		}
		else if (pCurrentLODStru->Lnk == pLODEndStru->Lnk)
		{
			if (pCurrentLODStru->Off>pLODEndStru->Off)
			{
				/*当前C点Lnk与E点相同且在区域内*/
				*pLocInArea = 1u;
				chReturnValue = 1u;
				return chReturnValue;
			}
			else
			{
				*pLocInArea = 0u;
				chReturnValue = 1u;
				return chReturnValue;
			}
		}
		else
		{
			/*不满足，C点与S点或E点不相同*/
		}
	}
	else
	{
		/*入口参数异常，查询失败*/
		chReturnValue = 0u;
		return chReturnValue;
	}

	/*判断C点不在S点或者E点所在Lnk上，循环查找C点Lnk是否与SE中间某一个Lnk相等*/
	for (i = 0u; i < wObjNumber; i++)
	{
		if (pCurrentLODStru->Lnk == wLinkID[i])
		{
			/*当前C点Lnk与SE区域内的某一个LnkId相同，返回在区域内*/
			*pLocInArea = 1u;
			chReturnValue = 1u;
			return chReturnValue;
		}
	}

	/*当前C点与SE中任意一个Lnk都不相同，返回不在区域内*/
	*pLocInArea = 0u;
	chReturnValue = 1u;
	return chReturnValue;

}

/*
* 函数功能： 此函数的功能是判断当前位置是否在指定区域范围内，此函数是对dsuCheckLocInAreaQuick
的一个补充。用于将当前方向取反，再计算是否在区域内。
* 入口参数： const LOD_STRU * pLODStartStru,	区域始端位置
*            const LOD_STRU * pLODEndStru,	区域终端位置
* 出口参数： UINT16 wLinkID[DSU_MAX_LINK_BETWEEN]				Link序列数组
*            UINT16 *wObjNumber                             Link个数
*			 const LOD_STRU * pCurrentLODStru,当前位置
*            UINT8 *pLocInArea         1,在指定区域
*                                     0,不在指定区域
* 返回值：   0，查询失败
*            1，查询成功
*/
UINT8 dsuCheckLocInAreaQuickExp2(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UINT16 wObjNumber, const UINT16 wLinkID[DSU_MAX_LINK_BETWEEN],
	const LOD_STRU * pCurrentLODStru, UINT8 *pLocInArea)
{
	LOD_STRU TempLODStartStru = { 0 };
	LOD_STRU TempLODEndStru = { 0 };
	LOD_STRU TempCurrentLODStru = { 0 };

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != wLinkID) && (NULL != pCurrentLODStru) && (NULL != pLocInArea))
	{
		CommonMemCpy(&TempLODStartStru, sizeof(LOD_STRU), pLODStartStru, sizeof(LOD_STRU));
		CommonMemCpy(&TempLODEndStru, sizeof(LOD_STRU), pLODEndStru, sizeof(LOD_STRU));
		CommonMemCpy(&TempCurrentLODStru, sizeof(LOD_STRU), pCurrentLODStru, sizeof(LOD_STRU));

		/*将三个LOD的方向取反*/
		if (EMAP_SAME_DIR == pLODEndStru->Dir)
		{
			TempLODStartStru.Dir = EMAP_CONVER_DIR;
			TempLODEndStru.Dir = EMAP_CONVER_DIR;
			TempCurrentLODStru.Dir = EMAP_CONVER_DIR;
		}
		else
		{
			TempLODStartStru.Dir = EMAP_SAME_DIR;
			TempLODEndStru.Dir = EMAP_SAME_DIR;
			TempCurrentLODStru.Dir = EMAP_SAME_DIR;
		}

		return dsuCheckLocInAreaQuickExp(pDsuEmapStru, &TempLODStartStru, &TempLODEndStru, wObjNumber, wLinkID, &TempCurrentLODStru, pLocInArea);
	}
	else
	{
		return 0;
	}

}
#endif

/*****************************以下为坡度平缓计算过程中需要用到的函数**************************/
/************************************************************************

函数功能：	此函数的功能是查询当前Garde的相邻Grade
入口参数：	const UINT16 wCurrentGradeID	当前坡度编号
const UCHAR chDirection			期望运行方向
const UCHAR PointNum			MA范围内道岔数目
const DQU_POINT_STATUS_STRU * pPointStru	MA范围内道岔信息
出口参数：	无
返回值：	UINT16 wFindGradeID	查询失败: 0xffff; 查询成功: 返回相邻的坡度ID
************************************************************************/
/*功能函数,查询当前Garde的相邻Grade*/
UINT16 FindAdjacentGradeIDExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wCurrentGradeID, const UCHAR chDirection,
	const UCHAR PointNum, const DQU_POINT_STATUS_STRU * pPointStru)
{
	UINT16 wFindGradeID = 0u;							/*函数返回值*/
	DSU_GRADE_STRU *pstGrade = NULL;					/*指向坡度信息的结构体指针*/
	const DQU_POINT_STATUS_STRU * pstPointStrut = NULL;	/*存储道岔信息的局部变量*/
	UINT16 wPointId = 0u;								/*存储坡度关联道岔点的ID*/
	UCHAR i = 0u;										/*循环用变量*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 GRADEINDEXNUM = 0u;
	UINT16* dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pPointStru))
	{
		GRADEINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->GRADEINDEXNUM;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*数据无效，返回查询失败*/
		wFindGradeID = 0xffffu;
		return wFindGradeID;
	}

	/*判断输入的有效性*/
	if ((wCurrentGradeID > GRADEINDEXNUM) || (0xffffu == dsuGradeIndex[wCurrentGradeID]))
	{
		/*数据无效，返回查询失败*/
		wFindGradeID = 0xffffu;
		return wFindGradeID;
	}

	/*判断入口数据方向的有效性*/
	if ((EMAP_SAME_DIR != chDirection) && (EMAP_CONVER_DIR != chDirection))
	{
		/*数据无效，返回查询失败*/
		wFindGradeID = 0xffffu;
		return wFindGradeID;
	}

	/*在期望运行方向与link的逻辑方向相同时，向当前坡度的末端相邻坡度进行查询*/
	if (EMAP_SAME_DIR == chDirection)
	{
		/*利用管理函数直接找到当前坡度的信息*/
		pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[wCurrentGradeID];

		if ((0xffffu == pstGrade->wTmnlJointSideGradeId) && (0xffffu == pstGrade->wTmnlJointMainGradeId))
		{
			/*当前坡度已经没有相邻坡度，函数返回0*/
			wFindGradeID = 0xffffu;
			return wFindGradeID;
		}
		/*在当前坡度的末端相邻侧线坡度为空时，其末端相邻正线坡度即为所求*/
		if (0xffffu == pstGrade->wTmnlJointSideGradeId)
		{
			/*查询成功，返回相关信息*/
			wFindGradeID = pstGrade->wTmnlJointMainGradeId;
			return wFindGradeID;
		}
		else
		{
			/*在当前坡度的末端相邻侧线坡度不为空时，需利用实时的道岔状态来判断相邻坡度*/

			/*导入道岔信息*/
			pstPointStrut = pPointStru;

			/*遍历道岔信息*/
			wPointId = pstGrade->wTmnlRelatePointId;

			for (i = 0u; i < PointNum; i++)
			{
				if (wPointId == pstPointStrut->PointId)
				{
					/*道岔失去表示,数据异常，查询失败，返回默认值，退出*/
					if (EMAP_POINT_STATUS_LOSE == pstPointStrut->PointStatus)
					{
						wFindGradeID = 0xffffu;
						return wFindGradeID;
					}
					else
					{
						/*道岔处于定位,其末端相邻正线坡度即为所求*/
						if (EMAP_POINT_STATUS_MAIN == pstPointStrut->PointStatus)
						{
							wFindGradeID = pstGrade->wTmnlJointMainGradeId;
							return wFindGradeID;
						}
						else
						{
							/*道岔处于反位,其末端相邻侧线坡度即为所求*/
							wFindGradeID = pstGrade->wTmnlJointSideGradeId;
							return wFindGradeID;
						}
					}
				}
				pstPointStrut++;
			}

			/*没有找到相关的道岔信息,查询失败,返回默认值*/
			wFindGradeID = 0xffffu;
			return wFindGradeID;
		}
	}/*end of  if (SAMEDIR == chDirection)*/
	else
	{
		/*在期望运行方向与link的逻辑方向相反时，向当前坡度的始端端相邻坡度进行查询*/



		/*利用管理函数直接找到当前坡度的信息*/
		pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[wCurrentGradeID];

		if ((0xffffu == pstGrade->wOrgnJointSideGradeId) && (0xffffu == pstGrade->wOrgnJointMainGradeId))
		{
			/*当前坡度已经没有相邻坡度，函数返回0*/
			wFindGradeID = 0xffffu;
			return wFindGradeID;
		}
		/*在当前坡度的始端相邻侧线坡度为空时，其始端相邻正线坡度即为所求*/
		if (0xffffu == pstGrade->wOrgnJointSideGradeId)
		{
			/*查询成功，返回坡度ID*/
			wFindGradeID = pstGrade->wOrgnJointMainGradeId;
			return wFindGradeID;
		}
		else
		{
			/*在当前坡度的始端相邻侧线坡度不为空时，需利用实时的道岔状态来判断相邻坡度*/

			/*导入道岔信息*/
			pstPointStrut = pPointStru;

			/*遍历道岔信息*/
			wPointId = pstGrade->wOrgnRelatePointId;

			for (i = 0u; i < PointNum; i++)
			{
				if (wPointId == pstPointStrut->PointId)
				{
					/*道岔失去表示,数据异常，查询失败，返回默认值，退出*/
					if (EMAP_POINT_STATUS_LOSE == pstPointStrut->PointStatus)
					{
						wFindGradeID = 0xffffu;
						return wFindGradeID;
					}
					else
					{
						/*道岔处于定位,其始端相邻正线坡度即为所求*/
						if (EMAP_POINT_STATUS_MAIN == pstPointStrut->PointStatus)
						{
							wFindGradeID = pstGrade->wOrgnJointMainGradeId;
							return wFindGradeID;
						}
						else
						{
							/*道岔处于反位,其始端相邻侧线坡度即为所求*/
							wFindGradeID = pstGrade->wOrgnJointSideGradeId;
							return wFindGradeID;
						}
					}
				}
				pstPointStrut++;
			}

			/*没有找到相关的道岔信息,查询失败,返回默认值*/
			wFindGradeID = 0xffffu;
			return wFindGradeID;
		}
	}
}

/*
*函数功能：查询某点所在坡度ID
*输入参数：UINT16 linkId,UINT32 offset待计算点
*输出参数：UINT16 GradeID所在坡度ID
*返回值：  0函数执行失败，1成功
*/
static UINT8 dsuGetGradeIdbyLocExp_DFT(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, const UINT32 offset, UINT16 *GradeID)
{
#if 0
	UINT16 i = 0u, j = 0u;							/*循环计数变量*/
	DSU_GRADE_STRU* curGrade = NULL;          		/*当前坡度临时变量*/
	UINT8 bReturnValue = 0u;						/*函数返回值变量*/
	DSU_LINK_STRU *pLinkStru = NULL;				/*Link信息结构体指针*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != GradeID))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	if ((linkId > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[linkId]))
	{
		/*入口参数异常，查询失败*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*获取当前位置的link对应的位置信息行*/
	pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[linkId];

	/*当前link的“Link坡度信息属性”为无统一坡度*/
	if (0u == pLinkStru->wGrade)
	{
		for (i = 0u; i<dsuDataLenStru->wGradeLen; i++)
		{
			/*初始化当前坡度*/
			curGrade = dsuStaticHeadStru->pGradeStru + i;
			/*如果起始link与输入link相同*/
			if (linkId == curGrade->wOrgnLinkId)
			{
				/*输入偏移量>=当前坡度起点偏移量*/
				if ((offset >= curGrade->dwOrgnLinkOfst) && ((offset - curGrade->dwOrgnLinkOfst) <= curGrade->dwGradeLength))
				{
					/*输入当前坡度ID*/
					*GradeID = curGrade->wId;
					bReturnValue = 1;
					return bReturnValue;
				}
				else
				{
					/*继续向下执行*/
				}
			}
			/*如果终端link与输入link相同*/
			else if (linkId == curGrade->wTmnlLinkId)
			{
				/*输入偏移量<=当前坡度终点偏移量*/
				if (offset <= curGrade->dwTmnlLinkOfst)
				{
					/*输入当前坡度ID*/
					*GradeID = curGrade->wId;
					bReturnValue = 1u;
					return bReturnValue;
				}
				else
				{
					/*继续向下执行*/
				}
			}
			/*与中间link相同*/
			else
			{
				j = 0u;
				while (0xffffu != (curGrade->wIncludeLinkId[j]))
				{
					if (linkId == (curGrade->wIncludeLinkId[j]))
					{
						*GradeID = curGrade->wId;
						bReturnValue = 1u;
						return bReturnValue;
					}
					else
					{
						j++;
					}
				}
			}
		}
		/*遍历完坡度，没找到该点*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
		/*当前link的“Link坡度信息属性”为统一坡度*/
	{
		/*统一坡度不需要使用坡度ID信息,置为无效*/
		*GradeID = 0xffffu;
		bReturnValue = 1u;
		return bReturnValue;
	}
#else
	return 0U;
#endif
}

/*
*函数功能：查询某点所在坡度ID
*输入参数：UINT16 linkId,UINT32 offset待计算点
*输出参数：UINT16 GradeID所在坡度ID
*返回值：  0函数执行失败，1成功
*/
static UINT8 dsuGetGradeIdbyLocExp_CPK(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, const UINT32 offset, UINT16 *GradeID)
{
	UINT16 i = 0u;							/*循环计数变量*/
	UINT8 bReturnValue = 0u;				/*函数返回值变量*/
	DSU_LINK_STRU *pLinkStru = NULL;		/*Link信息结构体指针*/
	DSU_GRADE_STRU *pGradeStru = NULL;		/*坡度信息结构体指针*/

	DSU_GRADE_LINKIDX_STRU *pTempGradeLinkIdxStru = NULL;
	UINT16 tempGradeID = 0;

	UINT16 LINKINDEXNUM = 0;
	UINT16 *dsuLinkIndex = NULL;
	UINT16 *dsuGradeIndex = NULL;			/*坡度信息索引数组*/
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != pDsuEmapStru->dsuStaticHeadStru) && (NULL != GradeID))
	{
		/*全局变量改传参调用涉及的全局变量*/
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		if ((linkId <= LINKINDEXNUM) && (DSU_NULL_16 != dsuLinkIndex[linkId]))
		{
			/*获取当前位置的link对应的位置信息行*/
			pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[linkId];
			/*飞检修改，对应2021年2月份飞检中规则MISRA2004-13_7，yt，2021年4月19日*/
			if (offset <= pLinkStru->dwLength)
			{
				pTempGradeLinkIdxStru = g_dsuEmapExtendStru->pDsuGradeInLinkIdx;
				pTempGradeLinkIdxStru = pTempGradeLinkIdxStru + pLinkStru->wId;

				if (0u != pLinkStru->wGrade)/*当前link的“Link坡度信息属性”为统一坡度*/
				{
					if (0u == pTempGradeLinkIdxStru->wGradeCount)
					{
						*GradeID = DSU_NULL_16;/*统一坡度不需要使用坡度ID信息,置为无效*/
						bReturnValue = 1u;
					}
					else
					{
						/*错误分支*/
						bReturnValue = 0u;
					}
				}
				else/*当前link的“Link坡度信息属性”为无统一坡度*/
				{
					if (1u == pTempGradeLinkIdxStru->wGradeCount)
					{
						tempGradeID = pTempGradeLinkIdxStru->wGradeId[0];
						*GradeID = tempGradeID;
						bReturnValue = 1u;
					}
					else if (1u < pTempGradeLinkIdxStru->wGradeCount)
					{
						for (i = 0u; i < pTempGradeLinkIdxStru->wGradeCount; i++)
						{
							tempGradeID = pTempGradeLinkIdxStru->wGradeId[i];

							/*获取当前坡度对应的信息行*/
							pGradeStru = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[tempGradeID];

							/*当一个link上有多个坡度时，处在该link中间某段的坡度必不含中间link，故只需判断该link上每个坡度的首、尾link与输入的参数linkId是否相同即可。*/
							if ((linkId == pGradeStru->wOrgnLinkId) && (linkId == pGradeStru->wTmnlLinkId))/*一个link对应多个坡度，且坡度位于link的中间某段*/
							{
								if ((offset >= pGradeStru->dwOrgnLinkOfst) && ((offset - pGradeStru->dwOrgnLinkOfst) <= pGradeStru->dwGradeLength))
								{
									*GradeID = tempGradeID;
									bReturnValue = 1u;
									break;
								}
							}
							else if ((linkId != pGradeStru->wOrgnLinkId) && (linkId == pGradeStru->wTmnlLinkId))/*一个link对应多个坡度，且坡度位于link的始端*/
							{
								if (offset <= pGradeStru->dwTmnlLinkOfst)
								{
									*GradeID = tempGradeID;
									bReturnValue = 1u;
									break;
								}
							}
							else if ((linkId == pGradeStru->wOrgnLinkId) && (linkId != pGradeStru->wTmnlLinkId))/*一个link对应多个坡度，且坡度位于link的终端*/
							{
								if (offset >= pGradeStru->dwOrgnLinkOfst)
								{
									*GradeID = tempGradeID;
									bReturnValue = 1u;
									break;
								}
							}
							else
							{
								/*错误分支*/
								bReturnValue = 0u;
							}

						}
						/*link上没有找到对应的坡度*/
						if (i == pTempGradeLinkIdxStru->wGradeCount)
						{
							bReturnValue = 0u;
						}
					}
					else
					{
						/*错误分支*/
						bReturnValue = 0u;
					}
				}
			}
			else
			{
				/*输入偏移量参数不合法，查询失败*/
				bReturnValue = 0u;
			}
		}
		else
		{
			/*输入link编号异常，查询失败*/
			bReturnValue = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
	}
	return bReturnValue;
}

/*
*函数功能：查询某点所在坡度ID
*输入参数：UINT16 linkId,UINT32 offset待计算点
*输出参数：UINT16 GradeID所在坡度ID
*返回值：  0函数执行失败，1成功
*修改说明：对CPK/HLT查询某点所在坡度ID算法进行了优化，共同使用dsuGetGradeIdbyLocExp_CPK，
*		  FAO/DG继续使用原有算法，
*         modify by lmy 20171221
*/
UINT8 dsuGetGradeIdbyLocExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, const UINT32 offset, UINT16 *GradeID)
{
	UINT8 rtnValue = 0u;			/*函数返回值*/
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDsuEmapStru) && (NULL != GradeID))
	{
		cbtcVersionType = GetCbtcSysType();

		if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			rtnValue = dsuGetGradeIdbyLocExp_CPK(pDsuEmapStru, linkId, offset, GradeID);
		}
		else if ((DQU_CBTC_FAO == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType))
		{
			rtnValue = dsuGetGradeIdbyLocExp_DFT(pDsuEmapStru, linkId, offset, GradeID);
		}
		else
		{
			rtnValue = 0u;
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/*************
*函数功能：计算某点坡度平缓后的坡度值
*输入参数：Point 所求坡度点
*输出参数：
*返回值：  0函数执行失败 1 点在坡度第一段 2 点在坡度第2段 3 点在坡度第3段
****************/
UINT8 dsuCalculateGradeValueExp_FAO(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkId, const UINT32 ofst, FLOAT32* GradeVal)
{
#if 0
	UINT8  funcRtn = 0u;
	UINT16 gradeId = 0u;					/*点所在坡度ID*/
	UINT8  bReturnValue = 0u;				/*函数返回值*/
	UINT32 arcEndOfst = 0u;					/*坡度第一个分界点所处相对位置*/
	UINT32 arcStartOfst = 0u;				/*坡度第二个分界点所处相对位置*/
	UINT32 relLocation = 0u;				/*当前点所处相对位置*/
	FLOAT64 angleValue = 0.0f;				/*角度变化值*/
	FLOAT64 tempAngleValue = 0.0f;			/*计算出的临时值*/
	DSU_GRADE_STRU* curGrade = NULL;        /*当前坡度临时变量*/
	DSU_GRADE_STRU  curGradeTmp = { 0 };	/*当前坡度临时变量*/
	DSU_GRADE_STRU* frontGrade = NULL;		/*当前坡度前方相邻坡度临时变量*/
	DSU_GRADE_STRU* rearGrade = NULL;		/*当前坡度后方相邻坡度临时变量*/
	FLOAT32 curGradeVal = 0.0f;
	FLOAT64 tempValue = 0.0f;				/*临时变量，用来存表达式计算值*/
	UINT8 GradeTrend = 0u;
	UINT8 RtnGetGradFrnt = 0u;
	UINT8 RtnGetGradNxt = 0u;

	UINT16* dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	if ((NULL != pDsuEmapStru) && (NULL != GradeVal))
	{
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*计算出点所在坡度*/
	funcRtn = dsuGetGradeIdbyLocExp(pDsuEmapStru, LinkId, ofst, &gradeId);
	if (0u == funcRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*计算出点所在坡度两分界点所处相对位置*/
	funcRtn = dsuDividPointDistanceFromOriginExp(pDsuEmapStru, gradeId, LinkId, &arcEndOfst, &arcStartOfst);
	if (0u == funcRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*计算出点所在坡度两分界点所处相对位置*/
	funcRtn = dsuGetDistanceFromOriginExp(pDsuEmapStru, gradeId, LinkId, ofst, &relLocation);
	if (0u == funcRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*为了方便计算，对分界点的相对距离进行以下处理*/
	if (0xFFFFFFFFu == arcEndOfst)
	{
		arcEndOfst = 0u;
	}

	if (0xffffu != gradeId)
	{
		/*初始化当前坡度*/
		curGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[gradeId];
		/*方向为正，坡度值为正*/
		if (0x55u == curGrade->wInclineDir)
		{
			curGradeVal = (FLOAT32)((FLOAT32)curGrade->wGradValue) / 10000.0f;
		}
		else
			/*方向反，坡度值为负*/
		{
			curGradeVal = (FLOAT32)(-(FLOAT32)curGrade->wGradValue) / 10000.0f;
		}

		/*获取前面相邻坡度*/
		if (0xffffu != curGrade->wOrgnJointMainGradeId)
		{
			frontGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[curGrade->wOrgnJointMainGradeId];
		}
		else
		{
			/*线路终点或关联统一坡度,不再进行坡度平缓*/
			*GradeVal = (FLOAT32)(10000.0f * curGradeVal);
			/*返回成功*/
			bReturnValue = 2u;
			return bReturnValue;
		}

		/*获取后面相邻坡度*/
		if (0xffffu != curGrade->wTmnlJointMainGradeId)
		{
			rearGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[curGrade->wTmnlJointMainGradeId];
		}
		else
		{
			/*线路终点或关联统一坡度,不再进行坡度平缓*/
			*GradeVal = (FLOAT32)(10000.0f * curGradeVal);
			/*返回成功*/
			bReturnValue = 2u;
			return bReturnValue;
		}
	}
	else
	{
		/*获取当前的统一坡度值,因为0不再进行运算和方向判定*/
		curGradeVal = dsuStaticHeadStru->pConGradeStru->wGrade;

		/*当前为统一坡度时,为方便后面计算,在该处赋值*/
		curGradeTmp.wId = 0xFFFFu;
		curGradeTmp.dwRadius = 0u;
		curGradeTmp.wInclineDir = 0x55u;
		curGradeTmp.wGradValue = (UINT16)curGradeVal;
		curGrade = &curGradeTmp;


		/*使用前的初始化*/
		frontGrade = NULL;
		rearGrade = NULL;

		/*获取前面相邻坡度*/
		RtnGetGradFrnt = dsuGetGradeFrntExp(pDsuEmapStru, LinkId, &frontGrade);

		/*获取后面相邻坡度*/
		RtnGetGradNxt = dsuGetGradeRearExp(pDsuEmapStru, LinkId, &rearGrade);

		/*若前面或后面相邻坡度查询失败*/
		if ((0u == RtnGetGradFrnt) || (0u == RtnGetGradNxt))
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}

		/*20141122添加：若查询前面和后面坡度时,发现当前坡度在线路终点,不再进行平缓*/
		if ((2u == RtnGetGradFrnt) || (2u == RtnGetGradNxt))
		{
			*GradeVal = (FLOAT32)(10000.0f * curGradeVal);
			/*返回成功*/
			bReturnValue = 2u;
			return bReturnValue;
		}
	}

	/*
	1 没有分界点的时候，即相邻坡度与当前坡度差小于千分之20
	2 当前点处于分界点所标示的弧度外
	*/
	if ((relLocation >= arcEndOfst) && (relLocation <= arcStartOfst))
	{

		*GradeVal = (FLOAT32)(10000.0f * curGradeVal);
		/*返回成功*/
		bReturnValue = 2u;
		return bReturnValue;
	}
	/*当前点在第一个分界点弧度内*/
	else if (relLocation < arcEndOfst)
	{
		if (NULL != frontGrade)
		{
			/*计算角度变化值*/
			if (0u == frontGrade->dwRadius)
			{
				/*曲率半径为0，角度变化值为0*/
				angleValue = 0u;
			}
			else
			{
				/*角度变化值*/
				angleValue = (FLOAT32)((arcEndOfst + relLocation) / (1.0*frontGrade->dwRadius));
			}
			/*坡度是上坡，取正值*/
			if (0x55u == frontGrade->wInclineDir)
			{
				tempValue = ((FLOAT64)frontGrade->wGradValue) / 10000.0f;
			}
			/*下坡取负值*/
			else
			{
				tempValue = (-(FLOAT64)frontGrade->wGradValue) / 10000.0f;
			}
			/*if用来判断角度变化值是增加值还是减少值*/
			if (dquGradeTrendExp(pDsuEmapStru, frontGrade->wId, curGrade->wId, &GradeTrend))
			{
				/*坡度减少*/
				if (1u == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue - angleValue;
				}
				/*坡度增加*/
				else if (2u == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue + angleValue;
				}
				/*坡度不变化*/
				else
				{
					tempAngleValue = tempValue;
				}

				/*转换为FLOAT32类型*/
				*GradeVal = (FLOAT32)(tempAngleValue * 10000.0f);
				/*返回成功*/
				bReturnValue = 1u;
				return bReturnValue;
			}
			else
			{
				/*不可达*/
				bReturnValue = 0u;
				return bReturnValue;
			}
		}
		else
		{
			/*转换为FLOAT32类型*/
			*GradeVal = (FLOAT32)(curGradeVal);
			/*返回成功*/
			bReturnValue = 1u;
			return bReturnValue;
		}
	}
	/*在第三分界段内*/
	else
	{
		if (NULL != rearGrade)
		{
			/*计算角度变化值*/
			if (0u == curGrade->dwRadius)
			{
				/*曲率半径为0，角度变化值为0*/
				angleValue = 0u;
			}
			else
			{
				/*角度变化值*/
				angleValue = (((FLOAT32)(relLocation - arcStartOfst)) / ((FLOAT32)curGrade->dwRadius));
			}

			if (0x55u == curGrade->wInclineDir)
			{
				tempValue = ((FLOAT64)curGrade->wGradValue) / 10000.0F;
			}
			else
			{
				tempValue = (-(FLOAT64)curGrade->wGradValue) / 10000.0F;
			}

			/*if用来判断角度变化值是增加值还是减少值*/
			if (dquGradeTrendExp(pDsuEmapStru, curGrade->wId, rearGrade->wId, &GradeTrend))
			{
				/*坡度值减少*/
				if (1u == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue - angleValue;
				}
				/*坡度值增加*/
				else if (2u == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue + angleValue;
				}
				/*坡度值不变化*/
				else
				{
					tempAngleValue = tempValue;
				}

				/*转换为FLOAT32类型*/
				*GradeVal = (FLOAT32)(tempAngleValue * 10000.0f);
				/*返回成功*/
				bReturnValue = 3u;
				return bReturnValue;
			}
			else
			{
				/*不可达*/
				bReturnValue = 0u;
				return bReturnValue;
			}
		}
		else
		{
			/*转换为FLOAT32类型*/
			*GradeVal = (FLOAT32)(curGradeVal);
			/*返回成功*/
			bReturnValue = 3u;
			return bReturnValue;
		}
	}
#else
	return 0U;
#endif
}

UINT8 dsuCalculateGradeValueExp_CPK(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkId, const UINT32 ofst, FLOAT32* GradeVal)
{
	UINT8  funcRtn = 0u;
	UINT16 gradeId = 0u;					/*点所在坡度ID*/
	UINT8  bReturnValue = 0u;				/*函数返回值*/
	UINT32 arcEndOfst = 0u;					/*坡度第一个分界点所处相对位置*/
	UINT32 arcStartOfst = 0u;				/*坡度第二个分界点所处相对位置*/
	UINT32 relLocation = 0u;				/*当前点所处相对位置*/
	FLOAT64 angleValue = 0.0f;				/*角度变化值*/
	FLOAT64 tempAngleValue = 0.0f;			/*计算出的临时值*/
	DSU_GRADE_STRU* curGrade = NULL;		/*当前坡度临时变量*/
	DSU_GRADE_STRU  curGradeTmp = { 0 };	/*当前坡度临时变量*/
	DSU_GRADE_STRU* frontGrade = NULL;		/*当前坡度前方相邻坡度临时变量*/
	DSU_GRADE_STRU* rearGrade = NULL;		/*当前坡度后方相邻坡度临时变量*/
	FLOAT32 curGradeVal = 0.0f;
	FLOAT64 tempValue = 0.0f;				/*临时变量，用来存表达式计算值*/
	UINT8 GradeTrend = 0u;
	UINT8 RtnGetGradFrnt = 0u;
	UINT8 RtnGetGradNxt = 0u;

	/*全局变量改传参调用涉及的全局变量*/
	UINT16* dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	if ((NULL != pDsuEmapStru) && (NULL != GradeVal))
	{
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*计算出点所在坡度*/
	funcRtn = dsuGetGradeIdbyLocExp(pDsuEmapStru, LinkId, ofst, &gradeId);
	if (0u == funcRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*计算出点所在坡度两分界点所处相对位置*/
	funcRtn = dsuDividPointDistanceFromOriginExp(pDsuEmapStru, gradeId, LinkId, &arcEndOfst, &arcStartOfst);
	if (0u == funcRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*计算出点所在坡度两分界点所处相对位置*/
	funcRtn = dsuGetDistanceFromOriginExp(pDsuEmapStru, gradeId, LinkId, ofst, &relLocation);
	if (0u == funcRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*为了方便计算，对分界点的相对距离进行以下处理*/
	if (0xFFFFFFFFu == arcEndOfst)
	{
		arcEndOfst = 0u;
	}

	if (0xffffu != gradeId)
	{
		/*初始化当前坡度*/
		curGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[gradeId];
		/*方向为正，坡度值为正*/
		if (0x55u == curGrade->wInclineDir)
		{
			curGradeVal = ((FLOAT32)curGrade->wGradValue) / 10000.0f;
		}
		else
			/*方向反，坡度值为负*/
		{
			curGradeVal = (-(FLOAT32)curGrade->wGradValue) / 10000.0f;
		}

		/*获取前面相邻坡度*/
		if (0xffffu != curGrade->wOrgnJointMainGradeId)
		{
			frontGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[curGrade->wOrgnJointMainGradeId];
		}
		/*else
		{
		*GradeVal = (FLOAT32)(10000*curGradeVal);
		bReturnValue = 2;
		return bReturnValue;
		}*/

		/*获取后面相邻坡度*/
		if (0xffffu != curGrade->wTmnlJointMainGradeId)
		{
			rearGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[curGrade->wTmnlJointMainGradeId];
		}
		/*else
		{
		*GradeVal = (FLOAT32)(10000*curGradeVal);
		bReturnValue = 2;
		return bReturnValue;
		}*/
	}
	else
	{
		/*获取当前的统一坡度值,因为0不再进行运算和方向判定*/
		curGradeVal = dsuStaticHeadStru->pConGradeStru->wGrade;

		/*当前为统一坡度时,为方便后面计算,在该处赋值*/
		curGradeTmp.wId = 0xFFFFu;
		curGradeTmp.dwRadius = 0u;
		curGradeTmp.wInclineDir = 0x55u;
		curGradeTmp.wGradValue = (UINT16)curGradeVal;
		curGrade = &curGradeTmp;


		/*使用前的初始化*/
		frontGrade = NULL;
		rearGrade = NULL;

		/*获取前面相邻坡度*/
		RtnGetGradFrnt = dsuGetGradeFrntExp(pDsuEmapStru, LinkId, &frontGrade);

		/*获取后面相邻坡度*/
		RtnGetGradNxt = dsuGetGradeRearExp(pDsuEmapStru, LinkId, &rearGrade);

		/*若前面或后面相邻坡度查询失败*/
		if ((0u == RtnGetGradFrnt) || (0u == RtnGetGradNxt))
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}

		/*20141122添加：若查询前面和后面坡度时,发现当前坡度在线路终点,不再进行平缓*/
		if ((2u == RtnGetGradFrnt) || (2u == RtnGetGradNxt))
		{
			*GradeVal = (FLOAT32)(10000.0f * curGradeVal);
			/*返回成功*/
			bReturnValue = 2u;
			return bReturnValue;
		}
	}

	/*
	1 没有分界点的时候，即相邻坡度与当前坡度差小于千分之20
	2 当前点处于分界点所标示的弧度外
	*/
	if ((relLocation >= arcEndOfst) && (relLocation <= arcStartOfst))
	{

		*GradeVal = (FLOAT32)(10000.0f * curGradeVal);
		/*返回成功*/
		bReturnValue = 2u;
		return bReturnValue;
	}
	/*当前点在第一个分界点弧度内*/
	else if (relLocation < arcEndOfst)
	{
		if (NULL != frontGrade)
		{
			/*计算角度变化值*/
			if (0u == frontGrade->dwRadius)
			{
				/*曲率半径为0，角度变化值为0*/
				angleValue = 0u;
			}
			else
			{
				/*角度变化值*/
                angleValue = (FLOAT32)((arcEndOfst + relLocation) / (1.0f * frontGrade->dwRadius));
			}
			/*坡度是上坡，取正值*/
			if (0x55u == frontGrade->wInclineDir)
			{
				tempValue = ((FLOAT64)frontGrade->wGradValue) / 10000.0F;
			}
			/*下坡取负值*/
			else
			{
				tempValue = (-(FLOAT64)frontGrade->wGradValue) / 10000.0F;
			}
			/*if用来判断角度变化值是增加值还是减少值*/
			if (dquGradeTrendExp(pDsuEmapStru, frontGrade->wId, curGrade->wId, &GradeTrend))
			{
				/*坡度减少*/
				if (1u == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue - angleValue;
				}
				/*坡度增加*/
				else if (2u == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue + angleValue;
				}
				/*坡度不变化*/
				else
				{
					tempAngleValue = tempValue;
				}

				/*转换为FLOAT32类型*/
				*GradeVal = (FLOAT32)(tempAngleValue * 10000.0f);
				/*返回成功*/
				bReturnValue = 1u;
				return bReturnValue;
			}
			else
			{
				/*不可达*/
				bReturnValue = 0u;
				return bReturnValue;
			}
		}
		else
		{
			/*转换为FLOAT32类型*/
			*GradeVal = (FLOAT32)(curGradeVal);
			/*返回成功*/
			bReturnValue = 1u;
			return bReturnValue;
		}
	}
	/*在第三分界段内*/
	else
	{
		if (NULL != rearGrade)
		{
			/*计算角度变化值*/
			if (0u == curGrade->dwRadius)
			{
				/*曲率半径为0，角度变化值为0*/
				angleValue = 0u;
			}
			else
			{
				/*角度变化值*/
				angleValue = (FLOAT32)((relLocation - arcStartOfst) / (1.0f * curGrade->dwRadius));
			}

			if (0x55u == curGrade->wInclineDir)
			{
				tempValue = ((FLOAT64)curGrade->wGradValue) / 10000.0F;
			}
			else
			{
				tempValue = (-(FLOAT64)curGrade->wGradValue) / 10000.0F;
			}

			/*if用来判断角度变化值是增加值还是减少值*/
			if (dquGradeTrendExp(pDsuEmapStru, curGrade->wId, rearGrade->wId, &GradeTrend))
			{
				/*坡度值减少*/
				if (1U == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue - angleValue;
				}
				/*坡度值增加*/
				else if (2U == GradeTrend)
				{
					/*计算平缓后的坡度值*/
					tempAngleValue = tempValue + angleValue;
				}
				/*坡度值不变化*/
				else
				{
					tempAngleValue = tempValue;
				}

				/*转换为FLOAT32类型*/
				*GradeVal = (FLOAT32)(tempAngleValue * 10000.0F);
				/*返回成功*/
				bReturnValue = 3U;
				return bReturnValue;
			}
			else
			{
				/*不可达*/
				bReturnValue = 0u;
				return bReturnValue;
			}
		}
		else
		{
			/*转换为FLOAT32类型*/
			*GradeVal = (FLOAT32)(curGradeVal);
			/*返回成功*/
			bReturnValue = 3u;
			return bReturnValue;
		}
	}
}

/*************
*访问接口
*函数功能：计算某点坡度平缓后的坡度值
*输入参数：Point 所求坡度点
*输出参数：
*返回值：  0函数执行失败 1 点在坡度第一段 2 点在坡度第2段 3 点在坡度第3段
*备注：增加对单轨项目标识的处理，modify by qxt 20170726
****************/
UINT8 dsuCalculateGradeValueExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkId, const UINT32 ofst, FLOAT32 *GradeVal)
{
	UINT8 rtnValue = 0u;		/*函数返回值*/
	UINT8 cbtcVersionType = 0u;

	if ((NULL != pDsuEmapStru) && (NULL != GradeVal))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_FAO == cbtcVersionType)
		{
			rtnValue = dsuCalculateGradeValueExp_FAO(pDsuEmapStru, LinkId, ofst, GradeVal);
		}
		else if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			rtnValue = dsuCalculateGradeValueExp_CPK(pDsuEmapStru, LinkId, ofst, GradeVal);
		}
		else
		{
			rtnValue = 0u;
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/*************
*函数功能：计算某坡度平缓后，分成的几个部分的分界点与起点的距离
*输入参数：gradeId 坡度ID
*输出参数：UINT32* endPointDistance UINT32* startPointDistance
*返回值：  0函数执行失败   1成功
备注：     如果不需要平缓计算的点，设为无效值ffffffff  返回值为1
****************/
UINT8 dsuDividPointDistanceFromOriginExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 GradeId, const UINT16 linkId, UINT32* endPointDistance, UINT32* startPointDistance)
{
	DSU_GRADE_STRU* curGrade = NULL;		/*当前坡度临时变量*/
	DSU_GRADE_STRU* frontGrade = NULL;		/*前面坡度*/
	DSU_GRADE_STRU* rearGrade = NULL;		/*后面面坡度*/
	UINT16 gradeValue = 0u;					/*坡度差值*/
	UINT32 arcLength = 0u;					/*弧长*/
	UINT8 bReturnValue = 0u;				/*函数返回值*/
	UINT8 RtnGetGradFrnt = 0u;				/*获取前面坡度结果*/
	UINT8 RtnGetGradNxt = 0u;				/*获取后面坡度结果*/

	DSU_GRADE_STRU curGradeTmp = { 0 };		/*当前坡度临时变量*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 GRADEINDEXNUM = 0u;
	UINT16* dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != endPointDistance) && (NULL != startPointDistance))
	{
		GRADEINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->GRADEINDEXNUM;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*若输入的坡度编号不为空,则在坡度表中处理*/
	if (0xffffu != GradeId)
	{
		if ((GradeId > GRADEINDEXNUM) || (0xFFFFu == dsuGradeIndex[GradeId]))
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}

		/*初始化当前坡度*/
		curGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[GradeId];

		/*前面有相邻坡度*/
		if (0xFFFFu != curGrade->wOrgnJointMainGradeId)
		{
			/*初始化前面相邻坡度*/
			frontGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[curGrade->wOrgnJointMainGradeId];
		}
		/*前面没有相邻坡度*/
		else
		{
			/*终点设为无效值*/
			*endPointDistance = 0xFFFFFFFFu;
			frontGrade = NULL;
		}

		/*后面有相邻坡度*/
		if (0xFFFFu != curGrade->wTmnlJointMainGradeId)
		{
			/*初始化后面相邻坡度*/
			rearGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[curGrade->wTmnlJointMainGradeId];
		}
		/*后面没有相邻坡度*/
		else
		{
			/*起点设为无效值*/
			*startPointDistance = 0xFFFFFFFFu;
			rearGrade = NULL;
		}
	}
	else
		/*当前所在坡度为统一坡度*/
	{
		CommonMemSet(&curGradeTmp, sizeof(curGradeTmp), 0u, sizeof(curGradeTmp));

		/*根据信息,填写当前坡度信息*/
		curGradeTmp.wInclineDir = EMAP_SAME_DIR;
		curGradeTmp.wGradValue = dsuStaticHeadStru->pConGradeStru->wGrade;
		curGradeTmp.dwRadius = 0u;

		curGrade = &curGradeTmp;

		/*使用前的初始化*/
		frontGrade = NULL;
		rearGrade = NULL;

		/*获取前面相邻坡度*/
		RtnGetGradFrnt = dsuGetGradeFrntExp(pDsuEmapStru, linkId, &frontGrade);

		/*获取后面相邻坡度*/
		RtnGetGradNxt = dsuGetGradeRearExp(pDsuEmapStru, linkId, &rearGrade);

		/*若前面或后面相邻坡度查询失败*/
		if ((0u == RtnGetGradFrnt) || (0u == RtnGetGradNxt))
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}
	}

	/**************计算linkEndId 和  endOfst******************/
	/*前面有相邻坡度*/
	if (NULL != frontGrade)
	{
		/*竖曲线半径无效*/
		if (0xFFFFFFFFu == frontGrade->dwRadius)
		{
			/*终点设为无效值*/
			*endPointDistance = 0xFFFFFFFFu;
		}
		else
		{
			/*计算坡度差值，都是上坡或下坡*/
			if ((frontGrade->wInclineDir) == (curGrade->wInclineDir))
			{
				gradeValue = (UINT16)abs((INT16)curGrade->wGradValue - (INT16)frontGrade->wGradValue);
			}
			else
			{
				/*一个上坡，一个下坡*/
				gradeValue = curGrade->wGradValue + frontGrade->wGradValue;
			}
			/*计算弧长*/
			arcLength = ((UINT32)gradeValue) * ((frontGrade->dwRadius) / 20000u);
			*endPointDistance = arcLength;
		}
	}
	else
	{
		/*终点设为无效值*/
		*endPointDistance = 0xFFFFFFFFu;
	}
	/**************计算完毕linkEndId 和  endOfst******************/


	/**************计算linkStartId 和  startOfst******************/
	/*后面有相邻坡度*/
	if (NULL != rearGrade)
	{
		/*竖曲线半径无效*/
		if (0xFFFFFFFFu == rearGrade->dwRadius)
		{
			/*终点设为无效值*/
			*startPointDistance = 0xFFFFFFFFu;
		}
		else
		{
			/*计算坡度差值，都是上坡或下坡*/
			if ((rearGrade->wInclineDir) == (curGrade->wInclineDir))
			{
				gradeValue = (UINT16)abs((INT16)curGrade->wGradValue - (INT16)rearGrade->wGradValue);
			}
			else
			{
				/*一个上坡，一个下坡*/
				gradeValue = curGrade->wGradValue + rearGrade->wGradValue;
			}
			/*计算弧长*/
			arcLength = ((UINT32)gradeValue) * ((curGrade->dwRadius) / 20000U);
			*startPointDistance = curGrade->dwGradeLength - arcLength;
		}
	}
	else
	{
		/*起点设为无效值*/
		*startPointDistance = 0xFFFFFFFFu;
	}
	/**************计算完毕linkStartId 和  startOfst******************/

	bReturnValue = 1u;
	return bReturnValue;
}

/*************
*函数功能：给定坡度上某一点及坡度id，计算该点距离坡度起点的距离
*输入参数：gradeId 坡度ID
UINT16 linkId UINT32 Ofst
*输出参数：UINT32 *length  长度

*返回值：  0函数执行失败   1成功
****************/
UINT8 dsuGetDistanceFromOriginExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 GradeId, const UINT16 linkId, const UINT32 Ofst, UINT32* length)
{
	UINT16 j = 0u;						/*循环变量*/
	DSU_GRADE_STRU* curGrade = NULL;	/*坡度结构体临时变量*/
	DSU_LINK_STRU * plink = NULL;		/*link结构体指针临时变量*/
	UINT8 bReturnValue = 0u;			/*函数返回值*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 GRADEINDEXNUM = 0u;
	UINT16	*dsuLinkIndex = NULL;
	UINT16	*dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != length))
	{
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		GRADEINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->GRADEINDEXNUM;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	if (0xffffu != GradeId)
	{
		if ((GradeId > GRADEINDEXNUM) || (0xFFFFu == dsuGradeIndex[GradeId]))
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}

		curGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[GradeId];
		if (0u == curGrade->dwOrgnLinkOfst)
		{
			*length = 0u;
		}
		else
		{
			*length = 1u;
		}
		/*点在起始link上*/
		if ((linkId == curGrade->wOrgnLinkId) && (Ofst >= curGrade->dwOrgnLinkOfst))
		{
			*length += Ofst - curGrade->dwOrgnLinkOfst;
			bReturnValue = 1u;
			return bReturnValue;
		}
		else
		{
			plink = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[curGrade->wOrgnLinkId];
			*length += plink->dwLength - curGrade->dwOrgnLinkOfst;
			j = 0u;
			/*先从中间link上找找*/
			while (0xFFFFu != curGrade->wIncludeLinkId[j])
			{
				if (GRADE_INCLUDE_LINK_NUM <= j)
				{
					/*数据错误，退出 不可能有这么多包含link*/
					bReturnValue = 0u;
					return bReturnValue;
				}
				plink = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[curGrade->wIncludeLinkId[j]];
				/*落在中间某link上*/
				if ((linkId == curGrade->wIncludeLinkId[j]) && (Ofst <= plink->dwLength))
				{
					*length += Ofst;
					bReturnValue = 1u;
					return bReturnValue;
				}
				else
				{
					/*否则把中间link的长度累加起来*/
					*length += plink->dwLength;
					j++;
				}
			}
			/*中间link没找到，看末端link*/
			if ((linkId == curGrade->wTmnlLinkId) && (Ofst <= curGrade->dwTmnlLinkOfst))
			{
				*length += Ofst;
				bReturnValue = 1u;
				return bReturnValue;
			}
			else
			{
				bReturnValue = 0u;
				return bReturnValue;
			}
		}
	}
	else
	{
		*length = Ofst;
		bReturnValue = 1u;
		return bReturnValue;
	}
}

/************************************************************************
* 函数功能：	此函数的功能是查询给定的两位置间的最差坡度。函数查询起点至
* 				终点范围内所有的线路坡度，并挑出最差坡度（最大下坡）。规定
* 				上坡为正，下坡为负，函数的功能即为挑出范围内坡度的最小值。
* 				函数返回最差坡度的千分数。
* 
* 入口参数：	const LOD_STRU * pLODStartStru		查询起点位置和方向（实际为列车车尾位置和方向）
* 				const LOD_STRU * pLODEndStru		查询终点位置和方向
* 				const UCHAR PointNum				MA中的道岔数量
* 				const POINT_STRU * pPointStru		道岔信息结构体
* 出口参数：	FLOAT64 * Gradient					列车车尾至MA终点范围内最差坡度的千分数（上坡为正，下坡为负）
* 返回值：		UCHAR bReturnValue
* 				0：查询失败；
* 				1：查询正确；
* 备注：		在计算的过程中，用的万分值，返回值前，有/10的操作，因此返回的坡度值为千分值
* 修改记录：	修复YF0069-2196：浮点型不能直接使用>=、<=进行判断&类型不一致，浮点型和整型不能直接作比较。wyd 20220720
************************************************************************/
/*14 两位置间最差坡度查询*/
static UCHAR dsuGradientGetExp_FAO(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UCHAR PointNum, const DQU_POINT_STATUS_STRU * pPointStru, FLOAT32 * Gradient)
{
#if 0
	UINT16 wCurrentGradeID = 0u;		/*当前的坡度编号*/
	UINT16 wNextGradeID = 0u;			/*当前坡度的相邻坡度编号*/
	UCHAR bReturnValue = 0u;			/*函数返回值*/
	UINT8 bLocGradeReturn = 0u;			/*函数“获取当前位置的坡度信息”返回值*/
	UCHAR rstGradeNxt = 0u;				/*获取相邻link坡度的返回值*/
	UCHAR rstGetIdNxt = 0u;				/*获取相邻link的返回值*/
	DSU_GRADE_STRU *pstGrade = NULL;	/*指向坡度信息的结构体指针*/
	DSU_GRADE_STRU stGrade = { 0 };		/*指向坡度信息的结构体变量*/
	UINT16 i = 0u;
	UINT16 wCurrentLinkId = 0u;			/*当前坡度的link的ID*/

	FLOAT32 StartPointGradeVal = 0.0f;
	FLOAT32 EndPointGradeVal = 0.0f;
	UINT16 StartGradeId = 0u;
	UINT16 EndGradeId = 0u;
	UINT8  StartPointGradeRtn = 0u;
	UINT8  EndPointGradeRtn = 0u;
	FLOAT32 Coefficient = 1.0f;			/*坡度系数，上坡为1 下坡为-1*/
	FLOAT32 CurGradeVal = 0.0f;
	FLOAT32 minGradeVal = 0.0f;
	FLOAT32 maxGradeVal = 0.0f;

	UINT16  StartLinkId = 0u;
	UINT32  StartOfst = 0u;
	UINT16  EndLinkId = 0u;
	UINT32  EndOfst = 0u;

	/*全局变量改传参调用涉及的全局变量*/
    UINT16	LINKINDEXNUM = 0u;
	UINT16	*dsuLinkIndex = NULL;
	UINT16	*dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != Gradient))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*判断入口数据起点Link的有效性*/
	if ((pLODStartStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pLODStartStru->Lnk]))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*判断入口数据起点link偏移量的有效性*/
	if (pLODStartStru->Off > LinkLengthExp(pDsuEmapStru, pLODStartStru->Lnk))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*判断入口数据起点方向的有效性*/
	if ((EMAP_SAME_DIR != pLODStartStru->Dir) && (EMAP_CONVER_DIR != pLODStartStru->Dir))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*判断入口数据Link的有效性*/
	if ((pLODEndStru->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pLODEndStru->Lnk]))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*判断入口数据终点link偏移量的有效性*/
	if (pLODEndStru->Off > LinkLengthExp(pDsuEmapStru, pLODEndStru->Lnk))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*判断入口数据终点方向的有效性*/
	if ((EMAP_SAME_DIR != pLODEndStru->Dir) && (EMAP_CONVER_DIR != pLODEndStru->Dir))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}


	/*方向为正，系数取1*/
	if (0x55u == pLODStartStru->Dir)
	{
		Coefficient = 1.0f;
		StartLinkId = pLODStartStru->Lnk;
		StartOfst = pLODStartStru->Off;
		EndLinkId = pLODEndStru->Lnk;
		EndOfst = pLODEndStru->Off;
	}
	/*方向为反，系数取-1 起点终点调换位置*/
	else
	{
		Coefficient = -1.0f;
		StartLinkId = pLODEndStru->Lnk;
		StartOfst = pLODEndStru->Off;
		EndLinkId = pLODStartStru->Lnk;
		EndOfst = pLODStartStru->Off;
	}

	/*****************以下计算起点和终点坡度值*********************/
	bLocGradeReturn = dsuGetGradeIdbyLocExp(pDsuEmapStru, StartLinkId, StartOfst, &StartGradeId);
	/*函数调用失败，返回错误*/
	if (0u == bLocGradeReturn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	StartPointGradeRtn = dsuCalculateGradeValueExp(pDsuEmapStru, StartLinkId, StartOfst, &StartPointGradeVal);
	/*函数调用失败，返回错误*/
	if (0u == StartPointGradeRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*终点坡度值计算*/
	bLocGradeReturn = dsuGetGradeIdbyLocExp(pDsuEmapStru, EndLinkId, EndOfst, &EndGradeId);
	/*函数调用失败，返回错误*/
	if (0u == bLocGradeReturn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	EndPointGradeRtn = dsuCalculateGradeValueExp(pDsuEmapStru, EndLinkId, EndOfst, &EndPointGradeVal);
	/*函数调用失败，返回错误*/
	if (0u == EndPointGradeRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	/*****************起点和终点坡度值计算完毕*********************/


	/****如果两点在同一坡度内****/
	if ((StartGradeId == EndGradeId) &&
		(0xffffu != StartGradeId) && (0xffffu != EndGradeId))
	{
		/*在同一个分界段内*/
		if (StartPointGradeRtn == EndPointGradeRtn)
		{
			/*返回最小的那个*/
			if (EndPointGradeVal < StartPointGradeVal)
			{
				minGradeVal = EndPointGradeVal;
				maxGradeVal = StartPointGradeVal;
			}
			else
			{
				minGradeVal = StartPointGradeVal;
				maxGradeVal = EndPointGradeVal;
			}
		}
		/*不在同一分界段内，需考虑中间分界段坡度值*/
		else
		{
			pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[StartGradeId];
			/*当前坡度值，上坡为正，下坡为负*/
			if (0x55u == pstGrade->wInclineDir)
			{
				CurGradeVal = (FLOAT32)pstGrade->wGradValue;
			}
			else
			{
				CurGradeVal = -((FLOAT32)pstGrade->wGradValue);
			}

			/*取得最小值*/
			if ((EndPointGradeVal < CurGradeVal) && (EndPointGradeVal < StartPointGradeVal))
			{
				minGradeVal = EndPointGradeVal;
			}
			else if ((StartPointGradeVal < CurGradeVal) && (StartPointGradeVal < EndPointGradeVal))
			{
				minGradeVal = StartPointGradeVal;
			}
			else
			{
				minGradeVal = CurGradeVal;
			}
			/*取最大值*/
			if ((EndPointGradeVal > CurGradeVal) && (EndPointGradeVal > StartPointGradeVal))
			{
				maxGradeVal = EndPointGradeVal;
			}
			else if ((StartPointGradeVal > CurGradeVal) && (StartPointGradeVal > EndPointGradeVal))
			{
				maxGradeVal = StartPointGradeVal;
			}
			else
			{
				maxGradeVal = CurGradeVal;
			}

		}
	}
	/*****不在同一坡度*****/
	else
	{
		if (0xffffu != StartGradeId)
		{
			pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[StartGradeId];
			/*当前坡度值，上坡为正，下坡为负*/
			if (0x55 == pstGrade->wInclineDir)
			{
				CurGradeVal = (FLOAT32)pstGrade->wGradValue;
			}
			else
			{
				CurGradeVal = -((FLOAT32)pstGrade->wGradValue);
			}
			/*取得当前坡度段起点前方的最大最小坡度值*/
			if (1u == StartPointGradeRtn)
			{
				if (CurGradeVal > StartPointGradeVal)
				{
					minGradeVal = StartPointGradeVal;
					maxGradeVal = CurGradeVal;
				}
				else
				{
					minGradeVal = CurGradeVal;
					maxGradeVal = StartPointGradeVal;
				}
			}
			else if (2u == StartPointGradeRtn)
			{
				minGradeVal = CurGradeVal;
				maxGradeVal = StartPointGradeVal;
			}
			else
			{
				minGradeVal = StartPointGradeVal;
				maxGradeVal = StartPointGradeVal;
			}
		}
		else
		{
			/*起始坡度为统一坡度,仅对最大最小值进行初始赋值*/
			minGradeVal = StartPointGradeVal;
			maxGradeVal = StartPointGradeVal;
		}

		if (0xffffu != EndGradeId)
		{
			pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[EndGradeId];
			/*终端所在坡度值，上坡为正，下坡为负*/
			if (0x55u == pstGrade->wInclineDir)
			{
				CurGradeVal = (FLOAT32)pstGrade->wGradValue;
			}
			else
			{
				CurGradeVal = -((FLOAT32)pstGrade->wGradValue);
			}
			/*取得当前坡度段起点前方的最大最小坡度值*/
			if (1u == EndPointGradeRtn)
			{
				/*从末端点中找到最小最大坡度值*/
				if (maxGradeVal < EndPointGradeVal)
				{
					maxGradeVal = EndPointGradeVal;
				}
				else if (minGradeVal > EndPointGradeVal)
				{
					minGradeVal = EndPointGradeVal;
				}
				else
				{
					/*继续执行*/
				}
			}
			else if (2u == EndPointGradeRtn)
			{
				/*从末端点中找到最小最大坡度值*/
				if (maxGradeVal < EndPointGradeVal)
				{
					maxGradeVal = EndPointGradeVal;
				}
				else if (minGradeVal > EndPointGradeVal)
				{
					minGradeVal = EndPointGradeVal;
				}
				else
				{
					/*继续执行*/
				}
			}
			else
			{
				/*算出最大值*/
				if ((EndPointGradeVal > maxGradeVal) && (EndPointGradeVal > CurGradeVal))
				{
					maxGradeVal = EndPointGradeVal;
				}
				else if ((CurGradeVal > maxGradeVal) && (CurGradeVal > EndPointGradeVal))
				{
					maxGradeVal = CurGradeVal;
				}
				else
				{
					/*继续执行*/
				}

				/*算出最小值*/
				if ((EndPointGradeVal < minGradeVal) && (EndPointGradeVal < CurGradeVal))
				{
					minGradeVal = EndPointGradeVal;
				}
				else if ((CurGradeVal < minGradeVal) && (CurGradeVal < EndPointGradeVal))
				{
					minGradeVal = CurGradeVal;
				}
				else
				{
					/*继续执行*/
				}
			}
		}
		else
		{
			/*终点坡度为统一坡度*/
			/*从末端点中找到最小最大坡度值*/
			if (maxGradeVal < EndPointGradeVal)
			{
				maxGradeVal = EndPointGradeVal;
			}
			else if (minGradeVal > EndPointGradeVal)
			{
				minGradeVal = EndPointGradeVal;
			}
			else
			{
				/*继续执行*/
			}
		}


		/*设当前坡度id为起始坡度id*/
		wCurrentGradeID = StartGradeId;
		/*记录当前link的id信息*/
		wCurrentLinkId = StartLinkId;

		/*循环获取起点终点间的坡度信息并比对得到最大,最小值*/
		for (i = 0u; i<dsuDataLenStru->wGradeLen; i++)
		{
			if (0xffffu != wCurrentGradeID)
			{
				/*在坡度表中查询当前坡度的前面坡度*/
				wNextGradeID = FindAdjacentGradeIDExp(pDsuEmapStru, wCurrentGradeID, 0x55u, PointNum, pPointStru);

				/*获取前方坡度*/
				if (0xffffu != wNextGradeID)
				{
					/*若查询得到的坡度关联ID不为空,利用管理函数找到当前的坡度信息*/
					pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[wNextGradeID];
					/*获取当前所用坡度的终点link信息*/
					wCurrentLinkId = pstGrade->wTmnlLinkId;
				}
				else
				{
					/*若在坡度表中获取到的终点关联坡度为无效,则可能为线路终点,或者在统一坡度表中的信息*/
					/*在link表中查询前面坡度是否在统一坡度表中,*/
					rstGradeNxt = dsuGetGradeRearExp(pDsuEmapStru, wCurrentLinkId, &pstGrade);

					if ((1u == rstGradeNxt) && (NULL == pstGrade))
					{
						/*若前面坡度在统一坡度表中,则获取统一坡度表信息,参与坡度比较*/
						stGrade.wInclineDir = EMAP_SAME_DIR;
						stGrade.wGradValue = dsuStaticHeadStru->pConGradeStru->wGrade;
						pstGrade = &stGrade;

						/*相邻坡度为统一坡度,编号置为无效*/
						wNextGradeID = 0xffffu;
					}
					else
					{
						/*若前面坡度未在统一坡度表中,则当前查询应为线路终点*/
						/*线路终点(找到线路终点也未能找到终点坡度ID)*/
						bReturnValue = 0u;
						return bReturnValue;
					}
					/*更新当前link的ID信息以便统一坡度表关系的查询*/
					rstGetIdNxt = dsuGetAdjacentLinkIDExp(pDsuEmapStru, wCurrentLinkId, 0x55u, PointNum, pPointStru, &wCurrentLinkId);

					if (1u != rstGetIdNxt)
					{
						/*查询线路link失败*/
						bReturnValue = 0u;
						return bReturnValue;
					}
				}
			}
			else
			{
				/*查询当前link的始端相邻坡度*/
				rstGradeNxt = dsuGetGradeRearExp(pDsuEmapStru, wCurrentLinkId, &pstGrade);

				if (1u == rstGradeNxt)
				{
					if (NULL == pstGrade)
					{
						/*若前面坡度在统一坡度表中,则获取统一坡度表信息,参与坡度比较*/
						stGrade.wInclineDir = EMAP_SAME_DIR;
						stGrade.wGradValue = dsuStaticHeadStru->pConGradeStru->wGrade;
						pstGrade = &stGrade;

						/*相邻坡度为统一坡度,编号置为无效*/
						wNextGradeID = 0xffffu;
					}
					else
					{
						/*按照实际查询得到pstGrade的坡度表中的坡度值进行处理*/
						/*记录当前link的相邻坡度ID*/
						wNextGradeID = pstGrade->wId;
					}

					/*为避免在坡度表中的查询不完全,若在统一坡度表中的处理不计入循环*/
					if (i >= 1u)
					{
						i--;
					}
				}
				else
				{
					/*线路终点(找到线路终点也未能找到终点坡度ID)*/
					bReturnValue = 0u;
					return bReturnValue;
				}

				if (wCurrentLinkId == EndLinkId)
				{
					/*查到终端坡度*/
					break;
				}

				/*20141122添加：更新当前link的ID信息以便统一坡度表关系的查询*/
                rstGetIdNxt = dsuGetAdjacentLinkIDExp(pDsuEmapStru, wCurrentLinkId, 0x55u, PointNum, pPointStru, &wCurrentLinkId);

				if (1u != rstGetIdNxt)
				{
					/*查询线路link失败*/
					bReturnValue = 0u;
					return bReturnValue;
				}
			}

			/*更新当前坡度ID信息以便坡度表关系的查询*/
			wCurrentGradeID = wNextGradeID;

			if (((wNextGradeID == EndGradeId) && ((0xffffu != wNextGradeID) && (0xffffu != EndGradeId))) ||
				((wCurrentLinkId == EndLinkId) && (0xffffu == wNextGradeID)))
			{
				/*查到终端坡度*/
				break;
			}

			if (0x55u == pstGrade->wInclineDir)
			{
				/*如果有更大的坡度或更小的坡度，替换当前值*/
				if (minGradeVal >= (FLOAT32)pstGrade->wGradValue)
				{
					minGradeVal = (FLOAT32)pstGrade->wGradValue;
				}
				if (maxGradeVal <= (FLOAT32)pstGrade->wGradValue)
				{
					maxGradeVal = (FLOAT32)pstGrade->wGradValue;
				}
			}
			else
			{
				/*如果有更大的坡度或更小的坡度，替换当前值*/
				if (minGradeVal >= -((FLOAT32)pstGrade->wGradValue))
				{
					minGradeVal = -(FLOAT32)(pstGrade->wGradValue);
				}
				if (maxGradeVal <= -((FLOAT32)pstGrade->wGradValue))
				{
					maxGradeVal = -(FLOAT32)(pstGrade->wGradValue);
				}
			}
		}

		if (i >= dsuDataLenStru->wGradeLen)
		{
			/*没找到末端坡度*/
			bReturnValue = 0u;
			return bReturnValue;
		}
	}

	/*根据运行方向确定最差坡度值*/
	if (0x55u == pLODStartStru->Dir)
	{
		*Gradient = minGradeVal / 10.0f;
		bReturnValue = 1u;
		return bReturnValue;
	}
	else
	{
		*Gradient = (FLOAT32)(-1.0f * maxGradeVal / 10.0f);
		bReturnValue = 1u;
		return bReturnValue;
	}
#else
	return 0U;
#endif
}

/************************************************************************
函数功能：	此函数的功能是查询给定的两位置间的最差坡度。函数查询起点至
		终点范围内所有的线路坡度，并挑出最差坡度（最大下坡）。规定
		上坡为正，下坡为负，函数的功能即为挑出范围内坡度的最小值。
		函数返回最差坡度的千分数。

入口参数：	const LOD_STRU * pLODStartStru		查询起点位置和方向（实际为列车车尾位置和方向）
		const LOD_STRU * pLODEndStru		查询终点位置和方向
		const UCHAR PointNum				MA中的道岔数量
		const POINT_STRU * pPointStru		道岔信息结构体
出口参数：	FLOAT64 * Gradient					列车车尾至MA终点范围内最差坡度的千分数（上坡为正，下坡为负）
返回值：	UCHAR bReturnValue					 	
		0：查询失败；
			1：查询正确；
注：在计算的过程中，用的万分值，返回值前，有/10的操作，因此返回
 的坡度值为千分值       
修改说明：
1、增加识别方向变化点场景的坡度查询 m by songxy 2021-4-20                                              
************************************************************************/
static UCHAR dsuGradientGetExp_CPK(const DSU_EMAP_STRU *pDsuEmapStru,const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
					 const UCHAR PointNum, const DQU_POINT_STATUS_STRU * pPointStru, FLOAT32 * Gradient)
{
	UINT16 wCurrentGradeID = 0u;		/*当前的坡度编号*/
	UINT16 wNextGradeID = 0u;			/*当前坡度的相邻坡度编号*/
	UCHAR bReturnValue = 0u;			/*函数返回值*/
	UINT8 bLocGradeReturn = 0u;			/*函数“获取当前位置的坡度信息”返回值*/
	UCHAR rstGradeNxt = 0u;				/*获取相邻link坡度的返回值*/
	UCHAR rstGetIdNxt = 0u;				/*获取相邻link的返回值*/
	DSU_GRADE_STRU *pstGrade = NULL;	/*指向坡度信息的结构体指针*/
	DSU_GRADE_STRU stGrade = { 0u };    /*指向坡度信息的结构体变量*/
	UINT16 i = 0u;
	UINT16 wCurrentLinkId = 0u;			/*当前坡度的link的ID*/

	FLOAT32 StartPointGradeVal = 0.0f;
	FLOAT32 EndPointGradeVal = 0.0f;
	UINT16 StartGradeId = 0u;
	UINT16 EndGradeId = 0u;
	UINT8  StartPointGradeRtn = 0u;
	UINT8  EndPointGradeRtn = 0u;
	FLOAT32 CurGradeVal = 0.0f;
	FLOAT32 minGradeVal = 0.0f;
	FLOAT32 maxGradeVal = 0.0f;
	UINT8   findDir = 0u;
	
	UINT16  StartLinkId = 0u;
	UINT32  StartOfst = 0u;
	UINT16  EndLinkId = 0u;
	UINT32  EndOfst = 0u;
	UINT32  tempLength = 0u;  /*link长度*/

	DSU_LINK_STRU *pLinkStru = NULL;	/*Link信息结构体指针*/
	DSU_LINK_STRU *pLinkStruEnd = NULL;	/*Link信息结构体指针 add by zlj 20161024*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16	LINKINDEXNUM = 0u;
	UINT16	*dsuLinkIndex = NULL;
	UINT16	*dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	FLOAT32 GradientTemp = 0.0f;

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != Gradient))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*判断入口数据起点Link的有效性*/
	if ((pLODStartStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODStartStru->Lnk]))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}
	
	/*判断入口数据起点link偏移量的有效性*/
	tempLength = LinkLengthExp(pDsuEmapStru, pLODStartStru->Lnk);
	if (pLODStartStru->Off > tempLength)
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}

	/*判断入口数据起点方向的有效性*/
	if ((EMAP_SAME_DIR != pLODStartStru->Dir) && (EMAP_CONVER_DIR != pLODStartStru->Dir))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}

	/*判断入口数据Link的有效性*/
	if ((pLODEndStru->Lnk > LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODEndStru->Lnk]))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}

	/*判断入口数据终点link偏移量的有效性*/
	tempLength = LinkLengthExp(pDsuEmapStru, pLODEndStru->Lnk);
	if (pLODEndStru->Off > tempLength)
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}

	/*判断入口数据终点方向的有效性*/
	if ((EMAP_SAME_DIR != pLODEndStru->Dir) && (EMAP_CONVER_DIR != pLODEndStru->Dir))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}

	/*方向为正，系数取1 起点在后, 顺向查询,同向时不用转换方向*/
	if(EMAP_SAME_DIR == pLODStartStru->Dir)
	{
		StartLinkId = pLODStartStru->Lnk;
		StartOfst = pLODStartStru->Off;
		EndLinkId = pLODEndStru->Lnk;
		EndOfst = pLODEndStru->Off;

		findDir = pLODStartStru->Dir;
	}
	/*方向为反，系数取-1 起点终点调换位置*/
	else 
	{
		StartLinkId = pLODEndStru->Lnk;
		StartOfst = pLODEndStru->Off;
		EndLinkId = pLODStartStru->Lnk;
		EndOfst = pLODStartStru->Off;

		if (pLODEndStru->Dir != pLODStartStru->Dir)
		{
			/*起终点之间 奇数个方向变化点 仍按照车头方向查询*/
			findDir = pLODStartStru->Dir;
		}
		else
		{
			findDir = EMAP_SAME_DIR;
		}
	}

	/*****************以下计算起点和终点坡度值*********************/
	bLocGradeReturn = dsuGetGradeIdbyLocExp(pDsuEmapStru, StartLinkId, StartOfst, &StartGradeId);
	/*函数调用失败，返回错误*/
	if (0u == bLocGradeReturn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}
	
	StartPointGradeRtn = dsuCalculateGradeValueExp(pDsuEmapStru, StartLinkId, StartOfst, &StartPointGradeVal);
	/*函数调用失败，返回错误*/
	if (0u == StartPointGradeRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}
	
	/*终点坡度值计算*/
	bLocGradeReturn = dsuGetGradeIdbyLocExp(pDsuEmapStru, EndLinkId, EndOfst, &EndGradeId);
	/*函数调用失败，返回错误*/
	if (0u == bLocGradeReturn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}
	
	EndPointGradeRtn = dsuCalculateGradeValueExp(pDsuEmapStru, EndLinkId, EndOfst, &EndPointGradeVal);
	/*函数调用失败，返回错误*/
	if (0u == EndPointGradeRtn)
	{
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*nothing*/
	}
	/*****************起点和终点坡度值计算完毕*********************/

	/****如果两点在同一坡度内****/
	if ((StartGradeId == EndGradeId) &&
		(DSU_NULL_16 != StartGradeId) && (DSU_NULL_16 != EndGradeId))
	{
		/*在同一个分界段内*/
		if (StartPointGradeRtn == EndPointGradeRtn)
		{
			/*返回最小的那个*/
			if (EndPointGradeVal < StartPointGradeVal)
			{
				minGradeVal = EndPointGradeVal;
				maxGradeVal = StartPointGradeVal;
			}
			else
			{
				minGradeVal = StartPointGradeVal;
				maxGradeVal = EndPointGradeVal;
			}
		}
		/*不在同一分界段内，需考虑中间分界段坡度值*/
		else
		{
			pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[StartGradeId];
			/*当前坡度值，上坡为正，下坡为负*/
			if(EMAP_SAME_DIR == pstGrade->wInclineDir)
			{
				CurGradeVal = (FLOAT32)pstGrade->wGradValue;
			}
			else
			{
				CurGradeVal = -((FLOAT32)pstGrade->wGradValue);
			}

			/*取得最小值*/
			if ((EndPointGradeVal < CurGradeVal) && (EndPointGradeVal < StartPointGradeVal))
			{
				minGradeVal = EndPointGradeVal;
			}
			else if ((StartPointGradeVal < CurGradeVal) && (StartPointGradeVal < EndPointGradeVal))
			{
				minGradeVal = StartPointGradeVal;
			}
			else
			{
				minGradeVal = CurGradeVal;
			}
			/*取最大值*/
			if ((EndPointGradeVal > CurGradeVal) && (EndPointGradeVal > StartPointGradeVal))
			{
				maxGradeVal = EndPointGradeVal;
			}
			else if ((StartPointGradeVal > CurGradeVal) && (StartPointGradeVal > EndPointGradeVal))
			{
				maxGradeVal = StartPointGradeVal;
			}
			else
			{
				maxGradeVal = CurGradeVal;
			}

		}
	}
	/*****不在同一坡度*****/
	else
	{
		if (DSU_NULL_16 != StartGradeId)
		{
			pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[StartGradeId];
			/*当前坡度值，上坡为正，下坡为负*/
			if(EMAP_SAME_DIR == pstGrade->wInclineDir)
			{
				CurGradeVal = (FLOAT32)pstGrade->wGradValue;
			}
			else
			{
				CurGradeVal = -((FLOAT32)pstGrade->wGradValue);
			}
			/*取得当前坡度段起点前方的最大最小坡度值*/
			if (1u == StartPointGradeRtn)
			{
				if (CurGradeVal > StartPointGradeVal)
				{
					minGradeVal = StartPointGradeVal;
					maxGradeVal = CurGradeVal;
				}
				else
				{
					minGradeVal = CurGradeVal;
					maxGradeVal = StartPointGradeVal;
				}
			}
			else if (2u == StartPointGradeRtn)
			{
				minGradeVal = CurGradeVal;
				maxGradeVal = StartPointGradeVal;
			}
			else
			{
				minGradeVal = StartPointGradeVal;
				maxGradeVal = StartPointGradeVal;
			}
		}
		else
		{
			/*起始坡度为统一坡度,仅对最大最小值进行初始赋值*/
			minGradeVal = StartPointGradeVal;
			maxGradeVal = StartPointGradeVal;
		}

		if (DSU_NULL_16 != EndGradeId)
		{
			pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[EndGradeId];
			/*终端所在坡度值，上坡为正，下坡为负*/
			if(EMAP_SAME_DIR == pstGrade->wInclineDir)
			{
				CurGradeVal = (FLOAT32)pstGrade->wGradValue;
			}
			else
			{
				CurGradeVal = -((FLOAT32)pstGrade->wGradValue);
			}
			/*取得当前坡度段起点前方的最大最小坡度值*/
			if (1u == EndPointGradeRtn)
			{
				/*从末端点中找到最小最大坡度值*/
				if (maxGradeVal < EndPointGradeVal)
				{
					maxGradeVal = EndPointGradeVal;
				}
				else if (minGradeVal > EndPointGradeVal)
				{
					minGradeVal = EndPointGradeVal;
				}
				else
				{
					/*继续执行*/
				}
			}
			else if (2u == EndPointGradeRtn)
			{
				/*从末端点中找到最小最大坡度值*/
				if (maxGradeVal < EndPointGradeVal)
				{
					maxGradeVal = EndPointGradeVal;
				}
				else if (minGradeVal > EndPointGradeVal)
				{
					minGradeVal = EndPointGradeVal;
				}
				else
				{
					/*继续执行*/
				}
			}
			else
			{
				/*算出最大值*/
				if ((EndPointGradeVal > maxGradeVal) && (EndPointGradeVal > CurGradeVal))
				{
					maxGradeVal = EndPointGradeVal;
				}
				else if ((CurGradeVal > maxGradeVal) && (CurGradeVal > EndPointGradeVal))
				{
					maxGradeVal = CurGradeVal;
				}
				else
				{
					/*继续执行*/
				}

				/*算出最小值*/
				if ((EndPointGradeVal < minGradeVal) && (EndPointGradeVal < CurGradeVal))
				{
					minGradeVal = EndPointGradeVal;
				}
				else if ((CurGradeVal < minGradeVal) && (CurGradeVal < EndPointGradeVal))
				{
					minGradeVal = CurGradeVal;
				}
				else
				{
					/*继续执行*/
				}
			}
		}
		else
		{
			/*终点坡度为统一坡度*/
			/*从末端点中找到最小最大坡度值*/
			if (maxGradeVal < EndPointGradeVal)
			{
				maxGradeVal = EndPointGradeVal;
			}
			else if (minGradeVal > EndPointGradeVal)
			{
				minGradeVal = EndPointGradeVal;
			}
			else
			{
				/*继续执行*/
			}
		}


		/*设当前坡度id为起始坡度id*/
		wCurrentGradeID = StartGradeId;
		/*记录当前link的id信息*/
		wCurrentLinkId = StartLinkId;

		pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[wCurrentLinkId];

		pLinkStruEnd = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[EndLinkId];/*add by zlj 20161024*/
		
		/*当前link的“Link坡度信息属性”为无统一坡度*/
		/*if(0 == pLinkStru->wGrade)*/ /*changed by zlj 20161024*/
		if ((0u == pLinkStru->wGrade) || (0u == pLinkStruEnd->wGrade))
		{
			/*循环获取起点终点间的坡度信息并比对得到最大,最小值*/
			for (i = 0u; i < dsuDataLenStru->wGradeLen; i++)
			{
				if (DSU_NULL_16 != wCurrentGradeID)
				{
					/*在坡度表中查询当前坡度的前面坡度*/
					wNextGradeID = FindAdjacentGradeIDExp(pDsuEmapStru,wCurrentGradeID, findDir, PointNum, pPointStru);
					
					/*获取前方坡度*/
					if (DSU_NULL_16 != wNextGradeID)
					{						
                        /*获取当前所用坡度的终点link信息*/
                        if (EMAP_CONVER_DIR == findDir)
                        {
                            wCurrentLinkId = pstGrade->wOrgnLinkId;
                        }
                        else
                        {
                            wCurrentLinkId = pstGrade->wTmnlLinkId;
                        }

                        /*若查询得到的坡度关联ID不为空,利用管理函数找到当前的坡度信息*/
                        pstGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[wNextGradeID];

                        /*先根据上个坡度所处的link是否含方向变化点判断是否改变查询方向*/
                        pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[wCurrentLinkId];
                        if (((EMAP_CONVER_DIR == pLinkStru->wLogicDirChanged) && (EMAP_CONVER_DIR == findDir))
                            || ((EMAP_SAME_DIR == pLinkStru->wLogicDirChanged) && (EMAP_SAME_DIR == findDir)))
                        {
                            findDir = EMAP_SAME_DIR;
                        }
                        else
                        {
                            /*nothing*/
                        }
					}
					else
					{
						/*若在坡度表中获取到的终点关联坡度为无效,则可能为线路终点,或者在统一坡度表中的信息*/
                        /*获取当前所用坡度的终点link信息*/
                        if (EMAP_SAME_DIR == findDir)
                        {
                            /*在link表中查询前面坡度是否在统一坡度表中,*/
                            rstGradeNxt = dsuGetGradeRearExp(pDsuEmapStru, wCurrentLinkId, &pstGrade);
                        }
                        else
                        {
                            /*在link表中查询前面坡度是否在统一坡度表中,*/
                            rstGradeNxt = dsuGetGradeFrntExp(pDsuEmapStru, wCurrentLinkId, &pstGrade);
                        }
						
						if((1u == rstGradeNxt)&&(NULL == pstGrade))
						{
							/*若前面坡度在统一坡度表中,则获取统一坡度表信息,参与坡度比较*/
							stGrade.wInclineDir = EMAP_SAME_DIR;
							stGrade.wGradValue = dsuStaticHeadStru->pConGradeStru->wGrade;
							pstGrade = &stGrade;
							
							/*相邻坡度为统一坡度,编号置为无效*/
							wNextGradeID = DSU_NULL_16;
						}
						else
						{
							/*若前面坡度未在统一坡度表中,则当前查询应为线路终点*/
							/*线路终点(找到线路终点也未能找到终点坡度ID)*/
							bReturnValue = 0u;
							return bReturnValue;
						}
						
						/*更新当前link的ID信息以便统一坡度表关系的查询*/
                        pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[wCurrentLinkId];
						rstGetIdNxt = dsuGetAdjacentLinkIDExp(pDsuEmapStru,wCurrentLinkId, findDir, PointNum, pPointStru, &wCurrentLinkId);
                        /*先根据上个坡度所处的link是否含方向变化点判断是否改变查询方向*/
                        if (((EMAP_CONVER_DIR == pLinkStru->wLogicDirChanged) && (EMAP_CONVER_DIR == findDir))
                            || ((EMAP_SAME_DIR == pLinkStru->wLogicDirChanged) && (EMAP_SAME_DIR == findDir)))
                        {
                            findDir = EMAP_SAME_DIR;
                        }
                        else
                        {
                            /*nothing*/
                        }
						
						if(1u != rstGetIdNxt)
						{
							/*查询线路link失败*/
							bReturnValue = 0u;
							return bReturnValue;
						}
						else
						{
							/*nothing*/
						}
					}
				}
				else
				{
					/*查询当前link的始端相邻坡度*/
					rstGradeNxt = dsuGetGradeRearExp(pDsuEmapStru, wCurrentLinkId, &pstGrade);

					if (1u == rstGradeNxt)
					{
						if (NULL == pstGrade)
						{
							/*若前面坡度在统一坡度表中,则获取统一坡度表信息,参与坡度比较*/
							stGrade.wInclineDir = EMAP_SAME_DIR;
							stGrade.wGradValue = dsuStaticHeadStru->pConGradeStru->wGrade;
							pstGrade = &stGrade;

							/*相邻坡度为统一坡度,编号置为无效*/
							wNextGradeID = DSU_NULL_16;
						}
						else
						{
							/*按照实际查询得到pstGrade的坡度表中的坡度值进行处理*/
							/*记录当前link的相邻坡度ID*/
							wNextGradeID = pstGrade->wId;
						}
						
						/*为避免在坡度表中的查询不完全,若在统一坡度表中的处理不计入循环*/
						if (1u <= i)
						{
							i--;
						}
						else
						{
							/*nothing*/
						}
					}
					else
					{
						/*线路终点(找到线路终点也未能找到终点坡度ID)*/
						bReturnValue = 0u;
						return bReturnValue;
					}

					if (wCurrentLinkId == EndLinkId)
					{
						/*查到终端坡度*/
						break;
					}
					else
					{
						/*nothing*/
					}

					/*20141122添加：更新当前link的ID信息以便统一坡度表关系的查询*/
                    pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[wCurrentLinkId];
					rstGetIdNxt = dsuGetAdjacentLinkIDExp(pDsuEmapStru,wCurrentLinkId, findDir, PointNum, pPointStru, &wCurrentLinkId);
                    /*先根据上个坡度所处的link是否含方向变化点判断是否改变查询方向*/
                    if (((EMAP_CONVER_DIR == pLinkStru->wLogicDirChanged) && (EMAP_CONVER_DIR == findDir))
                        || ((EMAP_SAME_DIR == pLinkStru->wLogicDirChanged) && (EMAP_SAME_DIR == findDir)))
                    {
                        findDir = EMAP_SAME_DIR;
                    }
                    else
                    {
                        /*nothing*/
                    }
					
					if(1u != rstGetIdNxt)
					{
						/*查询线路link失败*/
						bReturnValue = 0u;
						return bReturnValue;
					}
					else
					{
						/*nothing*/
					}
				}

				/*更新当前坡度ID信息以便坡度表关系的查询*/
				wCurrentGradeID = wNextGradeID;

				if (((wNextGradeID == EndGradeId) && ((DSU_NULL_16 != wNextGradeID) && (DSU_NULL_16 != EndGradeId))) ||
					((wCurrentLinkId == EndLinkId) && (DSU_NULL_16 == wNextGradeID)))
				{
					/*查到终端坡度*/
					break;
				}
				else
				{
					/*nothing*/
				}
				
				if(EMAP_SAME_DIR == pstGrade->wInclineDir)
				{
					/*如果有更大的坡度或更小的坡度，替换当前值*/
					if(minGradeVal >= (FLOAT32)pstGrade->wGradValue)
					{
						minGradeVal = (FLOAT32)pstGrade->wGradValue;
					}
					else
					{
						/*nothing*/
					}
					
					if(maxGradeVal <= (FLOAT32)pstGrade->wGradValue)
					{
						maxGradeVal = (FLOAT32)pstGrade->wGradValue;
					}
					else
					{
						/*nothing*/
					}
				}
				else
				{
					/*如果有更大的坡度或更小的坡度，替换当前值*/
					if (minGradeVal >= -((FLOAT32)pstGrade->wGradValue))
					{
						minGradeVal = -((FLOAT32)pstGrade->wGradValue);
					}
					else
					{
						/*nothing*/
					}
					
					if (maxGradeVal <= -((FLOAT32)pstGrade->wGradValue))
					{
						maxGradeVal = -((FLOAT32)pstGrade->wGradValue);
					}
					else
					{
						/*nothing*/
					}
				}
			}
        }
		else
		{
			*Gradient = minGradeVal/10.0f;
			bReturnValue = 1u;
			return bReturnValue;
		}

		if(i >= dsuDataLenStru->wGradeLen)
		{
			/*没找到末端坡度*/
			bReturnValue = 0u;
			return bReturnValue;
		}
		else
		{
			/*nothing*/
		}
	}
	
   /*头尾之间存在一个方向变化点*/
    if (pLODStartStru->Dir!= pLODEndStru->Dir)
    {
        /*根据车头运行方向确定最差坡度值*/
        if (EMAP_SAME_DIR == pLODStartStru->Dir)
        {
            GradientTemp = minGradeVal / 10.0f;
        }
        else
        {
            GradientTemp = (FLOAT32)(-1.0f * maxGradeVal / 10.0f);
        }

        /*根据车尾运行方向确定最差坡度*/
        if (EMAP_SAME_DIR == pLODEndStru->Dir)
        {
            *Gradient = minGradeVal / 10.0f;
        }
        else
        {
            *Gradient = (FLOAT32)(-1.0f * maxGradeVal / 10.0f);
        }

        /*获取得出最差坡度*/
        if (*Gradient < GradientTemp)
        {
            *Gradient = GradientTemp;
        }
		else
		{
			/*do nothing*/
		}
        bReturnValue = 1u;
		
        return bReturnValue;
    }
    else
    {
        /*根据运行方向确定最差坡度值*/
        if (EMAP_SAME_DIR == pLODStartStru->Dir)
        {
            *Gradient = minGradeVal / 10.0f;
            bReturnValue = 1u;
            return bReturnValue;
        }
        else
        {
            *Gradient = (FLOAT32)(-1.0f * maxGradeVal / 10.0f);
            bReturnValue = 1u;
            return bReturnValue;
        }
    }
}

/*访问接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
UCHAR dsuGradientGetExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, FLOAT32 *Gradient)
{
	UINT8 cbtcVersionType = 0u;
	UCHAR bReturnValue = 0u;		/*函数返回值*/

	if ((NULL != pDsuEmapStru) && (NULL != pLODStartStru) && (NULL != pLODEndStru) && (NULL != pPointStru) && (NULL != Gradient))
	{
		cbtcVersionType = GetCbtcSysType();

		if (DQU_CBTC_FAO == cbtcVersionType)
		{
			bReturnValue = dsuGradientGetExp_FAO(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, Gradient);
		}
		else if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			bReturnValue = dsuGradientGetExp_CPK(pDsuEmapStru, pLODStartStru, pLODEndStru, PointNum, pPointStru, Gradient);
		}
		else
		{
			bReturnValue = 0u;
		}
	}
	else
	{
		bReturnValue = 0u;
	}
	return bReturnValue;
}


/*
函数功能：判断后一坡度的坡度值相比前一个坡度的坡度值是增加，不变化还是减少
输入参数：UINT16 fistGradeID,UINT16 secondGradeID
输出参数：GradeTrend 坡度
1： 减少
2： 增加
0： 不变化
返回值：0失败 1成功
*/
UINT8 dquGradeTrendExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 fistGradeID, const UINT16 secondGradeID, UINT8 *GradeTrend)
{
	UINT8 bReturnValue = 0u;
	INT32 signedFirstGrade = 0;			/*带有符号的坡度值--前一个坡度*/
	INT32 signedSecondGrade = 0;		/*带有符号的坡度值--后一个坡度*/
	DSU_GRADE_STRU *fistGrade = NULL;	/*指向坡度信息的结构体指针*/
	DSU_GRADE_STRU *secondGrade = NULL;	/*指向坡度信息的结构体指针*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 GRADEINDEXNUM = 0u;
	UINT16* dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	if ((NULL != pDsuEmapStru) && (NULL != GradeTrend))
	{
		GRADEINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->GRADEINDEXNUM;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}


	if (0xffffu != fistGradeID)
	{
		if ((fistGradeID > GRADEINDEXNUM) || (0xFFFFu == dsuGradeIndex[fistGradeID]))
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}

		/*初始化坡度*/
		fistGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[fistGradeID];

		/*0x55=EMAP_SAME_INC_LINE表示上坡,坡度值为正数
		0xaa=EMAP_CONVER_INC_LINE表示下坡，坡度值为负数*/

		/*前一个坡度*/
		if (EMAP_SAME_INC_LINE == fistGrade->wInclineDir)
		{
			signedFirstGrade = (INT32)fistGrade->wGradValue;
		}
		else if (EMAP_CONVER_INC_LINE == fistGrade->wInclineDir)
		{
			signedFirstGrade = (-1)*((INT32)(fistGrade->wGradValue));
		}
		else
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}
	}
	else
	{
		signedFirstGrade = dsuStaticHeadStru->pConGradeStru->wGrade;
	}

	if (0xffffu != secondGradeID)
	{
		if ((secondGradeID > GRADEINDEXNUM) || (0xFFFFu == dsuGradeIndex[secondGradeID]))
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0;
			return bReturnValue;
		}

		/*初始化坡度*/
		secondGrade = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[secondGradeID];

		/*后一个坡度*/
		if (EMAP_SAME_INC_LINE == secondGrade->wInclineDir)
		{
			signedSecondGrade = (INT32)secondGrade->wGradValue;
		}
		else if (EMAP_CONVER_INC_LINE == secondGrade->wInclineDir)
		{
			signedSecondGrade = (-1)*((INT32)(secondGrade->wGradValue));
		}
		else
		{
			/*入口参数异常，查询失败*/
			bReturnValue = 0u;
			return bReturnValue;
		}
	}
	else
	{
		signedSecondGrade = dsuStaticHeadStru->pConGradeStru->wGrade;
	}

	if ((0xffffu != fistGradeID) && (0xffffu != secondGradeID) &&
		(secondGradeID != fistGrade->wTmnlJointMainGradeId) && (secondGradeID != fistGrade->wTmnlJointSideGradeId))
	{
		/*入口参数异常，查询失败*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*
	用后一个坡度的坡度值减去前一个坡度的坡度值
	结果为正：坡度值增加
	结果为负：坡度值减少
	结果为零：坡度值不变化
	*/

	if (0 < signedSecondGrade - signedFirstGrade)
	{
		*GradeTrend = 2u;
		bReturnValue = 1u;
		return bReturnValue;
	}
	else if (0 > signedSecondGrade - signedFirstGrade)
	{
		*GradeTrend = 1u;
		bReturnValue = 1u;
		return bReturnValue;
	}
	else
	{
		*GradeTrend = 0u;
		bReturnValue = 1u;
		return bReturnValue;
	}

}

/*
函数功能：获取前面的坡度
输入参数：UINT16 linkId,当前link
输出参数：gradeFrnt 坡度
Null：  线路终点或前面坡度为统一坡度
非Null：前面坡度在坡度表中可查
返回值：0失败 1成功 2线路终点
*/
UINT8 dsuGetGradeFrntExp_CPK(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, DSU_GRADE_STRU** gradeFrnt)
{
	UINT16 GradeIDOrg = 0u;					/*记录前面坡度的ID*/
	UINT16 GradeID = 0u;					/*记录当前坡度的ID*/
	UINT8  bReturnValue = 0u;				/*函数返回值*/
	DSU_LINK_STRU *pLinkStru = NULL;		/*Link信息结构体指针*/
	DSU_LINK_STRU *pLinkStruOrg = NULL;		/*link的临时变量*/
	DSU_GRADE_STRU* gradeFrntTmp = NULL;	/*临时坡度变量*/
	UINT16 i = 0u;							/*循环计数变量*/
	UINT8 RstGetOrg = 0u;					/*查询前面坡度Id返回值*/
	UINT8 RstGetCur = 0u;					/*查询当前坡度Id返回值*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	UINT16* dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != pDsuEmapStru)
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*判断入口数据起点Link的有效性*/
	if ((linkId > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[linkId]))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*若当前link位置相邻的link在坡度表中可以查询到,则获取对应数据行;*/
	pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[linkId];

	for (i = 0u; i < dsuDataLenStru->wLinkLen; i++)
	{
		if (0xffffu != pLinkStru->wOrgnJointMainLkId)
		{
			pLinkStruOrg = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pLinkStru->wOrgnJointMainLkId];

			/*若相邻link的坡度不为统一坡度*/
			if (0u == pLinkStruOrg->wGrade)
			{
				/*查询起点关联link对应的坡度ID*/
				RstGetOrg = dsuGetGradeIdbyLocExp(pDsuEmapStru, pLinkStruOrg->wId, pLinkStruOrg->dwLength, &GradeIDOrg);

				/*查询当前link对应的坡度ID*/
				RstGetCur = dsuGetGradeIdbyLocExp(pDsuEmapStru, pLinkStru->wId, 0u, &GradeID);

				if ((0u == RstGetOrg) || (0u == RstGetCur))
				{
					/*查询失败*/
					bReturnValue = 0u;
					return bReturnValue;
				}

				/*相邻link与当前link的坡度ID不同*/
				if (GradeIDOrg != GradeID)
				{
					if (0xffffu != GradeIDOrg)
					{
						/*初始化前面相邻坡度*/
						gradeFrntTmp = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[GradeIDOrg];
						*gradeFrnt = gradeFrntTmp;
						bReturnValue = 1u;
						return bReturnValue;
					}
					else
					{
						/*数据出错(link表中不为统一坡度,查询得到为统一坡度)*/
						bReturnValue = 0u;
						return bReturnValue;
					}
				}
				else
				{
					pLinkStru = pLinkStruOrg;
				}
			}
			else
			{
				/*若相邻link的坡度仍为统一坡度*/
				*gradeFrnt = NULL;
				bReturnValue = 1u;
				return bReturnValue;
			}
		}
		else
		{
			/*若当前link无相邻link则为线路终点,起点设为无效值*/
			*gradeFrnt = NULL;
			bReturnValue = 2u;
			return bReturnValue;
		}
	}

	/*若循环完成未能找到对应坡度ID*/
	if (i == dsuDataLenStru->wLinkLen)
	{
		*gradeFrnt = NULL;
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*不可达分支*/
		bReturnValue = 0u;
		return bReturnValue;
	}
}
/*访问接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
UINT8 dsuGetGradeFrntExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, DSU_GRADE_STRU **gradeFrnt)
{
	UINT8 rtnValue = 0u;			/*函数返回值*/
	UINT8 cbtcVersionType = 0u;

	if (NULL != pDsuEmapStru)
	{
		cbtcVersionType = GetCbtcSysType();
		
		if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
		{
			rtnValue = dsuGetGradeFrntExp_CPK(pDsuEmapStru, linkId, gradeFrnt);
		}
		else
		{
			rtnValue = 0u;
		}
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/*
函数功能：获取后面的坡度
输入参数：UINT16 linkId,当前link
输出参数：gradeRear 坡度
Null：  线路终点或后面坡度为统一坡度
非Null：后面坡度在坡度表中可查
返回值：0失败 1成功 2线路终点
*/
UINT8 dsuGetGradeRearExp_CPK(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, DSU_GRADE_STRU** gradeRear)
{
	UINT16 GradeIDNxt = 0u;					/*记录后面坡度的ID*/
	UINT16 GradeID = 0u;					/*记录当前坡度的ID*/
	UINT8  bReturnValue = 0u;				/*函数返回值*/
	DSU_LINK_STRU* pLinkStru = NULL;		/*Link信息结构体指针*/
	DSU_LINK_STRU* pLinkStruNxt = NULL;		/*link的临时变量*/
	DSU_GRADE_STRU* gradeRearTmp = NULL;	/*坡度临时变量*/
	UINT16 i = 0u;							/*循环计数变量*/
	UINT8 RstGetNxt = 0u;					/*查询前面坡度Id返回值*/
	UINT8 RstGetCur = 0u;					/*查询当前坡度Id返回值*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	UINT16* dsuGradeIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	if (NULL != pDsuEmapStru)
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuGradeIndex = pDsuEmapStru->dsuEmapIndexStru->dsuGradeIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*判断入口数据起点Link的有效性*/
	if ((linkId > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[linkId]))
	{
		/*入口数据无效，数据异常，查询失败，返回错误*/
		bReturnValue = 0u;
		return bReturnValue;
	}

	/*若当前link位置相邻的link在坡度表中可以查询到,则获取对应数据行;*/
	pLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[linkId];

	for (i = 0u; i<dsuDataLenStru->wLinkLen; i++)
	{
		if (0xffffu != pLinkStru->wTmnlJointMainLkId)
		{
			pLinkStruNxt = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[pLinkStru->wTmnlJointMainLkId];

			/*若相邻link的坡度不为统一坡度*/
			if (0u == pLinkStruNxt->wGrade)
			{
				/*查询起点关联link对应的坡度ID*/
				RstGetNxt = dsuGetGradeIdbyLocExp(pDsuEmapStru, pLinkStruNxt->wId, 0, &GradeIDNxt);

				/*查询当前link对应的坡度ID*/
				RstGetCur = dsuGetGradeIdbyLocExp(pDsuEmapStru, pLinkStru->wId, pLinkStru->dwLength, &GradeID);

				if ((0u == RstGetNxt) || (0u == RstGetCur))
				{
					/*查询失败*/
					bReturnValue = 0u;
					return bReturnValue;
				}

				/*相邻link与当前link的坡度ID不同*/
				if (GradeIDNxt != GradeID)
				{
					if (0xffffu != GradeIDNxt)
					{
						/*初始化后面相邻坡度*/
						gradeRearTmp = dsuStaticHeadStru->pGradeStru + dsuGradeIndex[GradeIDNxt];
						*gradeRear = gradeRearTmp;
						bReturnValue = 1u;
						return bReturnValue;
					}
					else
					{
						/*则数据出错(link表中不为统一坡度,查询得到为统一坡度)*/
						bReturnValue = 0u;
						return bReturnValue;
					}
				}
				else
				{
					pLinkStru = pLinkStruNxt;
				}
			}
			else
			{
				/*若相邻link的坡度仍为统一坡度*/
				*gradeRear = NULL;
				bReturnValue = 1u;
				return bReturnValue;
			}
		}
		else
		{
			/*若当前link无相邻link则为线路终点*/
			*gradeRear = NULL;
			bReturnValue = 2u;
			return bReturnValue;
		}
	}

	/*若循环完成未能找到对应坡度ID*/
	if (i == dsuDataLenStru->wLinkLen)
	{
		*gradeRear = NULL;
		bReturnValue = 0u;
		return bReturnValue;
	}
	else
	{
		/*不可达分支*/
		bReturnValue = 0u;
		return bReturnValue;
	}
}

/*访问接口*/
/*备注：增加对单轨项目标识的处理，modify by qxt 20170726*/
UINT8 dsuGetGradeRearExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, DSU_GRADE_STRU **gradeRear)
{
	UINT8 rtnValue = 0u;
	UINT8 cbtcVersionType = 0u;
	cbtcVersionType = GetCbtcSysType();
	
	if ((DQU_CBTC_CPK == cbtcVersionType) || (DQU_CBTC_HLT == cbtcVersionType) || (DQU_CBTC_DG == cbtcVersionType) || (DQU_CBTC_CC == cbtcVersionType))
	{
		rtnValue = dsuGetGradeRearExp_CPK(pDsuEmapStru, linkId, gradeRear);
	}
	else
	{
		rtnValue = 0u;
	}
	return rtnValue;
}

/**
函数功能：获取当前LINK的静态限速个数
输入参数：
@linkId：当前link
输出参数：无
返回值：静态限速个数
*/
UINT8 GetStcLmtSpdCntByLnkIdExp(const DSU_EMAP_STRU *pDsuEmapStru, DSU_STC_LIMIT_LINKIDX_STRU *pDsuLimitStcLinkIdx, const UINT16 linkId)
{
	UINT8 ret = 0u;

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	if ((NULL != pDsuEmapStru) && (NULL != pDsuLimitStcLinkIdx))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
	}
	else
	{
		/*电子地图数据指针为空*/
		ret = 0u;
	}

	if (linkId <= LINKINDEXNUM)
	{
		if (pDsuLimitStcLinkIdx[linkId].linkId == linkId)
		{
			/*返回linkId对应静态限速表的限速个数*/
			ret = pDsuLimitStcLinkIdx[linkId].limitStcSpdInfoCount;
		}
		else
		{
			ret = 0u;
		}
	}
	else
	{
		ret = 0u;
	}

	return ret;
}

/**
函数功能：获取制定LINK的的指定下标的静态临时限速的对象指针
输入参数：
@linkId：当前Link编号
@idx：对应Link的第几个限速，下标取值范围0-9
返回值：
@NULL：获取失败
@非空：对应限速结构体指针
*/
DSU_STATIC_RES_SPEED_STRU *GetStcLmtSpdInfoByLnkIdExp(const DSU_EMAP_STRU *pDsuEmapStru, DSU_STC_LIMIT_LINKIDX_STRU *pDsuLimitStcLinkIdx, const UINT16 linkId, const UINT8 idx)
{
	DSU_STATIC_RES_SPEED_STRU *pStcLmtSpdInfo = NULL;

	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuStaticResSpeedIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;
	if ((NULL != pDsuEmapStru) && (NULL != pDsuLimitStcLinkIdx))
	{
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuStaticResSpeedIndex = pDsuEmapStru->dsuEmapIndexStru->dsuStaticResSpeedIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}
	else
	{
		/*电子地图数据指针为空*/
		pStcLmtSpdInfo = NULL;
	}
	/*飞检修改，对应2021年2月份飞检中规则MISRA2004-13_7，yt，2021年4月19日*/
	if (LINK_MAX_LIMIT_SPD <= idx)
	{
		return pStcLmtSpdInfo;
	}
	if (linkId <= LINKINDEXNUM)
	{
		if (idx < pDsuLimitStcLinkIdx[linkId].limitStcSpdInfoCount)
		{
			/*将linkId对应静态限速表的第idx静态限速取出来*/
			pStcLmtSpdInfo = dsuStaticHeadStru->pStaticResSpeedStru + dsuStaticResSpeedIndex[pDsuLimitStcLinkIdx[linkId].limitStcSpdInfoIdBuf[idx]];
		}
	}
	else
	{
		pStcLmtSpdInfo = NULL;
	}

	return pStcLmtSpdInfo;
}
#if 0
/*
函数功能：将多开道岔转为虚拟道岔
输入参数：multiPointId 多开道岔编号
multiPointState 多开道岔状态

输出参数：wVirtualPointIds 拆分后虚拟道岔编号
wVirtualPointStates 拆分后虚拟道岔状态
返回值：返回值：拆分后虚拟道岔个数, 0失败

函数算法：多开道岔包含的虚拟道岔： N位，R1位，L1位，R2位，L2位
所对应在数组中的索引： 0，  1，   2，   3，   4
所对应的多开道岔状态： 1，  2，   3，   4，   5
因此当 索引=多开道岔状态multiPointState-1 时，所对应的虚拟道岔保持原值，其余虚拟道岔状态取反（定位<=>反位）

注意事项：输出参数虚拟道岔wVirtualPointIds和状态wVirtualPointStates，对于无效道岔其道岔状态设置为3。如

/ L                                      / L
/                                        /
/                                        /       101
-|------------------------         ===》   ---|------/----------------- N
1         \           N                          102       \
\                                                \
\                                                \
R                                                 R
图1.实体三开道岔                            图2.虚拟单开道岔

当图1实体三开道岔1的状态为3时，推出图2虚拟单开道岔为L位。此时101为无效道岔，输出其状态为3，返回值道岔个数为2，如下
wVirtualPointIds[0]=102;
wVirtualPointStates[0]=2;
wVirtualPointIds[1]=101;
wVirtualPointStates[1]=3;
author:单轨项目 add by qxt 20170622
*/
UINT8 ConvertToVirtualSwitchesExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 multiPointId, const UINT8 multiPointState, UINT16 wVirtualPointIds[MAX_VIRTUAL_SWITCH_NUM], UINT8 wVirtualPointStates[MAX_VIRTUAL_SWITCH_NUM])
{
	UINT8 chReturnValue = 0u;						/*函数返回值*/
	UINT16 i = 0u, j = 0u, k = 0u;					/*循环用变量*/
	DSU_MULTI_SWITCH_STRU* pMultiPointStru = NULL;	/*多开道岔临时变量结结构体*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 MULTIPOINTINDEXNUM = 0u;
	UINT16 *dsuMultiPointIndex = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != wVirtualPointIds) && (NULL != wVirtualPointStates))
	{
		/*输出参数初始化为无效值65535*/
		for (i = 0u; i < MAX_VIRTUAL_SWITCH_NUM; i++)
		{
			wVirtualPointIds[i] = DSU_NULL_16;
			wVirtualPointStates[i] = DSU_NULL_8;
		}

		MULTIPOINTINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->MULTIPOINTINDEXNUM;
		dsuMultiPointIndex = pDsuEmapStru->dsuEmapIndexStru->dsuMultiPointIndex;
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断入口数据输入的有效性*/
		if ((multiPointId <= MULTIPOINTINDEXNUM) && (DSU_NULL_16 != dsuMultiPointIndex[multiPointId]) && (MAX_VIRTUAL_SWITCH_NUM >= multiPointState) && (1 <= multiPointState))
		{
			/*遍历多开道岔表*/
			for (j = 0u; j<dsuDataLenStru->wMultiPointLen; j++)
			{
				pMultiPointStru = dsuStaticHeadStru->pMultiPointStru + j;
				if ((pMultiPointStru->wId == multiPointId) && (multiPointState <= pMultiPointStru->wType))
				{
					/*当多开道岔状态是1时，需单独处理，因为N位道岔和R1位道岔属于同一道岔,为防止输出两个相同道岔，遍历虚拟道岔时，数组索引应从1开始*/
					if (1u == multiPointState)
					{
						for (k = 1u; k<pMultiPointStru->wType; k++)
						{
							/*虚拟道岔编号赋值*/
							wVirtualPointIds[k - 1u] = pMultiPointStru->wSwitchIds[k];
							/*虚拟道岔状态赋值，虚拟道岔状态只与N位相同，其他位道岔状态均取反(2=>1;1=>2*/
							wVirtualPointStates[k - 1u] = (UINT8)(0x03 & ~pMultiPointStru->wSwitchStates[k]);
						}
					}
					else
					{
						for (k = 1u; k<pMultiPointStru->wType; k++)
						{
							/*虚拟道岔编号赋值*/
							wVirtualPointIds[k - 1u] = pMultiPointStru->wSwitchIds[k];
							/*虚拟道岔状态赋值*/
							if (k == (multiPointState - 1u))
							{
								wVirtualPointStates[k - 1u] = (UINT8)(pMultiPointStru->wSwitchStates[k]);
							}
							/*索引在multiPointState-1之前的虚拟道岔均为无效的*/
							else if (k < (multiPointState - 1u))
							{
								wVirtualPointStates[k - 1u] = EMAP_POINT_STATUS_LOSE;
							}
							/*索引在multiPointState-1之后的虚拟道岔状态置反*/
							else
							{
								/*道岔状态取反(2=>1;1=>2)*/
								wVirtualPointStates[k - 1u] = (UINT8)(0x03u & ~pMultiPointStru->wSwitchStates[k]);

							}

						}

					}

					chReturnValue = pMultiPointStru->wType - 1u;
				}
			}
		}
		else
		{
			/*入口参数异常，查询失败*/
			chReturnValue = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
	}
	return chReturnValue;
}

/*
函数功能：将虚拟道岔转为多开道岔
输入参数：wVirtualPointIds[] 虚拟道岔编号
wVirtualPointStates[] 虚拟道岔状态
wVirtualPointNum    虚拟道岔数量
输出参数：multiPointId 对应多开道岔编号
multiPointState 对应多开道岔状态
返回值：0失败 1成功

注意事项：输入参数虚拟道岔wVirtualPointIds和状态wVirtualPointStates，对于无效的虚拟道岔，不需要传入。如

/L                                                 L
/                                                /
/                                                /
/                                                /
---|------/-----------\------ N      =====>        ------------------ N
1          2 \                                    \
\                                    \
R                                      R
图1.虚拟单开道岔                             图2.实体三开道岔

当图1虚拟道岔编号1的状态为2时，推出图2多开道岔的状态为L位。此时图一虚拟道岔2为无效的，其输入参数可设置为如下：
wVirtualPointIds[0]=1;
wVirtualPointStates[0]=2;
wVirtualPointNum=1;
author:单轨项目 add by qxt 20170622
*修改：by yt 2021年9月11日 修改白盒U16赋值给U8问题
*/
UINT8 ConvertToMultiSwitchExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wVirtualPointIds[MAX_VIRTUAL_SWITCH_NUM], const UINT8 wVirtualPointStates[MAX_VIRTUAL_SWITCH_NUM], const UINT8 wVirtualPointNum, UINT16 *multiPointId, UINT8 *multiPointState)
{
	UINT8 chReturnValue = 0u;						/*函数返回值*/
	UINT16 i = 0u, k = 0u;							/*循环用变量*/
	UINT8 j = 0u;
	UINT8 IdCount = 0u;								/* 用于计数，输入参数虚拟道岔编号与表中道岔编号相等的个数。*/
	UINT8 StateCount = 0u;							/*虚拟道岔状态相等的个数，有且只能有一个*/
	UINT8 tempPointState = 0u;						/*多开道岔状态*/
	DSU_MULTI_SWITCH_STRU *pMultiPointStru = NULL;	/*多开道岔临时变量结结构体*/
	
	/*全局变量改传参调用涉及的全局变量*/
	UINT16 *dsuMultiPointIndex = NULL;
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != wVirtualPointIds) && (NULL != wVirtualPointStates) && (NULL != multiPointId) && (NULL != multiPointState))
	{
		/*输出参数初始化为无效值*/
		*multiPointId = DSU_NULL_16;
		*multiPointState = DSU_NULL_8;

		dsuMultiPointIndex = pDsuEmapStru->dsuEmapIndexStru->dsuMultiPointIndex;
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断入口数据输入的有效性*/
		if (MAX_VIRTUAL_SWITCH_NUM >= wVirtualPointNum)
		{
			/*遍历多开道岔表*/
			for (i = 0u; i<dsuDataLenStru->wMultiPointLen; i++)
			{
				IdCount = 0u;
				StateCount = 0u;
				pMultiPointStru = dsuStaticHeadStru->pMultiPointStru + i;

				for (k = 0u; k < wVirtualPointNum; k++)
				{
					for (j = 0u; j < pMultiPointStru->wType; j++)
					{
						if (wVirtualPointIds[k] == pMultiPointStru->wSwitchIds[j])
						{
							IdCount++;
							if (wVirtualPointStates[k] == pMultiPointStru->wSwitchStates[j])
							{
								tempPointState = j; /*多开道岔的状态=该虚拟道岔索引值+1*/

								StateCount++;  /*虚拟道岔状态相等的个数只能有一个，*/
							}
						}
					}

				}
				/*找到所对应的多开道岔满足的条件：
				①当输入的道岔有N位（也是R1位）的道岔时，找到的相等的虚拟道岔数量IdCount等于输入参数虚拟道岔数量wVirtualPointNum+1。（因为N位和R位是相同道岔编号，遍历时会多计一次）；
				当输入的道岔没有N位（也是R1位）的道岔时，找到的相等的虚拟道岔数量IdCount等于输入参数虚拟道岔数量wVirtualPointNum。
				②输入参数道岔状态和表中虚拟道岔状态相等的个数只能有一个，即StateCount=1
				*/
				if (((wVirtualPointNum == IdCount) || (wVirtualPointNum + 1u == IdCount)) && (1u == StateCount))
				{
					*multiPointId = pMultiPointStru->wId;  /*多开道岔编号*/
					*multiPointState = tempPointState + 1u;    /*多开道岔状态(索引值+1)*/
					chReturnValue = 1u;
					break;
				}
			}
		}
		else
		{
			/*入口参数异常，查询失败*/
			chReturnValue = 0u;
		}
	}
	else
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
	}

	return chReturnValue;
}
#endif

/************************************************
*函数功能:根据link获取对应的OC的ID(车车新增接口)
*输入参数:linkId
*输出参数:ocId
*返回值:0失败,1成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuOcByLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, UINT16 *ocId)
{
	UINT8 rstValue = 1u;
	UINT16 ocIndex = 0u;

	/*非法防护*/
	if ((NULL == pDsuEmapStru) || (NULL == ocId))
	{
		rstValue = 0u;
	}
	else
	{
		*ocId = DSU_NULL_16;

		/*linkID值非法或非预期*/
		if ((linkId > (pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM)) || (0 == linkId))
		{
			rstValue = 0u;
		}
		else
		{
			/*查询OC索引信息*/
			ocIndex = (pDsuEmapStru->dsuStaticHeadStru->pLinkStru + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[linkId]))->wManageTIOC;

			/*OC索引值非法或非预期*/
			if ((ocIndex > pDsuEmapStru->dsuEmapIndexStru->CIINDEXNUM) || (0xFFFFu == pDsuEmapStru->dsuEmapIndexStru->dsuCIIndex[ocIndex]))
			{
				rstValue = 0u;
			}
			else
			{
				/*获取OC设备ID*/
				*ocId = (pDsuEmapStru->dsuStaticHeadStru->pCIStruStru + (pDsuEmapStru->dsuEmapIndexStru->dsuCIIndex[ocIndex]))->wID;
			}
		}
	}

	return rstValue;
}


/************************************************
*函数功能:获取信号机防护点位置(车车新增接口)
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处link以及偏移量
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuGetSignalPositionExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, LOD_STRU *position)
{
	/*函数返回值*/
	UINT8 rstValue = 0u;

	/*指向信号机信息的结构体指针*/
	DSU_SIGNAL_STRU *pStru = NULL;

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 tmpSignalIndexNum = 0u;
	UINT16* signalIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	/*非法防护*/
	if ((NULL != pDsuEmapStru) && (NULL != position))
	{
		/*获取信号机索引数组空间大小*/
		tmpSignalIndexNum = pDsuEmapStru->dsuEmapIndexStru->SIGNALINDEXNUM;
		signalIndex = pDsuEmapStru->dsuEmapIndexStru->dsuSignalIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断数据输入的有效性*/
		if ((wId > tmpSignalIndexNum) || (DSU_NULL_16 == signalIndex[wId]))
		{
			rstValue = 0u;
		}
		else
		{
			pStru = dsuStaticHeadStru->pSignalStru + signalIndex[wId];
			position->Lnk = pStru->wProtectLinkId;
			position->Off = pStru->dwProtectLinkOfst;
			/*返回信号机防护方向，wyd 20211110*/
			position->Dir = (UINT8)pStru->wProtectDir;
			rstValue = 1u;
		}
	}

	return rstValue;
}

/************************************************
*函数功能:获取信号机真实位置
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处真实link以及偏移量
*返回值:0:失败 1:成功
*备注：
************************************************/
UINT8 dsuGetRealSignalPositionExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, LOD_STRU *position)
{
	/*函数返回值*/
	UINT8 rstValue = 0u;

	/*指向信号机信息的结构体指针*/
	DSU_SIGNAL_STRU *pStru = NULL;

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 tmpSignalIndexNum = 0u;
	UINT16* signalIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	UINT16 LinkIndex = 0U;           	/*当前Link Index*/
	DSU_LINK_STRU *pLinkStru = NULL;   	/*当前Link,获取本地数据使用*/

	/*非法防护*/
	if ((NULL != pDsuEmapStru) && (NULL != position))
	{
		/*获取信号机索引数组空间大小*/
		tmpSignalIndexNum = pDsuEmapStru->dsuEmapIndexStru->SIGNALINDEXNUM;
		signalIndex = pDsuEmapStru->dsuEmapIndexStru->dsuSignalIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断数据输入的有效性*/
		if ((wId > tmpSignalIndexNum) || (DSU_NULL_16 == signalIndex[wId]))
		{
			rstValue = 0u;
		}
		else
		{
			pStru = dsuStaticHeadStru->pSignalStru + signalIndex[wId];

			/*先初始化为防护方向相同得信号机位置*/
			position->Lnk = pStru->wPositionLinkId;
			position->Off = pStru->dwPositionLinkOfst;
			position->Dir = (UINT8)pStru->wProtectDir;

			/*根据LinkID获取防护点的link结构体*/
			LinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pStru->wProtectLinkId];
			pLinkStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + LinkIndex;

			/*20230726-cjq有方向变化点且不在同一link时，防护方向与信号机得位置可能存在不一致得情况。*/
			if ((pStru->wProtectLinkId != pStru->wPositionLinkId)
			 && ((EMAP_CONVER_DIR == pLinkStru->wLogicDirChanged) || (EMAP_SAME_DIR == pLinkStru->wLogicDirChanged)))
			{
				/*此处认为信号机所在link与防护点所在link相同或者相邻，方向变化点不可能在岔心处，所以不判断侧向的连接link*/
				if (((pStru->wPositionLinkId == pLinkStru->wOrgnJointMainLkId) && (EMAP_DIRCHANGE_ORGN2ORGN == pLinkStru->wLogicDirChanged))
				|| ((pStru->wPositionLinkId == pLinkStru->wTmnlJointMainLkId) && (EMAP_DIRCHANGE_TMNL2TMNL == pLinkStru->wLogicDirChanged)))
				{
					position->Dir = (EMAP_SAME_DIR == position->Dir) ? EMAP_CONVER_DIR : EMAP_SAME_DIR;/*有变化点，改变查找方向*/
				}
			}
			else
			{
				/*nothing*/
			}

			rstValue = 1u;
		}
	}

	return rstValue;
}

#if 0
/************************************************
*函数功能:获取当前link是否有折返停车点(不含折返后停车点)(车车新增接口)
*输入参数:wId:Link索引编号
*输出参数:result:0x55有;0xAA:无
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuCheckArStopOnLinkExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, UINT8 *result)
{
	UINT8 rstValue = 0u;
	UINT16 len = 0u;/*停车点表数据量*/
	UINT16 i = 0u;
	UINT32 stopAttribute = 0u;
	const UINT32 bitAndValue = 20u;/*逻辑与因子*/
	const UINT32 expectValue = 4u;/*逻辑与的期望结果*/
	DSU_STOPPOINT_STRU *pStru = NULL;/*指向停车点信息的结构体指针*/

	/*全局变量改传参调用涉及的全局变量*/
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	/*非法防护*/
	if ((NULL != pDsuEmapStru) && (NULL != result))
	{
		*result = DSU_FALSE;/*默认无*/

		/*获取停车点结构体的数量*/
		len = pDsuEmapStru->dsuDataLenStru->wStopPointLen;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*逐个遍历数据*/
		for (i = 0u; i < len; i++)
		{
			/*获取实时数据*/
			pStru = dsuStaticHeadStru->pStopPointStru + i;

			/*满足link相等条件*/
			if (wId == pStru->wLinkId)
			{
				/*获取停车点属性*/
				stopAttribute = pStru->wAttribute;

				/*判定是否有符合预期的停车点信息*/
				if ((stopAttribute & bitAndValue) == expectValue)
				{
					rstValue = 1u;
					*result = DSU_TRUE;
					break;
				}
			}
		}
	}

	return rstValue;
}
#endif

/************************************************
*函数功能:查询link序列内的道岔信息(车车新增接口)
*输入参数:pLinkStru:Link序列信息
*输出参数:pBaliseStru:link序列内包含的道岔信息
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
*1、支持灯泡线 wyd 20230727
************************************************/
UINT8 dsuGetPointsBetweenLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pLinkStru, COM_DQU_POINT_STRU *pPointStru)
{
	UINT8 RtnNo = 0U;
	UINT8 rstValue = 1u;
	UINT8 CallFunRtn = 0u;
	UINT8 wFindDir = 0U;
	UINT16 i = 0u;
	DSU_LINK_STRU *pLinkCur = NULL;
	DSU_LINK_STRU *pLinkNext = NULL;

	/*非法防护*/
	if ((NULL == pDsuEmapStru) || (NULL == pLinkStru) || (NULL == pPointStru))
	{
		rstValue = 0u;
	}
	else
	{
		/*清空输出*/
		(void)CommonMemSet(pPointStru, sizeof(COM_DQU_POINT_STRU), 0x00U, sizeof(COM_DQU_POINT_STRU));

		/*校验link信息是否合法*/
		for (i = 0U; i < pLinkStru->ObjNum; i++)
		{
			if ((pLinkStru->ObjIds[i] > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkStru->ObjIds[i]]))
			{
				rstValue = 0u;
				break;
			}
			else
			{
				/*nothing*/
			}
		}

		if (1u == rstValue)
		{
			/*当只有一个Link的时候*/
			if (1u == pLinkStru->ObjNum)
			{
				pLinkCur = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkStru->ObjIds[0]]);

				/*起点是道岔 将起点相关信息值取出并使用*/
				if (EMAP_POINT_DOT == pLinkCur->wOrgnPointType)
				{
					pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wOrgnPointId;
					pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = DSU_NULL_8;
					pPointStru->PointNum++;
				}
				else
				{
					/*nothing*/
				}

				/*终点是道岔 将终点相关信息值取出并使用*/
				if (EMAP_POINT_DOT == pLinkCur->wTmnlPointType)
				{
					pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wTmnlPointId;
					pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = DSU_NULL_8;
					pPointStru->PointNum++;
				}
				else
				{
					/*nothing*/
				}
			}
			else if (1u < pLinkStru->ObjNum)
			{
				/*link多于1个*/
				/*Step1：判断初始查找方向，并添加第一个link【始端】的道岔*/
				pLinkCur = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkStru->ObjIds[0]]);
				if ((pLinkCur->wTmnlJointMainLkId == pLinkStru->ObjIds[1]) || (pLinkCur->wTmnlJointSideLkId == pLinkStru->ObjIds[1]))
				{
					/*初始查找方向为正向*/
					wFindDir = EMAP_SAME_DIR;
					/*添加始端道岔，最始端道岔不关心道岔期望状态*/
					if (EMAP_POINT_DOT == pLinkCur->wOrgnPointType)
					{
						pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wOrgnPointId;
						pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = DSU_NULL_8;
						pPointStru->PointNum++;
					}
					else
					{
						/*nothing*/
					}
				}
				else if ((pLinkCur->wOrgnJointMainLkId == pLinkStru->ObjIds[1]) || (pLinkCur->wOrgnJointSideLkId == pLinkStru->ObjIds[1]))
				{
					/*初始查找方向为反向*/
					wFindDir = EMAP_CONVER_DIR;
					/*添加始端道岔，最始端道岔不关心道岔期望状态*/
					if (EMAP_POINT_DOT == pLinkCur->wTmnlPointType)
					{
						pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wTmnlPointId;
						pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = DSU_NULL_8;
						pPointStru->PointNum++;
					}
					else
					{
						/*nothing*/
					}
				}
				else
				{
					/*无法判定初始查找方向，查询失败*/
					rstValue = 0U;
				}

				if (1U == rstValue)
				{
					/*Step2：依次添加第一个link到倒数第二个link【终端】的道岔*/
					for (i = 0U; i < pLinkStru->ObjNum - 1U; i++)
					{
						pLinkCur = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkStru->ObjIds[i]]);
						if (EMAP_SAME_DIR == wFindDir)
						{
							if (EMAP_POINT_DOT == pLinkCur->wTmnlPointType)
							{
								if (pLinkCur->wTmnlJointSideLkId == pLinkStru->ObjIds[i + 1U])
								{
									/*道岔期望反位*/
									pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wTmnlPointId;
									pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = EMAP_POINT_STATUS_SIDE;
									pPointStru->PointNum++;
								}
								else if (pLinkCur->wTmnlJointMainLkId == pLinkStru->ObjIds[i + 1U])
								{
									/*需要通过下一个link判断道岔期望位置*/
									pLinkNext = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkStru->ObjIds[i + 1U]]);
									if (((EMAP_DIRCHANGE_TMNL2TMNL == pLinkCur->wLogicDirChanged) && (pLinkNext->wTmnlJointMainLkId == pLinkCur->wId))
										|| (pLinkNext->wOrgnJointMainLkId == pLinkCur->wId))
									{
										/*道岔期望定位*/
										pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wTmnlPointId;
										pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = EMAP_POINT_STATUS_MAIN;
										pPointStru->PointNum++;
									}
									else if (((EMAP_DIRCHANGE_TMNL2TMNL == pLinkCur->wLogicDirChanged) && (pLinkNext->wTmnlJointSideLkId == pLinkCur->wId))
										|| (pLinkNext->wOrgnJointSideLkId == pLinkCur->wId))
									{
										/*道岔期望反位*/
										pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wTmnlPointId;
										pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = EMAP_POINT_STATUS_SIDE;
										pPointStru->PointNum++;
									}
									else
									{
										/*连接不到下一个link，查询失败*/
										rstValue = 0U;
									}
								}
								else
								{
									/*连接不到下一个link，查询失败*/
									rstValue = 0U;
								}
							}
							else if (EMAP_AXLE_DOT == pLinkCur->wTmnlPointType)
							{
								/*也校验一下相邻link*/
								if (pLinkCur->wTmnlJointMainLkId == pLinkStru->ObjIds[i + 1U])
								{
									/*无问题*/
								}
								else
								{
									/*连接不到下一个link，查询失败*/
									rstValue = 0U;
								}
							}
							else
							{
								/*中间link，终点既不是计轴也不是道岔，查询失败*/
								rstValue = 0U;
							}
						}
						else /*if (EMAP_CONVER_DIR == wFindDir)*/
						{
							if (EMAP_POINT_DOT == pLinkCur->wOrgnPointType)
							{
								if (pLinkCur->wOrgnJointSideLkId == pLinkStru->ObjIds[i + 1U])
								{
									/*道岔期望反位*/
									pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wOrgnPointId;
									pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = EMAP_POINT_STATUS_SIDE;
									pPointStru->PointNum++;
								}
								else if (pLinkCur->wOrgnJointMainLkId == pLinkStru->ObjIds[i + 1U])
								{
									/*需要通过下一个link判断道岔期望位置*/
									pLinkNext = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkStru->ObjIds[i + 1U]]);
									if (((EMAP_DIRCHANGE_ORGN2ORGN == pLinkCur->wLogicDirChanged) && (pLinkNext->wOrgnJointMainLkId == pLinkCur->wId))
										|| (pLinkNext->wTmnlJointMainLkId == pLinkCur->wId))
									{
										/*道岔期望定位*/
										pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wOrgnPointId;
										pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = EMAP_POINT_STATUS_MAIN;
										pPointStru->PointNum++;
									}
									else if (((EMAP_DIRCHANGE_ORGN2ORGN == pLinkCur->wLogicDirChanged) && (pLinkNext->wOrgnJointSideLkId == pLinkCur->wId))
										|| (pLinkNext->wTmnlJointSideLkId == pLinkCur->wId))
									{
										/*道岔期望反位*/
										pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wOrgnPointId;
										pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = EMAP_POINT_STATUS_SIDE;
										pPointStru->PointNum++;
									}
									else
									{
										/*连接不到下一个link，查询失败*/
										rstValue = 0U;
									}
								}
								else
								{
									/*连接不到下一个link，查询失败*/
									rstValue = 0U;
								}
							}
							else if (EMAP_AXLE_DOT == pLinkCur->wOrgnPointType)
							{
								/*也校验一下相邻link*/
								if (pLinkCur->wOrgnJointMainLkId == pLinkStru->ObjIds[i + 1U])
								{
									/*无问题*/
								}
								else
								{
									/*连接不到下一个link，查询失败*/
									rstValue = 0U;
								}
							}
							else
							{
								/*中间link，终点既不是计轴也不是道岔，查询失败*/
								rstValue = 0U;
							}
						}

						/*方向变化点处理*/
						if (((EMAP_SAME_DIR == wFindDir) && (EMAP_DIRCHANGE_TMNL2TMNL == pLinkCur->wLogicDirChanged))
							|| ((EMAP_CONVER_DIR == wFindDir) && (EMAP_DIRCHANGE_ORGN2ORGN == pLinkCur->wLogicDirChanged)))
						{
							wFindDir = (EMAP_SAME_DIR == wFindDir) ? EMAP_CONVER_DIR : EMAP_SAME_DIR;/*有变化点，改变查找方向*/
						}
						else
						{
							/*nothing*/
						}

						/*数组越界防护*/
						if (POINT_NUM_MAX <= pPointStru->PointNum)
						{
							/*道岔达到100个即进行防护，查询失败*/
							rstValue = 0U;
						}
						else
						{
							/*nothing*/
						}

						if (1u == rstValue)
						{
							/*继续查找*/
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					/*已经查询失败，do nothing*/
				}

				if (1U == rstValue)
				{
					/*Step3：添加最后一个link【终端】的道岔*/
					pLinkCur = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkStru->ObjIds[pLinkStru->ObjNum - 1U]]);
					if (EMAP_SAME_DIR == wFindDir)
					{
						if (EMAP_POINT_DOT == pLinkCur->wTmnlPointType)
						{
							/*最终端道岔不关心道岔期望状态*/
							pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wTmnlPointId;
							pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = DSU_NULL_8;
							pPointStru->PointNum++;
						}
						else
						{
							/*nothing*/
						}
					}
					else /*if (EMAP_CONVER_DIR == wFindDir)*/
					{
						if (EMAP_POINT_DOT == pLinkCur->wOrgnPointType)
						{
							/*最终端道岔不关心道岔期望状态*/
							pPointStru->PointInfo[(pPointStru->PointNum)].PointId = pLinkCur->wOrgnPointId;
							pPointStru->PointInfo[(pPointStru->PointNum)].PointExpectedStatus = DSU_NULL_8;
							pPointStru->PointNum++;
						}
						else
						{
							/*中间link，终点既不是计轴也不是道岔，查询失败*/
						}
					}
				}
				else
				{
					/*已经查询失败，do nothing*/
				}

			}
			else
			{
				/*link数量为零，直接返回查询成功*/
			}
		}
		else
		{
			/*已经查询失败，do nothing*/
		}

		if (1u == rstValue)
		{
			RtnNo = 1u;
		}
		else
		{
			/*清空输出*/
			(void)CommonMemSet(pPointStru, sizeof(COM_DQU_POINT_STRU), 0x00U, sizeof(COM_DQU_POINT_STRU));
		}
	}

	return RtnNo;
}

#if 0
/************************************************
*函数功能:传入一个位置和一段长度，查找此长度范围内的道岔信息和link序列(车车新增接口)
* 输入参数：	const LOD_STRU * pLODStru,	始端位置     const INT32     disPlacement,	查找长度
* 输出参数：	DQU_LINKS_STRU *pLinkStru		Link序列 DQU_BALISE_STRU *pBaliseStru    道岔序列
* 返回值：   	0,  查询失败  1,  查询成功
*备注：  经过的道岔均按定位处理 add by sds 2019-6-17
*修改：by yt 2021年9月11日 删除冗余变量
************************************************/
UINT8 dsuGetLinkAndBaliseBtwLocExp(DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement,
	COM_DQU_OBJECT_STRU *pLinkStru, COM_DQU_POINT_STRU *pBaliseStru, LOD_STRU *pEndLodStru)
{
	UINT8 chReturnValue = 0u;				/*用于函数返回值*/
	UINT16 CurrentLinkIndex = 0u;			/*当前Link Index*/
	DSU_LINK_STRU *pCurrentLinkStru = NULL;	/*用于循环查找过程中临时保存当前Link结构体*/
	UINT8 FindDir = 0u;						/*用于记录实际操作过程中的查找方向*/
	INT32 FindDisplacement = 0;				/*临时保存偏移量*/
	INT32 FindDisplacementSum = 0;			/*用于累加位移偏移量*/
	UINT16 k = 0u;							/*循环用变量*/

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;


	/*数据防护*/
	if ((NULL == pDsuEmapStru) || (NULL == pLODStru) || (NULL == pLinkStru) || (NULL == pBaliseStru) || (NULL == pEndLodStru))
	{
		/*电子地图数据指针为空*/
		chReturnValue = 0u;
	}
	else
	{
		/*初始化link序列和道岔序列的有效数量为0*/
		pLinkStru->ObjNum = 0u;
		pBaliseStru->PointNum = 0u;

		/*数据赋值*/
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断输入LinkID的有效性*/
		if ((pLODStru->Lnk>LINKINDEXNUM) || (DSU_NULL_16 == dsuLinkIndex[pLODStru->Lnk]))
		{
			/*输入LinkId无效，查询失败，返回*/
			chReturnValue = 0u;
		}
		/*判断入口数据偏移量的有效性*/
		else if (pLODStru->Off > LinkLengthExp(pDsuEmapStru, pLODStru->Lnk))
		{
			/*数据无效，查询失败，返回*/
			chReturnValue = 0u;
		}
		/*判断输入方向有效性*/
		else if ((EMAP_SAME_DIR != pLODStru->Dir) && (EMAP_CONVER_DIR != pLODStru->Dir))
		{
			/*输入方向无效，查询失败，返回*/
			chReturnValue = 0u;
		}
		/*特殊情况处理，偏移量Displacement为0*/
		else if (0 == Displacement)
		{
			/*displacement如果为0，返回当前位置为所求*/
			/*根据LinkID从数据库中查找当前link信息*/
			CurrentLinkIndex = dsuLinkIndex[pLODStru->Lnk];
			pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

			pLinkStru->ObjIds[pLinkStru->ObjNum] = pCurrentLinkStru->wId;
			(pLinkStru->ObjNum)++;

			/*查询成功，返回查询结果*/
			chReturnValue = 1u;
			CommonMemCpy(pEndLodStru, sizeof(LOD_STRU), pLODStru, sizeof(LOD_STRU));
		}
		else
		{
			/*如果displacement小于0，则按照displacement绝对值来查找新位置，方向取反*/
			if ((EMAP_SAME_DIR == pLODStru->Dir) && (0 < Displacement))
			{
				/*正向查找*/
				FindDir = EMAP_SAME_DIR;
				FindDisplacement = Displacement;
			}
			else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (0 > Displacement))
			{
				/*正向查找*/
				FindDir = EMAP_SAME_DIR;
				FindDisplacement = -Displacement;
			}
			else if ((EMAP_SAME_DIR == pLODStru->Dir) && (0 > Displacement))
			{
				/*反向查找*/
				FindDir = EMAP_CONVER_DIR;
				FindDisplacement = -Displacement;
			}
			else if ((EMAP_CONVER_DIR == pLODStru->Dir) && (0<Displacement))
			{
				/*反向查找*/
				FindDir = EMAP_CONVER_DIR;
				FindDisplacement = Displacement;
			}
			else
			{
				/*不可达分支*/
			}

			/*根据LinkID从数据库中查找当前link信息*/
			CurrentLinkIndex = dsuLinkIndex[pLODStru->Lnk];
			pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

			/*累加偏移量置0*/
			FindDisplacementSum = 0;

			if (EMAP_SAME_DIR == FindDir)
			{
				/*根据当前位置正向累加偏移量*/
				FindDisplacementSum = pCurrentLinkStru->dwLength - pLODStru->Off;
			}
			else
			{
				/*根据当前位置反向累加偏移量*/
				FindDisplacementSum = pLODStru->Off;
			}

			/*根据位移判断新位置是否在同一个Link上*/
			if (FindDisplacementSum >= FindDisplacement)
			{
				/*在同一个link上*/
				pLinkStru->ObjIds[pLinkStru->ObjNum] = pCurrentLinkStru->wId;
				(pLinkStru->ObjNum)++;
				/*计算路径终点*/
				pEndLodStru->Lnk = pCurrentLinkStru->wId;
				pEndLodStru->Dir = pLODStru->Dir;
				if (EMAP_SAME_DIR == pLODStru->Dir)
				{
					pEndLodStru->Off = pLODStru->Off + FindDisplacement;
				}
				else
				{
					pEndLodStru->Off = pLODStru->Off - FindDisplacement;
				}
				chReturnValue = 1u;
			}
			else
			{
				for (k = 0u; k < OBJECT_NUM_MAX; k++)
				{
					/*正向查找*/
					if (EMAP_SAME_DIR == FindDir)
					{
						/*连接link有效*/
						if (DSU_NULL_16 != pCurrentLinkStru->wTmnlJointMainLkId)
						{
							/*端点是道岔*/
							if (EMAP_POINT_DOT == pCurrentLinkStru->wTmnlPointType)
							{
								/*直接赋值端点编号和状态*/
								pBaliseStru->PointInfo[pBaliseStru->PointNum].PointId = pCurrentLinkStru->wTmnlPointId;
								pBaliseStru->PointInfo[pBaliseStru->PointNum].PointExpectedStatus = DSU_TRUE;
								(pBaliseStru->PointNum)++;
							}
							/*端点为非道岔*/
							else
							{
								/*不进行处理*/
							}

							/*pCurrentLinkStru指向相邻Link,相邻Link变为当前Link*/
							CurrentLinkIndex = dsuLinkIndex[pCurrentLinkStru->wTmnlJointMainLkId];
							pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

							/*累加偏移长度*/
							FindDisplacementSum += pCurrentLinkStru->dwLength;
							pLinkStru->ObjIds[pLinkStru->ObjNum] = pCurrentLinkStru->wId;
							(pLinkStru->ObjNum)++;

							/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置当前link上*/
							if (FindDisplacementSum >= FindDisplacement)
							{
								/*计算路径终点*/
								pEndLodStru->Lnk = pCurrentLinkStru->wId;
								pEndLodStru->Dir = pLODStru->Dir;
								if (EMAP_SAME_DIR == pLODStru->Dir)
								{
									pEndLodStru->Off = pCurrentLinkStru->dwLength - (FindDisplacementSum - FindDisplacement);
								}
								else
								{
									pEndLodStru->Off = FindDisplacementSum - FindDisplacement;
								}
								chReturnValue = 1u;
								break;
							}
							else
							{
								/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
							}
						}
						else
						{
							/*获取下一个link失败*/
							/*已查找至线路终点*/
							/*计算路径终点*/
							pEndLodStru->Lnk = pCurrentLinkStru->wId;
							pEndLodStru->Dir = pLODStru->Dir;
							if (EMAP_SAME_DIR == pLODStru->Dir)
							{
								pEndLodStru->Off = pCurrentLinkStru->dwLength;
							}
							else
							{
								pEndLodStru->Off = 0u;
							}
							chReturnValue = 1u;
							break;
						}
					}
					else/*反向查找*/
					{
						/*连接link有效*/
						if (DSU_NULL_16 != pCurrentLinkStru->wOrgnJointMainLkId)
						{
							/*端点是道岔*/
							if (EMAP_POINT_DOT == pCurrentLinkStru->wOrgnPointType)
							{
								/*直接赋值端点编号和状态*/
								pBaliseStru->PointInfo[pBaliseStru->PointNum].PointId = pCurrentLinkStru->wOrgnPointId;
								pBaliseStru->PointInfo[pBaliseStru->PointNum].PointExpectedStatus = DSU_TRUE;
								(pBaliseStru->PointNum)++;
							}
							/*端点为非道岔*/
							else
							{
								/*不进行处理*/
							}

							/*pCurrentLinkStru指向相邻Link,相邻Link变为当前Link*/
							CurrentLinkIndex = dsuLinkIndex[pCurrentLinkStru->wOrgnJointMainLkId];
							pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + CurrentLinkIndex;

							/*累加偏移长度*/
							FindDisplacementSum += pCurrentLinkStru->dwLength;
							pLinkStru->ObjIds[pLinkStru->ObjNum] = pCurrentLinkStru->wId;
							(pLinkStru->ObjNum)++;

							/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置当前link上*/
							if (FindDisplacementSum >= FindDisplacement)
							{
								/*计算路径终点*/
								pEndLodStru->Lnk = pCurrentLinkStru->wId;
								pEndLodStru->Dir = pLODStru->Dir;
								if (EMAP_SAME_DIR == pLODStru->Dir)
								{
									pEndLodStru->Off = pCurrentLinkStru->dwLength - (FindDisplacementSum - FindDisplacement);
								}
								else
								{
									pEndLodStru->Off = FindDisplacementSum - FindDisplacement;
								}
								chReturnValue = 1u;
								break;
							}
							else
							{
								/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
							}
						}
						else
						{
							/*获取下一个link失败*/
							/*已查找至线路终点*/
							/*计算路径终点*/
							pEndLodStru->Lnk = pCurrentLinkStru->wId;
							pEndLodStru->Dir = pLODStru->Dir;
							if (EMAP_SAME_DIR == pLODStru->Dir)
							{
								pEndLodStru->Off = pCurrentLinkStru->dwLength;
							}
							else
							{
								pEndLodStru->Off = 0u;
							}
							chReturnValue = 1u;
							break;
						}
					}
				}
			}
		}
	}

	return chReturnValue;
}

/************************************************
*函数功能:沿着应答器终点相邻方向查找两个应答器之间的距离(车车新增接口)
*输入参数：wB1Index 应答器1索引编号  wB2Index 应答器2索引编号  wMaxLen	 两个应答器之间的最大距离
*输出参数：*len 两个应答器之间的实际距离
*isFound 是否找到指定应答器
*返回值：0:失败或未找到 1:成功找到
*备注：  经过的道岔均按定位处理 add by sds 2019-6-17
************************************************/
static UINT8 GetLenBetweenBaliseExpTmnl(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wB1Index, const UINT16 wB2Index, const UINT32 wMaxLen, UINT32 *len, UINT8 *isFound)
{
	UINT8 rstValue = 0u;				/*函数返回值 0:未找到或者找到但有错误 1:正确找到*/
	DSU_BALISE_STRU *pPreBalise = NULL;	/*上一层应答器*/

    UINT16 tmpArray[101u] = { 0u };     /*临时变量，保存当前遍历到的应答器索引编号，假设最多wMaxLen范围内可能存在100层应答器*/
    UINT8 tmpArrayIndex[101u] = { 0u }; /*临时变量，保存当前遍历到的应答器所在层中的索引，假设最多wMaxLen范围内可能存在100层应答器*/
	UINT8 tmpLevel = 1u;				/*临时变量，保存当前处理的应答器所在层*/
	UINT32 tmpLen = 0u;					/*临时长度*/

	if ((NULL != pDsuEmapStru) && (NULL != len) && (NULL != isFound))
	{
		*len = DSU_NULL_32;
		*isFound = 0u;

		/*把应答器1作为起始层开始查找*/
		tmpArray[0] = wB1Index;

		while ((0u < tmpLevel) && (100u > tmpLevel))
		{
			/*上一层应答器*/
			pPreBalise = pDsuEmapStru->dsuStaticHeadStru->pBaliseStru + (pDsuEmapStru->dsuEmapIndexStru->dsuBaliseIndex[(tmpArray[tmpLevel - 1])]);

			/*判断上一层应答器的相邻应答器是否都遍历完成*/
			if (tmpArrayIndex[tmpLevel] < (pPreBalise->wTmnlBaliseNum))
			{
				/*设置当前层应答器*/
				tmpArray[tmpLevel] = (pPreBalise->dwTmnlBaliseId[tmpArrayIndex[tmpLevel]]) && DSU_NULL_16;
				tmpLen += pPreBalise->dwDistanceToTmnlBalise[tmpArrayIndex[tmpLevel]];

				/*找到应答器*/
				if (wB2Index == tmpArray[tmpLevel])
				{
					*isFound = 1u;
					if (tmpLen <= wMaxLen)
					{
						*len = tmpLen;
						rstValue = 1u;
					}

					break;
				}
				else
				{
					/*长度没超，继续找下一层*/
					if (tmpLen < wMaxLen)
					{
						tmpLevel += 1u;
						tmpArrayIndex[tmpLevel] = 0u;
					}
					else/*长度已超，长度回退，从本层下一个应答器继续*/
					{
						tmpLen -= pPreBalise->dwDistanceToTmnlBalise[tmpArrayIndex[tmpLevel]];
						tmpArrayIndex[tmpLevel] += 1u;
					}
				}
			}
			else/*本层查找完，回到上一层的下一个应答器开始*/
			{
				if (0u < pPreBalise->wTmnlBaliseNum)
				{
					/*距离回退*/
					tmpLen -= pPreBalise->dwDistanceToTmnlBalise[tmpArrayIndex[tmpLevel] - 1u];
				}

				tmpLevel -= 1u;
				tmpArrayIndex[tmpLevel] += 1u;
			}
		}
	}
	else
	{
		rstValue = 0;
	}

	return rstValue;
}


/************************************************
*函数功能:沿着应答器起点相邻方向查找两个应答器之间的距离(车车新增接口)
*输入参数：wB1Index 应答器1索引编号  wB2Index 应答器2索引编号  wMaxLen	 两个应答器之间的最大距离
*输出参数：*len 两个应答器之间的实际距离
*isFound 是否找到指定应答器
*返回值：0:失败或未找到 1:成功找到
*备注：  经过的道岔均按定位处理 add by sds 2019-6-17
************************************************/
static UINT8 GetLenBetweenBaliseExpOrgn(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wB1Index, const UINT16 wB2Index, const UINT32 wMaxLen, UINT32 *len, UINT8 *isFound)
{
	UINT8 rstValue = 0u;		/*函数返回值 0:未找到或者找到但有错误 1:正确找到*/
	DSU_BALISE_STRU *pPreBalise = NULL;/*上一层应答器*/

	UINT16 tmpArray[101] = { 0u };		/*临时变量，保存当前遍历到的应答器索引编号，假设最多wMaxLen范围内可能存在100层应答器*/
	UINT8 tmpArrayIndex[101] = { 0u };	/*临时变量，保存当前遍历到的应答器所在层中的索引，假设最多wMaxLen范围内可能存在100层应答器*/
	UINT8 tmpLevel = 1u;				/*临时变量，保存当前处理的应答器所在层*/
	UINT32 tmpLen = 0u;					/*临时长度*/

	if ((NULL != pDsuEmapStru) && (NULL != len) && (NULL != isFound))
	{
		*len = DSU_NULL_32;
		*isFound = 0u;

		/*把应答器1作为起始层开始查找*/
		tmpArray[0] = wB1Index;

		while ((0u < tmpLevel) && (100u > tmpLevel))
		{
			/*上一层应答器*/
			pPreBalise = pDsuEmapStru->dsuStaticHeadStru->pBaliseStru + (pDsuEmapStru->dsuEmapIndexStru->dsuBaliseIndex[(tmpArray[tmpLevel - 1])]);

			/*判断上一层应答器的相邻应答器是否都遍历完成*/
			if (tmpArrayIndex[tmpLevel] < (pPreBalise->wOrgnBaliseNum))
			{
				/*设置当前层应答器*/
				tmpArray[tmpLevel] = (pPreBalise->dwOrgnBaliseId[tmpArrayIndex[tmpLevel]]) && DSU_NULL_16;
				tmpLen += pPreBalise->dwDistanceToOrgnBalise[tmpArrayIndex[tmpLevel]];

				if (wB2Index == tmpArray[tmpLevel])/*找到应答器*/
				{
					*isFound = 1u;
					if (tmpLen <= wMaxLen)
					{
						*len = tmpLen;
						rstValue = 1u;
					}

					break;
				}
				else
				{
					if (tmpLen < wMaxLen)/*长度没超，继续找下一层*/
					{
						tmpLevel += 1u;
						tmpArrayIndex[tmpLevel] = 0u;
					}
					else/*长度已超，长度回退，从本层下一个应答器继续*/
					{
						tmpLen -= pPreBalise->dwDistanceToOrgnBalise[tmpArrayIndex[tmpLevel]];
						tmpArrayIndex[tmpLevel] += 1u;
					}
				}
			}
			else/*本层查找完，回到上一层的下一个应答器开始*/
			{
				if (0u < pPreBalise->wOrgnBaliseNum)
				{
					tmpLen -= pPreBalise->dwDistanceToOrgnBalise[tmpArrayIndex[tmpLevel] - 1];/*距离回退*/
				}

				tmpLevel -= 1u;
				tmpArrayIndex[tmpLevel] += 1u;
			}
		}
	}
	else
	{
		rstValue = 0u;
	}

	return rstValue;
}

/************************************************
*函数功能:查找两个应答器之间的距离(车车新增接口)
*输入参数：wBaliseId1 应答器1ID wBaliseId2 应答器2ID  wMaxLen	 两个应答器之间的最大距离
*输出参数：*len 两个应答器之间的实际距离
*返回值：0：失败  1：成功
*备注：  经过的道岔均按定位处理 add by sds 2019-6-17
************************************************/
UINT8 GetLenBetweenBaliseExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT32 wBaliseId1, const UINT32 wBaliseId2, const UINT32 wMaxLen, UINT32 *len)
{
	UINT16 wB1Index = 0u, wB2Index = 0u;/*索引编号*/
	UINT8 rstValue = 0u;
	UINT8 isFound = 0u;

	if ((NULL != pDsuEmapStru) && (NULL != len))
	{
		/*根据应答器ID获取索引编号*/
		wB1Index = wBaliseId1&DSU_NULL_16;
		wB2Index = wBaliseId2&DSU_NULL_16;

		/*检查应答器索引编号有效性*/
		if (NULL != pDsuEmapStru && NULL != (pDsuEmapStru->dsuEmapIndexStru))
		{
			if ((wB1Index > (pDsuEmapStru->dsuEmapIndexStru->BALISEINDEXNUM)) || (DSU_NULL_16 == (pDsuEmapStru->dsuEmapIndexStru->dsuBaliseIndex[wB1Index])))
			{
				rstValue = 0u;
			}
			else
			{
				if ((wB2Index > (pDsuEmapStru->dsuEmapIndexStru->BALISEINDEXNUM)) || ((DSU_NULL_16 == pDsuEmapStru->dsuEmapIndexStru->dsuBaliseIndex[wB2Index])))
				{
					rstValue = 0u;
				}
				else
				{
					rstValue = 1u;
				}
			}

		}
		/*先沿着终点方向查询*/
		if (1u == rstValue)
		{
			rstValue = GetLenBetweenBaliseExpTmnl(pDsuEmapStru, wB1Index, wB2Index, wMaxLen, len, &isFound);
		}

		/*再沿着起点方向查询*/
		if ((0u == rstValue) && (0u == isFound))
		{
			rstValue = GetLenBetweenBaliseExpOrgn(pDsuEmapStru, wB1Index, wB2Index, wMaxLen, len, &isFound);
		}
	}
	else
	{
		rstValue = 0u;
	}
	return rstValue;
}
#endif

/************************************************
*函数功能:沿指定方向查找两个位置间的道岔序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)  pEnd   查找终点位置(方向无效) pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pPointExp 查找到的序列 (0x55:定位 0xAA:反位)
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
************************************************/
UINT8 GetPointsInfoInAreaExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_POINT_STRU *pPointExp)
{
	UINT8 rstValue = 1u;
	UINT8 RtnNo = 0u;
	UINT16 i = 0u;
	UINT16 j = 0u;
	UINT8 CallFunRtn = 0U;
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU* dsuStaticHeadStru = NULL;
	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN] = { 0u };
	UINT16 LinkNum = 0U;
	COM_DQU_OBJECT_STRU LinkStru = { 0 };
	COM_DQU_POINT_STRU PointStru = { 0 };
	LOD_STRU EndLoc = { 0 };

	/*基本条件判断*/
	if ((NULL == pDsuEmapStru) || (NULL == pStart) || (NULL == pEnd) || (NULL == pPointAll) || (NULL == pPointExp))	/*判断指针为空*/
	{
		rstValue = 0u;
	}
	else
	{
		if (POINT_NUM_MAX < pPointAll->PointNum)
		{
			rstValue = 0u;
		}
		else
		{
			pPointExp->PointNum = 0u;/*初始化数量为0*/
		}

		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*判断入口数据位置Link的有效性*/
		if ((pStart->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pStart->Lnk]))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			rstValue = 0u;
		}
		/*判断入口数据位置偏移量的有效性*/
		else if (pStart->Off > LinkLengthExp(pDsuEmapStru, pStart->Lnk))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			rstValue = 0u;
		}
		/*判断入口数据位置方向的有效性*/
		else if ((EMAP_SAME_DIR != pStart->Dir) && (EMAP_CONVER_DIR != pStart->Dir))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			rstValue = 0u;
		}
		else
		{
			/*do nothing*/
		}

		/*判断入口数据位置Link的有效性*/
		if ((pEnd->Lnk > LINKINDEXNUM) || (0xFFFFu == dsuLinkIndex[pEnd->Lnk]))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			rstValue = 0u;
		}
		/*判断入口数据位置偏移量的有效性*/
		else if (pEnd->Off > LinkLengthExp(pDsuEmapStru, pEnd->Lnk))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			rstValue = 0u;
		}
		/*判断入口数据位置方向的有效性*/
		else if ((EMAP_SAME_DIR != pEnd->Dir) && (EMAP_CONVER_DIR != pEnd->Dir))
		{
			/*入口数据无效，数据异常，查询失败，返回错误*/
			rstValue = 0u;
		}
		else
		{
			/*do nothing*/
		}

		/*处理逻辑*/
		if (1u == rstValue)
		{
			/*先计算起点到终点的link序列*/
			CallFunRtn = dsuGetLinkBtwLocExp(pDsuEmapStru, pStart, pEnd, pPointAll->PointNum, pPointAll->PointInfo, wLinkID, &LinkNum);
			if (1u == CallFunRtn)
			{
				/*计算成功，得到的link序列是不含起终点的，需考虑是否添加起点和终点所在link*/
				if (((EMAP_SAME_DIR == pStart->Dir) && (0U == pStart->Off))
					|| ((EMAP_CONVER_DIR == pStart->Dir) && (LinkLengthExp(pDsuEmapStru, pStart->Lnk) == pStart->Off)))
				{
					/*正向查找时，起点link偏移量为零；或反向查找时，起点link偏移量为最大值，需要把起点link考虑进来*/
					LinkStru.ObjIds[0] = pStart->Lnk;
					(void)CommonMemCpy(&LinkStru.ObjIds[1], LinkNum * sizeof(UINT16), &wLinkID[0], LinkNum * sizeof(UINT16));
					LinkStru.ObjNum = LinkNum + 1U;
				}
				else
				{
					/*无需考虑起点link*/
					(void)CommonMemCpy(&LinkStru.ObjIds[0], LinkNum * sizeof(UINT16), &wLinkID[0], LinkNum * sizeof(UINT16));
					LinkStru.ObjNum = LinkNum;
				}

				/*判断是否添加终点所在link*/
				EndLoc.Lnk = pEnd->Lnk;
				EndLoc.Off = pEnd->Off;
				EndLoc.Dir = pStart->Dir;
				CallFunRtn = dquIsDirSameExp(pDsuEmapStru, pStart, &EndLoc, pPointAll->PointNum, pPointAll->PointInfo);
				if (DSU_TRUE == CallFunRtn)
				{
					/*起点到终点之间不跨方向变化点，或者跨偶数个变化点*/
					if ((((EMAP_CONVER_DIR == pStart->Dir) && (0U == pEnd->Off))
						|| ((EMAP_SAME_DIR == pStart->Dir) && (LinkLengthExp(pDsuEmapStru, pEnd->Lnk) == pEnd->Off)))
						&& (LinkStru.ObjIds[0] != pEnd->Lnk))
					{
						/*正向查找时，终点link偏移量为最大值；或反向查找时，终点link偏移量为零，需要把终点link考虑进来。还需要考虑该link未作为始端link被添加过*/
						LinkStru.ObjIds[LinkStru.ObjNum] = pEnd->Lnk;
						LinkStru.ObjNum += 1U;
					}
					else
					{
						/*无需考虑终点link*/
					}
				}
				else if (DSU_FALSE == CallFunRtn)
				{
					/*需要对跨（奇数个）变化点的场景单独处理*/
					if ((((EMAP_SAME_DIR == pStart->Dir) && (0U == pEnd->Off))
						|| ((EMAP_CONVER_DIR == pStart->Dir) && (LinkLengthExp(pDsuEmapStru, pEnd->Lnk) == pEnd->Off)))
						&& (LinkStru.ObjIds[0] != pEnd->Lnk))
					{
						/*正向查找时，跨方向变化点后终点link偏移量为零；或反向查找时，跨方向变化点后终点link偏移量为最大值，需要把终点link考虑进来。还需要考虑该link未作为始端link被添加过*/
						LinkStru.ObjIds[LinkStru.ObjNum] = pEnd->Lnk;
						LinkStru.ObjNum += 1U;
					}
					else
					{
						/*无需考虑终点link*/
					}
				}
				else
				{
					/*查询失败*/
					rstValue = 0u;
				}

				/*调用查询函数查link序列包含的道岔*/
				CallFunRtn = dsuGetPointsBetweenLinkExp(pDsuEmapStru, &LinkStru, &PointStru);
				if (1U == CallFunRtn)
				{
					/*查询成功，添加道岔状态*/
					for (i = 0U; i < PointStru.PointNum; i++)
					{
						for (j = 0U; j < pPointAll->PointNum; j++)
						{
							if (PointStru.PointInfo[i].PointId == pPointAll->PointInfo[j].PointId)
							{
								/*赋值道岔实际状态，清空查询函数多余输出的道岔期望状态*/
								PointStru.PointInfo[i].PointStatus = pPointAll->PointInfo[j].PointStatus;
								PointStru.PointInfo[i].PointExpectedStatus = 0U;
								break;
							}
							else
							{
								/*继续查找*/
							}
						}

						if (j >= pPointAll->PointNum)
						{
							/*没找到该道岔，查询失败*/
							rstValue = 0u;
							break;
						}
						else
						{
							/*继续查找*/
						}
					}
				}
				else
				{
					/*查询失败*/
					rstValue = 0u;
				}
			}
			else
			{
				/*查询失败*/
				rstValue = 0u;
			}
		}
		
		if (1U == rstValue)
		{
			/*查询成功，赋值*/
			(void)CommonMemCpy(pPointExp, sizeof(COM_DQU_POINT_STRU), &PointStru, sizeof(COM_DQU_POINT_STRU));
			RtnNo = 1U;
		}
		else
		{
			/*查询失败，清空输出*/
			(void)CommonMemSet(pPointExp, sizeof(COM_DQU_POINT_STRU), 0x00U, sizeof(COM_DQU_POINT_STRU));
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:沿指定方向查找两个位置间的PSD序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)  pEnd   查找终点位置(方向无效)  pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pObjectExp 查找到的序列
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
*		1、重新整理，支持灯泡线。wyd 020230725
************************************************/
UINT8 GetPsdsInfoInAreaExp(const DSU_EMAP_STRU *pDsuEmapStru, const DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru, 
	const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp)
{
	UINT8 RtnNo = 0U;
	UINT8 rstValue = 1u;
	UINT16 i = 0u;
	UINT8 j = 0u;
	DSU_LINK_STRU *pLink = NULL;
	DSU_DEVICE_IN_LINK_STRU *pstLinkExtend = NULL;/* 扩展结构体指针 */
	DSU_SCREEN_STRU *pPsdStru = NULL;
	UINT8 CallFunRtn = 0U;
	UINT16 LinkID[DSU_MAX_LINK_BETWEEN] = { 0u };
	UINT16 wLinkNum = 0U;
	COM_DQU_OBJECT_STRU LinkStru = { 0 };

	/*基本条件判断*/
	if ((NULL == pDsuEmapStru) || (NULL == pDsuEmapExtendStru) || (NULL == pStart) || (NULL == pEnd) || (NULL == pPointAll) || (NULL == pObjectExp))	/*判断指针为空*/
	{
		rstValue = 0u;
	}
	else
	{
		if ((pStart->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pStart->Lnk])
			|| (pEnd->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pEnd->Lnk])
			|| ((EMAP_SAME_DIR != pStart->Dir) && (EMAP_CONVER_DIR != pStart->Dir)))
		{
			rstValue = 0u;
		}
		else if (POINT_NUM_MAX < pPointAll->PointNum)
		{
			rstValue = 0u;
		}
		else
		{
			pObjectExp->ObjNum = 0u;/*初始化数量为0*/
		}

		/*处理逻辑*/
		if (1u == rstValue)
		{
			/*获取起点到终点的link序列*/
			if (pStart->Lnk == pEnd->Lnk)
			{
				/*当起终点link相同时，直接添加*/
				LinkStru.ObjIds[0] = pStart->Lnk;
				LinkStru.ObjNum = 1U;
			}
			else
			{
				/*调用查询函数查起终点之间的link序列*/
				CallFunRtn = dsuGetLinkBtwLocExp(pDsuEmapStru, pStart, pEnd, pPointAll->PointNum, pPointAll->PointInfo, LinkID, &wLinkNum);
				if (1u == CallFunRtn)
				{
					/*把起点和终点所在的link添加进来*/
					LinkStru.ObjIds[0] = pStart->Lnk;
					(void)CommonMemCpy(&LinkStru.ObjIds[1], wLinkNum * sizeof(UINT16), &LinkID[0], wLinkNum * sizeof(UINT16));
					LinkStru.ObjIds[wLinkNum + 1U] = pEnd->Lnk;
					LinkStru.ObjNum = wLinkNum + 2U;
				}
				else
				{
					/*查询失败*/
					rstValue = 0U;
				}
			}

			if (1U == rstValue)
			{
				/*找link序列对应的屏蔽门序列*/
				for (i = 0U; i < LinkStru.ObjNum; i++)
				{
					/*遍历link序列，从电子地图扩展结构体中获取link对应的屏蔽门ID*/
					pstLinkExtend = pDsuEmapExtendStru->pDsuDeviceInLinkIdx + LinkStru.ObjIds[i];
					for (j = 0U; j < pstLinkExtend->struScreenDev.wDevCount; j++)
					{
						pPsdStru = pDsuEmapStru->dsuStaticHeadStru->pScreenStru + pDsuEmapStru->dsuEmapIndexStru->dsuScreenIndex[pstLinkExtend->struScreenDev.wDevSetId[j]];
						if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
						{
							pObjectExp->ObjIds[pObjectExp->ObjNum] = pPsdStru->wId;
							pObjectExp->ObjNum += 1u;
						}
						else
						{
							/*数组越界防护 查询失败*/
							rstValue = 0U;
							break;
						}
					}

					if (1U == rstValue)
					{
						/*继续遍历*/
					}
					else
					{
						rstValue = 0U;
						break;
					}
				}
			}
			else
			{
				/*nothing*/
			}
		}
		else
		{
			/*nothing*/
		}

		if (1u == rstValue)
		{
			RtnNo = 1U;
		}
		else
		{
			(void)CommonMemSet(pObjectExp, sizeof(COM_DQU_OBJECT_STRU), 0x00U, sizeof(COM_DQU_OBJECT_STRU));
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:沿指定方向查找两个位置间的ESB序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向) pEnd   查找终点位置(方向无效)  pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pObjectExp 查找到的序列
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
*		1. 修复查不到最后一个link的问题。202109010.hekuan.
*		2、重新整理，支持灯泡线。wyd 020230725
************************************************/
UINT8 GetEsbsInfoInAreaExp(const DSU_EMAP_STRU *pDsuEmapStru, const DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru, 
	const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp)
{
	UINT8 RtnNo = 0U;
	UINT8 rstValue = 1u;
	UINT16 i = 0u;
	UINT8 j = 0u;
	DSU_LINK_STRU *pLink = NULL;
	DSU_DEVICE_IN_LINK_STRU *pstLinkExtend = NULL;/* 扩展结构体指针 */
	DSU_EMERG_STOP_STRU *pEsbStru = NULL;
	UINT8 CallFunRtn = 0U;
	UINT16 LinkID[DSU_MAX_LINK_BETWEEN] = { 0u };
	UINT16 wLinkNum = 0U;
	COM_DQU_OBJECT_STRU LinkStru = { 0 };

	/*基本条件判断*/
	if ((NULL == pDsuEmapStru) || (NULL == pDsuEmapExtendStru) || (NULL == pStart) || (NULL == pEnd) || (NULL == pPointAll) || (NULL == pObjectExp))	/*判断指针为空*/
	{
		rstValue = 0u;
	}
	else
	{
		if ((pStart->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pStart->Lnk])
			|| (pEnd->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pEnd->Lnk])
			|| ((EMAP_SAME_DIR != pStart->Dir) && (EMAP_CONVER_DIR != pStart->Dir)))
		{
			rstValue = 0u;
		}
		else if (POINT_NUM_MAX < pPointAll->PointNum)
		{
			rstValue = 0u;
		}
		else
		{
			pObjectExp->ObjNum = 0u;/*初始化数量为0*/
		}

		/*处理逻辑*/
		if (1u == rstValue)
		{
			/*获取起点到终点的link序列*/
			if (pStart->Lnk == pEnd->Lnk)
			{
				/*当起终点link相同时，直接添加*/
				LinkStru.ObjIds[0] = pStart->Lnk;
				LinkStru.ObjNum = 1U;
			}
			else
			{
				/*调用查询函数查起终点之间的link序列*/
				CallFunRtn = dsuGetLinkBtwLocExp(pDsuEmapStru, pStart, pEnd, pPointAll->PointNum, pPointAll->PointInfo, LinkID, &wLinkNum);
				if (1u == CallFunRtn)
				{
					/*把起点和终点所在的link添加进来*/
					LinkStru.ObjIds[0] = pStart->Lnk;
					(void)CommonMemCpy(&LinkStru.ObjIds[1], wLinkNum * sizeof(UINT16), &LinkID[0], wLinkNum * sizeof(UINT16));
					LinkStru.ObjIds[wLinkNum + 1U] = pEnd->Lnk;
					LinkStru.ObjNum = wLinkNum + 2U;
				}
				else
				{
					/*查询失败*/
					rstValue = 0U;
				}
			}

			if (1U == rstValue)
			{
				/*找link序列对应的屏蔽门序列*/
				for (i = 0U; i < LinkStru.ObjNum; i++)
				{
					/*遍历link序列，从电子地图扩展结构体中获取link对应的紧急停车按钮ID*/
					pstLinkExtend = pDsuEmapExtendStru->pDsuDeviceInLinkIdx + LinkStru.ObjIds[i];
					for (j = 0U; j < pstLinkExtend->struScreenDev.wDevCount; j++)
					{
						pEsbStru = pDsuEmapStru->dsuStaticHeadStru->pEmergStopStru + pDsuEmapStru->dsuEmapIndexStru->dsuScreenIndex[pstLinkExtend->struESBDev.wDevSetId[j]];
						if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
						{
							pObjectExp->ObjIds[pObjectExp->ObjNum] = pEsbStru->wId;
							pObjectExp->ObjNum += 1u;
						}
						else
						{
							/*数组越界防护 查询失败*/
							rstValue = 0U;
							break;
						}
					}

					if (1U == rstValue)
					{
						/*继续遍历*/
					}
					else
					{
						rstValue = 0U;
						break;
					}
				}
			}
			else
			{
				/*nothing*/
			}
		}
		else
		{
			/*nothing*/
		}

		if (1u == rstValue)
		{
			RtnNo = 1U;
		}
		else
		{
			(void)CommonMemSet(pObjectExp, sizeof(COM_DQU_OBJECT_STRU), 0x00U, sizeof(COM_DQU_OBJECT_STRU));
		}
	}

	return RtnNo;
}

#if 0
/************************************************
*函数功能:通过输入的link序列、车尾位置和配置距离，查询车尾位置至配置距离范围内的link数量和ID(车车新增接口)
*输入参数:pLinkIn:输入的link序列
pTailLoc:车尾位置
dis:配置距离
*输出参数:pLinkOut:查询到的link数量和ID
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
************************************************/
UINT8 dsuLinksWithCfgDisInInputLinksExp(const DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pLinkIn, const LOD_STRU *pTailLoc, const INT32 dis, COM_DQU_OBJECT_STRU *pLinkOut)
{
	UINT8 rstValue = 1u;
	DSU_LINK_STRU *pLinkTmp = NULL;
	UINT32 len = 0u;
	UINT8 i = 0u;

	/*异常防护*/
	if ((NULL == pDsuEmapStru) || (NULL == pLinkIn) || (NULL == pTailLoc))
	{
        rstValue = 0u;
	}
	else if ((pTailLoc->Dir != DSU_TRUE) && (pTailLoc->Dir != DSU_FALSE))
	{
        rstValue = 0u;
	}
	else
	{
		pLinkOut->ObjNum = 0u;
		pLinkTmp = pDsuEmapStru->dsuStaticHeadStru->pLinkStru;

		/*查找车尾所在link在link序列中的位置*/
		for (i = 0u; i < pLinkIn->ObjNum; i++)
		{
			if ((pTailLoc->Lnk) == (pLinkIn->ObjIds[i]))
			{
				break;
			}
		}

		if (i == (pLinkIn->ObjNum))
		{
			rstValue = 0u;/*车尾所在link不在link序列*/
		}
		else
		{
			i++;
			pLinkOut->ObjNum = 1u;
			pLinkOut->ObjIds[0] = (pTailLoc->Lnk);

			/*根据方向计算当前车尾位置的长度*/
			if (DSU_FALSE == pTailLoc->Dir)
			{
				len = pTailLoc->Off;
			}
			else
			{
				len = (pLinkTmp + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pTailLoc->Lnk]))->dwLength - pTailLoc->Off;
			}

			for (; i < pLinkIn->ObjNum; i++)
			{
				if (len >= (UINT32)dis)
				{
					break;
				}
				else
				{
					pLinkOut->ObjIds[(pLinkOut->ObjNum)] = (pLinkIn->ObjIds[i]);
					(pLinkOut->ObjNum) = (pLinkOut->ObjNum) + 1u;
					len += (pLinkTmp + (pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLinkIn->ObjIds[i]]))->dwLength;
				}
			}
		}
	}

	return rstValue;
}
#endif

/************************************************
*函数功能:通过输入的link序列、车尾位置和配置距离，查询车尾位置至配置距离范围内的link所属的OC数量和ID(车车新增接口)
*输入参数:pLinkIn:输入的link序列
pTailLoc:车尾位置
dis:配置距离
*输出参数:pLinkOut:查询到的OC数量和ID
*返回值:0:失败道岔失表 1:成功 0xFF 线路尽头
*备注： add by sds 2019-6-17
*修改：by yt 2021年9月11日 增加局部变量pLinkTmp、pOcTmp初始化
************************************************/
UINT8 dsuOcsWithCfgDisInInputLinksExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pTailLoc, const INT32 dis, IN const UINT8 PointNum, const DQU_POINT_STATUS_STRU * pPointStru, COM_BASE_COMM_OC_STRU *pOcOut)
{
	UINT8 rstValue = 1u;
	DSU_LINK_STRU *pLinkTmp = NULL;
	DSU_LINK_STRU *pCurrentLinkStru = NULL;
	DSU_CI_STRU *pOcTmp = NULL;
	UINT32 len = 0u;
	UINT16 tmpOc = 0u;
	UINT16 i = 0u;
	UINT16 wTempFindLink = 0u;
	UINT32 dCurrentFindOff = 0u;
	UINT8 FindDir = 0u;

	/*异常防护*/
	if ((NULL == pDsuEmapStru) || (NULL == pPointStru) || (NULL == pTailLoc) || (NULL == pOcOut))
	{
		rstValue = 0u;
	}
	else if ((EMAP_SAME_DIR != pTailLoc->Dir) && (DSU_FALSE != pTailLoc->Dir))
	{
		rstValue = 0u;
	}
	else
	{
		/*初始化数据*/
		pOcOut->CommOcNum = 0u;

		dCurrentFindOff = pTailLoc->Off;
		FindDir = pTailLoc->Dir;

		pLinkTmp = pDsuEmapStru->dsuStaticHeadStru->pLinkStru;
		pCurrentLinkStru = pLinkTmp + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pTailLoc->Lnk];
		pOcTmp = pDsuEmapStru->dsuStaticHeadStru->pCIStruStru;

		/*查找车尾所在link在link序列中的位置*/
		i++;
		rstValue = dsuOcByLinkExp(pDsuEmapStru, pCurrentLinkStru->wId, &tmpOc);

	}

	if (1u == rstValue)
	{
		pOcOut->CommOcNum = 1u;
		pOcOut->CommOcIDs[0] = tmpOc;

		/*根据方向计算当前车尾位置的长度*/

		if (DSU_FALSE == FindDir)
		{
			len = pTailLoc->Off;
		}
		else
		{
			len = pCurrentLinkStru->dwLength - pTailLoc->Off;
		}

		for (; i<pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM; i++)
		{
			if (len >= (UINT32)dis)
			{
				rstValue = 1u;
				break;
			}
			else
			{
				/*查下一个Link*/
				rstValue = dsuGetAdjacentLinkIDExp(pDsuEmapStru, pCurrentLinkStru->wId, FindDir, PointNum, pPointStru, &wTempFindLink);
				if (1u == rstValue)
				{
					if (((EMAP_SAME_DIR == FindDir) && (EMAP_DIRCHANGE_TMNL2TMNL == pCurrentLinkStru->wLogicDirChanged)) 
						|| ((EMAP_CONVER_DIR == FindDir) && (EMAP_DIRCHANGE_ORGN2ORGN == pCurrentLinkStru->wLogicDirChanged)))
					{
						FindDir = (EMAP_SAME_DIR == FindDir) ? EMAP_CONVER_DIR : EMAP_SAME_DIR;/*有变化点，改变查找方向*/
					}
					else
					{
						/*nothing*/
					}

					pCurrentLinkStru = pLinkTmp + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[wTempFindLink];
					rstValue = dsuOcByLinkExp(pDsuEmapStru, wTempFindLink, &tmpOc);
					if (1u == rstValue)
					{
						/*如果当前OC与上一个OC一样，则不重复放入*/
						if (tmpOc != pOcOut->CommOcIDs[(pOcOut->CommOcNum) - 1u])
						{
							pOcOut->CommOcIDs[(pOcOut->CommOcNum)] = tmpOc;
							(pOcOut->CommOcNum) = (pOcOut->CommOcNum) + 1u;
						}

						len += pCurrentLinkStru->dwLength;
					}
					else
					{
						break;
					}
				}
				else if (0xFFu == rstValue)
				{
					/*失败或线路终点 不找了*/
					rstValue = 0xFFu;
					break;
				}
				else
				{
					/*道岔失表*/
					rstValue = 0u;
					break;
				}
			}
		}
	}

	return rstValue;
}

#if 0
/************************************************
*函数功能:查询设备所属OC(车车新增接口)
*输入参数:devId:设备ID
*输出参数:devType:设备类型(1:道岔,2:信号机,3:紧急停车按钮,4:交叉渡线)(对于道岔，暂时取汇合link为准)
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
************************************************/
UINT8 dsuOcWithInputDevIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 devId, const UINT8 devType, UINT16 *ocId)
{
	UINT8 rstValue = 1u;
	UINT16 linkId = 0u;
	UINT16 PointId = 0u;

	/*异常防护*/
    if ((NULL == pDsuEmapStru) || (1u > devType) || (4u < devType) || (NULL == ocId))
	{
		rstValue = 0u;
	}
	else
	{
		*ocId = DSU_NULL_16;

		if (1u == devType)/*1:道岔,*/
		{
			if (devId > pDsuEmapStru->dsuEmapIndexStru->POINTINDEXNUM)
			{
				rstValue = 0u;
			}
			else
			{
				linkId = (pDsuEmapStru->dsuStaticHeadStru->pPointStru + (pDsuEmapStru->dsuEmapIndexStru->dsuPointIndex[devId]))->wConfluentLkId;
			}
		}
		else if (2u == devType)/*2:信号机,*/
		{
			if (devId > pDsuEmapStru->dsuEmapIndexStru->SIGNALINDEXNUM)
			{
				rstValue = 0u;
			}
			else
			{
				linkId = (pDsuEmapStru->dsuStaticHeadStru->pSignalStru + (pDsuEmapStru->dsuEmapIndexStru->dsuSignalIndex[devId]))->wProtectLinkId;
			}
		}
		else if (3u == devType)/*3:紧急停车按钮*/
		{
			if (devId > pDsuEmapStru->dsuEmapIndexStru->EMERGSTOPINDEXNUM)
			{
				rstValue = 0u;
			}
			else
			{
				linkId = (pDsuEmapStru->dsuStaticHeadStru->pEmergStopStru + (pDsuEmapStru->dsuEmapIndexStru->dsuEmergStopIndex[devId]))->wRelatLkId;
			}
		}
		else
		{
			/*nothing*/
		}

		if (1u == rstValue)
		{
			rstValue = dsuOcByLinkExp(pDsuEmapStru, linkId, ocId);
		}
	}
	return rstValue;
}

/************************************************
*函数功能:查询link上的信号机(车车新增接口)
*输入参数:linkId:link
dir:信号机作用方向
*输出参数:pObject 查找到的序列
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
************************************************/
static UINT8 GetSignalsOnLink(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, const UINT16 dir, COM_DQU_OBJECT_STRU *pObject)
{
	UINT8 rstValue = 1u;
	UINT16 i = 0u;
	DSU_SIGNAL_STRU *pSignal = NULL;

	/*异常防护*/
	if ((NULL == pDsuEmapStru) || (NULL == pObject))
	{
		rstValue = 0u;
	}
	else
	{
		/*获取信号机结构体*/
		pSignal = pDsuEmapStru->dsuStaticHeadStru->pSignalStru;

		/*重置数量*/
		pObject->ObjNum = 0u;

		for (i = 0u; i < pDsuEmapStru->dsuDataLenStru->wSignalLen; i++)
		{
			if ((linkId == pSignal->wProtectLinkId) && (dir == pSignal->wProtectDir))
			{
				pObject->ObjIds[pObject->ObjNum] = pSignal->wId;
				pObject->ObjNum++;
			}
		}
	}

	return rstValue;
}

/************************************************
*函数功能:查询一定范围内与电子地图逻辑方向相同的信号机(车车新增接口)
*输入参数：		pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
*				pEnd   查找终点位置(方向无效)
*				pPointAll 从起点到终点的道岔信息(有序排列)
*输出参数：		pObjectExp 查找到的序列
*返回值:		0:失败 1:成功
*备注：			add by sds 2019-6-17
*				1、lxc,2020/12/15,VBTC-Bug-498 修复局部变量为初始化问题
*				2. 修复最后一个link查询不到的问题 by kuan.he 20211028。
*				3. 优化 wyd 20230604
************************************************/
UINT8 dsuSignalBetweenPisitionsExpDirSame(const DSU_EMAP_STRU *pDsuEmapStru, const DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru,
	const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp)
{
	UINT8 rstValue = 0U;
	UINT8 CallFunRtn = 0U;
	DSU_SIGNAL_STRU *pSignal = NULL;
	UINT16 i = 0u;
	UINT16 linkCur = 0u;/*lxc,2020/12/15,VBTC-Bug-498 修复局部变量为初始化问题*/
	UINT16 linkNext = 0u;
	DSU_DEVICE_IN_LINK_STRU *pstLinkExtend = NULL;/* 扩展结构体指针 */
	UINT16 LoopIndex = 0U;	/*循环计数，防止陷入死循环*/
	UINT8 isEndLink = 0u;	/*是否到了处理最后一个link的步骤*/

	if ((NULL == pDsuEmapStru) || (NULL == pStart) || (NULL == pEnd) || (NULL == pPointAll) || (NULL == pObjectExp))
	{
		/*入参为空*/
	}
	else if ((pStart->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (DSU_NULL_16 == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pStart->Lnk])
		|| (pEnd->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (DSU_NULL_16 == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pEnd->Lnk])
		|| (pStart->Off > LinkLengthExp(pDsuEmapStru, pStart->Lnk)) || (pEnd->Off > LinkLengthExp(pDsuEmapStru, pEnd->Lnk)))
	{
		/*入参非法*/
	}
	else
	{
		rstValue = 1U;

		/*首尾端是同一link的情况*/
		if (pStart->Lnk == pEnd->Lnk)
		{
			pstLinkExtend = pDsuEmapExtendStru->pDsuDeviceInLinkIdx + pStart->Lnk;
			for (i = 0u; i < pstLinkExtend->struSigalDev.wDevCount; i++)
			{
				/*记录该link上的信号机*/
				pSignal = pDsuEmapStru->dsuStaticHeadStru->pSignalStru + pDsuEmapStru->dsuEmapIndexStru->dsuSignalIndex[pstLinkExtend->struSigalDev.wDevSetId[i]];
				
				/*起点和终点是同一link，起终点offset都要判断*/
				if ((pStart->Lnk == pSignal->wProtectLinkId) && (EMAP_SAME_DIR == pSignal->wProtectDir)
					&& (pSignal->dwProtectLinkOfst <= pEnd->Off) && (pSignal->dwProtectLinkOfst >= pStart->Off))
				{
					if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
					{
						pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
						pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
					}
					else
					{
						/*数组越界防护*/
						rstValue = 0U;
						break;
					}
				}
				else
				{
					/*继续遍历*/
				}
			}
		}
		else
		{
			linkCur = pStart->Lnk;

			while ((1U == rstValue) && ((linkCur != pEnd->Lnk) || (1u == isEndLink)))
			{
				/*找到无效link或把所有link都循环了一遍还没结束就退出*/
				if ((0xFFFFu == linkCur) || (LoopIndex >= pDsuEmapStru->dsuDataLenStru->wLinkLen))
				{
					rstValue = 0u;
					break;
				}

				/*step1:处理当前link，记录当前link上所有信号机*/
				pstLinkExtend = pDsuEmapExtendStru->pDsuDeviceInLinkIdx + linkCur;
				for (i = 0u; i < pstLinkExtend->struSigalDev.wDevCount; i++)
				{
					pSignal = pDsuEmapStru->dsuStaticHeadStru->pSignalStru + pDsuEmapStru->dsuEmapIndexStru->dsuSignalIndex[pstLinkExtend->struSigalDev.wDevSetId[i]];
					if (linkCur == pStart->Lnk)
					{
						/*当前link是起点link，需要判断起点offset*/
						if ((linkCur == pSignal->wProtectLinkId) && (EMAP_SAME_DIR == pSignal->wProtectDir) && (pSignal->dwProtectLinkOfst >= pStart->Off))
						{
							if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
							{
								pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
								pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
							}
							else
							{
								/*数组越界防护*/
								rstValue = 0U;
								break;
							}
						}
						else
						{
							/*继续遍历*/
						}
					}
					else if (linkCur == pEnd->Lnk)
					{
						/*当前位置是终点link，需要判断终点offset*/
						if ((linkCur == pSignal->wProtectLinkId) && (EMAP_SAME_DIR == pSignal->wProtectDir) && (pSignal->dwProtectLinkOfst <= pEnd->Off))
						{
							if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
							{
								pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
								pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
							}
							else
							{
								/*数组越界防护*/
								rstValue = 0U;
								break;
							}
						}
						else
						{
							/*继续遍历*/
						}
					}
					else
					{
						/*当前位置是中间link*/
						if ((linkCur == pSignal->wProtectLinkId) && (EMAP_SAME_DIR == pSignal->wProtectDir))
						{
							if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
							{
								pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
								pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
							}
							else
							{
								/*数组越界防护*/
								rstValue = 0U;
								break;
							}
						}
						else
						{
							/*继续遍历*/
						}
					}
				}

				/*step2:计算下一个link*/
				if (1u == isEndLink)
				{
					/*到达终点，上方已经查询过终点link的信号机。不再计算下一个link*/
					break;
				}
				else
				{
					CallFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, linkCur, EMAP_SAME_DIR, pPointAll->PointNum, pPointAll->PointInfo, &linkNext);
					if (1U == CallFunRtn)
					{
						linkCur = linkNext;
					}
					else
					{
						rstValue = 0U;
						break;
					}
				}

				/*判断是否到了终点link*/
				if (linkCur == pEnd->Lnk)
				{
					isEndLink = 1u;
				}
				else
				{
					/*没有到终点link*/
					isEndLink = 0u;
				}

				LoopIndex += 1U;
			}
		}

		if (1u != rstValue)
		{
			(void)CommonMemSet(pObjectExp, sizeof(COM_DQU_OBJECT_STRU), 0x00U, sizeof(COM_DQU_OBJECT_STRU));
		}
		else
		{
			/*nothing*/
		}
	}

	return rstValue;
}

/************************************************
*函数功能:		查询一定范围内与列车方向(0xAA)相同的信号机(车车新增接口)
*输入参数：		pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
*				pEnd   查找终点位置(方向无效)
*				pPointAll 从起点到终点的道岔信息(有序排列)
*输出参数：		pObjectExp 查找到的序列
*返回值:		0:失败 1:成功
*备注：			add by sds 2019-6-17
*				1、lxc,2020/12/15,VBTC-Bug-498 修复局部变量为初始化问题
*				2. 修复最后一个link查询不到的问题 by kuan.he 20211028。
*				3. 优化 wyd 20230604
************************************************/
UINT8 dsuSignalBetweenPisitionsExpDirConv(const DSU_EMAP_STRU *pDsuEmapStru, const DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru,
	const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp)
{
	UINT8 rstValue = 0U;
	UINT8 CallFunRtn = 0U;
	DSU_SIGNAL_STRU *pSignal = NULL;
	UINT16 i = 0u;
	UINT16 linkCur = 0u;/*lxc,2020/12/15,VBTC-Bug-498 修复局部变量为初始化问题*/
	UINT16 linkNext = 0u;
	DSU_DEVICE_IN_LINK_STRU *pstLinkExtend = NULL;/* 扩展结构体指针 */
	UINT16 LoopIndex = 0U;	/*循环计数，防止陷入死循环*/
	UINT8 isEndLink = 0u;	/*是否到了处理最后一个link的步骤*/

	if ((NULL == pDsuEmapStru) || (NULL == pStart) || (NULL == pEnd) || (NULL == pPointAll) || (NULL == pObjectExp))
	{
		/*入参为空*/
	}
	else if ((pStart->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (DSU_NULL_16 == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pStart->Lnk])
		|| (pEnd->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (DSU_NULL_16 == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pEnd->Lnk])
		|| (pStart->Off > LinkLengthExp(pDsuEmapStru, pStart->Lnk)) || (pEnd->Off > LinkLengthExp(pDsuEmapStru, pEnd->Lnk)))
	{
		/*入参非法*/
	}
	else
	{
		rstValue = 1U;

		/*首尾端是同一link的情况*/
		if (pStart->Lnk == pEnd->Lnk)
		{
			pstLinkExtend = pDsuEmapExtendStru->pDsuDeviceInLinkIdx + pStart->Lnk;
			for (i = 0u; i < pstLinkExtend->struSigalDev.wDevCount; i++)
			{
				/*记录该link上的信号机*/
				pSignal = pDsuEmapStru->dsuStaticHeadStru->pSignalStru + pDsuEmapStru->dsuEmapIndexStru->dsuSignalIndex[pstLinkExtend->struSigalDev.wDevSetId[i]];
				
				/*起点和终点是同一link，起终点offset都要判断*/
				if ((pStart->Lnk == pSignal->wProtectLinkId) && (EMAP_CONVER_DIR == pSignal->wProtectDir)
					&& (pSignal->dwProtectLinkOfst >= pEnd->Off) && (pSignal->dwProtectLinkOfst <= pStart->Off))
				{
					if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
					{
						pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
						pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
					}
					else
					{
						/*数组越界防护*/
						rstValue = 0U;
						break;
					}
				}
				else
				{
					/*继续遍历*/
				}
			}
		}
		else
		{
			linkCur = pStart->Lnk;

			while ((1U == rstValue) && ((linkCur != pEnd->Lnk) || (1u == isEndLink)))
			{
				/*找到无效link或把所有link都循环了一遍还没结束就退出*/
                if ((0xFFFFu == linkCur) || (LoopIndex >= pDsuEmapStru->dsuDataLenStru->wLinkLen))
				{
					rstValue = 0u;
					break;
				}

				/*step1:处理当前link，记录当前link上所有信号机*/
				pstLinkExtend = pDsuEmapExtendStru->pDsuDeviceInLinkIdx + linkCur;
				for (i = 0u; i < pstLinkExtend->struSigalDev.wDevCount; i++)
				{
					pSignal = pDsuEmapStru->dsuStaticHeadStru->pSignalStru + pDsuEmapStru->dsuEmapIndexStru->dsuSignalIndex[pstLinkExtend->struSigalDev.wDevSetId[i]];
					if (linkCur == pStart->Lnk)
					{
						/*当前link是起点link，需要判断起点offset*/
						if ((linkCur == pSignal->wProtectLinkId) && (EMAP_CONVER_DIR == pSignal->wProtectDir) && (pSignal->dwProtectLinkOfst <= pStart->Off))
						{
							if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
							{
								pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
								pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
							}
							else
							{
								/*数组越界防护*/
								rstValue = 0U;
								break;
							}
						}
						else
						{
							/*继续遍历*/
						}
					}
					else if (linkCur == pEnd->Lnk)
					{
						/*当前位置是终点link，需要判断终点offset*/
						if ((linkCur == pSignal->wProtectLinkId) && (EMAP_CONVER_DIR == pSignal->wProtectDir) && (pSignal->dwProtectLinkOfst >= pEnd->Off))
						{
							if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
							{
								pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
								pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
							}
							else
							{
								/*数组越界防护*/
								rstValue = 0U;
								break;
							}
						}
						else
						{
							/*继续遍历*/
						}
					}
					else
					{
						/*当前位置是中间link*/
						if ((linkCur == pSignal->wProtectLinkId) && (EMAP_CONVER_DIR == pSignal->wProtectDir))
						{
							if (OBJECT_NUM_MAX > pObjectExp->ObjNum)
							{
								pObjectExp->ObjNum = pObjectExp->ObjNum + 1u;
								pObjectExp->ObjIds[pObjectExp->ObjNum - 1u] = pSignal->wId;
							}
							else
							{
								/*数组越界防护*/
								rstValue = 0U;
								break;
							}
						}
						else
						{
							/*继续遍历*/
						}
					}
				}

				/*step2:计算下一个link*/
				if (1u == isEndLink)
				{
					/*到达终点，上方已经查询过终点link的信号机。不再计算下一个link*/
					break;
				}
				else
				{
					CallFunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, linkCur, EMAP_CONVER_DIR, pPointAll->PointNum, pPointAll->PointInfo, &linkNext);
					if (1U == CallFunRtn)
					{
						linkCur = linkNext;
					}
					else
					{
						rstValue = 0U;
						break;
					}
				}

				/*判断是否到了终点link*/
				if (linkCur == pEnd->Lnk)
				{
					isEndLink = 1u;
				}
				else
				{
					/*没有到终点link*/
					isEndLink = 0u;
				}

				LoopIndex += 1U;
			}
		}

		if (1u != rstValue)
		{
			(void)CommonMemSet(pObjectExp, sizeof(COM_DQU_OBJECT_STRU), 0x00U, sizeof(COM_DQU_OBJECT_STRU));
		}
		else
		{
			/*nothing*/
		}
	}

	return rstValue;
}
#endif

/************************************************
*函数功能:根据道岔ID获取道岔的位置(车车新增接口)
*输入参数：当前位置
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
* *修改记录：
*			1. 由遍历道岔方式修改获取道岔信息为直接获取道岔信息 kuan.he 20230724
************************************************/
UINT8 GetPointLocFromIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, DQU_POINT_LOC_STRU *pPointLoc)
{
	UINT16 j = 0u;
	UINT8 RtnNo = 0u;
	DSU_POINT_STRU* pPointStru = NULL;							/*道岔信息结构体指针*/

	/*异常防护*/
	if ((NULL != pDsuEmapStru) && (NULL != pPointLoc))
	{
		if (pDsuEmapStru->dsuEmapIndexStru->POINTINDEXNUM >= PointId)
		{
			/*获取道岔信息*/
			pPointStru = pDsuEmapStru->dsuStaticHeadStru->pPointStru + pDsuEmapStru->dsuEmapIndexStru->dsuPointIndex[PointId];
			if (NULL != pPointStru)
			{
				pPointLoc->PointId = PointId;
				pPointLoc->ConfluentLkId = pPointStru->wConfluentLkId;
				pPointLoc->MainLkId = pPointStru->wMainLkId;
				pPointLoc->SideLkId = pPointStru->wSideLkId;

				RtnNo = 1u;
			}
			else
			{
				/*查询失败*/
			}
		}
		else
		{
			/*id 不正确*/
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:根据道岔ID获取道岔岔心位置(按照道岔的汇合link)
*输入参数：当前位置
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注： 创建 zy
*修改记录：
*			1. 由遍历道岔方式修改获取道岔信息为直接获取道岔信息 kuan.he 20230724
************************************************/
UINT8 GetSwitchCenterExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, LOD_STRU *pPointLoc)
{
	UINT16 j = 0u;
	UINT8 RtnNo = 0u;

	DSU_POINT_STRU *pPointStru = NULL;							/*道岔信息结构体指针*/

	/*异常防护*/
	if ((NULL != pDsuEmapStru) && (NULL != pPointLoc))
	{
		if (pDsuEmapStru->dsuEmapIndexStru->POINTINDEXNUM >= PointId)
		{
			/*获取道岔信息*/
			pPointStru = pDsuEmapStru->dsuStaticHeadStru->pPointStru + pDsuEmapStru->dsuEmapIndexStru->dsuPointIndex[PointId];
			if (NULL != pPointStru)
			{
				pPointLoc->Lnk = pPointStru->wConfluentLkId;
				pPointLoc->Off = pPointStru->dwConfluentLkOfst;

				RtnNo = 1u;
			}
			else
			{
				/*查询失败*/
			}
		}
		else
		{
			/*id 不正确*/
		}
		
	}

	return RtnNo;
}

#if 0
/************************************************
*函数功能:根据道岔ID获取道岔的防护距离(车车新增接口)
*输入参数：当前位置
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
************************************************/
UINT8 GetProtectLengthFromIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, UINT16 *pProtectLength)
{
	UINT16 j = 0u;
	UINT8 RtnNo = 0u;

	/*异常防护*/
	if ((NULL != pDsuEmapStru) && (NULL != pProtectLength))
	{
		/*遍历道岔数据*/
		for (j = 0u; j<pDsuEmapStru->dsuDataLenStru->wPointLen; j++)
		{
			if (PointId == (pDsuEmapStru->dsuStaticHeadStru->pPointStru + j)->wId)
			{
				*pProtectLength = (pDsuEmapStru->dsuStaticHeadStru->pPointStru + j)->wRearSwitchProtecDis;
				RtnNo = 1u;
				break;
			}
		}
	}
	return RtnNo;
}

/************************************************
*函数功能:根据道岔ID获取道岔的属性(车车新增接口)
*输入参数：电子地图结构体指针；道岔ID
*输出参数：道岔的属性
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
************************************************/
UINT8 GetDPropertyFromIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, UINT8 *DProperty)
{
	UINT16 j = 0u;
	UINT8 RtnNo = 0u;

	/*异常防护*/
	if ((NULL != pDsuEmapStru) && (NULL != DProperty))
	{
		/*遍历道岔数据*/
		for (j = 0u; j<pDsuEmapStru->dsuDataLenStru->wPointLen; j++)
		{
			if (PointId == (pDsuEmapStru->dsuStaticHeadStru->pPointStru + j)->wId)
			{
				*DProperty = (pDsuEmapStru->dsuStaticHeadStru->pPointStru + j)->DProperty;
				RtnNo = 1u;
				break;
			}
		}
	}
	return RtnNo;
}

/************************************************
*函数功能:查询2个link的相对方向（若LINK B是LINK A的终点相邻LINK  则A到B为正向）(车车新增接口)
*输入参数:		linkAID:LinkA的索引ID
linkBID:LinkB的索引ID
*输出参数:dir:2个link的相对方向  0x55=正向 0xAA=反向  0xFF=无效
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
*修改：by yt 2021年9月11日 删除冗余变量
************************************************/
UINT8 dsuGetDirOfLinkToLinkExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkAID, const UINT16 linkBID, UINT8 *dir)
{

	UINT8 reValue = 0;/*函数返回值*/
	DSU_LINK_STRU *pCurrentLinkStru = NULL;/*用于循环查找过程中临时保存当前Link结构体*/

										   /*全局变量改传参调用涉及的全局变量*/
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;/*add by sds 2019-1-24*/

												   /*电子地图数据结构体防护*/
	if ((NULL != pDsuEmapStru) && (NULL != pDsuEmapStru->dsuEmapIndexStru) && (NULL != pDsuEmapStru->dsuStaticHeadStru) && (NULL != dir))
	{
		/*默认置为无效*/
		*dir = DSU_NULL_8;

		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;/*add by sds 2019-1-24*/

															/*根据LinkID从数据库中查找当前link信息*/
		pCurrentLinkStru = dsuStaticHeadStru->pLinkStru + dsuLinkIndex[linkAID];

		/*判断连接方向*/
		if (linkBID == pCurrentLinkStru->wOrgnJointMainLkId
			|| linkBID == pCurrentLinkStru->wOrgnJointSideLkId)
		{
			/*反向*/
			*dir = DSU_FALSE;
			reValue = 1u;

		}
		else if (linkBID == pCurrentLinkStru->wTmnlJointMainLkId
			|| linkBID == pCurrentLinkStru->wTmnlJointSideLkId)
		{
			/*正向*/
			*dir = DSU_TRUE;
			reValue = 1u;

		}
		else
		{
			/*两个link不相连，意味着查询失败*/
			reValue = 0u;
		}

	}
	else
	{
		/*电子地图数据指针为空*/
		reValue = 0u;
	}

	return reValue;
}

/************************************************
*函数功能:根据物理区段序列，查询包含的link序列(车车新增接口)
*输入参数:		PhysicalSgmt	物理区段信息
*输出参数:		pLinks link序列信息
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuRegionBlockTolinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *PhysicalSgmtStru, COM_DQU_OBJECT_STRU *LinksStru)
{

	UINT16 i = 0u, j = 0u, n = 0u;
	UINT8 RtnNo = 0u;
	UINT16 checkIndex = 0u;
	DSU_AXLE_SGMT_STRU *pAxleSgmtStru = NULL;	/*计轴区段临时变量结构体*/
	UINT16 iTempIndex = 0u;						/*存放LINK序列数组所对应的索引*/

	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;			/*全局变量，记录读取的DSU数据中的各类型数据的数量*/
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;		/*存放数据库中每种结构的头指针*/

	if ((NULL != pDsuEmapStru) && (NULL != PhysicalSgmtStru) && (NULL != LinksStru))
	{
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		iTempIndex = 0u;
		/*遍历包含的物理区段*/
		for (i = 0u; i < PhysicalSgmtStru->ObjNum; i++)
		{
			/*遍历计轴区段表，寻找与当前物理区段所对应的计轴区段，计算当前计轴区段所包含的LINK*/
			for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
			{
				pAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + j;
				if (pAxleSgmtStru->wRelatePhysicalSgmtId == PhysicalSgmtStru->ObjIds[i])
				{
					/*存储起点LINK*/
					for (checkIndex = 0u; checkIndex < iTempIndex; checkIndex++)
					{
						if (LinksStru->ObjIds[checkIndex] == pAxleSgmtStru->wOrgnLkId)
						{
							break;
						}
					}
					if (checkIndex == iTempIndex)
					{
						LinksStru->ObjIds[iTempIndex++] = pAxleSgmtStru->wOrgnLkId;
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
							for (checkIndex = 0u; checkIndex < iTempIndex; checkIndex++)
							{
								if (LinksStru->ObjIds[checkIndex] == pAxleSgmtStru->wOrgnLkId)
								{
									break;
								}
								else
								{
									/*do nothing*/
								}
							}

							if (checkIndex == iTempIndex)
							{
								LinksStru->ObjIds[iTempIndex++] = pAxleSgmtStru->wMiddleLinkId[n];
							}
							else
							{
								/*重复LINK，不添加*/
							}
						}
					}

					/*存储终点LINK*/
					if (pAxleSgmtStru->wOrgnLkId != pAxleSgmtStru->wTmnlLkId)
					{
						LinksStru->ObjIds[iTempIndex++] = pAxleSgmtStru->wTmnlLkId;
					}
					else
					{
						/*重复LINK，不添加*/
					}
				}
			}
		}

		if (0 == iTempIndex)/*数组越界或未找到对应的link*/
		{
			RtnNo = 0u;
		}
		else
		{
			/*去掉重复的Link*/
			LinksStru->ObjNum = iTempIndex;
			RtnNo = 1u;/*查询成功*/
		}
	}

	return RtnNo;
}
#endif

/************************************************
*函数功能:根据物理区段序列，查询包含的link序列(车车新增接口)
*输入参数:		PhysicalSgmt	物理区段信息
*输出参数:		pLinks link序列信息
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
*		1、改为从电子地图结构体直接读取。wyd 20230725
************************************************/
UINT8 dsuRegionBlockTolinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pPhysicalSgmtStru, COM_DQU_OBJECT_STRU *pLinksStru)
{
	UINT16 i = 0u;
	UINT8 RtnNo = 0u;
	UINT8 TempRtnVal = 1U;
	DSU_PHYSICAL_SGMT_STRU *pDsuPhySgmtStru = NULL;	/*物理区段临时变量结构体*/

	if ((NULL != pDsuEmapStru) && (NULL != pPhysicalSgmtStru) && (NULL != pLinksStru))
	{
		(void)CommonMemSet(pLinksStru, sizeof(COM_DQU_OBJECT_STRU), 0x00U, sizeof(COM_DQU_OBJECT_STRU));

		/*遍历包含的物理区段*/
		for (i = 0u; i < pPhysicalSgmtStru->ObjNum; i++)
		{
			if ((pPhysicalSgmtStru->ObjIds[i] > pDsuEmapStru->dsuEmapIndexStru->PHYSICALSGMTINDEXNUM) || (0xFFFFU == pDsuEmapStru->dsuEmapIndexStru->dsuPhysicalSgmtIndex[pPhysicalSgmtStru->ObjIds[i]]))
			{
				/*入参非法，查询失败*/
				TempRtnVal = 0U;
			}
			else
			{
				pDsuPhySgmtStru = pDsuEmapStru->dsuStaticHeadStru->pPhysicalSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuPhysicalSgmtIndex[pPhysicalSgmtStru->ObjIds[i]];
				if (OBJECT_NUM_MAX >= pLinksStru->ObjNum + pDsuPhySgmtStru->wIncLinkNum)
				{
					/*拷贝物理区段包含的link*/
					TempRtnVal &= CommonMemCpy(&pLinksStru->ObjIds[pLinksStru->ObjNum], (OBJECT_NUM_MAX - pLinksStru->ObjNum) * sizeof(UINT16),
						&pDsuPhySgmtStru->wIncLinkId[0], pDsuPhySgmtStru->wIncLinkNum * sizeof(UINT16));
					pLinksStru->ObjNum += pDsuPhySgmtStru->wIncLinkNum;
				}
				else
				{
					/*数组越界防护，查询失败*/
					TempRtnVal = 0U;
				}
			}

			if (1U == TempRtnVal)
			{
				/*继续遍历*/
			}
			else
			{
				/*查询失败，不再循环*/
				break;
			}
		}

		if (1U == TempRtnVal)
		{
			/*查询成功*/
			RtnNo = 1U;
		}
		else
		{
			/*查询失败，清空输出*/
			(void)CommonMemSet(pLinksStru, sizeof(COM_DQU_OBJECT_STRU), 0x00U, sizeof(COM_DQU_OBJECT_STRU));
		}
	}
	else
	{
		/*入参非法*/
	}

	return RtnNo;
}

/************************************************
*函数功能:根据link编号，查询所属ITS设备ID(车车新增接口)
*输入参数:		UINT16 LinkWid	link编号
*输出参数:		UINT16  ItsId 所属ITS设备ID
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
*修改：by yt 2021年9月11日 增加输入参数有效值防护
************************************************/
UINT8 dsuItsIdOfLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkWid, UINT16 *ItsId)
{
	UINT8 RtnNo = 0u;
	DSU_LINK_STRU	*pLinkStruStru = NULL;		/*LINK信息结构体指针*/

	if ((NULL != pDsuEmapStru) && (0u != LinkWid) && (DSU_NULL_16 != LinkWid) && (NULL != ItsId))
	{
		/*输出所属ITS设备ID信息*/
		pLinkStruStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[LinkWid];
		*ItsId = pLinkStruStru->wManageITS;
		RtnNo = 1u;/*查询成功*/
	}
	else
	{
		RtnNo = 0u;/*查询失败*/
	}
	return RtnNo;
}
#if 0
/************************************************
*函数功能:获取x限速区段信息(车车新增接口)
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处link以及偏移量
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
*修改：by yt 2021年9月11日 删除冗余变量
************************************************/
UINT8 dsuGetRespeedSgmtExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, DSU_RESPEED_SGMT_STRU *RespeedSgmtStru)
{
	/*函数返回值*/
	UINT8 rstValue = 0u;
	UINT8 i = 0u;

	/*非法防护*/
	if ((NULL != pDsuEmapStru) && (NULL != RespeedSgmtStru))
	{
		/*获取信号机索引数组空间大小*/
		for (i = 0u; i < pDsuEmapStru->dsuDataLenStru->wRespeedSgmtLen; i++)
		{
			if (wId == (pDsuEmapStru->dsuStaticHeadStru->pRespeedSgmtStru + i)->wId)
			{
				CommonMemCpy(RespeedSgmtStru, sizeof(DSU_RESPEED_SGMT_STRU), pDsuEmapStru->dsuStaticHeadStru->pRespeedSgmtStru + i, sizeof(DSU_RESPEED_SGMT_STRU));
				rstValue = 1u;
			}
		}

	}

	return rstValue;
}
#endif
/************************************************
*函数功能:获取逻辑区段信息(车车新增接口)
*输入参数:wId:逻辑区段ID
*输出参数:逻辑区段信息
*返回值:0:失败 1:成功
*备注：add by db 20220623
************************************************/
UINT8 dsuGetLogicSgmtInfoByIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, DSU_LOGIC_SGMT_STRU * pLogicSgmtStru)
{
	/*函数返回值*/
	UINT8 rstValue = 0u;

	/*非法防护*/
	if ((NULL == pDsuEmapStru) || (NULL == pLogicSgmtStru))
	{
		/*入参非法*/
	}
	else
	{
		if ((wId > pDsuEmapStru->dsuEmapIndexStru->LOGICSGMTINDEXNUM) || (0xFFFFU == pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[wId]))
		{
			/*入参非法*/
		}
		else
		{
			rstValue = CommonMemCpy(pLogicSgmtStru, sizeof(DSU_LOGIC_SGMT_STRU),
				pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[wId], sizeof(DSU_LOGIC_SGMT_STRU));
		}
	}

	return rstValue;
}

#if 0
/************************************************
*函数功能:根据计轴区段序列，查询包含的link序列(车车新增接口)
*输入参数:		PhysicalSgmt	物理区段信息
*输出参数:		pLinks link序列信息
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuAxleSgmtTolinkExp(DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pAxleSgmtStru, COM_DQU_OBJECT_STRU *LinksStru)
{

	UINT16 i = 0u, j = 0u, n = 0u;
	UINT8 RtnNo = 0u;
	DSU_AXLE_SGMT_STRU *TempAxleSgmtStru = NULL;	/*计轴区段临时变量结构体*/
    UINT16 arTempLink[200u] = { 0u };               /*临时存放LINK序列*/
	UINT16 iTempIndex = 0u;							/*存放LINK序列数组所对应的索引*/

	/*全局变量改传参调用涉及的全局变量*/
	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;		/*全局变量，记录读取的DSU数据中的各类型数据的数量*/
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;	/*存放数据库中每种结构的头指针*/

	if ((NULL != pDsuEmapStru) && (NULL != pAxleSgmtStru) && (NULL != LinksStru))
	{
		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		iTempIndex = 0u;
		/*遍历包含的物理区段*/
		for (i = 0u; i < pAxleSgmtStru->ObjNum; i++)
		{
			/*遍历计轴区段表，寻找与当前物理区段所对应的计轴区段，计算当前计轴区段所包含的LINK*/
			for (j = 0u; j < dsuDataLenStru->wAxleSgmtLen; j++)
			{
				TempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + j;
				if (TempAxleSgmtStru->wId == pAxleSgmtStru->ObjIds[i])
				{
					/*存储起点LINK*/
					arTempLink[iTempIndex++] = TempAxleSgmtStru->wOrgnLkId;

					/*存储中间LINK*/
					for (n = 0u; n < MIDDLE_LINK_MAX_NUM; n++)
					{
						if (DSU_NULL_16 != TempAxleSgmtStru->wMiddleLinkId[n])
						{
							arTempLink[iTempIndex++] = TempAxleSgmtStru->wMiddleLinkId[n];

						}
					}

					/*存储终点LINK*/
					arTempLink[iTempIndex++] = TempAxleSgmtStru->wTmnlLkId;
				}
			}
		}
		if (0u == iTempIndex)/*数组越界或未找到对应的link*/
		{
			RtnNo = 0u;
		}
		else
		{
			/*去掉重复的Link*/
			LinksStru->ObjNum = (UINT16)RemoveRep(arTempLink, (UINT8)iTempIndex, LinksStru->ObjIds);
			RtnNo = 1u;/*查询成功*/
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:根据link编号，查找所属的计轴区段ID
*输入参数:		linkId		link的ID
*输出参数:		AxleSgmtId	计轴区段Id
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsulinkofAxleSgmtExp(DSU_EMAP_STRU *pDsuEmapStru, UINT16 linkId, UINT16 *AxleSgmtId)
{
	UINT16 i = 0u, n = 0u;
	UINT8 RtnNo = 0u;
	DSU_AXLE_SGMT_STRU *TempAxleSgmtStru = NULL;    /*计轴区段临时变量结构体*/

	DSU_DATA_LEN_STRU *dsuDataLenStru = NULL;                /*全局变量，记录读取的DSU数据中的各类型数据的数量*/
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;          /*存放数据库中每种结构的头指针*/

	if ((NULL != AxleSgmtId) && (NULL != pDsuEmapStru))
	{
		*AxleSgmtId = 0;

		dsuDataLenStru = pDsuEmapStru->dsuDataLenStru;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;

		/*遍历计轴区段表，寻找包含link的计轴区段*/
		for (i = 0u; i < dsuDataLenStru->wAxleSgmtLen; i++)
		{
			TempAxleSgmtStru = dsuStaticHeadStru->pAxleSgmtStru + i;

			/*存储中间LINK*/
			for (n = 0u; n < MIDDLE_LINK_MAX_NUM; n++)
			{
				if (linkId == TempAxleSgmtStru->wMiddleLinkId[n])
				{
					*AxleSgmtId = TempAxleSgmtStru->wId;
					RtnNo = 1u;
					break;
				}
			}
			if (0u != *AxleSgmtId)
			{
				break;
			}
		}
	}
	return RtnNo;
}
#endif

/**********************************************************************************************
*函数功能:根据既有的路径终点link查找下一个link，若遇到道岔分支则添加为新路径(车车新增接口)
*输入参数:		pDsuEmapStru		数据
*				cDir				查询方向
*				pPathNum			路径数量
*				szPath				路径数据
*				szLinkNum			每条路径的长度
*输出参数:		pPathNum			路径数量
*				szPath				路径数据
*				szLinkNum			每条路径的长度
*返回值:0:失败 1:查询成功，且各路径未到终点 2：查询成功，且各路径到达终点
*备注： add by lxc 2021-2-28
*修改：by yt 2021年9月11日 删除冗余变量
**********************************************************************************************/
UINT8 dsuPathEndNextLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, UINT8 szLinkQueryDir[50], UINT8* pPathNum, UINT16 szPath[50][50], UINT16 szLinkNum[50])
{
	UINT8 cRetVal = 0u;

	/* 用于承接输入数据 */
	UINT8 cPathNum = 0u;

	/* 临时变量 */
	UINT8 i = 0u;
	UINT16 j = 0u;
	UINT8 cTmpPathNum = 0u;
	UINT16 wTmpEndLinkId = 0u;
	UINT16 wTmpEndLinkIndex = 0u;
	DSU_LINK_STRU *pTmpEndLinkStru = NULL;   /*当前Link,获取本地数据使用*/
	UINT8 cFunFlag = 0u;				/* 为0时延申当前路径，为1时延申当前路径并开辟新路径 */
	UINT16 wTmpNextMainLinkId = 0u;	/* 下一段正线link */
	UINT16 wTmpNextSideLinkId = 0u;	/* 下一段侧线link */
	UINT16 wAddLinkId = 0u;
	UINT16 wNewLinkId = 0u;
	UINT16 wTmpPathLinkNum = 0u;
	UINT16 szTmpAddPath[25][50] = { 0u };
	UINT16 szTmpAddLinkNum[25] = { 0u };
	UINT16 wTmpEndPathNum = 0u;

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	UINT8 cQueryDir = 0U;
	UINT8 cCurLinkDirChanged = 0xFFU;  /*当前Link方向变化点*/
	UINT8 cNxtLinkDirChanged = 0xFFU;  /*下一Link方向变化点*/
	UINT8 cTempPathIndex = 0U;

	/* 1.入参检查 */
	if ((NULL != pDsuEmapStru) && (NULL != szLinkQueryDir)
		&& (NULL != pPathNum) && (50u > *pPathNum) && (0u != *pPathNum)
		&& (NULL != szPath) && (NULL != szLinkNum))
	{
		for (i = 0U; i < 50U; i++)
		{
			/*方向检查*/
			if ((EMAP_SAME_DIR == szLinkQueryDir[i]) || (EMAP_CONVER_DIR == szLinkQueryDir[i]))
			{
				cRetVal = 1U;
			}
			else
			{
				cRetVal = 0U;
				break;
			}
		}

		/* 入参承接 */
		cPathNum = *pPathNum;

		/* 全局变量透传 */
		LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
		dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
		dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
	}

	/* 2.查询所有路径最后一个link：
	*	1）值为0xff，证明路径已到线路终点，或存储数据空间不够，跳过本条路径继续查询下一条路径；
	*	2）沿查询方向上获取端点类型：
	*				a）若为终点记录终点为0xFF；
	*				b）若为计轴点则获取下一个link并延申该路径；
	*				c）若为道岔点且本link为汇合link，则路径分叉，根据定位link与反位link添加新路径；
	*				d）若为道岔点且本link为非汇合link，获取起点连接正线link并延申该路径；
	*/
	if (1u == cRetVal)
	{
		cTmpPathNum = 0u;
		for (i = 0u; i < cPathNum; i++)
		{
			wTmpEndLinkId = szPath[i][(szLinkNum[i] - 1U)];
			cQueryDir = szLinkQueryDir[i];

			/* 1） */
			if (DSU_NULL_16 != wTmpEndLinkId)
			{

				cCurLinkDirChanged = dquGetLinkLogicDirChangedByIdExp(pDsuEmapStru,wTmpEndLinkId);  /*获取当前link的方向变化点*/
				/*根据LinkID获取当前link结构体*/
				wTmpEndLinkIndex = dsuLinkIndex[wTmpEndLinkId];
				pTmpEndLinkStru = dsuStaticHeadStru->pLinkStru + wTmpEndLinkIndex;

				wTmpNextMainLinkId = DSU_NULL_16;
				wTmpNextSideLinkId = DSU_NULL_16;

				/* 获取查询方向上的端点类型,与查询方向上的下一个link */
				if (EMAP_SAME_DIR == cQueryDir)
				{
					wTmpNextMainLinkId = pTmpEndLinkStru->wTmnlJointMainLkId;
					wTmpNextSideLinkId = pTmpEndLinkStru->wTmnlJointSideLkId;
				}
				else
				{
					wTmpNextMainLinkId = pTmpEndLinkStru->wOrgnJointMainLkId;
					wTmpNextSideLinkId = pTmpEndLinkStru->wOrgnJointSideLkId;
				}

				/* 2） */
				cFunFlag = 0u;/* 路径需要延申 */
				wAddLinkId = wTmpNextMainLinkId;
				wNewLinkId = wTmpNextSideLinkId;

				if (DSU_NULL_16 != wNewLinkId)
				{
					cFunFlag = 1u;/* 需要新建路径 */
				}

				/*判断是否存在方向变化点*/


				/* 处理路径 */
				wTmpPathLinkNum = szLinkNum[i];

				/* 延申当前路径,判断当前路径link容量是否达到上限 */
				if (49u == szLinkNum[i])
				{
					/* 容量未达上限，但最后一个link只能存为终点 */
					szPath[i][wTmpPathLinkNum] = DSU_NULL_16;
					szLinkNum[i]++;
				}
				else
				{
					/* 可以延申 */
					szPath[i][wTmpPathLinkNum] = wAddLinkId;
					szLinkNum[i]++;

					/*判断是否存在方向变化点*/
					cNxtLinkDirChanged = dquGetLinkLogicDirChangedByIdExp(pDsuEmapStru,wAddLinkId);
					if (((EMAP_DIRCHANGE_ORGN2ORGN == cNxtLinkDirChanged) || (EMAP_DIRCHANGE_TMNL2TMNL == cNxtLinkDirChanged))
						&& (cNxtLinkDirChanged == cCurLinkDirChanged))
					{
						/*存在方向变化点*/
						cQueryDir = ~cQueryDir;
					}
					else
					{
						/*不存在方向变化点，不处理*/
					}
					szLinkQueryDir[i] = cQueryDir;  /*存储此路径下的查询方向*/
				}

				if (1u == cFunFlag)
				{
					/* 生成一条新路径 */
					/* 1.判断当前路径link容量是否达到上限 */
					if (49u == wTmpPathLinkNum)
					{
						/* 容量未达上限，但最后一个link只能存为终点，新开辟路径也没有意义 */
					}
					else
					{
						/* 2.判断路径数量是否达到上限 */
						if ((25u > cTmpPathNum) && (DSU_NULL_16 != wNewLinkId))
						{
							/*为新增路径添加查询方向*/
							cTempPathIndex = cTmpPathNum + cPathNum;  /*当前路径与新增路径总数索引*/
							if (50U > cTempPathIndex)
							{
								cQueryDir = szLinkQueryDir[cTempPathIndex];

								/*判断是否存在方向变化点*/
								cNxtLinkDirChanged = dquGetLinkLogicDirChangedByIdExp(pDsuEmapStru,wNewLinkId);
								if (((EMAP_DIRCHANGE_ORGN2ORGN == cNxtLinkDirChanged) || (EMAP_DIRCHANGE_TMNL2TMNL == cNxtLinkDirChanged))
									&& (cNxtLinkDirChanged == cCurLinkDirChanged))
								{
									/*存在方向变化点*/
									cQueryDir = ~cQueryDir;
								}
								else
								{
									/*不存在方向变化点，不处理*/
								}
								szLinkQueryDir[cTempPathIndex] = cQueryDir;  /*存储新增路径下的查询方向*/
							}
							else
							{
								/*路径达到上限，不处理*/
							}

							/* 新增一条路径 */
							szTmpAddLinkNum[cTmpPathNum] = wTmpPathLinkNum;
							for (j = 0u; j < wTmpPathLinkNum; j++)
							{
								szTmpAddPath[cTmpPathNum][j] = szPath[i][j];
							}
							szTmpAddPath[cTmpPathNum][wTmpPathLinkNum] = wNewLinkId;
							szTmpAddLinkNum[cTmpPathNum]++;
							cTmpPathNum++;
						}
					}
				}
			}
			else
			{
				/* 记录查到终点的路径数量，用于判断每条路径是否都查到终点 */
				wTmpEndPathNum++;
			}
		}

		/* 拷贝路径，输出 */
		for (i = 0u; i < cTmpPathNum; i++)
		{
			if (cPathNum >= 50u)
			{
				/* 先判断是否超出路径容量 */
				break;
			}

			for (j = 0u; j < szTmpAddLinkNum[i]; j++)
			{
				szPath[cPathNum][j] = szTmpAddPath[i][j];
			}

			szLinkNum[cPathNum] = szTmpAddLinkNum[i];
			cPathNum++;
		}

		*pPathNum = cPathNum;
	}

	if ((1u == cRetVal) && (cPathNum == wTmpEndPathNum))
	{
		cRetVal = 2u;
	}

	return cRetVal;
}

/*********************************************************************************************
*函数功能:判断两个路径终点是否为同一link，若为同一link则输出这条路径(车车通信新增接口)
*输入参数: cPathANum A路径的数量
szPathA A路径数据
szLinkANum A路径的link数量
cPathBNum B路径的数量
szPathB B路径数据
szLinkBNum B路径的link数量
cABPathDir A/B路径的查询方向
*输出参数: szPath 找到路径数据
pPathLinkNum 找到路径数量
pLinkDirChangedNum link序列中的方向变化点数
*返回值:终点无相同link:0
终点存在相同link:1
*备注： add by lxc 2021-2-28
*********************************************************************************************/
UINT8 dsuPathSameEndLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT8 cPathANum, const UINT16 szPathA[50][50], const UINT16 szLinkANum[50], const UINT8 cPathBNum
	, const UINT16 szPathB[50][50], const UINT16 szLinkBNum[50], UINT16 szPath[100], UINT16* pPathLinkNum, UINT8* pLinkDirChangedNum, UINT8 cABPathDir)
{
	UINT8 retVal = 0u;
	UINT8 cKeepWorkFlag = 0u;
	UINT8 cFunCreatPathFlag = 0u;
	UINT8 i = 0u;
	UINT8 j = 0u;
	UINT16 x = 0u;

	UINT16 wTmpPathA1LinkNum = 0u;
	UINT16 wTmpPathB1LinkNum = 0u;
	UINT16 wTmpPathA2LinkNum = 0u;
	UINT16 wTmpPathB2LinkNum = 0u;

	UINT16 wTmpPathALinkNum = 0u;
	UINT16 wTmpPathBLinkNum = 0u;

	/* 最后一个link */
	UINT16 wTmpPathAEnd1LinkId = 0u;
	UINT16 wTmpPathBEnd1LinkId = 0u;

	/* 倒数第二个link */
	UINT16 wTmpPathAEnd2LinkId = 0u;
	UINT16 wTmpPathBEnd2LinkId = 0u;

	UINT16 wTmpPathLinkNum = 0u;
	UINT16 wTmpLinkIndex = 0u;

	UINT8 cCurLinkDirChanged = 0xFFU;  /*当前Link方向变化点*/
	UINT8 cNxtLinkDirChanged = 0xFFU;  /*下一Link方向变化点*/
	UINT8 cLinkDirChangedNum = 0U;  /*link序列方向变化点数量*/
	UINT8 cResiDue = 0U;

	if ((NULL == pDsuEmapStru) || (NULL == szPathA) || (NULL == szLinkANum) || (NULL == szPathB) || (NULL == szLinkBNum)
		|| (NULL == szPath) || (NULL == pPathLinkNum) || (NULL == pLinkDirChangedNum)
		|| ((DSU_TRUE != cABPathDir) && (DSU_FALSE != cABPathDir)))
	{
		/*do nothing*/
	}
	else
	{
		cKeepWorkFlag = 1u;
	}

	if (1u == cKeepWorkFlag)
	{
		for (i = 0u; i < cPathANum; i++)
		{
			for (j = 0u; j < cPathBNum; j++)
			{
				wTmpPathA1LinkNum = szLinkANum[i] - 1u;
				wTmpPathA2LinkNum = szLinkANum[i] - 2u;

				wTmpPathB1LinkNum = szLinkBNum[j] - 1u;
				wTmpPathB2LinkNum = szLinkBNum[j] - 2u;

				wTmpPathAEnd1LinkId = szPathA[i][wTmpPathA1LinkNum];
				wTmpPathAEnd2LinkId = szPathA[i][wTmpPathA2LinkNum];

				wTmpPathBEnd1LinkId = szPathB[j][wTmpPathB1LinkNum];
				wTmpPathBEnd2LinkId = szPathB[j][wTmpPathB2LinkNum];

				/* 以下三种情况为查询到相交路径，但需要区别处理 */
				if ((wTmpPathAEnd1LinkId == wTmpPathBEnd1LinkId) && (DSU_NULL_16 != wTmpPathAEnd1LinkId))
				{
					wTmpPathALinkNum = wTmpPathA1LinkNum;
					wTmpPathBLinkNum = wTmpPathB1LinkNum;
					cFunCreatPathFlag = 1u;
				}
				else if (wTmpPathAEnd2LinkId == wTmpPathBEnd1LinkId)
				{
					wTmpPathALinkNum = wTmpPathA2LinkNum;
					wTmpPathBLinkNum = wTmpPathB1LinkNum;
					cFunCreatPathFlag = 1u;
				}
				else if (wTmpPathAEnd2LinkId == wTmpPathBEnd2LinkId)
				{
					wTmpPathALinkNum = wTmpPathA2LinkNum;
					wTmpPathBLinkNum = wTmpPathB2LinkNum;
					cFunCreatPathFlag = 1u;
				}
				else if (wTmpPathAEnd1LinkId == wTmpPathBEnd2LinkId)
				{
					wTmpPathALinkNum = wTmpPathA1LinkNum;
					wTmpPathBLinkNum = wTmpPathB2LinkNum;
					cFunCreatPathFlag = 1u;
				}
				else
				{
					/*do nothing*/
				}


				if (1u == cFunCreatPathFlag)
				{
					break;
				}
			}
			if (1u == cFunCreatPathFlag)
			{
				break;
			}
		}

		wTmpPathALinkNum++;
		wTmpPathBLinkNum++;
		if ((1u == cFunCreatPathFlag) && (100u >= (wTmpPathALinkNum + wTmpPathBLinkNum)))
		{

			/* 存储前半段路径,不存储最后一个link */
			wTmpPathALinkNum--;
			for (x = 0u; x < wTmpPathALinkNum; x++)
			{
				szPath[wTmpPathLinkNum] = szPathA[i][x];
				wTmpPathLinkNum++;
			}

			/* 存储后半段路径,不存储端点 */
			for (x = 0u; x < wTmpPathBLinkNum; x++)
			{
				wTmpLinkIndex = wTmpPathBLinkNum - x;
				wTmpLinkIndex--;
				szPath[wTmpPathLinkNum] = szPathB[j][wTmpLinkIndex];
				wTmpPathLinkNum++;
			}

			/*查找link序列中的方向变化点*/
			for (x = 0u; x < (wTmpPathLinkNum - 1U); x++)
			{
				cCurLinkDirChanged = dquGetLinkLogicDirChangedByIdExp(pDsuEmapStru, szPath[x]);
				cNxtLinkDirChanged = dquGetLinkLogicDirChangedByIdExp(pDsuEmapStru, szPath[x + 1U]);
				if (((EMAP_DIRCHANGE_ORGN2ORGN == cNxtLinkDirChanged) || (EMAP_DIRCHANGE_TMNL2TMNL == cNxtLinkDirChanged))
					&& (cNxtLinkDirChanged == cCurLinkDirChanged))
				{
					/*存在方向变化点*/
					cLinkDirChangedNum++;
				}
				else
				{
					/*不存在方向变化点，不处理*/
				}
			}

			cResiDue = cLinkDirChangedNum % 2U;

			if (((0U == cResiDue) && (DSU_FALSE == cABPathDir)) || ((0U != cResiDue) && (DSU_TRUE == cABPathDir)))
			{
				*pPathLinkNum = wTmpPathLinkNum;
				*pLinkDirChangedNum = cLinkDirChangedNum;
				retVal = 1u;
			}
			else
			{
				/*查询失败，返回失败*/
			}
		}
	}

	return retVal;
}

/************************************************
*函数功能:	获取LINK逻辑方向变化点Exp
*输入参数:	DSU_EMAP_STRU *pDsuEmapStru
*			UINT16 linkId
*输出参数:
*返回值:	失败:0
*			成功:0x55 0xAA 0xFF LINK逻辑方向变化点
*备注：
************************************************/
UINT8 dquGetLinkLogicDirChangedByIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId)
{
	UINT8 RtnNo = 0u;
	DSU_LINK_STRU *pLinkStruStru = NULL;		/*LINK信息结构体指针*/

	if (NULL != pDsuEmapStru)
	{
		if ((linkId > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xFFFFU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[linkId]))
		{
			/*do nothing*/
		}
		else
		{
			/*获取当前link的方向变化点*/
			pLinkStruStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[linkId];
			RtnNo = pLinkStruStru->wLogicDirChanged;
		}
	}
	else
	{
		/* do nothing */
	}
	return RtnNo;
}

/************************************************
*函数功能:查询AB两端之间的距离
*输入参数:		pDsuEmapStru 电子地图数据
*				pLODAStru	A点
*				pLODBStru	B点
*输出参数:		pLinkNum	输出路径的link数量
*				szLinkId	输出路径的linkId序列
*				pDir		输出A->B的方向
*               pLinkDirChangedNum  方向变化点数量
*返回值:失败:DSU_NULL_32
*		成功:距离
*备注： add by lxc 2021-2-28
1、当起点和终点为同一点，且A点方向有效时，输出A点方向。wyd 20220303
2、修改3.0白盒未覆盖72、73，wyd 20220509
************************************************/
UINT32 dsuLenByPointExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODAStru, const LOD_STRU *pLODBStru, UINT16* pLinkNum, UINT16 szLinkId[100], UINT8 *pDir, UINT8* pLinkDirChangedNum)
{
	UINT32 reLen = DSU_NULL_32;
	UINT8 cFunRetVal = 0u;
	UINT8 cFun1RetVal = 0u;
	UINT8 cFun2RetVal = 0u;
	UINT8 cFun3RetVal = 0u;
	UINT8 cFun4RetVal = 0u;
	UINT8 cFun5RetVal = 0u;
	UINT8 cKeepWorkFlag = 0u;
	UINT16 dwCycle = 0u;

	/*全局变量改传参调用涉及的全局变量*/
	UINT16 LINKINDEXNUM = 0u;
	UINT16* dsuLinkIndex = NULL;
	DSU_STATIC_HEAD_STRU *dsuStaticHeadStru = NULL;

	/* 两点沿两个方向上的所有路径数据 */
	UINT16 szSameDirPathA[50][50] = { 0u };
	UINT16 szPathASameDirLinkNum[50] = { 0u };
	UINT8 wPathASameDirNum = 0u;

	UINT16 szConverDirPathA[50][50] = { 0u };
	UINT16 szPathAConverDirLinkNum[50] = { 0u };
	UINT8 wPathAConverDirNum = 0u;

	UINT16 szSameDirPathB[50][50] = { 0u };
	UINT16 szPathBSameDirLinkNum[50] = { 0u };
	UINT8 wPathBSameDirNum = 0u;

	UINT16 szConverDirPathB[50u][50] = { 0u };
	UINT16 szPathBConverDirLinkNum[50] = { 0u };
	UINT8 wPathBConverDirNum = 0u;

	/* 临时变量 */
	UINT16 i = 0u;
	UINT16 wTmpLinkId = 0u;
	UINT16 wTmpLinkIndex = 0u;
	DSU_LINK_STRU *pTmpLinkStru = NULL;
	UINT16 szOutputPath[100] = { 0u };
	UINT16 wOutputLinkNum = 0u;
	UINT32 wTmpLen = 0u;
	UINT8 cQueryADir = EMAP_SAME_DIR;
	UINT8 cQueryBDir = EMAP_SAME_DIR;

	UINT8 szASameQueryDir[50] = { 0U };     /*点A同向查询各路径的方向*/
	UINT8 szAConvertQueryDir[50] = { 0U };  /*点A反向查询各路径的方向*/
	UINT8 szBSameQueryDir[50] = { 0U };     /*点B同向查询各路径的方向*/
	UINT8 szBConvertQueryDir[50] = { 0U };  /*点B反向查询各路径的方向*/
	UINT8 cTmepDirChangedNum = 0U;   /*最终路径包含的方向变化点数量*/

	memset(szASameQueryDir, EMAP_SAME_DIR, sizeof(UINT8) * 50u);
	memset(szAConvertQueryDir, EMAP_CONVER_DIR, sizeof(UINT8) * 50u);
	memset(szBSameQueryDir, EMAP_SAME_DIR, sizeof(UINT8) * 50u);
	memset(szBConvertQueryDir, EMAP_CONVER_DIR, sizeof(UINT8) * 50u);

	if ((NULL != pLODAStru) && (NULL != pLODBStru) && (NULL != pDsuEmapStru) && (NULL != szLinkId)
		&& (NULL != pLinkNum) && (NULL != pDir) && (NULL != szLinkId) && (NULL != pDir)
		&& (0u != pLODAStru->Lnk) && (0u != pLODBStru->Lnk) && (NULL != pLinkDirChangedNum))
	{
		if ((pLODAStru->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffu == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLODAStru->Lnk])
			|| (pLODBStru->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffu == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLODBStru->Lnk]))
		{
			/*do nothing*/
		}
		else
		{
			cKeepWorkFlag = 1u;
			*pLinkNum = 0u;
			*pDir = EMAP_SAME_DIR;

			/* 全局变量透传 */
			LINKINDEXNUM = pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM;
			dsuLinkIndex = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex;
			dsuStaticHeadStru = pDsuEmapStru->dsuStaticHeadStru;
		}
	}
	else
	{
		/*none*/
	}

	if (1u == cKeepWorkFlag)
	{
		if ((LINKINDEXNUM < pLODAStru->Lnk) || (LINKINDEXNUM < pLODBStru->Lnk))
		{
			cKeepWorkFlag = 0u;
		}
		else
		{
			/* 偏移量检查 */
			wTmpLinkId = pLODAStru->Lnk;
			wTmpLinkIndex = dsuLinkIndex[wTmpLinkId];
			pTmpLinkStru = dsuStaticHeadStru->pLinkStru + wTmpLinkIndex;
			if (pTmpLinkStru->dwLength < pLODAStru->Off)
			{
				cKeepWorkFlag = 0u;
			}

			wTmpLinkId = pLODBStru->Lnk;
			wTmpLinkIndex = dsuLinkIndex[wTmpLinkId];
			pTmpLinkStru = dsuStaticHeadStru->pLinkStru + wTmpLinkIndex;
			if (pTmpLinkStru->dwLength < pLODBStru->Off)
			{
				cKeepWorkFlag = 0u;
			}
		}
	}


	if (1u == cKeepWorkFlag)
	{
		if (pLODAStru->Lnk == pLODBStru->Lnk)
		{
			/* 两点在同一link */
			if (pLODAStru->Off > pLODBStru->Off)
			{
				cQueryADir = EMAP_CONVER_DIR;
				reLen = pLODAStru->Off - pLODBStru->Off;
			}
			else
			{
				cQueryADir = EMAP_SAME_DIR;
				reLen = pLODBStru->Off - pLODAStru->Off;
			}

			szLinkId[wOutputLinkNum] = pLODAStru->Lnk;
			wOutputLinkNum++;
			*pLinkDirChangedNum = 0u;
			*pLinkNum = wOutputLinkNum;
		}
		else
		{
			/* 两点不在同一link */
			/* 初始化路径 */
			szSameDirPathA[0][0] = pLODAStru->Lnk;
			szPathASameDirLinkNum[0] = 1u;
			wPathASameDirNum = 1u;

			szConverDirPathA[0][0] = pLODAStru->Lnk;
			szPathAConverDirLinkNum[0] = 1u;
			wPathAConverDirNum = 1u;

			szSameDirPathB[0][0] = pLODBStru->Lnk;
			szPathBSameDirLinkNum[0] = 1u;
			wPathBSameDirNum = 1u;

			szConverDirPathB[0][0] = pLODBStru->Lnk;
			szPathBConverDirLinkNum[0] = 1u;
			wPathBConverDirNum = 1u;

			cFunRetVal = 1u;
			cFun1RetVal = 1u;
			cFun2RetVal = 1u;
			cFun3RetVal = 1u;
			cFun4RetVal = 1u;

			dwCycle = 1u;
			while (1u == cFunRetVal)
			{
				cFunRetVal = 0u;

				/* 1.调用函数向，向两个点的两个方向分别查询下一个link，生成新的路径 */
				if (2u != cFun1RetVal)
				{
					cFun1RetVal = dsuPathEndNextLinkExp(pDsuEmapStru, szASameQueryDir, &wPathASameDirNum, szSameDirPathA, szPathASameDirLinkNum);
				}

				if (2u != cFun2RetVal)
				{
					cFun2RetVal = dsuPathEndNextLinkExp(pDsuEmapStru, szAConvertQueryDir, &wPathAConverDirNum, szConverDirPathA, szPathAConverDirLinkNum);
				}

				if (2u != cFun3RetVal)
				{
					cFun3RetVal = dsuPathEndNextLinkExp(pDsuEmapStru, szBSameQueryDir, &wPathBSameDirNum, szSameDirPathB, szPathBSameDirLinkNum);
				}

				if (2u != cFun4RetVal)
				{
					cFun4RetVal = dsuPathEndNextLinkExp(pDsuEmapStru, szBConvertQueryDir, &wPathBConverDirNum, szConverDirPathB, szPathBConverDirLinkNum);
				}

				if (((2u == cFun1RetVal) && (cFun1RetVal == cFun2RetVal) && (cFun2RetVal == cFun3RetVal) && (cFun3RetVal == cFun4RetVal))
					|| ((cFun1RetVal == 0u) || (cFun2RetVal == 0u) || (cFun3RetVal == 0u) || (cFun4RetVal == 0u)))
				{
					/* 都查询到了终点，或查询失败，退出循环 */
					cFunRetVal = 0u;
				}
				else
				{
					/* 存在新的路径或者路径未到终点 */
					cFunRetVal = 1u;
				}

				cFun5RetVal = 0u;
				/* 2.判断两个点的发散路径最后的link是否存在相交link,并输出路径 */
				if (1u == cFunRetVal)
				{
					cQueryADir = EMAP_SAME_DIR;
					cQueryBDir = EMAP_CONVER_DIR;
					cFun5RetVal = dsuPathSameEndLinkExp(pDsuEmapStru, wPathASameDirNum, szSameDirPathA, szPathASameDirLinkNum
						, wPathBConverDirNum, szConverDirPathB, szPathBConverDirLinkNum, szOutputPath, &wOutputLinkNum, &cTmepDirChangedNum, DSU_FALSE);

					if (0u == cFun5RetVal)
					{
						cQueryADir = EMAP_CONVER_DIR;
						cQueryBDir = EMAP_SAME_DIR;
						cFun5RetVal = dsuPathSameEndLinkExp(pDsuEmapStru, wPathAConverDirNum, szConverDirPathA, szPathAConverDirLinkNum
							, wPathBSameDirNum, szSameDirPathB, szPathBSameDirLinkNum, szOutputPath, &wOutputLinkNum, &cTmepDirChangedNum, DSU_FALSE);
					}
					if (0u == cFun5RetVal)
					{
						cQueryADir = EMAP_SAME_DIR;
						cQueryBDir = EMAP_SAME_DIR;
						cFun5RetVal = dsuPathSameEndLinkExp(pDsuEmapStru, wPathASameDirNum, szSameDirPathA, szPathASameDirLinkNum
							, wPathBSameDirNum, szSameDirPathB, szPathBSameDirLinkNum, szOutputPath, &wOutputLinkNum, &cTmepDirChangedNum, DSU_TRUE);
					}
					if (0u == cFun5RetVal)
					{
						cQueryADir = EMAP_CONVER_DIR;
						cQueryBDir = EMAP_CONVER_DIR;
						cFun5RetVal = dsuPathSameEndLinkExp(pDsuEmapStru, wPathAConverDirNum, szConverDirPathA, szPathAConverDirLinkNum
							, wPathBConverDirNum, szConverDirPathB, szPathBConverDirLinkNum, szOutputPath, &wOutputLinkNum, &cTmepDirChangedNum, DSU_TRUE);
					}
				}

				if ((1u == cFunRetVal) && (1u == cFun5RetVal))
				{
					/* 4.计算距离 */
					/* 查询到了一条路径，输出本条路径与距离,终止查询 */
					cFunRetVal = 0u;
					*pLinkNum = wOutputLinkNum;
					for (i = 0u; i < wOutputLinkNum; i++)
					{
						szLinkId[i] = szOutputPath[i];

						wTmpLinkId = szLinkId[i];
						wTmpLinkIndex = dsuLinkIndex[wTmpLinkId];
						pTmpLinkStru = dsuStaticHeadStru->pLinkStru + wTmpLinkIndex;

						if ((wTmpLinkId != pLODAStru->Lnk) && (wTmpLinkId != pLODBStru->Lnk))
						{
							/* 累加距离 */
							wTmpLen += pTmpLinkStru->dwLength;
						}
					}

					wTmpLinkId = pLODAStru->Lnk;
					wTmpLinkIndex = dsuLinkIndex[wTmpLinkId];
					pTmpLinkStru = dsuStaticHeadStru->pLinkStru + wTmpLinkIndex;
					if (EMAP_SAME_DIR == cQueryADir)
					{
						wTmpLen += (pTmpLinkStru->dwLength - pLODAStru->Off);
					}
					else
					{
						wTmpLen += pLODAStru->Off;
					}

					wTmpLinkId = pLODBStru->Lnk;
					wTmpLinkIndex = dsuLinkIndex[wTmpLinkId];
					pTmpLinkStru = dsuStaticHeadStru->pLinkStru + wTmpLinkIndex;
					if (EMAP_SAME_DIR == cQueryBDir)
					{
						wTmpLen += (pTmpLinkStru->dwLength - pLODBStru->Off);
					}
					else
					{
						wTmpLen += pLODBStru->Off;
					}

					*pLinkDirChangedNum = cTmepDirChangedNum;  /*返回link序列中包含的方向变化点数量*/
					reLen = wTmpLen;
				}

				/* 只允许查询49次 */
				if (49u <= dwCycle)
				{
					break;
				}
				dwCycle++;
			}
		}

		if ((pLODAStru->Lnk == pLODBStru->Lnk) && (pLODAStru->Off == pLODBStru->Off) && ((EMAP_SAME_DIR == pLODAStru->Dir) || (EMAP_CONVER_DIR == pLODAStru->Dir)))
		{
			/*特殊情况：当起点和终点为同一点，且A点方向有效时，输出A点方向。wyd 20220303*/
			*pDir = pLODAStru->Dir;
		}
		else
		{
			/* 输出方向 */
			*pDir = cQueryADir;
		}
	}

	return reLen;
}


/************************************************
*函数功能:查询逻辑区段的相邻逻辑区段（数据初始化用）
*输入参数:	pDsuEmapStru	电子地图数据
*			LogicSgmtId		逻辑区段ID
*			Dir				查找方向
*输出参数:	pLogicSgmtNum	相邻逻辑区段数量
*			pLogicSgmtId	相邻逻辑区段ID
*			pDirChangedTag	逻辑方向变化点 
*			0x55起点相邻 0xAA终点相邻 0xFF非变化点
*返回值:	失败:0
*			成功:1
*备注：		仅供内部调用，不提供外部接口
add by wyd 20220309
************************************************/
UINT8 dsuGetAdjacentLogicSgmtImp(const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT8 Dir, OUT UINT8 *pLogicSgmtNum, OUT UINT16 *pLogicSgmtIdArray, OUT UINT8 *pDirChangedTag)
{
	UINT8 RtnNo = 0u;											/*函数返回值*/
	UINT8 tempRtnVal = 1u;										/*临时返回值*/
	UINT32 tempLength = 0u;										/*长度临时变量*/
	DSU_LOGIC_SGMT_STRU *pCurrLogicSgmtStru = NULL;				/*当前逻辑区段电子地图指针*/
	DSU_LOGIC_SGMT_STRU *pTempLogicSgmtStru = NULL;				/*临时逻辑区段电子地图指针*/
	DSU_AXLE_SGMT_STRU *pAxleSgmtStru = NULL;					/*计轴区段电子地图指针*/
	DSU_LINK_STRU *pLinkStru = NULL;							/*Link电子地图指针*/
	UINT16 i = 0u;												/*循环变量*/
	UINT16 j = 0u;												/*循环变量*/
	UINT8 ResultNum = 0u;										/*查询结果数量*/
	UINT16 ResultArray[MAX_ADJACENT_LOGICSGMT_NUM] = { 0u };	/*查询结果数组*/
	UINT16 tempLinkId = 0u;										/*Link ID临时变量*/
	UINT16 tempAxleId = 0u;										/*计轴器ID临时变量*/
	UINT8 tempDirChangedTag = 0xFFu;							/*逻辑方向变化点标志*/

	/*输入有效性检查*/
	if ((NULL == pDsuEmapStru) || (NULL == pLogicSgmtNum) || (NULL == pLogicSgmtIdArray) || (NULL == pDirChangedTag))
	{
		/*空指针防护*/
		RtnNo = 0u;
	}
    else if ((0u == LogicSgmtId) || (0xffffu == LogicSgmtId) || (LogicSgmtId > pDsuEmapStru->dsuEmapIndexStru->LOGICSGMTINDEXNUM)
        || (0xffffu == pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId]) || ((EMAP_SAME_DIR != Dir) && (EMAP_CONVER_DIR != Dir)))
	{
		/*逻辑区段ID和方向有效性检查*/
		RtnNo = 0u;
	}
	else
	{
		pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId];
		if (EMAP_SAME_DIR == Dir)
		{
			/*向右查找*/
			/*判断逻辑区段终端的link偏移量是否和终端link的长度相等*/
			tempLength = LinkLengthExp(pDsuEmapStru, pCurrLogicSgmtStru->wTmnlLkId);
			if (tempLength == pCurrLogicSgmtStru->dwTmnlLkOfst)
			{
				/*相等，说明该逻辑区段位于计轴区段最右边，查右边相邻计轴区段包含的逻辑区段*/
				/*找当前逻辑区段终点Link的电子地图信息*/
				pLinkStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pCurrLogicSgmtStru->wTmnlLkId];
				if (0xffffu == pLinkStru->wTmnlJointMainLkId)
				{
					/*没有终点相邻Link，线路终点，返回查找成功和数量0*/
					ResultNum = 0u;
				}
				else
				{
					/*非线路终点*/
					if (EMAP_AXLE_DOT == pLinkStru->wTmnlPointType)
					{
						/*Link终点是计轴器*/
						tempAxleId = pLinkStru->wTmnlPointId;
						/*返回逻辑方向变化点*/
						tempDirChangedTag = pLinkStru->wLogicDirChanged;
						/*遍历计轴区段*/
						for (i = 0u; i < pDsuEmapStru->dsuDataLenStru->wAxleSgmtLen; i++)
						{
							pAxleSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pAxleSgmtStru + i;
							/*考虑link逻辑方向变化点*/
							if (((EMAP_DIRCHANGE_TMNL2TMNL == pLinkStru->wLogicDirChanged) && (tempAxleId == pAxleSgmtStru->wTmnlAxleId) && (pAxleSgmtStru->wTmnlLkId != pLinkStru->wId))
								|| ((EMAP_DIRCHANGE_TMNL2TMNL != pLinkStru->wLogicDirChanged) && (tempAxleId == pAxleSgmtStru->wOrgnAxleId)))
							{
								/*该计轴区段的起点计轴器与上面的Link终点计轴器相等，说明该计轴区段与当前逻辑区段的右侧相邻*/
								/*遍历计轴区段包含的逻辑区段*/
								for (j = 0u; j < pAxleSgmtStru->wIncludeLogicSgmtNum; j++)
								{
									pTempLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru
										+ pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pAxleSgmtStru->wLogicSgmtId[j]];
									tempLength = LinkLengthExp(pDsuEmapStru, pTempLogicSgmtStru->wTmnlLkId);
									if (((EMAP_DIRCHANGE_TMNL2TMNL != pLinkStru->wLogicDirChanged) && (pTempLogicSgmtStru->wOrgnLkId == pLinkStru->wTmnlJointMainLkId) && (0U == pTempLogicSgmtStru->dwOrgnLkOfst))
										|| ((EMAP_DIRCHANGE_TMNL2TMNL == pLinkStru->wLogicDirChanged) && (pTempLogicSgmtStru->wTmnlLkId == pLinkStru->wTmnlJointMainLkId) && (tempLength == pTempLogicSgmtStru->dwTmnlLkOfst)))
									{
										/*遍历到的逻辑区段的起点link和查找的逻辑区段的终点link相等，
										且遍历到的逻辑区段起点偏移量等于0，则该逻辑区段是右侧相邻逻辑区段*/
										if (MAX_ADJACENT_LOGICSGMT_NUM <= ResultNum)
										{
											/*数组越界防护*/
											tempRtnVal = 0u;
										}
										else
										{
											ResultArray[ResultNum] = pTempLogicSgmtStru->wId;
											ResultNum += 1u;
										}
									}
									else
									{
										/*继续遍历*/
									}
								}
							}
							else
							{
								/*继续遍历*/
							}
						}

						if (0u == ResultNum)
						{
							/*非线路尽头，没找到相邻逻辑区段，报错*/
							tempRtnVal = 0u;
						}
						else
						{
							/*do nothing*/
						}
					}
					else
					{
						/*逻辑区段端点不是计轴器，报错*/
						tempRtnVal = 0u;
					}
				}
			}
			else
			{
				/*不相等，说明同一计轴区段内当前逻辑区段右侧还有逻辑区段，直接在当前计轴区段里找*/
				for (j = 0U; j < pCurrLogicSgmtStru->wAxleSgmtNum; j++)
				{
					pAxleSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pAxleSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuAxleSgmtIndex[pCurrLogicSgmtStru->wAxleSgmtId[j]];
					for (i = 0u; i < pAxleSgmtStru->wIncludeLogicSgmtNum; i++)
					{
						/*遍历当前计轴区段的所有逻辑区段*/
						pTempLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pAxleSgmtStru->wLogicSgmtId[i]];
						if ((pTempLogicSgmtStru->wOrgnLkId == pCurrLogicSgmtStru->wTmnlLkId) && (pTempLogicSgmtStru->dwOrgnLkOfst == pCurrLogicSgmtStru->dwTmnlLkOfst + 1u))
						{
							/*遍历到的逻辑区段的起点link和查找的逻辑区段的终点link相等，且遍历到的逻辑区段起点偏移量等于查找的逻辑区段的终点加1，则该逻辑区段是右侧相邻逻辑区段*/
							if (MAX_ADJACENT_LOGICSGMT_NUM <= ResultNum)
							{
								/*数组越界防护*/
								tempRtnVal = 0u;
							}
							else
							{
								ResultArray[ResultNum] = pTempLogicSgmtStru->wId;
								ResultNum += 1u;
							}
							break;
						}
						else
						{
							/*继续遍历*/
						}
					}
				}

				if (0u == ResultNum)
				{
					/*非线路尽头，没找到相邻逻辑区段，报错*/
					tempRtnVal = 0u;
				}
				else
				{
					/*do nothing*/
				}
			}
		}
		else /*if (EMAP_CONVER_DIR == Dir)*/
		{
			/*向左查找*/
			/*判断逻辑区段始端的link偏移量是否为0*/
			if (0u == pCurrLogicSgmtStru->dwOrgnLkOfst)
			{
				/*相等，说明该逻辑区段位于计轴区段最左边，查左边相邻计轴区段包含的逻辑区段*/
				/*找当前逻辑区段起点Link的电子地图信息*/
				pLinkStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pCurrLogicSgmtStru->wOrgnLkId];
				if (0xffffu == pLinkStru->wOrgnJointMainLkId)
				{
					/*没有起点相邻Link，线路终点，返回查找成功和数量0*/
					ResultNum = 0u;
				}
				else
				{
					/*非线路终点*/
					if (EMAP_AXLE_DOT == pLinkStru->wOrgnPointType)
					{
						/*Link起点是计轴器*/
						tempAxleId = pLinkStru->wOrgnPointId;
						/*返回逻辑方向变化点*/
						tempDirChangedTag = pLinkStru->wLogicDirChanged;
						/*遍历计轴区段*/
						for (i = 0u; i < pDsuEmapStru->dsuDataLenStru->wAxleSgmtLen; i++)
						{
							pAxleSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pAxleSgmtStru + i;
							/*考虑link逻辑方向变化点*/
							if (((EMAP_DIRCHANGE_ORGN2ORGN != pLinkStru->wLogicDirChanged) && (tempAxleId == pAxleSgmtStru->wTmnlAxleId))
								|| ((EMAP_DIRCHANGE_ORGN2ORGN == pLinkStru->wLogicDirChanged) && (tempAxleId == pAxleSgmtStru->wOrgnAxleId) && (pAxleSgmtStru->wOrgnLkId != pLinkStru->wId)))
							{
								/*该计轴区段的终点计轴器与上面的Link起点计轴器相等，说明该计轴区段与当前逻辑区段的左侧相邻*/
								/*遍历计轴区段包含的逻辑区段*/
								for (j = 0u; j < pAxleSgmtStru->wIncludeLogicSgmtNum; j++)
								{
									pTempLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru
										+ pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pAxleSgmtStru->wLogicSgmtId[j]];
									tempLength = LinkLengthExp(pDsuEmapStru, pTempLogicSgmtStru->wTmnlLkId);
									if (((EMAP_DIRCHANGE_ORGN2ORGN != pLinkStru->wLogicDirChanged) && (pTempLogicSgmtStru->wTmnlLkId == pLinkStru->wOrgnJointMainLkId) && (tempLength == pTempLogicSgmtStru->dwTmnlLkOfst))
										|| ((EMAP_DIRCHANGE_ORGN2ORGN == pLinkStru->wLogicDirChanged) && (pTempLogicSgmtStru->wOrgnLkId == pLinkStru->wOrgnJointMainLkId) && (0U == pTempLogicSgmtStru->dwOrgnLkOfst)))
									{
										/*遍历到的逻辑区段的终点link和查找的逻辑区段的起点link相等，
										且遍历到的逻辑区段终点偏移量等于LinkLength，则该逻辑区段是左侧相邻逻辑区段*/
										if (MAX_ADJACENT_LOGICSGMT_NUM <= ResultNum)
										{
											/*数组越界防护*/
											tempRtnVal = 0u;
										}
										else
										{
											ResultArray[ResultNum] = pTempLogicSgmtStru->wId;
											ResultNum += 1u;
										}
									}
									else
									{
										/*继续遍历*/
									}
								}
							}
							else
							{
								/*继续遍历*/
							}
						}

						if (0u == ResultNum)
						{
							/*非线路尽头，没找到相邻逻辑区段，报错*/
							tempRtnVal = 0u;
						}
						else
						{
							/*do nothing*/
						}
					}
					else
					{
						/*逻辑区段端点不是计轴器，报错*/
						tempRtnVal = 0u;
					}
				}
			}
			else
			{
				/*不相等，说明同一计轴区段内当前逻辑区段左侧还有逻辑区段，直接在当前计轴区段里找*/
				for (j = 0U; j < pCurrLogicSgmtStru->wAxleSgmtNum; j++)
				{
					pAxleSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pAxleSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuAxleSgmtIndex[pCurrLogicSgmtStru->wAxleSgmtId[j]];
					for (i = 0u; i < pAxleSgmtStru->wIncludeLogicSgmtNum; i++)
					{
						/*遍历当前计轴区段的所有逻辑区段*/
						pTempLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pAxleSgmtStru->wLogicSgmtId[i]];
						if ((pTempLogicSgmtStru->wTmnlLkId == pCurrLogicSgmtStru->wOrgnLkId) && (pTempLogicSgmtStru->dwTmnlLkOfst + 1u == pCurrLogicSgmtStru->dwOrgnLkOfst))
						{
							/*遍历到的逻辑区段的起点link和查找的逻辑区段的终点link相等，且遍历到的逻辑区段终点偏移量加1等于查找的逻辑区段的起点，则该逻辑区段是左侧相邻逻辑区段*/
							if (MAX_ADJACENT_LOGICSGMT_NUM <= ResultNum)
							{
								/*数组越界防护*/
								tempRtnVal = 0u;
							}
							else
							{
								ResultArray[ResultNum] = pTempLogicSgmtStru->wId;
								ResultNum += 1u;
							}
							break;
						}
						else
						{
							/*继续遍历*/
						}
					}
				}

				if (0u == ResultNum)
				{
					/*非线路尽头，没找到相邻逻辑区段，报错*/
					tempRtnVal = 0u;
				}
				else
				{
					/*do nothing*/
				}
			}
		}

		if (1u == tempRtnVal)
		{
			/*查找成功，输出*/
			*pLogicSgmtNum = ResultNum;
			CommonMemCpy(pLogicSgmtIdArray, ResultNum * sizeof(UINT16), &ResultArray, ResultNum * sizeof(UINT16));
			*pDirChangedTag = tempDirChangedTag;
			RtnNo = 1u;
		}
		else
		{
			/*查找失败*/
			*pLogicSgmtNum = 0u;
			*pDirChangedTag = 0xFFu;
			RtnNo = 0u;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:根据当前位置和位移，得到所有新位置
*输入参数:	pDsuEmapStru	电子地图数据
*			pLODStru		当前位置（含方向）
*			Displacement	位移
*输出参数:	pNewLODNum		新位置数量
*			pNewLODStru		新位置数组
*返回值:	失败:0
*			成功:1
*备注：		pNewLODStru需传入容量为50的数组
*			查到线路尽头时，认为查询成功，返回线路尽头点
*			add by wyd 20220621
************************************************/
UINT8 dsuPointByLenExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const LOD_STRU *pLODStru, IN const INT32 Displacement, OUT UINT8 *pNewLODNum, OUT LOD_STRU *pNewLODStru)
{
	UINT8 RtnNo = 0u;									/*函数返回值*/
	UINT8 tempRtnVal = 1u;								/*临时函数返回值*/
	UINT8 FindDir[50u] = { 0u };						/*查找方向*/
	UINT16 CurrentLinkIndex[50u] = { 0U };				/*当前Link Index*/
	DSU_LINK_STRU *pCurrentLinkStru[50u] = { NULL };	/*用于循环查找过程中临时保存当前Link结构体*/
	INT32 FindDisplacement = 0;							/*目标查找距离*/
	INT32 FindDisplacementSum[50u] = { 0 };				/*当前查找距离（所有路径的）*/
	UINT8 FindPathSum = 0u;								/*路径总数*/
	UINT8 i = 0u;										/*循环变量*/
	UINT16 k = 0u;										/*循环变量*/
	UINT8 CallFuncRtn = 0u;								/*调用函数的返回值*/
	UINT16 CurrentLinkId[2] = { 0u };					/*当前Link ID*/
	UINT8 BreakTag = DSU_FALSE;							/*跳出循环标志*/
	UINT8 NewLODNum = 0u;								/*查到的位置数量*/
	LOD_STRU NewLODStru[50u] = { 0 };					/*查到的位置数组*/

	if ((NULL == pDsuEmapStru) || (NULL == pLODStru) || (NULL == pNewLODNum) || (NULL == pNewLODStru))
	{
		/*入参错误*/
		RtnNo = 0u;
	}
	else
	{
		/*输出参数清空*/
		*pNewLODNum = 0u;
		CommonMemSet(pNewLODStru, 50u * sizeof(LOD_STRU), 0x00u, 50u * sizeof(LOD_STRU));

		if ((pLODStru->Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xFFFFu == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLODStru->Lnk]))
		{
			/*Link ID非法*/
			RtnNo = 0u;
		}
		else if (pLODStru->Off > LinkLengthExp(pDsuEmapStru, pLODStru->Lnk))
		{
			/*Link偏移量非法*/
			RtnNo = 0u;
		}
		else if ((EMAP_SAME_DIR != pLODStru->Dir) && (EMAP_CONVER_DIR != pLODStru->Dir))
		{
			/*方向非法*/
			RtnNo = 0u;
		}
		else
		{
			if (0 == Displacement)
			{
				/*特殊情况，Displacement等于0*/
				NewLODStru[0].Lnk = pLODStru->Lnk;
				NewLODStru[0].Off = pLODStru->Off;
				NewLODStru[0].Dir = pLODStru->Dir;
				NewLODNum = 1u;
			}
			else
			{
				/*确定查找方向*/
				if (EMAP_SAME_DIR == pLODStru->Dir)
				{
					if (0 < Displacement)
					{
						FindDir[0] = EMAP_SAME_DIR;
						FindDisplacement = Displacement;
					}
					else /*if (0 > Displacement)*/
					{
						FindDir[0] = EMAP_CONVER_DIR;
						FindDisplacement = -Displacement;
					}
				}
				else /*if (EMAP_CONVER_DIR == pLODStru->Dir)*/
				{
					if (0 < Displacement)
					{
						FindDir[0] = EMAP_CONVER_DIR;
						FindDisplacement = Displacement;
					}
					else /*if (0 > Displacement)*/
					{
						FindDir[0] = EMAP_SAME_DIR;
						FindDisplacement = -Displacement;
					}
				}

				/*获取第一个link信息*/
				CurrentLinkIndex[0] = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLODStru->Lnk];
				pCurrentLinkStru[0] = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + CurrentLinkIndex[0];
				FindPathSum = 1u;

				if (EMAP_SAME_DIR == FindDir[0])
				{
					/*初始查找方向为正向*/
					FindDisplacementSum[0] = (INT32)pCurrentLinkStru[0]->dwLength - (INT32)pLODStru->Off;
					if (FindDisplacementSum[0] >= FindDisplacement)
					{
						/*所求点在同一个link上，累加位移得到新位置，返回*/
						NewLODStru[0].Lnk = pCurrentLinkStru[0]->wId;
						NewLODStru[0].Off = pLODStru->Off + (UINT32)FindDisplacement;
						NewLODStru[0].Dir = FindDir[0];
						NewLODNum = 1u;
					}
					else
					{
						/*nothing*/
					}
				}
				else /*if (EMAP_CONVER_DIR == FindDir[0])*/
				{
					/*初始查找方向为反向*/
					FindDisplacementSum[0] = (INT32)pLODStru->Off;
					/*根据位移判断新位置是否在同一个Link上*/
					if (FindDisplacementSum[0] >= FindDisplacement)
					{
						/*所求点在同一个link上，累加位移得到新位置，返回*/
						NewLODStru[0].Lnk = pCurrentLinkStru[0]->wId;
						NewLODStru[0].Off = (UINT32)(FindDisplacementSum[0] - FindDisplacement);
						NewLODStru[0].Dir = FindDir[0];
						NewLODNum = 1u;
					}
					else
					{
						/*nothing*/
					}
				}

				if (1u == NewLODNum)
				{
					/*已经查找成功了*/
				}
				else
				{
					/*遍历所有路径*/
					for (i = 0u; i < FindPathSum; i++)
					{
						BreakTag = DSU_FALSE;
						for (k = 0U; k < pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM; k++)
						{
							/*此处循环并不是为了要遍历每一个Link，而是对循环计数，不能超过LINKINDEXNUM*/
							/*将相邻link置为无效*/
							CurrentLinkId[0] = DSU_NULL_16;
							CurrentLinkId[1] = DSU_NULL_16;
							/*查询所有相邻link*/
							CallFuncRtn = dsuGetAdjacentLinkIDArrayExp(pDsuEmapStru, pCurrentLinkStru[i]->wId, FindDir[i], CurrentLinkId);
							if (1u == CallFuncRtn)
							{
								if (((EMAP_SAME_DIR == FindDir[i]) && (EMAP_DIRCHANGE_TMNL2TMNL == pCurrentLinkStru[i]->wLogicDirChanged))
									|| ((EMAP_CONVER_DIR == FindDir[i]) && (EMAP_DIRCHANGE_ORGN2ORGN == pCurrentLinkStru[i]->wLogicDirChanged)))
								{
									/*跨方向变化点*/
									FindDir[i] = ~FindDir[i];
								}
								else
								{
									/*不处理*/
								}

								if (DSU_NULL_16 != CurrentLinkId[1])
								{
									/*相邻侧向link不为无效值，为侧向link开辟一条新路径添加到数组中*/
									CurrentLinkIndex[FindPathSum] = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[CurrentLinkId[1]];
									pCurrentLinkStru[FindPathSum] = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + CurrentLinkIndex[FindPathSum];
									FindDisplacementSum[FindPathSum] = FindDisplacementSum[i] + pCurrentLinkStru[FindPathSum]->dwLength;

									if (FindDisplacementSum[FindPathSum] >= FindDisplacement)
									{
										/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置在当前link上*/
										if (49u < NewLODNum)
										{
											/*数组越界*/
											tempRtnVal = 0u;
											BreakTag = DSU_TRUE;
										}
										else
										{
											/*直接找到所求的点，就不用在这条路径继续往前查了，无需记录该路径，FindPathSum不加1*/
											/*目前方向变化点不会布置在道岔上，使用之前路径的查询方向就行*/
											NewLODStru[NewLODNum].Lnk = pCurrentLinkStru[FindPathSum]->wId;
											if (EMAP_CONVER_DIR == FindDir[i])
											{
												NewLODStru[NewLODNum].Off = (UINT32)(FindDisplacementSum[FindPathSum] - FindDisplacement);
											}
											else if (EMAP_SAME_DIR == FindDir[i])
											{
												NewLODStru[NewLODNum].Off = (UINT32)((UINT32)FindDisplacement - ((UINT32)FindDisplacementSum[FindPathSum] - pCurrentLinkStru[FindPathSum]->dwLength));
											}
											else
											{
												/*方向有误，查询失败*/
												tempRtnVal = 0u;
												BreakTag = DSU_TRUE;
											}
											NewLODStru[NewLODNum].Dir = FindDir[i];
											NewLODNum += 1u;
										}
									}
									else
									{
										/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
										/*增加一条路径，后面i层继续循环，FindPathSum加1*/
										if (49u < FindPathSum)
										{
											/*数组越界，寄了*/
											tempRtnVal = 0u;
											BreakTag = DSU_TRUE;
										}
										else
										{
											FindDir[FindPathSum] = FindDir[i];
											FindPathSum += 1u;
										}
									}
								}
								else
								{
									/*do nothing*/
								}

								/*在当前路径处理相邻直向link，pCurrentLinkStru指向相邻Link，相邻Link变为当前Link*/
								CurrentLinkIndex[i] = pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[CurrentLinkId[0]];
								pCurrentLinkStru[i] = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + CurrentLinkIndex[i];
								FindDisplacementSum[i] += pCurrentLinkStru[i]->dwLength;
								if (FindDisplacementSum[i] >= FindDisplacement)
								{
									/*累加的位移值加上当前Link长度大于输入偏移量，说明目的位置当前link上*/
									if (49u < NewLODNum)
									{
										/*数组越界*/
										tempRtnVal = 0u;
									}
									else
									{
										NewLODStru[NewLODNum].Lnk = pCurrentLinkStru[i]->wId;
										if (EMAP_CONVER_DIR == FindDir[i])
										{
											NewLODStru[NewLODNum].Off = (UINT32)(FindDisplacementSum[i] - FindDisplacement);
										}
										else if (EMAP_SAME_DIR == FindDir[i])
										{
											NewLODStru[NewLODNum].Off = (UINT32)((UINT32)FindDisplacement - ((UINT32)FindDisplacementSum[i] - pCurrentLinkStru[i]->dwLength));
										}
										else
										{
											/*方向有误，查询失败*/
											tempRtnVal = 0u;
											BreakTag = DSU_TRUE;
										}
										NewLODStru[NewLODNum].Dir = FindDir[i];
										NewLODNum += 1u;
									}
									BreakTag = DSU_TRUE;
								}
								else
								{
									/*累加的位移值加上当前Link长度小于输入偏移量，说明目的位置不在当前link上，循环继续向前查找下一个Link*/
								}
							}
							else if (0xFFu == CallFuncRtn)
							{
								/*查询到线路尽头，返回线路尽头的点*/
								if (49u < NewLODNum)
								{
									/*数组越界*/
									tempRtnVal = 0u;
								}
								else
								{
									NewLODStru[NewLODNum].Lnk = pCurrentLinkStru[i]->wId;
									if (EMAP_CONVER_DIR == FindDir[i])
									{
										NewLODStru[NewLODNum].Off = 0U;
									}
									else if (EMAP_SAME_DIR == FindDir[i])
									{
										NewLODStru[NewLODNum].Off = LinkLengthExp(pDsuEmapStru, pCurrentLinkStru[i]->wId);
									}
									else
									{
										/*方向有误，查询失败*/
										tempRtnVal = 0u;
										BreakTag = DSU_TRUE;
									}
									NewLODStru[NewLODNum].Dir = FindDir[i];
									NewLODNum += 1u;
								}
								BreakTag = DSU_TRUE;
							}
							else
							{
								/*查询相邻link失败，寄*/
								tempRtnVal = 0u;
							}

							if (DSU_TRUE == BreakTag)
							{
								/*可以跳出循环*/
								break;
							}
							else
							{
								/*do nothing*/
							}
						}

						if (k >= pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM)
						{
							/*把所有link都循环了一遍还没查完，寄*/
							tempRtnVal = 0u;
						}
						else
						{
							/*do nothing*/
						}

						if (1u != tempRtnVal)
						{
							/*已经查询失败了，跳出循环*/
							break;
						}
						else
						{
							/*do nothing*/
						}
					}
				}
			}

			if (1u == tempRtnVal)
			{
				/*查询成功，赋值*/
				*pNewLODNum = NewLODNum;
				CommonMemCpy(pNewLODStru, 50u * sizeof(LOD_STRU), NewLODStru, 50u * sizeof(LOD_STRU));
				RtnNo = 1u;
			}
			else
			{
				/*查询失败，输出参数已清空*/
				RtnNo = 0u;
			}
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	查询逻辑区段的相邻逻辑区段
*输入参数:	LogicSgmtId		逻辑区段ID
*			Dir				查找方向
*			PointNum		道岔数量
*			pPointInfo		道岔信息
*输出参数:	pLogicSgmtId	相邻逻辑区段ID
*返回值:	失败:0
*			成功且不跨方向变化点:1
*			成功且跨方向变化点:2
*备注：		add by wyd 20220309
************************************************/
UINT8 dsuGetAdjacentLogicSgmtExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT8 Dir, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *pPointInfo, OUT UINT16 *pLogicSgmtId)
{
	UINT8 RtnNo = 0U;                                               /*函数返回值*/
	UINT8 tempRtnVal = 1U;                                          /*多个相邻逻辑区段时，查询结果临时变量*/
	UINT8 FunRtn = 0U;                                              /*承接函数返回值变量*/
	DSU_LOGIC_SGMT_STRU *pCurrLogicSgmtStru = NULL;					/*当前逻辑区段电子地图指针*/
	UINT8 AllAdjLogicSgmtNum = 0U;                                  /*所有相邻逻辑区段数量*/
	UINT16 AllAdjLogicSgmtId[MAX_ADJACENT_LOGICSGMT_NUM] = { 0u };  /*所有相邻逻辑区段数组*/
	UINT16 i = 0U;                                                  /*循环变量*/
	UINT16 tempLinkId = 0U;                                         /*Link ID临时变量*/
	UINT16 tempAdjLinkId = 0U;                                      /*相邻Link ID临时变量*/
	UINT16 LinkCount = 0U;                                          /*Link查找计数*/

	/*输入有效性检查*/
	if ((NULL == pDsuEmapStru) || (NULL == pPointInfo) || (NULL == pLogicSgmtId))
	{
		/*空指针防护*/
		RtnNo = 0U;
	}
	else if ((0U == LogicSgmtId) || (0xffffU == LogicSgmtId)
		|| (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId]) || (LogicSgmtId > pDsuEmapStru->dsuEmapIndexStru->LOGICSGMTINDEXNUM)
		|| ((EMAP_SAME_DIR != Dir) && (EMAP_CONVER_DIR != Dir)))
	{
		/*逻辑区段ID和方向有效性检查*/
		RtnNo = 0U;
	}
	else
	{
		pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId];
		if (EMAP_SAME_DIR == Dir)
		{
			if (MAX_ADJACENT_LOGICSGMT_NUM < pCurrLogicSgmtStru->wTmnlLogicSgmtNum)
			{
				/*数组越界防护*/
				RtnNo = 0U;
			}
			else
			{
				AllAdjLogicSgmtNum = pCurrLogicSgmtStru->wTmnlLogicSgmtNum;
				CommonMemCpy(AllAdjLogicSgmtId, pCurrLogicSgmtStru->wTmnlLogicSgmtNum * sizeof(UINT16), pCurrLogicSgmtStru->wTmnlLogicSgmtId, pCurrLogicSgmtStru->wTmnlLogicSgmtNum * sizeof(UINT16));
				if (0U == AllAdjLogicSgmtNum)
				{
					/*该方向没有相邻逻辑区段，查找成功，ID返回无效值*/
					*pLogicSgmtId = DSU_NULL_16;
					RtnNo = 1U;
				}
				else if (1U == AllAdjLogicSgmtNum)
				{
					/*该方向有且仅有一个相邻逻辑区段，查找成功*/
					*pLogicSgmtId = AllAdjLogicSgmtId[0];
					RtnNo = 1U;
				}
				else
				{
					/*该方向有多于1个相邻逻辑区段，根据道岔状态继续判断*/
					for (i = 0U; i < AllAdjLogicSgmtNum; i++)
					{
						/*判断思路是根据道岔查逻辑区段起点Link向右的相邻Link，循环往右查，查到逻辑区段终点Link则说明该逻辑区段是根据道岔状态相邻的逻辑区段*/
						pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[AllAdjLogicSgmtId[i]];
						tempLinkId = pCurrLogicSgmtStru->wOrgnLkId;
						tempRtnVal = 1U;
						LinkCount = 0U;

						/*当找到的Link为逻辑区段终点Link时，跳出循环*/
						while (tempLinkId != pCurrLogicSgmtStru->wTmnlLkId)
						{
							if (4U <= LinkCount)
							{
								/*目前数据结构一个逻辑区段最多包含5个Link，查了4个相邻Link后还需要查，肯定超出1个逻辑区段范围了，报错*/
								tempRtnVal = 0U;
								break;
							}
							else
							{
								/*查找相邻Link*/
								FunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, tempLinkId, EMAP_SAME_DIR, PointNum, pPointInfo, &tempAdjLinkId);
								if (1U == FunRtn)
								{
									/*查找成功，赋值计数*/
									tempLinkId = tempAdjLinkId;
									LinkCount += 1U;
								}
								else
								{
									/*查找失败，报错*/
									tempRtnVal = 0U;
									break;
								}
							}
						}

						if (1U == tempRtnVal)
						{
							/*查找成功，返回当前逻辑区段*/
							*pLogicSgmtId = AllAdjLogicSgmtId[i];
							RtnNo = 1U;
							break;
						}
						else
						{
							/*继续查找*/
						}
					}
				}
			}

			if ((1U == RtnNo) && (EMAP_DIRCHANGE_TMNL2TMNL == pCurrLogicSgmtStru->wDirChangedTag))
			{
				/*跨方向变化点*/
				RtnNo = 2U;
			}
			else
			{
				/*nothing*/
			}
		}
		else /*if (EMAP_CONVER_DIR == Dir)*/
		{
			if (MAX_ADJACENT_LOGICSGMT_NUM < pCurrLogicSgmtStru->wOrgnLogicSgmtNum)
			{
				/*数组越界防护*/
				RtnNo = 0U;
			}
			else
			{
				AllAdjLogicSgmtNum = pCurrLogicSgmtStru->wOrgnLogicSgmtNum;
				CommonMemCpy(AllAdjLogicSgmtId, pCurrLogicSgmtStru->wOrgnLogicSgmtNum * sizeof(UINT16), pCurrLogicSgmtStru->wOrgnLogicSgmtId, pCurrLogicSgmtStru->wOrgnLogicSgmtNum * sizeof(UINT16));
				if (0U == AllAdjLogicSgmtNum)
				{
					/*该方向没有相邻逻辑区段，查找成功，ID返回无效值*/
					*pLogicSgmtId = DSU_NULL_16;
					RtnNo = 1U;
				}
				else if (1U == AllAdjLogicSgmtNum)
				{
					/*该方向有且仅有一个相邻逻辑区段，查找成功*/
					*pLogicSgmtId = AllAdjLogicSgmtId[0];
					RtnNo = 1U;
				}
				else
				{
					/*该方向有多于1个相邻逻辑区段，根据道岔状态继续判断*/
					for (i = 0U; i < AllAdjLogicSgmtNum; i++)
					{
						/*判断思路是根据道岔查逻辑区段起点Link向右的相邻Link，循环往右查，查到逻辑区段终点Link则说明该逻辑区段是根据道岔状态相邻的逻辑区段*/
						pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[AllAdjLogicSgmtId[i]];
						tempLinkId = pCurrLogicSgmtStru->wTmnlLkId;
						tempRtnVal = 1U;
						LinkCount = 0U;

						/*当找到的Link为逻辑区段终点Link时，跳出循环*/
						while (tempLinkId != pCurrLogicSgmtStru->wOrgnLkId)
						{
							if (4U <= LinkCount)
							{
								/*目前数据结构一个逻辑区段最多包含5个Link，查了4个相邻Link后还需要查，肯定超出1个逻辑区段范围了，报错*/
								tempRtnVal = 0U;
								break;
							}
							else
							{
								/*查找相邻Link*/
								FunRtn = dsuGetAdjacentLinkIDExp(pDsuEmapStru, tempLinkId, EMAP_CONVER_DIR, PointNum, pPointInfo, &tempAdjLinkId);
								if (1U == FunRtn)
								{
									/*查找成功，赋值计数*/
									tempLinkId = tempAdjLinkId;
									LinkCount += 1U;
								}
								else
								{
									/*查找失败，报错*/
									tempRtnVal = 0U;
									break;
								}
							}
						}

						if (1U == tempRtnVal)
						{
							/*查找成功，返回当前逻辑区段*/
							*pLogicSgmtId = AllAdjLogicSgmtId[i];
							RtnNo = 1U;
							break;
						}
						else
						{
							/*继续查找*/
						}
					}
				}
			}

			if ((1U == RtnNo) 
				&& (((EMAP_DIRCHANGE_ORGN2ORGN == pCurrLogicSgmtStru->wDirChangedTag) && (EMAP_CONVER_DIR == Dir))
					|| ((EMAP_DIRCHANGE_TMNL2TMNL == pCurrLogicSgmtStru->wDirChangedTag) && (EMAP_SAME_DIR == Dir))))
			{
				/*跨方向变化点*/
				RtnNo = 2U;
			}
			else
			{
				/*nothing*/
			}
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	查询【连续】逻辑区段序列对应的连续Link序列（最多500）
*输入参数:	LogicSgmtNum	逻辑区段数量
*			pLogicSgmtId	逻辑区段序列
*			Dir				查询方向
*输出参数:	pLinkNum		Link数量
*			pLinkId			Link序列
*返回值:	失败:0
*			成功:1
*备注：		需保证输入逻辑区段连续
*			add by wyd 20220312
************************************************/
UINT8 dsuLogicSgmtToLinkExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtNum, IN const UINT16 *pLogicSgmtId, IN const UINT8 Dir, OUT UINT16 *pLinkNum, OUT UINT16 *pLinkId)
{
	UINT8 RtnNo = 0U;
	UINT8 tempRtnVal = 1U;
	UINT16 i = 0U;
	UINT16 j = 0U;
	UINT16 k = 0U;
	UINT16 ResultLinkNum = 0U;
	UINT16 ResultLinkId[OBJECT_NUM_MAX] = { 0U };
	DSU_LOGIC_SGMT_STRU *pLogicSgmtStru = NULL;
	UINT16 AddLinkNum = 0U;							/*待添加的link数量*/
	UINT16 AddLinkId[OBJECT_NUM_MAX] = { 0U };		/*待添加的link编号*/
	UINT8 DuplicateTag = DSU_FALSE;				/*link重复标志*/
	UINT8 MemRtn = 1U;

	if ((NULL == pLogicSgmtId) || (NULL == pLinkNum) || (NULL == pLinkId) || (NULL == pDsuEmapStru))
	{
		/*空指针防护*/
		RtnNo = 0U;
	}
	else if ((EMAP_SAME_DIR != Dir) && (EMAP_CONVER_DIR != Dir))
	{
		/*输入方向防护*/
		*pLinkNum = 0U;
		RtnNo = 0U;
	}
	else
	{
		/*遍历逻辑区段*/
		for (i = 0U; i < LogicSgmtNum; i++)
		{
			if ((pLogicSgmtId[i] > pDsuEmapStru->dsuEmapIndexStru->LOGICSGMTINDEXNUM) || (0xFFFFU == pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pLogicSgmtId[i]]))
			{
				/*逻辑区段ID合法性防护*/
				tempRtnVal = 0U;
				break;
			}
			else
			{
				/*初始化待添加link数组*/
				AddLinkNum = 0U;
				MemRtn &= CommonMemSet(AddLinkId, OBJECT_NUM_MAX * sizeof(UINT16), 0x00U, OBJECT_NUM_MAX * sizeof(UINT16));

				pLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pLogicSgmtId[i]];
				if (pLogicSgmtStru->wOrgnLkId == pLogicSgmtStru->wTmnlLkId)
				{
					AddLinkNum = 1U;
					AddLinkId[0] = pLogicSgmtStru->wOrgnLkId;
				}
				else
				{
					/*逻辑区段跨Link*/
					if (EMAP_SAME_DIR == Dir)
					{
						/*方向向右*/
						if (0xFFFFU == pLogicSgmtStru->wMiddleLinkId[0])
						{
							/*没有MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wOrgnLkId;
							AddLinkId[1] = pLogicSgmtStru->wTmnlLkId;
							AddLinkNum = 2U;
						}
						else if (0xffffU == pLogicSgmtStru->wMiddleLinkId[1])
						{
							/*有1个MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wOrgnLkId;
							AddLinkId[1] = pLogicSgmtStru->wMiddleLinkId[0];
							AddLinkId[2] = pLogicSgmtStru->wTmnlLkId;
							AddLinkNum = 3U;
						}
						else if (0xffffU == pLogicSgmtStru->wMiddleLinkId[2])
						{
							/*有2个MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wOrgnLkId;
							AddLinkId[1] = pLogicSgmtStru->wMiddleLinkId[0];
							AddLinkId[2] = pLogicSgmtStru->wMiddleLinkId[1];
							AddLinkId[3] = pLogicSgmtStru->wTmnlLkId;
							AddLinkNum = 4U;
						}
						else
						{
							/*有3个MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wOrgnLkId;
							AddLinkId[1] = pLogicSgmtStru->wMiddleLinkId[0];
							AddLinkId[2] = pLogicSgmtStru->wMiddleLinkId[1];
							AddLinkId[3] = pLogicSgmtStru->wMiddleLinkId[2];
							AddLinkId[4] = pLogicSgmtStru->wTmnlLkId;
							AddLinkNum = 5U;
						}
					}
					else /*if (EMAP_CONVER_DIR == Dir)*/
					{
						/*方向向左*/
						if (0xffffU == pLogicSgmtStru->wMiddleLinkId[0])
						{
							/*没有MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wTmnlLkId;
							AddLinkId[1] = pLogicSgmtStru->wOrgnLkId;
							AddLinkNum = 2U;
						}
						else if (0xffffU == pLogicSgmtStru->wMiddleLinkId[1])
						{
							/*有1个MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wTmnlLkId;
							AddLinkId[1] = pLogicSgmtStru->wMiddleLinkId[0];
							AddLinkId[2] = pLogicSgmtStru->wOrgnLkId;
							AddLinkNum = 3U;
						}
						else if (0xFFFFU == pLogicSgmtStru->wMiddleLinkId[2])
						{
							/*有2个MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wTmnlLkId;
							AddLinkId[1] = pLogicSgmtStru->wMiddleLinkId[1];
							AddLinkId[2] = pLogicSgmtStru->wMiddleLinkId[0];
							AddLinkId[3] = pLogicSgmtStru->wOrgnLkId;
							AddLinkNum = 4U;
						}
						else
						{
							/*有3个MiddleLink*/
							AddLinkId[0] = pLogicSgmtStru->wTmnlLkId;
							AddLinkId[1] = pLogicSgmtStru->wMiddleLinkId[2];
							AddLinkId[2] = pLogicSgmtStru->wMiddleLinkId[1];
							AddLinkId[3] = pLogicSgmtStru->wMiddleLinkId[0];
							AddLinkId[4] = pLogicSgmtStru->wOrgnLkId;
							AddLinkNum = 5U;
						}
					}
				}
			}

			/*依次添加并查重*/
			for (j = 0U; j < AddLinkNum; j++)
			{
				DuplicateTag = DSU_FALSE;
				for (k = 0U; k < ResultLinkNum; k++)
				{
					if (AddLinkId[j] == ResultLinkId[k])
					{
						/*有重复*/
						DuplicateTag = DSU_TRUE;
						break;
					}
					else
					{
						/*继续遍历*/
					}
				}

				if (DSU_TRUE == DuplicateTag)
				{
					/*已有该link，不添加*/
				}
				else
				{
					/*添加*/
					if (OBJECT_NUM_MAX <= ResultLinkNum)
					{
						/*数组已满*/
						tempRtnVal = 0U;
					}
					else
					{
						ResultLinkId[ResultLinkNum] = AddLinkId[j];
						ResultLinkNum += 1U;
					}
				}
			}
		}

		if (1U == tempRtnVal)
		{
			/*查询成功*/
			*pLinkNum = ResultLinkNum;
			MemRtn &= CommonMemCpy(pLinkId, ResultLinkNum * sizeof(UINT16), ResultLinkId, ResultLinkNum * sizeof(UINT16));
			RtnNo = 1U;
		}
		else
		{
			/*查询失败*/
			*pLinkNum = 0U;
			RtnNo = 0U;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	查询逻辑区段端点
*输入参数:	LogicSgmtId		逻辑区段ID
*			Dir				查询方向（输入EMAP_SAME_DIR返回右侧端点，EMAP_CONVER_DIR返回左侧端点）
*输出参数:	pEndLoc			逻辑区段端点（方向返回查询方向）
*返回值:	失败:0
*			成功:1
*备注：		add by wyd 20220312
************************************************/
UINT8 dsuGetLogicSgmtEndLocExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT8 Dir, OUT LOD_STRU *pEndLoc)
{
	UINT8 RtnNo = 0U;
	DSU_LOGIC_SGMT_STRU *pLogicSgmtStru = NULL;

	if ((NULL == pEndLoc) || (NULL == pDsuEmapStru))
	{
		/*空指针防护*/
		RtnNo = 0U;
	}
	else if (((EMAP_SAME_DIR != Dir) && (EMAP_CONVER_DIR != Dir))
		|| (LogicSgmtId > pDsuEmapStru->dsuEmapIndexStru->LOGICSGMTINDEXNUM) || (0xFFFFU == pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId]))
	{
		/*方向、逻辑区段ID合法性防护*/
		CommonMemSet(pEndLoc, sizeof(LOD_STRU), 0x00u, sizeof(LOD_STRU));
		RtnNo = 0U;
	}
	else
	{
		pLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId];
		if (EMAP_SAME_DIR == Dir)
		{
			pEndLoc->Lnk = pLogicSgmtStru->wTmnlLkId;
			pEndLoc->Off = pLogicSgmtStru->dwTmnlLkOfst;
			pEndLoc->Dir = EMAP_SAME_DIR;
		}
		else /*if (EMAP_CONVER_DIR == Dir)*/
		{
			pEndLoc->Lnk = pLogicSgmtStru->wOrgnLkId;
			pEndLoc->Off = pLogicSgmtStru->dwOrgnLkOfst;
			pEndLoc->Dir = EMAP_CONVER_DIR;
		}
		RtnNo = 1U;
	}

	return RtnNo;
}

/************************************************
*函数功能:	查询逻辑区段长度
*输入参数:	LogicSgmtId		逻辑区段ID
*输出参数:	pDisp			逻辑区段长度
*返回值:	失败:0
*			成功:1
*备注：		add by wyd 20230703
************************************************/
UINT8 dsuGetLogicSgmtDispExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, OUT UINT32 *pDisp)
{
	UINT8 RtnNo = 0U;
	UINT8 CallFunRtn = 0U;
	LOD_STRU StartLoc = { 0 };
	LOD_STRU EndLoc = { 0 };
	UINT16 LinkNum = 0U;
	UINT16 LinkId[100] = { 0U };
	UINT8 Dir = 0U;
	UINT32 Disp = DSU_NULL_32;
	UINT8 ChangeNum = 0u;

	if ((NULL == pDisp) || (NULL == pDsuEmapStru))
	{
		/*空指针防护*/
		RtnNo = 0U;
	}
	else if ((LogicSgmtId > pDsuEmapStru->dsuEmapIndexStru->LOGICSGMTINDEXNUM) || (0xFFFFU == pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId]))
	{
		/*方向、逻辑区段ID合法性防护*/
		*pDisp = DSU_NULL_32;
		RtnNo = 0U;
	}
	else
	{
		CallFunRtn = dsuGetLogicSgmtEndLocExp(pDsuEmapStru, LogicSgmtId, EMAP_SAME_DIR, &EndLoc);
		CallFunRtn &= dsuGetLogicSgmtEndLocExp(pDsuEmapStru, LogicSgmtId, EMAP_CONVER_DIR, &StartLoc);
		if (1U == CallFunRtn)
		{
			Disp = dsuLenByPointExp(pDsuEmapStru, &StartLoc, &EndLoc, &LinkNum, LinkId, &Dir,&ChangeNum);
			if (DSU_NULL_32 == Disp)
			{
				/*nothing*/
			}
			else
			{
				RtnNo = 1U;
			}
		}
		else
		{
			/*nothing*/
		}

		*pDisp = Disp;
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定起点、终点、方向和道岔信息，查询包含的逻辑区段序列
*输入参数:	LocStart			起点（含查找方向）
*			LocEnd				终点
*			PointNum			道岔数量
*			PointInfo			道岔信息
*输出参数:	pLogicSgmtNum		逻辑区段数量（最大500）
*			pLogicSgmtStru		逻辑区段序列
*返回值:	失败:0
*			成功:1
*备注：		add by wyd 20220310
*			修复YF0069-2173：编码规范问题：判断条件的优先级未使用括号()进行明确区分。wyd 20220720
*			重构 by wyd 20221012
*			修复YF0069-Bug-2779 对电子地图指针进行防空 kuan.he 220221107
************************************************/
UINT8 dsuLogicSgmtCalcuByLocExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const LOD_STRU LocStart, IN const LOD_STRU LocEnd, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *pPointInfo, OUT UINT16 *pLogicSgmtNum, OUT UINT16 *pLogicSgmtId)
{
	UINT8 RtnNo = 0U;                                    	/*函数返回值*/
	UINT8 tempRtnVal = 1U;                               	/*查询成功标志*/
	UINT8 FunRtn = 0U;                                   	/*承接返回值*/
	DSU_LINK_STRU *pLinkStru = NULL;                    	/*Link电子地图指针*/
	DSU_LOGIC_SGMT_STRU *pLogicSgmtStru = NULL;         	/*逻辑区段电子地图指针*/
	UINT32 LinkLengthStart = 0U;                         	/*起点所在Link长度（校验用）*/
	UINT32 LinkLengthEnd = 0U;                           	/*终点所在Link长度（校验用）*/
	UINT16 k = 0U;                                       	/*循环变量*/
	UINT16 ResultLogicSgmtNum = 0U;                      	/*逻辑区段数量查询结果*/
	UINT16 ResultLogicSgmtId[OBJECT_NUM_MAX] = { 0U };   	/*逻辑区段序列查询结果*/
	LOD_STRU LogicStartLoc = { 0 };							/*逻辑区段起点（左侧端点）*/
	LOD_STRU LogicEndLoc = { 0 };							/*逻辑区段终点（右侧端点）*/
	UINT8 LocInArea1 = 0U;									/*起/终点是否在区域内标识*/
	UINT8 LocInArea2 = 0U;									/*起/终点是否在区域内标识*/
	UINT16 StartLogicSgmtId = 0xFFFFU;						/*起点所在逻辑区段ID*/
	UINT16 EndLogicSgmtId = 0xFFFFU;						/*终点所在逻辑区段ID*/
	UINT16 AdjLogicSgmtId = 0xFFFFU;						/*相邻逻辑区段ID*/
	LOD_STRU tempEndLoc = { 0 };							/*终点临时变量（特殊处理方向用）*/
	UINT8 SearchDir = 0U;										/*查找方向*/

	if ((NULL == pPointInfo) || (NULL == pLogicSgmtNum) || (NULL == pLogicSgmtId) || (NULL == pDsuEmapStru))
	{
		RtnNo = 0U;
	}
	else if ((LocStart.Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[LocStart.Lnk])
		|| (LocEnd.Lnk > pDsuEmapStru->dsuEmapIndexStru->LINKINDEXNUM) || (0xffffU == pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[LocEnd.Lnk])
		|| ((EMAP_SAME_DIR != LocStart.Dir) && (EMAP_CONVER_DIR != LocStart.Dir)))
	{
		/*Link ID、查找方向防护*/
		*pLogicSgmtNum = 0U;
		RtnNo = 0U;
	}
	else
	{
		LinkLengthStart = LinkLengthExp(pDsuEmapStru, LocStart.Lnk);
		LinkLengthEnd = LinkLengthExp(pDsuEmapStru, LocEnd.Lnk);
		if ((DSU_NULL_32 == LinkLengthStart) || (DSU_NULL_32 == LinkLengthEnd) || (LocStart.Off > LinkLengthStart) || (LocEnd.Off > LinkLengthEnd))
		{
			/*Link偏移量防护*/
			*pLogicSgmtNum = 0U;
			RtnNo = 0U;
		}
		else
		{
			/*整体思路是先找到起点和终点所在的逻辑区段，然后从起点所在逻辑区段开始，向指定方向查相邻逻辑区段*/
			/*直到查到终点逻辑区段查找成功，或查到线路尽头查询失败*/

			/*终点方向特殊处理成起点方向，否则判断在哪个逻辑区段内的dsuCheckLocInArea函数会报错*/
			tempEndLoc.Lnk = LocEnd.Lnk;
			tempEndLoc.Off = LocEnd.Off;
			tempEndLoc.Dir = LocStart.Dir;

			/*查询起点所在逻辑区段*/
			pLinkStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[LocStart.Lnk];
			for (k = 0U; k < pLinkStru->LogicSgmtNum; k++)
			{
				/*遍历起点link对应的所有逻辑区段，找起点是否在逻辑区段两个端点范围内*/
				pLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pLinkStru->LogicSgmtIdBuff[k]];

				/*获取逻辑区段端点*/
				FunRtn = dsuGetLogicSgmtEndLocExp(pDsuEmapStru, pLogicSgmtStru->wId, EMAP_CONVER_DIR, &LogicStartLoc);
				FunRtn &= dsuGetLogicSgmtEndLocExp(pDsuEmapStru, pLogicSgmtStru->wId, EMAP_SAME_DIR, &LogicEndLoc);
				if (1U == FunRtn)
				{
					/*特殊处理一下端点方向，适配dsuCheckLocInArea查询函数*/
					LogicStartLoc.Dir = LocStart.Dir;
					LogicEndLoc.Dir = LocStart.Dir;

					/*查询起点是否在当前逻辑区段内*/
					FunRtn = dsuCheckLocInAreaExp(pDsuEmapStru, &LogicStartLoc, &LogicEndLoc, PointNum, pPointInfo, &LocStart, &LocInArea1);
					FunRtn &= dsuCheckLocInAreaExp(pDsuEmapStru, &LogicEndLoc, &LogicStartLoc, PointNum, pPointInfo, &LocStart, &LocInArea2);
					if (1U == FunRtn)
					{
						if ((1U == LocInArea1) || (1U == LocInArea2))
						{
							/*起点在该逻辑区段范围内，跳出循环*/
							StartLogicSgmtId = pLogicSgmtStru->wId;
						}
						else
						{
							/*do nothing*/
						}
					}
					else
					{
						/*do nothing*/
					}
				}
				else
				{
					/*do nothing*/
				}

				if (0xFFFFU != StartLogicSgmtId)
				{
					/*逻辑区段找到了，可以跳出循环*/
					break;
				}
				else
				{
					/*没找到，继续遍历*/
				}
			}

			/*查询终点所在逻辑区段*/
			pLinkStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[LocEnd.Lnk];
			for (k = 0U; k < pLinkStru->LogicSgmtNum; k++)
			{
				/*遍历终点link对应的所有逻辑区段，找终点是否在逻辑区段两个端点范围内*/
				pLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pLinkStru->LogicSgmtIdBuff[k]];

				/*获取逻辑区段端点*/
				FunRtn = dsuGetLogicSgmtEndLocExp(pDsuEmapStru, pLogicSgmtStru->wId, EMAP_CONVER_DIR, &LogicStartLoc);
				FunRtn &= dsuGetLogicSgmtEndLocExp(pDsuEmapStru, pLogicSgmtStru->wId, EMAP_SAME_DIR, &LogicEndLoc);
				if (1U == FunRtn)
				{
					/*特殊处理一下端点方向，适配dsuCheckLocInArea查询函数*/
					LogicStartLoc.Dir = LocStart.Dir;
					LogicEndLoc.Dir = LocStart.Dir;

					/*查询终点是否在当前逻辑区段内*/
					FunRtn = dsuCheckLocInAreaExp(pDsuEmapStru, &LogicStartLoc, &LogicEndLoc, PointNum, pPointInfo, &tempEndLoc, &LocInArea1);
					FunRtn &= dsuCheckLocInAreaExp(pDsuEmapStru, &LogicEndLoc, &LogicStartLoc, PointNum, pPointInfo, &tempEndLoc, &LocInArea2);
					if (1U == FunRtn)
					{
						if ((1U == LocInArea1) || (1U == LocInArea2))
						{
							/*起点在该逻辑区段范围内，跳出循环*/
							EndLogicSgmtId = pLogicSgmtStru->wId;
						}
						else
						{
							/*do nothing*/
						}
					}
					else
					{
						/*do nothing*/
					}
				}
				else
				{
					/*do nothing*/
				}

				if (0xFFFFU != EndLogicSgmtId)
				{
					/*逻辑区段找到了，跳出循环*/
					break;
				}
				else
				{
					/*没找到，继续遍历*/
				}
			}

			if ((0U != StartLogicSgmtId) && (0U != EndLogicSgmtId) && (0xFFFFU != StartLogicSgmtId) && (0xFFFFU != EndLogicSgmtId))
			{
				/*起点和终点所在逻辑区段都找到了*/
				/*先将起点添加至结果数组*/
				ResultLogicSgmtNum = 1U;
				ResultLogicSgmtId[0] = StartLogicSgmtId;
				if (StartLogicSgmtId == EndLogicSgmtId)
				{
					/*起点终点所在逻辑区段相同，只返回1个逻辑区段，无需继续查找*/
					RtnNo = 1U;
				}
				else
				{
					SearchDir = LocStart.Dir;
					/*查找的逻辑区段数量必不超过所有逻辑区段数量，这里用FOR循环代替WHILE(1)防止陷入死循环查找*/
					for (k = 0U; k < pDsuEmapStru->dsuDataLenStru->wLogicSgmtLen; k++)
					{
						/*查找起点所在逻辑区段的相邻逻辑区段*/
						FunRtn = dsuGetAdjacentLogicSgmtExp(pDsuEmapStru, ResultLogicSgmtId[ResultLogicSgmtNum - 1U], SearchDir, PointNum, pPointInfo, &AdjLogicSgmtId);
						if ((1U == FunRtn) || (2U == FunRtn))
						{
							if ((0U != AdjLogicSgmtId) && (0xFFFFU != AdjLogicSgmtId) && (OBJECT_NUM_MAX > ResultLogicSgmtNum))
							{
								/*存在相邻逻辑区段，添加至结果数组*/
								ResultLogicSgmtId[ResultLogicSgmtNum] = AdjLogicSgmtId;
								ResultLogicSgmtNum += 1U;

								if (2U == FunRtn)
								{
									SearchDir = ~SearchDir;
								}
								
								if (AdjLogicSgmtId == EndLogicSgmtId)
								{
									/*找到终点逻辑区段了，查找成功*/
									RtnNo = 1U;
								}
								else
								{
									/*继续查找*/
								}
							}
							else
							{
								/*找到线路尽头了，查找失败*/
								tempRtnVal = 0U;
							}
						}
						else
						{
							/*查找失败*/
							tempRtnVal = 0U;
						}

						if ((1U == RtnNo) || (1U != tempRtnVal))
						{
							/*查找成功或查找失败，跳出循环*/
							break;
						}
						else
						{
							/*继续查找*/
						}
					}
				}
			}
			else
			{
				/*没找到起点所在逻辑区段，返回失败*/
				tempRtnVal = 0U;
			}
		}

		/*输出*/
		if ((1U == tempRtnVal) && (1U == RtnNo))
		{
			*pLogicSgmtNum = ResultLogicSgmtNum;
			CommonMemCpy(pLogicSgmtId, ResultLogicSgmtNum * sizeof(UINT16), ResultLogicSgmtId, ResultLogicSgmtNum * sizeof(UINT16));
		}
		else
		{
			*pLogicSgmtNum = 0U;
			RtnNo = 0U;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定一个位置，查询这个位置所属的逻辑区段id
*输入参数:	pLocStru			位置结构体
*			PointNum			道岔数量
*			pPointInfo			道岔信息
*输出参数:	pLogicSgmtId		逻辑区段id
*返回值:	失败:0
*			成功:1
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetLogicSgmtIdByLocExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const LOD_STRU *pLocStru, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointInfo, OUT UINT16 *pLogicSgmtId)
{
	UINT8 RtnNo = 0u;
	UINT8 CallFuncRtn = 0u;
	DSU_LOGIC_SGMT_STRU* pCurrLogicSgmtStru = NULL;						/*逻辑区段信息结构体指针*/
	DSU_LINK_STRU* pCurrLinkStru = NULL;								/*link信息*/
	UINT32 linkLen = 0u;
	UINT16 i = 0u;
	UINT16 FindLogicSgmt = 0u;
	LOD_STRU LocAStru = { 0u };
	LOD_STRU LocBStru = { 0u };
	INT32 Disp = 0;

	if ((NULL == pDsuEmapStru) || (NULL == pLogicSgmtId) ||(NULL ==  pPointInfo))
	{
		RtnNo = 0U;
	}
	else
	{
		/*检查数据有效性*/
		if ((0u == pLocStru->Lnk) || (0xFFFFU == pLocStru->Lnk) || ((EMAP_SAME_DIR != pLocStru->Dir) && (EMAP_CONVER_DIR != pLocStru->Dir)))
		{
			RtnNo = 0U;
		}
		else
		{
			/*检查位置长度的正确性*/
			linkLen = LinkLengthExp(pDsuEmapStru, pLocStru->Lnk);
			if (pLocStru->Off > linkLen)
			{
				/*长度超长*/
				RtnNo = 0U;
			}
			else
			{
				/*获取link信息*/
				pCurrLinkStru = pDsuEmapStru->dsuStaticHeadStru->pLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuLinkIndex[pLocStru->Lnk];
				if (NULL != pCurrLinkStru)
				{
					/*遍历包含的逻辑区段*/
					for (i = 0u; i < pCurrLinkStru->LogicSgmtNum; i++)
					{
						/*获取逻辑区段信息*/
						pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[pCurrLinkStru->LogicSgmtIdBuff[i]];
						if (NULL != pCurrLogicSgmtStru)
						{
							/*先检查逻辑区段的起点终点是否在同一link*/
							if (pCurrLogicSgmtStru->wOrgnLkId == pCurrLogicSgmtStru->wTmnlLkId)
							{
								/*起点和终点都在同一个linkid，说明逻辑区段没有包含道岔*/
								if (pCurrLogicSgmtStru->wOrgnLkId == pLocStru->Lnk)
								{
									/*逻辑区段link起点终点一致*/
									if ((pCurrLogicSgmtStru->dwOrgnLkOfst <= pLocStru->Off) && (pLocStru->Off <= pCurrLogicSgmtStru->dwTmnlLkOfst))
									{
										/*找到了id*/
										FindLogicSgmt = pCurrLogicSgmtStru->wId;
										break;
									}
									else
									{
										/*继续查找*/
									}
								}
								else
								{
									/*do nothing*/
								}
							}
							else
							{
								/*起点和终点不在同一link，说明中间含有道岔，根据实际道岔位置匹配*/
								LocAStru.Lnk = pCurrLogicSgmtStru->wOrgnLkId;
								LocAStru.Dir = pLocStru->Dir;
								LocBStru.Lnk = pCurrLogicSgmtStru->wTmnlLkId;
								LocBStru.Dir = pLocStru->Dir;
								if (EMAP_SAME_DIR == pLocStru->Dir)
								{
									LocAStru.Off = LinkLengthExp(pDsuEmapStru, LocAStru.Lnk);
									LocBStru.Off = LinkLengthExp(pDsuEmapStru, LocBStru.Lnk);
								}
								else
								{
									LocAStru.Off = 0u;
									LocBStru.Off = 0u;
								}
								CallFuncRtn = dsuDispCalcuByLOCExp(pDsuEmapStru, &LocAStru, &LocBStru, PointNum, pPointInfo, &Disp);
								if (1U == CallFuncRtn)
								{
									/*查找到了正确的逻辑区段*/
									if ((pCurrLogicSgmtStru->wOrgnLkId == pLocStru->Lnk) && (pCurrLogicSgmtStru->wTmnlLkId != pLocStru->Lnk))
									{
										/*起点一致终点不一致，只需要检查起点link*/
										if (pCurrLogicSgmtStru->dwOrgnLkOfst <= pLocStru->Off)
										{
											/*找到了id*/
											FindLogicSgmt = pCurrLogicSgmtStru->wId;
											break;
										}
										else
										{
											/*继续查找*/
										}
									}
									else if ((pCurrLogicSgmtStru->wOrgnLkId != pLocStru->Lnk) && (pCurrLogicSgmtStru->wTmnlLkId == pLocStru->Lnk))
									{
										/*起点不一致，终点一致，只需要检查终点*/
										if (pLocStru->Off <= pCurrLogicSgmtStru->dwTmnlLkOfst)
										{
											/*找到了id*/
											FindLogicSgmt = pCurrLogicSgmtStru->wId;
											break;
										}
										else
										{
											/*继续查找*/
										}
									}
									else
									{
										/*起点终点link均不一致，数据结构出现了问题*/
										RtnNo = 0U;
										break;
									}
								}
								else
								{
									/*查不到，需要继续查找*/
								}

							}
						}
						else
						{
							/*指针为空*/
							RtnNo = 0U;
						}
					}
				}
				else
				{
					/*指针为空*/
					RtnNo = 0U;
				}
			}

			/*检查返回值有效性*/
			if ((0u != FindLogicSgmt) && (0xFFFFu != FindLogicSgmt))
			{
				/*数据有效*/
				*pLogicSgmtId = FindLogicSgmt;
				RtnNo = 1U;
			}
			else
			{
				/*数据无效*/
				RtnNo = 0U;
			}
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定一个逻辑区段id，查询这个逻辑区段属于那些进路
*输入参数:	LogicSgmtId			逻辑区段id
*输出参数:	pRouteNum			所属进路数量
*			pRouteIdBuff		所属进路id列表
*返回值:	失败:0
*			成功:1
*使用注意：		如果给定的逻辑区段id是某个进路A的保护区段，则进路A不在列表内。即保护区段不计入进路。
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetRouteIdByLogicSgmtExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN UINT16 LogicSgmtId, IN UINT32 sizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff)
{
	UINT8 RtnNo = 0u;
	UINT8 CallFuncRtn = 0u;
	DSU_LOGIC_SGMT_STRU *pCurrLogicSgmtStru = NULL;						/*逻辑区段信息结构体指针*/

	if ((NULL == pDsuEmapStru) || (NULL == pRouteNum) || (NULL == pRouteIdBuff) || (0u == LogicSgmtId))
	{
		RtnNo = 0U;
	}
	else
	{
		/*获取给定逻辑区段的信息*/
		pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId];

		if (NULL != pCurrLogicSgmtStru)
		{
			RtnNo = 1u;

			if ((0xFFu == pCurrLogicSgmtStru->RouteNum) || (0u == pCurrLogicSgmtStru->RouteNum))
			{
				/*本逻辑区段不属于任何进路*/
				*pRouteNum = 0u;
			}
			else
			{
				CallFuncRtn = CommonMemCpy(&pRouteIdBuff[0], sizeofBuff, &pCurrLogicSgmtStru->RouteIdBuff[0], sizeof(UINT16) * pCurrLogicSgmtStru->RouteNum);
				if (1u == CallFuncRtn)
				{
					/*复制成功*/
					*pRouteNum = pCurrLogicSgmtStru->RouteNum;
				}
				else
				{
					/*复制失败*/
					RtnNo = 0u;
				}
			}
		}
		else
		{
			/*指针为空*/
			RtnNo = 0u;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定一个位置，查询这个位置属于那些进路
*输入参数:	pLocStru			位置
*			PointNum			道岔数量
*			pPointInfo			道岔信息
*			sizeofBuff			数组的大小
*输出参数:	pRouteNum			所属进路数量
*			pRouteIdBuff		所属进路id列表
*返回值:	失败:0
*			成功:1
*使用注意：		如果给定的逻辑区段id是某个进路A的保护区段，则进路A不在列表内。即保护区段不计入进路。
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetRouteIdBuffByLocExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const LOD_STRU* pLocStru,IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointInfo, IN UINT32 sizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff)
{
	UINT8 RtnNo = 0u;
	UINT8 CallFuncRtn = 0u;
	UINT16 LogicSgmtId = 0U;											/*逻辑区段id*/
	DSU_LOGIC_SGMT_STRU* pCurrLogicSgmtStru = NULL;						/*逻辑区段信息结构体指针*/
	DSU_ROUTE_STRU* pCurrRouteStru = NULL;								/*进路信息结构体指针*/

	UINT8 RouteNum = 0u;
	UINT16 RouteIdBuff[128] = { 0u };

	LOD_STRU routeStartLocStru = { 0u };								/*进路起点位置*/

	UINT16 i = 0u;

	if ((NULL == pDsuEmapStru) || (NULL == pLocStru) || (NULL == pPointInfo) || (NULL == pRouteNum) || (NULL == pRouteIdBuff))
	{
		RtnNo = 0U;
	}
	else
	{
		/*查询指定位置所在的逻辑区段id*/
		CallFuncRtn = dsuGetLogicSgmtIdByLocExp(pDsuEmapStru, pLocStru, PointNum, pPointInfo, &LogicSgmtId);
		if (1u == CallFuncRtn)
		{
			/*获取给定逻辑区段的信息*/
			pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId];
			 
			if (NULL != pCurrLogicSgmtStru)
			{
				RtnNo = 1u;

				if ((0xFFu == pCurrLogicSgmtStru->RouteNum) || (0u == pCurrLogicSgmtStru->RouteNum))
				{
					/*本逻辑区段不属于任何进路*/
					*pRouteNum = 0u;
				}
				else
				{
					for (i = 0; i < pCurrLogicSgmtStru->RouteNum; i++)
					{
						pCurrRouteStru = pDsuEmapStru->dsuStaticHeadStru->pRouteStru + pDsuEmapStru->dsuEmapIndexStru->dsuRouteIndex[pCurrLogicSgmtStru->RouteIdBuff[i]];
						if (NULL != pCurrRouteStru)
						{
							/*获取进路起点位置信息*/
							routeStartLocStru.Lnk = pCurrRouteStru->wOrgnLkId;
							if (EMAP_SAME_DIR == pCurrRouteStru->wStartLocDir)
							{
								routeStartLocStru.Off = 0u;
								routeStartLocStru.Dir = EMAP_SAME_DIR;
							}
							else
							{
								routeStartLocStru.Off = LinkLengthExp(pDsuEmapStru, pCurrRouteStru->wOrgnLkId);
								routeStartLocStru.Dir = EMAP_CONVER_DIR;
							}
							
							/*这里使用从进路起点到传入位置之间比较，如果计算出的方向和进路方向一致，则认为找到了进路*/
							CallFuncRtn = dquIsDirSameExp(pDsuEmapStru, &routeStartLocStru, pLocStru, PointNum, pPointInfo);
							if (0x55u == CallFuncRtn)
							{
								RouteIdBuff[RouteNum] = pCurrRouteStru->wId;
								RouteNum++;
							}
							else
							{
								/*继续查找*/
							}
						}
						else
						{
							/*指针为空*/
							break;
							RtnNo = 0u;
						}
					}

					CallFuncRtn = CommonMemCpy(&pRouteIdBuff[0], sizeofBuff, &RouteIdBuff[0], sizeof(UINT16) * RouteNum);
					if (1u == CallFuncRtn)
					{
						/*复制成功*/
						*pRouteNum = RouteNum;
					}
					else
					{
						/*复制失败*/
						RtnNo = 0u;
					}
				}
			}
			else
			{
				/*指针为空*/
				RtnNo = 0u;
			}
		}
		else
		{
			/*查询失败*/
			RtnNo = 0u;
		}
	}

	return RtnNo;
}


/************************************************
*函数功能:	给定一个逻辑区段id，查询这个逻辑区段属于哪些CBTC接近区段
*输入参数:	LogicSgmtId			逻辑区段id
*输出参数:	pRouteNum			所属CBTC接近区段索引id数量
*			pRouteIdBuff		所属CBTC接近区段索引id列表
*返回值:	失败:0
*			成功:1
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetCbtcAccessSgmtIdByLogicSgmtExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT32 sizeofBuff, OUT UINT8* pCbtcAccessSgmtNum, OUT UINT16* pCbtcAccessSgmtIdBuff)
{
	UINT8 RtnNo = 0u;
	UINT8 CallFuncRtn = 0u;
	DSU_LOGIC_SGMT_STRU* pCurrLogicSgmtStru = NULL;						/*逻辑区段信息结构体指针*/

	if ((NULL == pDsuEmapStru) || (NULL == pCbtcAccessSgmtNum) || (NULL == pCbtcAccessSgmtIdBuff) || (0u == LogicSgmtId))
	{
		RtnNo = 0U;
	}
	else
	{
		/*获取给定逻辑区段的信息*/
		pCurrLogicSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pLogicSgmtStru + pDsuEmapStru->dsuEmapIndexStru->dsuLogicSgmtIndex[LogicSgmtId];

		if (NULL != pCurrLogicSgmtStru)
		{
			RtnNo = 1u;

			if ((0xFFu == pCurrLogicSgmtStru->AccessSgmtNum) || (0u == pCurrLogicSgmtStru->AccessSgmtNum))
			{
				/*本逻辑区段不属于任何接近区段*/
				*pCbtcAccessSgmtNum = 0u;
			}
			else
			{
				CallFuncRtn = CommonMemCpy(&pCbtcAccessSgmtIdBuff[0], sizeofBuff, &pCurrLogicSgmtStru->AccessSgmtIdBuff[0], sizeof(UINT16) * pCurrLogicSgmtStru->AccessSgmtNum);
				if (1u == CallFuncRtn)
				{
					/*复制成功*/
					*pCbtcAccessSgmtNum = pCurrLogicSgmtStru->AccessSgmtNum;
				}
				else
				{
					/*复制失败*/
					RtnNo = 0u;
				}
			}
		}
		else
		{
			/*指针为空*/
			RtnNo = 0u;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定一个进路列表，返回逻辑区段最多的进路
*输入参数:	RouteNum			进路数量
*			pRouteIdBuff		进路列表
*输出参数:	pRouteId			进路id
*返回值:	失败:0
*			成功:1
* 使用说明：若存在逻辑区段数量一致的情况，则返回列表中靠前的进路id
* 备注：	add by kuan.he 20230516
************************************************/
UINT8 dsuGetLongestRouteIdByRouteIdBuffExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT8 RouteNum, IN const UINT16* pRouteIdBuff, OUT UINT16* pRouteId)
{
	UINT8 RtnNo = 0u;
	UINT8 CallFuncRtn = 0u;

	DSU_ROUTE_STRU* pCurrRouteStru = NULL;								/*进路信息结构体指针*/

	UINT16 i = 0u;
	UINT16 LongestLogicSgmt = 0u;
	UINT16 LongestRouteId = 0u;

	if ((NULL == pDsuEmapStru) || (NULL == pRouteIdBuff) || (NULL == pRouteId))
	{
		RtnNo = 0U;
	}
	else
	{
		/*遍历列表*/
		for (i = 0; i < RouteNum; i++)
		{
			pCurrRouteStru = pDsuEmapStru->dsuStaticHeadStru->pRouteStru + pDsuEmapStru->dsuEmapIndexStru->dsuRouteIndex[pRouteIdBuff[i]];
			if (NULL != pCurrRouteStru)
			{
				/*比较长度*/
				if (pCurrRouteStru->wIncludeLogicSgmtNum > LongestLogicSgmt)
				{
					LongestLogicSgmt = pCurrRouteStru->wIncludeLogicSgmtNum;
					LongestRouteId = pCurrRouteStru->wId;
				}
				else
				{
					/*继续比较*/
				}
			}
			else
			{
				/*指针为空*/
				RtnNo = 0u;
				break;
			}
		}

		/*输出*/
		if ((0u != LongestRouteId) && (0xFFFFu != LongestRouteId))
		{
			*pRouteId = LongestRouteId;
			RtnNo = 1u;
		}
		else
		{
			/*id 不正确*/
			RtnNo = 0u;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定一个进路列表，通过道岔状态匹配出唯一一条进路
*输入参数:	RouteNum			进路数量
*			pRouteIdBuff		进路列表
*			PointNum			道岔数量
*			pPointStru			道岔列表
*输出参数:	pRouteId			进路id
*返回值:	失败:0
*			成功:1
* 使用说明：
*			1. 当列表中有多条进路，且某进路存在不含有道岔的情况时，该进路不会计入匹配列表
*			2. 当列表中有多条进路，且全部进路都不含有道岔的情况时，会返回一条逻辑区段最多的进路
*			3. 当筛选出的结果存在多条进路，则在这些进路中返回一条逻辑区段最多的进路
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetRouteIdByRouteIdBuffExp(IN const DSU_EMAP_STRU* pDsuEmapStru,  IN const UINT8 RouteNum, IN const UINT16* pRouteIdBuff, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointStru, OUT UINT16 *pRouteId)
 {
	UINT8 RtnNo = 0u;
	UINT8 CallFuncRtn = 0u;
	DSU_ROUTE_STRU* pCurrRouteStru = NULL;								/*进路信息结构体指针*/

	UINT16 i = 0u, j = 0u, k = 0u;
	UINT8 FindRouteNum = 0u;											/*找到了的进路数量*/
	UINT16 FindRouteIdBuff[128] = { 0u };								/*找到了的进路id列表*/
	UINT16 FindSwitchNum = 0U;											/*匹配到了道岔数量*/
	UINT8  NotHasPointRouteNum = 0;										/*有多少条进路不含道岔*/
	UINT16 LongestRouteId = 0u;											/*逻辑区段最多的进路id*/

	if ((NULL == pDsuEmapStru) || (NULL == pRouteIdBuff) || (NULL == pPointStru) || (NULL == pRouteId))
	{
		RtnNo = 0U;
	}
	else
	{
		/*遍历列表*/
		for ( i = 0; i < RouteNum; i++)
		{
			FindSwitchNum = 0u;

			pCurrRouteStru = pDsuEmapStru->dsuStaticHeadStru->pRouteStru + pDsuEmapStru->dsuEmapIndexStru->dsuRouteIndex[pRouteIdBuff[i]];
			if (NULL != pCurrRouteStru)
			{
				/*检查进路是否含有道岔*/
				if ((0 == pCurrRouteStru->wIncludePointNum) && (1u == RouteNum))
				{
					/*唯一的进路不含道岔，直接终止，返回成功*/
					FindRouteIdBuff[FindRouteNum] = pCurrRouteStru->wId;
					FindRouteNum = 1u;
					break;
				}
				else
				{
					/*遍历进路的道岔列表*/
					for (j = 0; j < pCurrRouteStru->wIncludePointNum; j++)
					{
						/*遍历给定的道岔信息*/
						for ( k = 0; k < PointNum; k++)
						{
							if (pCurrRouteStru->wPointId[j] == pPointStru[k].PointId)
							{
								/*找到了道岔，开始匹配状态*/
								if (pCurrRouteStru->wPointStatus[j] == pPointStru[k].PointStatus)
								{
									FindSwitchNum += 1u;
								}
								else
								{
									/*道岔状态不正确*/
								}

								break;
							}
							else
							{
								/*继续匹配*/
							}
						}

						if ((j+1) != FindSwitchNum)
						{
							/*本次没有找到道岔，说明需要的道岔不在状态，不需要找了*/
							break;
						}
						else
						{
							/*继续匹配*/
						}
					}

					if ((FindSwitchNum == pCurrRouteStru->wIncludePointNum) && (0u != FindSwitchNum))
					{
						/*匹配到的道岔状态和需要的一致，说明这条进路ok*/
						FindRouteIdBuff[FindRouteNum] = pCurrRouteStru->wId;
						FindRouteNum++;
					}
					else if(0u == pCurrRouteStru->wIncludePointNum)
					{
						/*这条进路不含道岔*/
						NotHasPointRouteNum++;
					}
					else
					{
						/*没有匹配到*/
					}
				}
			}
			else
			{
				/*指针为空*/
				RtnNo = 0u;
				break;
			}
		}

		/*检查匹配到的数量*/
		if (1u == FindRouteNum)
		{
			*pRouteId = FindRouteIdBuff[0];
			RtnNo = 1u;
		}
		else if((NotHasPointRouteNum == RouteNum) || (2u <= FindRouteNum))
		{
			if (2u <= FindRouteNum)
			{
				/*筛选出来的进路列表含有多个进路，在筛选出来的列表中选一条*/
				CallFuncRtn = dsuGetLongestRouteIdByRouteIdBuffExp(pDsuEmapStru, FindRouteNum, FindRouteIdBuff, &LongestRouteId);
			}
			else
			{
				/*所有的进路都不含道岔,需要选择一条最长的进路*/
				CallFuncRtn = dsuGetLongestRouteIdByRouteIdBuffExp(pDsuEmapStru, RouteNum, pRouteIdBuff, &LongestRouteId);
			}
			
			if (1U == CallFuncRtn)
			{
				*pRouteId = LongestRouteId;
				RtnNo = 1u;
			}
			else
			{
				/*无法获取最长的进路*/
				RtnNo = 0u;
			}
		}
		else
		{
			/*不存在进路*/
			RtnNo = 0u;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定一个接近区段索引id，查询这个接近区段属于哪些进路
*输入参数:	AccessSgmtId		接近区段索引id
*			SizeofBuff			进路数组大小
*输出参数:	pRouteNum			进路数量
*			pRouteIdBuff		进路数组
*返回值:	失败:0
*			成功:1
* 备注：	add by kuan.he 20230516
*	修复进路数量未正确防护的问题。 kuan.he 20230604
************************************************/
UINT8 dsuGetRouteIdByAccessSgmtIdExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT16 AccessSgmtId, IN const UINT32 SizeofBuff, OUT UINT8 *pRouteNum, OUT UINT16 *pRouteIdBuff)
{
	UINT8 RtnNo = 0u;
	UINT8 CallFuncRtn = 0u;
	DSU_CBTC_ACCESS_LINK_STRU* pCbtcAccessSgmtStru = NULL;

	if ((NULL == pDsuEmapStru) || (0u == AccessSgmtId) || (0xFFFFu == AccessSgmtId) || (NULL == pRouteNum) || (NULL == pRouteIdBuff))
	{
		RtnNo = 0U;
	}
	else
	{
		/*获取给定逻辑区段的信息*/
		pCbtcAccessSgmtStru = pDsuEmapStru->dsuStaticHeadStru->pCBTCAccessLinkStru + pDsuEmapStru->dsuEmapIndexStru->dsuCBTCAccessLinkIndex[AccessSgmtId];

		if (NULL != pCbtcAccessSgmtStru)
		{
			RtnNo = 1u;

			if ((0u != pCbtcAccessSgmtStru->BelongRouteNum) && (0xFFFFu != pCbtcAccessSgmtStru->BelongRouteNum))
			{
				CallFuncRtn = CommonMemCpy(&pRouteIdBuff[0], SizeofBuff, &pCbtcAccessSgmtStru->BelongRouteIdBuff[0], sizeof(UINT16) * pCbtcAccessSgmtStru->BelongRouteNum);
				if (1u == CallFuncRtn)
				{
					/*复制成功*/
					*pRouteNum = (UINT8)pCbtcAccessSgmtStru->BelongRouteNum;
					RtnNo = 1u;
				}
				else
				{
					/*复制失败*/
					RtnNo = 0u;
				}
			}
			else
			{
				/*指定接近区段id不属于任何进路*/
				RtnNo = 0u;
			}
		}
		else
		{
			/*指针为空*/
			RtnNo = 0u;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	给定一组接近区段序列，查询这个接近区段所属的所有进路
*输入参数:	AccessSgmtNum		接近区段索引数量
*			AccessSgmtBuff		接近区段索引数组
*			SizeofBuff			进路数组大小
*输出参数:	pRouteNum			进路数量
*			pRouteIdBuff		进路数组
*返回值:	失败:0
*			成功:1
* 使用注意：返回的进路列表已完成去重。
* 备注：	add by kuan.he 20230516
************************************************/
UINT8 dsuGetRouteIdBuffByAccessSgmtIdBuffExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT16 AccessSgmtNum, IN const UINT16* pAccessSgmtBuff,IN const UINT32 SizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff)
{
	UINT8 RtnNo = 1u;
	UINT8 CallFuncRtn = 0u;

	UINT8 CurrRouteNum = 0u;
	UINT16 CurrRouteIdBuff[128] = { 0u };

	UINT8 TotalRouteNum = 0u;
	UINT16 TotalRouteIdBuff[1024] = { 0u };

	UINT16 i = 0u, j = 0u, k = 0u;

	if ((NULL == pDsuEmapStru) || (NULL == pAccessSgmtBuff) || (NULL == pRouteNum) || (0u == pRouteIdBuff))
	{
		RtnNo = 0U;
	}
	else
	{
		/*遍历id数组*/
		for (i = 0; i < AccessSgmtNum; i++)
		{
			CallFuncRtn = dsuGetRouteIdByAccessSgmtIdExp(pDsuEmapStru, pAccessSgmtBuff[i], sizeof(CurrRouteIdBuff), &CurrRouteNum, &CurrRouteIdBuff[0]);
			if (1u == CallFuncRtn)
			{
				if (0u != CurrRouteNum)
				{
					CallFuncRtn = CommonMemCpy(&TotalRouteIdBuff[TotalRouteNum], (sizeof(TotalRouteIdBuff) - (sizeof(UINT16) * TotalRouteNum)), &CurrRouteIdBuff[0], CurrRouteNum * sizeof(UINT16));
					if (1u == CallFuncRtn)
					{
						TotalRouteNum += CurrRouteNum;
					}
					else
					{
						/*复制失败*/
						RtnNo &= 0u;
						break;
					}
				}
				else
				{
					/*do nothing*/
				}
				
			}
		}

		if (1u == RtnNo)
		{
			/*去除重复项，遍历数组中的每一个数据*/
			for (i = 0; i < TotalRouteNum; i++)
			{
				for (j = i + 1; j < TotalRouteNum; j++)
				{
					if (TotalRouteIdBuff[i] == TotalRouteIdBuff[j])
					{
						/*使用for循环向前移一个位置，不使用memcpy*/
						for (k = j; k < TotalRouteNum - 1; k++)
						{
							TotalRouteIdBuff[k] = TotalRouteIdBuff[k + 1];
						}

						j--;
						TotalRouteNum--;
					}
				}
			}

			/*对外输出数据*/
			CallFuncRtn = CommonMemCpy(&pRouteIdBuff[0], SizeofBuff, &TotalRouteIdBuff[0], TotalRouteNum * sizeof(UINT16));
			if (1u == CallFuncRtn)
			{
				*pRouteNum = TotalRouteNum;
			}
			else
			{
				/*复制失败*/
				RtnNo &= 0u;
			}
		}
		else
		{
			/*do nothing*/
		}
	}

	return RtnNo;
}


/************************************************
*函数功能:	给定一个进路id，查询这个进路的起点位置
*输入参数:	RouteId				进路Id
*输出参数:	pRouteStartLoc		进路起点位置
*返回值:	失败:0
*			成功:1
* 使用注意：进路起点位置方向是指进路起点指向终点的方向，非信号机防护方向
* 备注：	add by kuan.he 20230731
************************************************/
UINT8 dsuGetRouteStartLocByRouteIdExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN UINT16 RouteId, OUT LOD_STRU *pRouteStartLoc)
{
	UINT8 RtnNo = 1u;
	
	DSU_ROUTE_STRU *pRouteStru = NULL;							/*进路信息结构体指针*/

	if ((NULL == pDsuEmapStru) || (NULL == pRouteStartLoc))
	{
		RtnNo = 0U;
	}
	else
	{
		pRouteStru = pDsuEmapStru->dsuStaticHeadStru->pRouteStru + pDsuEmapStru->dsuEmapIndexStru->dsuRouteIndex[RouteId];
		if (NULL != pRouteStru)
		{
			pRouteStartLoc->Lnk = pRouteStru->wOrgnLkId;
			pRouteStartLoc->Dir = (UINT8)pRouteStru->wStartLocDir;
			if (EMAP_SAME_DIR == pRouteStru->wStartLocDir)
			{
				pRouteStartLoc->Off = 0U;
			}
			else
			{
				pRouteStartLoc->Off = LinkLengthExp(pDsuEmapStru, pRouteStru->wOrgnLkId);
			}

			RtnNo = 1U;
		}
		else
		{
			/*进路信息获取错误*/
			RtnNo = 0U;
		}
	}

	return RtnNo;
}


/************************************************
*函数功能:	给定一个进路id，查询这个进路的终点位置
*输入参数:	RouteId				进路Id
*输出参数:	pRouteStartLoc		进路起点位置
*返回值:	失败:0
*			成功:1
* 使用注意：进路终点方向是指进路起点指向终点的方向，非信号机防护方向
* 备注：	add by kuan.he 20230731
************************************************/
UINT8 dsuGetRouteEndLocByRouteIdExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN UINT16 RouteId, OUT LOD_STRU* pRouteEndLoc)
{
	UINT8 RtnNo = 1u;

	DSU_ROUTE_STRU* pRouteStru = NULL;							/*进路信息结构体指针*/

	if ((NULL == pDsuEmapStru) || (NULL == pRouteEndLoc))
	{
		RtnNo = 0U;
	}
	else
	{
		pRouteStru = pDsuEmapStru->dsuStaticHeadStru->pRouteStru + pDsuEmapStru->dsuEmapIndexStru->dsuRouteIndex[RouteId];
		if (NULL != pRouteStru)
		{
			pRouteEndLoc->Lnk = pRouteStru->wTmnlLkId;
			pRouteEndLoc->Dir = (UINT8)pRouteStru->wEndLocDir;
			if (EMAP_SAME_DIR == pRouteStru->wEndLocDir)
			{
				pRouteEndLoc->Off = 0U;
			}
			else
			{
				pRouteEndLoc->Off = LinkLengthExp(pDsuEmapStru, pRouteStru->wTmnlLkId);
			}

			RtnNo = 1U;
		}
		else
		{
			/*进路信息获取错误*/
			RtnNo = 0U;
		}
	}

	return RtnNo;
}

/************************************************
*函数功能:	指定A点位置和方向、获取B点基于A的方向(考虑了方向变化点)
*输入参数:	UINT16 wLinkA, UINT32 dwLinkAOff, UINT8 LinkDirA,A点位置和方向
*			UINT16 wLinkB, UINT32 dwLinkBOff,B点位置
*			PointNum、pPointStru-道岔信息
*输出参数:	无
*返回值:	0:查询失败
*			EMAP_SAME_DIR/EMAP_CONVER_DIR:逻辑方向
*备注：		cjq 20230801
************************************************/
UINT8 dsuGetPosBDirByPosAExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT16 wLinkA, IN const UINT32 dwLinkAOff, IN const UINT8 LinkDirA, 
	IN const UINT16 wLinkB, IN const UINT32 dwLinkBOff, IN const UCHAR PointNum, IN const DQU_POINT_STATUS_STRU *pPointStru)
{
	UINT8 retDir = 0U;
	LOD_STRU stuTailLoc = { 0 };
	LOD_STRU stuHeadLoc = { 0 };
	UINT8 IsDirSame = 0u;

	if ((NULL == pPointStru) || (NULL == pDsuEmapStru))
	{
		/*nothing*/
	}
	else
	{
		stuTailLoc.Lnk = wLinkA;
		stuTailLoc.Off = dwLinkAOff;
		stuTailLoc.Dir = LinkDirA;

		stuHeadLoc.Lnk = wLinkB;
		stuHeadLoc.Off = dwLinkBOff;
		stuHeadLoc.Dir = LinkDirA; 	/*没有方向变化点时，两个位置应该是同向，先赋值一个方向*/

									/*判断是否同向，如果不是，则把车头的方向做反向处理*/
		IsDirSame = dquIsDirSameExp(pDsuEmapStru, &stuTailLoc, &stuHeadLoc, PointNum, pPointStru);
		if (DSU_TRUE == IsDirSame)
		{
			retDir = LinkDirA;
		}
		/*本车与前车对向*/
		else if (DSU_FALSE == IsDirSame)
		{
			retDir = (EMAP_SAME_DIR == LinkDirA) ? EMAP_CONVER_DIR : EMAP_SAME_DIR;
		}
		else
		{
			retDir = 0U;
		}
	}

	return retDir;
}

/************************************************
*函数功能:	指定查找位置，按指定得位置找A、B中距离较近得点
*输入参数:	pPosFind-查找起始点（包含位置、方向）
*			pPosA、pPosB-AB两个点
*			PointNum、pPointStru-道岔信息
*输出参数:	pDist-查询成功时，返回较近点的距离
*返回值:	0:查询失败，沿指定方向未找到A、B点;
*			1-A点距离起点较近;
*			2-B点距离起点较近;
*注意：		cjq 20230802。
*			1.如果A、B点一个可达、一个不可达，则可达得点认为是距离起点较近得点(针对前方有AB两个点，且两点之间有一个四开道岔的场景)
*			2.两个点都可达时，A\B必须都在起点指定得前方，否则返回错误
*			3.如果AB点重合也会返回失败
************************************************/
UINT8 dsuGetNearPosByPointExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const LOD_STRU *pPosFind, 
	IN const LOD_STRU *pPosA, IN const LOD_STRU *pPosB, UCHAR PointNum, IN const DQU_POINT_STATUS_STRU *pPointStru, OUT INT32 *pDist)
{
	UINT8 retNo = 0U;

	UINT8 byFunA = 0u;
	UINT8 byFunB = 0u;
	INT32 nDistA = 0;
	INT32 nDistB = 0;

	if ((NULL == pPosFind) || (NULL == pPosA) || (NULL == pPosB) || (NULL == pPointStru) || (NULL == pDist))
	{
		/*nothing*/
	}
	else
	{
		/*非通信车没有方向，所以取头尾两个点，看那个点近，取较近的距离*/
		byFunA = dsuDispCalcuByLOCExp(pDsuEmapStru, pPosFind, pPosA, PointNum, pPointStru, &nDistA);
		byFunB = dsuDispCalcuByLOCExp(pDsuEmapStru, pPosFind, pPosB, PointNum, pPointStru, &nDistB);

		/*都找到了，则比远近*/
		if ((1U == byFunA) && (1U == byFunB))
		{
			if ((nDistA <= 0) || (nDistB <= 0) || (nDistA == nDistB))
			{
				retNo = 0U;
			}
			else
			{
				retNo = (nDistA < nDistB) ? 1u : 2u;
				*pDist = (nDistA < nDistB) ? nDistA : nDistB;
			}
		}
		/*只找到一个且距离大于0时，找到那个认为那个近*/
		else if (1U == byFunA)
		{
			retNo = (nDistA > 0) ? 1u : 0u;
			*pDist = nDistA;
		}
		else if (1U == byFunB)
		{
			retNo = (byFunB > 0) ? 2u : 0u;
			*pDist = nDistB;
		}
		/*一个都找不到，说明不可达（可能因为道岔位置）*/
		else
		{
			retNo = 0u;
		}
	}

	return retNo;
}
