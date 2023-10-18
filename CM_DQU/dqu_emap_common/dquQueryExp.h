/************************************************************************
*
* 文件名   ：  dsuQuery.h
* 版权说明 ：  北京交控科技科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2009.11.21
* 作者     ：  软件部
* 功能描述 ：  dsu查询函数公共函数头文件
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/

#ifndef DSU_QUERYEXP_H
#define DSU_QUERYEXP_H

#ifdef ZCAPP_VXWORKS_CODECONTROL
#include <vxWorks.h>
#endif /*End of ZCAPP_VXWORKS_CODECONTROL*/

#include "CommonTypes.h"
#include "dsuStruct.h"
#include "dquVobcCommonStru.h"

#ifdef __cplusplus
extern "C" {
#endif

/*ATO函数声明*/

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
UINT8 dquIsDirSameExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const LOD_STRU *pLocA, IN const LOD_STRU *pLocB, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *PointInfo);

/*2.两点间相对距离查询*/
UINT8 dsuDispCalcuByLOCExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODAStru, const LOD_STRU *pLODBStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru, INT32 *Displacement);

/*功能函数，根据道岔状态获取当前Link的相邻Link*/
UINT8 dsuGetAdjacentLinkIDExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkId, const UCHAR Direction, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, UINT16 *pLinkId);


/*功能函数： 利用运行方向信息，得到当前link沿运行方向的相邻link组*/
UINT8 dsuGetAdjacentLinkIDArrayExp(const DSU_EMAP_STRU* pDsuEmapStru, const UINT16 LinkId, const UCHAR Direction, UINT16* pLinkId);

/*功能函数，旧位置+位移=新位置*/
UCHAR dsuLODCalcuExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru);

/*
* 功能描述： 功能函数，根据当前位置，和位移以及输入的道岔信息，得到新位置.
如果一个点即当前Lnk起点，又是下一个Link终点，按照离当前点pLODStru最近的Link Id返回
查找到线路尽头时返回ff，输出新位置
*/
UINT8 dsuLODCalcuNewExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement, const UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru);

/*功能函数，获取两点之间的link序列*/
UINT8 dsuGetLinkBtwLocExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN], UINT16 *wLinkNumber);

/*功能函数，判断一个点是否在区域范围内*/
UINT8 dsuCheckLocInAreaExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	const UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	const LOD_STRU *pCurrentLODStru, UINT8 *pLocInArea);

/*功能函数，获取link长度*/
UINT32 LinkLengthExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wLinkID);
#if 0
/*
* 函数功能： 此函数的功能是判断当前位置是否在指定区域范围内，此函数是对dsuCheckLocInAreaQuick
的一个补充。用于将当前方向取反，再计算是否在区域内。
* 入口参数： const LOD_STRU * pLODStartStru,	区域始端位置
*            const LOD_STRU * pLODEndStru,	区域终端位置
* 出口参数： UINT16 wLinkID[DSU_MAX_LINK_BETWEEN]				Link序列数组
*            UINT16 *wLinkNumber                             Link个数
*			 const LOD_STRU * pCurrentLODStru,当前位置
*            UINT8 *pLocInArea         1,在指定区域
*                                     0,不在指定区域
* 返回值：   0，查询失败
*            1，查询成功
*/
UINT8 dsuCheckLocInAreaQuickExp2(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UINT16 wLinkNumber, const UINT16 wLinkID[DSU_MAX_LINK_BETWEEN],
	const LOD_STRU * pCurrentLODStru, UINT8 *pLocInArea);
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
*               UINT16 *wLinkNumber                             Link个数
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
	const UINT16 wLinkNumber, const UINT16 wLinkID[DSU_MAX_LINK_BETWEEN],
	const LOD_STRU * pCurrentLODStru, UINT8 *pLocInArea);
#endif

/*功能函数,查询当前Garde的相邻Grade*/
UINT16 FindAdjacentGradeIDExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wCurrentGradeID, const UCHAR chDirection, const UCHAR PointNum, const DQU_POINT_STATUS_STRU * pstPointStrut);

/*计算点所在坡度*/
UINT8 dsuGetGradeIdbyLocExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, const UINT32 offset, UINT16* GradeID);
/*计算点到所在坡度起点的距离*/
UINT8 dsuGetDistanceFromOriginExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 GradeId, const UINT16 linkId, const UINT32 Ofst, UINT32* length);
/*计算坡度变坡点于坡度起点的距离*/
UINT8 dsuDividPointDistanceFromOriginExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 GradeId, const UINT16 linkId, UINT32* endPointDistance, UINT32* startPointDistance);
/*计算点所在坡度值*/
UINT8 dsuCalculateGradeValueExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkId, const UINT32 ofst, FLOAT32* GradeVal);
/*计算最差坡度*/
UCHAR dsuGradientGetExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UCHAR PointNum, const DQU_POINT_STATUS_STRU * pPointStru, FLOAT32 * Gradient);
/*判断后一坡度的坡度值相比前一个坡度的坡度值是增加，不变化还是减少 */
UINT8 dquGradeTrendExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 fistGradeID, const UINT16 secondGradeID, UINT8 *GradeTrend);

/*
函数功能：获取前面的坡度
输入参数：UINT16 linkId,当前link
输出参数：gradeFrnt 坡度
Null：  线路终点或前面坡度为统一坡度
非Null：前面坡度在坡度表中可查
返回值：0失败 1成功
*/
UINT8 dsuGetGradeFrntExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, DSU_GRADE_STRU** gradeFrnt);
/*
函数功能：获取后面的坡度
输入参数：UINT16 linkId,当前link
输出参数：gradeRear 坡度
Null：  线路终点或后面坡度为统一坡度
非Null：后面坡度在坡度表中可查
返回值：0失败 1成功
*/
UINT8 dsuGetGradeRearExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, DSU_GRADE_STRU** gradeRear);
/**
函数功能：获取当前LINK的静态限速个数
输入参数：
@linkId：当前link
输出参数：无
返回值：静态限速个数
*/
UINT8 GetStcLmtSpdCntByLnkIdExp(const DSU_EMAP_STRU *pDsuEmapStru, DSU_STC_LIMIT_LINKIDX_STRU *pDsuLimitStcLinkIdx, const UINT16 linkId);

/**
函数功能：获取制定LINK的的指定下标的静态临时限速的对象指针
输入参数：
@linkId：当前Link编号
@lmtSpdIdx：对应Link的第几个限速，取值范围1-10
返回值：
@NULL：获取失败
@非空：对应限速结构体指针
*/
DSU_STATIC_RES_SPEED_STRU *GetStcLmtSpdInfoByLnkIdExp(const DSU_EMAP_STRU *pDsuEmapStru, DSU_STC_LIMIT_LINKIDX_STRU *pDsuLimitStcLinkIdx, const UINT16 linkId, const UINT8 idx);
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
UINT8 ConvertToVirtualSwitchesExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 multiPointId, const UINT8 multiPointState, UINT16 wVirtualPointIds[MAX_VIRTUAL_SWITCH_NUM], UINT8 wVirtualPointStates[MAX_VIRTUAL_SWITCH_NUM]);

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
*/
UINT8 ConvertToMultiSwitchExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wVirtualPointIds[MAX_VIRTUAL_SWITCH_NUM], const UINT8 wVirtualPointStates[MAX_VIRTUAL_SWITCH_NUM], const UINT8 wVirtualPointNum, UINT16 *multiPointId, UINT8 *multiPointState);
#endif
/************************************************
*函数功能:根据link获取对应的OC的ID(车车新增接口)
*输入参数:linkId
*输出参数:ocId
*返回值:0失败,1成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuOcByLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId, UINT16 *ocId);


/************************************************
*函数功能:获取信号机位置(车车新增接口)
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处link以及偏移量
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuGetSignalPositionExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, LOD_STRU *position);

/************************************************
*函数功能:获取信号机真实位置
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处真实link以及偏移量
*返回值:0:失败 1:成功
*备注：
************************************************/
UINT8 dsuGetRealSignalPositionExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, LOD_STRU *position);

#if 0
/************************************************
*函数功能:获取当前link是否有折返停车点(不含折返后停车点)(车车新增接口)
*输入参数:wId:Link索引编号
*输出参数:result:0x55有;0xAA:无
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuCheckArStopOnLinkExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, UINT8 *result);
#endif

/************************************************
*函数功能:查询link序列内的道岔信息(车车新增接口)
*输入参数:pLinkStru:Link序列信息
*输出参数:pBaliseStru:link序列内包含的道岔信息
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
*1、支持灯泡线 wyd 20230727
************************************************/
UINT8 dsuGetPointsBetweenLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pLinkStru, COM_DQU_POINT_STRU *pPointStru);

#if 0
/************************************************
*函数功能:传入一个位置和一段长度，查找此长度范围内的道岔信息和link序列(车车新增接口)
* 输入参数：	const LOD_STRU * pLODStru,	始端位置     const INT32     disPlacement,	查找长度
* 输出参数：	DQU_LINKS_STRU *pLinkStru		Link序列 DQU_BALISE_STRU *pBaliseStru    道岔序列
* 返回值：   	0,  查询失败  1,  查询成功
*备注：  经过的道岔均按定位处理 add by sds 2019-6-17
************************************************/
UINT8 dsuGetLinkAndBaliseBtwLocExp(DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement,
	COM_DQU_OBJECT_STRU *pLinkStru, COM_DQU_POINT_STRU *pBaliseStru, LOD_STRU *pEndLodStru);

/************************************************
*函数功能:查找两个应答器之间的距离(车车新增接口)
*输入参数：wBaliseId1 应答器1ID wBaliseId2 应答器2ID  wMaxLen	 两个应答器之间的最大距离
*输出参数：*len 两个应答器之间的实际距离
*返回值：0：失败  1：成功
*备注：  经过的道岔均按定位处理 add by sds 2019-6-17
************************************************/
UINT8 GetLenBetweenBaliseExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT32 wBaliseId1, const UINT32 wBaliseId2, const UINT32 wMaxLen, UINT32 *len);
#endif

/************************************************
*函数功能:沿指定方向查找两个位置间的道岔序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)  pEnd   查找终点位置(方向无效) pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pPointExp 查找到的序列 (0x55:定位 0xAA:反位)
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
************************************************/
UINT8 GetPointsInfoInAreaExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_POINT_STRU *pPointExp);


/************************************************
*函数功能:沿指定方向查找两个位置间的PSD序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)  pEnd   查找终点位置(方向无效)  pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pObjectExp 查找到的序列
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
*		1、重新整理，支持灯泡线。wyd 020230725
************************************************/
UINT8 GetPsdsInfoInAreaExp(const DSU_EMAP_STRU *pDsuEmapStru, const DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru,
	const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp);

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
	const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp);

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
UINT8 dsuLinksWithCfgDisInInputLinksExp(const DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pLinkIn, const LOD_STRU *pTailLoc, const INT32 dis, COM_DQU_OBJECT_STRU *pLinkOut);
#endif

/************************************************
*函数功能:通过输入的link序列、车尾位置和配置距离，查询车尾位置至配置距离范围内的link所属的OC数量和ID(车车新增接口)
*输入参数:pLinkIn:输入的link序列
pTailLoc:车尾位置
dis:配置距离
*输出参数:pLinkOut:查询到的OC数量和ID
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
************************************************/
UINT8 dsuOcsWithCfgDisInInputLinksExp(const DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pTailLoc, const INT32 dis, IN const UINT8 PointNum, const DQU_POINT_STATUS_STRU * pPointStru, COM_BASE_COMM_OC_STRU *pOcOut);
#if 0
/************************************************
*函数功能:查询设备所属OC(车车新增接口)
*输入参数:devId:设备ID
*输出参数:devType:设备类型(1:道岔,2:信号机,3:紧急停车按钮)(对于道岔，暂时取汇合link为准)
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
************************************************/
UINT8 dsuOcWithInputDevIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 devId, const UINT8 devType, UINT16 *ocId);

/************************************************
*函数功能:查询一定范围内与列车方向(0x55)相同的信号机(车车新增接口)
*输入参数    pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
pEnd   查找终点位置(方向无效)
pPointAll 从起点到终点的道岔信息(有序排列)
*输出参数：pObjectExp 查找到的序列
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
2. 修复最后一个link查询不到的问题 by kuan.he 20211028。
************************************************/
UINT8 dsuSignalBetweenPisitionsExpDirSame(const DSU_EMAP_STRU *pDsuEmapStru, const DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru, const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp);

/************************************************
*函数功能:查询一定范围内与列车方向(0xAA)相同的信号机(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
pEnd   查找终点位置(方向无效)
pPointAll 从起点到终点的道岔信息(有序排列)
*输出参数：pObjectExp 查找到的序列
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
1、lxc,2020/12/15,VBTC-Bug-498 修复局部变量为初始化问题
2. 修复最后一个link查询不到的问题 by kuan.he 20211028。
************************************************/
UINT8 dsuSignalBetweenPisitionsExpDirConv(const DSU_EMAP_STRU *pDsuEmapStru, const DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru, const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp);
#endif
/************************************************
*函数功能:根据道岔ID获取道岔的位置(车车新增接口)
*输入参数：当前位置
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
************************************************/
UINT8 GetPointLocFromIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, DQU_POINT_LOC_STRU *pPointLoc);

/************************************************
*函数功能:根据道岔ID获取道岔岔心位置(按照道岔的汇合link)
*输入参数：当前位置
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注：创建 zy
************************************************/
UINT8 GetSwitchCenterExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, LOD_STRU *pPointLoc);
#if 0
/************************************************
*函数功能:根据道岔ID获取道岔的岔后防护距离
*输入参数：道岔ID
*输出参数：道岔的岔后防护距离
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
************************************************/
UINT8 GetProtectLengthFromIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, UINT16 *pProtectLength);

/************************************************
*函数功能:查询2个link的相对方向（若LINK B是LINK A的终点相邻LINK  则A到B为正向）(车车新增接口)
*输入参数:		linkAID:LinkA的索引ID
linkBID:LinkB的索引ID
*输出参数:dir:2个link的相对方向  0x55=正向 0xAA=反向  0xFF=无效
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-17
************************************************/
UINT8 dsuGetDirOfLinkToLinkExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkAID, const UINT16 linkBID, UINT8 *dir);
#endif
/************************************************
*函数功能:根据物理区段序列，查询包含的link序列(车车新增接口)
*输入参数:		PhysicalSgmt	物理区段信息
*输出参数:		pLinks link序列信息
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuRegionBlockTolinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pPhysicalSgmtStru, COM_DQU_OBJECT_STRU *pLinksStru);

/************************************************
*函数功能:根据link编号，查询所属ITS设备ID(车车新增接口)
*输入参数:		UINT16 LinkWid	link编号
*输出参数:		UINT16  ItsId 所属ITS设备ID
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuItsIdOfLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 LinkWid, UINT16 *ItsId);
#if 0
/************************************************
*函数功能:获取限速区段信息(车车新增接口)
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处link以及偏移量
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuGetRespeedSgmtExp(DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, DSU_RESPEED_SGMT_STRU *RespeedSgmtStru);
#endif
/************************************************
*函数功能:获取逻辑区段信息(车车新增接口)
*输入参数:wId:逻辑区段ID
*输出参数:逻辑区段信息
*返回值:0:失败 1:成功
*备注：add by db 20220623
************************************************/
UINT8 dsuGetLogicSgmtInfoByIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 wId, DSU_LOGIC_SGMT_STRU * pLogicSgmtStru);
#if 0
/************************************************
*函数功能:根据计轴区段序列，查询包含的link序列(车车新增接口)
*输入参数:		PhysicalSgmt	物理区段信息
*输出参数:		pLinks link序列信息
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuAxleSgmtTolinkExp(DSU_EMAP_STRU *pDsuEmapStru, const COM_DQU_OBJECT_STRU *pAxleSgmtStru, COM_DQU_OBJECT_STRU *LinksStru);

/************************************************
*函数功能:根据link编号，查找所属的计轴区段ID
*输入参数:		linkId		link的ID
*输出参数:		AxleSgmtId	计轴区段Id
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsulinkofAxleSgmtExp(DSU_EMAP_STRU *pDsuEmapStru, UINT16 linkId, UINT16 *AxleSgmtId);
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
**********************************************************************************************/
UINT8 dsuPathEndNextLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, UINT8 szLinkQueryDir[50], UINT8* pPathNum, UINT16 szPath[50][50], UINT16 szLinkNum[50]);

/*********************************************************************************************
*函数功能:判断两个路径终点是否为同一link，若为同一link则输出这条路径(车车通信新增接口)
*输入参数:		cPathANum A路径的数量
szPathA A路径数据
szLinkANum A路径的link数量
cPathBNum B路径的数量
szPathB B路径数据
szLinkBNum B路径的link数量
*输出参数:		szPath 找到路径数据
pPathLinkNum 找到路径数量
*返回值:终点无相同link:0
终点存在相同link:1
*备注： add by lxc 2021-2-28
*********************************************************************************************/
UINT8 dsuPathSameEndLinkExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT8 cPathANum, const UINT16 szPathA[50][50], const UINT16 szLinkANum[50], const UINT8 cPathBNum
	, const UINT16 szPathB[50][50], const UINT16 szLinkBNum[50], UINT16 szPath[100], UINT16* pPathLinkNum, UINT8* pLinkDirChangedNum, UINT8 cABPathDir);
/************************************************
*函数功能:查询AB两端之间的距离
*输入参数:		pDsuEmapStru 电子地图数据
*				pLODAStru	A点
*				pLODBStru	B点
*输出参数:		pLinkNum	输出路径的link数量
*				szLinkId	输出路径的linkId序列
*				pDir		输出A->B的方向
*返回值:失败:DSU_NULL_32
*		成功:距离
*备注： add by lxc 2021-2-28
************************************************/
UINT32 dsuLenByPointExp(const DSU_EMAP_STRU* pDsuEmapStru, const LOD_STRU* pLODAStru, const LOD_STRU* pLODBStru, UINT16* pLinkNum, UINT16 szLinkId[100], UINT8* pDir, UINT8* pLinkDirChangedNum);


/************************************************
*函数功能:	获取LINK逻辑方向变化点Exp
*输入参数:	DSU_EMAP_STRU *pDsuEmapStru
*			UINT16 linkId
*输出参数:
*返回值:	失败:0
*			成功:0x55 0xAA 0xFF LINK逻辑方向变化点
*备注：
************************************************/
UINT8 dquGetLinkLogicDirChangedByIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 linkId);
#if 0
/************************************************
*函数功能:根据道岔ID获取道岔的属性(车车新增接口)
*输入参数：电子地图结构体指针；道岔ID
*输出参数：道岔的属性
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
修改：by yt 2021年9月11日 增加函数声明
************************************************/
UINT8 GetDPropertyFromIdExp(const DSU_EMAP_STRU *pDsuEmapStru, const UINT16 PointId, UINT8 *DProperty);
#endif
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
UINT8 dsuGetAdjacentLogicSgmtImp(const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT8 Dir, OUT UINT8 *pLogicSgmtNum, OUT UINT16 *pLogicSgmtIdArray, OUT UINT8 *pDirChangedTag);

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
UINT8 dsuPointByLenExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const LOD_STRU *pLODStru, IN const INT32 Displacement, OUT UINT8 *pNewLODNum, OUT LOD_STRU *pNewLODStru);

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
UINT8 dsuGetAdjacentLogicSgmtExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT8 Dir, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *pPointInfo, OUT UINT16 *pLogicSgmtId);

/************************************************
*函数功能:	查询逻辑区段端点
*输入参数:	LogicSgmtId		逻辑区段ID
*			Dir				查询方向（输入EMAP_SAME_DIR返回右侧端点，EMAP_CONVER_DIR返回左侧端点）
*输出参数:	pEndLoc			逻辑区段端点（方向返回查询方向）
*返回值:	失败:0
*			成功:1
*备注：		add by wyd 20220312
************************************************/
UINT8 dsuGetLogicSgmtEndLocExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT8 Dir, OUT LOD_STRU *pEndLoc);

/************************************************
*函数功能:	查询逻辑区段长度
*输入参数:	LogicSgmtId		逻辑区段ID
*输出参数:	pDisp			逻辑区段长度
*返回值:	失败:0
*			成功:1
*备注：		add by wyd 20230703
************************************************/
UINT8 dsuGetLogicSgmtDispExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtId, OUT UINT32 *pDisp);

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
UINT8 dsuLogicSgmtToLinkExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const UINT16 LogicSgmtNum, IN const UINT16 *pLogicSgmtId, IN const UINT8 Dir, OUT UINT16 *pLinkNum, OUT UINT16 *pLinkId);

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
************************************************/
UINT8 dsuLogicSgmtCalcuByLocExp(IN const DSU_EMAP_STRU *pDsuEmapStru, IN const LOD_STRU LocStart, IN const LOD_STRU LocEnd, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *pPointInfo, OUT UINT16 *pLogicSgmtNum, OUT UINT16 *pLogicSgmtId);

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
UINT8 dsuGetLogicSgmtIdByLocExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const LOD_STRU* pLocStru, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointInfo, OUT UINT16* pLogicSgmtId);

/************************************************
*函数功能:	给定一个逻辑区段id，查询这个逻辑区段属于那些进路
*输入参数:	LogicSgmtId			逻辑区段id
*			sizeofBuff			数组的大小
*输出参数:	pRouteNum			所属进路数量
*			pRouteIdBuff		所属进路id列表
*返回值:	失败:0
*			成功:1
*使用注意：		如果给定的逻辑区段id是某个进路A的保护区段，则进路A不在列表内。即保护区段不计入进路。
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetRouteIdByLogicSgmtExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN UINT16 LogicSgmtId, IN UINT32 sizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

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
*使用注意：
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetRouteIdBuffByLocExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const LOD_STRU* pLocStru, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointInfo, IN UINT32 sizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

/************************************************
*函数功能:	给定一个逻辑区段id，查询这个逻辑区段属于哪些CBTC接近区段
*输入参数:	LogicSgmtId			逻辑区段id
*			sizeofBuff			数组的大小
*输出参数:	pRouteNum			所属CBTC接近区段索引id数量
*			pRouteIdBuff		所属CBTC接近区段索引id列表
*返回值:	失败:0
*			成功:1
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetCbtcAccessSgmtIdByLogicSgmtExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT16 LogicSgmtId, IN const UINT32 sizeofBuff, OUT UINT8* pCbtcAccessSgmtNum, OUT UINT16* pCbtcAccessSgmtIdBuff);

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
UINT8 dsuGetLongestRouteIdByRouteIdBuffExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT8 RouteNum, IN const UINT16* pRouteIdBuff, OUT UINT16* pRouteId);

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
UINT8 dsuGetRouteIdByRouteIdBuffExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT8 RouteNum, IN const UINT16* pRouteIdBuff, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointStru, OUT UINT16* pRouteId);

/************************************************
*函数功能:	给定一个接近区段索引id，查询这个接近区段属于哪些进路
*输入参数:	AccessSgmtId		接近区段索引id
*			SizeofBuff			进路数组大小
*输出参数:	pRouteNum			进路数量
*			pRouteIdBuff		进路数组
*返回值:	失败:0
*			成功:1
* 备注：	add by kuan.he 20230516
************************************************/
UINT8 dsuGetRouteIdByAccessSgmtIdExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT16 AccessSgmtId, IN const UINT32 SizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

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
UINT8 dsuGetRouteIdBuffByAccessSgmtIdBuffExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const UINT16 AccessSgmtNum, IN const UINT16* pAccessSgmtBuff, IN const UINT32 SizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

/************************************************
*函数功能:	给定一个进路id，查询这个进路的起点位置
*输入参数:	RouteId				进路Id
*输出参数:	pRouteStartLoc		进路起点位置
*返回值:	失败:0
*			成功:1
* 使用注意：进路起点位置方向是指进路起点指向终点的方向，非信号机防护方向
* 备注：	add by kuan.he 20230731
************************************************/
UINT8 dsuGetRouteStartLocByRouteIdExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN UINT16 RouteId, OUT LOD_STRU* pRouteStartLoc);

/************************************************
*函数功能:	给定一个进路id，查询这个进路的终点位置
*输入参数:	RouteId				进路Id
*输出参数:	pRouteStartLoc		进路终点位置
*返回值:	失败:0
*			成功:1
* 使用注意：进路终点位置方向是指进路起点指向终点的方向，非信号机防护方向
* 备注：	add by kuan.he 20230731
************************************************/
UINT8 dsuGetRouteEndLocByRouteIdExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN UINT16 RouteId, OUT LOD_STRU* pRouteEndtLoc);

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
	IN const UINT16 wLinkB, IN const UINT32 dwLinkBOff, IN const UCHAR PointNum, IN const DQU_POINT_STATUS_STRU *pPointStru);

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
*			1.如果A、B点一个可达、一个不可达，则可达得点认为是距离起点较近得点
*			2.A\B必须都在起点指定得前方，否则查询失败
*			3.如果AB点重合也会返回失败
************************************************/
UINT8 dsuGetNearPosByPointExp(IN const DSU_EMAP_STRU* pDsuEmapStru, IN const LOD_STRU *pPosFind,
	IN const LOD_STRU *pPosA, IN const LOD_STRU *pPosB, UCHAR PointNum, IN const DQU_POINT_STATUS_STRU *pPointStru, OUT INT32 *pDist);
#ifdef __cplusplus
}
#endif

#endif
