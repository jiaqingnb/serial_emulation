/****************************************************************************************************
* 文件名   :  IOInput.h
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2018.08.23
* 作者     :
* 功能描述 :  IO输入解析处理头文件
* 使用注意 :
*
* 修改记录 :
*
**************************************************************************************************/
#ifndef  IO_INPUT_H_
#define  IO_INPUT_H_

#include "IOModuleExtern.h"
#include "IOConfig.h"


#define MAX_SAFE_CHANNELS_NUM		15U		/*0~14共15路，对应安全输入的1-15路*/
#define MAX_UNSAFE_CHANNELS_NUM		24U		/*0~23共24路，对应非安全输入的1-24路*/

#define MAX_CHANNELS_NUM	        48U            /*单节点采集最大采集数量，两块SID最大值*/
#define MAX_SAFE_INPUT_NUM          26U            /*目前安全输入最大数量*/

/* 输入板和输出板标志 */
#define TRAIN_INPUT_BOARD							(UINT8)0x01		/*插槽为输入板*/

/* 输入根据开关量含义查询的结构体 */
typedef struct
{
	UINT8 IoInputTypeIndex; /* 根据索引查询表中的索引找到对应的输入量 */
	UINT8 *IoInputStruIndex; /* 索引相匹配的输入结构体中的变量指针 */
}IO_INPUT_INDEX;

extern IO_CFG_DATA_STRU gIoConfigDataStru;		/* IO配置数据结构体(输入和输出共用一个结构体) */

#define GET_BIT_FROM_INPUT_MSCP(x, y)		((((x)>>(y))&(0x01U)) ? IO_TRUE_MSCP: IO_FALSE_MSCP)
#define OPEN_VALID_MSCP		(UINT8)0x55
#define CLOOSE_VALID_MSCP	(UINT8)0xAA

typedef struct 
{
	UINT8   TimeLock;                        /*时间锁*/
	UINT32  LastStateTime;                  /*上周期的失效的情况*/
} DIGITAL_IO_Time_MSCP;

/***************************************************************************************
* 功能描述:			根据插箱ID查找对应插箱输入信息配置数
* 输入参数:			UINT16 PlugBoxId 插箱ID
*					IO_CFG_DATA_STRU *pDataConfigStru  IO配置数据结构体指针
* 输入输出参数:		无
* 输出参数:			UINT8 *pFindIndex  根据插箱ID查找到的在配置结构体中的ID
* 全局变量:
* 返回值:			0x00：成功；
*					0x01:根据插箱ID未找到相关索引
*					0x02:传入参数不正确
* 修改记录：
****************************************************************************************/
UINT8 IoModuleInputFindConfigDataFromPlugBoxId(UINT16 PlugBoxId, IO_CFG_DATA_STRU *pDataConfigStru, UINT8 *pFindIndex);

/***************************************************************************************
* 功能描述:			将接收的两端数据结构体给到IOHHUser模块
* 输入参数:			无
* 输入输出参数:
* 输出参数:			IO_INPUT_STRUCT        *pTc1HHIoInputStru;                从1端接收到的IO数据的结构体
*					IO_INPUT_STRUCT        *pTc2HHIoInputStru;                从2端接收到的IO数据的结构体
* 全局变量:			无
* 返回值:			0x00：成功；
*					FUNC_CODE1:传入指针失败
* 修改记录：
****************************************************************************************/
UINT8 IoModuleInPut_Api_GetIOInputStru(IO_INPUT_STRUCT *pTc1HHIoInputStru, IO_INPUT_STRUCT *pTc2HHIoInputStru);

/***************************************************************************************
* 功能描述:			将接收的两端数据数据流给到IOWindPress模块
* 输入参数:			无
* 输入输出参数:
* 输出参数:			UINT8 *pTc1IOBuff, 1端数据流
*					UINT8 *pTc2IOBuff 2端数据流
* 全局变量:			无
* 返回值:			0x00：成功；
*					FUNC_CODE1:传入指针失败
* 修改记录：
****************************************************************************************/
UINT8 IoModuleInPut_Api_GetBuffStru(UINT8 *pTc1IOBuff, UINT8 *pTc2IOBuff);

/***************************************************************************************
* 功能描述:			IO输入模块需要初始化的值以及读取IO配置的数据，增加映射与数据的映射关系
* 输入参数:			无
* 输入输出参数:		无
* 输出参数:
* 全局变量:
* 返回值:			0x00:初始化成功
*					0x01:读取IO配置数据失败
* 修改记录：		added by Jielu 20230406
****************************************************************************************/
UINT8 IoModuleMscp_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru);

#endif
