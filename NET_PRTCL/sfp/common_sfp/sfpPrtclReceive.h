
/*******************************************************************************
* 文件名    ：  sfpPrtclReceive.h
* 版权说明  ：  
* 版本号    ：  1.0  
* 创建时间  ：  2008.07.29
* 作者      ：  
* 功能描述  ：  协议输入单元头文件
* 使用注意  ： 
                无
* 修改记录  ：  
    2008.7.8
        1.将函数名中设计type以及ID的改为name形式
        2.增加DevNameSNcmp函数
*******************************************************************************/

#ifndef SFPPRTCLRECEIVE_H
#define SFPPRTCLRECEIVE_H

#include "sfpplatform.h"
#include "sfpLinkManage.h"
#include "dsuRelatedDeal.h"
#include "sfpDeclare.h"
#include "CommonQueue.h"
#include "Convert.h"
#include "CRC32.h"
#include "sfpPrtclCommon.h"


#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************************
函数定义
********************************************************************************/
/******************************************************************************************
* 功能描述      : Redundancy()是主机协议输入模块的一个主函数，负责冗余数据的处理
                  该函数中又包含一个子函数：读取数据并进行冗余处理的函数RedundancyReceive()
* 输入参数      :
* 输入输出参数  : struc_Unify_Info *pUnifyInfo    统一结构体指针
* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0        
*******************************************************************************************/
static UINT8 Redundancy(struc_Unify_Info *pUnifyInfo);



/******************************************************************************************
* 功能描述      : 协议冗余处理的初始化
* 输入参数      :
* 输入输出参数  : struc_Unify_Info *pUnifyInfo 统一结构体指针
* 输出参数      :
* 全局变量      :                 
* 返回值        : 成功返回1,失败返回0        
*******************************************************************************************/
UINT8 RedundancyInitial(struc_Unify_Info *pUnifyInfo);

/*
*函数名:	RedundancyReceive
*功能描述:	接收数据，并进行冗余处理
*				1、从队列中读出每一帧数据
*				2、判断本方与对方的逻辑地址是否合法
*				3、判断CRC是否正确
*				4、判断报方类型。
*				5、收到RFC帧，查链路管理表，如果存在，根据链路管理表判断该RFC是否合法，如果不存在，插入一维链表。
*				6、收到ACK帧，通过链路管理表判断该ACK是否合法，如果合法，通过一维链表判断是否是冗余帧
*				7、收到DATA帧，通过链路管理表判断该DATA是否合法，如果合法，通过一维链表判断是否是冗余帧
*输入参数:	struc_Unify_Info *pUnifyInfoForRedun	SFP结构体
*输入出参:	无
*输出参数:	无
*返回值:	1:成功;0失败
*/
static UINT16 RedundancyReceive(struc_Unify_Info *pUnifyInfoForRedun);

/*
*函数名:	SearchGroupNode
*功能描述:	在节点存储表里查询某设备下的节点，完成定位链路的功能
*输入参数:	UINT32 DeviceName	对方设备名
*			struc_Unify_Info *pUnifyInfoForRedun	SFP结构体
*输入出参:	无
*输出参数:	无
*返回值:	1:成功;0失败
*/
static struc_InputBuffer* SearchGroupNode(UINT32 DeviceName, struc_Unify_Info *pUnifyInfoForRedun);



/******************************************************************************************
* 功能描述      : 将指定的节点插入到节点存储表表中规定的位置
* 输入参数      : UINT32 DestDevName           对方设备名
*                 UINT8  *DestSN               对方设备SN
*                 UINT8  *Inputarray           输入数组
* 输入输出参数  : struc_Unify_Info *pUnifyInfo    与操作链表对应对象的统一结构体
* 输出参数      :
* 全局变量      :
* 返回值        : 新插入节点的指针        
*******************************************************************************************/
static struc_InputBuffer* InsertNode(UINT32 DestDevName,UINT8 *DestSN,UINT8 *Inputarray,struc_Unify_Info *pUnifyInfo);


/******************************************************************************************
* 功能描述      : 将输入数组的数据填入当前节点中
* 输入参数      : UINT8   *a                                  数组信息
* 输入输出参数  : struc_InputBuffer   *p                      要插入的节点
* 输出参数      :
* 全局变量      :
* 返回值        : 无        
*******************************************************************************************/
static void BuildNode( struc_InputBuffer *p,UINT8 *a);

/******************************************************************************************
* 功能描述      : 删除冗余处理节点存储表没有时效性的节点,
*                 如果该节点没有时效性，那么该函数将删除该节点及其以后的没有时效性的SN节点
* 输入参数      : struc_InputBuffer   *NodeIndex             当前要删除的指针
* 输入输出参数  : struc_Unify_Info    *pUnifyInfo            需要删除的节点所属的通信对象对应的统一结构体
* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0        
*******************************************************************************************/
static UINT8 DelGroupNode(struc_InputBuffer *NodeIndex,struc_Unify_Info *pUnifyInfo);
            


/***********************************其它函数************************************/

/*
*函数名:	JudgeCrc
*功能描述:	判断CRC,主机这里对接收来的数据作CRC判断,如果是数据帧或者IDLE帧则在数据帧中要加入RFC和ACK的SN
*输入参数:	UINT8 Rcv[]	收到的数据
*			struc_Unify_Info *pUnifyInfoForRedun	SFP结构体
*输入出参:	无
*输出参数:	无
*返回值:	1:成功;0失败
*/
static UINT16 JudgeCrc(UINT8 Rcv[], struc_Unify_Info *pUnifyInfoForRedun);

/******************************************************************************************
* 功能描述      : 冗余处理判断帧的实效性.判断序列号是否合法满足，即符合：
*                  1.收到的本机序列号要小于当前的序列号，并且应当大于超时前发送的最后一个数据的本机序列号
*                  2.受到的对方序列号要大于当前的对方序列号还要小于超时周期内发送的最大序列号
*                  3.有数组溢出的处理;
* 输入参数      : struc_Lnk_Mngr *lnk_index                   链路管理表中的链路编号
*                 UINT32 local_sn                             本机的序列号
*                 UINT32 dest_sn                              对方的序列号
*				  UINT8 *pRecord							  日志数组
*				  UINT16 recordMaxLen						  日志数组最大长度
* 输入输出参数  : 
* 输出参数      :
* 全局变量      :
* 返回值        : 如果序列号合法返回1，否则返回0        
*******************************************************************************************/
static UINT32 JudgeSN(struc_Lnk_Mngr *lnk_index, UINT32 local_sn, UINT32 dest_sn, UINT8 *pRecord, UINT16 recordMaxLen);



/******************************************************************************************
* 功能描述      : 比较DeviceName+SN的大小：比较入口中DeviceName+SN组合后的数据的大小。设备DeviceName在前，Sn在后
* 输入参数      : UINT32 DeviceNameA                          设备A的Name
*                 UINT8 SnA[]                                 设备A的SN
*                 UINT32 DeviceNameB                          设备B的Name
*                 UINT8 SnB[]                                 设备B的SN
* 输入输出参数  : 
* 输出参数      :
* 全局变量      :
* 返回值        : 如果a > b返回1;如果a == b，返回 0;如果a < b返回-1        
*******************************************************************************************/
static INT8 DevNameSNcmp(UINT32 DeviceNameA,UINT8 SnA[],UINT32 DeviceNameB,UINT8 SnB[]);




/******************************************************************************************
* 功能描述      : 协议输入模块的输入处理主函数
                    1. 首先把冗余单元给输入单元的接口数组取最新，把旧的数据去掉；
                    2. 根据每一帧数据修改冗余链表，删除该节点其及同type和ID下后面的节点；
                    3. 修改一维链表结束后，开始修改链路管理表的相关信息，若帧类型为DATA，将数据写入输入给应用的队列中；
                    4. 数据处理结束后，开始遍历链路管理表，处理超时，若为跟随方且超时，则删除链路；
* 输入参数      :
* 输入输出参数  : struc_Unify_Info *pUnifyInfo        协议使用的统一结构体指针
* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0        
*******************************************************************************************/
static UINT8 Prtcl_Receive(struc_Unify_Info *pUnifyInfo);



/******************************************************************************************
* 功能描述      : 协议输入处理的初始化
                   1. 对统一结构体中的输入单元给应用的接口队列进行初始化；
* 输入参数      :
* 输入输出参数  : struc_Unify_Info *pUnifyInfoForRcv          协议使用的统一结构体指针
* 输出参数      :
* 全局变量      :
* 返回值        : 成功返回1,失败返回0        
*******************************************************************************************/
UINT8 Prtcl_RecInitial(struc_Unify_Info *pUnifyInfo);

#ifdef __cplusplus
}
#endif

#else

#endif

