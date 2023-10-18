/****************************************************************************************************
* �ļ���   :  IOModuleExtern.h
* ��Ȩ˵�� :  ���ؿƼ��ɷ����޹�˾
* �汾��   :  1.0
* ����ʱ�� :  2018.08.16
* ����     :
* �������� :  IOģ��ӿڣ������롢�����IO�������ṹ��
* ʹ��ע�� :
*
* �޸ļ�¼ :
*
**************************************************************************************************/
#ifndef IO_MODULE_EXTERN_H_
#define IO_MODULE_EXTERN_H_

#include "CommDefine.h"
#include "dquVobcConfigData.h"
#include "CommonTypes.h"
#include "App_ComStru.h"
#include "IOConfig.h"


/* �궨�� */
#define IO_NULL8_MSCP					((UINT8)0xFFU)
#define IO_TRUE_MSCP					((UINT8)0x55U)
#define IO_FALSE_MSCP					((UINT8)0xAAU)

#define INPUT_FLAG_NUM								(UINT8)82U		/*�ֶ�Table�а�ȫ�ͷǰ�ȫ������ܹ���Ŀ*/
#define INPUT_FLAG_NUM_SINGLE                       (UINT8)126U     /*���ڵ�ɼ�Table�е��ܹ�����*/
#define INPUT_DEFINE_MAX							(UINT16)0xAA	/*�����궨��IO���������ֵ*/

/*��ȫ���뿪��������--����������ƥ��*/
#define IN_TRAIN_CAB_ACTIVE_MSCP_NC					(UINT8)0x01		/*��ʻ�Ҽ����*/
#define IN_TRAIN_CAB_ACTIVE_MSCP_NO					(UINT8)0x02		/*��ʻ�Ҽ����*/
#define IN_TRAIN_EB_ENABLE_MSCP_NC					(UINT8)0x03		/*�г�������ʵʩ�����ƶ�����*/
#define IN_TRAIN_EB_ENABLE_MSCP_NO					(UINT8)0x04		/*�г�������ʵʩ�����ƶ�����*/
#define IN_TRAIN_TRACTION_CUT_MSCP_NC				(UINT8)0x05		/*ǣ����ж�س���*/
#define IN_TRAIN_TRACTION_CUT_MSCP_NO				(UINT8)0x06		/*ǣ����ж�س���*/
#define IN_TRAIN_DIR_FORWARD_MCSP_NC				(UINT8)0x07		/*�����ֱ���ǰ����*/
#define IN_TRAIN_DIR_FORWARD_MCSP_NO				(UINT8)0x08		/*�����ֱ���ǰ����*/
#define IN_TRAIN_DIR_BACKWARD_MSCP_NC				(UINT8)0x09		/*�����ֱ���󳣱�*/
#define IN_TRAIN_DIR_BACKWARD_MSCP_NO				(UINT8)0x0A		/*�����ֱ���󳣿�*/
#define IN_TRAIN_HANDLE_TRACTION_MSCP_NC			(UINT8)0x0B		/*ǣ���ֱ���ǣ��λ����*/
#define IN_TRAIN_HANDLE_TRACTION_MSCP_NO			(UINT8)0x0C		/*ǣ���ֱ���ǣ��λ����*/
#define IN_TRAIN_ATP_CONTROL_MSCP_NC				(UINT8)0x0D		/*�Զ��л���·����ATP�س�����*/
#define IN_TRAIN_ATP_CONTROL_MSCP_NO				(UINT8)0x0E		/*�Զ��л���·����ATP�س�����*/
#define IN_BCU_IN_ATP_MSCP_NC						(UINT8)0x0F		/*BCU������ATPģʽ����*/
#define IN_BCU_IN_ATP_MSCP_NO						(UINT8)0x10		/*BCU������ATPģʽ����*/
#define IN_TRAIN_AIR_FILL_MSCP_NC					(UINT8)0x11		/*�г����ڳ��״̬����*/
#define IN_TRAIN_AIR_FILL_MSCP_NO					(UINT8)0x12		/*�г����ڳ��״̬����*/
#define IN_LKJ_BRAKE_STATE_MSCP_NC					(UINT8)0x13		/*LKJ�����ƶ�ʵʩ״̬����*/
#define IN_LKJ_BRAKE_STATE_MSCP_NO					(UINT8)0x14		/*LKJ�����ƶ�ʵʩ״̬����*/
#define IN_BCU_IN_LKJ_MSCP_NC						(UINT8)0x15		/*BCU������LKJģʽ����*/
#define IN_BCU_IN_LKJ_MSCP_NO						(UINT8)0x16		/*BCU������LKJģʽ����*/
#define IN_TRAIN_OPEN_FSB_VALVE_MSCP_NC				(UINT8)0x17		/*VOBC�ѿ��������ƶ�������*/
#define IN_TRAIN_OPEN_FSB_VALVE_MSCP_NO				(UINT8)0x18		/*VOBC�ѿ��������ƶ�������*/
#define IN_TRAIN_ALLOW_ENTRY_ATO_MSCP_NC			(UINT8)0x19		/*�г��������ATO����*/
#define IN_TRAIN_ALLOW_ENTRY_ATO_MSCP_NO			(UINT8)0x1A		/*�г��������ATO����*/
#define IN_TRAIN_LKJ_CONTRO_MSCP_NC                 (UINT8)0x1B     /*�Զ��л���·����LKJ�س�����*/
#define IN_TRAIN_LKJ_CONTRO_MSCP_NO                 (UINT8)0x1C     /*�Զ��л���·����LKJ�س�����*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL1_MSCP_NC		(UINT8)0x1D		/*�����ƶ�1�����ƶ�������*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL1_MSCP_NO		(UINT8)0x1E		/*�����ƶ�1�����ƶ�������*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL2_MSCP_NC		(UINT8)0x1F		/*�����ƶ�2�����ƶ�������*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL2_MSCP_NO		(UINT8)0x20		/*�����ƶ�2�����ƶ�������*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL3_MSCP_NC		(UINT8)0x21		/*�����ƶ�3�����ƶ�������*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL3_MSCP_NO		(UINT8)0x22		/*�����ƶ�3�����ƶ�������*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL4_MSCP_NC		(UINT8)0x23		/*�����ƶ�4�����ƶ�������*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL4_MSCP_NO		(UINT8)0x24		/*�����ƶ�4�����ƶ�������*/
#define IN_TRAIN_CONFIRM_BUTTON_MSCP_NC				(UINT8)0x25     /*�г�ȷ�ϰ�ť״̬ (�����ź�)����*/
#define IN_TRAIN_CONFIRM_BUTTON_MSCP_NO             (UINT8)0x26     /*�г�ȷ�ϰ�ť״̬ (�����ź�)����*/
#define IN_ATP_TRAIN_INTEG_MSCP_NC					(UINT8)0x27		/*�г�����������*/
#define IN_ATP_TRAIN_INTEG_MSCP_NO					(UINT8)0x28		/*�г�����������*/
#define IN_ATP_BCZD_VALID_MSCP_NC					(UINT8)0x29		/*��ʵʩ�����ƶ�������*/
#define IN_ATP_BCZD_VALID_MSCP_NO					(UINT8)0x2A		/*��ʵʩ�����ƶ�������*/
#define IN_ATP_SB_RIGHT_MSCP_NC						(UINT8)0x2B		/*ǣ���ƶ��ֱ�����λ�ҷ����ֱ�����ǰλ������*/
#define IN_ATP_SB_RIGHT_MSCP_NO						(UINT8)0x2C		/*ǣ���ƶ��ֱ�����λ�ҷ����ֱ�����ǰλ������*/
#define IN_TRAIN_DOORCLOSE_MSCP_NC                  (UINT8)0x2D     /*���Źر�(ATP��ȫ����)����*/
#define IN_TRAIN_DOORCLOSE_MSCP_NO                  (UINT8)0x2E     /*���Źر�(ATP��ȫ����)����*/
#define IN_TRAIN_EMERDOORBOARD_MSCP_NC              (UINT8)0x2F     /*�����Ÿǰ�״̬�ල�������Ž��������ֱ�״̬,ATP��ȫ���룩����*/
#define IN_TRAIN_EMERDOORBOARD_MSCP_NO              (UINT8)0x30     /*�����Ÿǰ�״̬�ල�������Ž��������ֱ�״̬,ATP��ȫ���룩����*/
#define IN_TRAIN_KEY_OPEN_MSCP_NC					(UINT8)0x31		/*˾��Կ�״򿪳���*/
#define IN_TRAIN_KEY_OPEN_MSCP_NO					(UINT8)0x32		/*˾��Կ�״򿪳���*/
#define IN_ATP_DOOR_CLOSEDANDLOCK_MSCP_NC			(UINT8)0x33		/*���Źر�������,����*/
#define IN_ATP_DOOR_CLOSEDANDLOCK_MSCP_NO           (UINT8)0x34		/*���Źر�������,����*/
#define IN_TRAIN_RESECT_SWITCH_MSCP_NC				(UINT8)0x35     /*�г��������г�λ���� */    
#define IN_TRAIN_RESECT_SWITCH_MSCP_NO				(UINT8)0x36     /*�г��������г�λ ����*/  
#define IN_TRAIN_EBHANDLE_ACTIVE_MSCP_NC			(UINT8)0x37		/*���������ֱ������*/
#define IN_TRAIN_EBHANDLE_ACTIVE_MSCP_NO			(UINT8)0x38		/*���������ֱ������*/
#define IN_TRAIN_OBSTDET_INPUT_MSCP_NC				(UINT8)0x39		/*�ϰ����ѹ��ⳣ��*/
#define IN_TRAIN_OBSTDET_INPUT_MSCP_NO				(UINT8)0x3A		/*�ϰ����ѹ��ⳣ��*/
#define IN_DOUBLE1_DOORCLOSELOCK_MSCP_NC			(UINT8)0x3B		/*��ʿ�ᳵ�Źر������գ�˫1��Ч��������*/
#define IN_DOUBLE1_DOORCLOSELOCK_MSCP_NO			(UINT8)0x3C		/*��ʿ�ᳵ�Źر������գ�˫1��Ч��������*/
#define IN_RESERVE_0x3D_SINGLE						(UINT8)0x3D		/*Ԥ��*/
#define IN_RESERVE_0x3E_SINGLE						(UINT8)0x3E		/*Ԥ��*/
#define IN_RESERVE_0x3F_SINGLE						(UINT8)0x3F		/*Ԥ��*/
#define IN_RESERVE_0x40_SINGLE						(UINT8)0x40		/*Ԥ��*/
#define IN_TRAIN_BRAKE_FAULT_MSCP_NC				(UINT8)0x41		/*�ƶ��ع��ϳ���*/
#define IN_TRAIN_BRAKE_FAULT_MSCP_NO				(UINT8)0x42		/*�ƶ��ع��ϳ���*/
#define IN_ATP_TRACTION_RIGHT_MSCP_NC				(UINT8)0x43		/*ǣ���ƶ��ֱ���ǣ��λ�ҷ����ֱ���ǰ����*/
#define IN_ATP_TRACTION_RIGHT_MSCP_NO				(UINT8)0x44		/*ǣ���ƶ��ֱ���ǣ��λ�ҷ����ֱ���ǰ����*/
#define IN_EMERGENCYDOOR_SINGLE_MSCP_NC  			(UINT8)0x45 	/*������״̬����*/
#define IN_EMERGENCYDOOR_SINGLE_MSCP_NO  			(UINT8)0x46 	/*������״̬����*/

/*Debug todo ˷�ư�ȫ*/

#define IN_TRAIN_CONFIRM_BUTTON2_SINGLE             (UINT8)0x47     /* ȷ�ϰ�ť2�����ź�*/
/*�ǰ�ȫ���뿪��������--����������ƥ��*/
#define IN_ATP_ATO_START1_SINGLE  					(UINT8)0x48	    /*ATO������ť1�Ѱ���*/
#define IN_ATP_MODE_UP_SINGLE  						(UINT8)0x49		/*ģʽѡ���ϰ�ť����*/
#define IN_ATP_MODE_DOWN_SINGLE  					(UINT8)0x4A		/*ģʽѡ���°�ť����*/
#define IN_ATP_AR_SINGLE  							(UINT8)0x4B		/*AR��ť����*/
#define IN_ATP_RIGHT_DOOR_OPEN_SINGLE  				(UINT8)0x4C		/*���ſ��Ű�ť����*/
#define IN_ATP_RIGHT_DOOR_CLOSE_SINGLE  			(UINT8)0x4D		/*���Ź��Ű�ť����*/
#define IN_ATP_EUM_SWITCH_SINGLE  					(UINT8)0x4E		/*EUM���ؼ���*/
#define IN_ATP_LEFT_DOOR_OPEN_SINGLE  				(UINT8)0x4F		/*���ſ��Ű�ť����*/
#define IN_ATP_LEFT_DOOR_CLOSE_SINGLE  				(UINT8)0x50		/*���Ź��Ű�ť����*/
#define IN_ATP_ATO_START2_SINGLE  					(UINT8)0x51 	/*ATO������ť2�Ѱ���*/

#define IN_ATP_SLEEP_SINGLE  						(UINT8)0x52		/*����*/
#define IN_TRAIN_NOBODY_ALERT_SIGNAL_SINGLE  		(UINT8)0x53		/*���˾���������ź�*/
#define IN_TRAIN_WHISTLE_SIGNAL_SINGLE  			(UINT8)0x54		/*�����ź�*/
#define IN_ATP_TVS1_Check_SINGLE         			(UINT8)0x55		/*EMC��1�Ļ�����⣨�ǰ�ȫ��*/
#define IN_ATP_TVS2_Check_SINGLE         			(UINT8)0x56		/*EMC��2�Ļ�����⣨�ǰ�ȫ��*/
#define IN_CHANGE_SWITCH_LKJ_SINGLE                 (UINT8)0x57		/*�ֶ��л����ص�LKJλ*/
#define IN_CHANGE_SWITCH_AUTO_SINGLE                (UINT8)0x58		/*�ֶ��л����ص��Զ�λ*/
#define IN_TRAIN_FSB_1_SINGLE                       (UINT8)0x59		/*�г���ʵʩ�����ƶ�1*/
#define IN_TRAIN_FSB_2_SINGLE                       (UINT8)0x5A		/*�г���ʵʩ�����ƶ�2,�ó����ƶ�1�ͳ����ƶ�2��ϳ�80kp.100Kp,120Kpa*/
#define IN_TRAIN_FSB_120KPA_SINGLE                  (UINT8)0x5B		/*�г���ʵʩ�����ƶ�120KPa */
#define IN_LKJ_EB_ENABLE_SINGLE                     (UINT8)0x5C		/*LKJ�����ƶ�ʵʩ״̬*/
#define IN_TRAIN_CUT_SWITCH_SINGLE                  (UINT8)0x5D		/*�г��������г�λ��Ԥ����������510ʹ��0x4E��*/
#define IN_TRAIN_RELIEVE_SINGLE                     (UINT8)0x5E		/*VOBC�����г�����*/
#define IN_ATP_HANDLE_EB_SINGLE                     (UINT8)0x5F		/*ǣ���ƶ��ֱ��ڵ���λ*/
#define IN_RESERVE_0x60_SINGLE						(UINT8)0x60		/*Ԥ��*/
#define IN_RESERVE_0x61_SINGLE						(UINT8)0x61		/*Ԥ��*/
#define IN_RESERVE_0x62_SINGLE						(UINT8)0x62		/*Ԥ��*/
#define IN_RESERVE_0x63_SINGLE						(UINT8)0x63		/*Ԥ��*/
#define IN_RESERVE_0x64_SINGLE						(UINT8)0x64		/*Ԥ��*/
#define IN_RESERVE_0x65_SINGLE						(UINT8)0x65		/*Ԥ��*/
#define IN_RESERVE_0x66_SINGLE						(UINT8)0x66		/*Ԥ��*/
#define IN_RESERVE_0x67_SINGLE						(UINT8)0x67		/*Ԥ��*/
#define IN_RESERVE_0x68_SINGLE						(UINT8)0x68		/*Ԥ��*/

/***********************************Disney Unsafe Start***************************************************/
#define IN_IVOC_VALIDITY_SINGLE  					(UINT8)0x69		/*(Unsafe)IVOC������Ч*/
/***********************************Disney End***************************************************/

/**********************************�ɶ����������ǰ�ȫ���� Start***************************************/
#define IN_TRAIN_REDUNDANCY_SINGLE					(UINT8)0x6A		/*����״̬����(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_FAN1_SINGLE						(UINT8)0x6B		/*����1(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_FAN2_SINGLE						(UINT8)0x6C		/*����2(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_DOORLOCK_SINGLE					(UINT8)0x6D		/*��������(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_ESCAPDOOR_UNLOCK_SINGLE			(UINT8)0x6E		/*�����Ž��������ֱ�������λ(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_GUESTDOOR_UNLOCK_SINGLE			(UINT8)0x6F		/*�����Ž��������ֱ�������λ(ATP�ǰ�ȫ����)*/
/**********************************�ɶ����������ǰ�ȫ���� End***************************************/


/********************************�ɶ�ʵ�����������룬FAO���ܺ�ATO���� Start***************************/
#define IN_ATP_SMOKE_ALARM_SINGLE  					(UINT8)0x70		/*������*/
#define IN_ATP_DOOR_MM_SINGLE  						(UINT8)0x71		/*�˹����˹���*/
#define IN_ATP_DOOR_AM_SINGLE  						(UINT8)0x72		/*�Զ����˹���*/
#define IN_ATP_DOOR_AA_SINGLE  						(UINT8)0x73		/*�Զ����Զ���*/

/*�»������п���������*/
#define IN_EBI1_FEEDBACK_SINGLE  					(UINT8)0x74		/*�����̵���1����״̬*/
#define IN_EBI2_FEEDBACK_SINGLE  					(UINT8)0x75		/*�����̵���2����״̬*/
#define IN_TVS1CHECK_SINGLE  						(UINT8)0x76		/*����TVS1���*/
#define IN_TVS2CHECK_SINGLE  						(UINT8)0x77		/*����TVS2���*/
#define IN_DOORALLCLOSE_SINGLE  					(UINT8)0x78		/*���Źرհ�ť*/
/********************************�ɶ�ʵ�����������룬FAO���ܺ�ATO���� End***************************/

/***********************************Disney safe Start***************************************************/

#define IN_CM_MODE_SINGLE_MSCP_NC  					(UINT8)0x79		/*(Safe)CMģʽ����*/
#define IN_CM_MODE_SINGLE_MSCP_NO  					(UINT8)0x7A		/*(Safe)CMģʽ����*/
#define IN_NONSD_MODE_SINGLE_MSCP_NC  				(UINT8)0x7B		/*(Safe)NONSD ģʽ,����*/
#define IN_NONSD_MODE_SINGLE_MSCP_NO                (UINT8)0x7C		/*(Safe)NONSD ģʽ������*/
#define IN_FWD_MODE_SINGLE_MSCP_NC  				(UINT8)0x7D		/*(Safe)FWDģʽ,����*/
#define IN_FWD_MODE_SINGLE_MSCP_NO  				(UINT8)0x7E		/*(Safe)FWDģʽ,����*/
#define IN_REV_MODE_SINGLE_MSCP_NC  				(UINT8)0x7F		/*(Safe)REVģʽ,����*/
#define IN_REV_MODE_SINGLE_MSCP_NO  				(UINT8)0x80		/*(Safe)REVģʽ,����*/
#define IN_STBY_MODE_SINGLE_MSCP_NC  				(UINT8)0x81		/*(Safe)��STBYģʽ,����*/
#define IN_STBY_MODE_SINGLE_MSCP_NO  				(UINT8)0x82		/*(Safe)��STBYģʽ,����*/
#define IN_SD_MODE_SINGLE_MSCP_NC  					(UINT8)0x83		/*(Safe)��SDģʽ,����*/
#define IN_SD_MODE_SINGLE_MSCP_NO  					(UINT8)0x84		/*(Safe)��SDģʽ,����*/
#define IN_EB_FEEDBACK_SINGLE_MSCP_NC  				(UINT8)0x85		/*(Safe)�ź�ϵͳ�����ƶ���������ɼ�,����*/
#define IN_EB_FEEDBACK_SINGLE_MSCP_NO  				(UINT8)0x86		/*(Safe)�ź�ϵͳ�����ƶ���������ɼ�,����*/
#define IN_CONDUCT_ZERO_SPEED_SINGLE_MSCP_NC  		(UINT8)0x87		/*(Safe)�����ź���ʵʩ,����*/
#define IN_CONDUCT_ZERO_SPEED_SINGLE_MSCP_NO  		(UINT8)0x88		/*(Safe)�����ź���ʵʩ,����*/
#define IN_SAFE_INTERLOCK_FEEDBACK_SINGLE_MSCP_NC  	(UINT8)0x89		/*(Safe)��ȫ�����̵���״̬�ɼ�,����*/
#define IN_SAFE_INTERLOCK_FEEDBACK_SINGLE_MSCP_NO  	(UINT8)0x8A		/*(Safe)��ȫ�����̵���״̬�ɼ�������*/
#define IN_TRAIN_Reserved                           (UINT8)0xFF		/*Ԥ��*/

/***********************************Disney safe End***************************************************/


/*********************************IOģ�������Start*************************************************/

/*��ȫ�������������--����������ƥ��*/
#define OUT_EB										(UINT8)0x01		/*EB���*/
#define OUT_FORBID_EB_RELIEVE						(UINT8)0x02		/*��ֹ�ƶ��������*/
#define OUT_TRACTION_CUT							(UINT8)0x03		/*ATPǣ���г� */
#define OUT_BEGIN_FSB                               (UINT8)0x04		/*VOBC���������ƶ���*/
#define OUT_AUTO_SWITCH_SINGLE1                     (UINT8)0x05		/*�Զ��л���·�����ź�1*/
#define OUT_AUTO_SWITCH_SINGLE2                     (UINT8)0x06		/*�Զ��л���·�����ź�2*/
#define OUT_TRAIN_CONTROL_RIGHT						(UINT8)0x07		/*VOBC��ȡ��������Ȩ*/
#define OUT_RESERVE_0x08                            (UINT8)0x08		/*Ԥ��*/
#define OUT_ATP_CONTRAL_TRAIN                       (UINT8)0x09		/*�г����ڿس�״̬*/	
#define OUT_ELECTRIC_BRAKE_CHANL3					(UINT8)0x0A		/*�����ƶ�3�����ƶ���*/
#define OUT_ELECTRIC_BRAKE_CHANL4					(UINT8)0x0B		/*�����ƶ�4�����ƶ���*/
#define OUT_FAULT_EB								(UINT8)0x0C		/*ATP�����ƶ�*/
#define OUT_ATP_LEFTDOOR_EN							(UINT8)0x0D		/*����ʹ��*/
#define OUT_ATP_RIGHTDOOR_EN						(UINT8)0x0E		/*����ʹ��*/
#define OUT_ATP_AR_LAMP								(UINT8)0x0F		/*ARָʾ�����*/
#define OUT_RESERVE_0x10							(UINT8)0x10		/*Ԥ��*/

#define OUT_ATP_AR_RELAY							(UINT8)0x11		/*AR�̵����������*/
#define OUT_ATP_ZERO_SPEEDSIG						(UINT8)0x12		/*�����ź����*/
#define OUT_ATP_START_LIGHT							(UINT8)0x13		/*������ָʾ�����*/
#define OUT_ATP_WAKEUP								(UINT8)0x14		/* OutputValue��Bit10�������*/
#define OUT_ATP_SLEEP								(UINT8)0x15		/* OutputValue��Bit11�������*/
#define OUT_ATP_REVOKE								(UINT8)0x16		/*OutputValue��Bit12ATP�������*/
#define OUT_TRAIN_RELIEVE                     		(UINT8)0x17		/* VOBC�����г�����*/
#define OUT_TRAIN_LIGHT1                     		(UINT8)0x18		/*����ָʾ��1*/
#define OUT_TRAIN_LIGHT2                     		(UINT8)0x19		/*����ָʾ��2*/
#define OUT_RESERVE_0x1A							(UINT8)0x1A		/*Ԥ��*/
#define OUT_RESERVE_0x1B							(UINT8)0x1B		/*Ԥ��*/
#define OUT_RESERVE_0x1C							(UINT8)0x1C		/*Ԥ��*/
#define OUT_RESERVE_0x1D							(UINT8)0x1D		/*Ԥ��*/
#define OUT_RESERVE_0x1E							(UINT8)0x1E		/*Ԥ��*/
#define OUT_RESERVE_0x1F							(UINT8)0x1F		/*Ԥ��*/

/*�ǰ�ȫ�������������--����������ƥ��*/
#define OUT_PASS_NEUTRAL_FAULT						(UINT8)0x20		/*�Զ���������� */	
#define OUT_PASS_NEUTRAL_PREVIEW					(UINT8)0x21		/*�Զ�������Ԥ�� */	
#define OUT_PASS_NEUTRAL_FORCE						(UINT8)0x22		/*�Զ�������ǿ�� */	
#define OUT_PILOT_LAMP1                             (UINT8)0x23		/*Ԥ��ָʾ��1 */
#define OUT_PILOT_LAMP2                             (UINT8)0x24		/*Ԥ��ָʾ��2 */
#define OUT_VOBC_FSB_1                              (UINT8)0x25		/*VOBC�����ƶ�1*/
#define OUT_VOBC_FSB_2                              (UINT8)0x26		/*VOBC�����ƶ�2*/
#define OUT_VOBC_FSB_120KPA                         (UINT8)0x27		/*VOBC�����ƶ�120Kpa*/
#define OUT_ATO_ENABLE_1                            (UINT8)0x28		/*ATOʹ��1*/
#define OUT_ATO_ENABLE_2                            (UINT8)0x29		/*ATOʹ��2*/
#define OUT_AIR_BRAKE								(UINT8)0x2A		/*�����ƶ�(�ƶ����ϸ���)*/

/*�ɶ�ʵ�������������FAO���ܺ�ATO���*/
#define IN_TRAIN_OPPOSITE_RESTRAT					(UINT8)0x2B		/*VBTC�Զ�����ָ��*/
#define IN_TRAIN_CAB_ACTIVE_OUTPUT					(UINT8)0x2C		/*��ʻ�Ҽ������*/
#define ATP_DIRFORWARD_OUTPUT						(UINT8)0x2D		/*FAO����:������ǰָ��(�ǰ�ȫ) */
#define ATP_DIRBACKWARD_OUTPUT						(UINT8)0x2E		/*FAO����:�������ָ��(�ǰ�ȫ)*/
#define ATP_FAMMODE_OUTPUT							(UINT8)0x2F		/*FAO����:FAMģʽ���(�ǰ�ȫ) */
#define ATP_CAMMODE_OUTPUT							(UINT8)0x30		/*FAO����:CAMģʽ���(�ǰ�ȫ) */
#define ATP_JUMPDRDER_OUTPUT						(UINT8)0x31		/*FAO����:��Ծָ��(�ǰ�ȫ)*/
#define AOM_PARKING_BRAKING_IMPLEMENT_OUTPUT_ATP    (UINT8)0x32		/*AOMͣ���ƶ�ʩ�����*/
#define AOM_PARKING_BRAKING_FREE_OUTPUT_ATP         (UINT8)0x33		/*AOMͣ���ƶ��������*/

#define ATO_ADDROFFSET_OUT_ENABLE					(UINT8)0x34		/*ATO�Ѽ��� 1*/
#define ATO_ADDROFFSET_OUT_TRACTION					(UINT8)0x35		/*ǣ��״̬ 2*/
#define ATO_ADDROFFSET_OUT_BRAKE					(UINT8)0x36		/*�ƶ�״̬ 4*/
#define ATO_ADDROFFSET_OUT_HOLD_BRAKE				(UINT8)0x37		/*�����ƶ� 8*/
#define ATO_ADDROFFSET_OUT_LEFT_DOOR_OPEN			(UINT8)0x38		/*�������ſ������ 1*/
#define ATO_ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE		(UINT8)0x39		/*ȫ�г����Źر���� 2*/
#define ATO_ADDROFFSET_OUT_RIGHT_DOOR_OPEN			(UINT8)0x3A		/*�����ҳ��ſ������ 4*/
#define ATO_ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE		(UINT8)0x3B		/*ȫ�г��ҳ��Źر���� 8*/
#define ATO_ADDROFFSET_START_LIGHT      			(UINT8)0x3C		/*ATO������*/
#define ATO_ADDROFFSET_BAG_LEFTDOOR_OPEN           	(UINT8)0x3D		/*���������ʹ��*/
#define ATO_ADDROFFSET_BAG_RIGHTDOOR_OPEN           (UINT8)0x3E		/*���������ʹ��*/
#define OUT_RESERVE_0x3F							(UINT8)0x3F		/*Ԥ��*/
#define OUT_RESERVE_0x40							(UINT8)0x40		/*Ԥ��*/
#define OUT_RESERVE_0x41							(UINT8)0x41		/*Ԥ��*/
/*�ɶ���������*/
#define OUT_TRAIN_EMERDOOR                          (UINT8)0x42		/*�����Ž���������Ȩ(�����Ž������,ATP��ȫ���)*/
#define OUT_TRAIN_REDUNDANCY                        (UINT8)0x43		/*����״̬���(ATP�ǰ�ȫ���)*/
#define OUT_TRAIN_REMOTE_RELIFE_BOGIE               (UINT8)0x44		/*Զ�̻���ת���(ATP�ǰ�ȫ���)*/
#define OUT_TRAIN_DOOR_UNLOCK_LEFT                  (UINT8)0x45		/*�������Ž����������(ATP��ȫ���)*/
#define OUT_TRAIN_DOOR_UNLOCK_RIGHT                 (UINT8)0x46		/*�Ҳ�����Ž����������(ATP��ȫ���)*/
#define OUT_TRAIN_EMCY_DOOR_UNLOCK_LEFT             (UINT8)0x47		/*��������+�����Ž����������(ATP��ȫ���)*/
#define OUT_TRAIN_EMCY_DOOR_UNLOCK_RIGHT            (UINT8)0x48		/*�Ҳ������+�����Ž����������(ATP��ȫ���)*/
#define OUT_TRAIN_FORCED_RELEASE                    (UINT8)0x49		/*ǿ�Ȼ���ָ��(ATP�ǰ�ȫ���)*/
#define OUT_TRAIN_FAM_START_LIGHT                   (UINT8)0x4A		/*FAM����ָʾ��(ATP�ǰ�ȫ���)*/
#define OUT_TRAIN_ALL_DOOR_UNLOCK                   (UINT8)0x4B		/*�����źͿ����Ž�������*/
#define OUT_NOT_ALLOW_REBOOT                        (UINT8)0x4C     /*ATP�����������������*/
#define OUT_ALLOW_REBOOT_ATO                        (UINT8)0x4D     /*ATP�������ATO*/

/*********************************IOģ�������End*************************************************/


/*********************************���¶���ΪIOģ���Ӧ�õĶ��壬�м�Ӧ����Ҫ�ĳ������������������************************************************/
/*��ȫ���뿪��������*/
#define IN_TRAIN_CAB_ACTIVE							(UINT8)0x01		/*��ʻ�Ҽ���*/
#define IN_TRAIN_EB_ENABLE							(UINT8)0x02		/*�г�������ʵʩ�����ƶ�*/
#define IN_TRAIN_TRACTION_CUT						(UINT8)0x03		/*ǣ����ж��*/
#define IN_TRAIN_DIR_FORWARD						(UINT8)0x04		/*�����ֱ���ǰ*/
#define IN_TRAIN_DIR_BACKWARD						(UINT8)0x05		/*�����ֱ����*/
#define IN_TRAIN_HANDLE_TRACTION					(UINT8)0x06		/*ǣ���ֱ���ǣ��λ*/
#define IN_TRAIN_ATP_CONTROL						(UINT8)0x07		/*�Զ��л���·����ATP�س�*/
#define IN_BCU_IN_ATP							    (UINT8)0x08		/*BCU������ATPģʽ*/
#define IN_TRAIN_AIR_FILL							(UINT8)0x09		/*�г����ڳ��״̬*/
#define IN_LKJ_BRAKE_STATE							(UINT8)0x0A		/*LKJ�����ƶ�ʵʩ״̬*/
#define IN_BCU_IN_LKJ								(UINT8)0x0B		/*BCU������LKJģʽ*/
#define IN_TRAIN_OPEN_FSB_VALVE					    (UINT8)0x0C		/*VOBC�ѿ��������ƶ���*/
#define IN_TRAIN_ALLOW_ENTRY_ATO					(UINT8)0x0D		/*�г��������ATO*/
#define IN_TRAIN_LKJ_CONTROL	                    (UINT8)0x0E		/*�Զ��л���·����LKJ�س�*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL1				(UINT8)0x0F		/*�����ƶ�1�����ƶ���*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL2				(UINT8)0x10		/*�����ƶ�2�����ƶ���*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL3				(UINT8)0x11		/*�����ƶ�3�����ƶ���*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL4				(UINT8)0x12		/*�����ƶ�4�����ƶ���*/
#define IN_TRAIN_CONFIRM_BUTTON						(UINT8)0x13		/*�г�ȷ�ϰ�ť״̬ (�����ź�)*/
#define IN_TRAIN_RESECT_SWITCH						(UINT8)0x14		/*�г��������г�λ */    
#define IN_TRAIN_CONFIRM_BUTTON2	                (UINT8)0x70		/* ȷ�ϰ�ť2�����ź�*/
/*�ǰ�ȫ���뿪��������--����������ƥ��*/
#define IN_ATP_ATO_START1							(UINT8)0x15		/*ATO������ť1�Ѱ���*/
#define IN_ATP_MODE_UP								(UINT8)0x16		/*ģʽѡ���ϰ�ť����*/
#define IN_ATP_MODE_DOWN							(UINT8)0x17		/*ģʽѡ���°�ť����*/
#define IN_ATP_AR									(UINT8)0x18		/*AR��ť����*/
#define IN_ATP_RIGHT_DOOR_OPEN						(UINT8)0x19		/*���ſ��Ű�ť����*/
#define IN_ATP_RIGHT_DOOR_CLOSE						(UINT8)0x1A		/*���Ź��Ű�ť����*/
#define IN_ATP_EUM_SWITCH							(UINT8)0x1B		/*EUM���ؼ���*/
#define IN_ATP_LEFT_DOOR_OPEN						(UINT8)0x1C		/*���ſ��Ű�ť����*/
#define IN_ATP_LEFT_DOOR_CLOSE						(UINT8)0x1D		/*���Ź��Ű�ť����*/
#define IN_ATP_ATO_START2							(UINT8)0x1E		/*ATO������ť2(�ǰ�ȫ),�������2����*/
#define IN_ATP_TRAIN_INTEG							(UINT8)0x1F		/*�г�����*/
#define IN_ATP_DOOR_CLOSEDANDLOCK					(UINT8)0x20		/*���Źر�������*/
#define IN_ATP_BCZD_VALID							(UINT8)0x21		/*��ʵʩ�����ƶ�*/
#define IN_ATP_SB_RIGHT								(UINT8)0x22		/*ǣ���ƶ��ֱ�����λ�ҷ����ֱ�����ǰλ*/
#define IN_ATP_SLEEP								(UINT8)0x23		/*����*/
#define IN_TRAIN_NOBODY_ALERT_SIGNAL				(UINT8)0x24		/*���˾���������ź�*/
#define IN_TRAIN_WHISTLE_SIGNAL						(UINT8)0x25		/*�����ź�*/
#define IN_ATP_TVS1_Check	       					(UINT8)0x26		/*EMC��1�Ļ�����⣨�ǰ�ȫ��*/
#define IN_ATP_TVS2_Check	       					(UINT8)0x27		/*EMC��2�Ļ�����⣨�ǰ�ȫ��*/
#define IN_CHANGE_SWITCH_LKJ	                    (UINT8)0x28		/* �ֶ��л����ص�LKJλ*/
#define IN_CHANGE_SWITCH_AUTO	                    (UINT8)0x29		/* �ֶ��л����ص��Զ�λ*/
#define IN_TRAIN_FSB_1	                            (UINT8)0x2A		/* �г���ʵʩ�����ƶ�1 */
#define IN_TRAIN_FSB_2	                            (UINT8)0x2B		/* �г���ʵʩ�����ƶ�2,�ó����ƶ�1�ͳ����ƶ�2��ϳ�80kp.100Kp,120Kpa*/
#define IN_TRAIN_FSB_120KPA	                        (UINT8)0x2C		/* �г���ʵʩ�����ƶ�120KPa */
#define IN_LKJ_EB_ENABLE	                        (UINT8)0x2D		/*LKJ�����ƶ�ʵʩ״̬*/
#define IN_TRAIN_CUT_SWITCH	                        (UINT8)0x2E		/* �г��������г�λ*/
#define IN_TRAIN_RELIEVE	                     	(UINT8)0x2F		/* VOBC�����г�����*/
#define IN_ATP_HANDLE_EB	                     	(UINT8)0x30		/* ǣ���ƶ��ֱ��ڵ���λ*/

/***********************************Disney Start***************************************************/
#define IN_CM_MODE									(UINT8)0x31		/*(Safe)CMģʽ*/
#define IN_NONSD_MODE								(UINT8)0x32		/*(Safe)NONSD ģʽ*/
#define IN_FWD_MODE									(UINT8)0x33		/*(Safe)FWDģʽ*/
#define IN_REV_MODE									(UINT8)0x34		/*(Safe)REVģʽ*/  
#define IN_STBY_MODE								(UINT8)0x35		/*(Safe)��STBYģʽ*/
#define IN_SD_MODE									(UINT8)0x36		/*(Safe)��SDģʽ*/
#define IN_EB_FEEDBACK								(UINT8)0x37		/*(Safe)�ź�ϵͳ�����ƶ���������ɼ�*/
#define IN_CONDUCT_ZERO_SPEED						(UINT8)0x38		/*(Safe)�����ź���ʵʩ*/
#define IN_SAFE_INTERLOCK_FEEDBACK					(UINT8)0x39		/*(Safe)��ȫ�����̵���״̬�ɼ�*/
#define IN_IVOC_VALIDITY							(UINT8)0x3A		/*(Unsafe)IVOC������Ч*/
/***********************************Disney End***************************************************/


/*�ɶ�ʵ�����������룬FAO���ܺ�ATO����*/
#define IN_TRAIN_KEY_OPEN							(UINT8)0x97		/*˾��Կ�״�*/
#define IN_TRAIN_EBHANDLE_ACTIVE					(UINT8)0x71		/*���������ֱ�����*/
#define IN_TRAIN_OBSTDET_INPUT						(UINT8)0x72		/*�ϰ����ѹ���*/
#define IN_TRAIN_BRAKE_FAULT						(UINT8)0x73		/*�ƶ��ع���*/
#define IN_ATP_TRACTION_RIGHT						(UINT8)0x74		/*ǣ���ƶ��ֱ���ǣ��λ�ҷ����ֱ���ǰ*/
#define IN_ATP_SMOKE_ALARM							(UINT8)0x75		/*������*/
#define IN_ATP_DOOR_MM								(UINT8)0x76		/*�˹����˹���*/
#define IN_ATP_DOOR_AM								(UINT8)0x78		/*�Զ����˹���*/
#define IN_ATP_DOOR_AA								(UINT8)0x77		/*�Զ����Զ���*/

/*�»������п���������*/
#define IN_EBI1_FEEDBACK							(UINT8)0x79		/*�����̵���1����״̬*/
#define IN_EBI2_FEEDBACK							(UINT8)0x7A		/*�����̵���2����״̬*/
#define IN_EMERGENCYDOOR							(UINT8)0x7B		/*������״̬(ATP��ȫ����)*/
#define IN_TVS1CHECK								(UINT8)0x7C		/*����TVS1���*/
#define IN_TVS2CHECK								(UINT8)0x7D		/*����TVS2���*/
#define IN_DOORALLCLOSE								(UINT8)0x7E		/*���Źرհ�ť*/

/*�ɶ���������*/
#define IN_TRAIN_DOORCLOSE							(UINT8)0x98		/*���Źر�(ATP��ȫ����)*/
#define IN_TRAIN_EMERDOORBOARD						(UINT8)0x99		/*�����Ÿǰ�״̬�ල�������Ž��������ֱ�״̬,ATP��ȫ���룩*/
#define IN_TRAIN_REDUNDANCY							(UINT8)0x9A		/*����״̬����(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_FAN1								(UINT8)0x9B		/*����1(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_FAN2								(UINT8)0x9C		/*����2(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_DOORLOCK							(UINT8)0x9D		/*��������(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_ESCAPDOOR_UNLOCK					(UINT8)0x9E		/*�����Ž��������ֱ�������λ(ATP�ǰ�ȫ����)*/
#define IN_TRAIN_GUESTDOOR_UNLOCK					(UINT8)0x9F		/*�����Ž��������ֱ�������λ(ATP�ǰ�ȫ����)*/

typedef struct
{
	/*------��ȫ�����������-------*/
	UINT8 InTrainCabActive;						/*��ʻ�Ҽ���*/
	UINT8 InTrainEbEnable;						/*�г�������ʵʩ�����ƶ�*/
	UINT8 InTrainTractionCut;					/*ǣ����ж��*/
	UINT8 InTrainDirForward;					/*�����ֱ���ǰ*/
	UINT8 InTrainDirBackward;					/*�����ֱ����*/
	UINT8 InTrainHandleTraction;				/*ǣ���ֱ���ǣ��λ*/
	UINT8 InTrainAtpControl;					/*�Զ��л���·����ATP�س�*/
	UINT8 InTrainBcuInAtp;						/*BCU������ATPģʽ*/
	UINT8 InTrainAirFill;						/*�г����ڳ��״̬*/
	UINT8 InLkjBrakeState;						/*LKJ�����ƶ�ʵʩ״̬*/
	UINT8 InTrainBcuInLkj;						/*BCU������LKJģʽ*/
	UINT8 InTrainOpenFsbValve;					/*VOBC�ѿ��������ƶ���*/
	UINT8 InTrainAllowEntryAto;					/*�г��������ATO*/
	UINT8 InTrainLkjControl;					/* �Զ��л���·����LKJ�س�*/
	UINT8 InTrainElectricBrakeChanl1;			/*�����ƶ�1�����ƶ���*/
	UINT8 InTrainElectricBrakeChanl2;			/*�����ƶ�2�����ƶ���*/
	UINT8 InTrainElectricBrakeChanl3;			/*�����ƶ�3�����ƶ���*/
	UINT8 InTrainElectricBrakeChanl4;			/*�����ƶ�4�����ƶ���*/
	UINT8 InTrainConfirmButton;					/*�г�ȷ�ϰ�ť״̬ */
	UINT8 InTrainResectSwitch;					/*�г��������г�λ */
	UINT8 InAtpModeUp;							/*ģʽѡ���ϰ�ť����*/
	UINT8 InAtpModeDown;						/*ģʽѡ���°�ť����*/
	UINT8 InAtpArInput;							/*AR��ť����*/
	UINT8 InAtpRightDoorOpen;					/*���ſ��Ű�ť����*/
	UINT8 InAtpRightDoorClose;					/*���Ź��Ű�ť����*/
	UINT8 InAtpEumSwitch;						/*EUM���ؼ���*/
	UINT8 InAtpLeftDoorOpen;					/*���ſ��Ű�ť����*/
	UINT8 InAtpLeftDoorClose;					/*���Ź��Ű�ť����*/
	UINT8 InAtpTrainInteg;						/*�г�����*/
	UINT8 InTrainDoorCloseAndLock;				/*���Źر�������*/
	UINT8 InAtpBczdValid;						/*��ʵʩ�����ƶ�*/
	UINT8 InAtpSbRightInput;					/*ǣ���ƶ��ֱ�����λ�ҷ����ֱ�����ǰλ*/
	UINT8 InAtpSleep;							/*����*/
	UINT8 InAtpConfirmButton2;					/* ȷ�ϰ�ť2��״̬*/
	UINT8 InTrainDoorClose;						/*���Źر�*/
	UINT8 InTrainEmergencyDoorBoard;			/*�����Ÿǰ�״̬�ල�������Ž��������ֱ�״̬��*/

	/*------�ǰ�ȫ�����������-------*/
	UINT8 InTrainNobodyAlertSignal;				/*���˾���������ź�*/
	UINT8 InTrainWhistleSignal;					/*�����ź�*/
	UINT8 InAtpAtoStart1;						/*ATO������ť1�Ѱ���*/
	UINT8 InAtpAtoStart2;						/*ATO������ť2�Ѱ���(�ǰ�ȫ)*/
	UINT8 InAtpTvs1Check;						/*EMC��1�Ļ�����⣨�ǰ�ȫ��*/
	UINT8 InAtpTvs2Check;						/*EMC��2�Ļ�����⣨�ǰ�ȫ��*/
	UINT8 InChangeSwitchLkj;					/* �ֶ��л����ص�LKJλ*/
	UINT8 InChangeSwitchAuto;					/* �ֶ��л����ص��Զ�λ*/
	UINT8 InTrainFsb1;							/* �г���ʵʩ�����ƶ�1 */
	UINT8 InTrainFsb2;							/* �г���ʵʩ�����ƶ�2 */
	UINT8 InTrainFsb120Kpa;						/* �г���ʵʩ�����ƶ�120KPa */
	UINT8 InLkjEbEnable;						/*LKJ�����ƶ�ʵʩ״̬*/
	UINT8 InTrainCutSwitch;						/* �г��������г�λ��Ԥ����������510ʹ��InAtpEumSwitch��*/
	UINT8 InTrainRelieve;						/* VOBC�����г�����*/
	UINT8 InHandleEB;							/* ǣ���ƶ��ֱ��ڵ���λ*/

	/*------��ʿ�ᰲȫ&�ǰ�ȫ�����������-------*/
	UINT8 InAtpCMMode;							/*(Safe)CMģʽ*/
	UINT8 InAtpNotSDMode;						/*(Safe)NOT SD ģʽ*/
	UINT8 InAtpFORMode;							/*(Safe)FORģʽ*/
	UINT8 InAtpREVMode;							/*(Safe)REVģʽ*/
	UINT8 InAtpSBMode;							/*(Safe)��SBģʽ*/
	UINT8 InAtpSDMode;							/*(Safe)��SDģʽ*/
	UINT8 InAtpEBFeedback;						/*(Safe)�ź�ϵͳ�����ƶ���������ɼ�*/
	UINT8 InAtpConductZeroSpeed;				/*(Safe)�����ź���ʵʩ*/
	UINT8 InAtpSafeInterLockFeedback;			/*(Safe)��ȫ�����̵���״̬�ɼ�*/
	UINT8 InAtpIVOCValidity;					/*(Unsafe)IVOC������Ч*/

	/*����������,H-FAO����*/
	UINT8 TRAIN_KEY_OPEN;						/*˾��Կ�׼���*/
	UINT8 TRAIN_EBHANDLE_ACTIVE;				/*�����ֱ�����*/
	UINT8 TRAIN_OBSTDET_INPUT;					/*�ϰ����ѹ���Ч*/
	UINT8 TRAIN_BRAKE_FAULT;					/*�ƶ��ع���*/
	UINT8 ATP_TRACTION_RIGHT;					/*ǣ���ֱ���ǣ��λ�ҷ����ֱ���ǰλ*/
	UINT8 ATP_SMOKE_ALARM;						/*�̻𱨾�*/
	UINT8 ATP_DOOR_MM;							/*�˹����˹���*/
	UINT8 ATP_DOOR_AM;							/*�Զ����˹���*/
	UINT8 ATP_DOOR_AA;							/*�Զ����Զ���*/
	/*�»���*/
	UINT8 EBI1_FEEDBACK;						/*�����̵���1����״̬*/
	UINT8 EBI2_FEEDBACK;						/*�����̵���2����״̬*/
	UINT8 EMERGENCYDOOR;						/*������״̬(ATP��ȫ����)*/
	UINT8 TVS1CHECK;							/*����TVS1���*/
	UINT8 TVS2CHECK;							/*����TVS2���*/
	UINT8 DOORALLCLOSE;							/*���Źرհ�ť*/

	/*�ɶ����߿���������*/
	UINT8 InTrainRedundancy;					/*����״̬����*/
	UINT8 InTrainFan1;							/*����1*/
	UINT8 InTrainFan2;							/*����2*/
	UINT8 InTrainDoorLock;						/*��������*/
	UINT8 InTrainEscapDoorUnlock;				/*�����Ž��������ֱ�������λ*/
	UINT8 InTrainGuestDoorUnlock;				/*�����Ž��������ֱ�������λ*/
}IO_INPUT_STRUCT;

typedef struct
{
	/*------��ȫ�����������-------*/
	UINT8 OutEb;								/*EB���*/
	UINT8 OutForbidEbRelieve;					/*��ֹ�ƶ��������*/
	UINT8 OutTractionCut;						/*ATPǣ���г� */
	UINT8 OutBeginFsb;							/*VOBC���������ƶ���*/
	UINT8 OutAutoSwitchSingle1;					/*�Զ��л���·�����ź�1*/
	UINT8 OutAutoSwitchSingle2;					/*�Զ��л���·�����ź�2*/
	UINT8 OutTrainControlRight;					/*VOBC��ȡ��������Ȩ*/
	UINT8 OutAtpContralTrain;					/* �г����ڿس�״̬*/
	UINT8 OutElectricBrakeChanl3;				/*�����ƶ�3�����ƶ���*/
	UINT8 OutElectricBrakeChanl4;				/*�����ƶ�4�����ƶ���*/
	UINT8 OutFaultEb;							/*ATP�����ƶ�*/
	UINT8 OutAtpLeftDoorEn;						/*����ʹ��*/
	UINT8 OutAtpRightDoorEn;					/*����ʹ��*/
	UINT8 OutAtpArLamp;							/*ARָʾ�����*/

	UINT8 OutAtpArRelay;						/*AR�̵����������*/
	UINT8 OutAtpZeroSpeedsig;					/*�����ź����*/
	UINT8 OutAtpWakeup;							/* OutputValue��Bit10�������*/
	UINT8 OutAtpSleep;							/* OutputValue��Bit11�������*/
	UINT8 OutAtpRevoke;							/*OutputValue��Bit12ATP�������*/
	UINT8 OutTrainRelieve;						/* VOBC�����г�����*/
	UINT8 ATPSafeLigth1;						/*��ȫָʾ��1*/
	UINT8 ATPSafeLigth2;						/*��ȫָʾ��2*/
												/*�ɶ���������*/
	UINT8 EmcyDoorUnlock;						/*�����Ž���������Ȩ(�����Ž������)*/
	UINT8 OutDoorUnlockLeft;					/*�������Ž����������*/
	UINT8 OutDoorUnlockRight;					/*�Ҳ�����Ž����������*/
	UINT8 OutEmcyUnlockLeft;					/*��������+�����Ž����������*/
	UINT8 OutEmcyUnlockRight;					/*�Ҳ������+�����Ž����������*/

	/*------�ǰ�ȫ�����������-------*/
	UINT8 OutPassNeutralFault;					/*�Զ���������� */
	UINT8 OutPassNeutralPreview;				/* �Զ�������Ԥ�� */
	UINT8 OutPassNeutralForce;					/*�Զ�������ǿ�� */
	UINT8 OutPilotLamp1;						/*Ԥ��ָʾ��1 */
	UINT8 OutPilotLamp2;						/*Ԥ��ָʾ��2 */
	UINT8 OutVobcFsb1;							/*VOBC�����ƶ�1*/
	UINT8 OutVobcFsb2;							/*VOBC�����ƶ�2*/
	/*UINT8 OutVobcFsb120Kpa; VOBC�����ƶ�120Kpa*/
	UINT8 OutForbidTrainAirFill;				/*VOBC��ֹ�г����*/

	UINT8 OutStartLight;						/*������ָʾ�����*/
	UINT8 TRAIN_OPPOSITE_RESTRAT;				/*�Զ���������*/
	UINT8 TRAIN_CAB_ACTIVE_OUTPUT;				/*AR�̵����������(��ʻ�Ҽ������)*/
	UINT8 DIRFORWARD_OUTPUT;					/*������ǰ�ź�*/
	UINT8 DIRBACKWARD_OUTPUT;					/*��������ź�*/
	UINT8 FAMMODE_OUTPUT;						/*FAMģʽ���*/
	UINT8 CAMMODE_OUTPUT;						/*CAMģʽ���*/
	UINT8 JUMPDRDER_OUTPUT;						/*��Ծָ��*/
	UINT8 PARKING_BRAKING_IMPLEMENT_OUTPUT;		/*ͣ���ƶ����*/
	UINT8 PARKING_BRAKING_FREE_OUTPUT;			/*ͣ���ƶ�����*/

	UINT8 ADDROFFSET_OUT_ENABLE;				/*ATO�Ѽ���*/
	UINT8 ADDROFFSET_OUT_TRACTION;				/*ǣ��״̬*/
	UINT8 ADDROFFSET_OUT_BRAKE;					/*�ƶ�״̬*/
	UINT8 ADDROFFSET_OUT_HOLD_BRAKE;			/*�����ƶ�*/
	UINT8 ADDROFFSET_OUT_LEFT_DOOR_OPEN;		/*�������ſ������*/
	UINT8 ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE;	/*ȫ�г����Źر����*/
	UINT8 ADDROFFSET_OUT_RIGHT_DOOR_OPEN;		/*�����ҳ��ſ������*/
	UINT8 ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE;	/*ȫ�г��ҳ��Źر����*/
	UINT8 ADDROFFSET_START_LIGHT;				/*ATO������*/
	UINT8 ADDROFFSET_BAG_LEFTDOOR_OPEN;			/*���������ʹ��*/
	UINT8 ADDROFFSET_BAG_RIGHTDOOR_OPEN;		/*���������ʹ��*/
	UINT8 ATO_ENABLE_1;							/*ATOʹ��1*/
	UINT8 ATO_ENABLE_2;							/*ATOʹ��2*/
	UINT8 AIR_BRAKE;							/*�����ƶ��������*/

	UINT8 OutTrainFAMStartLamp;					/*FAM����ָʾ��*/
	UINT8 OutEmergencyDoorUnlock;				/*�����źͿ����Ž���*/
	UINT8 Redundancy;							/*����״̬���*/
	UINT8 OutRemoteRelifeBogie;					/*Զ�̻���ת���*/
	UINT8 OutForcedReleaseCmd;					/*ǿ�Ȼ���ָ��*/

	UINT8 OutNotAllowReboot;                 /*��ֹ��������*/
	UINT8 OutRebootAto;                      /*ATO��������*/
}IO_OUTPUT_STRUCT;

/*IO����ģ��������Ϣ�ṹ��*/
typedef struct
{
	UINT8 MainCtrSide;				/*���ƶ�*/
	UINT8 IdleSide;					/*�ȴ���*/
	UINT8 Tc1DigtalInputState[INPUT_DEFINE_MAX];	/*TC1�˿�����,������Ϊ���е�������*/
	UINT8 Tc2DigtalInputState[INPUT_DEFINE_MAX];	/*TC2�˿�����*/
	UINT8 CtrlDigtalInputState[INPUT_DEFINE_MAX];	/*���ƶ˿�����*/
	UINT8 IdleDigtalInputState[INPUT_DEFINE_MAX];	/*�ȴ��˿�����*/
}IO_IN_PERIOD_OUTPUT_STRUCT;

/*IO���ģ���������Ϣ�ṹ��*/
typedef struct
{
	UINT32 CycCount;  							/*���ں�*/
	UINT32 OutUnSafeDataMask;					/*�ǰ�ȫ������Чλ����*/
	UINT32 OutSafeDataMask;						/*��ȫ��������Чλ����*/
	UINT8 dwAnaValueValid;						/*�����ģ����������Ч����*/
	UINT8 dwPwmValueValid;						/*PWM������Ч����*/
	UINT8 AtoCmdEnable;							/*���ATO��Ȩ��־*/
	UINT8 MainCtrlSide;							/*��ʼ��Ϊ�����ض�*/
	UINT8 IdleSide;								/*��ʼ��Ϊ�����ض�*/
	UINT8 LocalVobcAttri;						/*�����Ƿ�Ϊ���ƶ�*/
	UINT8 WakeCtrlIVOC;							/*������������*/
	IO_OUTPUT_STRUCT IOOutputStruCtrl;			/*���ƶ�IO��Ϣ*/
	IO_OUTPUT_STRUCT IOOutputStruIdle;			/*�ȴ���IO��Ϣ*/
	UINT16 wCtrlSideSwValue;					/*���ƶ˵Ŀ��������ǿ��ƶ�����̶���Чֵ*/
	UINT32 dwCtrlSideAnaValue;					/*���ƶ˵�ģ�������,�ǿ��ƶ�����̶���Чֵ*/
	UINT32 dwCtrlSidePwmValue;					/*���ƶ˵�pwm���,�ǿ��ƶ�����̶���Чֵ*/
	UINT8 FAMReverseTag;						/*FAM�۷���־*/
	UINT8 ARReverseTag;							/*AR�۷���־*/
	UINT8 ReverseTag;							/*�۷���������*/
}IO_OUT_PERIOD_INPUT_STRUCT;

/*���SID���ڵ�ʱ���г������սڵ�ṹ��*/
typedef struct
{
	/*----------------��ȫ�����������---------------------*/
	/*------��ȫ�����������-------*/
	UINT8 InTrainCabActiveMscpNc;				/*��ʻ�Ҽ����*/
	UINT8 InTrainCabActiveMscpNo;				/*��ʻ�Ҽ����*/
	UINT8 InTrainEbEnableMscpNc;				/*�г�������ʵʩ�����ƶ�����*/
	UINT8 InTrainEbEnableMscpNo;				/*�г�������ʵʩ�����ƶ�����*/
	UINT8 InTrainTractionCutMscpNc;				/*ǣ����ж�س���*/
	UINT8 InTrainTractionCutMscpNo;				/*ǣ����ж�س���*/
	UINT8 InTrainDirForwardMscpNc;				/*�����ֱ���ǰ����*/
	UINT8 InTrainDirForwardMscpNo;				/*�����ֱ���ǰ����*/
	UINT8 InTrainDirBackwardMscpNc;				/*�����ֱ���󳣱�*/
	UINT8 InTrainDirBackwardMscpNo;				/*�����ֱ���󳣿�*/
	UINT8 InTrainHandleTractionMscpNc;			/*ǣ���ֱ���ǣ��λ����*/
	UINT8 InTrainHandleTractionMscpNo;			/*ǣ���ֱ���ǣ��λ����*/
	UINT8 InTrainAtpControlMscpNc;				/*�Զ��л���·����ATP�س�����*/
	UINT8 InTrainAtpControlMscpNo;				/*�Զ��л���·����ATP�س�����*/
	UINT8 InTrainBcuInAtpMscpNc;				/*BCU������ATPģʽ����*/
	UINT8 InTrainBcuInAtpMscpNo;				/*BCU������ATPģʽ����*/
	UINT8 InTrainAirFillMscpNc;					/*�г����ڳ��״̬����*/
	UINT8 InTrainAirFillMscpNo;					/*�г����ڳ��״̬����*/
	UINT8 InLkjBrakeStateMscpNc;				/*LKJ�����ƶ�ʵʩ״̬����*/
	UINT8 InLkjBrakeStateMscpNo;				/*LKJ�����ƶ�ʵʩ״̬����*/
	UINT8 InTrainBcuInLkjMscpNc;				/*BCU������LKJģʽ����*/
	UINT8 InTrainBcuInLkjMscpNo;				/*BCU������LKJģʽ����*/
	UINT8 InTrainOpenFsbValveMscpNc;			/*VOBC�ѿ��������ƶ�������*/
	UINT8 InTrainOpenFsbValveMscpNo;			/*VOBC�ѿ��������ƶ�������*/
	UINT8 InTrainAllowEntryAtoMscpNc;			/*�г��������ATO����*/
	UINT8 InTrainAllowEntryAtoMscpNo;			/*�г��������ATO����*/
	UINT8 InTrainLkjControlMscpNc;				/* �Զ��л���·����LKJ�س�����*/
	UINT8 InTrainLkjControlMscpNo;				/* �Զ��л���·����LKJ�س�����*/
	UINT8 InTrainElectricBrakeChanl1MscpNc;		/*�����ƶ�1�����ƶ�������*/
	UINT8 InTrainElectricBrakeChanl1MscpNo;		/*�����ƶ�1�����ƶ�������*/
	UINT8 InTrainElectricBrakeChanl2MscpNc;		/*�����ƶ�2�����ƶ�������*/
	UINT8 InTrainElectricBrakeChanl2MscpNo;		/*�����ƶ�2�����ƶ�������*/
	UINT8 InTrainElectricBrakeChanl3MscpNc;		/*�����ƶ�3�����ƶ�������*/
	UINT8 InTrainElectricBrakeChanl3MscpNo;		/*�����ƶ�3�����ƶ�������*/
	UINT8 InTrainElectricBrakeChanl4MscpNc;		/*�����ƶ�4�����ƶ�������*/
	UINT8 InTrainElectricBrakeChanl4MscpNo;		/*�����ƶ�4�����ƶ�������*/
	UINT8 InTrainConfirmButtonMscpNc;			/*�г�ȷ�ϰ�ť״̬ ����*/
	UINT8 InTrainConfirmButtonMscpNo;			/*�г�ȷ�ϰ�ť״̬ ����*/
	UINT8 InTrainResectSwitchMscpNc;			/*�г��������г�λ ����*/
	UINT8 InTrainResectSwitchMscpNo;			/*�г��������г�λ ����*/
	UINT8 InAtpTrainIntegMscpNc;				/*�г���������*/
	UINT8 InAtpTrainIntegMscpNo;				/*�г���������*/
	UINT8 InAtpBczdValidMscpNc;					/*��ʵʩ�����ƶ�����*/
	UINT8 InAtpBczdValidMscpNo;					/*��ʵʩ�����ƶ�����*/
	UINT8 InAtpSbRightInputMscpNc;				/*ǣ���ƶ��ֱ�����λ�ҷ����ֱ�����ǰλ����*/
	UINT8 InAtpSbRightInputMscpNo;				/*ǣ���ƶ��ֱ�����λ�ҷ����ֱ�����ǰλ����*/
	UINT8 InTrainEmergencyDoorBoardMscpNc;		/*�����Ÿǰ�״̬�ල�������Ž��������ֱ�״̬������*/
	UINT8 InTrainEmergencyDoorBoardMscpNo;		/*�����Ÿǰ�״̬�ල�������Ž��������ֱ�״̬������*/
	UINT8 InTrainDoorCloseMscpNc;				/*���Źرճ���*/
	UINT8 InTrainDoorCloseMscpNo;				/*���Źرճ���*/
	UINT8 TRAIN_KEY_OPENMscpNc;					/*˾��Կ�׼����*/
	UINT8 TRAIN_KEY_OPENMscpNo;					/*˾��Կ�׼����*/
	UINT8 InTrainDoorCloseAndLockMscpNc;		/*���Źر������ճ���*/
	UINT8 InTrainDoorCloseAndLockMscpNo;		/*���Źر������ճ���*/
	UINT8 TRAIN_EBHANDLE_ACTIVEMscpNc;			/*�����ֱ��������*/
	UINT8 TRAIN_EBHANDLE_ACTIVEMscpNo;			/*�����ֱ��������*/
	UINT8 TRAIN_OBSTDET_INPUTMscpNc;			/*�ϰ����ѹ���Ч������*/
	UINT8 TRAIN_OBSTDET_INPUTMscpNo;			/*�ϰ����ѹ���Ч������*/
	UINT8 InDouble1DoorCloseLockMscpNc;			/*��ʿ�ᳵ�Źر������գ�˫1��Ч������*/
	UINT8 InDouble1DoorCloseLockMscpNo;			/*��ʿ�ᳵ�Źر������գ�˫1��Ч������*/
	UINT8 TRAIN_BRAKE_FAULTMscpNc;				/*�ƶ��ع��ϣ�����*/
	UINT8 TRAIN_BRAKE_FAULTMscpNo;				/*�ƶ��ع��ϣ�����*/
	UINT8 ATP_TRACTION_RIGHTMscpNc;				/*ǣ���ƶ��ֱ���ǣ��λ�ҷ�����ǰ,����*/
	UINT8 ATP_TRACTION_RIGHTMscpNo;				/*ǣ���ƶ��ֱ���ǣ��λ�ҷ�����ǰ,����*/
	UINT8 EMERGENCYDOORMscpNc;					/*������״̬����*/
	UINT8 EMERGENCYDOORMscpNo;					/*������״̬����*/

	UINT8 InAtpCMModeMscpNc;					/*(Safe)CMģʽ,����*/
	UINT8 InAtpCMModeMscpNo;					/*(Safe)CMģʽ,����*/
	UINT8 InAtpNotSDModeMscpNc;					/*(Safe)NOT SD ģʽ,����*/
	UINT8 InAtpNotSDModeMscpNo;					/*(Safe)NOT SD ģʽ,����*/
	UINT8 InAtpFORModeMscpNc;					/*(Safe)FORģʽ,����*/
	UINT8 InAtpFORModeMscpNo;					/*(Safe)FORģʽ,����*/
	UINT8 InAtpREVModeMscpNc;					/*(Safe)REVģʽ,����*/
	UINT8 InAtpREVModeMscpNo;					/*(Safe)REVģʽ,����*/
	UINT8 InAtpSBModeMscpNc;					/*(Safe)��SBģʽ������*/
	UINT8 InAtpSBModeMscpNo;					/*(Safe)��SBģʽ������*/
	UINT8 InAtpSDModeMscpNc;					/*(Safe)��SDģʽ,����*/
	UINT8 InAtpSDModeMscpNo;					/*(Safe)��SDģʽ,����*/
	UINT8 InAtpEBFeedbackMscpNc;				/*(Safe)�ź�ϵͳ�����ƶ���������ɼ�������*/
	UINT8 InAtpEBFeedbackMscpNo;				/*(Safe)�ź�ϵͳ�����ƶ���������ɼ�������*/
	UINT8 InAtpConductZeroSpeedMscpNc;			/*(Safe)�����ź���ʵʩ,����*/
	UINT8 InAtpConductZeroSpeedMscpNo;			/*(Safe)�����ź���ʵʩ,����*/
	UINT8 InAtpSafeInterLockFeedbackMscpNc;		/*(Safe)��ȫ�����̵���״̬�ɼ�������*/
	UINT8 InAtpSafeInterLockFeedbackMscpNo;		/*(Safe)��ȫ�����̵���״̬�ɼ�������*/

	/*TODO ���¼�����������Ŀ�ģ������Ҫȷ���ǰ�ȫ���Ƿǰ�ȫ*/
	UINT8 InAtpKeyOpen;
	UINT8 InAtpSleep;

	/*------�ǰ�ȫ�����������-------*/
	UINT8 InAtpModeUp;							/*ģʽѡ���ϰ�ť����*/
	UINT8 InAtpModeDown;						/*ģʽѡ���°�ť����*/
	UINT8 InAtpArInput;							/*AR��ť����*/
	UINT8 InAtpRightDoorOpen;					/*���ſ��Ű�ť����*/
	UINT8 InAtpRightDoorClose;					/*���Ź��Ű�ť����*/
	UINT8 InAtpEumSwitch;						/*EUM���ؼ���*/
	UINT8 InAtpLeftDoorOpen;					/*���ſ��Ű�ť����*/
	UINT8 InAtpLeftDoorClose;					/*���Ź��Ű�ť����*/
	UINT8 InTrainDoorLock;						/*��������*/
	UINT8 InAtpConfirmButton2;					/* ȷ�ϰ�ť2��״̬*/
	UINT8 InTrainNobodyAlertSignal;				/*���˾���������ź�*/
	UINT8 InTrainWhistleSignal;					/*�����ź�*/
	UINT8 InAtpAtoStart1;						/*ATO������ť1�Ѱ���*/
	UINT8 InAtpAtoStart2;						/*ATO������ť2�Ѱ���(�ǰ�ȫ)*/
	UINT8 InAtpTvs1Check;						/*EMC��1�Ļ�����⣨�ǰ�ȫ��*/
	UINT8 InAtpTvs2Check;						/*EMC��2�Ļ�����⣨�ǰ�ȫ��*/
	UINT8 InChangeSwitchLkj;					/* �ֶ��л����ص�LKJλ*/
	UINT8 InChangeSwitchAuto;					/* �ֶ��л����ص��Զ�λ*/
	UINT8 InTrainFsb1;							/* �г���ʵʩ�����ƶ�1 */
	UINT8 InTrainFsb2;							/* �г���ʵʩ�����ƶ�2 */
	UINT8 InTrainFsb120Kpa;						/* �г���ʵʩ�����ƶ�120KPa */
	UINT8 InLkjEbEnable;						/*LKJ�����ƶ�ʵʩ״̬*/
	UINT8 InTrainCutSwitch;						/* �г��������г�λ��Ԥ����������510ʹ��InAtpEumSwitch��*/
	UINT8 InTrainRelieve;						/* VOBC�����г�����*/
	UINT8 InHandleEB;							/* ǣ���ƶ��ֱ��ڵ���λ*/

	/*------��ʿ�ᰲȫ&�ǰ�ȫ�����������-------*/
	UINT8 InAtpIVOCValidity;					/*(Unsafe)IVOC������Ч*/
	/*����������,H-FAO����*/
	UINT8 ATP_SMOKE_ALARM;						/*�̻𱨾�*/
	UINT8 ATP_DOOR_MM;							/*�˹����˹���*/
	UINT8 ATP_DOOR_AM;							/*�Զ����˹���*/
	UINT8 ATP_DOOR_AA;							/*�Զ����Զ���*/
	/*�»���*/
	UINT8 EBI1_FEEDBACK;						/*�����̵���1����״̬*/
	UINT8 EBI2_FEEDBACK;						/*�����̵���2����״̬*/
	UINT8 TVS1CHECK;							/*����TVS1���*/
	UINT8 TVS2CHECK;							/*����TVS2���*/
	UINT8 DOORALLCLOSE;							/*���Źرհ�ť*/
	/*�ɶ����߿���������*/
	UINT8 InTrainRedundancy;					/*����״̬����*/
	UINT8 InTrainFan1;							/*����1*/
	UINT8 InTrainFan2;							/*����2*/
	UINT8 InTrainEscapDoorUnlock;				/*�����Ž��������ֱ�������λ*/
	UINT8 InTrainGuestDoorUnlock;				/*�����Ž��������ֱ�������λ*/
	UINT8 reservedInput;						/*Ԥ��λ*/
}IO_SINGLE_INPUT_STRUCT;

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
UINT8 IoModule_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru);

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
UINT8 IoModuleInPut_Api_PreiodProcess(OUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);

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
UINT32 IoModulOutPut_Api_PreiodProcess(IN IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UCHAR LastActiveState);

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
UINT8 IoOutModule_Api_PowerOnInit(IN const UINT8 *IvocCtrlType);

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
UINT8 GetIoInfoStaticDate(UINT8 *pGetIoInfoBuff, UINT16* pBuffLen, UINT16 BuffMaxLen);

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
UINT8 SetIoInfoStaticDate(UINT8 *pSetIoInfoBuff, UINT16 *BuffLen);
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
void GetIoInfoPrintf(void);
#endif
/***************************************************************************************
* ��������:			�ֱ�����ATP/ATO��������ֵ
* �������:
* �����������:		��
* �������:			ATP���룬ATO����
* ȫ�ֱ���:			��
* ����ֵ:			0x00���ɹ���
* �޸ļ�¼��		added by Jielu 20230326
****************************************************************************************/
void IoModulCalMask_Api_PreiodProcess(OUT UINT32 *atpMask, OUT UINT32 *atoMask, IN IO_CFG_DATA_STRU *IOConfigFunc);
#endif 
