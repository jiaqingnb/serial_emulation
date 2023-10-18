/****************************************************************************************************
* �ļ���   :  IOOutputVbtc.c
* ��Ȩ˵�� :  ���ؿƼ��ɷ����޹�˾
* �汾��   :  1.0
* ����ʱ�� :  2020.05.07
* ����     :  ��ܷӱ
* �������� : IO��������������ļ�
* ʹ��ע�� : ����ͨ��IOģ����Բ�ͬ��·�Ĳ�ͬ����;
*
* �޸ļ�¼ : 
*
**************************************************************************************************/
#include "IOOutput.h"
#include "IOModuleExtern.h"
#include "IOModuleVbtcExtern.h"
#ifdef ITO_USE_IO_MODULE
#include "MaintainInfoProcess.h"
#endif

extern IO_CFG_STRU	gIoVbtcConfigDataStru;
extern UINT8 gLastTc1_Cab_Key;
extern UINT8 Tc1_Tolerate_tic;
extern UINT8 gLastTc2_Cab_Key;
extern UINT8 Tc2_Tolerate_tic;

extern UINT8 gARReverStrTag;      /*����ARģʽ�۷��Ŀ�ʼ��־*/
extern UINT8 gFamDirBackTag;      /*FAM��������־*/
extern UINT8 gFamCtrlSide;
extern UINT8 gFamReverStrTag;     /*����FAMģʽ�۷��Ŀ�ʼ��־*/
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
UINT8 IoOutPut_Api_PreiodProcess(IN IO_OUT_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo)
{
	UINT8 FuncRtn = FUNC_SUCESS;
	UINT8 CallFuncRtn = FUNC_CODE1;
	UINT8 ZeroBuff[DO_DATA_OUTPUT_BUF_SIZE] = { 0U };

	if (NULL != pIoIn_InputInfo)
	{
	    if (10U >= pIoIn_InputInfo->IoOut_InputInfo.CycCount)
	    {
	        /*ǰ10��������������ȫ0����ֹ��������ABϵ����彨���ٶȲ�һ�£������������ϵ�����һ�£���ϵ�����崻��������*/
            CallFuncRtn = CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC1_OUT_1_TYPE_ID,BOARD_INNET_CPU_MSG,0U,ZeroBuff,DO_DATA_OUTPUT_BUF_SIZE);
            CallFuncRtn |= CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC1_OUT_2_TYPE_ID,BOARD_INNET_CPU_MSG,0U,ZeroBuff,DO_DATA_OUTPUT_BUF_SIZE);
            CallFuncRtn |= CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC2_OUT_1_TYPE_ID,BOARD_INNET_CPU_MSG,0U,ZeroBuff,DO_DATA_OUTPUT_BUF_SIZE);
            CallFuncRtn |= CommonModeule_Api_DataFramSendProcess((UINT16)IO_TC2_OUT_2_TYPE_ID,BOARD_INNET_CPU_MSG,0U,ZeroBuff,DO_DATA_OUTPUT_BUF_SIZE);

            if (FUNC_SUCESS == CallFuncRtn)
            {
                /*nothing*/
            }
            else
            {
                FuncRtn |= FUNC_CODE2;
            }
	    }
	    else
	    {
	        if (LINE_DISNEY == gIoVbtcConfigDataStru.LineConfig)
	        {
	            /*��ʿ�Ὺ������Ҫ��һ��ת��*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb = CBTC_ENABLE;
	            }

	            if (CONTROL_MODE != pIoIn_InputInfo->shadowModeFlag)
	            {
	                /*Ӱ��ģʽ�£�IVOC�������ȫ�࣬����������������ƶ�(��EBģ�鴦��)�������١���������ʹ��*/
	                /*�����ź����*/
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutAtpZeroSpeedsig = CBTC_FALSE;
	                /*����ʹ��*/
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutAtpLeftDoorEn = CBTC_FALSE;
	                /*����ʹ��*/
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutAtpRightDoorEn = CBTC_FALSE;
	            }
	            else
	            {
	                /*nothing*/
	            }
	        }
	        else if (LINE_COMMON == gIoVbtcConfigDataStru.LineConfig)
	        {
	            /*����11���������ת��*/
	            /*���ƶ�*/
	            /*�����ƶ����*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb = CBTC_ENABLE;
	            }

	            /*ǣ���г�*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutTractionCut)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutTractionCut = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutTractionCut = CBTC_ENABLE;
	            }

	            /*�ȴ���*/
	            /*�����ƶ����*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutEb)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutEb = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutEb = CBTC_ENABLE;
	            }

	            /*ǣ���г�*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutTractionCut)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutTractionCut = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutTractionCut = CBTC_ENABLE;
	            }
	        }
	        else
	        {
	            /*nothing*/
	        }

	        /*��ס�ս���FAM�۷��Ŀ��ƶ�*/
	        if ((CBTC_TRUE == pIoIn_InputInfo->IoOut_InputInfo.FAMReverseTag)&&(CBTC_FALSE ==gFamReverStrTag))
	        {
	            gFamCtrlSide = pIoIn_InputInfo->IoOut_InputInfo.MainCtrlSide;
	            gFamReverStrTag = CBTC_TRUE;
	        }
	        else
	        {
	            /*nothing*/
	        }

	        /*��ס�ս���AR�۷��Ŀ��ƶ�*/
	        if ((CBTC_TRUE == pIoIn_InputInfo->IoOut_InputInfo.ARReverseTag) && (CBTC_FALSE == gARReverStrTag))
	        {
	            gFamCtrlSide = pIoIn_InputInfo->IoOut_InputInfo.MainCtrlSide;
	            gARReverStrTag = CBTC_TRUE;
	        }
	        else
	        {
	            /*nothing*/
	        }

	        if (CBTC_TRUE != pIoIn_InputInfo->IoOut_InputInfo.FAMReverseTag)
	        {
	            gFamReverStrTag = CBTC_FALSE;
	        }

	        if (CBTC_TRUE != pIoIn_InputInfo->IoOut_InputInfo.ARReverseTag)
	        {
	            gARReverStrTag = CBTC_FALSE;
	        }

	        if (CBTC_TRUE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.DIRBACKWARD_OUTPUT)
	        {
	            gFamDirBackTag = CBTC_TRUE;
	        }
	        else
	        {
	            gFamDirBackTag = CBTC_FALSE;
	        }

	        AppLogPrintStr(ENUM_LOG_ERR,"ReverStrTag:%x,%x,%x,%x\n",pIoIn_InputInfo->IoOut_InputInfo.FAMReverseTag,gFamReverStrTag,gFamCtrlSide,gFamReverStrTag);
	        FuncRtn = IoModulOutPut_Api_PreiodProcess(&pIoIn_InputInfo->IoOut_InputInfo, pIoIn_InputInfo->lastIOCtrlSide);
			
	    }
	}
	else
	{
		FuncRtn |= FUNC_CODE1;
	}

	return FuncRtn;
}

/***************************************************************************************
* ��������:			��ȡFAM��������־
* �������:			��
* �����������:     ��
* �������:			��
* ȫ�ֱ���:			gFamDirBackTag
* ����ֵ:			FAM��������־
* �޸ļ�¼��		������wyd 20230217
****************************************************************************************/
UINT8 IoVbtcOut_Api_GetFamDirBack(void)
{
	return gFamDirBackTag;
}
