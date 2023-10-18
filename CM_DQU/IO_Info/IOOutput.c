/****************************************************************************************************
* �ļ���   :  IOOutput.c
* ��Ȩ˵�� :  ���ؿƼ��ɷ����޹�˾
* �汾��   :  1.0
* ����ʱ�� :  2018.08.23
* ����     :  ������
* �������� :  IO���ģ�鴦�����ļ�
* ʹ��ע�� :  DO���ģ�鲻���Ŀ��ƶ˺ͷǿ��ƶ���Ϣ��ֻ�Ǹ����ռ�1�˺�2��
��������յ�λ���ô�������͡�
*
* �޸ļ�¼ :  �Ż���������֯���ȣ����ӽӿں�����mjz-2018.10.28.
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


IO_OUTPUT_STRUCT 		gIOOutputStru = { 0 }; 			/* IO����ṹ�����ڶ�Ӧ������*/
IO_OUTPUT_STRUCT 		gIOOutputStruCtrl = { 0 }; 		/* IO����ṹ�����ڶ�Ӧ������--���ƶ���Ϣ */
IO_OUTPUT_STRUCT 		gIOOutputStruIdle = { 0 }; 		/* IO����ṹ�����ڶ�Ӧ������--�ȴ�����Ϣ */
IO_OUTPUT_DATA_STRU 	gIoOutputDataStru = { 0 }; 		/* IO����Ĺ��ڲ���Լ���Ϣ�Ľṹ�� (�˽ṹ����ܻ�������������ṹ�������仯)*/
UINT8  gIvocCtrlType = 0U;

extern UINT8 gFamCtrlSide;
extern UINT8 gFamReverStrTag;

extern UINT8 gARReverStrTag;
UINT8 ARuptime = 0U;

/* ���������������������ṹ������������� */
#if ((1U == H_FAO_PROJECT_USER) || (1U == CC_PROJECT_USER))
IO_OUTPUT_INDEX gIoOutputIndexTable[OUTPUT_FLAG_NUM] =
{
	/* ��ȫ�����Ӧ�� */
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

	/* �ǰ�ȫ�����Ӧ�� */
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
	/* ��ȫ�����Ӧ�� */
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

	/* �ǰ�ȫ�����Ӧ�� */
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
	/* ��ȫ�����Ӧ�� */
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

	/* �ǰ�ȫ�����Ӧ�� */
	{ &(gIOOutputStru.OutPassNeutralFault),OUT_PASS_NEUTRAL_FAULT },
	{ &(gIOOutputStru.OutPassNeutralPreview),OUT_PASS_NEUTRAL_PREVIEW },
	{ &(gIOOutputStru.OutPassNeutralForce),OUT_PASS_NEUTRAL_FORCE },
	{ &(gIOOutputStru.OutPilotLamp1),OUT_PILOT_LAMP1 },
	{ &(gIOOutputStru.OutPilotLamp2),OUT_PILOT_LAMP2 },
	{ &(gIOOutputStru.OutVobcFsb1),OUT_VOBC_FSB_1 },
	{ &(gIOOutputStru.OutVobcFsb2),OUT_VOBC_FSB_2 },

	/*����11�����������������*/
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
* ��������:			��������ṹ���е�ֵת��Ϊbitλ�е�0��1
* �������:			UINT8 OutValue ��Ӧ����ṹ�����
* �����������:		��
* �������:			UINT8 *pIoValue bitλ��ֵָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleOutputProcess(UINT8 OutValue, UINT8 *pIoValue);

/***************************************************************************************
* ��������:			��IO����ṹ�������ݴ����������
* �������:			BOARD_IO_INFO_STRU *pDataConfigStru IO���õĽṹ��
* �����������:		��
* �������:			UINT32 *pOutUnSafeData ���ṹ���е�����ת��Ϊ�ǰ�ȫ�����������ָ��
*					UINT32 *pOutSafeData   ���ṹ���е�����ת��Ϊ��ȫ�����������ָ��
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
****************************************************************************************/
static UINT8 IoModuleOutputPackage(BOARD_IO_INFO_STRU *pDataConfigStru, UINT32 *pOutUnSafeData, UINT32 *pOutSafeData);

/***************************************************************************************
* ��������:			��IO������ݷŵ��ֽ����У�H-FAOʹ�ã�
* �������:			IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO����ṹ��
*					IN UINT32 OutUnSafeData 	  ����ķǰ�ȫ����(�ǰ�ȫ������)
*					IN UINT32 OutSafeData   	  ����İ�ȫ����(��ȫ������)
* �����������:		��
* �������:			OUT UINT8 *pSendBuf		      ���������ָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleOutputBuf_with_Ana(IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UINT32 OutUnSafeData, IN UINT32 OutSafeData, OUT UINT8 *pSendBuf);

/***************************************************************************************
* ��������:			��IO������ݷŵ��ֽ����У�HHTC��CCʹ�ã�
* �������:			UINT32 OutUnSafeData ����ķǰ�ȫ����
*					UINT16 OutSafeData   ����İ�ȫ����
*					UINT32 dwAnaValue    �����ģ����
*					UINT32 dwPwmValue    �����PWM
* �����������:		��
* �������:			UINT8 *pSendBuf ���������ָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleOutputBuf(UINT8 *pSendBuf, UINT32 OutUnSafeData, UINT32 OutSafeData);

/***************************************************************************************
* ��������:			��IO����ṹ���е�����ת��Ϊ������(H-FAO)
* �������:			IN UINT16 PlugBoxId ����ID
*					IN IO_CFG_DATA_STRU *pDataConfigStru IO�������ݽṹ��ָ��
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO����ṹ��
* �����������:		��
* �������:			OUT UINT8 *pSendBuf ���������ָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
*					0x02:����IO��������ݳ���
*					0x04:���ݲ���IDѰ��IO���ñ�IDʧ��
*					0x08:����ȫ����ͷǰ�ȫ��������ݴ�ŵ����͵���������ʧ��
****************************************************************************************/
static UINT8 IoModuleOutputHandler_with_Ana(IN UINT16 PlugBoxId, OUT UINT8 *pSendBuf, IN IO_CFG_DATA_STRU *pDataConfigStru, IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo);

/***************************************************************************************
* ��������:			��IO����ṹ���е�����ת��Ϊ������(HHTC,CC)
* �������:			UINT16 PlugBoxId ����ID
*					UINT8 *pSendBuf �յ���ԭʼ����
*					IO_CFG_DATA_STRU *pDataConfigStru IO�������ݽṹ��ָ��
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
*					0x02:����IO��������ݳ���
*					0x04:���ݲ���IDѰ��IO���ñ�IDʧ��
*					0x08:����ȫ����ͷǰ�ȫ��������ݴ�ŵ����͵�������ʧ��
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleOutputHandler(UINT16 PlugBoxId, UINT8 *pSendBuf, IO_CFG_DATA_STRU *pDataConfigStru);

/***************************************************************************************
* ��������:			����IO��������Ӧ��IO�����bitλ��ŵ��ֽ���
* �������:			UINT8 CtrlSide ��ǰ�Ŀ��ƶ�
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��������ȫ�ֱ���
* ����ֵ:			0x00���ɹ���
*					FUNC_CODE1:IoOutSlotNum IO�������������ȷ
*					FUNC_CODE2:AtpData_Api_GetOutDeviceTypeId�����������
****************************************************************************************/
static UINT8 IoModule_SlotNumIdPrepare(UINT8 CtrlSide);

/***************************************************************************************
* ��������:			���˿������������׼��
* �������:			CtrlIdleType:������
*					CTRL_SIDE:���ƶ�
*					IDLE_SIDE:�ǿ��ƶ�
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��
* ����ֵ:			FUNC_SUCESS���ɹ���
*					FUNC_CODE1:�����Բ�������
*					FUNC_CODE2:���ƶ����������׼�����ش���
*					FUNC_CODE3:�ǿ��ƶ����������׼�����ش���
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModule_SingleSideOutputPrepare(UINT8 CtrlSide, UINT8 CtrlIdleType);

/***************************************************************************************
* ��������:			�����������׼���������H-FAO��
* �������:			IN UINT8 CtrlSide:
*					CAB_A_TC1:1��/A��
*					CAB_B_TC2:2��/B��
*					IN UINT8 CtrlIdleType:
*					CTRL_SIDE:���ƶ�
*					IDLE_SIDE:�ǿ��ƶ�
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO����ṹ��
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��������ȫ�ֱ���
* ����ֵ:			FUNC_SUCESS���ɹ���
*					FUNC_CODE1:��������������ID���ش���
*					FUNC_CODE2:���������׼�����ش���
*					FUNC_CODE3:���ӳ�䷵�ش���
*					FUNC_CODE4:���������ش���
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput_with_Ana(IN UINT8 CtrlSide, IN UINT8 CtrlIdleType, IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo);

/***************************************************************************************
* ��������:			�����������׼�������(HHTC,CC)
* �������:			CtrlSide:
*					CAB_A_TC1:1��/A��
*					CAB_B_TC2:2��/B��
*					CtrlIdleType:
*					CTRL_SIDE:���ƶ�
*					IDLE_SIDE:�ǿ��ƶ�
* �����������:		��
* �������:			��
* ȫ�ֱ���:			gIoOutputDataStru,gIoConfigDataStru
* ����ֵ:			FUNC_SUCESS���ɹ���
*					FUNC_CODE1:��������������ID���ش���
*					FUNC_CODE2:���������׼�����ش���
*					FUNC_CODE3:���ӳ�䷵�ش���
*					FUNC_CODE4:���������ش���
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput(UINT8 CtrlSide, UINT8 CtrlIdleType);

/***************************************************************************************
* ��������:			IO���ģ����Ҫ��ʼ����ֵ�Լ���ȡIO���õ�����
* �������:			��
* �����������:		��
* �������:
* ȫ�ֱ���:
* ����ֵ:			0x00:��ʼ���ɹ�
*					0x01:��ȡIO��������ʧ��
* �޸ļ�¼��
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
* ��������:			����IO��������Ӧ��IO�����bitλ��ŵ��ֽ���
* �������:			UINT8 Index �������
*					UINT8 OutValue �������������ϵ�bitλ��ֵ
* �����������:     ��
* �������:			UINT32 *pOutUnSafeData ���ṹ���е�����ת��Ϊ�ǰ�ȫ�����������ָ��
*					UINT16 *pOutSafeData   ���ṹ���е�����ת��Ϊ��ȫ�����������ָ��
* ȫ�ֱ���:         ��
* ����ֵ:           0x00���ɹ���
*					0x01:�����������ȷ
*					0x02:����ֵ����ȷ
****************************************************************************************/
UINT8 IoModuleBitToByte(UINT8 Index, UINT8 OutValue, UINT32 *pOutUnSafeData, UINT32 *pOutSafeData)
{
	UINT8 RtnNo = FUNC_CODE1;

	if ((NULL != pOutUnSafeData) && (NULL != pOutSafeData))
	{
		/*���������ڵ���22ʱΪ��ȫ���룬������С��22ʱΪ�ǰ�ȫ���� */
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
					/* ��������0*/
				}

				RtnNo = FUNC_SUCESS;
			}
			else
			{
				/* �������� */
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
				/* ��λ����Ϊ0*/
			}

			RtnNo = FUNC_SUCESS;
		}
	}
	else
	{
		/* ���δ��� */
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* ��������:			��������ṹ���е�ֵת��Ϊbitλ�е�0��1
* �������:			UINT8 OutValue ��Ӧ����ṹ�����
* �����������:		��
* �������:			UINT8 *pIoValue bitλ��ֵָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼��
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
* ��������:			��IO����ṹ�������ݴ����������
* �������:			BOARD_IO_INFO_STRU *pDataConfigStru IO���õĽṹ��
* �����������:		��
* �������:			UINT32 *pOutUnSafeData ���ṹ���е�����ת��Ϊ�ǰ�ȫ�����������ָ��
*					UINT32 *pOutSafeData   ���ṹ���е�����ת��Ϊ��ȫ�����������ָ��
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
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
		/* ����IO���ñ���ÿһ·��IO�ĺ��� */
		for (Index = 0U; Index < (UINT8)MAX_CHANNELS_NUM; Index++)
		{
			/* ����IO����������ṹ���Ӧ�������� */
			for (IoTableIndex = 0U; IoTableIndex < OUTPUT_FLAG_NUM; IoTableIndex++)
			{
				/* ��IO���ñ��е�IO�������������Ӧʱ������Ӧ������ṹ���е��������ֵ */
				if (pDataConfigStru->GfgIoBuf[Index] == gIoOutputIndexTable[IoTableIndex].IoOutputTypeIndex)
				{
					/* ���ṹ���е�ֵת��Ϊ0����1��bitֵ */
					FuncRtn = IoModuleOutputProcess(*(gIoOutputIndexTable[IoTableIndex].IoOutputStruIndex), &BitValue);
					if (FUNC_SUCESS == FuncRtn)
					{
						/* ��bitλ����������ŵ���ȫ����ͷǰ�ȫ������ֽ��� */
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
					/*�޲���*/
				}
			}
		}
	}
	else
	{
		/*�����������*/
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* ��������:			��IO������ݷŵ��ֽ����У�HHTC��CCʹ�ã�
* �������:			UINT32 OutUnSafeData ����ķǰ�ȫ����
*					UINT16 OutSafeData   ����İ�ȫ����
*					UINT32 dwAnaValue    �����ģ����
*					UINT32 dwPwmValue    �����PWM
* �����������:		��
* �������:			UINT8 *pSendBuf ���������ָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleOutputBuf(UINT8 *pSendBuf, UINT32 OutUnSafeData, UINT32 OutSafeData)
{
	UINT8 FuncRtn = FUNC_CODE1;
	UINT8 i = 0U;
	if (NULL != pSendBuf)
	{
		AppLogPrintStr(ENUM_LOG_ERR, "Out safe %x,Out Unsafe %x\n", OutSafeData, OutUnSafeData);
		/* �����ݴ������Ӧ���������� */
		LongToChar(0, &pSendBuf[i]); /*���*/
		i = i + 4U;
		LongToChar(OutUnSafeData, &pSendBuf[i]); /* ������ķǰ�ȫ���ݴ��������� */
		i = i + 4U;
		LongToChar(OutSafeData, &pSendBuf[i]); /* ������İ�ȫ���ݴ����������� */
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
* ��������:			��IO������ݷŵ��ֽ����У�H-FAOʹ�ã�
* �������:			IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO����ṹ��
*					IN UINT32 OutUnSafeData 	  ����ķǰ�ȫ����(�ǰ�ȫ������)
*					IN UINT32 OutSafeData   	  ����İ�ȫ����(��ȫ������)
* �����������:		��
* �������:			OUT UINT8 *pSendBuf		      ���������ָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModuleOutputBuf_with_Ana(IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UINT32 OutUnSafeData, IN UINT32 OutSafeData, OUT UINT8 *pSendBuf)
{
	UINT8 FuncRtn = (UINT8)FUNC_CODE1;
	UINT8 i = 0u;

	if ((NULL != pIoOut_InputInfo) && (NULL != pSendBuf))
	{
		AppLogPrintStr(ENUM_LOG_ERR, "Out safe 0x%x, Out Unsafe 0x%x, AnaValue %d, PwmValue %d; Enable 0x%x\n",
			OutSafeData, OutUnSafeData, pIoOut_InputInfo->dwCtrlSideAnaValue, pIoOut_InputInfo->dwCtrlSidePwmValue, pIoOut_InputInfo->AtoCmdEnable);

#ifdef SIM_VERSION   /*������������֮ǰ����������ݸ�ʽ*/
		/* �����ݴ������Ӧ���������� */
		CM_LongToChar(0, &pSendBuf[i]); /*���*/
		i = i + 4U;
		CM_LongToChar(OutUnSafeData, &pSendBuf[i]); /* ������ķǰ�ȫ���ݴ��������� */
		i = i + 4U;
		CM_LongToChar(OutSafeData, &pSendBuf[i]); /* ������İ�ȫ���ݴ����������� */
		i = i + 4U;
		CM_LongToChar(pIoOut_InputInfo->dwCtrlSideAnaValue, &pSendBuf[i]);	/*4�ֽ�ģ����*/
		i = i + 4U;
		CM_LongToChar(pIoOut_InputInfo->dwCtrlSidePwmValue, &pSendBuf[i]);	/*4�ֽ�PWMռ�ձ�*/
		i = i + 4U;
		CM_LongToChar(OUT_PWM_FREQUENCY, &pSendBuf[i]);/*4�ֽ�PWMƵ��*/
		i = i + 4U;
#else   /*��ƽ̨�¸�ʽ*/
		/* �����ݴ������Ӧ���������� ,���ݰ�������DO_DATA_OUTPUT_BUF_SIZE��ֵҪһ��*/
		/*���ں�*/
		LongToChar(pIoOut_InputInfo->CycCount, &pSendBuf[i]);
		i += 4u;

		/*�ǰ�ȫ������Чλ����*/
		LongToChar(pIoOut_InputInfo->OutUnSafeDataMask, &pSendBuf[i]);
		i += 4u;

		/*����ķǰ�ȫ����(�ǰ�ȫ������)*/
		LongToChar(OutUnSafeData, &pSendBuf[i]);
		i += 4u;

		/*��ȫ��������Чλ����*/
		LongToChar(pIoOut_InputInfo->OutSafeDataMask, &pSendBuf[i]);
		i += 4u;

		/*����İ�ȫ����(��ȫ������)*/
		LongToChar(OutSafeData, &pSendBuf[i]);
		i += 4u;

		/*�����ģ����������Ч����,0xaa:��Ч,0x55:��Ч*/
		pSendBuf[i] = pIoOut_InputInfo->dwAnaValueValid;
		i += 1u;

		/*�����ģ��������*/
		LongToChar(pIoOut_InputInfo->dwCtrlSideAnaValue, &pSendBuf[i]);
		i += 4u;

		/*PWM������Ч����,0xaa:��Ч,0x55:��Ч*/
		pSendBuf[i] = pIoOut_InputInfo->dwPwmValueValid;
		i += 1u;

		/*�����PWMֵ����*/
		LongToChar(pIoOut_InputInfo->dwCtrlSidePwmValue, &pSendBuf[i]);
		i += 4u;

		/*�����PWMֵ����Ƶ��*/
		LongToChar(OUT_PWM_FREQUENCY, &pSendBuf[i]);
		i += 4u;

		/*ATO�����Ȩ��־,ATP���,ATOԤ��,0xaa:��Ч,0x55:��Ч*/
		pSendBuf[i] = pIoOut_InputInfo->AtoCmdEnable;
		i += 1u;

		/*Ԥ��7�ֽ�*/
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
* ��������:			��IO����ṹ���е�����ת��Ϊ������(HHTC,CC)
* �������:			UINT8 PlugBoxId ����ID
*					UINT8 *pRecvBuf �յ���ԭʼ����
*					IO_DATA_CONFIG_STRUT *pDataConfigStru IO�������ݽṹ��ָ��
* �����������:		��
* �������:
* ȫ�ֱ���:
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
*					0x02:����IO��������ݳ���
*					0x04:���ݲ���IDѰ��IO���ñ�IDʧ��
*					0x08:����ȫ����ͷǰ�ȫ��������ݴ�ŵ����͵�������ʧ��
* �޸ļ�¼��
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
		/*���ݲ���ID���Ҷ�Ӧ���������Ϣ��������*/
		FuncRtn = IoModuleInputFindConfigDataFromPlugBoxId(PlugBoxId, pDataConfigStru, &PlugBoxIndex);
		if ((FUNC_SUCESS == FuncRtn) && (TRAIN_OUTPUT_BOARD == pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex].BoxAttr))
		{
			/* ��IO�ṹ���е����ݸ���IO���ñ�����ŵ�������ֽ��� */
			FuncRtn = IoModuleOutputPackage(&pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex], &OutUnSafeData, &OutSafeData);
			if (FUNC_SUCESS == FuncRtn)
			{
				/* ����ȫ����ͷǰ�ȫ��������ݴ�ŵ����͵��������� */

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
		/* ����������� */
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* ��������:			��IO����ṹ���е�����ת��Ϊ������(H-FAO)
* �������:			IN UINT16 PlugBoxId ����ID
*					IN IO_CFG_DATA_STRU *pDataConfigStru IO�������ݽṹ��ָ��
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO����ṹ��
* �����������:		��
* �������:			OUT UINT8 *pSendBuf ���������ָ��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					0x01:�����������ȷ
*					0x02:����IO��������ݳ���
*					0x04:���ݲ���IDѰ��IO���ñ�IDʧ��
*					0x08:����ȫ����ͷǰ�ȫ��������ݴ�ŵ����͵���������ʧ��
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
		/*���ݲ���ID���Ҷ�Ӧ���������Ϣ��������*/
		FuncRtn = IoModuleInputFindConfigDataFromPlugBoxId(PlugBoxId, pDataConfigStru, &PlugBoxIndex);
		if ((FUNC_SUCESS == FuncRtn) && (TRAIN_OUTPUT_BOARD == pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex].BoxAttr))
		{
			/*��IO�ṹ���е����ݸ���IO���ñ�����ŵ�������ֽ���*/
			FuncRtn = IoModuleOutputPackage(&pDataConfigStru->BoardIoMsgBuf[PlugBoxIndex], &OutUnSafeData, &OutSafeData);
			if (FUNC_SUCESS == FuncRtn)
			{
				/*����ȫ����ͷǰ�ȫ��������ݴ�ŵ����͵���������*/
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
					/*������־��¼�����������*/
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
						/*�忨���Ͳ���ȷ���������*/
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
		/* ����������� */
		RtnNo = FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* ��������:			����IO��������Ӧ��IO�����bitλ��ŵ��ֽ���
* �������:			UINT8 CtrlSide ��ǰ�Ŀ��ƶ�
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��������ȫ�ֱ���
* ����ֵ:			0x00���ɹ���
*					FUNC_CODE1:IoOutSlotNum IO�������������ȷ
*					FUNC_CODE2:AtpData_Api_GetOutDeviceTypeId�����������
****************************************************************************************/
static UINT8 IoModule_SlotNumIdPrepare(UINT8 CtrlSide)
{
	UINT8 RtnNum = FUNC_CODE1;		/* �������ķ���ֵ */
	UINT8 CallFuncRtn = FUNC_CODE1;		/* ���ú����ķ���ֵ */
	UINT16 OutputBoxDeviceTypeId[BOARD_CONFIG_NUM] = { 0U };	/*��������豸typeID*/
	UINT8 OutputBoxNum = 0U;	/*������� ���� */
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
* ��������:			���˿������������׼��
* �������:			CtrlIdleType:������
*					CTRL_SIDE:���ƶ�
*					IDLE_SIDE:�ǿ��ƶ�
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��
* ����ֵ:			FUNC_SUCESS���ɹ���
*					FUNC_CODE1:�����Բ�������
*					FUNC_CODE2:���ƶ����������׼�����ش���
*					FUNC_CODE3:�ǿ��ƶ����������׼�����ش���
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModule_SingleSideOutputPrepare(UINT8 CtrlSide, UINT8 CtrlIdleType)
{
	UINT8 FuncRtn = FUNC_CODE1;		/* �������ķ���ֵ */

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

	/*��FAM/AR�۷���ʱ��ԭ���ƶ�����AR�̵�����ԭβ������AR�̵���*/
	if ((CBTC_TRUE == gFamReverStrTag) || (CBTC_TRUE == gARReverStrTag))
	{
		if (gFamCtrlSide == CtrlSide)
		{
			gIOOutputStru.TRAIN_CAB_ACTIVE_OUTPUT = CBTC_FALSE;
		}
		else
		{
			/*β��AR�̵���������ʱ��*/
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
* ��������:			�����������׼�������(HHTC,CC)
* �������:			CtrlSide:
*					CAB_A_TC1:1��/A��
*					CAB_B_TC2:2��/B��
*					CtrlIdleType:
*					CTRL_SIDE:���ƶ�
*					IDLE_SIDE:�ǿ��ƶ�
* �����������:		��
* �������:			��
* ȫ�ֱ���:			gIoOutputDataStru,gIoConfigDataStru
* ����ֵ:			FUNC_SUCESS���ɹ���
*					FUNC_CODE1:��������������ID���ش���
*					FUNC_CODE2:���������׼�����ش���
*					FUNC_CODE3:���ӳ�䷵�ش���
*					FUNC_CODE4:���������ش���
* �޸ļ�¼��
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput(UINT8 CtrlSide, UINT8 CtrlIdleType)
{
	UINT8 CallFuncRtn = FUNC_CODE1;    	/* ���ú����ķ���ֵ */
	UINT8 FuncRtn = FUNC_CODE1;      /* �������ķ���ֵ */
	UINT8 FrameIndex = 0U;

	/*׼����������������ID*/
	CallFuncRtn = IoModule_SlotNumIdPrepare(CtrlSide);
	if (FUNC_SUCESS == CallFuncRtn)
	{
		/*׼�����������*/
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
						/* �忨���Ͳ���ȷ���������*/
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
* ��������:			�����������׼���������H-FAO��
* �������:			IN UINT8 CtrlSide:
*					CAB_A_TC1:1��/A��
*					CAB_B_TC2:2��/B��
*					IN UINT8 CtrlIdleType:
*					CTRL_SIDE:���ƶ�
*					IDLE_SIDE:�ǿ��ƶ�
*					IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO����ṹ��
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��������ȫ�ֱ���
* ����ֵ:			FUNC_SUCESS���ɹ���
*					FUNC_CODE1:��������������ID���ش���
*					FUNC_CODE2:���������׼�����ش���
*					FUNC_CODE3:���ӳ�䷵�ش���
*					FUNC_CODE4:���������ش���
****************************************************************************************/
static UINT8 IoModule_SingleSideOutput_with_Ana(IN UINT8 CtrlSide, IN UINT8 CtrlIdleType, IN const IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo)
{
	UINT8 CallFuncRtn = FUNC_CODE1;    	/* ���ú����ķ���ֵ */
	UINT8 FuncRtn = FUNC_CODE1;      /* �������ķ���ֵ */
	UINT8 FrameIndex = 0U;

	/*׼����������������ID*/
	CallFuncRtn = IoModule_SlotNumIdPrepare(CtrlSide);
	if ((FUNC_SUCESS == CallFuncRtn) && (NULL != pIoOut_InputInfo))
	{
		/*׼�����������*/
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
						/*�忨���Ͳ���ȷ���������*/
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
* ��������:			IO����ṹ���е����ݴ�������������ֱ��͸�1�˺�2��
* �������:			IN IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO����ṹ��
* �����������:		��
* �������:			��
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
*					FUNC_CODE1:��ȡ���ƶ�ʧ��
*					FUNC_CODE2:�п��ƶ�ʱ�����ض����ʧ�ܣ�
*					FUNC_CODE3:�п��ƶ�ʱ�������ض����ʧ�ܣ�
*					FUNC_CODE4:�޿��ƶ�ʱ��TC1�����ʧ�ܣ�
*					FUNC_CODE5:�޿��ƶ�ʱ��TC2�����ʧ�ܣ�
* �޸ļ�¼��
****************************************************************************************/
UINT32 IoModulOutPut_Api_PreiodProcess(IN IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UCHAR LastActiveState)
{
	UINT8 CallFuncRtn = FUNC_CODE1;    	/* ���ú����ķ���ֵ */
	UINT32 FuncRtn = FUNC_CODE1;        /* �������ķ���ֵ */

	if (NULL != pIoOut_InputInfo)
	{
		(void)CommonMemCpy(&gIOOutputStruCtrl, sizeof(IO_OUTPUT_STRUCT), &pIoOut_InputInfo->IOOutputStruCtrl, sizeof(IO_OUTPUT_STRUCT));
		(void)CommonMemCpy(&gIOOutputStruIdle, sizeof(IO_OUTPUT_STRUCT), &pIoOut_InputInfo->IOOutputStruIdle, sizeof(IO_OUTPUT_STRUCT));
		if (ONE_CTRL_ONE == gIvocCtrlType)
		{
			if (CBTC_TRUE == pIoOut_InputInfo->LocalVobcAttri)
			{
				/*���ƶ����*/
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
						/* �������*/
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
						/* �������*/
					}
				}
				else
				{
					/*�޲���*/
				}
			}
			/*�Զ�Ϊ���ƶ�*/
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
						/* �������*/
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
						/* �������*/
					}
				}
				else
				{
					/*�޲���*/
				}
			}
		}
		/*���˿�����*/
		else
		{
			if (((CAB_A_TC1 == pIoOut_InputInfo->MainCtrlSide) && (CAB_B_TC2 == pIoOut_InputInfo->IdleSide)) || ((CAB_B_TC2 == pIoOut_InputInfo->MainCtrlSide) && (CAB_A_TC1 == pIoOut_InputInfo->IdleSide)))
			{
				/*�п��ƶ�*/
				/*���ƶ����*/
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
					/* �������*/
				}

				/*�ǿ��ƶ����*/
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
					/* �������*/
				}
			}
			else
			{
				/*�޵������ƶ�ʱ(�������˶�Ϊ���ƶ˻��߶���Ϊ���ƶˣ����߲ɼ��쳣ʱ)��������˶��Կ��ƶ˽������*/
				if (1U == CC_PROJECT_USER)
				{
					/*�����������TC1��*/
					if (CAB_A_TC1 == pIoOut_InputInfo->WakeCtrlIVOC)
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_A_TC1, CTRL_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE6;
						}
						else
						{
							/* �������*/
						}
						/*�ǿ��ƶ����*/
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_B_TC2, IDLE_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE7;
						}
						else
						{
							/* �������*/
						}
					}
					/*�����������TC2��*/
					else if (CAB_B_TC2 == pIoOut_InputInfo->WakeCtrlIVOC)
					{
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_B_TC2, CTRL_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE6;
						}
						else
						{
							/* �������*/
						}

						/*�ǿ��ƶ����*/
						CallFuncRtn = IoModule_SingleSideOutput_with_Ana(CAB_A_TC1, IDLE_SIDE, pIoOut_InputInfo);

						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE7;
						}
						else
						{
							/* �������*/
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
							/* �������*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE9;
						}
						else
						{
							/* �������*/
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
							/* �������*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE11;
						}
						else
						{
							/* �������*/
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
							/* �������*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE13;
						}
						else
						{
							/* �������*/
						}
					}
					else if (CBTC_TRUE == pIoOut_InputInfo->IOOutputStruCtrl.TRAIN_CAB_ACTIVE_OUTPUT)
					{
						/*��ʻ�Ҽ����źŶ�ʧ*/
						if (CAB_A_TC1 == LastActiveState)
						{
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_A_TC1, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE14;
							}
							else
							{
								/* �������*/
							}
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE15;
							}
							else
							{
								/* �������*/
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
								/* �������*/
							}
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE17;
							}
							else
							{
								/* �������*/
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
								/* �������*/
							}
							CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
							if (FUNC_SUCESS != CallFuncRtn)
							{
								FuncRtn |= FUNC_CODE19;
							}
							else
							{
								/* �������*/
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
							/* �������*/
						}

						CallFuncRtn = IoModule_SingleSideOutput_with_Ana((UINT8)CAB_B_TC2, (UINT8)IDLE_SIDE, pIoOut_InputInfo);
						if (FUNC_SUCESS != CallFuncRtn)
						{
							FuncRtn |= FUNC_CODE21;
						}
						else
						{
							/* �������*/
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
						/* �������*/
					}

					CallFuncRtn = IoModule_SingleSideOutput((UINT8)CAB_B_TC2, (UINT8)CTRL_SIDE);
					if (FUNC_SUCESS != CallFuncRtn)
					{
						FuncRtn |= FUNC_CODE23;
					}
					else
					{
						/* �������*/
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
			/*�޲���*/
		}
	}
	else
	{
		FuncRtn = FUNC_CODE24;
	}

	return FuncRtn;
}

/***************************************************************************************
* ��������:			�ֱ�����ATP/ATO��������ֵ
* �������:
* �����������:		��
* �������:			ATP���룬ATO����
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
* �޸ļ�¼��		added by Jielu 20230326
****************************************************************************************/
void IoModulCalMask_Api_PreiodProcess(OUT UINT32 *atpMask, OUT UINT32 *atoMask, IN IO_CFG_DATA_STRU *IOConfigFunc)
{
	UINT8 Index = 0U; /*�����е�·��*/
	UINT8 IoTableIndex = 0U;  /*ATO�ǰ�ȫ�����·��*/
							  /* ����IO���ñ���ÿһ·��IO�ĺ��� */
	UINT32 tmpAtpMask = 0xFFFFFFFFU; /*ATP ���룬��ʼ��Ϊȫ1*/
	UINT32 tmpAtoMask = 0x00000000U; /*ATO ���룬��ʼ��Ϊȫ1*/
	UINT8 boardNum = 0U;

	UINT8 bordIndex = 0U;

	if ((NULL != atpMask) && (NULL != atoMask) && (NULL != IOConfigFunc))
	{
		/*���Ұ忨����*/
		boardNum = IOConfigFunc->BoardNum;

		for (bordIndex = 0U; bordIndex<boardNum; bordIndex++)
		{
			if (TRAIN_OUTPUT_BOARD == IOConfigFunc->BoardIoMsgBuf[bordIndex].BoxAttr)
			{
				for (Index = 0U; Index < (UINT8)MAX_UNSAFE_CHANNELS_NUM; Index++)
				{
					/* ����IO����������ṹ���Ӧ�������� */
					for (IoTableIndex = 0U; IoTableIndex < OUTPUT_ATO_NUM; IoTableIndex++)
					{
						/*�����ȣ���ô��λֵATO������Ϊ1��ATP������Ϊ0*/
						if (IOConfigFunc->BoardIoMsgBuf[bordIndex].GfgIoBuf[Index] == gIoOutputAtoIndexTable[IoTableIndex].IoOutputTypeIndex)
						{
							tmpAtoMask |= (0x01U << Index);
							tmpAtpMask &= ~(0x01U << Index);
							break;
						}
						else
						{
							/*�޲���*/
						}
					}
				}
				break;
			}
			else
			{
				/*�޲���*/
			}
		}

		*atpMask = tmpAtpMask;
		*atoMask = tmpAtoMask;
	}
	else
	{
		/*�޲���*/
	}
}