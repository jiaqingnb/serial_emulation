/****************************************************************************************************
* 文件名   :  IOModuleVbtcExtern.h
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2020.05.07
* 作者     :
* 功能描述 :  IO模块接口（含输入、输出、IO索引及结构）
* 使用注意 :
*
* 修改记录 :
*
**************************************************************************************************/
#ifndef IO_MODULE_VBTC_EXTERN_H_
#define IO_MODULE_VBTC_EXTERN_H_

#include "App_ComStru.h"
#include "IOModuleExtern.h"

typedef struct
{
	UINT8  IVOCCtrlType;					/*列车控车属性*/
	UINT8  IvocTCAttri;						/*列车本端属性*/
	UINT8  LineConfig;						/*获取配置数据信息——线路配置信息*/
	UINT8  DoorCloseLockDivTag;             /*车门关闭且锁闭是否分开采集*/
	UINT8  DsnModeIoInputCycle;             /*迪士尼驾驶台模式旋钮容忍周期*/
	IO_CFG_DATA_STRU IoConfigDataStru;		/*IO配置数据信息结构体*/
}IO_CFG_STRU;

/*IO输入模块输入结构体*/
typedef struct
{
	UINT8 gOtherVobcAttri;					/*对端是否为控制端*/
}IO_IN_VBTC_PERIOD_INPUT_STRUCT;

/*IO输入模块的输出信息结构体*/
typedef struct
{
	UINT8 MainCtrSide;				/*控制端*/
	UINT8 IdleSide;					/*等待端*/
	/*当列车配置为一机控一端时用以下两字节，当一机控两端时以下两字节不使用*/
	UINT8 LocalVobcAttri;			/*本端是否为控制端*/
	UINT8 IoHaveErr;				/*IO插箱有错误 用于迪士尼的不可倒切场景 db 20220713*/
	UINT8 IoDsnModeKey;				/*迪士尼实验项目控制端模式开关*/
	UINT8 Tc1DigtalInputState[INPUT_DEFINE_MAX];	/*TC1端开关量*/
	UINT8 Tc2DigtalInputState[INPUT_DEFINE_MAX];	/*TC2端开关量*/
	UINT8 CtrlDigtalInputState[INPUT_DEFINE_MAX];	/*控制端开关量*/
	UINT8 IdleDigtalInputState[INPUT_DEFINE_MAX];	/*等待端开关量*/
}IO_IN_VBTC_PERIOD_OUTPUT_STRUCT;

/*IO输出模块输入结构体*/
typedef struct
{
	UINT8 shadowModeFlag;                       /*主控模式0x55 影子模式0xAA  影子模式下不可倒切 0xcc*/
	UINT8 lastIOCtrlSide;                       /*上周期控制端*/
	IO_OUT_PERIOD_INPUT_STRUCT IoOut_InputInfo;	/*IO输出模块周期输入结构体*/
}IO_OUT_VBTC_PERIOD_INPUT_STRUCT;


/***************************************************************************************
* 功能描述:        IO输入模块需要初始化的值以及读取IO配置的数据
* 输入参数:        无
* 输入输出参数:    无
* 输出参数:        
* 全局变量:         
* 返回值:           0x00:初始化成功
					0x01:读取IO配置数据失败
* 修改记录：		
****************************************************************************************/
UINT8 IoModuleVbtc_Api_PowerOnInit(IN const IO_CFG_STRU *pIoCfgStru);

/***************************************************************************************
* 功能描述:        IO输入的数据处理,将接收到的IO数据解析存放到IO输入数据的结构体中
* 输入参数:        无
* 输入输出参数:    无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
					0x01:接收TC1端IO报文有误
					0x02:处理TC1端IO报文有误
					0x04:接收TC2端IO报文有误
					0x08:处理TC2端IO报文有误
* 修改记录：
****************************************************************************************/
UINT8 IoInPut_Api_PreiodProcess(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, OUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);
/***************************************************************************************
* 功能描述:			IO输出去的数据处理,将接收到的IO数据解析存放到IO输入数据的结构体中
* 输入参数:			IO_OUT_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo
* 输入输出参数:     无
* 输出参数:
* 全局变量:
* 返回值:
					0x00：成功；
					0x01:
* 修改记录：
****************************************************************************************/
UINT8 IoOutPut_Api_PreiodProcess(IN IO_OUT_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo);

/***************************************************************************************
* 功能描述:			获取输入开关量的状态
* 输入参数:			tcType:1、2端属性
					DigtalInputIndex :输入索引，需与IOModuleExtern.h定义的索引一致；
					DigtalInputState:与输入索引对应的输入开关量的状态。
* 输入输出参数:     无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
					FUNC_CODE1:输入索引数据超范围
					FUNC_CODE2:1/2端属性错误
* 修改记录：
****************************************************************************************/
UINT8 IoInPut_Api_GetDigtalInput(UINT8 tcType,UINT8 DigtalInputIndex,UINT8 *DigtalInputState);

/*获取FAM方向向后标志*/
UINT8 IoVbtcOut_Api_GetFamDirBack(void);

/***************************************************************************************
* 功能描述:			获取控制等待端
* 输入参数:
* 输入输出参数:
* 输出参数:			控制端、等待端
* 全局变量:         gIoInOutputStru
* 返回值:           0x00：成功；
* 修改记录：		added by Jielu 20230326
****************************************************************************************/
UINT8 IoInPutVbtc_Api_GetMainCtrSide(OUT UINT8* pCtrlSide, OUT UINT8* pIdleSide);

/*获取跟随数据*/
UINT8 IoInVbtc_Api_GetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len, UINT16 BuffMaxLen);

/*设置跟随数据*/
UINT8 IoInVbtc_Api_SetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len);

#endif 