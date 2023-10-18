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

#ifndef DSU_QUERY_H
#define DSU_QUERY_H

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

/*设置当前数据源*/
UINT8 dquSetCurrentQueryDataSource(DSU_EMAP_STRU *pDsuEmapStru, DSU_EMAP_EXTEND_STRU *pDsuEmapExtendStru, DSU_STC_LIMIT_LINKIDX_STRU *pDsuLimitStcLinkIdx);

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
UINT8 dquIsDirSame(IN const LOD_STRU *pLocA, IN const LOD_STRU *pLocB, IN UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *PointInfo);

/*2.两点间相对距离查询*/
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
* 使用注意：cjq:20230801:【车车系统中不关注B点的方向】
合库和燕房项目:按照B的方向查找，AB位移以LinkB方向为参照；
互联互通项目因增加逻辑方向变化点，算法改为按照A的方向查找，AB位移以LinkA方向为参照。
* author:qxt 20170811
*/
UINT8 dsuDispCalcuByLOC(const LOD_STRU *pLODAStru, const LOD_STRU *pLODBStru,
	UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru, INT32 *Displacement);

/*功能函数，根据道岔状态获取当前Link的相邻Link*/
UINT8 dsuGetAdjacentLinkID(UINT16 LinkId, UCHAR Direction, UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, UINT16 *pLinkId);

/* 功能描述： 利用运行方向信息以及道岔状态信息，查询得到当前link沿运行方向的相邻link及运行方向在link上的方向
*			【传出方向是为了存在方向变化点时，获取运行方向在相邻link上的方向】。*/
UINT8 dsuGetAdjacentLinkIdAndDir(UINT16 LinkId, UCHAR Direction, UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, UINT16 *pLinkId, UINT8 *pLinkDir);

/*
* 功能描述： 利用运行方向信息，得到当前link沿运行方向的相邻link组。
* 输入参数： UINT16 LinkId, 当前Link编号
*            UCHAR Direction, 当前方向
* 输出参数： UINT16 *pLinkId，当前Link的相邻Link编号数组
* 返回值：   1,查询成功
*            0,查询失败,输入数据无效，道岔失表
*		     0xFF,线路终点，未找到道岔
*
*/
UINT8 dsuGetAdjacentLinkIDArray(UINT16 pLinkIdIn, UCHAR pDirection, UINT16* pLinkIdOut);

/*功能函数，旧位置+位移=新位置*/
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
*/

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
*/
UCHAR dsuLODCalcu(const LOD_STRU *pLODStru, INT32 Displacement, UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru);


/*
* 函数功能：	此函数的功能是查询两个位置间包含的link序列。
* ****************************************************************************************************
*        1          2              3                4                    5              6
*       -----×---------*--×--------------×--------------------×-------*-----×------------
*                       S                                                 E
*        图例：  ×，Link端点。*，查询输入的位置。1，2，3...Link示意编号
*        如图所示：Link3，4为所求结果。
* ****************************************************************************************************
* 入口参数：	const LOD_STRU * pLODStartStru,	始端位置
*               const LOD_STRU * pLODEndStru	终端位置
*               const UINT8 PointNum			道岔数目
*               const POINT_STRU * pPointStru	道岔信息
* 出口参数：	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN]				Link序列数组
*               UINT16 *wLinkNumber                             Link个数
* 返回值：   	0,  查询失败
*               1,  查询成功
* 使用注意：    目前的做法没有包含两端的Link Id。
*               函数要求两个点之间可以形成区域，否则返回0
*
合库和燕房项目： S沿着pLODEndStru->Dir方向查找，找到E则返回成功，找不到则返回失败。
*              互联互通项目： S沿着pLODStartStru->Dir方向查找，找到E则返回成功，找不到则返回失败。
*  author: qxt 20170814
*/
UINT8 dsuGetLinkBtwLoc(const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	UINT16 wLinkID[DSU_MAX_LINK_BETWEEN], UINT16 *wLinkNumber);


/*
* 函数功能： 此函数的功能是将判断当前位置是否在指定区域范围内。
* ****************************************************************************************************
*
*       --------------*--------------------*---------------------*-----------------
*                     S                    C                     E
*  合库和燕房项目： 查找算法为，计算位移DisplacementSC，DisplacementCE。如果两个位移符号相同，说明C在SE区域内
*  互联互通项目（新增逻辑方向变化点）：   查找算法为，计算位移DisplacementCS，DisplacementCE。如果两个位移符号不相同，说明C在SE区域内
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
*           4.燕房和合库项目：始端位置和终端位置传入的方向需保证一致
*             互联互通：保证始端位置的方向正确即可。
*/
UINT8 dsuCheckLocInArea(const LOD_STRU *pLODStartStru, const LOD_STRU *pLODEndStru,
	UINT8 PointNum, const DQU_POINT_STATUS_STRU *pPointStru,
	const LOD_STRU *pCurrentLODStru, UINT8 *pLocInArea);

/*功能函数，获取link长度*/
UINT32 LinkLength(UINT16 wLinkID);
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
UINT8 dsuCheckLocInAreaQuick2(const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	UINT16 wLinkNumber, UINT16 wLinkID[DSU_MAX_LINK_BETWEEN],
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
UINT8 dsuCheckLocInAreaQuick(const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	UINT16 wLinkNumber, UINT16 wLinkID[DSU_MAX_LINK_BETWEEN],
	const LOD_STRU * pCurrentLODStru, UINT8 *pLocInArea);
#endif

/*功能函数,查询当前Garde的相邻Grade*/
UINT16 FindAdjacentGradeID(const UINT16 wCurrentGradeID, const UCHAR chDirection, const UCHAR PointNum, const DQU_POINT_STATUS_STRU * pstPointStrut);

/*计算点所在坡度*/
UINT8 dsuGetGradeIdbyLoc(const UINT16 linkId, const UINT32 offset, UINT16* GradeID);
/*计算点到所在坡度起点的距离*/
UINT8 dsuGetDistanceFromOrigin(UINT16 gradeId, UINT16 linkId, UINT32 Ofst, UINT32* length);
/*计算坡度变坡点于坡度起点的距离*/
UINT8 dsuDividPointDistanceFromOrigin(UINT16 GradeId, UINT16 linkId, UINT32* endPointDistance, UINT32* startPointDistance);
/*计算点所在坡度值*/
UINT8 dsuCalculateGradeValue(UINT16 LinkId, UINT32 ofst, FLOAT32* GradeVal);
/*计算最差坡度*/
UCHAR dsuGradientGet(const LOD_STRU * pLODStartStru, const LOD_STRU * pLODEndStru,
	const UCHAR PointNum, const DQU_POINT_STATUS_STRU * pPointStru, FLOAT32 * Gradient);
/*判断后一坡度的坡度值相比前一个坡度的坡度值是增加，不变化还是减少 */
UINT8 dquGradeTrend(UINT16 fistGradeID, UINT16 secondGradeID, UINT8 *GradeTrend);

/*
函数功能：获取前面的坡度
输入参数：UINT16 linkId,当前link
输出参数：gradeFrnt 坡度
Null：  线路终点或前面坡度为统一坡度
非Null：前面坡度在坡度表中可查
返回值：0失败 1成功
*/
UINT8 dsuGetGradeFrnt(UINT16 linkId, DSU_GRADE_STRU** gradeFrnt);

/*
函数功能：获取后面的坡度
输入参数：UINT16 linkId,当前link
输出参数：gradeRear 坡度
Null：  线路终点或后面坡度为统一坡度
非Null：后面坡度在坡度表中可查
返回值：0失败 1成功
*/
UINT8 dsuGetGradeRear(UINT16 linkId, DSU_GRADE_STRU** gradeRear);

/**
函数功能：获取当前LINK的静态限速个数
输入参数：
@linkId：当前link
输出参数：无
返回值：静态限速个数
*/
UINT8 GetStcLmtSpdCntByLnkId(const UINT16 linkId);

/**
函数功能：获取制定LINK的的指定下标的静态临时限速的对象指针
输入参数：
@linkId：当前Link编号
@lmtSpdIdx：对应Link的第几个限速，取值范围1-10
返回值：
@NULL：获取失败
@非空：对应限速结构体指针
*/
DSU_STATIC_RES_SPEED_STRU *GetStcLmtSpdInfoByLnkId(const UINT16 linkId, const UINT8 idx);
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
UINT8 ConvertToVirtualSwitches(const UINT16 multiPointId, const UINT8 multiPointState, UINT16 wVirtualPointIds[MAX_VIRTUAL_SWITCH_NUM], UINT8 wVirtualPointStates[MAX_VIRTUAL_SWITCH_NUM]);

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
UINT8 ConvertToMultiSwitch(const UINT16 wVirtualPointIds[MAX_VIRTUAL_SWITCH_NUM], const UINT8 wVirtualPointStates[MAX_VIRTUAL_SWITCH_NUM], const UINT8 wVirtualPointNum, UINT16 *multiPointId, UINT8 *multiPointState);
#endif

/************************************************
*函数功能:查询link序列内的道岔信息(车车新增接口)
*输入参数:pLinkStru:Link序列信息
*输出参数:pPointStru:link序列内包含的道岔信息
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
*1、支持灯泡线 wyd 20230727
************************************************/
UINT8 dsuGetPointsBetweenLink(const COM_DQU_OBJECT_STRU *pLinkStru, COM_DQU_POINT_STRU *pPointStru);

#if 0
/************************************************
函数功能:获取当前link是否有折返停车点(不含折返后停车点)(车车新增接口)
输入参数:wId:Link索引编号
输出参数:result:0x55有;0xAA:无
返回值:0:失败 1:成功
备注：add by sds 2019-6-17
************************************************/
UINT8 dsuCheckArStopOnLink(const UINT16 wId, UINT8 *result);

/************************************************
函数功能:传入一个位置和一段长度，查找此长度范围内的道岔信息和link序列(车车新增接口)
* 输入参数：	const LOD_STRU * pLODStru,	始端位置
const INT32     disPlacement,	查找长度
* 输出参数：	DQU_LINKS_STRU *pLinkStru		Link序列
*               DQU_BALISE_STRU *pBaliseStru    道岔序列
* 返回值：   	0,  查询失败
*               1,  查询成功
* 使用注意：    经过的道岔均按定位处理
备注：add by sds 2019-6-17
************************************************/
UINT8 dsuGetLinkAndBaliseBtwLoc(DSU_EMAP_STRU *pDsuEmapStru, const LOD_STRU *pLODStru, const INT32 Displacement,
	COM_DQU_OBJECT_STRU *pLinkStru, COM_DQU_POINT_STRU *pBaliseStru, LOD_STRU *pEndLodStru);

/************************************************
函数功能:通过输入的link序列、车尾位置和配置距离，查询车尾位置至配置距离范围内的link数量和ID(车车新增接口)
输入参数:pLinkIn:输入的link序列
pTailLoc:车尾位置
dis:配置距离
输出参数:pLinkOut:查询到的link数量和ID
返回值:0:失败 1:成功
备注：add by sds 2019-6-17
************************************************/
UINT8 dsuLinksWithCfgDisInInputLinks(const COM_DQU_OBJECT_STRU *pLinkIn, const LOD_STRU *pTailLoc, const INT32 dis, COM_DQU_OBJECT_STRU *pLinkOut);
#endif

/************************************************
函数功能:通过输入的车尾位置和配置距离，查询车尾位置至配置距离范围内的link所属的OC数量和ID,直到线路终点或四开道岔停止(车车新增接口)
输入参数:pLinkIn:输入的link序列
pTailLoc:车尾位置
dis:配置距离
输出参数:pLinkOut:查询到的OC数量和ID
返回值:0:失败 1:成功
备注：add by db 2022-03-24
************************************************/
UINT8 dsuOcsWithCfgDisInInputLinks(const LOD_STRU *pTailLoc, const INT32 dis, IN const UINT8 PointNum, const DQU_POINT_STATUS_STRU * pPointStru, COM_BASE_COMM_OC_STRU *pOcOut);

#if 0
/************************************************
*函数功能:查找指定距离内两个应答器之间的距离(车车新增接口)
*输入参数:wBaliseId1:起始应答器ID
*wBaliseId2:终止应答器ID
*wMaxLen:指定最大查找距离
*输出参数:len:应答器之间的距离
*返回值:0:失败或未找到 1:成功找到
*备注：add by sds 2019-6-17
************************************************/
UINT8 GetLenBetweenBalise(const UINT32 wBaliseId1, const UINT32 wBaliseId2, const UINT32 wMaxLen, UINT32 *len);

/************************************************
*函数功能:查询设备所属OC(车车新增接口)
*输入参数:devId:设备ID
*输出参数:devType:设备类型(1:道岔,2:信号机,3:紧急停车按钮)
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuOcWithInputDevId(const UINT16 devId, const UINT8 devType, UINT16 *ocId);

/************************************************
*函数功能:	查询一定范围内与列车方向相同的信号机(车车新增接口)
*输入参数：	pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
*			pEnd   查找终点位置(方向无效)
*			pPointAll 从起点到终点的道岔信息(有序排列)
*输出参数：	pObjectExp 查找到的序列
*返回值:	0:失败 1:成功
*备注：		add by sds 2019-6-17
*			2. 修复最后一个link查询不到的问题 by kuan.he 20211028。
************************************************/
UINT8 dsuSignalBetweenPisitions(const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pSignalOut);
#endif

/************************************************
*函数功能:根据link获取对应的OC的ID(车车新增接口)
*输入参数:linkId
*输出参数:ocId
*返回值:0失败,1成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuOcByLink(const UINT16 wId, UINT16 *ocId);


/************************************************
*函数功能:获取信号机位置(车车新增接口)
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处link以及偏移量
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuGetSignalPosition(const UINT16 wId, LOD_STRU *position);

/************************************************
*函数功能:获取信号机真实位置(车车新增接口)
*输入参数:wId:信号机索引编号
*输出参数:position:信号机所处真实link以及偏移量
*返回值:0:失败 1:成功
*备注：
************************************************/
UINT8 dsuGetRealSignalPosition(const UINT16 wId, LOD_STRU *position);

/************************************************
*函数功能:沿指定方向查找两个位置间的道岔序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
*pEnd   查找终点位置(方向无效)
*pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pPointExp 查找到的序列 (0x55:定位 0xAA:反位)
*返回值：0:失败或未找到 1:成功找到
*备注：add by sds 2019-6-17
************************************************/
UINT8 GetPointsInfoInArea(const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_POINT_STRU *pPointExp);


/************************************************
*函数功能:沿指定方向查找两个位置间的PSD序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
*pEnd   查找终点位置(方向无效)
*pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pObjectExp 查找到的序列
*返回值：0:失败或未找到 1:成功找到
*备注：add by sds 2019-6-17
************************************************/
UINT8 GetPsdsInfoInArea(const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp);

/************************************************
*函数功能:沿指定方向查找两个位置间的ESB序列(车车新增接口)
*输入参数：pStart 查找起点位置以及方向(0x55:顺着link方向;0xAA:逆着link方向)
*pEnd   查找终点位置(方向无效)
*pPointAll 全部道岔信息(比查找的结果范围大且无序)
*输出参数：pObjectExp 查找到的序列
*返回值：0:失败或未找到 1:成功找到
*备注：add by sds 2019-6-17
************************************************/
UINT8 GetEsbsInfoInArea(const LOD_STRU *pStart, const LOD_STRU *pEnd, const COM_DQU_POINT_STRU *pPointAll, COM_DQU_OBJECT_STRU *pObjectExp);
#if 0
/************************************************
*函数功能:查询2个link的相对方向（若LINK B是LINK A的终点相邻LINK  则A到B为正向）(车车新增接口)
*输入参数:
*linkAID:LinkA的索引ID
*linkBID:LinkB的索引ID
*输出参数:dir:2个link的相对方向  0x55=正向 0xAA=反向  0xFF=无效
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuGetDirOfLinkToLink(const UINT16 linkAID, const UINT16 linkBID, UINT8 *dir);
#endif
/************************************************
*函数功能:根据列车位置获取该位置所属OC(车车新增接口)
*输入参数：当前位置
*输出参数：所属OC的ID
*返回值：0:失败或未找到 1:成功找到
*备注：add by sds 2019-6-17
************************************************/
UINT8 GetLocInOdId(const LOD_STRU *pCurrLoc, UINT16 *pOcID);

/************************************************
*函数功能:根据道岔ID获取道岔的位置(车车新增接口)
*输入参数：当前位置
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注：add by sds 2019-6-17
************************************************/
UINT8 GetPointLocFromId(const UINT16 PointId, DQU_POINT_LOC_STRU *pPointLoc);

/************************************************
*函数功能:根据道岔ID获取道岔岔心位置(按照道岔的汇合link)
*输入参数：当前位置
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注： 创建 zy
************************************************/
UINT8 GetSwitchCenterLoc(const UINT16 PointId, LOD_STRU *pPointLoc);
#if 0
/************************************************
*函数功能:根据道岔ID获取道岔的岔后防护距离
*输入参数：道岔的ID
*输出参数：道岔的位置
*返回值：0:失败或未找到 1:成功找到
*备注：add by sds 2019-6-17
************************************************/
UINT8 GetProtectLengthFromId(const UINT16 PointId, UINT16 *pProtectLength);

/************************************************
*函数功能:根据道岔ID获取道岔的属性(车车新增接口)
*输入参数：电子地图结构体指针；道岔ID
*输出参数：道岔的属性
*返回值：0:失败或未找到 1:成功找到
*备注： add by sds 2019-6-17
************************************************/
UINT8 GetDPropertyFromId(const UINT16 PointId, UINT8 *DProperty);
#endif
/************************************************
*函数功能:根据物理区段序列，查询包含的link序列(车车新增接口)
*输入参数:		PhysicalSgmt	物理区段信息
*输出参数:		pLinks link序列信息
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuRegionBlockTolink(const COM_DQU_OBJECT_STRU *PhysicalSgmtStru, COM_DQU_OBJECT_STRU *LinksStru);

/************************************************
*函数功能:根据link编号，查询所属ITS设备ID(车车新增接口)
*输入参数:		UINT16 LinkWid	link编号
*输出参数:		UINT16  ItsId 所属ITS设备ID
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuItsIdOfLink(const UINT16 LinkWid, UINT16 *ItsId);
#if 0
/************************************************
*函数功能:根据限速区段ID获取限速区段信息(车车新增接口)
*输入参数:wId:信号机索引编号
*输出参数:DSU_RESPEED_SGMT_STRU限速区段信息结构体
*返回值:0:失败 1:成功
*备注：add by sds 2019-6-17
************************************************/
UINT8 dsuGetRespeedSgmt(const UINT16 wId, DSU_RESPEED_SGMT_STRU *RespeedSgmtStru);
#endif
/************************************************
*函数功能:根据限速逻辑区段ID获取逻辑区段信息
*输入参数:wId:限速逻辑区段ID
*输出参数:逻辑区段信息
*返回值:0:失败 1:成功
*备注：add by db 2022-06-23
************************************************/
UINT8 dsuGetLogicSgmtInfoById(const UINT16 wId, DSU_LOGIC_SGMT_STRU * pLogicSgmtStru);
#if 0
/************************************************
*函数功能:根据计轴区段序列，查询包含的link序列(车车新增接口)
*输入参数:		PhysicalSgmt	物理区段信息
*输出参数:		pLinks link序列信息
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsuAxleSgmtTolink(const COM_DQU_OBJECT_STRU *pAxleSgmtStru, COM_DQU_OBJECT_STRU *LinksStru);

/************************************************
*函数功能:根据link编号，查找所属的计轴区段ID
*输入参数:		linkId		link的ID
*输出参数:		AxleSgmtId	计轴区段Id
*返回值:0:失败 1:成功
*备注： add by sds 2019-6-21
************************************************/
UINT8 dsulinkofAxleSgmt(UINT16 linkId, UINT16 *AxleSgmtId);
#endif
/************************************************
*函数功能:查询AB两端之间的距离
*输入参数:		pLODAStru	A点
*				pLODBStru	B点
*输出参数:		pLinkNum	输出路径的link数量
*				szLinkId	输出路径的linkId序列
*				pDir		输出A->B的方向
*返回值:失败:DSU_NULL_32
*		成功:距离
*备注： add by lxc 2021-2-28
当起点和终点为同一点，且A点方向有效时，输出A点方向。wyd 20220303
************************************************/
UINT32 dsuLenByPoint(const LOD_STRU *pLODAStru, const LOD_STRU *pLODBStru, UINT16* pLinkNum, UINT16 szLinkId[100], UINT8* pDir);

/************************************************
*函数功能:获取LINK逻辑方向变化点
*输入参数:		UINT16 linkIdA点
*输出参数:
*返回值:失败:0
*		成功:LINK逻辑方向变化点
*备注：
************************************************/
UINT8 dquGetLinkLogicDirChangedById(const UINT16 linkId);

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
UINT8 dsuPointByLen(IN const LOD_STRU *pLODStru, IN const INT32 Displacement, OUT UINT8 *pNewLODNum, OUT LOD_STRU *pNewLODStru);

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
*			 0xff:查找到线路尽头,输出线路尽头位置
* 修订记录： 1.封装dsuLODCalcu,查找到线路尽头时返回ff，输出线路尽头位置。by xh 20220818
*/
UCHAR dsuLODCalcuNew(const LOD_STRU *pLODStru, INT32 Displacement, UCHAR PointNum, const DQU_POINT_STATUS_STRU *pPointStru, LOD_STRU *pNewLODStru);

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
UINT8 dsuGetAdjacentLogicSgmt(IN const UINT16 LogicSgmtId, IN const UINT8 Dir, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *pPointInfo, OUT UINT16 *pLogicSgmtId);

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
UINT8 dsuLogicSgmtToLink(IN const UINT16 LogicSgmtNum, IN const UINT16 *pLogicSgmtId, IN const UINT8 Dir, OUT UINT16 *pLinkNum, OUT UINT16 *pLinkId);

/************************************************
*函数功能:	查询逻辑区段端点
*输入参数:	LogicSgmtId		逻辑区段ID
*			Dir				查询方向（输入EMAP_SAME_DIR返回右侧端点，EMAP_CONVER_DIR返回左侧端点）
*输出参数:	pEndLoc			逻辑区段端点（方向返回查询方向）
*返回值:	失败:0
*			成功:1
*备注：		add by wyd 20220312
************************************************/
UINT8 dsuGetLogicSgmtEndLoc(IN const UINT16 LogicSgmtId, IN const UINT8 Dir, OUT LOD_STRU *pEndLoc);

/************************************************
*函数功能:	查询逻辑区段长度
*输入参数:	LogicSgmtId		逻辑区段ID
*输出参数:	pDisp			逻辑区段长度
*返回值:	失败:0
*			成功:1
*备注：		add by wyd 20230703
************************************************/
UINT8 dsuGetLogicSgmtDisp(IN const UINT16 LogicSgmtId, OUT UINT32 *pDisp);

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
UINT8 dsuLogicSgmtCalcuByLoc(IN const LOD_STRU LocStart, IN const LOD_STRU LocEnd, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU *pPointInfo, OUT UINT16 *pLogicSgmtNum, OUT UINT16 *pLogicSgmtId);

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
UINT8 dsuGetLogicSgmtIdByLoc(IN const LOD_STRU* pLocStru, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointInfo, OUT UINT16* pLogicSgmtId);

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
UINT8 dsuGetRouteIdByLogicSgmt(IN UINT16 LogicSgmtId, IN UINT32 sizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

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
*			适配灯泡线 kuan.he 20230728
************************************************/
UINT8 dsuGetRouteIdBuffByLoc(IN const LOD_STRU* pLocStru, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointInfo, IN UINT32 sizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

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
UINT8 dsuGetCbtcAccessSgmtIdByLogicSgmt(IN UINT16 LogicSgmtId, IN UINT32 sizeofBuff, OUT UINT8* pCbtcAccessSgmtNum, OUT UINT16* pCbtcAccessSgmtIdBuff);

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
UINT8 dsuGetLongestRouteIdByRouteIdBuff(IN const UINT8 RouteNum, IN const UINT16* pRouteIdBuff, OUT UINT16* pRouteId);

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
* 备注：	add by kuan.he 20230515
************************************************/
UINT8 dsuGetRouteIdByRouteIdBuff(IN const UINT8 RouteNum, IN const UINT16* pRouteIdBuff, IN const UINT8 PointNum, IN const DQU_POINT_STATUS_STRU* pPointStru, OUT UINT16* pRouteId);

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
UINT8 dsuGetRouteIdByAccessSgmtId(IN const UINT16 AccessSgmtId, IN const UINT32 SizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

/************************************************
*函数功能:	给定一组接近区段序列，查询接近区段关联的所有进路
*输入参数:	AccessSgmtNum		接近区段索引数量
*			AccessSgmtBuff		接近区段索引数组
*			Dir					进路方向
*			SizeofBuff			进路数组大小
*输出参数:	pRouteNum			进路数量
*			pRouteIdBuff		进路数组
*返回值:	失败:0
*			成功:1
* 使用注意：返回的进路列表已完成去重。
* 备注：	add by kuan.he 20230516
************************************************/
UINT8 dsuGetRouteIdBuffByAccessSgmtIdBuff(IN const UINT16 AccessSgmtNum, IN const UINT16* pAccessSgmtBuff, IN const UINT32 SizeofBuff, OUT UINT8* pRouteNum, OUT UINT16* pRouteIdBuff);

/************************************************
*函数功能:	给定一个进路id，查询这个进路的起点位置
*输入参数:	RouteId				进路Id
*输出参数:	pRouteStartLoc		进路起点位置
*返回值:	失败:0
*			成功:1
* 使用注意：进路起点位置方向是指进路起点指向终点的方向，非信号机防护方向
* 备注：	add by kuan.he 20230731
************************************************/
UINT8 dsuGetRouteStartLocByRouteId(IN const UINT16 RouteId, OUT LOD_STRU* pRouteStartLoc);

/************************************************
*函数功能:	给定一个进路id，查询这个进路的终点位置
*输入参数:	RouteId				进路Id
*输出参数:	pRouteStartLoc		进路终点点位置
*返回值:	失败:0
*			成功:1
* 使用注意：进路终点位置方向是指进路起点指向终点的方向，非信号机防护方向。
			如果终点位置处于变化点，则终点位置方向可能与起点位置方向不同。
* 备注：	add by kuan.he 20230731
************************************************/
UINT8 dsuGetRouteEndLocByRouteId(IN const UINT16 RouteId, OUT LOD_STRU* pRouteEndLoc);

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
UINT8 dsuGetPosBDirByPosA(IN const UINT16 wLinkA, IN const UINT32 dwLinkAOff, IN const UINT8 LinkDirA, 
	IN const UINT16 wLinkB, IN const UINT32 dwLinkBOff, IN const UCHAR PointNum, IN const DQU_POINT_STATUS_STRU *pPointStru);

/************************************************
*函数功能:	指定查找位置，按指定得位置找A、B中距离较近得点
*输入参数:	pPosFind-查找起始点（包含位置、方向）
*			pPosA、pPosB-AB两个点
*			PointNum、pPointStru-道岔信息
*输出参数:	pDist-查询成功时，返回较近点的距离
*返回值:	0:查询失败，沿指定方向未找到A、B点
*			1-A点距离起点较近
*			2-B点距离起点较近
*注意：		cjq 20230802。
*			1.如果A、B点一个可达、一个不可达，则可达得点认为是距离起点较近得点
*			2.A\B必须都在起点指定得前方，否则查询失败
*			3.如果AB点重合也会返回失败
************************************************/
UINT8 dsuGetNearPosByPoint(IN const LOD_STRU *pPosFind, IN const LOD_STRU *pPosA, IN const LOD_STRU *pPosB, UCHAR PointNum, IN const DQU_POINT_STATUS_STRU *pPointStru, INT32 *pDist);

/************************************************
*函数功能:	根据当前link的指针，查询相邻link的方向
*输入参数:	pCurLnk-当前link指针
*			CurRunDir-在当前link上的运行方向
*			NextLnkId-相邻linkID
*输出参数:	无
*返回值:	0:查询失败（传入参数不对 或者 下一link不是相邻link）
*			EMAP_SAME_DIR/EMAP_CONVER_DIR:根据当前link上的运行方向获取的下一link上的运行方向
*注意：		cjq 20230805。	
************************************************/
UINT8 dsuGetNextLnkDirByLnkStru(const DSU_LINK_STRU *pCurLnk, UINT8 CurRunDir, UINT16 NextLnkId);

/************************************************
*函数功能:	根据当前link的指针，查询相邻link的方向。有条件的可以调用【dsuGetNextLnkDirByLnkStru】，此函数性能比他略低
*输入参数:	pCurLnk-当前link指针
*			CurRunDir-在当前link上的运行方向
*			NextLnkId-相邻linkID
*输出参数:	无
*返回值:	0:查询失败（传入参数不对 或者 下一link不是相邻link）
*			EMAP_SAME_DIR/EMAP_CONVER_DIR:根据当前link上的运行方向获取的下一link上的运行方向
*注意：		cjq 20230805。	
************************************************/
UINT8 dsuGetNextLnkDirByLnkId(UINT16 CurLnkId, UINT8 CurLnkDir, UINT16 NextLnkId);

/************************************************
*函数功能:查询AB两端之间的距离并输出两点之间的方向变化点数量
*输入参数:		pLODAStru	A点
*				pLODBStru	B点
*输出参数:		pLinkNum	输出路径的link数量
*				szLinkId	输出路径的linkId序列
*				pDir		输出A->B的方向
*               pLinkDirChangedNum  AB点之间的方向变化点数量
*返回值:失败:DSU_NULL_32
*		成功:距离
*备注： add by zy 20230808
************************************************/
UINT32 dsuLenAndDirCagNumByPoint(const LOD_STRU *pLODAStru, const LOD_STRU *pLODBStru, UINT16* pLinkNum, UINT16 szLinkId[100], UINT8* pDir, UINT8* pLinkDirChangedNum);

#ifdef __cplusplus
}
#endif

#endif
