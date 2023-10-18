/****************************************************************************************************
* �ļ���   :  IOOutput.h
* ��Ȩ˵�� :  ���ؿƼ��ɷ����޹�˾
* �汾��   :  1.0
* ����ʱ�� :  2018.08.23
* ����     :
* �������� :  IO���ģ�鴦��ͷ�ļ�
* ʹ��ע�� :
*
* �޸ļ�¼ :
*
**************************************************************************************************/
#ifndef IO_OUTPUT_H_
#define IO_OUTPUT_H_

#include "CM_Types.h"
#include "dquVobcConfigData.h"
#include "IOModuleExtern.h"
#include "IOConfig.h"

/*350��Ŀ*/
#if(1 == H_FAO_PROJECT_USER)
#define DO_DATA_OUTPUT_BUF_SIZE		42U			/*DO�������*/
#define OUTPUT_FLAG_NUM				63U			/*��ȫ�ͷǰ�ȫ������ܹ���Ŀ*/
#define OUTPUT_ATO_NUM              9U          /*ATO �ǰ�ȫ���������Ŀ�����ڼ�������*/
#endif
/*˷����Ŀ*/
#if(1 == HHTC_PROJECT_USER)
#define DO_DATA_OUTPUT_BUF_SIZE	12U		/*DO�������*/
#define OUTPUT_FLAG_NUM			30	/* ��ȫ�ͷǰ�ȫ������ܹ���Ŀ*/
#endif
/*����*/
#if(1 == CC_PROJECT_USER)
#define DO_DATA_OUTPUT_BUF_SIZE	42U		/*DO�������-��Ӧ����Ĵ������*/
#define OUTPUT_FLAG_NUM			52	/* ��ȫ�ͷǰ�ȫ������ܹ���Ŀ*/
#endif
#define TRAIN_OUTPUT_BOARD			0x02		/*���Ϊ�����*/

#define OUT_SW_INVALID_VALUE		0x0000		/*��Ч�Ŀ��������*/
#define OUT_ANA_INVALID_VALUE		0xFFFFFFFF	/*��Ч��ģ�������*/
#define OUT_PWM_FREQUENCY			500		    /*PWM�����Ƶ��*/
#define OUT_PWM_INVALID_VALUE		0xFFFFFFFF	/*��Ч��PWM�����*/

/* IO�����Ҫ�õ��Ŀ��۽ṹ�� */
typedef struct
{
	UINT8 IoOutSlotNum; 					/* �������Ҫ�õ��Ĳ������ */
	UINT16 IoOutSlotId[BOARD_CONFIG_NUM]; 	/* ����Ĳ��ID */
	UINT8 SendDataBuf[BOARD_CONFIG_NUM][DO_DATA_OUTPUT_BUF_SIZE]; /* ���͵�������Ϣ */
}IO_OUTPUT_DATA_STRU;

/* ������ݿ����������ѯ�Ľṹ�� */
typedef struct
{
	UINT8 * IoOutputStruIndex; /* ������ƥ�������ṹ���еı���ָ�� */
	UINT8 IoOutputTypeIndex; /* ����������ѯ���е������ҵ���Ӧ������� */
}IO_OUTPUT_INDEX;

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
UINT8 IoModuleBitToByte(UINT8 Index, UINT8 OutValue, UINT32 *pOutUnSafeData, UINT32 *pOutSafeData);

#endif
