/****************************************************************************************************
* �ļ���   :  IOModuleVbtcExtern.h
* ��Ȩ˵�� :  ���ؿƼ��ɷ����޹�˾
* �汾��   :  1.0
* ����ʱ�� :  2020.05.07
* ����     :
* �������� :  IOģ��ӿڣ������롢�����IO�������ṹ��
* ʹ��ע�� :
*
* �޸ļ�¼ :
*
**************************************************************************************************/
#ifndef IO_MODULE_VBTC_EXTERN_H_
#define IO_MODULE_VBTC_EXTERN_H_

#include "App_ComStru.h"
#include "IOModuleExtern.h"

typedef struct
{
	UINT8  IVOCCtrlType;					/*�г��س�����*/
	UINT8  IvocTCAttri;						/*�г���������*/
	UINT8  LineConfig;						/*��ȡ����������Ϣ������·������Ϣ*/
	UINT8  DoorCloseLockDivTag;             /*���Źر��������Ƿ�ֿ��ɼ�*/
	UINT8  DsnModeIoInputCycle;             /*��ʿ���ʻ̨ģʽ��ť��������*/
	IO_CFG_DATA_STRU IoConfigDataStru;		/*IO����������Ϣ�ṹ��*/
}IO_CFG_STRU;

/*IO����ģ������ṹ��*/
typedef struct
{
	UINT8 gOtherVobcAttri;					/*�Զ��Ƿ�Ϊ���ƶ�*/
}IO_IN_VBTC_PERIOD_INPUT_STRUCT;

/*IO����ģ��������Ϣ�ṹ��*/
typedef struct
{
	UINT8 MainCtrSide;				/*���ƶ�*/
	UINT8 IdleSide;					/*�ȴ���*/
	/*���г�����Ϊһ����һ��ʱ���������ֽڣ���һ��������ʱ�������ֽڲ�ʹ��*/
	UINT8 LocalVobcAttri;			/*�����Ƿ�Ϊ���ƶ�*/
	UINT8 IoHaveErr;				/*IO�����д��� ���ڵ�ʿ��Ĳ��ɵ��г��� db 20220713*/
	UINT8 IoDsnModeKey;				/*��ʿ��ʵ����Ŀ���ƶ�ģʽ����*/
	UINT8 Tc1DigtalInputState[INPUT_DEFINE_MAX];	/*TC1�˿�����*/
	UINT8 Tc2DigtalInputState[INPUT_DEFINE_MAX];	/*TC2�˿�����*/
	UINT8 CtrlDigtalInputState[INPUT_DEFINE_MAX];	/*���ƶ˿�����*/
	UINT8 IdleDigtalInputState[INPUT_DEFINE_MAX];	/*�ȴ��˿�����*/
}IO_IN_VBTC_PERIOD_OUTPUT_STRUCT;

/*IO���ģ������ṹ��*/
typedef struct
{
	UINT8 shadowModeFlag;                       /*����ģʽ0x55 Ӱ��ģʽ0xAA  Ӱ��ģʽ�²��ɵ��� 0xcc*/
	UINT8 lastIOCtrlSide;                       /*�����ڿ��ƶ�*/
	IO_OUT_PERIOD_INPUT_STRUCT IoOut_InputInfo;	/*IO���ģ����������ṹ��*/
}IO_OUT_VBTC_PERIOD_INPUT_STRUCT;


/***************************************************************************************
* ��������:        IO����ģ����Ҫ��ʼ����ֵ�Լ���ȡIO���õ�����
* �������:        ��
* �����������:    ��
* �������:        
* ȫ�ֱ���:         
* ����ֵ:           0x00:��ʼ���ɹ�
					0x01:��ȡIO��������ʧ��
* �޸ļ�¼��		
****************************************************************************************/
UINT8 IoModuleVbtc_Api_PowerOnInit(IN const IO_CFG_STRU *pIoCfgStru);

/***************************************************************************************
* ��������:        IO��������ݴ���,�����յ���IO���ݽ�����ŵ�IO�������ݵĽṹ����
* �������:        ��
* �����������:    ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
					0x01:����TC1��IO��������
					0x02:����TC1��IO��������
					0x04:����TC2��IO��������
					0x08:����TC2��IO��������
* �޸ļ�¼��
****************************************************************************************/
UINT8 IoInPut_Api_PreiodProcess(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, OUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);
/***************************************************************************************
* ��������:			IO���ȥ�����ݴ���,�����յ���IO���ݽ�����ŵ�IO�������ݵĽṹ����
* �������:			IO_OUT_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo
* �����������:     ��
* �������:
* ȫ�ֱ���:
* ����ֵ:
					0x00���ɹ���
					0x01:
* �޸ļ�¼��
****************************************************************************************/
UINT8 IoOutPut_Api_PreiodProcess(IN IO_OUT_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo);

/***************************************************************************************
* ��������:			��ȡ���뿪������״̬
* �������:			tcType:1��2������
					DigtalInputIndex :��������������IOModuleExtern.h���������һ�£�
					DigtalInputState:������������Ӧ�����뿪������״̬��
* �����������:     ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
					FUNC_CODE1:�����������ݳ���Χ
					FUNC_CODE2:1/2�����Դ���
* �޸ļ�¼��
****************************************************************************************/
UINT8 IoInPut_Api_GetDigtalInput(UINT8 tcType,UINT8 DigtalInputIndex,UINT8 *DigtalInputState);

/*��ȡFAM��������־*/
UINT8 IoVbtcOut_Api_GetFamDirBack(void);

/***************************************************************************************
* ��������:			��ȡ���Ƶȴ���
* �������:
* �����������:
* �������:			���ƶˡ��ȴ���
* ȫ�ֱ���:         gIoInOutputStru
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��		added by Jielu 20230326
****************************************************************************************/
UINT8 IoInPutVbtc_Api_GetMainCtrSide(OUT UINT8* pCtrlSide, OUT UINT8* pIdleSide);

/*��ȡ��������*/
UINT8 IoInVbtc_Api_GetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len, UINT16 BuffMaxLen);

/*���ø�������*/
UINT8 IoInVbtc_Api_SetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len);

#endif 