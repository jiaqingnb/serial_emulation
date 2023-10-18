/********************************************************
*                                                                                                            
* 文件名  ： RsspIFWl.h   
* 版权说明： 北京瑞安时代科技有限责任公司 
* 版本号  ： 1.0
* 创建时间： 2009.11.25
* 作者    ： 车载及协议部
* 功能描述： Rssp层对外接口定义 
* 使用注意： 
*   
* 修改记录：   
*
********************************************************/ 

#ifndef RSSP_IFWL_H
#define RSSP_IFWL_H

#include "CommonTypes.h"
#include "CommonQueue.h" 
#include "RsspWl.h"
#include "RsspStructWl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
*函数名:	RsspReceive_WL
*功能描述:	RSSPI输入处理
*输入参数:	RSSP_INFO_STRU* rssp RSSPI结构
*输入出参:	无
*输出参数:	无
*返回值:	是否成功,0:不成功,1:成功
*/
UINT8 RsspReceive_WL(RSSP_INFO_STRU* rssp);

/***********************************************************************
 * 方法名   : RsspOutput_WL
 * 功能描述 : 本函数将从应用层接受的数据，进行安全数据处理后提交给冗余处理。
 *          同时根据通信节点的链路状态作成SSE，SSR报文。
 * 输入参数 : 
 *  参数名     类型          输入输出    描述
 *  --------------------------------------------------------------
 *  RpToRs      QueueStruct*    IN          冗余处理到Rssp层的输入队列
 *  varArray    UINT8*          OUT         中间变量数组

 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 RsspOutput_WL(QueueStruct *AppToRs,UINT8 *VarArray,RSSP_INFO_STRU *pRsspStru);

/***********************************************************************
 * 方法名   : RsspInit_WL
 * 功能描述 : 通过该函数，实现对RSSP层的初始化函数。根据应用设定的基本值，计算各队列的大小。
 * 输入参数 : 
 *	参数名				类型							输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT			RSSP层统一结构体，需进行初始值设置。
 * 
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 RsspInit_WL(RSSP_INFO_STRU *pRsspStruct);

/***********************************************************************
 * 方法名   : RsspFreeSpace_WL
 * 功能描述 : 通过该函数，释放RSSP层开辟的空间。
 * 输入参数 : 
 *	参数名				类型							输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT			RSSP层统一结构体，需进行初始值设置。
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 RsspFreeSpace_WL(RSSP_INFO_STRU *pRsspStruct);

/*
*函数名:	RsspLnkStatus_WL
*功能描述:	得到某个安全接受节点的在本周期得通信状态：
* 				存在某个安全接受节点，则返回该安全节点的通信状态
* 				不存在某个安全接收节点，则返回错误
*输入参数:	UINT8 dstType		目的类型
*			UINT8 dstId			目的ID
*			UINT16 dstLogicId	目的逻辑ID
*			RSSP_INFO_STRU *pRsspi	RSSPI信息
*输入出参:	无
*输出参数:	链路状态 0x17:接收数据,0x2b:通信不正常
*返回值:	无
*/
UINT8 RsspLnkStatus_WL(UINT8 dstType, UINT8 dstId, UINT16 dstLogicId, RSSP_INFO_STRU *pRsspi);

/***********************************************************************
 * 方法名   : RsspLnkDelete_WL
 * 功能描述 : 通过该函数，应用可以删除某个发送节点。当应用不再需要对某个对象发送数据时，利用该函数删除相应的发送节点。
 * 输入参数 : 
 *	参数名				类型		输入输出		描述
 *  --------------------------------------------------------------
 *  devName         UINT32*         OUT             设备名称
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 RsspLnkDelete_WL(UINT32 destDevName,RSSP_INFO_STRU *pRsspStruct);

/***********************************************************************
 * 方法名   : RsspRefresh_WL
 * 功能描述 : 应用使用该函数来重新对本机RSSP层的中间变量重新赋值。
 * 输入参数 : 
 *	参数名		类型			输入输出	描述
 *  --------------------------------------------------------------
 *	VarLen  	UINT16			IN			中间数组的大小
 *	VarArray	UINT8*			IN			中间变量数组

 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 RsspRefresh_WL(UINT16 VarLen, const UINT8 *VarArray, RSSP_INFO_STRU *pRsspStruct);

/*
*功能描述:	获取RSSPI各网收到包数
*输入参数:	RSSP_INFO_STRU* rsspi rsspi变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
void GetNetsRecvPkgNum_WL(RSSP_INFO_STRU* rsspi, const UINT16 dstAddr, UINT16* redPkgNum, UINT16* bluePkgNum);

/*--------------------------------------------------------------------
 * RSSP接口函数申明 End
 *--------------------------------------------------------------------*/
 
#ifdef __cplusplus
}
#endif


#endif

