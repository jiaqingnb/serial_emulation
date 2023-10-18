/*
*
* 文件名   ： ProtclInterFun.h
* 版权说明 ：  北京北京交控科技有限公司 
* 版本号   ：  1.0
* 创建时间 ：  2013.09.15
* 作者     ：  王佩佩
* 功能描述 ：  协议主处理类，主要实现了周期性调用协议。
* 使用注意 ：  
* 修改记录 ：
*    时间			修改人		修改理由
*---------------------------------------------
* 1  2011/06/07	   楼宇伟		协议基类
*/
#ifndef PROTCL_Inter_Fun_H
#define PROTCL_Inter_Fun_H

#include "CommonTypes.h"
#include "CommonQueue.h"
#include "protclStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

	/*
* 协议初始化模块
* 参数：
* 输入参数：    char*  fileName         存放数据的地址
*				UINT32 inDataLen 内部参数长度
*				UINT8 inData[] 内部参数
*    			ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*   1 ：初始化成功
*   0: 初始化失败
*/
UINT8 ProtclInitFunc(UINT8* fileName, UINT32 inDataLen, UINT8 inData[], ProtclConfigInfoStru* pUnifyInfo);

/*
*函数名:	ProtclEndFunc
*功能描述:	适配层销毁清理协议
*输入参数:	ProtclConfigInfoStru *pUnifyInfo	适配层协议统一结构体
*输入出参:	无
*输出参数:	无
*返回值:	1;销毁成功,0:销毁失败
*/
UINT8 ProtclEndFunc(ProtclConfigInfoStru *pUnifyInfo);

/*
* 功能：协议接收数组
* 参数：
* 输入输出参数：ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    1：接收数据成功
0：失败
*/
UINT8 ProtclReceiveDataFunc (ProtclConfigInfoStru * pUnifyInfo);

/*
* 功能：协议发送数据
*
* 参数： 
* 输入输出参数：  ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    UINT8 1 正常，其他不正常
*/

UINT8 ProtclSendDataFunc(ProtclConfigInfoStru * pUnifyInfo);


/*
* 功能：删除链路
*
* 参数： 
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      动态ID
*             ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值：
*    UINT8 1 删链成功
*          0 删链失败
*/

UINT8 ProtclDeleteLnkFunc(UINT8 DestType,UINT8 DestID,UINT16 logId,ProtclConfigInfoStru * pUnifyInfo);

/*
* 功能：查询链路状态
*
* 参数： 
* 输入参数：  UINT8 DestType    对方设备类型
* 			  UINT8 DestID      对方设备ID
*             UINT16 logId      若卡斯柯的CI与VOBC通信，需要填写VOBC的站台号。其他的系统填0。
*             ProtclConfigInfoStru * pUnifyInfo   统一结构体
* 返回值： 
*     0：不存在该条链路，返回错误
*     0x17：链路在数据传输正常状态
*     0x2b：链路在通信中断状态
*     0x33：链路在准备建立链接状态
*     0x4e：链路在等待建立链接状态
*     0x01: 表示刷新无用的链路
*/
UINT8 ProtclGetLnkStatus (UINT8 destType,UINT8 destId,UINT16 logId,ProtclConfigInfoStru * pUnifyInfo);

/*
* 功能：刷新链路状态
*
* 参数： 
* 输入参数： 	 UINT8* pLnkStatusData     协议状态数据
* 返回值： 
*     0：刷新链路失败
*     1：刷新链路成功 
*/
UINT8 ProtclReFreshFunc(UINT8* pLnkStatusData, ProtclConfigInfoStru * pUnifyInfo);

/************************************************************************
功能：向协议层输入TCP链路状态信息
描述: 使用TCP通信（如RSSP2协议）时，用户需要每周期调用且仅调用一次本函数,
    为协议层更新TCP链路状态信息，注意，建议先调用本函数，后调用协议接收函
	数ProtclReceiveDataFunc()。否则可能导致本周期的协议接收函数无法正确工
	作，极端情况下会因为未能正确接收AU1消息导致建链时间延长
输入参数：
	UINT16 TcpStateDataLen,  TCP状态报文字节数
	UINT8* pTcpStateData	TCP状态报文
返回值：0失败 1成功
    TCP链路状态报文数据结构：
        长度（不含自身）   |               节点1                                |  节点2  |  ...
             2字节         |  远端设备互联互通编号ETCSID(4字节)  状态字(1字节)  |  ...    |  ...
    状态字数据结构：
	RRRRBBBB 高4位为红网状态信息，低4位为蓝网状态信息（状态信息：0无 1建链中 2已连接 3断开中 4已断开）
************************************************************************/
UINT8 ProtclReceiveTcpStateFunc(UINT16 TcpStateDataLen,UINT8* pTcpStateData, ProtclConfigInfoStru *pUnifyInfo);

/************************************************************************
功能：协议层输出TCP链路控制信息
描述：使用TCP通信（如RSSP2协议）时，用户需要每周期调用且仅调用一次本函数,
    获取协议层输出的TCP链路控制信息，并将该信息透明转发给2个通信控制器，
	注意，建议先向通信控制器发送TCP链路控制信息，后发送通信报文。
输出参数:
	UINT16 *pTcpCtrlDataLen, TCP控制信息字节数
	UINT8 *pTcpCtrlData      TCP控制信息
返回值：0失败 1成功
	TCP链路控制报文数据结构：
		长度（不含自身）   |               节点1                               |  节点2  |  ...
		     2字节         |  远端设备互联互通编号ETCSID(4字节) 控制字(1字节)  |  ...    |  ...
	控制字数据结构：
		RRRRBBBB 高4位为红网控制指令，低4位为蓝网控制指令（控制指令：0无 1建链 2保持连接 3断开）
************************************************************************/
UINT8 ProtclSendTcpCtrlFunc(UINT16 *pTcpCtrlDataLen,UINT8 *pTcpCtrlData, ProtclConfigInfoStru *pUnifyInfo);

/*
*函数名:	AdjustRsspiVarArray
*功能描述:	调整RSSPI链路,只保留指定的对方地址链路
*输入参数:	UINT8 oppIdsNum 对方类型ID逻辑ID数量
*			const UINT16 oppTypeIds[] 对方类型ID数组
*			const UINT16 oppLogicIds[] 对方逻辑ID数组
*			ProtclConfigInfoStru *pUnifyInfo 适配结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void AdjustRsspiVarArray(UINT8 oppIdsNum, const UINT16 oppTypeIds[], const UINT16 oppLogicIds[], ProtclConfigInfoStru *pUnifyInfo);

/*
*功能描述:	获取各网收到包数
*输入参数:	ProtclConfigInfoStru* pUnifyInfo 适配层变量
*			const UINT8 dstType	目的设备类型
*			const UINT8 dstId 目的设备ID
*			const UINT16 dstLogicId 目的逻辑ID
*输入出参:	UINT16* redRecvPkgNum 红网收到包数
*			UINT16* blueRecvPkgNum 篮网收到包数
*输出参数:	无
*返回值:	无
*/
void ProtclGetNetsRecvPkgNumFunc(ProtclConfigInfoStru* pUnifyInfo, const UINT8 dstType, const UINT8 dstId, const UINT16 dstLogicId, UINT16* redRecvPkgNum, UINT16* blueRecvPkgNum);

/*
*函数名:	ProtclGetSwapData
*功能描述:	获取输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据 2字节总长度+SFP(2字节长度+N个[4字节CRC])+RSSPI(2字节长度+N个[4字节SVC]) 空间大于2046
*           const UINT16 swapDataMaxLen 交互数据最大长度 大于2046
*           ProtclConfigInfoStru* protcl 协议结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void ProtclGetSwapData(UINT8 swapData[], const UINT16 swapDataMaxLen, ProtclConfigInfoStru* protcl);

/*
*函数名:	ProtclSetSwapData
*功能描述:	设置输出数据中双CPU交互数据
*输入参数:	UINT8 swapData[] 交互数据 2字节总长度+SFP(2字节长度+N个[4字节CRC])+RSSPI(2字节长度+N个[4字节SVC])
*			ProtclConfigInfoStru* protcl 协议结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void ProtclSetSwapData(UINT8 swapData[], ProtclConfigInfoStru* protcl);

/*
*函数名:	ProtclDelAllLink
*功能描述:	适配层删除所有链路
*输入参数:	ProtclConfigInfoStru* protcl 协议结构体变量
*输入出参:	无
*输出参数:	无
*返回值:	无
*/
void ProtclDelAllLink(ProtclConfigInfoStru* protcl);

#ifdef __cplusplus
}
#endif

#endif
