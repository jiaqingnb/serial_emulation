/****************************************************************************************************
* �ļ���   :  IOInputVbtc.c
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

#include "IOInput.h"
#include "IOInputVbtc.h"
#include "IOModuleVbtcExtern.h"

IO_CFG_STRU	gIoVbtcConfigDataStru = { 0U };
static IO_IN_PERIOD_OUTPUT_STRUCT gIoInOutputStru = { 0 };

static UINT8 gTc1DsnMode = 0u;
static UINT8 gTc1LastDsnMode = 0u;
static UINT8 gTc2DsnMode = 0u;
static UINT8 gTc2LastDsnMode = 0u;

/*���̿�����3����*/
extern UINT8 gLastTc1_Cab_Key;
extern UINT8 Tc1_Tolerate_tic;
extern UINT8 gLastTc2_Cab_Key;
extern UINT8 Tc2_Tolerate_tic;

extern UINT8 gARReverStrTag;      /*����ARģʽ�۷��Ŀ�ʼ��־*/
extern UINT8 gFamDirBackTag;      /*FAM��������־*/
extern UINT8 gFamCtrlSide;
extern UINT8 gFamReverStrTag;     /*����FAMģʽ�۷��Ŀ�ʼ��־*/

static UINT8 IoModuleInPutVbtc_Output(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, INOUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);

/***************************************************************************************
* ��������:			IO����ģ����Ҫ��ʼ����ֵ�Լ���ȡIO���õ�����
* �������:			��
* �����������:     ��
* �������:        
* ȫ�ֱ���:         
* ����ֵ:           0x00:��ʼ���ɹ�
					0x01:��ȡIO��������ʧ��
* �޸ļ�¼��		
****************************************************************************************/
UINT8 IoModuleVbtc_Api_PowerOnInit(IN const IO_CFG_STRU *pIoCfgStru)
{
	UINT8 RtnNo = FUNC_SUCESS;
	UINT8 FuncNo = FUNC_SUCESS;

	if (NULL != pIoCfgStru)
	{
		/*��ȡIO��������*/
		(void)CommonMemCpy(&gIoVbtcConfigDataStru, (UINT32)sizeof(IO_CFG_STRU), pIoCfgStru, (UINT32)sizeof(IO_CFG_STRU));
		FuncNo = IoModuleMscp_Api_PowerOnInit(&gIoVbtcConfigDataStru.IoConfigDataStru);
		if (FUNC_SUCESS != FuncNo)
		{
			/*IO���ݳ�ʼ��ʧ��*/
			RtnNo |= FUNC_CODE2;
		}
	}
	else
	{
		RtnNo |= FUNC_CODE1;
	}

	return RtnNo;
}


/***************************************************************************************
* ��������:			�����Ϣ����
* �������:			
* �����������:     ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��
* 1.һ����˫�ˣ����˶�Ϊ���ƶ�ʱ���ӿ��ƶ˵Ŀ�������by zy 20210425
* 2.�޸�YF0069-2262 δ��ָ������ж� by zy 20220809
****************************************************************************************/
static UINT8 IoModuleInPutVbtc_Output(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, INOUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	UINT8 FuncRtn = FUNC_SUCESS;
	UINT8 Tc1_CabA_Key = CBTC_FALSE;	/*1��/A��Կ��*/
	UINT8 Tc2_CabB_Key = CBTC_FALSE;	/*2��/B��Կ��*/
	/*��ʿ����·��Ҫ�õı���*/
	UINT8 mNOTSDMode = CBTC_FALSE;
	UINT8 mCMMode = CBTC_FALSE;
	UINT8 mFORMode = CBTC_FALSE;
	UINT8 mREVMode = CBTC_FALSE;
	UINT8 mSDMode = CBTC_FALSE;
	UINT8 mSBMode = CBTC_FALSE;
	UINT8 mDsnMode = 0u;

	if ((NULL == pIoIn_InputInfo) || (NULL == pIoIn_OutputInfo))
	{
		FuncRtn |= FUNC_CODE1;
	}
	else
	{
	    /*���Źر������շֿ��ɼ�ʱ���յ����Źر���Ч��������Ч����ֵ�����Źر������ա���Ч*/
	    if (CBTC_TRUE == gIoVbtcConfigDataStru.DoorCloseLockDivTag)
	    {
	        if ((CBTC_TRUE == pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DOORCLOSE]) && (CBTC_TRUE == pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DOORLOCK]))
	        {
	            pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_DOOR_CLOSEDANDLOCK] = CBTC_TRUE;
	        }
	        else
	        {
                pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_DOOR_CLOSEDANDLOCK] = CBTC_FALSE;
	        }

            if ((CBTC_TRUE == pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DOORCLOSE]) && (CBTC_TRUE == pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DOORLOCK]))
            {
                pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_DOOR_CLOSEDANDLOCK] = CBTC_TRUE;
            }
            else
            {
                pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_DOOR_CLOSEDANDLOCK] = CBTC_FALSE;
            }
	    }
	    else
	    {
	        /*������*/
	    }

		/*Line Disney*/
		if (LINE_DISNEY == gIoVbtcConfigDataStru.LineConfig)
		{
		    /*��ʿ���¿���������(���ſ����Źء����ſ����Ź���ͬһ����λ��ʾ���Ҹߵ�λ��ʾ���ţ��͵�λ��ʾ����)*/
		   /*TC1*/
		   if (CBTC_TRUE == pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_LEFT_DOOR_OPEN])
		   {
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_LEFT_DOOR_OPEN] = CBTC_TRUE;
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_LEFT_DOOR_CLOSE] = CBTC_FALSE;
		   }
		   else
		   {
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_LEFT_DOOR_CLOSE] = CBTC_TRUE;
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_LEFT_DOOR_OPEN] = CBTC_FALSE;
		   }

		   if (CBTC_TRUE == pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_RIGHT_DOOR_OPEN])
		   {
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_RIGHT_DOOR_OPEN] = CBTC_TRUE;
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_RIGHT_DOOR_CLOSE] = CBTC_FALSE;
		   }
		   else
		   {
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_RIGHT_DOOR_CLOSE] = CBTC_TRUE;
		       pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_RIGHT_DOOR_OPEN] = CBTC_FALSE;
		   }

		   /*TC2*/
		   if (CBTC_TRUE == pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_LEFT_DOOR_OPEN])
		   {
		      pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_LEFT_DOOR_OPEN] = CBTC_TRUE;
		      pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_LEFT_DOOR_CLOSE] = CBTC_FALSE;
		   }
		   else
		   {
		      pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_LEFT_DOOR_CLOSE] = CBTC_TRUE;
		      pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_LEFT_DOOR_OPEN] = CBTC_FALSE;
		   }

		   if (CBTC_TRUE == pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_RIGHT_DOOR_OPEN])
		   {
		       pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_RIGHT_DOOR_OPEN] = CBTC_TRUE;
		       pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_RIGHT_DOOR_CLOSE] = CBTC_FALSE;
		   }
		   else
		   {
		       pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_RIGHT_DOOR_CLOSE] = CBTC_TRUE;
		       pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_RIGHT_DOOR_OPEN] = CBTC_FALSE;
		   }

#ifndef SIM_VERSION
			/*һ�����ص��˵�ʱ��IO����Ĭ�Ϸ���Tc1��˴���������,��������2�ˣ������������Tc2�ṹ����  hqy 2020-05-09*/
			if (ONE_CTRL_ONE == gIoVbtcConfigDataStru.IVOCCtrlType)
			{
				if (CAB_TC2 == gIoVbtcConfigDataStru.IvocTCAttri)
				{
					MemCpy((&pIoIn_OutputInfo->Tc2DigtalInputState), (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
					CM_Memset(&pIoIn_OutputInfo->Tc1DigtalInputState, (UINT8)0x00, (sizeof(UINT8)*INPUT_DEFINE_MAX));
				}
			}
			else
			{
				/*���账��*/
			}
#endif

			/*TC1*/
			mNOTSDMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_NONSD_MODE];
			mCMMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_CM_MODE];
			mFORMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_FWD_MODE];
			mREVMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_REV_MODE];
			mSDMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_SD_MODE];
			mSBMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_STBY_MODE];
			/*��ȡ��ǰģʽ����*/
			if((CBTC_TRUE == mCMMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_CM_MODE;
			}
			else if((CBTC_TRUE == mFORMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_FWD_MODE;
			}
			else if((CBTC_TRUE == mREVMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_REV_MODE;
			}
			else if((CBTC_TRUE == mSBMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_STBY_MODE;
			}
			else if((CBTC_TRUE == mSDMode) && (CBTC_TRUE != mNOTSDMode))
			{
				mDsnMode = IN_SD_MODE;
			}
			else
			{
				/*������*/
			}

			/*ģʽ�����л�ʱ����������������ں�����л�*/
			if(mDsnMode == gTc1LastDsnMode)
			{
				Tc1_Tolerate_tic++;
				if(Tc1_Tolerate_tic >= gIoVbtcConfigDataStru.DsnModeIoInputCycle)
				{
					gTc1DsnMode = mDsnMode;
					Tc1_Tolerate_tic = gIoVbtcConfigDataStru.DsnModeIoInputCycle;
				}
				else
				{
					/*do nothing*/
				}
			}
			else
			{
				Tc1_Tolerate_tic = 0;
			}	
			gTc1LastDsnMode = mDsnMode;
			
			AppLogPrintStr(ENUM_LOG_ERR,"IO1:%x,%x,%x,%x,%x,%x;dsnMode=%x,%x\n", mNOTSDMode, mCMMode, mFORMode, mREVMode, mSDMode,mSBMode,
			mDsnMode,gTc1DsnMode);
			if ((IN_FWD_MODE == gTc1DsnMode) || (IN_CM_MODE == gTc1DsnMode))
			{
				Tc1_CabA_Key = CBTC_TRUE;
				/*�����ֱ���ǰ*/
				pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DIR_FORWARD] = CBTC_TRUE;
				pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DIR_BACKWARD] = CBTC_FALSE;
			}
			else if(IN_REV_MODE == gTc1DsnMode)
			{
				Tc1_CabA_Key = CBTC_TRUE;
				/*�����ֱ����*/
				pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DIR_BACKWARD] = CBTC_TRUE;
				pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DIR_FORWARD] = CBTC_FALSE;
			}
			else
			{
				Tc1_CabA_Key = CBTC_FALSE;
			}

			/*TC2*/
			mNOTSDMode = pIoIn_OutputInfo->Tc2DigtalInputState[IN_NONSD_MODE];
			mCMMode = pIoIn_OutputInfo->Tc2DigtalInputState[IN_CM_MODE];
			mFORMode = pIoIn_OutputInfo->Tc2DigtalInputState[IN_FWD_MODE];
			mREVMode = pIoIn_OutputInfo->Tc2DigtalInputState[IN_REV_MODE];
			mSDMode = pIoIn_OutputInfo->Tc2DigtalInputState[IN_SD_MODE];
			mSBMode = pIoIn_OutputInfo->Tc2DigtalInputState[IN_STBY_MODE];
			/*��ȡ��ǰģʽ����*/
			if((CBTC_TRUE == mCMMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_CM_MODE;
			}
			else if((CBTC_TRUE == mFORMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_FWD_MODE;
			}
			else if((CBTC_TRUE == mREVMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_REV_MODE;
			}
			else if((CBTC_TRUE == mSBMode) && (CBTC_TRUE == mNOTSDMode))
			{
				mDsnMode = IN_STBY_MODE;
			}
			else if((CBTC_TRUE == mSDMode) && (CBTC_TRUE != mNOTSDMode))
			{
				mDsnMode = IN_SD_MODE;
			}
			else
			{
				/*������*/
			}

			/*ģʽ�����л�ʱ����������������ں�����л�*/
			if(mDsnMode == gTc2LastDsnMode)
			{
				Tc2_Tolerate_tic++;
				if(Tc2_Tolerate_tic >= gIoVbtcConfigDataStru.DsnModeIoInputCycle)
				{
					gTc2DsnMode = mDsnMode;
					Tc2_Tolerate_tic = gIoVbtcConfigDataStru.DsnModeIoInputCycle;
				}
				else
				{
					/*do nothing*/
				}
			}
			else
			{
				Tc2_Tolerate_tic = 0;
			}	
			gTc2LastDsnMode = mDsnMode;
			
			AppLogPrintStr(ENUM_LOG_ERR,"IO2:%x,%x,%x,%x,%x,%x;dsnMode=%x,%x\n", mNOTSDMode, mCMMode, mFORMode, mREVMode, mSDMode,mSBMode,
			mDsnMode,gTc2DsnMode);

			if ((IN_FWD_MODE == gTc2DsnMode) || (IN_CM_MODE == gTc2DsnMode))
			{
				Tc2_CabB_Key = CBTC_TRUE;
				/*�����ֱ���ǰ*/
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_FORWARD] = CBTC_TRUE;
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_BACKWARD] = CBTC_FALSE;
			}
			else if(IN_REV_MODE == gTc2DsnMode)
			{
				Tc2_CabB_Key = CBTC_TRUE;
				/*�����ֱ����*/
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_BACKWARD] = CBTC_TRUE;
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_FORWARD] = CBTC_FALSE;
			}
			else
			{
				Tc2_CabB_Key = CBTC_FALSE;
			}

			/*�г������� ��ʿ��û���г������Կ����� ����Ĭ�ϲ��ᶪʧ������ db 20220711*/
			pIoIn_OutputInfo->Tc1DigtalInputState[IN_ATP_TRAIN_INTEG] = CBTC_TRUE;
			pIoIn_OutputInfo->Tc2DigtalInputState[IN_ATP_TRAIN_INTEG] = CBTC_TRUE;			
			if (CAB_TC1 == gIoVbtcConfigDataStru.IvocTCAttri) 
			{	
				pIoIn_OutputInfo->IoDsnModeKey = gTc1DsnMode;
			}
			else if (CAB_TC2 == gIoVbtcConfigDataStru.IvocTCAttri)
			{
				pIoIn_OutputInfo->IoDsnModeKey = gTc2DsnMode;
			}
			else
			{
				/*do nothing*/
			}
		}
		else
		{
			/*��ȡ1��/A��Կ��*/
			Tc1_CabA_Key = pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_CAB_ACTIVE];
			/*��ȡ2��/B��Կ��*/
			Tc2_CabB_Key = pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_CAB_ACTIVE];
		}
		
		if (ONE_CTRL_TWO == gIoVbtcConfigDataStru.IVOCCtrlType)
		{
			/*һ����������*/
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


			if (CAB_A_TC1 == pIoIn_OutputInfo->MainCtrSide)
			{
				(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
				(void)CommonMemCpy((&pIoIn_OutputInfo->IdleDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc2DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
			}
			else if (CAB_B_TC2 == pIoIn_OutputInfo->MainCtrSide)
			{
				(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc2DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
				(void)CommonMemCpy((&pIoIn_OutputInfo->IdleDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
			}
			else if(BOTH_CTR == pIoIn_OutputInfo->MainCtrSide)
			{
				/*���˶�Ϊ���ƶ�ʱ����ѡһ�˵Ŀ�������Ϊ���ƶ˵Ŀ�����(���͸�MMI����״̬��Ҫ�ӿ��ƶ˻�ȡ���������˴���)*/
			    (void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
			}
			else
			{
			    /*����д���ƶˡ��ǿ��ƶ˿�����*/
			}
		}
		else
		{
			/*һ�����ص���*/
			if ((CAB_TC1 == gIoVbtcConfigDataStru.IvocTCAttri) && (CBTC_TRUE == Tc1_CabA_Key))
			{
				/*��ǰ��ΪTC1����TC1�ɼ�����ʻ̨Կ�״򿪣���ǰ��ΪTC2����TC2�ɼ�����ʻ̨Կ�״򿪣���ǰ��Ϊ���ƶ�*/
				pIoIn_OutputInfo->LocalVobcAttri = CONTROL_IVOC;

				pIoIn_OutputInfo->MainCtrSide = CAB_A_TC1;
				pIoIn_OutputInfo->IdleSide = CAB_B_TC2;

				if (LINE_DISNEY == gIoVbtcConfigDataStru.LineConfig)
				{
					pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_KEY_OPEN] = CBTC_TRUE;

					pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_CAB_ACTIVE] = CBTC_TRUE;				
				}

				(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
			}
			else if ((CAB_TC2 == gIoVbtcConfigDataStru.IvocTCAttri) && (CBTC_TRUE == Tc2_CabB_Key))
			{
				/*��ǰ��ΪTC1����TC1�ɼ�����ʻ̨Կ�״򿪣���ǰ��ΪTC2����TC2�ɼ�����ʻ̨Կ�״򿪣���ǰ��Ϊ���ƶ�*/
				pIoIn_OutputInfo->LocalVobcAttri = CONTROL_IVOC;

				pIoIn_OutputInfo->MainCtrSide = CAB_B_TC2;
				pIoIn_OutputInfo->IdleSide = CAB_A_TC1;

				if (LINE_DISNEY == gIoVbtcConfigDataStru.LineConfig)
				{
					pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_KEY_OPEN] = CBTC_TRUE;

					pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_CAB_ACTIVE] = CBTC_TRUE;
				}

				(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc2DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
			}
			else
			{
				/*��ǰ��Ϊ�ȴ���*/
				pIoIn_OutputInfo->LocalVobcAttri = IDLE_IVOC;
				/*���ﲻ���Ϊʲô��Ҫ�������ƶ�*/
				if (CONTROL_IVOC == pIoIn_InputInfo->gOtherVobcAttri)
				{
					if (CAB_TC1 == gIoVbtcConfigDataStru.IvocTCAttri)
					{
						pIoIn_OutputInfo->MainCtrSide = CAB_B_TC2;
						pIoIn_OutputInfo->IdleSide = CAB_A_TC1;
#ifndef SIM_VERSION
						if (LINE_DISNEY == gIoVbtcConfigDataStru.LineConfig)
						{
							pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_KEY_OPEN] = CBTC_TRUE;

							pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_CAB_ACTIVE] = CBTC_TRUE;
						}
#endif
						(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
					}
					else if (CAB_TC2 == gIoVbtcConfigDataStru.IvocTCAttri)
					{
						pIoIn_OutputInfo->MainCtrSide = CAB_A_TC1;
						pIoIn_OutputInfo->IdleSide = CAB_B_TC2;
#ifndef SIM_VERSION
						if (LINE_DISNEY == gIoVbtcConfigDataStru.LineConfig)
						{
							pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_KEY_OPEN] = CBTC_TRUE;

							pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_CAB_ACTIVE] = CBTC_TRUE;
						}
#endif
						(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc2DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
					}
				}
				else
				{
					pIoIn_OutputInfo->MainCtrSide = BOTH_NO_CTR;
					pIoIn_OutputInfo->IdleSide = BOTH_NO_CTR;

					if (CAB_TC1 == gIoVbtcConfigDataStru.IvocTCAttri)
					{
						(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
					}
					else if (CAB_TC2 == gIoVbtcConfigDataStru.IvocTCAttri)
					{
						(void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc2DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
					}
				}
			}

			if ((CONTROL_IVOC == pIoIn_InputInfo->gOtherVobcAttri) && (CONTROL_IVOC == pIoIn_OutputInfo->LocalVobcAttri))
			{
				/*�Զ˺ͱ��˶��ǿ��ƶ�*/
				pIoIn_OutputInfo->MainCtrSide = BOTH_CTR;
				pIoIn_OutputInfo->IdleSide = BOTH_CTR;
			}
            AppLogPrintStr(ENUM_LOG_ERR,"12:%x,%x,%x,%x,%x,eb:%x,integ:%x,sbtn:%x,ivocAct:%x\n",Tc1_CabA_Key,Tc2_CabB_Key,pIoIn_OutputInfo->LocalVobcAttri,pIoIn_OutputInfo->MainCtrSide,pIoIn_OutputInfo->IdleSide,
                         pIoIn_OutputInfo->CtrlDigtalInputState[IN_TRAIN_EB_ENABLE],pIoIn_OutputInfo->CtrlDigtalInputState[IN_ATP_TRAIN_INTEG],pIoIn_OutputInfo->CtrlDigtalInputState[IN_TRAIN_CONFIRM_BUTTON],pIoIn_OutputInfo->CtrlDigtalInputState[IN_IVOC_VALIDITY]);
		}
	}
	return FuncRtn;
}

/***************************************************************************************
* ��������:			IO��������ݴ���,�����յ���IO���ݽ�����ŵ�IO�������ݵĽṹ����
* �������:			��
* �����������:     ��
* �������:        
* ȫ�ֱ���:         
* ����ֵ:           0x00���ɹ���
					0x01:����TC1��IO��������
					0x02:����TC1��IO��������
					0x04:����TC2��IO��������
					0x08:����TC2��IO��������
* �޸ļ�¼��		
****************************************************************************************/
UINT8 IoInPut_Api_PreiodProcess(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, OUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	UINT8 IoMainRtn = FUNC_CODE1;					/*IO����ģ��������������ֵ*/
	UINT8 CallFuncRtn = FUNC_SUCESS;				/* ���ú����ķ���ֵ */
	UINT8 FuncRtn = FUNC_SUCESS;					/* �������ķ���ֵ */
	
	if (NULL == pIoIn_OutputInfo)
	{
		FuncRtn |= FUNC_CODE1;
	}
	else
	{
		/*����IO��Ϣ*/
		(void)CommonMemSet(&gIoInOutputStru, sizeof(IO_IN_PERIOD_OUTPUT_STRUCT), (UINT8)0x00, sizeof(IO_IN_PERIOD_OUTPUT_STRUCT));
		IoMainRtn = IoModuleInPut_Api_PreiodProcess(&gIoInOutputStru);

		if (FUNC_CODE2 == (IoMainRtn & FUNC_CODE2))
		{
			/*TC1�������ͨ���жϣ�������δ�յ�TC1�������ݣ�����Ϊû�յ����п�����*/
			(void)CommonMemSet(&gIoInOutputStru.Tc1DigtalInputState, sizeof(gIoInOutputStru.Tc1DigtalInputState), 0x00U, sizeof(gIoInOutputStru.Tc1DigtalInputState));
			pIoIn_OutputInfo->IoHaveErr = CBTC_TRUE;
		}
		else
		{
			/*nothing*/
		}


		if (FUNC_CODE4 == (IoMainRtn & FUNC_CODE4))
		{
			/*TC2�������ͨ���жϣ�������δ�յ�TC2�������ݣ�����Ϊû�յ����п�����*/
			(void)CommonMemSet(&gIoInOutputStru.Tc2DigtalInputState, sizeof(gIoInOutputStru.Tc2DigtalInputState), 0x00U, sizeof(gIoInOutputStru.Tc2DigtalInputState));
		}
		else
		{
			/*nothing*/
		}

		(void)CommonMemCpy((&pIoIn_OutputInfo->Tc1DigtalInputState[0]), sizeof(UINT8)*(INPUT_DEFINE_MAX), (void*)(&gIoInOutputStru.Tc1DigtalInputState[0]), sizeof(UINT8)*(INPUT_DEFINE_MAX));
		(void)CommonMemCpy((&pIoIn_OutputInfo->Tc2DigtalInputState[0]), sizeof(UINT8)*(INPUT_DEFINE_MAX), (void*)(&gIoInOutputStru.Tc2DigtalInputState[0]), sizeof(UINT8)*(INPUT_DEFINE_MAX));

		if ((CAB_TC1 == gIoVbtcConfigDataStru.IvocTCAttri) || (CAB_TC2 == gIoVbtcConfigDataStru.IvocTCAttri)
			|| (CAB_TC0 == gIoVbtcConfigDataStru.IvocTCAttri))
		{
			CallFuncRtn = IoModuleInPutVbtc_Output(pIoIn_InputInfo, pIoIn_OutputInfo);
			if (FUNC_SUCESS != CallFuncRtn)
			{
				AppLogPrintStr(ENUM_LOG_ERR,"[IoIn]err2:%x\n", CallFuncRtn);
				FuncRtn |= FUNC_CODE4;
			}
		}
		else
		{
			/*���ƶ����ò���ȷ*/
			FuncRtn |= FUNC_CODE3;
		}

#if DARKLOG_ENABLE
		/*������־��¼*/
		DarkLogPrint(ENUM_LOG_ERR, &pIoIn_OutputInfo->MainCtrSide, DL_1B_FUNC_MAIN_CTRL_SIDE, DL_1B);
		DarkLogPrint(ENUM_LOG_ERR, &pIoIn_OutputInfo->IdleSide, DL_1B_FUNC_MAIN_IDLE_SIDE, DL_1B);
#endif
	}

	return (FuncRtn);
}

/***************************************************************************************
* ��������:			��ȡ���뿪������״̬
* �������:			tcType:1��2������
					DigtalInputIndex :��������������IOModuleExtern.h���������һ�£�
					DigtalInputState:������������Ӧ�����뿪������״̬��
* �����������:     ��
* �������:
* ȫ�ֱ���:
* ����ֵ:           0x00���ɹ���
					FUNC_CODE1:����ָ��Ϊ��
					FUNC_CODE2:�����������ݳ���Χ
					FUNC_CODE3:1/2�����Դ���
* �޸ļ�¼��
****************************************************************************************/
UINT8 IoInPut_Api_GetDigtalInput(UINT8 tcType,UINT8 DigtalInputIndex,UINT8 *DigtalInputState)
{
	UINT8 FuncRtn = FUNC_SUCESS;	/* �������ķ���ֵ */

	if (NULL != DigtalInputState)
	{
		/*�������ݺϷ���У��*/
		if ((DigtalInputIndex < IN_TRAIN_CAB_ACTIVE) || (DigtalInputIndex > IN_IVOC_VALIDITY))
		{
			/*�������ݳ���Χ*/
			FuncRtn |= FUNC_CODE2;
		}
		else
		{
			if (DATA_FROM_TC1 == tcType)
			{/*1��*/
				*DigtalInputState = gIoInOutputStru.Tc1DigtalInputState[DigtalInputIndex];
			}
			else if (DATA_FROM_TC2 == tcType)
			{/*2��*/
				*DigtalInputState = gIoInOutputStru.Tc2DigtalInputState[DigtalInputIndex];
			}
			else
			{/*1/2�����Դ���*/
				FuncRtn |= FUNC_CODE3;
			}
		}
	}
	else
	{
		FuncRtn |= FUNC_CODE1;
	}
	return FuncRtn;
}

/***************************************************************************************
* ��������:			��ȡ���Ƶȴ���
* �������:
* �����������:
* �������:			���ƶˡ��ȴ���
* ȫ�ֱ���:         gIoInOutputStru
* ����ֵ:           0x00���ɹ���
* �޸ļ�¼��		added by Jielu 20230326
****************************************************************************************/
UINT8 IoInPutVbtc_Api_GetMainCtrSide(OUT UINT8* pCtrlSide, OUT UINT8* pIdleSide)
{
	UINT8 RtnNo = FUNC_SUCESS;

	if ((NULL != pCtrlSide) && (NULL != pIdleSide))
	{
		*pCtrlSide = gIoInOutputStru.MainCtrSide;
		*pIdleSide = gIoInOutputStru.IdleSide;
	}
	else
	{
		RtnNo |= FUNC_CODE1;
	}

	return RtnNo;
}

/***************************************************************************************
* ��������:     ��ȡ��������
* �������:     ��
* �����������:  p_p8Buffer-�����buf��
* �������:     p_u16Lenʹ���˶��ٵ�buf�ֽ���
* ȫ�ֱ���:     ��ģ������ȫ�־�̬����
* ����ֵ:      FUNC_SUCESS ����
*             FUNC_CODE1  �����������
* �޸ļ�¼��
****************************************************************************************/
UINT8 IoInVbtc_Api_GetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len, UINT16 BuffMaxLen)
{
	UINT8 funRtn = FUNC_SUCESS;                                 /*��������ֵ*/
	UINT16 Index = 0u;                                           /*ѭ������*/
	UINT8 tmpBuff[APP_FOLLOW_MAXLEN] = { 0u };

	if ((NULL != p_p8Buffer) && (NULL != p_u16Len))
	{
		tmpBuff[Index] = gTc1DsnMode;
		Index += 1U;
		tmpBuff[Index] = gTc1LastDsnMode;
		Index += 1U;
		tmpBuff[Index] = gTc2DsnMode;
		Index += 1U;
		tmpBuff[Index] = gTc2LastDsnMode;
		Index += 1U;

		if (Index < BuffMaxLen)
		{
			(*p_u16Len) = Index;
			CommonMemCpy(p_p8Buffer, (UINT32)Index, (void*)&tmpBuff[0], (UINT32)Index);
			funRtn = FUNC_SUCESS;
		}
		else
		{
			(*p_u16Len) = 0;
			funRtn = FUNC_CODE2;
		}

	}
	else
	{
		funRtn = FUNC_CODE1;
	}
	return funRtn;
}

/***************************************************************************************
* ��������:     ���ø�������
* �������:     p_p8Buffer-��������buf
*             p_u16Len-����ģ�鳤��
* �����������:  ��
* �������:     ��
* ȫ�ֱ���:     ��ģ������ȫ�־�̬����
* ����ֵ:      FUNC_SUCESS ����
*             FUNC_CODE1  �����������
*             FUNC_CODE2  ����ģ�鳤�Ⱥͻ�ȡ���Ȳ�һ��
* �޸ļ�¼��
* 1.�Ƚϴ����ģ�鳤�Ⱥͻ�ȡ�ĳ���, by zwj, 20230330
****************************************************************************************/
UINT8 IoInVbtc_Api_SetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len)
{
	UINT8 FuncRtn = FUNC_SUCESS;      /*��������ֵ*/
	UINT16 Index = 0u;                /*ѭ������*/
	UINT8 *pTmpBuff = NULL;              /*������ڵ�ַ*/

	if ((NULL != p_p8Buffer) && (NULL != p_u16Len))

	{
		pTmpBuff = p_p8Buffer;

		gTc1DsnMode = pTmpBuff[Index];
		Index += 1U;
		gTc1LastDsnMode = pTmpBuff[Index];
		Index += 1U;
		gTc2DsnMode = pTmpBuff[Index];
		Index += 1U;
		gTc2LastDsnMode = pTmpBuff[Index];
		Index += 1U;

		if ((*p_u16Len) == Index)
		{
			FuncRtn = FUNC_SUCESS;
		}
		else
		{
			FuncRtn = FUNC_CODE2;
		}

	}
	else
	{
		FuncRtn = FUNC_CODE1;
	}

	return FuncRtn;
}