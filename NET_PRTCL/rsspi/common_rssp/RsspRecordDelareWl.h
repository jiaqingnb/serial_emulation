/********************************************************
*                                                                                                            
* 文件名  ： RsspRecordDelareWl.h 
* 版权说明： 北京瑞安时代科技有限责任公司 
* 版本号  ： 1.0
* 创建时间： 2009.12.16
* 作者    ： 车载及协议部
* 功能描述： RSR层记录相关宏定义。
* 使用注意： 
*   
* 修改记录： 
*	2010.05.10 修改		追加Rssp新周期开始标识。
*
********************************************************/ 

#ifndef RSSP_RECORD_DECLARE_WL_H
#define RSSP_RECORD_DECLARE_WL_H

#include "RsspConfigWl.h"

/********************************************************************************
宏定义
********************************************************************************/

#define	RSSP_CYCLE_START	0xEE	/*RSSP新周期开始标识*/
#define	RED_NET_DATA	0xF1	/*红网输入数据*/
#define	BLUE_NET_DATA	0xF2	/*蓝网输入数据*/
#define	RP_INPUT_LEN_ERR	0x01	/*冗余输入数据长度错误*/
#define	RP_INPUT_PKG_LEN_ERR	0x02	/*错误输入包长度*/
#define	RP_OVER_MAX_INPUT_PKG_LEN	0x03	/*超过最大输入包长度*/
#define RP_INPUT_PKG_ADDR_ERR	0x04	/*冗余输入包地址错误*/
#define	RP_INPUT_PKG_INVALID	0x05	/*冗余输入包校验失败*/
#define	RP_INPUT_STACK_NO	0x06	/*冗余输入堆栈无空间*/

#define RSSP_INPUT	0xF3	/*RSSP输入单元标识*/
#define	INPUT_LEN_ERR	0x20	/*输入数据长度错误*/
#define	INPUT_READ_ERR	0x21	/*输入数据读出队列错误*/
#define	INPUT_PKG_INVALID	0x22	/*输入包校验失败*/
#define	INPUT_STACK_NO	0x23	/*输入堆栈无空间*/
#define	INPUT_RSD_LEN_ERR	0x24	/*输入RSD帧长度错误*/
#define	NO_NODE	0x25	/*无可用节点*/
#define	INPUT_NEW_NODE_ERR	0x26	/*输入时新建节点错误*/
#define	INPUT_NEW_NODE	0x27	/*输入时新建节点*/
#define	CHK_RSD_ADDR_ERR	0x28	/*输入检查RSD地址错误*/
#define	CHK_RSD_ERR	0x29	/*输入检查RSD错误*/
#define	INPUT_TC_EQ	0x2A	/*RSD输入TC相等*/
#define	OVER_MAX_LOST	0x2B	/*RSD超过最大丢包*/
#define	OVER_MAX_TIME	0x2C	/*RSD超过最大超时*/
#define	SVC_ERR	0x2D	/*RSD的SVC校验错误*/
#define	RSD_HALT_SSE	0x2E	/*收到RSD,HALT状态触发SSE*/
#define	RSD_RCV_HALT_ERR	0x2F	/*收到RSD,接收状态错误*/
#define	SSE_LEN_ERR	0x30	/*收到SSE长度错误*/
#define	INPUT_SSE_NO_USE_NODE	0x31	/*收到SSE时,无在用的节点*/
#define	CHK_SSE_ADDR_ERR	0x32	/*输入检查SSE地址错误*/
#define	SSR_LEN_ERR	0x33	/*收到SSR长度错误*/
#define	INPUT_SSR_NO_USE_NODE	0x34	/*收到SSR时,无在用的节点*/
#define	CHK_SSR_ADDR_ERR	0x35	/*输入检查SSR地址错误*/
#define	UPD_SSR_ERR	0x36	/*更新SSR错误*/
#define	SSR_RCV_STATUS_ERR	0x37	/*收到SSR链路状态错误*/
#define	SSR_SSE_SN_ERR	0x38	/*收到SSR对应SSE序号错误*/
#define	RSD_DUP	0x39	/*RSD重复*/
#define	RSD_DEL_DATA_NODE_ERR	0x3A	/*RSD删除数据节点错误*/
#define	RSD_APP_WRITE_ERR	0x3B	/*RSD删除数据节点错误*/
#define	OLD_DATA	0x3C	/*RSD旧数据包*/
#define	INPUT_DEL_NODE_ERR	0x3D	/*输入删除节点错误*/
#define	INPUT_WRITE_ERR	0x3E	/*输入协议到应用错误*/
#define	INPUT_LNK_STATUS_ERR	0x3F	/*输入链路状态错误*/

#define RSSP_OUTPUT	0xF4	/*RSSPI输出单元标识*/
#define OUTPUT_LEN_ERR	0x60	/*输出数据长度错误*/
#define OUTPUT_READ_QUE_ERR	0x61	/*输出读取输出队列错误*/
#define OUTPUT_NEW_NODE_ERR	0x62	/*输出创建新节点错误*/
#define OUTPUT_NEW_NODE	0x63	/*输出创建新节点*/
#define OUTPUT_WRITE_NO_SPACE_ERR	0x64	/*输出写入数据无空间错误*/
#define OUTPUT_FRAME_ERR	0x65	/*输出数据帧错误*/

#define LINK_INFO	0xF5	/*RSSPI输出链路信息*/

#define GET_SVC	0xF6	/*获取RSSPI SVC*/
#define SET_SVC	0xF7	/*设置RSSPI SVC*/
#define SET_SVC_DATA_ERR 0xA0 /*设置SVC数据错误*/
#define NO_OUT_APP	0xA1 /*没有输出应用*/
#define PKG_LEN_ERR	0xA2 /*单包长度错误*/
#define FRAME_TYPE_ERR 0xA3 /*错误帧类型*/

#endif

