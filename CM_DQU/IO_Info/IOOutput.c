/****************************************************************************************************
* 文件名   :  IOOutput.c
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2018.08.23
* 作者     :  李淑娜
* 功能描述 :  IO输出模块处理函数文件
* 使用注意 :  DO输出模块不关心控制端和非控制端信息，只是负责收集1端和2端
输出，按照点位配置打包并发送。
*
* 修改记录 :  优化，重新组织调度，增加接口函数。mjz-2018.10.28.
*
**************************************************************************************************/

#include "CommDefine.h"
#include "CommDataSource.h"
#include "Convert.h"
#include "CommFunc.h"
#include "IOOutput.h"
#include "IOInput.h"
#include "IOModuleExtern.h"
#include "CommonMemory.h"
#include "PLT_MCP_InterfaceToApp.h"
#include "PLFM_LOG.h"
#include "IOConfig.h"

#ifdef ITO_USE_IO_MODULE
#include "PLT_MCP_InterfaceToApp.h"
#include "MaintainInfoProcess.h"
#else
#include "AtpDataConfigExtern.h"
#endif


IO_OUTPUT_STRUCT 		gIOOutputStru = { 0 }; 			/* IO输出结构体用于对应描述表*/
IO_OUTPUT_STRUCT 		gIOOutputStruCtrl = { 0 }; 		/* IO输出结构体用于对应描述表--控制端信息 */
IO_OUTPUT_STRUCT 		gIOOutputStruIdle = { 0 }; 		/* IO输出结构体用于对应描述表--等待端信息 */
IO_OUTPUT_DATA_STRU 	gIoOutputDataStru = { 0 }; 		/* IO输出的关于插槽以及信息的结构体 (此结构体可能会根据整体的输出结构体有所变化)*/
UINT8  gIvocCtrlType = 0U;

extern UINT8 gFamCtrlSide;
extern UINT8 gFamReverStrTag;

extern UINT8 gARReverStrTag;
UINT8 ARuptime = 0U;

/* 将输出开关量含义与输出结构体关联的描述表 */
#if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
IO_OUTPUT_INDEX gIoOutputIndexTable[OUTPUT_FLAG_NUM] =
{
	/* 安全输出对应表 */
	{ &(gIOOutputStru.OutEb),OUT_EB },
	{ &(gIOOutputStru.OutForbidEbRelieve),OUT_FORBID_EB_RELIEVE },
	{ &(gIOOutputStru.OutTractionCut),OUT_TRACTION_CUT },
	{ &(gIOOutputStru.OutBeginFsb),OUT_BEGIN_FSB },
	{ &(gIOOutputStru.OutAutoSwitchSingle1),OUT_AUTO_SWITCH_SINGLE1 },
	{ &(gIOOutputStru.OutAutoSwitchSingle2),OUT_AUTO_SWITCH_SINGLE2 },
	{ &(gIOOutputStru.OutTrainControlRight),OUT_TRAIN_CONTROL_RIGHT },
	{ &(gIOOutputStru.OutAtpContralTrain),OUT_ATP_CONTRAL_TRAIN },
	{ &(gIOOutputStru.OutElectricBrakeChanl3),OUT_ELECTRIC_BRAKE_CHANL3 },
	{ &(gIOOutputStru.OutElectricBrakeChanl4),OUT_ELECTRIC_BRAKE_CHANL4 },
	{ &(gIOOutputStru.OutFaultEb),OUT_FAULT_EB },
	{ &(gIOOutputStru.OutAtpLeftDoorEn),OUT_ATP_LEFTDOOR_EN },
	{ &(gIOOutputStru.OutAtpRightDoorEn),OUT_ATP_RIGHTDOOR_EN },
	{ &(gIOOutputStru.OutAtpArLamp),OUT_ATP_AR_LAMP },

	{ &(gIOOutputStru.OutAtpArRelay),OUT_ATP_AR_RELAY },
	{ &(gIOOutputStru.OutAtpZeroSpeedsig),OUT_ATP_ZERO_SPEEDSIG },
	{ &(gIOOutputStru.OutAtpWakeup),OUT_ATP_WAKEUP },
	{ &(gIOOutputStru.OutAtpSleep),OUT_ATP_SLEEP },
	{ &(gIOOutputStru.OutAtpRevoke),OUT_ATP_REVOKE },
	{ &(gIOOutputStru.OutTrainRelieve),OUT_TRAIN_RELIEVE },

	/* 非安全输出对应表 */
	{ &(gIOOutputStru.OutPassNeutralFault),OUT_PASS_NEUTRAL_FAULT },
	{ &(gIOOutputStru.OutPassNeutralPreview),OUT_PASS_NEUTRAL_PREVIEW },
	{ &(gIOOutputStru.OutPassNeutralForce),OUT_PASS_NEUTRAL_FORCE },
	{ &(gIOOutputStru.OutPilotLamp1),OUT_PILOT_LAMP1 },
	{ &(gIOOutputStru.OutPilotLamp2),OUT_PILOT_LAMP2 },
	{ &(gIOOutputStru.OutVobcFsb1),OUT_VOBC_FSB_1 },
	{ &(gIOOutputStru.OutVobcFsb2),OUT_VOBC_FSB_2 },

	{ &(gIOOutputStru.OutStartLight),OUT_ATP_START_LIGHT },

	{ &(gIOOutputStru.TRAIN_OPPOSITE_RESTRAT),IN_TRAIN_OPPOSITE_RESTRAT },
	{ &(gIOOutputStru.TRAIN_CAB_ACTIVE_OUTPUT),IN_TRAIN_CAB_ACTIVE_OUTPUT },
	{ &(gIOOutputStru.DIRFORWARD_OUTPUT),ATP_DIRFORWARD_OUTPUT },
	{ &(gIOOutputStru.DIRBACKWARD_OUTPUT),ATP_DIRBACKWARD_OUTPUT },
	{ &(gIOOutputStru.FAMMODE_OUTPUT),ATP_FAMMODE_OUTPUT },
	{ &(gIOOutputStru.CAMMODE_OUTPUT),ATP_CAMMODE_OUTPUT },
	{ &(gIOOutputStru.JUMPDRDER_OUTPUT),ATP_JUMPDRDER_OUTPUT },
	{ &(gIOOutputStru.PARKING_BRAKING_IMPLEMENT_OUTPUT),AOM_PARKING_BRAKING_IMPLEMENT_OUTPUT_ATP },
	{ &(gIOOutputStru.PARKING_BRAKING_FREE_OUTPUT),AOM_PARKING_BRAKING_FREE_OUTPUT_ATP },

	{ &(gIOOutputStru.ADDROFFSET_OUT_ENABLE),ATO_ADDROFFSET_OUT_ENABLE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_TRACTION),ATO_ADDROFFSET_OUT_TRACTION },
	{ &(gIOOutputStru.ADDROFFSET_OUT_BRAKE),ATO_ADDROFFSET_OUT_BRAKE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_HOLD_BRAKE),ATO_ADDROFFSET_OUT_HOLD_BRAKE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_LEFT_DOOR_OPEN),ATO_ADDROFFSET_OUT_LEFT_DOOR_OPEN },
	{ &(gIOOutputStru.ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE),ATO_ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_RIGHT_DOOR_OPEN),ATO_ADDROFFSET_OUT_RIGHT_DOOR_OPEN },
	{ &(gIOOutputStru.ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE),ATO_ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE },
	{ &(gIOOutputStru.ADDROFFSET_START_LIGHT),ATO_ADDROFFSET_START_LIGHT },
	{ &(gIOOutputStru.ADDROFFSET_BAG_LEFTDOOR_OPEN),ATO_ADDROFFSET_BAG_LEFTDOOR_OPEN },
	{ &(gIOOutputStru.ADDROFFSET_BAG_RIGHTDOOR_OPEN),ATO_ADDROFFSET_BAG_RIGHTDOOR_OPEN },
	{ &(gIOOutputStru.ATO_ENABLE_1),OUT_ATO_ENABLE_1 },
	{ &(gIOOutputStru.ATO_ENABLE_2),OUT_ATO_ENABLE_2 },
	{ &(gIOOutputStru.AIR_BRAKE),OUT_AIR_BRAKE },
	{ &(gIOOutputStru.EmcyDoorUnlock),OUT_TRAIN_EMERDOOR },
	{ &(gIOOutputStru.Redundancy),OUT_TRAIN_REDUNDANCY },
	{ &(gIOOutputStru.OutRemoteRelifeBogie),OUT_TRAIN_REMOTE_RELIFE_BOGIE },
	{ &(gIOOutputStru.OutDoorUnlockLeft),OUT_TRAIN_DOOR_UNLOCK_LEFT },
	{ &(gIOOutputStru.OutDoorUnlockRight),OUT_TRAIN_DOOR_UNLOCK_RIGHT },
	{ &(gIOOutputStru.OutEmcyUnlockLeft),OUT_TRAIN_EMCY_DOOR_UNLOCK_LEFT },
	{ &(gIOOutputStru.OutEmcyUnlockRight),OUT_TRAIN_EMCY_DOOR_UNLOCK_RIGHT },
	{ &(gIOOutputStru.OutForcedReleaseCmd),OUT_TRAIN_FORCED_RELEASE },
	{ &(gIOOutputStru.OutTrainFAMStartLamp),OUT_TRAIN_FAM_START_LIGHT },
	{ &(gIOOutputStru.OutEmergencyDoorUnlock),OUT_TRAIN_ALL_DOOR_UNLOCK },
	{ &(gIOOutputStru.OutNotAllowReboot),OUT_NOT_ALLOW_REBOOT},
	{ &(gIOOutputStru.OutRebootAto),OUT_ALLOW_REBOOT_ATO}
};


IO_OUTPUT_INDEX gIoOutputAtoIndexTable[OUTPUT_ATO_NUM] =
{
	{ &(gIOOutputStru.ADDROFFSET_OUT_ENABLE),ATO_ADDROFFSET_OUT_ENABLE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_TRACTION),ATO_ADDROFFSET_OUT_TRACTION },
	{ &(gIOOutputStru.ADDROFFSET_OUT_BRAKE),ATO_ADDROFFSET_OUT_BRAKE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_HOLD_BRAKE),ATO_ADDROFFSET_OUT_HOLD_BRAKE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_LEFT_DOOR_OPEN),ATO_ADDROFFSET_OUT_LEFT_DOOR_OPEN },
	{ &(gIOOutputStru.ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE),ATO_ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE },
	{ &(gIOOutputStru.ADDROFFSET_OUT_RIGHT_DOOR_OPEN),ATO_ADDROFFSET_OUT_RIGHT_DOOR_OPEN },
	{ &(gIOOutputStru.ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE),ATO_ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE },
	{ &(gIOOutputStru.ADDROFFSET_START_LIGHT),ATO_ADDROFFSET_START_LIGHT }
};

#endif
#if(1 == HHTC_PROJECT_USER)
IO_OUTPUT_INDEX gIoOutputIndexTable[OUTPUT_FLAG_NUM] =
{
	/* 安全输出对应表 */
	{ &(gIOOutputStru.OutEb),OUT_EB },
	{ &(gIOOutputStru.OutForbidEbRelieve),OUT_FORBID_EB_RELIEVE },
	{ &(gIOOutputStru.OutTractionCut),OUT_TRACTION_CUT },
	{ &(gIOOutputStru.OutBeginFsb),OUT_BEGIN_FSB },
	{ &(gIOOutputStru.OutAutoSwitchSingle1),OUT_AUTO_SWITCH_SINGLE1 },
	{ &(gIOOutputStru.OutAutoSwitchSingle2),OUT_AUTO_SWITCH_SINGLE2 },
	{ &(gIOOutputStru.OutTrainControlRight),OUT_TRAIN_CONTROL_RIGHT },
	{ &(gIOOutputStru.OutAtpContralTrain),OUT_ATP_CONTRAL_TRAIN },
	{ &(gIOOutputStru.OutElectricBrakeChanl3),OUT_ELECTRIC_BRAKE_CHANL3 },
	{ &(gIOOutputStru.OutElectricBrakeChanl4),OUT_ELECTRIC_BRAKE_CHANL4 },
	{ &(gIOOutputStru.OutFaultEb),OUT_FAULT_EB },
	{ &(gIOOutputStru.OutAtpLeftDoorEn),OUT_ATP_LEFTDOOR_EN },
	{ &(gIOOutputStru.OutAtpRightDoorEn),OUT_ATP_RIGHTDOOR_EN },
	{ &(gIOOutputStru.OutAtpArLamp),OUT_ATP_AR_LAMP },
	{ &(gIOOutputStru.OutAtpAtoEnable),OUT_ATP_ATO_ENABLE },
	{ &(gIOOutputStru.OutAtpArRelay),OUT_ATP_AR_RELAY },
	{ &(gIOOutputStru.OutAtpZeroSpeedsig),OUT_ATP_ZERO_SPEEDSIG },
	{ &(gIOOutputStru.OutAtpWakeup),OUT_ATP_WAKEUP },
	{ &(gIOOutputStru.OutAtpSleep),OUT_ATP_SLEEP },
	{ &(gIOOutputStru.OutAtpRevoke),OUT_ATP_REVOKE },
	{ &(gIOOutputStru.OutTrainRelieve),OUT_TRAIN_RELIEVE },

	{ &(gIOOutputStru.ATPSafeLigth1),OUT_TRAIN_LIGHT1 },
	{ &(gIOOutputStru.ATPSafeLigth2),OUT_TRAIN_LIGHT2 },

	/* 非安全输出对应表 */
	{ &(gIOOutputStru.OutPassNeutralFault),OUT_PASS_NEUTRAL_FAULT },
	{ &(gIOOutputStru.OutPassNeutralPreview),OUT_PASS_NEUTRAL_PREVIEW },
	{ &(gIOOutputStru.OutPassNeutralForce),OUT_PASS_NEUTRAL_FORCE },
	{ &(gIOOutputStru.OutPilotLamp1),OUT_PILOT_LAMP1 },
	{ &(gIOOutputStru.OutPilotLamp2),OUT_PILOT_LAMP2 },
	{ &(gIOOutputStru.OutVobcFsb1),OUT_VOBC_FSB_1 },
	{ &(gIOOutputStru.OutVobcFsb2),OUT_VOBC_FSB_2 }
	//{&(gIOOutputStru.OutVobcFsb120Kpa),OUT_VOBC_FSB_120KPA },
};
#endif
#if(1 == CC_PROJECT_USER)
IO_OUTPUT_INDEX gIoOutputIndexTable[OUTPUT_FLAG_NUM] =
{
	/* 安全输出对应表 */
	{ &(gIOOutputStru.OutEb),OUT_EB },
	{ &(gIOOutputStru.OutForbidEbRelieve),OUT_FORBID_EB_RELIEVE },
	{ &(gIOOutputStru.OutTractionCut),OUT_TRACTION_CUT },
	{ &(gIOOutputStru.OutBeginFsb),OUT_BEGIN_FSB },
	{ &(gIOOutputStru.OutAutoSwitchSingle1),OUT_AUTO_SWITCH_SINGLE1 },
	{ &(gIOOutputStru.OutAutoSwitchSingle2),OUT_AUTO_SWITCH_SINGLE2 },
	{ &(gIOOutputStru.OutTrainControlRight),OUT_TRAIN_CONTROL_RIGHT },
	{ &(gIOOutputStru.OutAtpContralTrain),OUT_ATP_CONTRAL_TRAIN },
	{ &(gIOOutputStru.OutElectricBrakeChanl3),OUT_ELECTRIC_BRAKE_CHANL3 },
	{ &(gIOOutputStru.OutElectricBrakeChanl4),OUT_ELECTRIC_BRAKE_CHANL4 },
	{ &(gIOOutputStru.OutFaultEb),OUT_FAULT_EB },
	{ &(gIOOutputStru.OutAtpLeftDoorEn),OUT_ATP_LEFTDOOR_EN },
	{ &(gIOOutputStru.OutAtpRightDoorEn),OUT_ATP_RIGHTDOOR_EN },
	{ &(gIOOutputStru.OutAtpArLamp),OUT_ATP_AR_LAMP },
	{ &(gIOOutputStru.OutAtpAtoEnable),OUT_ATP_ATO_ENABLE },
	{ &(gIOOutputStru.OutAtpArRelay),OUT_ATP_AR_RELAY },
	{ &(gIOOutputStru.OutAtpZeroSpeedsig),OUT_ATP_ZERO_SPEEDSIG },
	{ &(gIOOutputStru.OutAtpWakeup),OUT_ATP_WAKEUP },
	{ &(gIOOutputStru.OutAtpSleep),OUT_ATP_SLEEP },
	{ &(gIOOutputStru.OutAtpRevoke),OUT_ATP_REVOKE },
	{ &(gIOOutputStru.OutTrainRelieve),OUT_TRAIN_RELIEVE },

	{ &(gIOOutputStru.ATPSafeLigth1),OUT_TRAIN_LIGHT1 },
	{ &(gIOOutputStru.ATPSafeLigth2),OUT_TRAIN_LIGHT2 },

	/* 非安全输出对应表 */
	{ &(gIOOutputStru.OutPassNeutralFault),OUT_PASS_NEUTRAL_FAULT },
	{ &(gIOOutputStru.OutPassNeutralPreview),OUT_PASS_NEUTRAL_PREVIEW },
	{ &(gIOOutputStru.OutPassNeutralForce),OUT_PASS_NEUTRAL_FORCE },
	{ &(gIOOutputStru.OutPilotLamp1),OUT_PILOT_LAMP1 },
	{ &(gIOOutputStru.OutPilotLamp2),OUT_PILOT_LAMP2 },
	{ &(gIOOutputStru.OutVobcFsb1),OUT_VOBC_FSB_1 },
	{ &(gIOOutputStru.OutVobcFsb2),OUT_VOBC_FSB_2 },

	/*北京11号线新增输出开关量*/
	{ &(gIOOutputStru.OutAtpStartLight),OUT_ATP_START_LIGHT },

	{ &(gIOOutputStru.TrainOpppsiteRestrat),IN_TRAIN_OPPOSITE_RESTRAT },
	{ &(gIOOutputStru.TrainCabActiveOutput),IN_TRAIN_CAB_ACTIVE_OUTPUT },
	{ &(gIOOutputStru.DirforwardOutput),ATP_DIRFORWARD_OUTPUT },
	{ &(gIOOutputStru.DirbackwardOutput),ATP_DIRBACKWARD_OUTPUT },
	{ &(gIOOutputStru.FamModeOutput),ATP_FAMMODE_OUTPUT },
	{ &(gIOOutputStru.CamModeOutput),ATP_CAMMODE_OUTPUT },
	{ &(gIOOutputStru.JumpOrderOutput),ATP_JUMPDRDER_OUTPUT },
	{ &(gIOOutputStru.AirBrakingQuarantineOutput),ATP_AIR_BRAKING_QUARANTINE_OUTPUT },
	{ &(gIOOutputStru.ParkingBrakingImplementOutput),ATP_PARKING_BRAKING_IMPLEMENT_OUTPUT },
	{ &(gIOOutputStru.ParkingBrakingFreeOutput),ATP_PARKING_BRAKING_FREE_OUTPUT },

	{ &(gIOOutputStru.AddroffsetOutEnable),ATO_ADDROFFSET_OUT_ENABLE },
	{ &(gIOOutputStru.AddroffsetOutTraction),ATO_ADDROFFSET_OUT_TRACTION },
	{ &(gIOOutputStru.AddroffsetOutBrake),ATO_ADDROFFSET_OUT_BRAKE },
	{ &(gIOOutputStru.AddroffsetOutHoldBrake),ATO_ADDROFFSET_OUT_HOLD_BRAKE },
	{ &(gIOOutputStru.AddroffsetOutLeftDoorOpen),ATO_ADDROFFSET_OUT_LEFT_DOOR_OPEN },
	{ &(gIOOutputStru.AddroffsetOutAllLeftDoorClose),ATO_ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE },
	{ &(gIOOutputStru.AddroffsetOutRightDoorOpen),ATO_ADDROFFSET_OUT_RIGHT_DOOR_OPEN },
	{ &(gIOOutputStru.AddroffsetOutAllRightDoorClose),ATO_ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE },
	{ &(gIOOutputStru.AddroffsetBagLeftDoorOpen),ATO_ADDROFFSET_BAG_LEFTDOOR_OPEN },
	{ &(gIOOutputStru.AddroffsetBagRightDoorOpen),ATO_ADDROFFSET_BAG_RIGHTDOOR_OPEN },
	{ &(gIOOutputStru.AtoStartLight),ATO_ADDROFFSET_START_LIGHT },
	{ &(gIOOutputStru.OutNotAllowReboot),OUT_NOT_ALLOW_REBOOT_ATO},
	{ &(gIOOutputStru.OutRebootAto),OUT_ALLOW_REBOOT_ATO}
};
#endif
/***************************************************************************************
* 功能描述:			根据输出结构体中的值转换为bit位中的0和1
* 输入参数:			UINT8 OutValue 相应输出结构体变量
* 输入输出参数:		无
* 输出参数:			UINT8 *pIoValue bit位的值指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputProcess(UINT8 OutValue, UINT8 *pIoValue);

/***************************************************************************************
* 功能描述:			将IO输出结构体中数据打包成数据流
* 输入参数:			BOARD_IO_INFO_STRU *pDataConfigStru IO配置的结构体
* 输入输出参数:		无
* 输出参数:			UINT32 *pOutUnSafeData 将结构体中的数据转化为非安全输出数据流的指针
*					UINT32 *pOutSafeData   将结构体中的数据转化为安全输出数据流的指针
* 全局变量:
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
****************************************************************************************/
static UINT8 IoModuleOutputPackage(BOARD_IO_INFO_STRU *pDataConfigStru, UINT32 *pOutUnSafeData, UINT32 *pOutSafeData);

/***************************************************************************************
* 功能描述:			将IO输出数据放到字节流中（H-FAO使用）
* 输入参数:			IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
*					IN UINT32 OutUnSafeData 	  输出的非安全数据(非安全开关量)
*					IN UINT32 OutSafeData   	  输出的安全数据(安全开关量)
* 输入输出参数:		无
* 输出参数:			OUT UINT8 *pSendBuf		      输出数据流指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputBuf_with_Ana(IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UINT32 OutUnSafeData, IN UINT32 OutSafeData, OUT UINT8 *pSendBuf);

/***************************************************************************************
* 功能描述:			将IO输出数据放到字节流中（HHTC，CC使用）
* 输入参数:			UINT32 OutUnSafeData 输出的非安全数据
*					UINT16 OutSafeData   输出的安全数据
*					UINT32 dwAnaValue    输出的模拟量
*					UINT32 dwPwmValue    输出的PWM
* 输入输出参数:		无
* 输出参数:			UINT8 *pSendBuf 输出数据流指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputBuf(UINT8 *pSendBuf, UINT32 OutUnSafeData, UINT32 OutSafeData);

/***************************************************************************************
* 功能描述:			将IO输出结构体中的数据转化为数据流(H-FAO)
* 输入参数:			IN UINT16 PlugBoxId 插箱ID
*					IN IO_CFG_DATA_STRU *pDataConfigStru IO配置数据结构体指针
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
* 输入输出参数:		无
* 输出参数:			OUT UINT8 *pSendBuf 输出数据流指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
*					0x02:解析IO输入的数据出错
*					0x04:根据插箱ID寻找IO配置表ID失败
*					0x08:将安全输出和非安全输出的数据存放到发送的数据流中失败
****************************************************************************************/
static UINT8 IoModuleOutputHandler_with_Ana(IN UINT16 PlugBoxId, OUT UINT8 *pSendBuf, IN IO_CFG_DATA_STRU *pDataConfigStru, IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo);

/***************************************************************************************
* 功能描述:			将IO输出结构体中的数据转化为数据流(HHTC,CC)
* 输入参数:			UINT16 PlugBoxId 插箱ID
*					UINT8 *pSendBuf 收到的原始数据
*					IO_CFG_DATA_STRU *pDataConfigStru IO配置数据结构体指针
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
*					0x02:解析IO输入的数据出错
*					0x04:根据插箱ID寻找IO配置表ID失败
*					0x08:将安全输出和非安全输出的数据存放到发送的数据流失败
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputHandler(UINT16 PlugBoxId, UINT8 *pSendBuf, IO_CFG_DATA_STRU *pDataConfigStru);

/***************************************************************************************
* 功能描述:			根据IO索引将相应的IO输出的bit位存放到字节中
* 输入参数:			UINT8 CtrlSide 当前的控制端
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			见函数内全局变量
* 返回值:			0x00：成功；
*					FUNC_CODE1:IoOutSlotNum IO输出板数量不正确
*					FUNC_CODE2:AtpData_Api_GetOutDeviceTypeId函数处理错误
****************************************************************************************/
static UINT8 IoModule_SlotNumIdPrepare(UINT8 CtrlSide);

/***************************************************************************************
* 功能描述:			单端开关量输出数据准备
* 输入参数:			CtrlIdleType:端属性
*					CTRL_SIDE:控制端
*					IDLE_SIDE:非控制端
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			无
* 返回值:			FUNC_SUCESS：成功；
*					FUNC_CODE1:端属性参数错误
*					FUNC_CODE2:控制端输出开关量准备返回错误
*					FUNC_CODE3:非控制端输出开关量准备返回错误
* 修改记录：
****************************************************************************************/
static UINT8 IoModule_SingleSideOutputPrepare(UINT8 CtrlSide, UINT8 CtrlIdleType);

/***************************************************************************************
* 功能描述:			单端输出数据准备和输出（H-FAO）
* 输入参数:			IN UINT8 CtrlSide:
*					CAB_A_TC1:1端/A端
*					CAB_B_TC2:2端/B端
*					IN UINT8 CtrlIdleType:
*					CTRL_SIDE:控制端
*					IDLE_SIDE:非控制端
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			见函数内全局变量
* 返回值:			FUNC_SUCESS：成功；
*					FUNC_CODE1:输出插箱个数及其ID返回错误
*					FUNC_CODE2:输出开关量准备返回错误
*					FUNC_CODE3:输出映射返回错误
*					FUNC_CODE4:打包输出返回错误
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput_with_Ana(IN UINT8 CtrlSide, IN UINT8 CtrlIdleType, IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo);

/***************************************************************************************
* 功能描述:			单端输出数据准备和输出(HHTC,CC)
* 输入参数:			CtrlSide:
*					CAB_A_TC1:1端/A端
*					CAB_B_TC2:2端/B端
*					CtrlIdleType:
*					CTRL_SIDE:控制端
*					IDLE_SIDE:非控制端
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			gIoOutputDataStru,gIoConfigDataStru
* 返回值:			FUNC_SUCESS：成功；
*					FUNC_CODE1:输出插箱个数及其ID返回错误
*					FUNC_CODE2:输出开关量准备返回错误
*					FUNC_CODE3:输出映射返回错误
*					FUNC_CODE4:打包输出返回错误
* 修改记录：
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput(UINT8 CtrlSide, UINT8 CtrlIdleType);

/***************************************************************************************
* 功能描述:			IO输出模块需要初始化的值以及读取IO配置的数据
* 输入参数:			无
* 输入输出参数:		无
* 输出参数:
* 全局变量:
* 返回值:			0x00:初始化成功
*					0x01:读取IO配置数据失败
* 修改记录：
****************************************************************************************/
UINT8 IoOutModule_Api_PowerOnInit(IN const UINT8 *IvocCtrlType)
{
	UINT8 RtnNo = FUNC_CODE1;

	if (NULL != IvocCtrlType)
	{
		gIvocCtrlType = *IvocCtrlType;

		RtnNo = FUNC_SUCESS;
	}
	else
	{
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

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
UINT8 IoModuleBitToByte(UINT8 Index, UINT8 OutValue, UINT32 *pOutUnSafeData, UINT32 *pOutSafeData)
{
	UINT8 RtnNo = FUNC_CODE1;

	if ((NULL != pOutUnSafeData) && (NULL != pOutSafeData))
	{
		/*当索引大于等于22时为安全输入，当索引小于22时为非安全输入 */
		if (Index >= MAX_UNSAFE_CHANNELS_NUM)
		{
			if (Index < (UINT8)MAX_CHANNELS_NUM)
			{
				if (1U == OutValue)
				{
					*pOutSafeData |= (0x1U << (Index - MAX_UNSAFE_CHANNELS_NUM));
				}
				else
				{
					/* 继续保持0*/
				}

				RtnNo = FUNC_SUCESS;
			}
			else
			{
				/* 索引不对 */
				RtnNo = FUNC_CODE2;
			}
		}
		else
		{
			if (1U == OutValue)
			{
				*pOutUnSafeData |= (0x1U << Index);
			}
			else
			{
				/* 该位继续为0*/
			}

			RtnNo = FUNC_SUCESS;
		}
	}
	else
	{
		/* 传参错误 */
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* 功能描述:			根据输出结构体中的值转换为bit位中的0和1
* 输入参数:			UINT8 OutValue 相应输出结构体变量
* 输入输出参数:		无
* 输出参数:			UINT8 *pIoValue bit位的值指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputProcess(UINT8 OutValue, UINT8 *pIoValue)
{
	UINT8 RtnNo = FUNC_CODE1;

	if (NULL != pIoValue)
	{
		if (CBTC_TRUE == OutValue)
		{
			*pIoValue = 1U;
		}
		else
		{
			*pIoValue = 0U;
		}
		RtnNo = FUNC_SUCESS;
	}
	else
	{
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* 功能描述:			将IO输出结构体中数据打包成数据流
* 输入参数:			BOARD_IO_INFO_STRU *pDataConfigStru IO配置的结构体
* 输入输出参数:		无
* 输出参数:			UINT32 *pOutUnSafeData 将结构体中的数据转化为非安全输出数据流的指针
*					UINT32 *pOutSafeData   将结构体中的数据转化为安全输出数据流的指针
* 全局变量:
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
****************************************************************************************/
static UINT8 IoModuleOutputPackage(BOARD_IO_INFO_STRU *pDataConfigStru, UINT32 *pOutUnSafeData, UINT32 *pOutSafeData)
{
	UINT8 Index = 0U;
	UINT8 IoTableIndex = 0U;
	UINT8 BitValue = 0U;
	UINT8 RtnNo = FUNC_CODE1;
	UINT8 FuncRtn = FUNC_CODE1;

	if ((NULL != pDataConfigStru) && (NULL != pOutUnSafeData) && (NULL != pOutSafeData))
	{
		/* 遍历IO配置表中每一路的IO的含义 */
		for (Index = 0U; Index < (UINT8)MAX_CHANNELS_NUM; Index++)
		{
			/* 遍历IO含义与输入结构体对应的描述表 */
			for (IoTableIndex = 0U; IoTableIndex < OUTPUT_FLAG_NUM; IoTableIndex++)
			{
				/* 当IO配置表中的IO含义与描述表对应时，将相应的输入结构体中的相关量赋值 */
				if (pDataConfigStru->GfgIoBuf[Index] == gIoOutputIndexTable[IoTableIndex].IoOutputTypeIndex)
				{
					/* 将结构体中的值转化为0或者1的bit值 */
					FuncRtn = IoModuleOutputProcess(*(gIoOutputIndexTable[IoTableIndex].IoOutputStruIndex), &BitValue);
					if (FUNC_SUCESS == FuncRtn)
					{
						/* 将bit位根据索引存放到安全输出和非安全输出的字节中 */
						FuncRtn = IoModuleBitToByte(Index, BitValue, pOutUnSafeData, pOutSafeData);
						if (FUNC_SUCESS != FuncRtn)
						{
							RtnNo = FUNC_CODE2;
						}
						else
						{
							RtnNo = FUNC_SUCESS;
						}
					}
					else
					{
						RtnNo = FUNC_CODE3;
					}
					break;
				}
				else
				{
					/*无操作*/
				}
			}
		}
	}
	else
	{
		/*传入参数错误*/
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* 功能描述:			将IO输出数据放到字节流中（HHTC，CC使用）
* 输入参数:			UINT32 OutUnSafeData 输出的非安全数据
*					UINT16 OutSafeData   输出的安全数据
*					UINT32 dwAnaValue    输出的模拟量
*					UINT32 dwPwmValue    输出的PWM
* 输入输出参数:		无
* 输出参数:			UINT8 *pSendBuf 输出数据流指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputBuf(UINT8 *pSendBuf, UINT32 OutUnSafeData, UINT32 OutSafeData)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 i = 0U;
	if (NULL != pSendBuf)
	{
		AppLogPrintStr(ENUM_LOG_ERR, "Out safe %x,Out Unsafe %x\n", OutSafeData, OutUnSafeData);
		/* 将数据打包到相应的数据流中 */
		LongToChar(0, &pSendBuf[i]); /*序号*/
		i = i + 4U;
		LongToChar(OutUnSafeData, &pSendBuf[i]); /* 将输出的非安全数据存入数据流 */
		i = i + 4U;
		LongToChar(OutSafeData, &pSendBuf[i]); /* 将输出的安全数据存入数据流中 */
		i = i + 4U;

		FuncRtn = FUNC_SUCESS;
	}
	else
	{
		FuncRtn = FUNC_CODE1;
	}
	return(FuncRtn);
}

/***************************************************************************************
* 功能描述:			将IO输出数据放到字节流中（H-FAO使用）
* 输入参数:			IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
*					IN UINT32 OutUnSafeData 	  输出的非安全数据(非安全开关量)
*					IN UINT32 OutSafeData   	  输出的安全数据(安全开关量)
* 输入输出参数:		无
* 输出参数:			OUT UINT8 *pSendBuf		      输出数据流指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputBuf_with_Ana(IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UINT32 OutUnSafeData, IN UINT32 OutSafeData, OUT UINT8 *pSendBuf)
{
	UINT8 FuncRtn = (UINT8)FUNC_CODE1;
	UINT8 i = 0u;

	if ((NULL != pIoOut_InputInfo) && (NULL != pSendBuf))
	{
		AppLogPrintStr(ENUM_LOG_ERR, "Out safe 0x%x, Out Unsafe 0x%x, AnaValue %d, PwmValue %d; Enable 0x%x\n",
			OutSafeData, OutUnSafeData, pIoOut_InputInfo->dwCtrlSideAnaValue, pIoOut_InputInfo->dwCtrlSidePwmValue, pIoOut_InputInfo->AtoCmdEnable);

#ifdef SIM_VERSION   /*车车单机版用之前的输出板数据格式*/
		/* 将数据打包到相应的数据流中 */
		CM_LongToChar(0, &pSendBuf[i]); /*序号*/
		i = i + 4U;
		CM_LongToChar(OutUnSafeData, &pSendBuf[i]); /* 将输出的非安全数据存入数据流 */
		i = i + 4U;
		CM_LongToChar(OutSafeData, &pSendBuf[i]); /* 将输出的安全数据存入数据流中 */
		i = i + 4U;
		CM_LongToChar(pIoOut_InputInfo->dwCtrlSideAnaValue, &pSendBuf[i]);	/*4字节模拟量*/
		i = i + 4U;
		CM_LongToChar(pIoOut_InputInfo->dwCtrlSidePwmValue, &pSendBuf[i]);	/*4字节PWM占空比*/
		i = i + 4U;
		CM_LongToChar(OUT_PWM_FREQUENCY, &pSendBuf[i]);/*4字节PWM频率*/
		i = i + 4U;
#else   /*新平台新格式*/
		/* 将数据打包到相应的数据流中 ,数据包长度与DO_DATA_OUTPUT_BUF_SIZE宏值要一致*/
		/*周期号*/
		LongToChar(pIoOut_InputInfo->CycCount, &pSendBuf[i]);
		i += 4u;

		/*非安全数据有效位掩码*/
		LongToChar(pIoOut_InputInfo->OutUnSafeDataMask, &pSendBuf[i]);
		i += 4u;

		/*输出的非安全数据(非安全开关量)*/
		LongToChar(OutUnSafeData, &pSendBuf[i]);
		i += 4u;

		/*安全开关量有效位掩码*/
		LongToChar(pIoOut_InputInfo->OutSafeDataMask, &pSendBuf[i]);
		i += 4u;

		/*输出的安全数据(安全开关量)*/
		LongToChar(OutSafeData, &pSendBuf[i]);
		i += 4u;

		/*输出的模拟量数据有效掩码,0xaa:有效,0x55:无效*/
		pSendBuf[i] = pIoOut_InputInfo->dwAnaValueValid;
		i += 1u;

		/*输出的模拟量数据*/
		LongToChar(pIoOut_InputInfo->dwCtrlSideAnaValue, &pSendBuf[i]);
		i += 4u;

		/*PWM数据有效掩码,0xaa:有效,0x55:无效*/
		pSendBuf[i] = pIoOut_InputInfo->dwPwmValueValid;
		i += 1u;

		/*输出的PWM值数据*/
		LongToChar(pIoOut_InputInfo->dwCtrlSidePwmValue, &pSendBuf[i]);
		i += 4u;

		/*输出的PWM值数据频率*/
		LongToChar(OUT_PWM_FREQUENCY, &pSendBuf[i]);
		i += 4u;

		/*ATO输出授权标志,ATP填充,ATO预留,0xaa:有效,0x55:无效*/
		pSendBuf[i] = pIoOut_InputInfo->AtoCmdEnable;
		i += 1u;

		/*预留7字节*/
		FuncRtn &= CommonMemSet(&pSendBuf[i], 7u, 0x00u, 7u);
		i += 7u;

#endif
		FuncRtn = (UINT8)FUNC_SUCESS;
	}
	else
	{
		FuncRtn = (UINT8)FUNC_CODE1;
	}

	return FuncRtn;
}

/***************************************************************************************
* 功能描述:			将IO输出结构体中的数据转化为数据流(HHTC,CC)
* 输入参数:			UINT8 PlugBoxId 插箱ID
*					UINT8 *pRecvBuf 收到的原始数据
*					IO_DATA_CONFIG_STRUT *pDataConfigStru IO配置数据结构体指针
* 输入输出参数:		无
* 输出参数:
* 全局变量:
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
*					0x02:解析IO输入的数据出错
*					0x04:根据插箱ID寻找IO配置表ID失败
*					0x08:将安全输出和非安全输出的数据存放到发送的数据流失败
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleOutputHandler(UINT16 PlugBoxId, UINT8 *pSendBuf, IO_CFG_DATA_STRU *pDataConfigStru)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 RtnNo = FUNC_CODE1;
	UINT8 PlugBoxIndex = 0U;
	UINT32 OutUnSafeData = 0U;
	UINT32 OutSafeData = 0U;

	if ((NULL != pSendBuf) && (NULL != pDataConfigStru))
	{
		/*根据插箱ID查找对应插箱输出信息配置数据*/
		FuncRtn = IoModuleInputFindConfigDataFromPlugBoxId(PlugBoxId, pDataConfigStru, &PlugBoxIndex);
		if ((FUNC_SUCESS == FuncRtn) && (TRAIN_OUTPUT_BOARD == pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex].BoxAttr))
		{
			/* 将IO结构体中的数据根据IO配置表打包存放到输出的字节中 */
			FuncRtn = IoModuleOutputPackage(&pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex], &OutUnSafeData, &OutSafeData);
			if (FUNC_SUCESS == FuncRtn)
			{
				/* 将安全输出和非安全输出的数据存放到发送的数据流中 */

				FuncRtn = IoModuleOutputBuf(pSendBuf, OutUnSafeData, OutSafeData);

				if (FUNC_SUCESS == FuncRtn)
				{
					RtnNo = FUNC_SUCESS;
				}
				else
				{
					RtnNo = FUNC_CODE4;
				}
			}
			else
			{
				RtnNo = FUNC_CODE2;
			}
		}
		else
		{
			RtnNo = FUNC_CODE3;
		}
	}
	else
	{
		/* 传入参数有误 */
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* 功能描述:			将IO输出结构体中的数据转化为数据流(H-FAO)
* 输入参数:			IN UINT16 PlugBoxId 插箱ID
*					IN IO_CFG_DATA_STRU *pDataConfigStru IO配置数据结构体指针
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
* 输入输出参数:		无
* 输出参数:			OUT UINT8 *pSendBuf 输出数据流指针
* 全局变量:			无
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
*					0x02:解析IO输入的数据出错
*					0x04:根据插箱ID寻找IO配置表ID失败
*					0x08:将安全输出和非安全输出的数据存放到发送的数据流中失败
****************************************************************************************/
static UINT8 IoModuleOutputHandler_with_Ana(IN UINT16 PlugBoxId, OUT UINT8 *pSendBuf, IN IO_CFG_DATA_STRU *pDataConfigStru, IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 RtnNo = FUNC_CODE1;
	UINT8 PlugBoxIndex = 0U;
	UINT32 OutUnSafeData = 0U;
	UINT32 OutSafeData = 0U;

	if ((NULL != pSendBuf) && (NULL != pDataConfigStru) && (NULL != pIoOut_InputInfo))
	{
		/*根据插箱ID查找对应插箱输出信息配置数据*/
		FuncRtn = IoModuleInputFindConfigDataFromPlugBoxId(PlugBoxId, pDataConfigStru, &PlugBoxIndex);
		if ((FUNC_SUCESS == FuncRtn) && (TRAIN_OUTPUT_BOARD == pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex].BoxAttr))
		{
			/*将IO结构体中的数据根据IO配置表打包存放到输出的字节中*/
			FuncRtn = IoModuleOutputPackage(&pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex], &OutUnSafeData, &OutSafeData);
			if (FUNC_SUCESS == FuncRtn)
			{
				/*将安全输出和非安全输出的数据存放到发送的数据流中*/
				FuncRtn = IoModuleOutputBuf_with_Ana(pIoOut_InputInfo, OutUnSafeData, OutSafeData, pSendBuf);
				if (FUNC_SUCESS == FuncRtn)
				{
					RtnNo = FUNC_SUCESS;
				}
				else
				{
					RtnNo = FUNC_CODE4;
				}

				if (DARKLOG_ENABLE)
				{
					/*暗文日志记录：输出开关量*/
					if (IO_TC1_OUT_1_TYPE_ID == PlugBoxId)
					{
						DarkLogPrint(ENUM_LOG_ERR, &OutSafeData, DL_4B_OUT_IO_TC1_SAFE_IO, DL_4B);
						DarkLogPrint(ENUM_LOG_ERR, &OutUnSafeData, DL_4B_OUT_IO_TC1_UNSAFE_IO, DL_4B);
					}
					else if (IO_TC2_OUT_1_TYPE_ID == PlugBoxId)
					{
						DarkLogPrint(ENUM_LOG_ERR, &OutSafeData, DL_4B_OUT_IO_TC2_SAFE_IO, DL_4B);
						DarkLogPrint(ENUM_LOG_ERR, &OutUnSafeData, DL_4B_OUT_IO_TC2_UNSAFE_IO, DL_4B);
					}
					else
					{
						/*板卡类型不正确不进行输出*/
					}
				}
				else
				{
					/*do nothing*/
				}

			}
			else
			{
				RtnNo = FUNC_CODE2;
			}
		}
		else
		{
			RtnNo = FUNC_CODE3;
		}
	}
	else
	{
		/* 传入参数有误 */
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* 功能描述:			根据IO索引将相应的IO输出的bit位存放到字节中
* 输入参数:			UINT8 CtrlSide 当前的控制端
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			见函数内全局变量
* 返回值:			0x00：成功；
*					FUNC_CODE1:IoOutSlotNum IO输出板数量不正确
*					FUNC_CODE2:AtpData_Api_GetOutDeviceTypeId函数处理错误
****************************************************************************************/
static UINT8 IoModule_SlotNumIdPrepare(UINT8 CtrlSide)
{
	UINT8 RtnNum = FUNC_CODE1;		/* 本函数的返回值 */
	UINT8 CallFuncRtn = FUNC_CODE1;		/* 调用函数的返回值 */
	UINT16 OutputBoxDeviceTypeId[BOARD_CONFIG_NUM] = { 0U };	/*输出插箱设备typeID*/
	UINT8 OutputBoxNum = 0U;	/*输出插箱 个数 */
	UINT8 i = 0U;

	CallFuncRtn = AtpData_Api_GetOutDeviceTypeId(CtrlSide, &OutputBoxDeviceTypeId[0U], &OutputBoxNum);
	if (FUNC_SUCESS == CallFuncRtn)
	{
		gIoOutputDataStru.IoOutSlotNum = OutputBoxNum;
		if ((BOARD_CONFIG_NUM / 2U) >= gIoOutputDataStru.IoOutSlotNum)
		{
			for (i = 0U; i < gIoOutputDataStru.IoOutSlotNum; i++)
			{
				gIoOutputDataStru.IoOutSlotId[i] = OutputBoxDeviceTypeId[i];
			}

			RtnNum = FUNC_SUCESS;
		}
		else
		{
			RtnNum = FUNC_CODE1;
		}
	}
	else
	{
		RtnNum = FUNC_CODE2;
	}

	return RtnNum;
}

/***************************************************************************************
* 功能描述:			单端开关量输出数据准备
* 输入参数:			CtrlIdleType:端属性
*					CTRL_SIDE:控制端
*					IDLE_SIDE:非控制端
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			无
* 返回值:			FUNC_SUCESS：成功；
*					FUNC_CODE1:端属性参数错误
*					FUNC_CODE2:控制端输出开关量准备返回错误
*					FUNC_CODE3:非控制端输出开关量准备返回错误
* 修改记录：
****************************************************************************************/
static UINT8 IoModule_SingleSideOutputPrepare(UINT8 CtrlSide, UINT8 CtrlIdleType)
{
	UINT8 FuncRtn = FUNC_CODE1;		/* 本函数的返回值 */

	if (CTRL_SIDE == CtrlIdleType)
	{
		(void)CommonMemCpy(&gIOOutputStru, sizeof(IO_OUTPUT_STRUCT), &gIOOutputStruCtrl, sizeof(IO_OUTPUT_STRUCT));
		FuncRtn = FUNC_SUCESS;
	}
	else if (IDLE_SIDE == CtrlIdleType)
	{
		(void)CommonMemCpy(&gIOOutputStru, sizeof(IO_OUTPUT_STRUCT), &gIOOutputStruIdle, sizeof(IO_OUTPUT_STRUCT));
		FuncRtn = FUNC_SUCESS;
	}
	else
	{
		FuncRtn = FUNC_CODE1;
	}

	/*在FAM/AR折返的时候，原控制端落下AR继电器，原尾端吸起AR继电器*/
	if ((CBTC_TRUE == gFamReverStrTag) || (CBTC_TRUE == gARReverStrTag))
	{
		if (gFamCtrlSide == CtrlSide)
		{
			gIOOutputStru.TRAIN_CAB_ACTIVE_OUTPUT = CBTC_FALSE;
		}
		else
		{
			/*尾端AR继电器吸起延时，*/
			ARuptime++;
			if (6U < ARuptime)
			{
				ARuptime = 7U;
				gIOOutputStru.TRAIN_CAB_ACTIVE_OUTPUT = CBTC_TRUE;
			}

		}

		AppLogPrintStr(ENUM_LOG_ERR, "[IO]R:0x%x ,0x%x, Ctrl:0x%x, AR:0x%x\n", gFamReverStrTag, gFamCtrlSide, CtrlSide, gIOOutputStru.TRAIN_CAB_ACTIVE_OUTPUT);
	}
	else
	{
		ARuptime = 0U;
	}

	return FuncRtn;
}

/***************************************************************************************
* 功能描述:			单端输出数据准备和输出(HHTC,CC)
* 输入参数:			CtrlSide:
*					CAB_A_TC1:1端/A端
*					CAB_B_TC2:2端/B端
*					CtrlIdleType:
*					CTRL_SIDE:控制端
*					IDLE_SIDE:非控制端
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			gIoOutputDataStru,gIoConfigDataStru
* 返回值:			FUNC_SUCESS：成功；
*					FUNC_CODE1:输出插箱个数及其ID返回错误
*					FUNC_CODE2:输出开关量准备返回错误
*					FUNC_CODE3:输出映射返回错误
*					FUNC_CODE4:打包输出返回错误
* 修改记录：
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput(UINT8 CtrlSide, UINT8 CtrlIdleType)
{
	UINT8 CallFuncRtn = FUNC_CODE1;    	/* 调用函数的返回值 */
	UINT8 FuncRtn = FUNC_CODE1;      /* 本函数的返回值 */
	UINT8 FrameIndex = 0U;

	/*准备输出插箱个数及其ID*/
	CallFuncRtn = IoModule_SlotNumIdPrepare(CtrlSide);
	if (FUNC_SUCESS == CallFuncRtn)
	{
		/*准备输出开关量*/
		CallFuncRtn = IoModule_SingleSideOutputPrepare(CtrlSide, CtrlIdleType);
		if (FUNC_SUCESS == CallFuncRtn)
		{
			for (FrameIndex = 0U; FrameIndex < gIoOutputDataStru.IoOutSlotNum; FrameIndex++)
			{

				CallFuncRtn = IoModuleOutputHandler(gIoOutputDataStru.IoOutSlotId[FrameIndex], gIoOutputDataStru.SendDataBuf[FrameIndex], &gIoConfigDataStru);

				if (FUNC_SUCESS == CallFuncRtn)
				{

					if (IO_TC1_OUT_1_TYPE_ID == gIoOutputDataStru.IoOutSlotId[FrameIndex])
					{
						CallFuncRtn = CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC1_OUT_1_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], 12U);
						CallFuncRtn |= CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC1_OUT_2_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], 12U);
					}
					else if (IO_TC2_OUT_1_TYPE_ID == gIoOutputDataStru.IoOutSlotId[FrameIndex])
					{
						CallFuncRtn = CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC2_OUT_1_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], 12U);
						CallFuncRtn |= CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC2_OUT_2_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], 12U);
					}
					else
					{
						/* 板卡类型不正确不进行输出*/
					}

					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE4;
						break;
					}
					else
					{
						FuncRtn = FUNC_SUCESS;
					}
				}
				else
				{
					FuncRtn |= FUNC_CODE3;
				}
			}

		}
		else
		{
			FuncRtn |= FUNC_CODE2;
		}
	}
	else
	{
		FuncRtn = FUNC_CODE1;
	}
	return FuncRtn;
}

/***************************************************************************************
* 功能描述:			单端输出数据准备和输出（H-FAO）
* 输入参数:			IN UINT8 CtrlSide:
*					CAB_A_TC1:1端/A端
*					CAB_B_TC2:2端/B端
*					IN UINT8 CtrlIdleType:
*					CTRL_SIDE:控制端
*					IDLE_SIDE:非控制端
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			见函数内全局变量
* 返回值:			FUNC_SUCESS：成功；
*					FUNC_CODE1:输出插箱个数及其ID返回错误
*					FUNC_CODE2:输出开关量准备返回错误
*					FUNC_CODE3:输出映射返回错误
*					FUNC_CODE4:打包输出返回错误
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput_with_Ana(IN UINT8 CtrlSide, IN UINT8 CtrlIdleType, IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo)
{
	UINT8 CallFuncRtn = FUNC_CODE1;    	/* 调用函数的返回值 */
	UINT8 FuncRtn = FUNC_CODE1;      /* 本函数的返回值 */
	UINT8 FrameIndex = 0U;

	/*准备输出插箱个数及其ID*/
	CallFuncRtn = IoModule_SlotNumIdPrepare(CtrlSide);
	if ((FUNC_SUCESS == CallFuncRtn) && (NULL != pIoOut_InputInfo))
	{
		/*准备输出开关量*/
		CallFuncRtn = IoModule_SingleSideOutputPrepare(CtrlSide, CtrlIdleType);
		if (FUNC_SUCESS == CallFuncRtn)
		{
			for (FrameIndex = 0U; FrameIndex < gIoOutputDataStru.IoOutSlotNum; FrameIndex++)
			{
				CallFuncRtn = IoModuleOutputHandler_with_Ana(gIoOutputDataStru.IoOutSlotId[FrameIndex], gIoOutputDataStru.SendDataBuf[FrameIndex], &gIoConfigDataStru, pIoOut_InputInfo);
				if (FUNC_SUCESS == CallFuncRtn)
				{
					if (IO_TC1_OUT_1_TYPE_ID == gIoOutputDataStru.IoOutSlotId[FrameIndex])
					{
						CallFuncRtn = CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC1_OUT_1_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], DO_DATA_OUTPUT_BUF_SIZE);
						CallFuncRtn |= CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC1_OUT_2_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], DO_DATA_OUTPUT_BUF_SIZE);
					}
					else if (IO_TC2_OUT_1_TYPE_ID == gIoOutputDataStru.IoOutSlotId[FrameIndex])
					{
						CallFuncRtn = CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC2_OUT_1_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], DO_DATA_OUTPUT_BUF_SIZE);
						CallFuncRtn |= CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC2_OUT_2_TYPE_ID, BOARD_INNET_CPU_MSG, 0U, gIoOutputDataStru.SendDataBuf[FrameIndex], DO_DATA_OUTPUT_BUF_SIZE);
					}
					else
					{
						/*板卡类型不正确不进行输出*/
					}

					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE4;
						break;
					}
					else
					{
						FuncRtn = FUNC_SUCESS;
					}
				}
				else
				{
					FuncRtn |= FUNC_CODE3;
				}
			}
		}
		else
		{
			FuncRtn |= FUNC_CODE2;
		}
	}
	else
	{
		FuncRtn = FUNC_CODE1;
	}

	return FuncRtn;
}

/***************************************************************************************
* 功能描述:			IO输出结构体中的数据打包成数据流，分别发送给1端和2端
* 输入参数:			IN IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			无
* 返回值:			0x00：成功；
*					FUNC_CODE1:获取控制端失败
*					FUNC_CODE2:有控制端时，主控端输出失败；
*					FUNC_CODE3:有控制端时，非主控端输出失败；
*					FUNC_CODE4:无控制端时，TC1端输出失败；
*					FUNC_CODE5:无控制端时，TC2端输出失败；
* 修改记录：
****************************************************************************************/
UINT32 IoModulOutPut_Api_PreiodProcess(IN IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UCHAR LastActiveState)
{
	UINT8 CallFuncRtn = FUNC_CODE1;    	/* 调用函数的返回值 */
	UINT32 FuncRtn = FUNC_CODE1;        /* 本函数的返回值 */

	if (NULL != pIoOut_InputInfo)
	{
		(void)CommonMemCpy(&gIOOutputStruCtrl, sizeof(IO_OUTPUT_STRUCT), &pIoOut_InputInfo->IOOutputStruCtrl, sizeof(IO_OUTPUT_STRUCT));
		(void)CommonMemCpy(&gIOOutputStruIdle, sizeof(IO_OUTPUT_STRUCT), &pIoOut_InputInfo->IOOutputStruIdle, sizeof(IO_OUTPUT_STRUCT));
		if (ONE_CTRL_ONE == gIvocCtrlType)
		{
			if (CBTC_TRUE == pIoOut_InputInfo->LocalVobcAttri)
			{
				/*控制端输出*/
				if (CAB_A_TC1 == pIoOut_InputInfo->MainCtrlSide)
				{
					if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
					}
					else
					{
						CallFuncRtn = IoModule_SingleSideOutput((UINT8)CAB_A_TC1, (UINT8)CTRL_SIDE);
					}
					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE2;
					}
					else
					{
						/* 输出正常*/
					}
				}
				else if (CAB_B_TC2 == pIoOut_InputInfo->MainCtrlSide)
				{
					if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
					}
					else
					{
						CallFuncRtn = IoModule_SingleSideOutput((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE);
					}

					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE3;
					}
					else
					{
						/* 输出正常*/
					}
				}
				else
				{
					/*无操作*/
				}
			}
			/*对端为控制端*/
			else
			{
				if (CAB_A_TC1 == pIoOut_InputInfo->MainCtrlSide)
				{
					if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
					}
					else
					{
						CallFuncRtn = IoModule_SingleSideOutput((UINT8)CAB_A_TC1, (UINT8)IDLE_SIDE);
					}
					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE4;
					}
					else
					{
						/* 输出正常*/
					}
				}
				else if (CAB_B_TC2 == pIoOut_InputInfo->MainCtrlSide)
				{
					if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
					}
					else
					{
						CallFuncRtn = IoModule_SingleSideOutput((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE);
					}

					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE5;
					}
					else
					{
						/* 输出正常*/
					}
				}
				else
				{
					/*无操作*/
				}
			}
		}
		/*单端控两端*/
		else
		{
			if (((CAB_A_TC1 == pIoOut_InputInfo->MainCtrlSide) && (CAB_B_TC2 == pIoOut_InputInfo->IdleSide)) || ((CAB_B_TC2 == pIoOut_InputInfo->MainCtrlSide) && (CAB_A_TC1 == pIoOut_InputInfo->IdleSide)))
			{
				/*有控制端*/
				/*控制端输出*/
				if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
				{
					CallFuncRtn = IoModule_SingleSideOutput_with_Ana(pIoOut_InputInfo->MainCtrlSide, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
				}
				else
				{
					CallFuncRtn = IoModule_SingleSideOutput(pIoOut_InputInfo->MainCtrlSide, (UINT8)CTRL_SIDE);
				}

				if (FUNC_SUCESS != CallFuncRtn)
				{
					FuncRtn |= FUNC_CODE6;
				}
				else
				{
					/* 输出正常*/
				}

				/*非控制端输出*/
				if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
				{
					pIoOut_InputInfo->dwCtrlSideAnaValue = OUT_ANA_INVALID_VALUE;
					pIoOut_InputInfo->dwCtrlSidePwmValue = OUT_PWM_INVALID_VALUE;

					CallFuncRtn = IoModule_SingleSideOutput_with_Ana(pIoOut_InputInfo->IdleSide, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
				}
				else
				{
					CallFuncRtn = IoModule_SingleSideOutput(pIoOut_InputInfo->IdleSide, (UINT8)IDLE_SIDE);
				}

				if (FUNC_SUCESS != CallFuncRtn)
				{
					FuncRtn |= FUNC_CODE7;
				}
				else
				{
					/* 输出正常*/
				}
			}
			else
			{
				/*无单个控制端时(包含两端都为控制端或者都不为控制端，或者采集异常时)，需对两端都以控制端进行输出*/
				if (1U == CC_PROJECT_USER)
				{
					/*唤醒输出激活TC1端*/
					if (CAB_A_TC1 == pIoOut_InputInfo->WakeCtrlIVOC)
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_A_TC1, CTRL_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE6;
						}
						else
						{
							/* 输出正常*/
						}
						/*非控制端输出*/
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_B_TC2, IDLE_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE7;
						}
						else
						{
							/* 输出正常*/
						}
					}
					/*唤醒输出激活TC2端*/
					else if (CAB_B_TC2 == pIoOut_InputInfo->WakeCtrlIVOC)
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_B_TC2, CTRL_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE6;
						}
						else
						{
							/* 输出正常*/
						}

						/*非控制端输出*/
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_A_TC1, IDLE_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE7;
						}
						else
						{
							/* 输出正常*/
						}
					}
					else
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE8;
						}
						else
						{
							/* 输出正常*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE9;
						}
						else
						{
							/* 输出正常*/
						}
					}
				}
				else if (1U == H_FAO_PROJECT_USER)
				{
					if ((0x01U == pIoOut_InputInfo->WakeCtrlIVOC) || (0x01U == pIoOut_InputInfo->ReverseTag))
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE10;
						}
						else
						{
							/* 输出正常*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE11;
						}
						else
						{
							/* 输出正常*/
						}
					}
					else if ((0x02U == pIoOut_InputInfo->WakeCtrlIVOC) || (0x02U == pIoOut_InputInfo->ReverseTag))
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE12;
						}
						else
						{
							/* 输出正常*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE13;
						}
						else
						{
							/* 输出正常*/
						}
					}
					else if (CBTC_TRUE == pIoOut_InputInfo->IOOutputStruCtrl.TRAIN_CAB_ACTIVE_OUTPUT)
					{
						/*驾驶室激活信号丢失*/
						if (CAB_A_TC1 == LastActiveState)
						{
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE14;
							}
							else
							{
								/* 输出正常*/
							}
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE15;
							}
							else
							{
								/* 输出正常*/
							}
						}
						else if (CAB_B_TC2 == LastActiveState)
						{
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE16;
							}
							else
							{
								/* 输出正常*/
							}
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE17;
							}
							else
							{
								/* 输出正常*/
							}
						}
						else
						{
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE18;
							}
							else
							{
								/* 输出正常*/
							}
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE19;
							}
							else
							{
								/* 输出正常*/
							}
						}
					}
					else
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE20;
						}
						else
						{
							/* 输出正常*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE21;
						}
						else
						{
							/* 输出正常*/
						}
					}
				}
				else
				{
					CallFuncRtn = IoModule_SingleSideOutput((UINT8)CAB_A_TC1, (UINT8)CTRL_SIDE);
					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE22;
					}
					else
					{
						/* 输出正常*/
					}

					CallFuncRtn = IoModule_SingleSideOutput((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE);
					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE23;
					}
					else
					{
						/* 输出正常*/
					}
				}
			}
		}

		if (FUNC_CODE1 == FuncRtn)
		{
			FuncRtn = FUNC_SUCESS;
		}
		else
		{
			/*无操作*/
		}
	}
	else
	{
		FuncRtn = FUNC_CODE24;
	}

	return FuncRtn;
}

/***************************************************************************************
* 功能描述:			分别用于ATP/ATO计算掩码值
* 输入参数:
* 输入输出参数:		无
* 输出参数:			ATP掩码，ATO掩码
* 全局变量:			无
* 返回值:			0x00：成功；
* 修改记录：		added by Jielu 20230326
****************************************************************************************/
void IoModulCalMask_Api_PreiodProcess(OUT UINT32 *atpMask, OUT UINT32 *atoMask, IN IO_CFG_DATA_STRU *IOConfigFunc)
{
	UINT8 Index = 0U; /*数据中的路数*/
	UINT8 IoTableIndex = 0U;  /*ATO非安全输出的路数*/
							  /* 遍历IO配置表中每一路的IO的含义 */
	UINT32 tmpAtpMask = 0xFFFFFFFFU; /*ATP 掩码，初始化为全1*/
	UINT32 tmpAtoMask = 0x00000000U; /*ATO 掩码，初始化为全1*/
	UINT8 boardNum = 0U;

	UINT8 bordIndex = 0U;

	if ((NULL != atpMask) && (NULL != atoMask) && (NULL != IOConfigFunc))
	{
		/*先找板卡数量*/
		boardNum = IOConfigFunc->BoardNum;

		for (bordIndex = 0U; bordIndex<boardNum; bordIndex++)
		{
			if (TRAIN_OUTPUT_BOARD == IOConfigFunc->BoardIoMsgBuf[bordIndex].BoxAttr)
			{
				for (Index = 0U; Index < (UINT8)MAX_UNSAFE_CHANNELS_NUM; Index++)
				{
					/* 遍历IO含义与输入结构体对应的描述表 */
					for (IoTableIndex = 0U; IoTableIndex < OUTPUT_ATO_NUM; IoTableIndex++)
					{
						/*如果相等，那么该位值ATO的掩码为1，ATP的掩码为0*/
						if (IOConfigFunc->BoardIoMsgBuf[bordIndex].GfgIoBuf[Index] == gIoOutputAtoIndexTable[IoTableIndex].IoOutputTypeIndex)
						{
							tmpAtoMask |= (0x01U << Index);
							tmpAtpMask &= ~(0x01U << Index);
							break;
						}
						else
						{
							/*无操作*/
						}
					}
				}
				break;
			}
			else
			{
				/*无操作*/
			}
		}

		*atpMask = tmpAtpMask;
		*atoMask = tmpAtoMask;
	}
	else
	{
		/*无操作*/
	}
}