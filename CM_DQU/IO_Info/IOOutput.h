/****************************************************************************************************
* 文件名   :  IOOutput.h
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2018.08.23
* 作者     :
* 功能描述 :  IO输出模块处理头文件
* 使用注意 :
*
* 修改记录 :
*
**************************************************************************************************/
#ifndef IO_OUTPUT_H_
#define IO_OUTPUT_H_

#include "CM_Types.h"
#include "dquVobcConfigData.h"
#include "IOModuleExtern.h"
#include "IOConfig.h"

/*350项目*/
#if(1 == H_FAO_PROJECT_USER)
#define DO_DATA_OUTPUT_BUF_SIZE		42U			/*DO输出缓冲*/
#define OUTPUT_FLAG_NUM				63U			/*安全和非安全输出的总共数目*/
#define OUTPUT_ATO_NUM              9U          /*ATO 非安全输出表总数目，用于计算掩码*/
#endif
/*朔黄项目*/
#if(1 == HHTC_PROJECT_USER)
#define DO_DATA_OUTPUT_BUF_SIZE	12U		/*DO输出缓冲*/
#define OUTPUT_FLAG_NUM			30	/* 安全和非安全输出的总共数目*/
#endif
/*车车*/
#if(1 == CC_PROJECT_USER)
#define DO_DATA_OUTPUT_BUF_SIZE	42U		/*DO输出缓冲-对应输出的打包长度*/
#define OUTPUT_FLAG_NUM			52	/* 安全和非安全输出的总共数目*/
#endif
#define TRAIN_OUTPUT_BOARD			0x02		/*插槽为输出板*/

#define OUT_SW_INVALID_VALUE		0x0000		/*无效的开关量输出*/
#define OUT_ANA_INVALID_VALUE		0xFFFFFFFF	/*无效的模拟量输出*/
#define OUT_PWM_FREQUENCY			500		    /*PWM输出的频率*/
#define OUT_PWM_INVALID_VALUE		0xFFFFFFFF	/*无效的PWM量输出*/

/* IO输出需要用到的卡槽结构体 */
typedef struct
{
	UINT8 IoOutSlotNum; 					/* 输出所需要用到的插槽数量 */
	UINT16 IoOutSlotId[BOARD_CONFIG_NUM]; 	/* 输出的插槽ID */
	UINT8 SendDataBuf[BOARD_CONFIG_NUM][DO_DATA_OUTPUT_BUF_SIZE]; /* 发送的数据信息 */
}IO_OUTPUT_DATA_STRU;

/* 输出根据开关量含义查询的结构体 */
typedef struct
{
	UINT8 * IoOutputStruIndex; /* 索引相匹配的输出结构体中的变量指针 */
	UINT8 IoOutputTypeIndex; /* 根据索引查询表中的索引找到对应的输出量 */
}IO_OUTPUT_INDEX;

/***************************************************************************************
* 功能描述:			根据IO索引将相应的IO输出的bit位存放到字节中
* 输入参数:			UINT8 Index 相关索引
*					UINT8 OutValue 输出的相关索引上的bit位的值
* 输入输出参数:     无
* 输出参数:			UINT32 *pOutUnSafeData 将结构体中的数据转化为非安全输出数据流的指针
*					UINT16 *pOutSafeData   将结构体中的数据转化为安全输出数据流的指针
* 全局变量:         无
* 返回值:           0x00：成功；
*					0x01:传入参数不正确
*					0x02:索引值不正确
****************************************************************************************/
UINT8 IoModuleBitToByte(UINT8 Index, UINT8 OutValue, UINT32 *pOutUnSafeData, UINT32 *pOutSafeData);

#endif
