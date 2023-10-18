/****************************************************************************************************
* �ļ���   :  IOInput.h
* ��Ȩ˵�� :  ���ؿƼ��ɷ����޹�˾
* �汾��   :  1.0
* ����ʱ�� :  2018.08.23
* ����     :
* �������� :  IO�����������ͷ�ļ�
* ʹ��ע�� :
*
* �޸ļ�¼ :
*
**************************************************************************************************/
#ifndef  IO_INPUT_H_
#define  IO_INPUT_H_

#include "IOModuleExtern.h"
#include "IOConfig.h"


#define MAX_SAFE_CHANNELS_NUM		15U		/*0~14��15·����Ӧ��ȫ�����1-15·*/
#define MAX_UNSAFE_CHANNELS_NUM		24U		/*0~23��24·����Ӧ�ǰ�ȫ�����1-24·*/

#define MAX_CHANNELS_NUM	        48U            /*���ڵ�ɼ����ɼ�����������SID���ֵ*/
#define MAX_SAFE_INPUT_NUM          26U            /*Ŀǰ��ȫ�����������*/

/* ������������־ */
#define TRAIN_INPUT_BOARD							(UINT8)0x01		/*���Ϊ�����*/

/* ������ݿ����������ѯ�Ľṹ�� */
typedef struct
{
	UINT8 IoInputTypeIndex; /* ����������ѯ���е������ҵ���Ӧ�������� */
	UINT8 *IoInputStruIndex; /* ������ƥ�������ṹ���еı���ָ�� */
}IO_INPUT_INDEX;

extern IO_CFG_DATA_STRU gIoConfigDataStru;		/* IO�������ݽṹ��(������������һ���ṹ��) */

#define GET_BIT_FROM_INPUT_MSCP(x, y)		((((x)>>(y))&(0x01U)) ? IO_TRUE_MSCP: IO_FALSE_MSCP)
#define OPEN_VALID_MSCP		(UINT8)0x55
#define CLOOSE_VALID_MSCP	(UINT8)0xAA

typedef struct 
{
	UINT8   TimeLock;                        /*ʱ����*/
	UINT32  LastStateTime;                  /*�����ڵ�ʧЧ�����*/
} DIGITAL_IO_Time_MSCP;

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
UINT8 IoModuleInputFindConfigDataFromPlugBoxId(UINT16 PlugBoxId, IO_CFG_DATA_STRU *pDataConfigStru, UINT8 *pFindIndex);

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
UINT8 IoModuleInPut_Api_GetIOInputStru(IO_INPUT_STRUCT *pTc1HHIoInputStru, IO_INPUT_STRUCT *pTc2HHIoInputStru);

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
UINT8 IoModuleInPut_Api_GetBuffStru(UINT8 *pTc1IOBuff, UINT8 *pTc2IOBuff);

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
UINT8 IoModuleMscp_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru);

#endif
