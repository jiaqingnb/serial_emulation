/****************************************************************************************************
* �ļ���   :  IOInput.c
* ��Ȩ˵�� :  ���ؿƼ��ɷ����޹�˾
* �汾��   :  1.0
* ����ʱ�� :  2018.08.23
* ����     :  ������
* �������� : IO��������������ļ�
* ʹ��ע�� : �ϲ����ȵ���IoModuleInPut_Api_PreiodProcess(),�ٵ���IoModuleInPut_Api_GetDigtalInput()
* ��IoModuleInPut_Api_GetMainCtrSide();
*
* �޸ļ�¼ : �Ż������ӽӿں�����mjz-2018.10.28.
*
**************************************************************************************************/
#include "IOInput.h"
#include "IOModuleExtern.h"
#include "CommDataSourceExtern.h"
#include "CommABChooseExtern.h"
#include "dquVobcConfigData.h"
#include "PLFM_LOG.h"
#include "IOConfig.h"

IO_CFG_DATA_STRU	gIoConfigDataStru = { 0U };       /* IO�������ݽṹ��(������������һ���ṹ��)������Ҫͬ��*/
IO_INPUT_STRUCT		gIOInputStruMscp = { 0U };        /* IO����ṹ�����ڶ�Ӧ���������ڸ��£�����Ҫͬ�� */
IO_INPUT_STRUCT		gTc1IoInputStru = { 0U };	        /* ��1�˽��յ���IO���ݵĽṹ�壬���ڸ��£�����Ҫͬ��*/
IO_INPUT_STRUCT		gTc2IoInputStru = { 0U };         /* ��2�˽��յ���IO���ݵĽṹ�壬���ڸ��£�����Ҫͬ��*/
IO_INPUT_STRUCT     gLastTc1InputStru = { 0U };       /*������һ�˲ɼ��������ȫ�����Ѿ������ϣ�����Ҫͬ��*/
IO_INPUT_STRUCT     gLastTc2InputStru = { 0U };       /*�����ڶ��˲ɼ��������ȫ�����Ѿ������ϣ�����Ҫͬ��*/

IO_SINGLE_INPUT_STRUCT gIoSingleInput = { 0U };          /*IO�ɼ����ڵ�ɼ������ڸ��£�����Ҫͬ��*/
UINT8 *gIoSingleMapTable[MAX_CHANNELS_NUM] = { 0U };     /*IOӳ����ϵ�ʱ����ӳ�䣬����Ҫͬ��*/

UINT8 RecvTc1IoBuff[256] = { 0U, }; /* �����յ���TC1��IO��ԭʼ������ ����Ҫͬ��*/
UINT8 RecvTc2IoBuff[256] = { 0U, }; /*�����յ���Tc2��IO��ԭʼ������ ����Ҫͬ��*/
static UINT8 TC1cablastState = 0xAAU; /* ���ڴ洢�����ڵļ���״̬ ��Ҫͬ��*/
static UINT8 TC2cablastState = 0xAAU; /* ���ڴ洢�����ڵļ���״̬ ��Ҫͬ��*/

/*��ǰ���ڼ���ͳһʹ��gPeriodSn*/
static DIGITAL_IO_Time_MSCP gPeriodSn_Mscp_TC1[MAX_SAFE_INPUT_NUM] =
{
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U }
};								/*�������ڼ�������Ҫͬ��*/

static DIGITAL_IO_Time_MSCP gPeriodSn_Mscp_TC2[MAX_SAFE_INPUT_NUM] =
{
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },{ 0xAAU,0U },
	{ 0xAAU,0U }
};                              /*�������ڼ�������Ҫͬ��*/

static UINT32 gPeriodSn = 0U;	/*ģ������������ڼ�������Ҫͬ��*/

extern UINT8 ARuptime;

UINT8 gLastTc1_Cab_Key = CBTC_FALSE;
UINT8 Tc1_Tolerate_tic = 0U;
UINT8 gLastTc2_Cab_Key = CBTC_FALSE;
UINT8 Tc2_Tolerate_tic = 0U;
UINT8 gARReverStrTag = CBTC_FALSE;      /*����ARģʽ�۷��Ŀ�ʼ��־*/
UINT8 gFamDirBackTag = CBTC_FALSE;      /*FAM��������־*/
UINT8 gFamCtrlSide = CBTC_FALSE;
UINT8 gFamReverStrTag = CBTC_FALSE;

/* �����뿪��������������ṹ������������� */
IO_INPUT_INDEX gIoInputIndexTable[INPUT_FLAG_NUM_SINGLE] =
{
	/* ��ȫ�����Ӧ�� */
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
	/*-------------����Ϊ��ȫ����---------����Ϊ�ǰ�ȫ����------------*/
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
	/*--------------����Ϊ��ʿ�ᰲȫ&�ǰ�ȫ����------------*/
	{ IN_IVOC_VALIDITY_SINGLE, &(gIOInputStruMscp.InAtpIVOCValidity) },
	/* ˷��������ȷ�ϰ�ť2�����ں�Ϊ0x70,�������޸ĳ�0x3B����������һ��*/
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
	/*--------------����Ϊ�ɶ����߷ǰ�ȫ����------------*/
	{ IN_TRAIN_REDUNDANCY_SINGLE,&(gIOInputStruMscp.InTrainRedundancy) },
	{ IN_TRAIN_FAN1_SINGLE,&(gIOInputStruMscp.InTrainFan1) },
	{ IN_TRAIN_FAN2_SINGLE,&(gIOInputStruMscp.InTrainFan2) },
	{ IN_TRAIN_DOORLOCK_SINGLE,&(gIOInputStruMscp.InTrainDoorLock) },
	{ IN_TRAIN_ESCAPDOOR_UNLOCK_SINGLE,&(gIOInputStruMscp.InTrainEscapDoorUnlock) },
	{ IN_TRAIN_GUESTDOOR_UNLOCK_SINGLE,&(gIOInputStruMscp.InTrainGuestDoorUnlock) },
	{ IN_TRAIN_Reserved,&(gIoSingleInput.reservedInput) }
};


/* ��TC1�����뿪��������������ṹ������������� */
IO_INPUT_INDEX gTc1InputIndexTable[INPUT_FLAG_NUM] =
{
	/* ��ȫ�����Ӧ�� */
	/*��ʻ�Ҽ��������ڵ�һ���������ƶ������дIoModuleInPut_Output�����л�ȡ��ʻ�Ҽ���ķ�ʽ*/
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

	/*-------------����Ϊ��ȫ����---------����Ϊ�ǰ�ȫ����------------*/
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

	/*--------------����Ϊ��ʿ�ᰲȫ&�ǰ�ȫ����------------*/
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
	/* ˷��������ȷ�ϰ�ť2�����ں�Ϊ0x70,�������޸ĳ�0x3B����������һ��*/
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

	/*--------------����Ϊ�ɶ����߰�ȫ&�ǰ�ȫ����------------*/
	{ IN_TRAIN_DOORCLOSE,&(gTc1IoInputStru.InTrainDoorClose) },
	{ IN_TRAIN_EMERDOORBOARD,&(gTc1IoInputStru.InTrainEmergencyDoorBoard) },
	{ IN_TRAIN_REDUNDANCY,&(gTc1IoInputStru.InTrainRedundancy) },
	{ IN_TRAIN_FAN1,&(gTc1IoInputStru.InTrainFan1) },
	{ IN_TRAIN_FAN2,&(gTc1IoInputStru.InTrainFan2) },
	{ IN_TRAIN_DOORLOCK,&(gTc1IoInputStru.InTrainDoorLock) },
	{ IN_TRAIN_ESCAPDOOR_UNLOCK,&(gTc1IoInputStru.InTrainEscapDoorUnlock) },
	{ IN_TRAIN_GUESTDOOR_UNLOCK,&(gTc1IoInputStru.InTrainGuestDoorUnlock) },
};

/* ��TC2�����뿪��������������ṹ������������� */
IO_INPUT_INDEX gTc2InputIndexTable[INPUT_FLAG_NUM] =
{
	/* ��ȫ�����Ӧ�� */
	/*��ʻ�Ҽ��������ڵ�һ���������ƶ������дIoModuleInPut_Output�����л�ȡ��ʻ�Ҽ���ķ�ʽ*/
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

	/*-------------����Ϊ��ȫ����---------����Ϊ�ǰ�ȫ����------------*/
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
	/*--------------����Ϊ��ʿ�ᰲȫ&�ǰ�ȫ����------------*/
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
	/* ˷��������ȷ�ϰ�ť2�����ں�Ϊ0x70,�������޸ĳ�0x3B����������һ��*/
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

	/*--------------����Ϊ�ɶ����߰�ȫ&�ǰ�ȫ����------------*/
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
* ��������:			����IO���������
* �������:			UINT8 *pRecvBuf
* �����������:		��
* �������:			pInUnSafeData �ǰ�ȫֵ
*					pInSafeData ��ȫֵ
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
*					0x01:����ָ��Ϊ��
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleInputAnalysis(UINT8 *pRecvBuf, UINT32 *pInUnSafeData, UINT32 *pInSafeData);

/***************************************************************************************
* ��������:			�����յ���ԭʼ���ݽ���������ṹ����
* �������:			UINT16 ProType ��Ŀ��ʶ
* 					UINT16 DeviceTypeID ����ID
* �����������:     IO_CFG_DATA_STRU *pDataConfigStru IO�������ݽṹ��ָ��
* �������:      	UINT8 *pRecvBuf �յ���ԭʼ����
* ȫ�ֱ���:         ��
* ����ֵ:           0x00���ɹ���
* 					0x01:�����������ȷ
* 					0x02:����IO��������ݳ���
* 					0x04:�����������ݴ����Ӧ�ṹ�����
* 					0x08:���ݲ���IDѰ��IO���ñ�IDʧ��
****************************************************************************************/
static UINT8 IoModuleInputHandler(UINT8 whitchEnd, UINT16 DeviceTypeID, UINT8 *pRecvBuf, IO_CFG_DATA_STRU *pDataConfigStru);

/***************************************************************************************
* ��������:			�����Ϣ����
* �������:			IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo io����ṹ��
* �����������:     ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleInPut_Output(INOUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);

/***************************************************************************************
* ��������:			����������ӳ���ϵ
* �������:			IO��ʼ���ṹ��
* �����������:       ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��added by Jielu 20230406
****************************************************************************************/
static UINT8 IoModule_Api_Value_Map_Init_Mscp(IN const IO_CFG_DATA_STRU *pIoConfigDataStru);

/***************************************************************************************
* ��������:			��ȡ�����Ӧ�ı���ֵ
* �������:			BitMean-�������ֵ
* �����������:     ��
* �������:			��
* ȫ�ֱ���:			gIoOutputIndexTable
* ����ֵ:           ����ֵ��ַָ��
* �޸ļ�¼�� added by Jielu 20230406
****************************************************************************************/
static UINT8 *IoModuleGetValuePoint_Mscp(UINT8 BitMean);


/***************************************************************************************
* ��������:			�����������SID�忨�Ŀ���������55��AA����
* �������:			IN digitalData1:SID1�忨���ڵ�ɼ��Ŀ�������IN digitalData2:SID2�忨�ɼ��Ŀ�����
* �����������: ��
* �������:         OUT UINT8 *pRcvDigitalBuff
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��          added by Jielu 20230404
****************************************************************************************/
static void IoModuleInputSave_Mscp(IN UINT32 digitalData1, IN UINT32 digitalData2);


/***************************************************************************************
* ��������:			��ȫ�����봦��
* �������:			UINT8 NO			��ȫ����ĳ����ڵ�
*					UINT8 NC			��ȫ����ĳ��սڵ�
*					UINT8 OpenOrClosse  ������Ч������
*					UINT8 DEFAULT		Ĭ��ֵ
* �����������:		��
* �������:
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼�� added by Jielu 20230407
****************************************************************************************/
static UINT8 IoSafeInputProc_Mscp(UINT8 NO, UINT8 NC, UINT8 LastState, UINT8 OpenOrClosse, UINT8 DEFAULT, UINT32 *Err, DIGITAL_IO_Time_MSCP* PeriodSn_Mscp, UINT8 n);


/***********************************************************************
* ��������:        ʱ�Ӷ�ʱ����
* �������:         const UINT32 start��ʼ��ʱʱ��const UINT32 expiration ��ʱʱ��
* �����������:     ��
* �������:         ��
* ȫ�ֱ���:         ..
* ����ֵ:              CBTC_TRUE �Ѵﵽ��ʱʱ�䣻CBTC_FALSE δ�ﵽ��ʱʱ��
* �޸�ʱ��       added by Jielu 20230408
*************************************************************************/
static UINT8 IOTimerExpired_Mscp(const UINT32 start, const UINT32 expiration);

/***********************************************************************
* ��������:        ��ȡTC1������������
* �������:         UINT8 whitchend
* �����������:     ��
* �������:         ��
* ȫ�ֱ���:         ..
* ����ֵ:    TCI����������ַ
* �޸�ʱ��       added by Jielu 20230417
*************************************************************************/
static DIGITAL_IO_Time_MSCP* GetDigitalIO_Mscp_Lock(UINT8 whichEnd);

/***************************************************************************************
* ��������:			IO����ģ����Ҫ��ʼ����ֵ�Լ���ȡIO���õ����ݣ�����ӳ�������ݵ�ӳ���ϵ
* �������:			��
* �����������:		��
* �������:
* ȫ�ֱ���:
* ����ֵ:			0x00:��ʼ���ɹ�
*					0x01:��ȡIO��������ʧ��
* �޸ļ�¼��		added by Jielu 20230406
****************************************************************************************/
UINT8 IoModuleMscp_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru)
{
	UINT8 RtnNo = (UINT8)FUNC_CODE1;

	if (NULL != pIoConfigDataStru)
	{
		(void)CommonMemSet(&gIoSingleInput, sizeof(IO_SINGLE_INPUT_STRUCT), 0xFFU, sizeof(IO_SINGLE_INPUT_STRUCT));			/*���ڵ�ɼ�IO�ṹ��*/
		(void)CommonMemCpy(&gIoConfigDataStru, sizeof(IO_CFG_DATA_STRU), pIoConfigDataStru, sizeof(IO_CFG_DATA_STRU));
		(void)CommonMemSet(&gTc1IoInputStru, sizeof(IO_INPUT_STRUCT), 0x00U, sizeof(IO_INPUT_STRUCT)); 		/*��ʼ��TC1�ɼ���Ľڵ�*/
		(void)CommonMemSet(&gTc2IoInputStru, sizeof(IO_INPUT_STRUCT), 0x00U, sizeof(IO_INPUT_STRUCT)); 		/*��ʼ��TC2�ɼ���Ľڵ�*/

		(void)CommonMemSet(&gLastTc1InputStru, sizeof(IO_INPUT_STRUCT), 0x00, sizeof(IO_INPUT_STRUCT)); /*��ʼ��TC1�������ڽṹ��ڵ�*/
		(void)CommonMemSet(&gLastTc2InputStru, sizeof(IO_INPUT_STRUCT), 0x00, sizeof(IO_INPUT_STRUCT)); /*��ʼ��TC2�������ڽṹ��ڵ�*/
																										
		RtnNo = IoModule_Api_Value_Map_Init_Mscp(pIoConfigDataStru);	/*���忨λ��ַӳ����ʼ��*/
	}
	else
	{
		/*do nothing*/
	}

	return RtnNo;
}

/***************************************************************************************
* ��������:			����������ӳ���ϵ
* �������:			IO��ʼ���ṹ��
* �����������:       ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��added by Jielu 20230406
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
* ��������:		��ȡTC1������������
* �������:		��
* �����������:	��
* �������:		��
* ȫ�ֱ���:		
* ����ֵ:		TCI����������ַ
* �޸�ʱ��		added by Jielu 20230417
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
* ��������:			��ȡ�����Ӧ�ı���ֵ
* �������:			BitMean-�������ֵ
* �����������:     ��
* �������:			��
* ȫ�ֱ���:			gIoOutputIndexTable
* ����ֵ:           ����ֵ��ַָ��
* �޸ļ�¼�� added by Jielu 20230406
****************************************************************************************/
static UINT8 *IoModuleGetValuePoint_Mscp(UINT8 BitMean)
{
	UINT8 *retVal = NULL;
	UINT8 Index = 0U;

	if (0x00U == BitMean)
	{
		retVal = NULL;		/*����λ*/
	}
	else
	{
		/*��������ֵ�������Ӧ��*/
		for (Index = 0U; Index < INPUT_FLAG_NUM_SINGLE; Index++)
		{
			if (BitMean == gIoInputIndexTable[Index].IoInputTypeIndex)
			{
				/*����ӳ����ж�Ӧ����ֵ�ĵ�ַ*/
				retVal = gIoInputIndexTable[Index].IoInputStruIndex;
				break;
			}
		}
	}

	return retVal;
}

/***************************************************************************************
* ��������:			IO����ģ����Ҫ��ʼ����ֵ�Լ���ȡIO���õ�����
* �������:			��
* �����������:		��
* �������:
* ȫ�ֱ���:
* ����ֵ:			0x00:��ʼ���ɹ�
*					0x01:��ȡIO��������ʧ��
* �޸ļ�¼��
****************************************************************************************/
UINT8 IoModule_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru)
{
	UINT8 RtnNo = FUNC_CODE1;

	if (NULL != pIoConfigDataStru)
	{
		/*��ȡIO��������*/
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
* ��������:			���ݲ���ID���Ҷ�Ӧ����������Ϣ������
* �������:			UINT16 PlugBoxId ����ID
*					IO_CFG_DATA_STRU *pDataConfigStru  IO�������ݽṹ��ָ��
* �����������:		��
* �������:			UINT8 *pFindIndex  ���ݲ���ID���ҵ��������ýṹ���е�ID
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
*					0x01:���ݲ���IDδ�ҵ��������
*					0x02:�����������ȷ
* �޸ļ�¼��
****************************************************************************************/
UINT8 IoModuleInputFindConfigDataFromPlugBoxId(UINT16 PlugBoxId, IO_CFG_DATA_STRU *pDataConfigStru, UINT8 *pFindIndex)
{
	UINT8 Index = 0U;
	UINT8 RtnNo = FUNC_CODE1; 			/* ��������ֵ */

	if ((NULL != pDataConfigStru) && (NULL != pFindIndex))
	{
		/* ���ݲ���ID�������������е�ID ��ID��ͬ�����������е��������*/
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
				/* ����ID�ͱ��е�ID��ƥ�䲻������*/
			}
		}
	}
	else
	{
		/*�����������*/
		RtnNo = FUNC_CODE2;
	}

	return RtnNo;
}

/***************************************************************************************
* ��������:			����IO���������
* �������:			UINT8 *pRecvBuf
* �����������:		��
* �������:			pInUnSafeData �ǰ�ȫֵ
*					pInSafeData ��ȫֵ
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
*					0x01:����ָ��Ϊ��
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleInputAnalysis(UINT8 *pRecvBuf, UINT32 *pInUnSafeData, UINT32 *pInSafeData)
{
	UINT8 FuncRtn = FUNC_CODE1;
	if ((NULL != pRecvBuf) && (NULL != pInUnSafeData) && (NULL != pInSafeData))
	{
		/* �����ݽ�������Ӧ���ֽ��� */
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
* ��������:			�����յ���ԭʼ���ݽ���������ṹ����
* �������:
*					UINT16 DeviceTypeID ����ID
* �����������:		IO_CFG_DATA_STRU *pDataConfigStru IO�������ݽṹ��ָ��
* �������:			UINT8 *pRecvBuf �յ���ԭʼ����
* ȫ�ֱ���:			��
* ����ֵ:           0x00���ɹ���
*					0x01:�����������ȷ
*					0x02:����IO��������ݳ���
*					0x04:�����������ݴ����Ӧ�ṹ�����
*					0x08:���ݲ���IDѰ��IO���ñ�IDʧ��
*�޸ļ�¼��1.���յ��ڵ�ɼ��������ƴ��  added by Jielu 20230411
****************************************************************************************/
static UINT8 IoModuleInputHandler(UINT8 whichEnd, UINT16 DeviceTypeID, UINT8 *pRecvBuf, IO_CFG_DATA_STRU *pDataConfigStru)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT32 FuncRtn32 = 0U;
	UINT8 Rtn = FUNC_CODE1;
	UINT16 DeviceID = CBTC_NULL16;
	UINT8 PlugBoxIndex = 0U;		/*���������*/
	UINT32 digitalData1 = 0U; 		/* SID�忨1�ɼ������� */
	UINT32 digitalData2 = 0U;   	/* SID�忨2�ɼ������� */
	IO_INPUT_STRUCT *tmpLastInputStru = NULL;        /*��������ڲɼ��������ʱ�������Ѿ������ģ�*/
	DIGITAL_IO_Time_MSCP *tmpPeriodSn_Mscp = NULL;   /*��ʱ������̽ṹ��*/
	UINT8 TempConvertNC = CBTC_NULL;				/*���յ�λ��ʱת������*/

	DeviceID = DeviceTypeID;	/*ֱ��ȡ�豸ID (Lower 8bit),���ᶪʧ����*/
	if ((NULL != pRecvBuf) && (NULL != pDataConfigStru))
	{
		/*��ȡ������������һ�˵Ľṹ��*/
		tmpPeriodSn_Mscp = GetDigitalIO_Mscp_Lock(whichEnd);

		if (NULL != tmpPeriodSn_Mscp)
		{
			/*���ݲ���ID���Ҷ�Ӧ����������Ϣ��������*/
			Rtn = IoModuleInputFindConfigDataFromPlugBoxId(DeviceID, pDataConfigStru, &PlugBoxIndex);
			if ((FUNC_SUCESS == Rtn) && (TRAIN_INPUT_BOARD == pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex].BoxAttr))
			{
				/* ����IO��������� */
				Rtn = IoModuleInputAnalysis(pRecvBuf, &digitalData1, &digitalData2);

				/*������SID�忨�Ĳɼ�ֵ������Ӧת����������ӳ��table��*/
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
						/*��ʻ�Ҽ��������Ч*/
						gIOInputStruMscp.InTrainCabActive = IoSafeInputProc_Mscp(gIoSingleInput.InTrainCabActiveMscpNo, gIoSingleInput.InTrainCabActiveMscpNc, 
							tmpLastInputStru->InTrainCabActive, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 0U);
						
						/*EB��ʵʩ��������Ч*/
						gIOInputStruMscp.InTrainEbEnable = IoSafeInputProc_Mscp(gIoSingleInput.InTrainEbEnableMscpNo, gIoSingleInput.InTrainEbEnableMscpNc, 
							tmpLastInputStru->InTrainEbEnable, CLOOSE_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 1U);

						/*ǣ���г���������Ч*/
						gIOInputStruMscp.InTrainTractionCut = IoSafeInputProc_Mscp(gIoSingleInput.InTrainTractionCutMscpNo, gIoSingleInput.InTrainTractionCutMscpNc, 
							tmpLastInputStru->InTrainTractionCut, CLOOSE_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 2U);

						/*�����ֱ���󣬳�����Ч*/
						gIOInputStruMscp.InTrainDirBackward = IoSafeInputProc_Mscp(gIoSingleInput.InTrainDirBackwardMscpNo, gIoSingleInput.InTrainDirBackwardMscpNc, 
							tmpLastInputStru->InTrainDirBackward, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 3U);

						/*˾��Կ�׼��������Ч*/
						gIOInputStruMscp.TRAIN_KEY_OPEN = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_KEY_OPENMscpNo, gIoSingleInput.TRAIN_KEY_OPENMscpNc, 
							tmpLastInputStru->TRAIN_KEY_OPEN, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 4U);

						/*�г������ԣ�������Ч*/
						gIOInputStruMscp.InAtpTrainInteg = IoSafeInputProc_Mscp(gIoSingleInput.InAtpTrainIntegMscpNo, gIoSingleInput.InAtpTrainIntegMscpNc, 
							tmpLastInputStru->InAtpTrainInteg, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 5U);

						/*���Źر������գ�������Ч*/
						gIOInputStruMscp.InTrainDoorCloseAndLock = IoSafeInputProc_Mscp(gIoSingleInput.InTrainDoorCloseAndLockMscpNo, gIoSingleInput.InTrainDoorCloseAndLockMscpNc, 
							tmpLastInputStru->InTrainDoorCloseAndLock, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 6U);

						if (IO_NULL8_MSCP == gIOInputStruMscp.InTrainDoorCloseAndLock)
						{
							/*�������������û����͵ĳ��Źر������գ����Ի�ȡ��ʿ��˫1��Ч���Źر�������*/
							/*˫1��Ч������ȡ���󰴳�����Ч�ж�*/
							TempConvertNC = ~(gIoSingleInput.InDouble1DoorCloseLockMscpNc);
							gIOInputStruMscp.InTrainDoorCloseAndLock = IoSafeInputProc_Mscp(gIoSingleInput.InDouble1DoorCloseLockMscpNo, TempConvertNC,
								tmpLastInputStru->InTrainDoorCloseAndLock, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 6U);
						}
						else
						{
							/*nothing*/
						}

						/*�����ƶ���ʵʩ��������Ч*/
						gIOInputStruMscp.InAtpBczdValid = IoSafeInputProc_Mscp(gIoSingleInput.InAtpBczdValidMscpNo, gIoSingleInput.InAtpBczdValidMscpNc, 
							tmpLastInputStru->InAtpBczdValid, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 7U);

						/*ǣ���ƶ��ֱ�����λ�ҷ����ֱ�����ǰλ��������Ч*/
						gIOInputStruMscp.InAtpSbRightInput = IoSafeInputProc_Mscp(gIoSingleInput.InAtpSbRightInputMscpNo, gIoSingleInput.InAtpSbRightInputMscpNc, 
							tmpLastInputStru->InAtpSbRightInput, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 8U);

						/*ȷ�ϰ�ť״̬��������Ч*/
						gIOInputStruMscp.InTrainConfirmButton = IoSafeInputProc_Mscp(gIoSingleInput.InTrainConfirmButtonMscpNo, gIoSingleInput.InTrainConfirmButtonMscpNc, 
							tmpLastInputStru->InTrainConfirmButton, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 9U);

						/*���������ֱ����������Ч*/
						gIOInputStruMscp.TRAIN_EBHANDLE_ACTIVE = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNo, gIoSingleInput.TRAIN_EBHANDLE_ACTIVEMscpNc, 
							tmpLastInputStru->TRAIN_EBHANDLE_ACTIVE, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 10U);

						/*�ϰ����ѹ���Ч��������Ч*/
						gIOInputStruMscp.TRAIN_OBSTDET_INPUT = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNo, gIoSingleInput.TRAIN_OBSTDET_INPUTMscpNc, 
							tmpLastInputStru->TRAIN_OBSTDET_INPUT, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 11U);

						/*�ƶ��ع��ϣ�������Ч*/
						gIOInputStruMscp.TRAIN_BRAKE_FAULT = IoSafeInputProc_Mscp(gIoSingleInput.TRAIN_BRAKE_FAULTMscpNo, gIoSingleInput.TRAIN_BRAKE_FAULTMscpNc, 
							tmpLastInputStru->TRAIN_BRAKE_FAULT, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 12U);

						/*ǣ���ƶ��ֱ���ǣ��λ�ҷ�����ǰ��������Ч*/
						gIOInputStruMscp.ATP_TRACTION_RIGHT = IoSafeInputProc_Mscp(gIoSingleInput.ATP_TRACTION_RIGHTMscpNo, gIoSingleInput.ATP_TRACTION_RIGHTMscpNc, 
							tmpLastInputStru->ATP_TRACTION_RIGHT, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 13U);

						/*���Źرգ�������Ч*/
						gIOInputStruMscp.InTrainDoorClose = IoSafeInputProc_Mscp(gIoSingleInput.InTrainDoorCloseMscpNo, gIoSingleInput.InTrainDoorCloseMscpNc, 
							tmpLastInputStru->InTrainDoorClose, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 14U);

						/*�����Ÿǰ�״̬��������Ч*/
						gIOInputStruMscp.InTrainEmergencyDoorBoard = IoSafeInputProc_Mscp(gIoSingleInput.InTrainEmergencyDoorBoardMscpNo, gIoSingleInput.InTrainEmergencyDoorBoardMscpNc, 
							tmpLastInputStru->InTrainEmergencyDoorBoard, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 15U);

						/*������״̬��������Ч*/
						gIOInputStruMscp.EMERGENCYDOOR = IoSafeInputProc_Mscp(gIoSingleInput.EMERGENCYDOORMscpNo, gIoSingleInput.EMERGENCYDOORMscpNc, 
							tmpLastInputStru->EMERGENCYDOOR, CLOOSE_VALID_MSCP, IO_TRUE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 16U);

						/*����Disney��ȫ���룬��������˫1˫0��Ч����Ҫ��һ��ת��*/
						/*(Safe)CMģʽ��˫1��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpCMModeMscpNc);
						gIOInputStruMscp.InAtpCMMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpCMModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpCMMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 17U);
						
						/*(Safe)NOT SD ģʽ��˫1��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpNotSDModeMscpNc);
						gIOInputStruMscp.InAtpNotSDMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpNotSDModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpNotSDMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 18U);
						
						/*(Safe)FORģʽ��˫1��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpFORModeMscpNc);
						gIOInputStruMscp.InAtpFORMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpFORModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpFORMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 19U);
						
						/*(Safe)REVģʽ��˫1��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpREVModeMscpNc);
						gIOInputStruMscp.InAtpREVMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpREVModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpREVMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 20U);
						
						/*(Safe)��SBģʽ��˫1��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpSBModeMscpNc);
						gIOInputStruMscp.InAtpSBMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpSBModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpSBMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 21U);
						
						/*(Safe)��SDģʽ��˫1��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpSDModeMscpNc);
						gIOInputStruMscp.InAtpSDMode = IoSafeInputProc_Mscp(gIoSingleInput.InAtpSDModeMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpSDMode, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 22U);
						
						/*(Safe)�ź�ϵͳ�����ƶ���������ɼ���˫1��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpEBFeedbackMscpNc);
						gIOInputStruMscp.InAtpEBFeedback = IoSafeInputProc_Mscp(gIoSingleInput.InAtpEBFeedbackMscpNo, TempConvertNC,
							tmpLastInputStru->InAtpEBFeedback, OPEN_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 23U);
						
						/*(Safe)�����ź���ʵʩ��˫0��Ч������ȡ���󰴳�����Ч�ж�*/
						TempConvertNC = ~(gIoSingleInput.InAtpConductZeroSpeedMscpNc);
						gIOInputStruMscp.InAtpConductZeroSpeed = IoSafeInputProc_Mscp(gIoSingleInput.InAtpConductZeroSpeedMscpNo, TempConvertNC, 
							tmpLastInputStru->InAtpConductZeroSpeed, CLOOSE_VALID_MSCP, IO_FALSE_MSCP, &FuncRtn32, tmpPeriodSn_Mscp, 24U);
						
						/*(Safe)��ȫ�����̵���״̬�ɼ���˫1��Ч������ȡ���󰴳�����Ч�ж�*/
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

					/*�ǰ�ȫ�Ѿ���ֵ��*/
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
		/* ����������� */
		FuncRtn = FUNC_CODE1;
	}

	return FuncRtn;
}

/***************************************************************************************
* ��������:			�����������SID�忨�Ŀ���������55��AA����
* �������:			IN digitalData1:SID1�忨���ڵ�ɼ��Ŀ�������IN digitalData2:SID2�忨�ɼ��Ŀ�����
* �����������:		��
* �������:			OUT UINT8 *pRcvDigitalBuff
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
* �޸ļ�¼��		added by Jielu 20230404
****************************************************************************************/
static void IoModuleInputSave_Mscp(IN UINT32 digitalData1, IN UINT32 digitalData2)
{
	UINT8 loopIndex = 0u;   /*��ȫ���ǰ�ȫ������·��*/

	for (loopIndex = 0U; loopIndex < (MAX_CHANNELS_NUM / 2U); loopIndex++)
	{
		/*��һ��SID*/
		if (NULL == gIoSingleMapTable[loopIndex])
		{
			/*ָ��Ϊ��*/
		}
		else
		{
			*(gIoSingleMapTable[loopIndex]) = GET_BIT_FROM_INPUT_MSCP(digitalData2, loopIndex);
		}

		/*�ڶ���SID*/
		if (NULL == gIoSingleMapTable[loopIndex + (MAX_CHANNELS_NUM / 2U)])
		{
			/*ָ��Ϊ��*/
		}
		else
		{
			*(gIoSingleMapTable[loopIndex + (MAX_CHANNELS_NUM / 2U)]) = GET_BIT_FROM_INPUT_MSCP(digitalData1, loopIndex);
		}
	}
}

/***************************************************************************************
* ��������:			��ȫ�����봦��
* �������:			UINT8 NO			��ȫ����ĳ����ڵ�
*					UINT8 NC			��ȫ����ĳ��սڵ�
*					UINT8 OpenOrClosse  ������Ч������
*					UINT8 DEFAULT		Ĭ��ֵ
* �����������:     ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼�� added by Jielu 20230407
****************************************************************************************/
static UINT8 IoSafeInputProc_Mscp(UINT8 NO, UINT8 NC, UINT8 LastState, UINT8 OpenOrClosse, UINT8 DEFAULT, UINT32 *Err, DIGITAL_IO_Time_MSCP* PeriodSn_Mscp, UINT8 n)
{
	UINT8 FuncRtn = (UINT8)FUNC_SUCESS;
	UINT8 RtnValue = DEFAULT;
	UINT8 TemValue = IO_FALSE_MSCP;

	if ((NULL == Err) || (NULL == PeriodSn_Mscp))
	{
		/*��δ���*/
		RtnValue = DEFAULT;
		FuncRtn |= (UINT8)FUNC_CODE2;
	}
	else
	{
		/* ����ֵ�Ϸ��Լ��*/
		if (((IO_TRUE_MSCP == NO) || (IO_FALSE_MSCP == NO))
			&& ((IO_TRUE_MSCP == NC) || (IO_FALSE_MSCP == NC)))
		{
			/*��ȫ����ĳ�������ͳ��մ���ֵ�෴*/
			if (IO_NULL8_MSCP == (NO^NC))
			{
				/*����������Ч�Ҵ�����Ч��Ϊ�����򳣱մ�����Ч�Ҵ�����Ч��Ϊ����*/
				if (((IO_TRUE_MSCP == NO) && (OPEN_VALID_MSCP == OpenOrClosse))
					|| ((IO_TRUE_MSCP == NC) && (CLOOSE_VALID_MSCP == OpenOrClosse)))
				{
					RtnValue = IO_TRUE_MSCP;/*�ð�ȫ������Ч*/
				}
				else
				{
					RtnValue = IO_FALSE_MSCP;/*�ð�ȫ������Ч*/
				}
				PeriodSn_Mscp[n].TimeLock = IO_FALSE_MSCP;
			}
			else if (0x00U == (NO^NC))
			{
				/*��ȫ����ĳ�������ͳ��մ���ֵ��ͬʱ���������̣�����ʱ��Ϊ3������*/
				if (IO_FALSE_MSCP == PeriodSn_Mscp[n].TimeLock)
				{
					PeriodSn_Mscp[n].LastStateTime = gPeriodSn;
					PeriodSn_Mscp[n].TimeLock = IO_TRUE_MSCP;
				}
				TemValue = IOTimerExpired_Mscp(PeriodSn_Mscp[n].LastStateTime, 3U);
				if (IO_TRUE_MSCP != TemValue)
				{
					/*������ʱ���ڣ����������ڵ�״̬*/
					RtnValue = LastState;
				}
				else
				{
					/*��������ʱ�䣬����Ĭ��ֵ*/
					RtnValue = DEFAULT;
					*Err |= (1U << n);
				}

			}
			else
			{
				;/*���ɴ��֧��������*/
			}
		}
		else if ((IO_NULL8_MSCP == NO) && (IO_NULL8_MSCP == NC))
		{
			RtnValue = IO_NULL8_MSCP;/*�����δ���ã�����0xFF*/
		}
		else
		{
			/*����ֵ���Ϸ�*/
			RtnValue = DEFAULT;
			FuncRtn |= (UINT8)FUNC_CODE1;
		}
	}

	return (RtnValue);
}

/*************************************************************************
* ��������:			ʱ�Ӷ�ʱ����
* �������:			const UINT32 start��ʼ��ʱʱ��const UINT32 expiration ��ʱʱ��
* �����������:		��
* �������:			��
* ȫ�ֱ���:			..
* ����ֵ:			CBTC_TRUE �Ѵﵽ��ʱʱ�䣻CBTC_FALSE δ�ﵽ��ʱʱ��
* �޸�ʱ��			added by Jielu 20230408
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
* ��������:			�����Ϣ����
* �������:			IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo io����ṹ��
* �����������:     ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleInPut_Output(INOUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT16 Index = 0u;
	UINT16 Index4Io = 0u;
	UINT8 Tc1_CabA_Key = CBTC_FALSE;	/*1��/A�˼�ʻ�Ҽ���*/
	UINT8 Tc2_CabB_Key = CBTC_FALSE;	/*2��/B�˼�ʻ�Ҽ���*/

	if (NULL == pIoIn_OutputInfo)
	{
		FuncRtn = FUNC_CODE1;
	}
	else
	{
		/*��ȡ1��/A�˼�ʻ�Ҽ���*/
		if (NULL == gTc1InputIndexTable[IN_TRAIN_CAB_ACTIVE - 1U].IoInputStruIndex)
		{
			/*ָ��Ϊ��*/
		}
		else
		{
			Tc1_CabA_Key = *(gTc1InputIndexTable[IN_TRAIN_CAB_ACTIVE - 1U].IoInputStruIndex);
		}

		/*��ȡ2��/B�˼�ʻ�Ҽ���*/
		if (NULL == gTc2InputIndexTable[IN_TRAIN_CAB_ACTIVE - 1U].IoInputStruIndex)
		{
			/*ָ��Ϊ��*/
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

		/*������ֵ������ȵı�����˳��ֵ  ��ֹ������*/
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
			/*����д���ƶˡ��ǿ��ƶ˿�����*/
		}

		FuncRtn = FUNC_SUCESS;
	}

	return FuncRtn;
}

/***************************************************************************************
* ��������:			IO��������ݴ���,�����յ���IO���ݽ�����ŵ�IO�������ݵĽṹ����
* �������:			��
* �����������:		��
* �������:			IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo io����ṹ��
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
*					0x01:����TC1��IO��������
*					0x02:����TC1��IO��������
*					0x04:����TC2��IO��������
*					0x08:����TC2��IO��������
* �޸ļ�¼��		���ӵ��ڵ�ṹ���ʼ����added by Jielu 20230408
****************************************************************************************/
UINT8 IoModuleInPut_Api_PreiodProcess(OUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	RECV_SAME_DEVICE_DATA_STRU IoRecvStru = { 0 };	/* ���յ���IO��ԭʼ��Ϣ */
	UINT8 CallFuncRtn = FUNC_CODE1;				/* ���ú����ķ���ֵ */
	UINT8 FuncRtn = FUNC_CODE1;					/* �������ķ���ֵ */

	if (NULL == pIoIn_OutputInfo)
	{
		FuncRtn = FUNC_CODE1;
	}
	else
	{
		(void)CommonMemSet(&IoRecvStru, sizeof(RECV_SAME_DEVICE_DATA_STRU), (UINT8)0x00, sizeof(RECV_SAME_DEVICE_DATA_STRU));
		(void)CommonMemSet(&gIOInputStruMscp, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gTc1IoInputStru, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gIoSingleInput, sizeof(gIoSingleInput), 0xFFU, sizeof(gIoSingleInput));			/*���ڵ�ɼ�IO�ṹ��*/
		gPeriodSn += 1U;
		FuncRtn = FUNC_SUCESS;

		/*����1�˵�IO��Ϣ*/
		CallFuncRtn = CommonModeule_Api_ABChooseDataFunc(IO_TC1_IN_1_TYPE_ID, (UINT8)DATA_FROM_TC1, &IoRecvStru);
		if ((FUNC_SUCESS == CallFuncRtn) && (1U <= IoRecvStru.DataFramNum))
		{

			/* ���յ���IO���ݴ�ŵ������з���˷�Ƶ�IOģ����������*/
			CommonMemCpy(RecvTc1IoBuff, (UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen,
				IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf,
				(UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen);
			/*���ݲ���IDѰ����Ӧ�����ò���������ؽṹ���� */
			CallFuncRtn = IoModuleInputHandler((UINT8)DATA_FROM_TC1, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].TypeAndId, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf, &gIoConfigDataStru);
			if (FUNC_SUCESS == CallFuncRtn)
			{
				/* ���յ������ݴ�ŵ�1���������ݵĽṹ���� */
				CommonMemCpy(&gTc1IoInputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/*���������ڵ�����*/
				CommonMemCpy(&gLastTc1InputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/* ���´���Ϊ�˽��һ�����ڽ��յ�����Ϊ��ʻ��δ�������*/
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

		/*���ݳ�ʼ������װ��������*/
		(void)CommonMemSet(&IoRecvStru, sizeof(RECV_SAME_DEVICE_DATA_STRU), (UINT8)0x00, sizeof(RECV_SAME_DEVICE_DATA_STRU));
		(void)CommonMemSet(&gIOInputStruMscp, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gTc2IoInputStru, sizeof(IO_INPUT_STRUCT), (UINT8)0x00, sizeof(IO_INPUT_STRUCT));
		(void)CommonMemSet(&gIoSingleInput, sizeof(gIoSingleInput), 0xFFU, sizeof(gIoSingleInput));			/*���ڵ�ɼ�IO�ṹ��*/

		/*����2��IO��Ϣ*/
		CallFuncRtn = CommonModeule_Api_ABChooseDataFunc(IO_TC2_IN_1_TYPE_ID, (UINT8)DATA_FROM_TC2, &IoRecvStru);
		if ((FUNC_SUCESS == CallFuncRtn) && (1U <= IoRecvStru.DataFramNum))
		{

			/* ���յ���IO���ݴ�ŵ������з���˷�Ƶ�IOģ����������*/
			CommonMemCpy(RecvTc2IoBuff, (UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen,
				IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf,
				(UINT32)IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].DataLen);
			/*���ݲ���IDѰ����Ӧ�����ò���������ؽṹ���� */
			CallFuncRtn = IoModuleInputHandler((UINT8)DATA_FROM_TC2, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].TypeAndId, IoRecvStru.RcvDataStruBuf[IoRecvStru.DataFramNum - 1U].RecvBuf, &gIoConfigDataStru);

			if (FUNC_SUCESS == CallFuncRtn)
			{
				/* ���յ������ݴ�ŵ�2���������ݵĽṹ���� */
				CommonMemCpy(&gTc2IoInputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/*���������һ���ڵ�����*/
				CommonMemCpy(&gLastTc2InputStru, sizeof(IO_INPUT_STRUCT), &gIOInputStruMscp, sizeof(IO_INPUT_STRUCT));

				/* ���´���Ϊ�˽��һ�����ڽ��յ�����Ϊ��ʻ��δ�������*/
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
			/* �������׼����ȷ����������*/
		}
	}

	return FuncRtn;
}

/***************************************************************************************
* ��������:			�����յ��������ݽṹ�����IOHHUserģ��
* �������:			��
* �����������:	
* �������:			IO_INPUT_STRUCT        *pTc1HHIoInputStru;                ��1�˽��յ���IO���ݵĽṹ��
*					IO_INPUT_STRUCT        *pTc2HHIoInputStru;                ��2�˽��յ���IO���ݵĽṹ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					FUNC_CODE1:����ָ��ʧ��
* �޸ļ�¼��
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
* ��������:			�����յ�������������������IOWindPressģ��
* �������:			��
* �����������:
* �������:			UINT8 *pTc1IOBuff, 1��������
*					UINT8 *pTc2IOBuff 2��������
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					FUNC_CODE1:����ָ��ʧ��
* �޸ļ�¼��
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
* ��������:     ��¼��ģ��2x�������ݵ��׵�ַ�볤��
* �������:     ��
* �����������:  ��
* �������:     ��
* ȫ�ֱ���:     ��ģ������ȫ�־�̬����
* ����ֵ:      ��
* �޸ļ�¼��
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
* ��������:     ��ȡ��̬����
* �������:     *pGetIoInfoBuff  ��Ҫͬ����������
*				UINT16* pBuffLen  ��Ҫͬ�������ݳ���
*				UINT16 BuffMaxLen ��ģ��ͬ����������ݳ���
* �������:     ��
* ȫ�ֱ���:     ��
* ����ֵ:       FUNC_CODE1 Ĭ�ϴ���ֵ
*				FUNC_CODE2 �����ָ��
*				FUNC_CODE3 Get�����ݳ��ȹ���
*				FUNC_SUCESS ִ�гɹ���
* �޸ļ�¼��	lsn 2020/12/15
****************************************************************************************/
UINT8 GetIoInfoStaticDate(UINT8 *pGetIoInfoBuff, UINT16* pBuffLen, UINT16 BuffMaxLen)
{
	UINT16 BufLen = 0;
	UINT8 GetIoInfoArr[APP_FOLLOW_MAXLEN] = { 0, };
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 loopIndex = 0U;
	if ((NULL != pGetIoInfoBuff) && (NULL != pBuffLen))
	{
		GetIoInfoArr[BufLen++] = TC1cablastState; /* ���ڴ洢�����ڵļ���״̬ ��Ҫͬ��*/
		GetIoInfoArr[BufLen++] = TC2cablastState; /* ���ڴ洢�����ڵļ���״̬ ��Ҫͬ��*/
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
* ��������:		���þ�̬����
* �������:		*pSetIoInfoBuff ��ϵ��Ҫ������ϵ������
*				BuffLen �������ݵĳ���
*
* �������:		��
* ȫ�ֱ���:		��
* ����ֵ:		FUNC_CODE1 Ĭ�ϴ���ֵ
*				FUNC_CODE2 �����ָ��
*				FUNC_CODE3 Set�����ݳ��ȹ���
*				FUNC_SUCESS ִ�гɹ�
* �޸ļ�¼��	lsn 2020/12/15
****************************************************************************************/
UINT8 SetIoInfoStaticDate(UINT8 *pSetIoInfoBuff, UINT16 *BuffLen)
{
	UINT16 BuffLenIndex = 0;
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 loopIndex = 0U;

	if ((NULL != pSetIoInfoBuff) && (NULL != BuffLen))
	{
		TC1cablastState = pSetIoInfoBuff[BuffLenIndex++]; /* ���ڴ洢�����ڵļ���״̬ ��Ҫͬ��*/
		TC2cablastState = pSetIoInfoBuff[BuffLenIndex++]; /* ���ڴ洢�����ڵļ���״̬ ��Ҫͬ��*/
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
