/****************************************************************************************************
* 文件名   :  IOInput.c
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2018.08.23
* 作者     :  李淑娜
* 功能描述 : IO输入解析处理函数文件
* 使用注意 : 上层需先调用IoModuleInPut_Api_PreiodProcess(),再调用IoModuleInPut_Api_GetDigtalInput()
* 和IoModuleInPut_Api_GetMainCtrSide();
*
* 修改记录 : 优化，增加接口函数。mjz-2018.10.28.
*
**************************************************************************************************/
#include "IOInput.h"
#include "IOModuleExtern.h"
#include "CommDataSourceExtern.h"
#include "CommABChooseExtern.h"
#include "dquVobcConfigData.h"
#include "PLFM_LOG.h"
#include "IOConfig.h"

IO_CFG_DATA_STRU	gIoConfigDataStru = { 0U };       /* IO配置数据结构体(输入和输出共用一个结构体)，不需要同步*/
IO_INPUT_STRUCT		gIOInputStruMscp = { 0U };        /* IO输入结构体用于对应描述表，周期更新，不需要同步 */
IO_INPUT_STRUCT		gTc1IoInputStru = { 0U };	        /* 从1端接收到的IO数据的结构体，周期更新，不需要同步*/
IO_INPUT_STRUCT		gTc2IoInputStru = { 0U };         /* 从2端接收到的IO数据的结构体，周期更新，不需要同步*/
IO_INPUT_STRUCT     gLastTc1InputStru = { 0U };       /*上周期一端采集结果（安全输入已经组合完毕），需要同步*/
IO_INPUT_STRUCT     gLastTc2InputStru = { 0U };       /*上周期二端采集结果（安全输入已经组合完毕），需要同步*/

IO_SINGLE_INPUT_STRUCT gIoSingleInput = { 0U };          /*IO采集单节点采集，周期更新，不需要同步*/
UINT8 *gIoSingleMapTable[MAX_CHANNELS_NUM] = { 0U };     /*IO映射表，上电时进行映射，不需要同步*/

UINT8 RecvTc1IoBuff[256] = { 0U, }; /* 保存收到的TC1端IO的原始数据流 不需要同步*/
UINT8 RecvTc2IoBuff[256] = { 0U, }; /*保存收到的Tc2端IO的原始数据流 不需要同步*/
static UINT8 TC1cablastState = 0xAAU; /* 用于存储上周期的激活状态 需要同步*/
static UINT8 TC2cablastState = 0xAAU; /* 用于存储上周期的激活状态 需要同步*/

/*当前周期计数统一使用gPeriodSn*/
static DIGITAL_IO_Time_MSCP gPeriodSn_Mscp_TC1[MAX_SAFE_INPUT_NUM] =
{
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U }
};								/*调用周期计数，需要同步*/

static DIGITAL_IO_Time_MSCP gPeriodSn_Mscp_TC2[MAX_SAFE_INPUT_NUM] =
{
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U }
};                              /*调用周期计数，需要同步*/

static UINT32 gPeriodSn = 0U;	/*模块代码运行周期计数，需要同步*/

extern UINT8 ARuptime;

UINT8 gLastTc1_Cab_Key = CBTC_FALSE;
UINT8 Tc1_Tolerate_tic = 0U;
UINT8 gLastTc2_Cab_Key = CBTC_FALSE;
UINT8 Tc2_Tolerate_tic = 0U;
UINT8 gARReverStrTag = CBTC_FALSE;      /*进入AR模式折返的开始标志*/
UINT8 gFamDirBackTag = CBTC_FALSE;      /*FAM方向向后标志*/
UINT8 gFamCtrlSide = CBTC_FALSE;
UINT8 gFamReverStrTag = CBTC_FALSE;

/* 将输入开关量含义与输入结构体关联的描述表 */
IO_INPUT_INDEX gIoInputIndexTable[INPUT_FLAG_NUM_SINGLE] =
{
	/* 安全输入对应表 */
	{ IN_TRAIN_CAB_ACTIVE_MSCP_NC,&(gIoSingleInput.InTrainCabActiveMscpNc) },
	{ IN_TRAIN_CAB_ACTIVE_MSCP_NO,&(gIoSingleInput.InTrainCabActiveMscpNo) },
	{ IN_TRAIN_EB_ENABLE_MSCP_NC,&(gIoSingleInput.InTrainEbEnableMscpNc) },
	{ IN_TRAIN_EB_ENABLE_MSCP_NO,&(gIoSingleInput.InTrainEbEnableMscpNo) },
	{ IN_TRAIN_TRACTION_CUT_MSCP_NC,&(gIoSingleInput.InTrainTractionCutMscpNc) },
	{ IN_TRAIN_TRACTION_CUT_MSCP_NO,&(gIoSingleInput.InTrainTractionCutMscpNo) },
	{ IN_TRAIN_DIR_FORWARD_MCSP_NC,&(gIoSingleInput.InTrainDirForwardMscpNc) },
	{ IN_TRAIN_DIR_FORWARD_MCSP_NO,&(gIoSingleInput.InTrainDirForwardMscpNo) },
	{ IN_TRAIN_DIR_BACKWARD_MSCP_NC,&(gIoSingleInput.InTrainDirBackwardMscpNc) },
	{ IN_TRAIN_DIR_BACKWARD_MSCP_NO,&(gIoSingleInput.InTrainDirBackwardMscpNo) },
	{ IN_TRAIN_HANDLE_TRACTION_MSCP_NC,&(gIoSingleInput.InTrainHandleTractionMscpNc) },
	{ IN_TRAIN_HANDLE_TRACTION_MSCP_NO,&(gIoSingleInput.InTrainHandleTractionMscpNo) },
	{ IN_TRAIN_ATP_CONTROL_MSCP_NC,&(gIoSingleInput.InTrainAtpControlMscpNc) },
	{ IN_TRAIN_ATP_CONTROL_MSCP_NO,&(gIoSingleInput.InTrainAtpControlMscpNo) },
	{ IN_BCU_IN_ATP_MSCP_NC,&(gIoSingleInput.InTrainBcuInAtpMscpNc) },
	{ IN_BCU_IN_ATP_MSCP_NO,&(gIoSingleInput.InTrainBcuInAtpMscpNo) },
	{ IN_TRAIN_AIR_FILL_MSCP_NC,&(gIoSingleInput.InTrainAirFillMscpNc) },
	{ IN_TRAIN_AIR_FILL_MSCP_NO,&(gIoSingleInput.InTrainAirFillMscpNo) },
	{ IN_LKJ_BRAKE_STATE_MSCP_NC,&(gIoSingleInput.InLkjBrakeStateMscpNc) },
	{ IN_LKJ_BRAKE_STATE_MSCP_NO,&(gIoSingleInput.InLkjBrakeStateMscpNo) },
	{ IN_BCU_IN_LKJ_MSCP_NC,&(gIoSingleInput.InTrainBcuInLkjMscpNc) },
	{ IN_BCU_IN_LKJ_MSCP_NO,&(gIoSingleInput.InTrainBcuInLkjMscpNo) },
	{ IN_TRAIN_OPEN_FSB_VALVE_MSCP_NC,&(gIoSingleInput.InTrainOpenFsbValveMscpNc) },
	{ IN_TRAIN_OPEN_FSB_VALVE_MSCP_NO,&(gIoSingleInput.InTrainOpenFsbValveMscpNo) },
	{ IN_TRAIN_ALLOW_ENTRY_ATO_MSCP_NC,&(gIoSingleInput.InTrainAllowEntryAtoMscpNc) },
	{ IN_TRAIN_ALLOW_ENTRY_ATO_MSCP_NO,&(gIoSingleInput.InTrainAllowEntryAtoMscpNo) },
	{ IN_TRAIN_LKJ_CONTRO_MSCP_NC,&(gIoSingleInput.InTrainLkjControlMscpNc) },
	{ IN_TRAIN_LKJ_CONTRO_MSCP_NO,&(gIoSingleInput.InTrainLkjControlMscpNo) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL1_MSCP_NC,&(gIoSingleInput.InTrainElectricBrakeChanl1MscpNc) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL1_MSCP_NO,&(gIoSingleInput.InTrainElectricBrakeChanl1MscpNo) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL2_MSCP_NC,&(gIoSingleInput.InTrainElectricBrakeChanl2MscpNc) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL2_MSCP_NO,&(gIoSingleInput.InTrainElectricBrakeChanl2MscpNo) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL3_MSCP_NC,&(gIoSingleInput.InTrainElectricBrakeChanl3MscpNc) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL3_MSCP_NO,&(gIoSingleInput.InTrainElectricBrakeChanl3MscpNo) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL4_MSCP_NC,&(gIoSingleInput.InTrainElectricBrakeChanl4MscpNc) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL4_MSCP_NO,&(gIoSingleInput.InTrainElectricBrakeChanl4MscpNo) },
	{ IN_TRAIN_CONFIRM_BUTTON_MSCP_NC,&(gIoSingleInput.InTrainConfirmButtonMscpNc) },
	{ IN_TRAIN_CONFIRM_BUTTON_MSCP_NO,&(gIoSingleInput.InTrainConfirmButtonMscpNo) },
	{ IN_ATP_TRAIN_INTEG_MSCP_NC,&(gIoSingleInput.InAtpTrainIntegMscpNc) },
	{ IN_ATP_TRAIN_INTEG_MSCP_NO,&(gIoSingleInput.InAtpTrainIntegMscpNo) },
	{ IN_ATP_BCZD_VALID_MSCP_NC,&(gIoSingleInput.InAtpBczdValidMscpNc) },
	{ IN_ATP_BCZD_VALID_MSCP_NO,&(gIoSingleInput.InAtpBczdValidMscpNo) },
	{ IN_ATP_SB_RIGHT_MSCP_NC,&(gIoSingleInput.InAtpSbRightInputMscpNc) },
	{ IN_ATP_SB_RIGHT_MSCP_NO,&(gIoSingleInput.InAtpSbRightInputMscpNo) },
	{ IN_TRAIN_DOORCLOSE_MSCP_NC,&(gIoSingleInput.InTrainDoorCloseMscpNc) },
	{ IN_TRAIN_DOORCLOSE_MSCP_NO,&(gIoSingleInput.InTrainDoorCloseMscpNo) },
	{ IN_TRAIN_EMERDOORBOARD_MSCP_NC,&(gIoSingleInput.InTrainEmergencyDoorBoardMscpNc) },
	{ IN_TRAIN_EMERDOORBOARD_MSCP_NO,&(gIoSingleInput.InTrainEmergencyDoorBoardMscpNo) },
	{ IN_TRAIN_KEY_OPEN_MSCP_NC, &(gIoSingleInput.TRAIN_KEY_OPENMscpNc) },
	{ IN_TRAIN_KEY_OPEN_MSCP_NO, &(gIoSingleInput.TRAIN_KEY_OPENMscpNo) },
	{ IN_ATP_DOOR_CLOSEDANDLOCK_MSCP_NC,&(gIoSingleInput.InTrainDoorCloseAndLockMscpNc) },
	{ IN_ATP_DOOR_CLOSEDANDLOCK_MSCP_NO,&(gIoSingleInput.InTrainDoorCloseAndLockMscpNo) },
	{ IN_TRAIN_RESECT_SWITCH_MSCP_NC,&(gIoSingleInput.InTrainResectSwitchMscpNc) },
	{ IN_TRAIN_RESECT_SWITCH_MSCP_NO,&(gIoSingleInput.InTrainResectSwitchMscpNo) },
	{ IN_TRAIN_EBHANDLE_ACTIVE_MSCP_NC, &(gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNc) },
	{ IN_TRAIN_EBHANDLE_ACTIVE_MSCP_NO, &(gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNo) },
	{ IN_TRAIN_OBSTDET_INPUT_MSCP_NC, &(gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNc) },
	{ IN_TRAIN_OBSTDET_INPUT_MSCP_NO, &(gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNo) },
	{ IN_DOUBLE1_DOORCLOSELOCK_MSCP_NC,&(gIoSingleInput.InDouble1DoorCloseLockMscpNc) },
	{ IN_DOUBLE1_DOORCLOSELOCK_MSCP_NO,&(gIoSingleInput.InDouble1DoorCloseLockMscpNo) },
	{ IN_TRAIN_BRAKE_FAULT_MSCP_NC, &(gIoSingleInput.TRAIN_BRAKE_FAULTMscpNc) },
	{ IN_TRAIN_BRAKE_FAULT_MSCP_NO, &(gIoSingleInput.TRAIN_BRAKE_FAULTMscpNo) },
	{ IN_ATP_TRACTION_RIGHT_MSCP_NC, &(gIoSingleInput.ATP_TRACTION_RIGHTMscpNc) },
	{ IN_ATP_TRACTION_RIGHT_MSCP_NO, &(gIoSingleInput.ATP_TRACTION_RIGHTMscpNo) },
	{ IN_EMERGENCYDOOR_SINGLE_MSCP_NC, &(gIoSingleInput.EMERGENCYDOORMscpNc) },
	{ IN_EMERGENCYDOOR_SINGLE_MSCP_NO, &(gIoSingleInput.EMERGENCYDOORMscpNo) },
	{ IN_CM_MODE_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpCMModeMscpNc) },
	{ IN_CM_MODE_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpCMModeMscpNo) },
	{ IN_NONSD_MODE_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpNotSDModeMscpNc) },
	{ IN_NONSD_MODE_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpNotSDModeMscpNo) },
	{ IN_FWD_MODE_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpFORModeMscpNc) },
	{ IN_FWD_MODE_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpFORModeMscpNo) },
	{ IN_REV_MODE_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpREVModeMscpNc) },
	{ IN_REV_MODE_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpREVModeMscpNo) },
	{ IN_STBY_MODE_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpSBModeMscpNc) },
	{ IN_STBY_MODE_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpSBModeMscpNo) },
	{ IN_SD_MODE_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpSDModeMscpNc) },
	{ IN_SD_MODE_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpSDModeMscpNo) },
	{ IN_EB_FEEDBACK_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpEBFeedbackMscpNc) },
	{ IN_EB_FEEDBACK_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpEBFeedbackMscpNo) },
	{ IN_CONDUCT_ZERO_SPEED_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpConductZeroSpeedMscpNc) },
	{ IN_CONDUCT_ZERO_SPEED_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpConductZeroSpeedMscpNo) },
	{ IN_SAFE_INTERLOCK_FEEDBACK_SINGLE_MSCP_NC, &(gIoSingleInput.InAtpSafeInterLockFeedbackMscpNc) },
	{ IN_SAFE_INTERLOCK_FEEDBACK_SINGLE_MSCP_NO, &(gIoSingleInput.InAtpSafeInterLockFeedbackMscpNo) },
	/*-------------以上为安全输入---------以下为非安全输入------------*/
	{ IN_ATP_ATO_START1_SINGLE,&(gIOInputStruMscp.InAtpAtoStart1) },
	{ IN_ATP_MODE_UP_SINGLE,&(gIOInputStruMscp.InAtpModeUp) },
	{ IN_ATP_MODE_DOWN_SINGLE,&(gIOInputStruMscp.InAtpModeDown) },
	{ IN_ATP_AR_SINGLE,&(gIOInputStruMscp.InAtpArInput) },
	{ IN_ATP_RIGHT_DOOR_OPEN_SINGLE,&(gIOInputStruMscp.InAtpRightDoorOpen) },
	{ IN_ATP_RIGHT_DOOR_CLOSE_SINGLE,&(gIOInputStruMscp.InAtpRightDoorClose) },
	{ IN_ATP_EUM_SWITCH_SINGLE,&(gIOInputStruMscp.InAtpEumSwitch) },
	{ IN_ATP_LEFT_DOOR_OPEN_SINGLE,&(gIOInputStruMscp.InAtpLeftDoorOpen) },
	{ IN_ATP_LEFT_DOOR_CLOSE_SINGLE,&(gIOInputStruMscp.InAtpLeftDoorClose) },
	{ IN_ATP_ATO_START2_SINGLE,&(gIOInputStruMscp.InAtpAtoStart2) },
	{ IN_ATP_SLEEP_SINGLE,&(gIOInputStruMscp.InAtpSleep) },
	{ IN_TRAIN_NOBODY_ALERT_SIGNAL_SINGLE,&(gIOInputStruMscp.InTrainNobodyAlertSignal) },
	{ IN_TRAIN_WHISTLE_SIGNAL_SINGLE,&(gIOInputStruMscp.InTrainWhistleSignal) },
	{ IN_ATP_TVS1_Check_SINGLE,&(gIOInputStruMscp.InAtpTvs1Check) },
	{ IN_ATP_TVS2_Check_SINGLE,&(gIOInputStruMscp.InAtpTvs2Check) },
	{ IN_CHANGE_SWITCH_LKJ_SINGLE,&(gIOInputStruMscp.InChangeSwitchLkj) },
	{ IN_CHANGE_SWITCH_AUTO_SINGLE,&(gIOInputStruMscp.InChangeSwitchAuto) },
	{ IN_TRAIN_FSB_1_SINGLE,&(gIOInputStruMscp.InTrainFsb1) },
	{ IN_TRAIN_FSB_2_SINGLE,&(gIOInputStruMscp.InTrainFsb2) },
	{ IN_TRAIN_FSB_120KPA_SINGLE,&(gIOInputStruMscp.InTrainFsb120Kpa) },
	{ IN_LKJ_EB_ENABLE_SINGLE,&(gIOInputStruMscp.InLkjEbEnable) },
	{ IN_TRAIN_CUT_SWITCH_SINGLE,&(gIOInputStruMscp.InTrainCutSwitch) },
	{ IN_TRAIN_RELIEVE_SINGLE,&(gIOInputStruMscp.InTrainRelieve) },
	{ IN_ATP_HANDLE_EB_SINGLE,&(gIOInputStruMscp.InHandleEB) },
	/*--------------以下为迪士尼安全&非安全输入------------*/
	{ IN_IVOC_VALIDITY_SINGLE, &(gIOInputStruMscp.InAtpIVOCValidity) },
	/* 朔黄新增的确认按钮2，现在宏为0x70,后续会修改成0x3B和现在数据一致*/
	{ IN_TRAIN_CONFIRM_BUTTON2_SINGLE,&(gIOInputStruMscp.InAtpConfirmButton2) },
	{ IN_ATP_SMOKE_ALARM_SINGLE, &(gIOInputStruMscp.ATP_SMOKE_ALARM) },
	{ IN_ATP_DOOR_MM_SINGLE, &(gIOInputStruMscp.ATP_DOOR_MM) },
	{ IN_ATP_DOOR_AM_SINGLE, &(gIOInputStruMscp.ATP_DOOR_AM) },
	{ IN_ATP_DOOR_AA_SINGLE, &(gIOInputStruMscp.ATP_DOOR_AA) },
	{ IN_EBI1_FEEDBACK_SINGLE, &(gIOInputStruMscp.EBI1_FEEDBACK) },
	{ IN_EBI2_FEEDBACK_SINGLE, &(gIOInputStruMscp.EBI2_FEEDBACK) },
	{ IN_TVS1CHECK_SINGLE, &(gIOInputStruMscp.TVS1CHECK) },
	{ IN_TVS2CHECK_SINGLE, &(gIOInputStruMscp.TVS2CHECK) },
	{ IN_DOORALLCLOSE_SINGLE, &(gIOInputStruMscp.DOORALLCLOSE) },
	/*--------------以下为成都四线非安全输入------------*/
	{ IN_TRAIN_REDUNDANCY_SINGLE,&(gIOInputStruMscp.InTrainRedundancy) },
	{ IN_TRAIN_FAN1_SINGLE,&(gIOInputStruMscp.InTrainFan1) },
	{ IN_TRAIN_FAN2_SINGLE,&(gIOInputStruMscp.InTrainFan2) },
	{ IN_TRAIN_DOORLOCK_SINGLE,&(gIOInputStruMscp.InTrainDoorLock) },
	{ IN_TRAIN_ESCAPDOOR_UNLOCK_SINGLE,&(gIOInputStruMscp.InTrainEscapDoorUnlock) },
	{ IN_TRAIN_GUESTDOOR_UNLOCK_SINGLE,&(gIOInputStruMscp.InTrainGuestDoorUnlock) },
	{ IN_TRAIN_Reserved,&(gIoSingleInput.reservedInput) }
};


/* 将TC1端输入开关量含义与输入结构体关联的描述表 */
IO_INPUT_INDEX gTc1InputIndexTable[INPUT_FLAG_NUM] =
{
	/* 安全输入对应表 */
	/*驾驶室激活必须放在第一个，如需移动，需改写IoModuleInPut_Output函数中获取驾驶室激活的方式*/
	{ IN_TRAIN_CAB_ACTIVE,&(gTc1IoInputStru.InTrainCabActive) },
	{ IN_TRAIN_EB_ENABLE,&(gTc1IoInputStru.InTrainEbEnable) },
	{ IN_TRAIN_TRACTION_CUT,&(gTc1IoInputStru.InTrainTractionCut) },
	{ IN_TRAIN_DIR_FORWARD,&(gTc1IoInputStru.InTrainDirForward) },
	{ IN_TRAIN_DIR_BACKWARD,&(gTc1IoInputStru.InTrainDirBackward) },
	{ IN_TRAIN_HANDLE_TRACTION,&(gTc1IoInputStru.InTrainHandleTraction) },
	{ IN_TRAIN_ATP_CONTROL,&(gTc1IoInputStru.InTrainAtpControl) },
	{ IN_BCU_IN_ATP,&(gTc1IoInputStru.InTrainBcuInAtp) },
	{ IN_TRAIN_AIR_FILL,&(gTc1IoInputStru.InTrainAirFill) },
	{ IN_LKJ_BRAKE_STATE,&(gTc1IoInputStru.InLkjBrakeState) },
	{ IN_BCU_IN_LKJ,&(gTc1IoInputStru.InTrainBcuInLkj) },
	{ IN_TRAIN_OPEN_FSB_VALVE,&(gTc1IoInputStru.InTrainOpenFsbValve) },
	{ IN_TRAIN_ALLOW_ENTRY_ATO,&(gTc1IoInputStru.InTrainAllowEntryAto) },
	{ IN_TRAIN_LKJ_CONTROL,&(gTc1IoInputStru.InTrainLkjControl) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL1,&(gTc1IoInputStru.InTrainElectricBrakeChanl1) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL2,&(gTc1IoInputStru.InTrainElectricBrakeChanl2) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL3,&(gTc1IoInputStru.InTrainElectricBrakeChanl3) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL4,&(gTc1IoInputStru.InTrainElectricBrakeChanl4) },
	{ IN_TRAIN_CONFIRM_BUTTON,&(gTc1IoInputStru.InTrainConfirmButton) },
	{ IN_TRAIN_RESECT_SWITCH,&(gTc1IoInputStru.InTrainResectSwitch) },
	{ IN_ATP_DOOR_CLOSEDANDLOCK,&(gTc1IoInputStru.InTrainDoorCloseAndLock) },

	/*-------------以上为安全输入---------以下为非安全输入------------*/
	{ IN_ATP_ATO_START1,&(gTc1IoInputStru.InAtpAtoStart1) },
	{ IN_ATP_MODE_UP,&(gTc1IoInputStru.InAtpModeUp) },
	{ IN_ATP_MODE_DOWN,&(gTc1IoInputStru.InAtpModeDown) },
	{ IN_ATP_AR,&(gTc1IoInputStru.InAtpArInput) },
	{ IN_ATP_RIGHT_DOOR_OPEN,&(gTc1IoInputStru.InAtpRightDoorOpen) },
	{ IN_ATP_RIGHT_DOOR_CLOSE,&(gTc1IoInputStru.InAtpRightDoorClose) },
	{ IN_ATP_EUM_SWITCH,&(gTc1IoInputStru.InAtpEumSwitch) },
	{ IN_ATP_LEFT_DOOR_OPEN,&(gTc1IoInputStru.InAtpLeftDoorOpen) },
	{ IN_ATP_LEFT_DOOR_CLOSE,&(gTc1IoInputStru.InAtpLeftDoorClose) },
	{ IN_ATP_ATO_START2,&(gTc1IoInputStru.InAtpAtoStart2) },
	{ IN_ATP_TRAIN_INTEG,&(gTc1IoInputStru.InAtpTrainInteg) },
	{ IN_ATP_BCZD_VALID,&(gTc1IoInputStru.InAtpBczdValid) },
	{ IN_ATP_SB_RIGHT,&(gTc1IoInputStru.InAtpSbRightInput) },
	{ IN_ATP_SLEEP,&(gTc1IoInputStru.InAtpSleep) },
	{ IN_TRAIN_NOBODY_ALERT_SIGNAL,&(gTc1IoInputStru.InTrainNobodyAlertSignal) },
	{ IN_TRAIN_WHISTLE_SIGNAL,&(gTc1IoInputStru.InTrainWhistleSignal) },
	{ IN_ATP_TVS1_Check,&(gTc1IoInputStru.InAtpTvs1Check) },
	{ IN_ATP_TVS2_Check,&(gTc1IoInputStru.InAtpTvs2Check) },
	{ IN_CHANGE_SWITCH_LKJ,&(gTc1IoInputStru.InChangeSwitchLkj) },
	{ IN_CHANGE_SWITCH_AUTO,&(gTc1IoInputStru.InChangeSwitchAuto) },
	{ IN_TRAIN_FSB_1,&(gTc1IoInputStru.InTrainFsb1) },
	{ IN_TRAIN_FSB_2,&(gTc1IoInputStru.InTrainFsb2) },
	{ IN_TRAIN_FSB_120KPA,&(gTc1IoInputStru.InTrainFsb120Kpa) },
	{ IN_LKJ_EB_ENABLE,&(gTc1IoInputStru.InLkjEbEnable) },
	{ IN_TRAIN_CUT_SWITCH,&(gTc1IoInputStru.InTrainCutSwitch) },
	{ IN_TRAIN_RELIEVE,&(gTc1IoInputStru.InTrainRelieve) },
	{ IN_ATP_HANDLE_EB,&(gTc1IoInputStru.InHandleEB) },

	/*--------------以下为迪士尼安全&非安全输入------------*/
	{ IN_CM_MODE, &(gTc1IoInputStru.InAtpCMMode) },
	{ IN_NONSD_MODE, &(gTc1IoInputStru.InAtpNotSDMode) },
	{ IN_FWD_MODE, &(gTc1IoInputStru.InAtpFORMode) },
	{ IN_REV_MODE, &(gTc1IoInputStru.InAtpREVMode) },
	{ IN_STBY_MODE, &(gTc1IoInputStru.InAtpSBMode) },
	{ IN_SD_MODE, &(gTc1IoInputStru.InAtpSDMode) },
	{ IN_EB_FEEDBACK, &(gTc1IoInputStru.InAtpEBFeedback) },
	{ IN_CONDUCT_ZERO_SPEED, &(gTc1IoInputStru.InAtpConductZeroSpeed) },
	{ IN_SAFE_INTERLOCK_FEEDBACK, &(gTc1IoInputStru.InAtpSafeInterLockFeedback) },
	{ IN_IVOC_VALIDITY, &(gTc1IoInputStru.InAtpIVOCValidity) },
	/* 朔黄新增的确认按钮2，现在宏为0x70,后续会修改成0x3B和现在数据一致*/
	{ IN_TRAIN_CONFIRM_BUTTON2,&(gTc1IoInputStru.InAtpConfirmButton2) },

	{ IN_TRAIN_KEY_OPEN, &(gTc1IoInputStru.TRAIN_KEY_OPEN) },
	{ IN_TRAIN_EBHANDLE_ACTIVE, &(gTc1IoInputStru.TRAIN_EBHANDLE_ACTIVE) },
	{ IN_TRAIN_OBSTDET_INPUT, &(gTc1IoInputStru.TRAIN_OBSTDET_INPUT) },
	{ IN_TRAIN_BRAKE_FAULT, &(gTc1IoInputStru.TRAIN_BRAKE_FAULT) },
	{ IN_ATP_TRACTION_RIGHT, &(gTc1IoInputStru.ATP_TRACTION_RIGHT) },
	{ IN_ATP_SMOKE_ALARM, &(gTc1IoInputStru.ATP_SMOKE_ALARM) },
	{ IN_ATP_DOOR_MM, &(gTc1IoInputStru.ATP_DOOR_MM) },
	{ IN_ATP_DOOR_AM, &(gTc1IoInputStru.ATP_DOOR_AM) },
	{ IN_ATP_DOOR_AA, &(gTc1IoInputStru.ATP_DOOR_AA) },
	{ IN_EBI1_FEEDBACK, &(gTc1IoInputStru.EBI1_FEEDBACK) },
	{ IN_EBI2_FEEDBACK, &(gTc1IoInputStru.EBI2_FEEDBACK) },
	{ IN_EMERGENCYDOOR, &(gTc1IoInputStru.EMERGENCYDOOR) },
	{ IN_TVS1CHECK, &(gTc1IoInputStru.TVS1CHECK) },
	{ IN_TVS2CHECK, &(gTc1IoInputStru.TVS2CHECK) },
	{ IN_DOORALLCLOSE, &(gTc1IoInputStru.DOORALLCLOSE) },

	/*--------------以下为成都四线安全&非安全输入------------*/
	{ IN_TRAIN_DOORCLOSE,&(gTc1IoInputStru.InTrainDoorClose) },
	{ IN_TRAIN_EMERDOORBOARD,&(gTc1IoInputStru.InTrainEmergencyDoorBoard) },
	{ IN_TRAIN_REDUNDANCY,&(gTc1IoInputStru.InTrainRedundancy) },
	{ IN_TRAIN_FAN1,&(gTc1IoInputStru.InTrainFan1) },
	{ IN_TRAIN_FAN2,&(gTc1IoInputStru.InTrainFan2) },
	{ IN_TRAIN_DOORLOCK,&(gTc1IoInputStru.InTrainDoorLock) },
	{ IN_TRAIN_ESCAPDOOR_UNLOCK,&(gTc1IoInputStru.InTrainEscapDoorUnlock) },
	{ IN_TRAIN_GUESTDOOR_UNLOCK,&(gTc1IoInputStru.InTrainGuestDoorUnlock) },
};

/* 将TC2端输入开关量含义与输入结构体关联的描述表 */
IO_INPUT_INDEX gTc2InputIndexTable[INPUT_FLAG_NUM] =
{
	/* 安全输入对应表 */
	/*驾驶室激活必须放在第一个，如需移动，需改写IoModuleInPut_Output函数中获取驾驶室激活的方式*/
	{ IN_TRAIN_CAB_ACTIVE,&(gTc2IoInputStru.InTrainCabActive) },
	{ IN_TRAIN_EB_ENABLE,&(gTc2IoInputStru.InTrainEbEnable) },
	{ IN_TRAIN_TRACTION_CUT,&(gTc2IoInputStru.InTrainTractionCut) },
	{ IN_TRAIN_DIR_FORWARD,&(gTc2IoInputStru.InTrainDirForward) },
	{ IN_TRAIN_DIR_BACKWARD,&(gTc2IoInputStru.InTrainDirBackward) },
	{ IN_TRAIN_HANDLE_TRACTION,&(gTc2IoInputStru.InTrainHandleTraction) },
	{ IN_TRAIN_ATP_CONTROL,&(gTc2IoInputStru.InTrainAtpControl) },
	{ IN_BCU_IN_ATP,&(gTc2IoInputStru.InTrainBcuInAtp) },
	{ IN_TRAIN_AIR_FILL,&(gTc2IoInputStru.InTrainAirFill) },
	{ IN_LKJ_BRAKE_STATE,&(gTc2IoInputStru.InLkjBrakeState) },
	{ IN_BCU_IN_LKJ,&(gTc2IoInputStru.InTrainBcuInLkj) },
	{ IN_TRAIN_OPEN_FSB_VALVE,&(gTc2IoInputStru.InTrainOpenFsbValve) },
	{ IN_TRAIN_ALLOW_ENTRY_ATO,&(gTc2IoInputStru.InTrainAllowEntryAto) },
	{ IN_TRAIN_LKJ_CONTROL,&(gTc2IoInputStru.InTrainLkjControl) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL1,&(gTc2IoInputStru.InTrainElectricBrakeChanl1) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL2,&(gTc2IoInputStru.InTrainElectricBrakeChanl2) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL3,&(gTc2IoInputStru.InTrainElectricBrakeChanl3) },
	{ IN_TRAIN_ELECTRIC_BRAKE_CHANL4,&(gTc2IoInputStru.InTrainElectricBrakeChanl4) },
	{ IN_TRAIN_CONFIRM_BUTTON,&(gTc2IoInputStru.InTrainConfirmButton) },
	{ IN_TRAIN_RESECT_SWITCH,&(gTc2IoInputStru.InTrainResectSwitch) },
	{ IN_ATP_DOOR_CLOSEDANDLOCK,&(gTc2IoInputStru.InTrainDoorCloseAndLock) },

	/*-------------以上为安全输入---------以下为非安全输入------------*/
	{ IN_ATP_ATO_START1,&(gTc2IoInputStru.InAtpAtoStart1) },
	{ IN_ATP_MODE_UP,&(gTc2IoInputStru.InAtpModeUp) },
	{ IN_ATP_MODE_DOWN,&(gTc2IoInputStru.InAtpModeDown) },
	{ IN_ATP_AR,&(gTc2IoInputStru.InAtpArInput) },
	{ IN_ATP_RIGHT_DOOR_OPEN,&(gTc2IoInputStru.InAtpRightDoorOpen) },
	{ IN_ATP_RIGHT_DOOR_CLOSE,&(gTc2IoInputStru.InAtpRightDoorClose) },
	{ IN_ATP_EUM_SWITCH,&(gTc2IoInputStru.InAtpEumSwitch) },
	{ IN_ATP_LEFT_DOOR_OPEN,&(gTc2IoInputStru.InAtpLeftDoorOpen) },
	{ IN_ATP_LEFT_DOOR_CLOSE,&(gTc2IoInputStru.InAtpLeftDoorClose) },
	{ IN_ATP_ATO_START2,&(gTc2IoInputStru.InAtpAtoStart2) },
	{ IN_ATP_TRAIN_INTEG,&(gTc2IoInputStru.InAtpTrainInteg) },
	{ IN_ATP_BCZD_VALID,&(gTc2IoInputStru.InAtpBczdValid) },
	{ IN_ATP_SB_RIGHT,&(gTc2IoInputStru.InAtpSbRightInput) },
	{ IN_ATP_SLEEP,&(gTc2IoInputStru.InAtpSleep) },
	{ IN_TRAIN_NOBODY_ALERT_SIGNAL,&(gTc2IoInputStru.InTrainNobodyAlertSignal) },
	{ IN_TRAIN_WHISTLE_SIGNAL,&(gTc2IoInputStru.InTrainWhistleSignal) },
	{ IN_ATP_TVS1_Check,&(gTc2IoInputStru.InAtpTvs1Check) },
	{ IN_ATP_TVS2_Check,&(gTc2IoInputStru.InAtpTvs2Check) },
	{ IN_CHANGE_SWITCH_LKJ,&(gTc2IoInputStru.InChangeSwitchLkj) },
	{ IN_CHANGE_SWITCH_AUTO,&(gTc2IoInputStru.InChangeSwitchAuto) },
	{ IN_TRAIN_FSB_1,&(gTc2IoInputStru.InTrainFsb1) },
	{ IN_TRAIN_FSB_2,&(gTc2IoInputStru.InTrainFsb2) },
	{ IN_TRAIN_FSB_120KPA,&(gTc2IoInputStru.InTrainFsb120Kpa) },
	{ IN_LKJ_EB_ENABLE,&(gTc2IoInputStru.InLkjEbEnable) },
	{ IN_TRAIN_CUT_SWITCH,&(gTc2IoInputStru.InTrainCutSwitch) },
	{ IN_TRAIN_RELIEVE,&(gTc2IoInputStru.InTrainRelieve) },
	{ IN_ATP_HANDLE_EB,&(gTc2IoInputStru.InHandleEB) },
	/*--------------以下为迪士尼安全&非安全输入------------*/
	{ IN_CM_MODE, &(gTc2IoInputStru.InAtpCMMode) },
	{ IN_NONSD_MODE, &(gTc2IoInputStru.InAtpNotSDMode) },
	{ IN_FWD_MODE, &(gTc2IoInputStru.InAtpFORMode) },
	{ IN_REV_MODE, &(gTc2IoInputStru.InAtpREVMode) },
	{ IN_STBY_MODE, &(gTc2IoInputStru.InAtpSBMode) },
	{ IN_SD_MODE, &(gTc2IoInputStru.InAtpSDMode) },
	{ IN_EB_FEEDBACK, &(gTc2IoInputStru.InAtpEBFeedback) },
	{ IN_CONDUCT_ZERO_SPEED, &(gTc2IoInputStru.InAtpConductZeroSpeed) },
	{ IN_SAFE_INTERLOCK_FEEDBACK, &(gTc2IoInputStru.InAtpSafeInterLockFeedback) },
	{ IN_IVOC_VALIDITY, &(gTc2IoInputStru.InAtpIVOCValidity) },
	/* 朔黄新增的确认按钮2，现在宏为0x70,后续会修改成0x3B和现在数据一致*/
	{ IN_TRAIN_CONFIRM_BUTTON2,&(gTc2IoInputStru.InAtpConfirmButton2) },
	{ IN_TRAIN_KEY_OPEN, &(gTc2IoInputStru.TRAIN_KEY_OPEN) },
	{ IN_TRAIN_EBHANDLE_ACTIVE, &(gTc2IoInputStru.TRAIN_EBHANDLE_ACTIVE) },
	{ IN_TRAIN_OBSTDET_INPUT, &(gTc2IoInputStru.TRAIN_OBSTDET_INPUT) },
	{ IN_TRAIN_BRAKE_FAULT, &(gTc2IoInputStru.TRAIN_BRAKE_FAULT) },
	{ IN_ATP_TRACTION_RIGHT, &(gTc2IoInputStru.ATP_TRACTION_RIGHT) },
	{ IN_ATP_SMOKE_ALARM, &(gTc2IoInputStru.ATP_SMOKE_ALARM) },
	{ IN_ATP_DOOR_MM, &(gTc2IoInputStru.ATP_DOOR_MM) },
	{ IN_ATP_DOOR_AM, &(gTc2IoInputStru.ATP_DOOR_AM) },
	{ IN_ATP_DOOR_AA, &(gTc2IoInputStru.ATP_DOOR_AA) },
	{ IN_EBI1_FEEDBACK, &(gTc2IoInputStru.EBI1_FEEDBACK) },
	{ IN_EBI2_FEEDBACK, &(gTc2IoInputStru.EBI2_FEEDBACK) },
	{ IN_EMERGENCYDOOR, &(gTc2IoInputStru.EMERGENCYDOOR) },
	{ IN_TVS1CHECK, &(gTc2IoInputStru.TVS1CHECK) },
	{ IN_TVS2CHECK, &(gTc2IoInputStru.TVS2CHECK) },
	{ IN_DOORALLCLOSE, &(gTc2IoInputStru.DOORALLCLOSE) },

	/*--------------以下为成都四线安全&非安全输入------------*/
	{ IN_TRAIN_DOORCLOSE,&(gTc2IoInputStru.InTrainDoorClose) },
	{ IN_TRAIN_EMERDOORBOARD,&(gTc2IoInputStru.InTrainEmergencyDoorBoard) },
	{ IN_TRAIN_REDUNDANCY,&(gTc2IoInputStru.InTrainRedundancy) },
	{ IN_TRAIN_FAN1,&(gTc2IoInputStru.InTrainFan1) },
	{ IN_TRAIN_FAN2,&(gTc2IoInputStru.InTrainFan2) },
	{ IN_TRAIN_DOORLOCK,&(gTc2IoInputStru.InTrainDoorLock) },
	{ IN_TRAIN_ESCAPDOOR_UNLOCK,&(gTc2IoInputStru.InTrainEscapDoorUnlock) },
	{ IN_TRAIN_GUESTDOOR_UNLOCK,&(gTc2IoInputStru.InTrainGuestDoorUnlock) },
};

/***************************************************************************************
* 功能描述:			解析IO输入的数据
* 输入参数:			UINT8 *pRecvBuf
* 输入输出参数:		无
* 输出参数:			pInUnSafeData 非安全值
*					pInSafeData 安全值
* 全局变量:
* 返回值:			0x00：成功；
*					0x01:传入指针为空
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleInputAnalysis(UINT8 *pRecvBuf, UINT32 *pInUnSafeData, UINT32 *pInSafeData);

/***************************************************************************************
* 功能描述:			将接收到的原始数据解析到输入结构体中
* 输入参数:			UINT16 ProType 项目标识
* 					UINT16 DeviceTypeID 插箱ID
* 输入输出参数:     IO_CFG_DATA_STRU *pDataConfigStru IO配置数据结构体指针
* 输出参数:      	UINT8 *pRecvBuf 收到的原始数据
* 全局变量:         无
* 返回值:           0x00：成功；
* 					0x01:传入参数不正确
* 					0x02:解析IO输入的数据出错
* 					0x04:将解析的数据存放相应结构体出错
* 					0x08:根据插箱ID寻找IO配置表ID失败
****************************************************************************************/
static UINT8 IoModuleInputHandler(UINT8 whitchEnd, UINT16 DeviceTypeID, UINT8 *pRecvBuf, IO_CFG_DATA_STRU *pDataConfigStru);

/***************************************************************************************
* 功能描述:			输出信息处理
* 输入参数:			IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo io输入结构体
* 输入输出参数:     无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleInPut_Output(INOUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);

/***************************************************************************************
* 功能描述:			输入与数据映射关系
* 输入参数:			IO初始化结构体
* 输入输出参数:       无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
* 修改记录：added by Jielu 20230406
****************************************************************************************/
static UINT8 IoModule_Api_Value_Map_Init_Mscp(IN const IO_CFG_DATA_STRU *pIoConfigDataStru);

/***************************************************************************************
* 功能描述:			获取输出对应的变量值
* 输入参数:			BitMean-输出含义值
* 输入输出参数:     无
* 输出参数:			无
* 全局变量:			gIoOutputIndexTable
* 返回值:           变量值地址指针
* 修改记录： added by Jielu 20230406
****************************************************************************************/
static UINT8 *IoModuleGetValuePoint_Mscp(UINT8 BitMean);


/***************************************************************************************
* 功能描述:			将输入的两块SID板卡的开关量按照55和AA区分
* 输入参数:			IN digitalData1:SID1板卡单节点采集的开关量；IN digitalData2:SID2板卡采集的开关量
* 输入输出参数: 无
* 输出参数:         OUT UINT8 *pRcvDigitalBuff
* 全局变量:
* 返回值:           0x00：成功；
* 修改记录：          added by Jielu 20230404
****************************************************************************************/
static void IoModuleInputSave_Mscp(IN UINT32 digitalData1, IN UINT32 digitalData2);


/***************************************************************************************
* 功能描述:			安全的输入处理
* 输入参数:			UINT8 NO			安全输入的常开节点
*					UINT8 NC			安全输入的常闭节点
*					UINT8 OpenOrClosse  触点有效性类型
*					UINT8 DEFAULT		默认值
* 输入输出参数:		无
* 输出参数:
* 全局变量:
* 返回值:			0x00：成功；
*					0x01:传入参数不正确
* 修改记录： added by Jielu 20230407
****************************************************************************************/
static UINT8 IoSafeInputProc_Mscp(UINT8 NO, UINT8 NC, UINT8 LastState, UINT8 OpenOrClosse, UINT8 DEFAULT, UINT32 *Err, DIGITAL_IO_Time_MSCP* PeriodSn_Mscp, UINT8 n);


/***********************************************************************
* 功能描述:        时钟定时功能
* 输入参数:         const UINT32 start开始计时时间const UINT32 expiration 定时时间
* 输入输出参数:     无
* 输出参数:         无
* 全局变量:         ..
* 返回值:              CBTC_TRUE 已达到计时时间；CBTC_FALSE 未达到计时时间
* 修改时间       added by Jielu 20230408
*************************************************************************/
static UINT8 IOTimerExpired_Mscp(const UINT32 start, const UINT32 expiration);

/***********************************************************************
* 功能描述:        获取TC1端容忍周期锁
* 输入参数:         UINT8 whitchend
* 输入输出参数:     无
* 输出参数:         无
* 全局变量:         ..
* 返回值:    TCI端容忍锁地址
* 修改时间       added by Jielu 20230417
*************************************************************************/
static DIGITAL_IO_Time_MSCP* GetDigitalIO_Mscp_Lock(UINT8 whichEnd);

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
UINT8 IoModuleMscp_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru)
{
	UINT8 RtnNo = (UINT8)FUNC_CODE1;

	if (NULL != pIoConfigDataStru)
	{
		(void)CommonMemSet(&gIoSingleInput, sizeof(IO_SINGLE_INPUT_STRUCT), 0xFFU, sizeof(IO_SINGLE_INPUT_STRUCT));			/*单节点采集IO结构体*/
		(void)CommonMemCpy(&gIoConfigDataStru, sizeof(IO_CFG_DATA_STRU), pIoConfigDataStru, sizeof(IO_CFG_DATA_STRU));
		(void)CommonMemSet(&gTc1IoInputStru, sizeof(IO_INPUT_STRUCT), 0x00U, sizeof(IO_INPUT_STRUCT)); 		/*初始化TC1采集后的节点*/
		(void)CommonMemSet(&gTc2IoInputStru, sizeof(IO_INPUT_STRUCT), 0x00U, sizeof(IO_INPUT_STRUCT)); 		/*初始化TC2采集后的节点*/

		(void)CommonMemSet(&gLastTc1InputStru, sizeof(IO_INPUT_STRUCT), 0x00, sizeof(IO_INPUT_STRUCT)); /*初始化TC1端上周期结构体节点*/
		(void)CommonMemSet(&gLastTc2InputStru, sizeof(IO_INPUT_STRUCT), 0x00, sizeof(IO_INPUT_STRUCT)); /*初始化TC2端上周期结构体节点*/
																										
		RtnNo = IoModule_Api_Value_Map_Init_Mscp(pIoConfigDataStru);	/*各板卡位地址映射表初始化*/
	}
	else
	{
		/*do nothing*/
	}

	return RtnNo;
}

/***************************************************************************************
* 功能描述:			输入与数据映射关系
* 输入参数:			IO初始化结构体
* 输入输出参数:       无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
* 修改记录：added by Jielu 20230406
****************************************************************************************/
static UINT8 IoModule_Api_Value_Map_Init_Mscp(IN const IO_CFG_DATA_STRU *pIoConfigDataStru)
{
	UINT8 boardIndex = 0U;
	UINT8 loopIndex = 0U;
	UINT8 rtNo = FUNC_CODE1;
	if (NULL != pIoConfigDataStru)
	{

		for (boardIndex = 0U; boardIndex < pIoConfigDataStru->BoardNum; boardIndex++)
		{

			if (TRAIN_INPUT_BOARD == pIoConfigDataStru->BoardIoMsgBuf[boardIndex].BoxAttr)
			{

				for (loopIndex = 0U; loopIndex < MAX_CHANNELS_NUM; loopIndex++)
				{
					gIoSingleMapTable[loopIndex] = IoModuleGetValuePoint_Mscp(pIoConfigDataStru->BoardIoMsgBuf[boardIndex].GfgIoBuf[loopIndex]);
				}

				rtNo = FUNC_SUCESS;
				break;
			}
			else
			{
				/*do nothing*/

			}

		}
	}
	else
	{
		rtNo = FUNC_CODE2;
	}

	return rtNo;
}

/***********************************************************************
* 功能描述:		获取TC1端容忍周期锁
* 输入参数:		无
* 输入输出参数:	无
* 输出参数:		无
* 全局变量:		
* 返回值:		TCI端容忍锁地址
* 修改时间		added by Jielu 20230417
*************************************************************************/
static DIGITAL_IO_Time_MSCP* GetDigitalIO_Mscp_Lock(UINT8 whichEnd)
{
	DIGITAL_IO_Time_MSCP *pPeriodSn_Mscp = NULL;

	if (1U == whichEnd)
	{
		pPeriodSn_Mscp = gPeriodSn_Mscp_TC1;
	}
	else if (2U == whichEnd)
	{
		pPeriodSn_Mscp = gPeriodSn_Mscp_TC2;
	}
	else
	{
		/*do nothing*/
	}

	return pPeriodSn_Mscp;
}

/***************************************************************************************
* 功能描述:			获取输出对应的变量值
* 输入参数:			BitMean-输出含义值
* 输入输出参数:     无
* 输出参数:			无
* 全局变量:			gIoOutputIndexTable
* 返回值:           变量值地址指针
* 修改记录： added by Jielu 20230406
****************************************************************************************/
static UINT8 *IoModuleGetValuePoint_Mscp(UINT8 BitMean)
{
	UINT8 *retVal = NULL;
	UINT8 Index = 0U;

	if (0x00U == BitMean)
	{
		retVal = NULL;		/*保留位*/
	}
	else
	{
		/*遍历含义值与变量对应表*/
		for (Index = 0U; Index < INPUT_FLAG_NUM_SINGLE; Index++)
		{
			if (BitMean == gIoInputIndexTable[Index].IoInputTypeIndex)
			{
				/*返回映射表中对应涵义值的地址*/
				retVal = gIoInputIndexTable[Index].IoInputStruIndex;
				break;
			}
		}
	}

	return retVal;
}

/***************************************************************************************
* 功能描述:			IO输入模块需要初始化的值以及读取IO配置的数据
* 输入参数:			无
* 输入输出参数:		无
* 输出参数:
* 全局变量:
* 返回值:			0x00:初始化成功
*					0x01:读取IO配置数据失败
* 修改记录：
****************************************************************************************/
UINT8 IoModule_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru)
{
	UINT8 RtnNo = FUNC_CODE1;

	if (NULL != pIoConfigDataStru)
	{
		/*读取IO配置数据*/
		CommonMemCpy(&gIoConfigDataStru, sizeof(IO_CFG_DATA_STRU), pIoConfigDataStru, sizeof(IO_CFG_DATA_STRU));

		RtnNo = FUNC_SUCESS;
	}
	else
	{
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

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
UINT8 IoModuleInputFindConfigDataFromPlugBoxId(UINT16 PlugBoxId, IO_CFG_DATA_STRU *pDataConfigStru, UINT8 *pFindIndex)
{
	UINT8 Index = 0U;
	UINT8 RtnNo = FUNC_CODE1; 			/* 函数返回值 */

	if ((NULL != pDataConfigStru) && (NULL != pFindIndex))
	{
		/* 根据插箱ID遍历配置数据中的ID 将ID相同的配置数据中的索引输出*/
		for (Index = 0U; Index < pDataConfigStru->BoardNum; Index++)
		{
			if (pDataConfigStru->BoardIoMsgBuf[Index].BoxId == PlugBoxId)
			{
				*pFindIndex = Index;
				RtnNo = FUNC_SUCESS;
				break;
			}
			else
			{
				/* 插箱ID和表中的ID不匹配不做处理*/
			}
		}
	}
	else
	{
		/*传入参数错误*/
		RtnNo = FUNC_CODE2;
	}

	return RtnNo;
}

/***************************************************************************************
* 功能描述:			解析IO输入的数据
* 输入参数:			UINT8 *pRecvBuf
* 输入输出参数:		无
* 输出参数:			pInUnSafeData 非安全值
*					pInSafeData 安全值
* 全局变量:
* 返回值:			0x00：成功；
*					0x01:传入指针为空
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleInputAnalysis(UINT8 *pRecvBuf, UINT32 *pInUnSafeData, UINT32 *pInSafeData)
{
	UINT8 FuncRtn = FUNC_CODE1;
	if ((NULL != pRecvBuf) && (NULL != pInUnSafeData) && (NULL != pInSafeData))
	{
		/* 将数据解析到相应的字节中 */
		*pInUnSafeData = LongFromChar(&pRecvBuf[0]);
		*pInSafeData = LongFromChar(&pRecvBuf[4]);
		AppLogPrintStr(ENUM_LOG_ERR, "IN safe %x, Unsa %x\n", *pInSafeData, *pInUnSafeData);
		FuncRtn = FUNC_SUCESS;
	}
	else
	{
		FuncRtn = FUNC_CODE1;
	}

	return (FuncRtn);
}

/***************************************************************************************
* 功能描述:			将接收到的原始数据解析到输入结构体中
* 输入参数:
*					UINT16 DeviceTypeID 插箱ID
* 输入输出参数:		IO_CFG_DATA_STRU *pDataConfigStru IO配置数据结构体指针
* 输出参数:			UINT8 *pRecvBuf 收到的原始数据
* 全局变量:			无
* 返回值:           0x00：成功；
*					0x01:传入参数不正确
*					0x02:解析IO输入的数据出错
*					0x04:将解析的数据存放相应结构体出错
*					0x08:根据插箱ID寻找IO配置表ID失败
*修改记录：1.按照单节点采集结果进行拼接  added by Jielu 20230411
****************************************************************************************/
static UINT8 IoModuleInputHandler(UINT8 whichEnd, UINT16 DeviceTypeID, UINT8 *pRecvBuf, IO_CFG_DATA_STRU *pDataConfigStru)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT32 FuncRtn32 = 0U;
	UINT8 Rtn = FUNC_CODE1;
	UINT16 DeviceID = CBTC_NULL16;
	UINT8 PlugBoxIndex = 0U;		/*输入板索引*/
	UINT32 digitalData1 = 0U; 		/* SID板卡1采集开关量 */
	UINT32 digitalData2 = 0U;   	/* SID板卡2采集开关量 */
	IO_INPUT_STRUCT *tmpLastInputStru = NULL;        /*存放上周期采集结果的临时变量（已经组合完的）*/
	DIGITAL_IO_Time_MSCP *tmpPeriodSn_Mscp = NULL;   /*临时存放容忍结构体*/
	UINT8 TempConvertNC = CBTC_NULL;				/*常闭点位临时转换变量*/

	DeviceID = DeviceTypeID;	/*直接取设备ID (Lower 8bit),不会丢失数据*/
	if ((NULL != pRecvBuf) && (NULL != pDataConfigStru))
	{
		/*获取容忍周期是那一端的结构体*/
		tmpPeriodSn_Mscp = GetDigitalIO_Mscp_Lock(whichEnd);

		if (NULL != tmpPeriodSn_Mscp)
		{
			/*根据插箱ID查找对应插箱输入信息配置数据*/
			Rtn = IoModuleInputFindConfigDataFromPlugBoxId(DeviceID, pDataConfigStru, &PlugBoxIndex);
			if ((FUNC_SUCESS == Rtn) && (TRAIN_INPUT_BOARD == pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex].BoxAttr))
			{
				/* 解析IO输入的数据 */
				Rtn = IoModuleInputAnalysis(pRecvBuf, &digitalData1, &digitalData2);

				/*将两块SID板卡的采集值进行相应转换，并存入映射table中*/
				IoModuleInputSave_Mscp(digitalData1, digitalData2);

				if (FUNC_SUCESS == Rtn)
				{
					if ((UINT8)DATA_FROM_TC1 == whichEnd)
					{
						tmpLastInputStru = &gLastTc1InputStru;
					}
					else if ((UINT8)DATA_FROM_TC2 == whichEnd)
					{
						tmpLastInputStru = &gLastTc2InputStru;
					}
					else
					{
						/*do nothing*/
					}

					if (NULL != tmpLastInputStru)
					{
						/******************************************************Safe Start*********************************************************/
						/*驾驶室激活，常开有效*/
						gIOInputStruMscp.InTrainCabActive = IoSafeInputProc_Mscp(gIoSingleInput.InTrainCabActiveMscpNo, gIoSingleInput.InTrainCabActiveMscpNc, 
							tmpLastInputStru->InTrainCabActive, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 0U);
						
						/*EB已实施，常闭有效*/
						gIOInputStruMscp.InTrainEbEnable = IoSafeInputProc_Mscp(gIoSingleInput.InTrainEbEnableMscpNo, gIoSingleInput.InTrainEbEnableMscpNc, 
							tmpLastInputStru->InTrainEbEnable, CLOOSE_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 1U);

						/*牵引切除，常闭有效*/
						gIOInputStruMscp.InTrainTractionCut = IoSafeInputProc_Mscp(gIoSingleInput.InTrainTractionCutMscpNo, gIoSingleInput.InTrainTractionCutMscpNc, 
							tmpLastInputStru->InTrainTractionCut, CLOOSE_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 2U);

						/*方向手柄向后，常开有效*/
						gIOInputStruMscp.InTrainDirBackward = IoSafeInputProc_Mscp(gIoSingleInput.InTrainDirBackwardMscpNo, gIoSingleInput.InTrainDirBackwardMscpNc, 
							tmpLastInputStru->InTrainDirBackward, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 3U);

						/*司机钥匙激活，常开有效*/
						gIOInputStruMscp.TRAIN_KEY_OPEN = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_KEY_OPENMscpNo, gIoSingleInput.TRAIN_KEY_OPENMscpNc, 
							tmpLastInputStru->TRAIN_KEY_OPEN, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 4U);

						/*列车完整性，常开有效*/
						gIOInputStruMscp.InAtpTrainInteg = IoSafeInputProc_Mscp(gIoSingleInput.InAtpTrainIntegMscpNo, gIoSingleInput.InAtpTrainIntegMscpNc, 
							tmpLastInputStru->InAtpTrainInteg, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 5U);

						/*车门关闭且锁闭，常开有效*/
						gIOInputStruMscp.InTrainDoorCloseAndLock = IoSafeInputProc_Mscp(gIoSingleInput.InTrainDoorCloseAndLockMscpNo, gIoSingleInput.InTrainDoorCloseAndLockMscpNc, 
							tmpLastInputStru->InTrainDoorCloseAndLock, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 6U);

						if (IO_NULL8_MSCP == gIOInputStruMscp.InTrainDoorCloseAndLock)
						{
							/*如果开关量数据没配典型的车门关闭且锁闭，尝试获取迪士尼双1有效车门关闭且锁闭*/
							/*双1有效，常闭取反后按常开有效判断*/
							TempConvertNC = ~(gIoSingleInput.InDouble1DoorCloseLockMscpNc);
							gIOInputStruMscp.InTrainDoorCloseAndLock = IoSafeInputProc_Mscp(gIoSingleInput.InDouble1DoorCloseLockMscpNo, TempConvertNC,
								tmpLastInputStru->InTrainDoorCloseAndLock, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 6U);
						}
						else
						{
							/*nothing*/
						}

						/*保持制动已实施，常开有效*/
						gIOInputStruMscp.InAtpBczdValid = IoSafeInputProc_Mscp(gIoSingleInput.InAtpBczdValidMscpNo, gIoSingleInput.InAtpBczdValidMscpNc, 
							tmpLastInputStru->InAtpBczdValid, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 7U);

						/*牵引制动手柄在零位且方向手柄在向前位，常开有效*/
						gIOInputStruMscp.InAtpSbRightInput = IoSafeInputProc_Mscp(gIoSingleInput.InAtpSbRightInputMscpNo, gIoSingleInput.InAtpSbRightInputMscpNc, 
							tmpLastInputStru->InAtpSbRightInput, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 8U);

						/*确认按钮状态，常开有效*/
						gIOInputStruMscp.InTrainConfirmButton = IoSafeInputProc_Mscp(gIoSingleInput.InTrainConfirmButtonMscpNo, gIoSingleInput.InTrainConfirmButtonMscpNc, 
							tmpLastInputStru->InTrainConfirmButton, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 9U);

						/*车辆紧急手柄激活，常闭有效*/
						gIOInputStruMscp.TRAIN_EBHANDLE_ACTIVE = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNo, gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNc, 
							tmpLastInputStru->TRAIN_EBHANDLE_ACTIVE, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 10U);

						/*障碍物脱轨有效，常闭有效*/
						gIOInputStruMscp.TRAIN_OBSTDET_INPUT = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNo, gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNc, 
							tmpLastInputStru->TRAIN_OBSTDET_INPUT, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 11U);

						/*制动重故障，常闭有效*/
						gIOInputStruMscp.TRAIN_BRAKE_FAULT = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_BRAKE_FAULTMscpNo, gIoSingleInput.TRAIN_BRAKE_FAULTMscpNc, 
							tmpLastInputStru->TRAIN_BRAKE_FAULT, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 12U);

						/*牵引制动手柄在牵引位且方向向前，常开有效*/
						gIOInputStruMscp.ATP_TRACTION_RIGHT = IoSafeInputProc_Mscp(gIoSingleInput.ATP_TRACTION_RIGHTMscpNo, gIoSingleInput.ATP_TRACTION_RIGHTMscpNc, 
							tmpLastInputStru->ATP_TRACTION_RIGHT, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 13U);

						/*车门关闭，常开有效*/
						gIOInputStruMscp.InTrainDoorClose = IoSafeInputProc_Mscp(gIoSingleInput.InTrainDoorCloseMscpNo, gIoSingleInput.InTrainDoorCloseMscpNc, 
							tmpLastInputStru->InTrainDoorClose, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 14U);

						/*逃生门盖板状态，常闭有效*/
						gIOInputStruMscp.InTrainEmergencyDoorBoard = IoSafeInputProc_Mscp(gIoSingleInput.InTrainEmergencyDoorBoardMscpNo, gIoSingleInput.InTrainEmergencyDoorBoardMscpNc, 
							tmpLastInputStru->InTrainEmergencyDoorBoard, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 15U);

						/*逃生门状态，常闭有效*/
						gIOInputStruMscp.EMERGENCYDOOR = IoSafeInputProc_Mscp(gIoSingleInput.EMERGENCYDOORMscpNo, gIoSingleInput.EMERGENCYDOORMscpNc, 
							tmpLastInputStru->EMERGENCYDOOR, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 16U);

						/*以下Disney安全输入，部分输入双1双0有效，需要做一个转换*/
						/*(Safe)CM模式，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpCMModeMscpNc);
						gIOInputStruMscp.InAtpCMMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpCMModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpCMMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 17U);
						
						/*(Safe)NOT SD 模式，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpNotSDModeMscpNc);
						gIOInputStruMscp.InAtpNotSDMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpNotSDModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpNotSDMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 18U);
						
						/*(Safe)FOR模式，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpFORModeMscpNc);
						gIOInputStruMscp.InAtpFORMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpFORModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpFORMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 19U);
						
						/*(Safe)REV模式，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpREVModeMscpNc);
						gIOInputStruMscp.InAtpREVMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpREVModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpREVMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 20U);
						
						/*(Safe)端SB模式，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpSBModeMscpNc);
						gIOInputStruMscp.InAtpSBMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpSBModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpSBMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 21U);
						
						/*(Safe)端SD模式，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpSDModeMscpNc);
						gIOInputStruMscp.InAtpSDMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpSDModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpSDMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 22U);
						
						/*(Safe)信号系统紧急制动输出反馈采集，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpEBFeedbackMscpNc);
						gIOInputStruMscp.InAtpEBFeedback = IoSafeInputProc_Mscp(gIoSingleInput.InAtpEBFeedbackMscpNo, TempConvertNC,
							tmpLastInputStru->InAtpEBFeedback, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 23U);
						
						/*(Safe)零速信号已实施，双0有效，常闭取反后按常闭有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpConductZeroSpeedMscpNc);
						gIOInputStruMscp.InAtpConductZeroSpeed = IoSafeInputProc_Mscp(gIoSingleInput.InAtpConductZeroSpeedMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpConductZeroSpeed, CLOOSE_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 24U);
						
						/*(Safe)安全联锁继电器状态采集，双1有效，常闭取反后按常开有效判断*/
						TempConvertNC = ~(gIoSingleInput.InAtpSafeInterLockFeedbackMscpNc);
						gIOInputStruMscp.InAtpSafeInterLockFeedback = IoSafeInputProc_Mscp(gIoSingleInput.InAtpSafeInterLockFeedbackMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpSafeInterLockFeedback, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 25U);

						AppLogPrintStr(ENUM_LOG_ERR, "IO_in[%d]=%x,%d,N1=%x,%x,N2=%x,%x,N3=%x,%x,N4=%x,%x,N5=%x,%x,N6=%x,%x,N7=%x,%x,N8=%x,%x,N9=%x,%x,N10=%x,%x,N11=%x,%x,N12=%x,%x,N13=%x,%x,N14=%x,%x,N15=%x,%x,N16=%x,%x,N17=%x,%x\n", 
							whichEnd, FuncRtn32, gPeriodSn, gIoSingleInput.InTrainCabActiveMscpNo, gIoSingleInput.InTrainCabActiveMscpNc,
							gIoSingleInput.InTrainEbEnableMscpNo, gIoSingleInput.InTrainEbEnableMscpNc, gIoSingleInput.InTrainTractionCutMscpNo, gIoSingleInput.InTrainTractionCutMscpNc, 
							gIoSingleInput.InTrainDirBackwardMscpNo, gIoSingleInput.InTrainDirBackwardMscpNc, gIoSingleInput.TRAIN_KEY_OPENMscpNo, gIoSingleInput.TRAIN_KEY_OPENMscpNc, 
							gIoSingleInput.InAtpTrainIntegMscpNo, gIoSingleInput.InAtpTrainIntegMscpNc, gIoSingleInput.InTrainDoorCloseAndLockMscpNo, gIoSingleInput.InTrainDoorCloseAndLockMscpNc, 
							gIoSingleInput.InAtpBczdValidMscpNo, gIoSingleInput.InAtpBczdValidMscpNc, gIoSingleInput.InAtpSbRightInputMscpNo, gIoSingleInput.InAtpSbRightInputMscpNc, 
							gIoSingleInput.InTrainConfirmButtonMscpNo, gIoSingleInput.InTrainConfirmButtonMscpNc, gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNo, gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNc,
							gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNo, gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNc, gIoSingleInput.TRAIN_BRAKE_FAULTMscpNo, gIoSingleInput.TRAIN_BRAKE_FAULTMscpNc,
							gIoSingleInput.ATP_TRACTION_RIGHTMscpNo, gIoSingleInput.ATP_TRACTION_RIGHTMscpNc, gIoSingleInput.InTrainDoorCloseMscpNo, gIoSingleInput.InTrainDoorCloseMscpNc,
							gIoSingleInput.InTrainEmergencyDoorBoardMscpNo, gIoSingleInput.InTrainEmergencyDoorBoardMscpNc, gIoSingleInput.EMERGENCYDOORMscpNo, gIoSingleInput.EMERGENCYDOORMscpNc);
						AppLogPrintStr(ENUM_LOG_ERR, "Dis_In[%d]N1=%x,%x,N2=%x,%x,N3=%x,%x,N4=%x,%x,N5=%x,%x,N6=%x,%x,N7=%x,%x,N8=%x,%x,N9=%x,%x\n", whichEnd, 
							gIoSingleInput.InAtpCMModeMscpNo, gIoSingleInput.InAtpCMModeMscpNc, gIoSingleInput.InAtpNotSDModeMscpNo, gIoSingleInput.InAtpNotSDModeMscpNc,
							gIoSingleInput.InAtpFORModeMscpNo, gIoSingleInput.InAtpFORModeMscpNc, gIoSingleInput.InAtpREVModeMscpNo, gIoSingleInput.InAtpREVModeMscpNc, 
							gIoSingleInput.InAtpSBModeMscpNo, gIoSingleInput.InAtpSBModeMscpNc, gIoSingleInput.InAtpSDModeMscpNo, gIoSingleInput.InAtpSDModeMscpNc,
							gIoSingleInput.InAtpEBFeedbackMscpNo, gIoSingleInput.InAtpEBFeedbackMscpNc, gIoSingleInput.InAtpConductZeroSpeedMscpNo, gIoSingleInput.InAtpConductZeroSpeedMscpNc,
							gIoSingleInput.InAtpSafeInterLockFeedbackMscpNo, gIoSingleInput.InAtpSafeInterLockFeedbackMscpNc);
						/******************************************************Safe End*********************************************************/
						FuncRtn = FUNC_SUCESS;
					}
					else
					{
						FuncRtn = FUNC_CODE3;
					}

					/*非安全已经赋值完*/
				}
				else
				{
					FuncRtn = FUNC_CODE2;
				}
			}
			else
			{
				FuncRtn = FUNC_CODE4;
			}
		}
		else
		{
			FuncRtn = FUNC_CODE5;
		}
	}
	else
	{
		/* 传入参数错误 */
		FuncRtn = FUNC_CODE1;
	}

	return FuncRtn;
}

/***************************************************************************************
* 功能描述:			将输入的两块SID板卡的开关量按照55和AA区分
* 输入参数:			IN digitalData1:SID1板卡单节点采集的开关量；IN digitalData2:SID2板卡采集的开关量
* 输入输出参数:		无
* 输出参数:			OUT UINT8 *pRcvDigitalBuff
* 全局变量:
* 返回值:			0x00：成功；
* 修改记录：		added by Jielu 20230404
****************************************************************************************/
static void IoModuleInputSave_Mscp(IN UINT32 digitalData1, IN UINT32 digitalData2)
{
	UINT8 loopIndex = 0u;   /*安全、非安全所有总路数*/

	for (loopIndex = 0U; loopIndex < (MAX_CHANNELS_NUM / 2U); loopIndex++)
	{
		/*第一块SID*/
		if (NULL == gIoSingleMapTable[loopIndex])
		{
			/*指针为空*/
		}
		else
		{
			*(gIoSingleMapTable[loopIndex]) = GET_BIT_FROM_INPUT_MSCP(digitalData2, loopIndex);
		}

		/*第二块SID*/
		if (NULL == gIoSingleMapTable[loopIndex + (MAX_CHANNELS_NUM / 2U)])
		{
			/*指针为空*/
		}
		else
		{
			*(gIoSingleMapTable[loopIndex + (MAX_CHANNELS_NUM / 2U)]) = GET_BIT_FROM_INPUT_MSCP(digitalData1, loopIndex);
		}
	}
}

/***************************************************************************************
* 功能描述:			安全的输入处理
* 输入参数:			UINT8 NO			安全输入的常开节点
*					UINT8 NC			安全输入的常闭节点
*					UINT8 OpenOrClosse  触点有效性类型
*					UINT8 DEFAULT		默认值
* 输入输出参数:     无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
*					0x01:传入参数不正确
* 修改记录： added by Jielu 20230407
****************************************************************************************/
static UINT8 IoSafeInputProc_Mscp(UINT8 NO, UINT8 NC, UINT8 LastState, UINT8 OpenOrClosse, UINT8 DEFAULT, UINT32 *Err, DIGITAL_IO_Time_MSCP* PeriodSn_Mscp, UINT8 n)
{
	UINT8 FuncRtn = (UINT8)FUNC_SUCESS;
	UINT8 RtnValue = DEFAULT;
	UINT8 TemValue = IO_FALSE_MSCP;

	if ((NULL == Err) || (NULL == PeriodSn_Mscp))
	{
		/*入参错误*/
		RtnValue = DEFAULT;
		FuncRtn |= (UINT8)FUNC_CODE2;
	}
	else
	{
		/* 输入值合法性检查*/
		if (((IO_TRUE_MSCP == NO) || (IO_FALSE_MSCP == NO))
			&& ((IO_TRUE_MSCP == NC) || (IO_FALSE_MSCP == NC)))
		{
			/*安全输入的常开触点和常闭触点值相反*/
			if (IO_NULL8_MSCP == (NO^NC))
			{
				/*常开触点有效且触点有效性为常开或常闭触点有效且触点有效性为常闭*/
				if (((IO_TRUE_MSCP == NO) && (OPEN_VALID_MSCP == OpenOrClosse))
					|| ((IO_TRUE_MSCP == NC) && (CLOOSE_VALID_MSCP == OpenOrClosse)))
				{
					RtnValue = IO_TRUE_MSCP;/*该安全输入有效*/
				}
				else
				{
					RtnValue = IO_FALSE_MSCP;/*该安全输入无效*/
				}
				PeriodSn_Mscp[n].TimeLock = IO_FALSE_MSCP;
			}
			else if (0x00U == (NO^NC))
			{
				/*安全输入的常开触点和常闭触点值相同时，进行容忍，容忍时间为3个周期*/
				if (IO_FALSE_MSCP == PeriodSn_Mscp[n].TimeLock)
				{
					PeriodSn_Mscp[n].LastStateTime = gPeriodSn;
					PeriodSn_Mscp[n].TimeLock = IO_TRUE_MSCP;
				}
				TemValue = IOTimerExpired_Mscp(PeriodSn_Mscp[n].LastStateTime, 3U);
				if (IO_TRUE_MSCP != TemValue)
				{
					/*在容忍时间内，采用上周期的状态*/
					RtnValue = LastState;
				}
				else
				{
					/*超过容忍时间，采用默认值*/
					RtnValue = DEFAULT;
					*Err |= (1U << n);
				}

			}
			else
			{
				;/*不可达分支，不处理*/
			}
		}
		else if ((IO_NULL8_MSCP == NO) && (IO_NULL8_MSCP == NC))
		{
			RtnValue = IO_NULL8_MSCP;/*该输出未配置，返回0xFF*/
		}
		else
		{
			/*输入值不合法*/
			RtnValue = DEFAULT;
			FuncRtn |= (UINT8)FUNC_CODE1;
		}
	}

	return (RtnValue);
}

/*************************************************************************
* 功能描述:			时钟定时功能
* 输入参数:			const UINT32 start开始计时时间const UINT32 expiration 定时时间
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			..
* 返回值:			CBTC_TRUE 已达到计时时间；CBTC_FALSE 未达到计时时间
* 修改时间			added by Jielu 20230408
*************************************************************************/
static UINT8 IOTimerExpired_Mscp(const UINT32 start, const UINT32 expiration)
{
	UINT8 ret = 0U;
	
	if ((gPeriodSn - start) >= expiration)
	{
		ret = IO_TRUE_MSCP;
	}
	else
	{
		ret = IO_FALSE_MSCP;
	}

	return ret;
}

/***************************************************************************************
* 功能描述:			输出信息处理
* 输入参数:			IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo io输入结构体
* 输入输出参数:     无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
* 修改记录：
****************************************************************************************/
static UINT8 IoModuleInPut_Output(INOUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT16 Index = 0u;
	UINT16 Index4Io = 0u;
	UINT8 Tc1_CabA_Key = CBTC_FALSE;	/*1端/A端驾驶室激活*/
	UINT8 Tc2_CabB_Key = CBTC_FALSE;	/*2端/B端驾驶室激活*/

	if (NULL == pIoIn_OutputInfo)
	{
		FuncRtn = FUNC_CODE1;
	}
	else
	{
		/*获取1端/A端驾驶室激活*/
		if (NULL == gTc1InputIndexTable[IN_TRAIN_CAB_ACTIVE - 1U].IoInputStruIndex)
		{
			/*指针为空*/
		}
		else
		{
			Tc1_CabA_Key = *(gTc1InputIndexTable[IN_TRAIN_CAB_ACTIVE - 1U].IoInputStruIndex);
		}

		/*获取2端/B端驾驶室激活*/
		if (NULL == gTc2InputIndexTable[IN_TRAIN_CAB_ACTIVE - 1U].IoInputStruIndex)
		{
			/*指针为空*/
		}
		else
		{
			Tc2_CabB_Key = *(gTc2InputIndexTable[IN_TRAIN_CAB_ACTIVE - 1U].IoInputStruIndex);
		}


		if ((CBTC_TRUE == Tc1_CabA_Key) && (CBTC_TRUE != Tc2_CabB_Key))
		{
			pIoIn_OutputInfo->MainCtrSide = CAB_A_TC1;
			pIoIn_OutputInfo->IdleSide = CAB_B_TC2;
		}
		else if ((CBTC_TRUE != Tc1_CabA_Key) && (CBTC_TRUE == Tc2_CabB_Key))
		{
			pIoIn_OutputInfo->MainCtrSide = CAB_B_TC2;
			pIoIn_OutputInfo->IdleSide = CAB_A_TC1;
		}
		else if ((CBTC_TRUE == Tc1_CabA_Key) && (CBTC_TRUE == Tc2_CabB_Key))
		{
			pIoIn_OutputInfo->MainCtrSide = BOTH_CTR;
			pIoIn_OutputInfo->IdleSide = BOTH_CTR;
		}
		else
		{
			pIoIn_OutputInfo->MainCtrSide = BOTH_NO_CTR;
			pIoIn_OutputInfo->IdleSide = BOTH_NO_CTR;
		}

		/*把索引值赋给相等的表，不按顺序赋值  防止表乱序*/
		for (Index = 1U; Index <= INPUT_FLAG_NUM; Index++)
		{
			for (Index4Io = 0U; Index4Io < (sizeof(pIoIn_OutputInfo->Tc1DigtalInputState) / sizeof(pIoIn_OutputInfo->Tc1DigtalInputState[0])); Index4Io++)
			{
				if ((gTc1InputIndexTable[Index - 1U].IoInputTypeIndex == Index4Io) && (NULL != gTc1InputIndexTable[Index - 1U].IoInputStruIndex))
				{
					pIoIn_OutputInfo->Tc1DigtalInputState[Index4Io] = *(gTc1InputIndexTable[Index - 1U].IoInputStruIndex);
				}
				else
				{
					/*nothing*/
				}

				if ((gTc2InputIndexTable[Index - 1U].IoInputTypeIndex == Index4Io) && (NULL != gTc2InputIndexTable[Index - 1U].IoInputStruIndex))
				{
					pIoIn_OutputInfo->Tc2DigtalInputState[Index4Io] = *(gTc2InputIndexTable[Index - 1U].IoInputStruIndex);
				}
				else
				{
					/*nothing*/
				}
			}
		}

		if (CAB_A_TC1 == pIoIn_OutputInfo->MainCtrSide)
		{
			(void)CommonMemCpy(&pIoIn_OutputInfo->CtrlDigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)), 
				&pIoIn_OutputInfo->Tc1DigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)));
			(void)CommonMemCpy(&pIoIn_OutputInfo->IdleDigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)), 
				&pIoIn_OutputInfo->Tc2DigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)));
		}
		else if (CAB_B_TC2 == pIoIn_OutputInfo->MainCtrSide)
		{
			(void)CommonMemCpy(&pIoIn_OutputInfo->CtrlDigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)), 
				&pIoIn_OutputInfo->Tc2DigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)));
			(void)CommonMemCpy(&pIoIn_OutputInfo->IdleDigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)), 
				&pIoIn_OutputInfo->Tc1DigtalInputState, (UINT32)(sizeof(UINT8)*(INPUT_DEFINE_MAX)));
		}
		else
		{
			/*不填写控制端、非控制端开关量*/
		}

		FuncRtn = FUNC_SUCESS;
	}

	return FuncRtn;
}

/***************************************************************************************
* 功能描述:			IO输入的数据处理,将接收到的IO数据解析存放到IO输入数据的结构体中
* 输入参数:			无
* 输入输出参数:		无
* 输出参数:			IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo io输出结构体
* 全局变量:
* 返回值:           0x00：成功；
*					0x01:接收TC1端IO报文有误
*					0x02:处理TC1端IO报文有误
*					0x04:接收TC2端IO报文有误
*					0x08:处理TC2端IO报文有误
* 修改记录：		增加单节点结构体初始化，added by Jielu 20230408
****************************************************************************************/
UINT8 IoModuleInPut_Api_PreiodProcess(OUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	RECV_SAME_DEVICE_DATA_STRU IoRecvStru = { 0 };	/* 接收到的IO的原始信息 */
	UINT8 CallFuncRtn = FUNC_CODE1;				/* 调用函数的返回值 */
	UINT8 FuncRtn = FUNC_CODE1;					/* 本函数的返回值 */

	if (NULL == pIoIn_OutputInfo)
	{
		FuncRtn = FUNC_CODE1;
	}
	else
	{
		(void)CommonMemSet(&IoRecvStru, sizeof(RECV_SAME_DEVICE_DATA_STRU), (UINT8)0x00, sizeof(RECV_SAME_DEVICE_DATA_STRU));
		(void)CommonMemSet(&gIOInputStruMscp, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gTc1IoInputStru, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gIoSingleInput, sizeof(gIoSingleInput), 0xFFU, sizeof(gIoSingleInput));			/*单节点采集IO结构体*/
		gPeriodSn += 1U;
		FuncRtn = FUNC_SUCESS;

		/*接收1端的IO信息*/
		CallFuncRtn = CommonModeule_Api_ABChooseDataFunc(IO_TC1_IN_1_TYPE_ID, (UINT8)DATA_FROM_TC1, &IoRecvStru);
		if ((FUNC_SUCESS == CallFuncRtn) && (1U <= IoRecvStru.DataFramNum))
		{

			/* 将收到的IO数据存放到数组中方便朔黄的IO模拟量的输入*/
			CommonMemCpy(RecvTc1IoBuff, (UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen,
				IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf,
				(UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen);
			/*根据插箱ID寻找相应的配置并解析到相关结构体中 */
			CallFuncRtn = IoModuleInputHandler((UINT8)DATA_FROM_TC1, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].TypeAndId, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf, &gIoConfigDataStru);
			if (FUNC_SUCESS == CallFuncRtn)
			{
				/* 将收到的数据存放到1端输入数据的结构体中 */
				CommonMemCpy(&gTc1IoInputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/*保存上周期的数据*/
				CommonMemCpy(&gLastTc1InputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/* 以下代码为了解决一个周期接收到数据为驾驶室未激活情况*/
				if ((0xAA == gTc1IoInputStru.InTrainCabActive) && (0x55 == TC1cablastState))
				{
					TC1cablastState = gTc1IoInputStru.InTrainCabActive;
					gTc1IoInputStru.InTrainCabActive = 0x55;
				}
				else
				{
					TC1cablastState = gTc1IoInputStru.InTrainCabActive;
				}
			}
			else
			{
				FuncRtn |= FUNC_CODE3;
			}
		}
		else
		{
			FuncRtn |= FUNC_CODE2;
		}

		/*数据初始化，整装待发哈哈*/
		(void)CommonMemSet(&IoRecvStru, sizeof(RECV_SAME_DEVICE_DATA_STRU), (UINT8)0x00, sizeof(RECV_SAME_DEVICE_DATA_STRU));
		(void)CommonMemSet(&gIOInputStruMscp, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gTc2IoInputStru, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gIoSingleInput, sizeof(gIoSingleInput), 0xFFU, sizeof(gIoSingleInput));			/*单节点采集IO结构体*/

		/*接收2端IO信息*/
		CallFuncRtn = CommonModeule_Api_ABChooseDataFunc(IO_TC2_IN_1_TYPE_ID, (UINT8)DATA_FROM_TC2, &IoRecvStru);
		if ((FUNC_SUCESS == CallFuncRtn) && (1U <= IoRecvStru.DataFramNum))
		{

			/* 将收到的IO数据存放到数组中方便朔黄的IO模拟量的输入*/
			CommonMemCpy(RecvTc2IoBuff, (UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen,
				IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf,
				(UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen);
			/*根据插箱ID寻找相应的配置并解析到相关结构体中 */
			CallFuncRtn = IoModuleInputHandler((UINT8)DATA_FROM_TC2, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].TypeAndId, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf, &gIoConfigDataStru);

			if (FUNC_SUCESS == CallFuncRtn)
			{
				/* 将收到的数据存放到2端输入数据的结构体中 */
				CommonMemCpy(&gTc2IoInputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/*保存二端上一周期的数据*/
				CommonMemCpy(&gLastTc2InputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/* 以下代码为了解决一个周期接收到数据为驾驶室未激活情况*/
				if ((0xAA == gTc2IoInputStru.InTrainCabActive) && (0x55 == TC2cablastState))
				{
					TC2cablastState = gTc2IoInputStru.InTrainCabActive;
					gTc2IoInputStru.InTrainCabActive = 0x55;
				}
				else
				{
					TC2cablastState = gTc2IoInputStru.InTrainCabActive;
				}
			}
			else
			{
				FuncRtn |= FUNC_CODE5;
			}
		}
		else
		{
			FuncRtn |= FUNC_CODE4;
		}

		CallFuncRtn = IoModuleInPut_Output(pIoIn_OutputInfo);
		if (FUNC_SUCESS != CallFuncRtn)
		{
			FuncRtn |= FUNC_CODE6;
		}
		else
		{
			/* 输出数据准备正确，不做处理*/
		}
	}

	return FuncRtn;
}

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
UINT8 IoModuleInPut_Api_GetIOInputStru(IO_INPUT_STRUCT *pTc1HHIoInputStru, IO_INPUT_STRUCT *pTc2HHIoInputStru)
{
	UINT8 FunRtn = FUNC_CODE1;
	if ((NULL != pTc1HHIoInputStru) && (NULL != pTc2HHIoInputStru))
	{
		CommonMemCpy(pTc1HHIoInputStru, sizeof(IO_INPUT_STRUCT), &gTc1IoInputStru, sizeof(IO_INPUT_STRUCT));
		CommonMemCpy(pTc2HHIoInputStru, sizeof(IO_INPUT_STRUCT), &gTc2IoInputStru, sizeof(IO_INPUT_STRUCT));

		FunRtn = FUNC_SUCESS;
	}
	else
	{
		FunRtn = FUNC_CODE1;
	}

	return FunRtn;
}

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
UINT8 IoModuleInPut_Api_GetBuffStru(UINT8 *pTc1IOBuff, UINT8 *pTc2IOBuff)
{
	UINT8 FunRtn = FUNC_CODE1;
	if ((NULL != pTc1IOBuff) && (NULL != pTc2IOBuff))
	{
		CommonMemCpy(pTc1IOBuff, (UINT32)50, RecvTc1IoBuff, (UINT32)50);
		CommonMemCpy(pTc2IOBuff, (UINT32)50, RecvTc2IoBuff, (UINT32)50);

		FunRtn = FUNC_SUCESS;

	}
	else
	{
		FunRtn = FUNC_CODE1;
	}
	return(FunRtn);
}
#if 0
/***************************************************************************************
* 功能描述:     记录本模块2x跟随数据的首地址与长度
* 输入参数:     无
* 输入输出参数:  无
* 输出参数:     无
* 全局变量:     本模块所有全局静态变量
* 返回值:      无
* 修改记录：
****************************************************************************************/
void GetIoInfoPrintf(void)
{
	STRU_MEM_ARRAY memArray[100] = { 0 };
	UINT32 i = 0;

	memArray[i].pAddr = &TC1cablastState;
	memArray[i].memLen = sizeof(TC1cablastState);
	i = i + 1;

	memArray[i].pAddr = &TC2cablastState;
	memArray[i].memLen = sizeof(TC2cablastState);
	i = i + 1;

	Print2X2(memArray, i);
}
#endif
/***************************************************************************************
* 功能描述:     获取静态数据
* 输入参数:     *pGetIoInfoBuff  需要同步的数据流
*				UINT16* pBuffLen  需要同步的数据长度
*				UINT16 BuffMaxLen 该模块同步的最大数据长度
* 输出参数:     无
* 全局变量:     无
* 返回值:       FUNC_CODE1 默认错误值
*				FUNC_CODE2 传入空指针
*				FUNC_CODE3 Get的数据长度过长
*				FUNC_SUCESS 执行成功无
* 修改记录：	lsn 2020/12/15
****************************************************************************************/
UINT8 GetIoInfoStaticDate(UINT8 *pGetIoInfoBuff, UINT16* pBuffLen, UINT16 BuffMaxLen)
{
	UINT16 BufLen = 0;
	UINT8 GetIoInfoArr[APP_FOLLOW_MAXLEN] = { 0, };
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 loopIndex = 0U;
	if ((NULL != pGetIoInfoBuff) && (NULL != pBuffLen))
	{
		GetIoInfoArr[BufLen++] = TC1cablastState; /* 用于存储上周期的激活状态 需要同步*/
		GetIoInfoArr[BufLen++] = TC2cablastState; /* 用于存储上周期的激活状态 需要同步*/
		GetIoInfoArr[BufLen++] = gFamCtrlSide;
		GetIoInfoArr[BufLen++] = gFamReverStrTag;
		GetIoInfoArr[BufLen++] = gARReverStrTag;
		GetIoInfoArr[BufLen++] = gFamDirBackTag;
		GetIoInfoArr[BufLen++] = ARuptime;
		GetIoInfoArr[BufLen++] = gLastTc1_Cab_Key;
		GetIoInfoArr[BufLen++] = Tc1_Tolerate_tic;
		GetIoInfoArr[BufLen++] = gLastTc2_Cab_Key;
		GetIoInfoArr[BufLen++] = Tc2_Tolerate_tic;

		(void)CommonMemCpy(&GetIoInfoArr[BufLen], sizeof(IO_INPUT_STRUCT), &gLastTc1InputStru, sizeof(IO_INPUT_STRUCT));
		BufLen += sizeof(IO_INPUT_STRUCT);

		(void)CommonMemCpy(&GetIoInfoArr[BufLen], sizeof(IO_INPUT_STRUCT), &gLastTc2InputStru, sizeof(IO_INPUT_STRUCT));
		BufLen += sizeof(IO_INPUT_STRUCT);

		for (loopIndex = 0U; loopIndex < MAX_SAFE_INPUT_NUM; loopIndex++)
		{
			GetIoInfoArr[BufLen++] = gPeriodSn_Mscp_TC1[loopIndex].TimeLock;
			LongToChar(gPeriodSn_Mscp_TC1[loopIndex].LastStateTime, &GetIoInfoArr[BufLen]);
			BufLen += 4U;

			GetIoInfoArr[BufLen++] = gPeriodSn_Mscp_TC2[loopIndex].TimeLock;
			LongToChar(gPeriodSn_Mscp_TC2[loopIndex].LastStateTime, &GetIoInfoArr[BufLen]);
			BufLen += 4U;
		}

		LongToChar(gPeriodSn, &GetIoInfoArr[BufLen]);
		BufLen += 4U;

		if (BufLen <= BuffMaxLen)
		{
			*pBuffLen = BufLen;
			CommonMemCpy(pGetIoInfoBuff, (UINT32)BufLen, &GetIoInfoArr[0], (UINT32)BufLen);
			FuncRtn = FUNC_SUCESS;
		}
		else
		{
			FuncRtn = FUNC_CODE3;
			*pBuffLen = 0;
		}
	}
	else
	{
		FuncRtn = FUNC_CODE2;
		if (NULL != pBuffLen)
		{
			*pBuffLen = 0;
		}
	}
	return FuncRtn;
}

/***************************************************************************************
* 功能描述:		设置静态数据
* 输入参数:		*pSetIoInfoBuff 备系需要跟随主系的数据
*				BuffLen 跟随数据的长度
*
* 输出参数:		无
* 全局变量:		无
* 返回值:		FUNC_CODE1 默认错误值
*				FUNC_CODE2 传入空指针
*				FUNC_CODE3 Set的数据长度过长
*				FUNC_SUCESS 执行成功
* 修改记录：	lsn 2020/12/15
****************************************************************************************/
UINT8 SetIoInfoStaticDate(UINT8 *pSetIoInfoBuff, UINT16 *BuffLen)
{
	UINT16 BuffLenIndex = 0;
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 loopIndex = 0U;

	if ((NULL != pSetIoInfoBuff) && (NULL != BuffLen))
	{
		TC1cablastState = pSetIoInfoBuff[BuffLenIndex++]; /* 用于存储上周期的激活状态 需要同步*/
		TC2cablastState = pSetIoInfoBuff[BuffLenIndex++]; /* 用于存储上周期的激活状态 需要同步*/
		gFamCtrlSide = pSetIoInfoBuff[BuffLenIndex++];
		gFamReverStrTag = pSetIoInfoBuff[BuffLenIndex++];
		gARReverStrTag = pSetIoInfoBuff[BuffLenIndex++];
		gFamDirBackTag = pSetIoInfoBuff[BuffLenIndex++];
		ARuptime = pSetIoInfoBuff[BuffLenIndex++];
		gLastTc1_Cab_Key = pSetIoInfoBuff[BuffLenIndex++];
		Tc1_Tolerate_tic = pSetIoInfoBuff[BuffLenIndex++];
		gLastTc2_Cab_Key = pSetIoInfoBuff[BuffLenIndex++];
		Tc2_Tolerate_tic = pSetIoInfoBuff[BuffLenIndex++];

		(void)CommonMemCpy(&gLastTc1InputStru, sizeof(IO_INPUT_STRUCT), &pSetIoInfoBuff[BuffLenIndex], sizeof(IO_INPUT_STRUCT));
		BuffLenIndex += sizeof(IO_INPUT_STRUCT);

		(void)CommonMemCpy(&gLastTc2InputStru, sizeof(IO_INPUT_STRUCT), &pSetIoInfoBuff[BuffLenIndex], sizeof(IO_INPUT_STRUCT));
		BuffLenIndex += sizeof(IO_INPUT_STRUCT);

		for (loopIndex = 0U; loopIndex < MAX_SAFE_INPUT_NUM; loopIndex++)
		{
			gPeriodSn_Mscp_TC1[loopIndex].TimeLock = pSetIoInfoBuff[BuffLenIndex++];
			gPeriodSn_Mscp_TC1[loopIndex].LastStateTime = LongFromChar(&pSetIoInfoBuff[BuffLenIndex]);
			BuffLenIndex += 4U;

			gPeriodSn_Mscp_TC2[loopIndex].TimeLock = pSetIoInfoBuff[BuffLenIndex++];
			gPeriodSn_Mscp_TC2[loopIndex].LastStateTime = LongFromChar(&pSetIoInfoBuff[BuffLenIndex]);
			BuffLenIndex += 4U;
		}

		gPeriodSn = LongFromChar(&pSetIoInfoBuff[BuffLenIndex]);
		BuffLenIndex += 4U;

		FuncRtn = FUNC_SUCESS;
	}
	else
	{
		FuncRtn = FUNC_CODE2;
	}

	return FuncRtn;
}
