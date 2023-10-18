/************************************************************************
* �ļ���	��  dsuTslConfigStruct.h
* �汾��	��  1.0
* ����ʱ��	��	2016.03.20
* ����		��	�����
* ��������	��	dsu�ṹ�嶨��ͷ�ļ�
* ʹ��ע��	��
* �޸ļ�¼	��  �ޡ�
************************************************************************/

#ifndef TSLCONFIG_STRUCT_H
#define TSLCONFIG_STRUCT_H

#include "CommonTypes.h"

/*TSL��������*/
#define  TSL_CONFIG_DATA_TYPE			0xFDu

/*TSL�������ݱ��*/
#define TSL_CONFIG_DATA_ID				1u

/*������������TSL���ͨ������*/
#define TSL_COMM_MAX_NUM				5u


#define LEN_WORD sizeof(UINT16)
#define LEN_DWORD sizeof(UINT32)

#ifdef __cplusplus
extern "C" {
#endif


/* ��̬���ݳ�����Ϣ�ṹ��*/
typedef struct
{
	UINT16 wTslConfigNumLen;			/*���ݿ���TSL�������ݱ�Ľṹ�������*/
}DSU_TSLCONFIG_LEN_STRU;

/*��̬���ݿ�ṹ��*/
/*TSL�������ݱ�*/
typedef struct {
	UINT16 index;				/*�������*/
	UINT16 tslId;				/*TSL_ID*/
	UINT16 linkId;				/*����link���*/
	UINT32 off;					/*����linkƫ����*/
	UINT32 tslIvocValidation;	/*TSL-IVOC��������У����Ϣ*/
	UINT16 tslSwitchID;			/*TSL�����õ���ID*/
} DSU_TSL_CONFIG_STRU;

/*��ž�̬���ݸ������ݽṹ��ͷָ��*/
typedef struct DSU_StaticTslConfig
{
	DSU_TSL_CONFIG_STRU *pTslConfigStru;		/*TSL�������ݱ�ṹ��ָ��*/
}DSU_STATIC_TSLCONFIG_STRU;

/*Ϊ����������ȫ������*/
typedef struct DSU_TslConfigIndexStruct
{
	UINT16 TSLCONFIGINDEXNUM;				/*TSL�������ݱ���������ռ��С*/
	UINT16* dsuTslConfigIndex;				/*TSL������Ϣ��������*/

}DSU_TSLCONFIG_INDEX_STRU;

/*TSL�������ݱ��ܽṹ��*/
typedef struct DSU_TslConfigStruct
{
	DSU_TSLCONFIG_INDEX_STRU *dsuTslConfigIndexStru;

	DSU_TSLCONFIG_LEN_STRU *dsuTslConfigLenStru;            /*ȫ�ֱ�������¼��ȡ��DSU�����еĸ��������ݵ�����*/

	DSU_STATIC_TSLCONFIG_STRU *dsuStaticTslConfigStru;		/*������ݿ���ÿ�ֽṹ��ͷָ��*/
} DSU_TSLCONFIG_STRU;

/*�������ݿ�ṹ��*/
extern DSU_TSLCONFIG_STRU	*g_dsuTslConfigStru;

#ifdef __cplusplus
}
#endif

#endif
