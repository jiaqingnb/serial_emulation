/********************************************************
*                                                                                                            
* 文件名  ： RsspWl.h   
* 版权说明： 北京瑞安时代科技有限责任公司 
* 版本号  ： 1.0
* 创建时间： 2009.10.25
* 作者    ： 车载及协议部
* 功能描述： Rsr层协议实现  
* 使用注意： 
*   
* 修改记录：   
*
********************************************************/ 

#ifndef RSSPWL_H
#define RSSPWL_H

#include "CommonTypes.h"
#include "CommonQueue.h" 
#include "RsspStructWl.h"
#include "RsspCommonWl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------
 * RSSP层对外接口函数申明 Start
 *--------------------------------------------------------------------*/
/***********************************************************************
 * 方法名   : Rssp2Receive
 * 功能描述 : 对红蓝网输入数据进行RSSP层的安全处理，将安全数据交付应用。
 * 输入参数 : 
 *	参数名		类型				输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP统一结构体
 *											输入队列：pRsspStruct->OutnetQueueA				
 *													 pRsspStruct->OutnetQueueB
 *											输出队列：pRsspStruct->DataToApp
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 Rssp2Receive(RSSP_INFO_STRU *pRsspStruct);

/***********************************************************************
 * 方法名   : Rssp2Output
 * 功能描述 : 对应用输出数据进行RSSP层的安全处理并输出。
 * 输入参数 : 
 *	参数名		类型				输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体
 *											输入队列：pRsspStruct->OutputDataQueue
 *											输出队列：pRsspStruct->AppArray
 *													 pRsspStruct->VarArray
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 Rssp2Output(RSSP_INFO_STRU *pRsspStruct);

/*
*函数名:	Rssp2Init
*功能描述:	RSSPI协议初始化
*输入参数:	UINT8 *pFsName	FS文件
*输入出参:	RSSP_INFO_STRU *pRsspStruct	RSSPI信息
*输出参数:	无
*返回值:	1:成功;0失败
*/
UINT8 Rssp2Init(UINT8 *pFsName, UINT32 inParamLen, UINT8 inParam[], RSSP_INFO_STRU *pRsspStruct);

/***********************************************************************
 * 方法名   : Rssp2FreeSpace
 * 功能描述 : 通过该函数，释放RSSP层开辟的空间。
 * 输入参数 : 
 *	参数名		类型				输入输出		描述
 *  --------------------------------------------------------------
 *	pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体，需进行初始值设置。
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 Rssp2FreeSpace(RSSP_INFO_STRU *pRsspStruct);


/***********************************************************************
 * 方法名   : Rssp2LnkStatus
 * 功能描述 : 得到某个安全接受节点的在本周期得通信状态：
 * 					 存在某个安全接受节点，则返回该安全节点的通信状态
 * 					 不存在某个安全接收节点，则返回错误
 * 输入参数 : 
 *	参数名			类型				输入输出			描述
 *  -------------------------------------------------------------
 *	destType  		UINT8			IN  			目标类型
 *  destId			UINT8			IN			    目标ID
 *	logicId			UINT16			IN				目标逻辑ID
 *  pRsspStruct  RSSP_INFO_STRU*	INOUT		RSSP层统一结构体，需进行初始值设置。
 * 返回值   : UINT8  	   0：不存在该条链路，返回错误；
 *						0x17：链路在数据传输正常状态DATA
 *	  					0x2b：链路在无法传输数据状态HALT
 ***********************************************************************/
UINT8 Rssp2LnkStatus(UINT8 destType, UINT8 destId, UINT16 logicId, RSSP_INFO_STRU *pRsspStruct);

/*
 *函数名:	Rssp2LnkDelete
 *功能描述:	通过对方类型ID+对方逻辑ID删除链路
 *输入参数:	UINT8 dstType	对方类型
 *			UINT8 dstId	对方ID
 *			UINT16 dstLogicId	对方逻辑ID
 *			RSSP_INFO_STRU *pRsspStruct	RSSPI配置信息
 *输入出参:	无
 *输出参数:	无
 *返回值:	1:成功;0;失败
 */
UINT8 Rssp2LnkDelete(UINT8 dstType, UINT8 dstId, UINT16 dstLogicId, RSSP_INFO_STRU *pRsspStruct);

/***********************************************************************
 * 方法名   : Rssp2Refresh
 * 功能描述 : 应用使用该函数来重新对本机RSSP层的中间变量重新赋值。
 * 输入参数 : 
 *	参数名				类型			输入输出		描述
 *  --------------------------------------------------------------
 *	sLen_Input  		UINT16		IN			中间数组的大小
 *	Input_Data_LnkMes	UINT8*		IN			中间变量数组
 * 
 * 返回值   : UINT8  1：正常返回  0：错误返回
 ***********************************************************************/
UINT8 Rssp2Refresh(UINT16 sLen_Input, UINT8* Input_Data_LnkMes,RSSP_INFO_STRU *pRsspStruct);

/*******************************************************************************************************
* 功能描述: 	   设置RSSP协议无发送应用数据周期数
* 输入参数: 		pNoSendAppData 无发送次收
*					pRsspStruct	RSSPI结构
* 输入输出参数: 	无
* 输出参数: 		无
* 返回值:			1:成功
*******************************************************************************************************/
UINT8 SetRsspNoSendAppData(UINT8 pNoSendAppData, RSSP_INFO_STRU *pRsspStruct);

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
void RsspiGetNetsRecvPkgNum(RSSP_INFO_STRU* rsspi, const UINT8 dstType, const UINT8 dstId, const  UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);

/*
*函数名:	RsspiGetSwapData
*功能描述:	获取输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据(2字节长度+N个[4字节SVC])
*           const UINT16 swapDataMaxLen 交互数据最大长度
*           RSSP_INFO_STRU* rsspi SFP结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiGetSwapData(UINT8 swapData[], const UINT16 swapDataMaxLen, RSSP_INFO_STRU* rsspi);

/*
*函数名:	RsspiSetSwapData
*功能描述:	设置输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据(2字节长度+N个[4字节SVC])
*			RSSP_INFO_STRU* rsspi SFP结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiSetSwapData(UINT8 swapData[], RSSP_INFO_STRU* rsspi);

/*
*函数名:	RsspiSetSwapDataForOut
*功能描述:	设置输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据(2字节长度+N个[4字节SVC])
*           UINT8 outData[] 指定输出数据
*			RSSP_INFO_STRU* rsspi SFP结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiSetSwapDataForOut(UINT8 swapData[], UINT8 outData[], RSSP_INFO_STRU* rsspi);

/*
*函数名:	RsspiDelAllLink
*功能描述:	RSSPI删除所有链路
*输入参数:	RSSP_INFO_STRU* rsspi RSSPI结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void RsspiDelAllLink(RSSP_INFO_STRU* rsspi);

/*--------------------------------------------------------------------
 * RSSP层对外接口函数申明 End
 *--------------------------------------------------------------------*/
 
#ifdef __cplusplus
}
#endif


#endif

