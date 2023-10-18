/****************************************************************************************************
* 文件名   :  IOInputVbtc.c
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2020.05.07
* 作者     :  何芊颖
* 功能描述 : IO输入解析处理函数文件
* 使用注意 : 车车通信IO模块针对不同线路的不同处理;
*
* 修改记录 : 
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

/*容忍开关量3周期*/
extern UINT8 gLastTc1_Cab_Key;
extern UINT8 Tc1_Tolerate_tic;
extern UINT8 gLastTc2_Cab_Key;
extern UINT8 Tc2_Tolerate_tic;

extern UINT8 gARReverStrTag;      /*进入AR模式折返的开始标志*/
extern UINT8 gFamDirBackTag;      /*FAM方向向后标志*/
extern UINT8 gFamCtrlSide;
extern UINT8 gFamReverStrTag;     /*进入FAM模式折返的开始标志*/

static UINT8 IoModuleInPutVbtc_Output(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, INOUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);

/***************************************************************************************
* 功能描述:			IO输入模块需要初始化的值以及读取IO配置的数据
* 输入参数:			无
* 输入输出参数:     无
* 输出参数:        
* 全局变量:         
* 返回值:           0x00:初始化成功
					0x01:读取IO配置数据失败
* 修改记录：		
****************************************************************************************/
UINT8 IoModuleVbtc_Api_PowerOnInit(IN const IO_CFG_STRU *pIoCfgStru)
{
	UINT8 RtnNo = FUNC_SUCESS;
	UINT8 FuncNo = FUNC_SUCESS;

	if (NULL != pIoCfgStru)
	{
		/*读取IO配置数据*/
		(void)CommonMemCpy(&gIoVbtcConfigDataStru, (UINT32)sizeof(IO_CFG_STRU), pIoCfgStru, (UINT32)sizeof(IO_CFG_STRU));
		FuncNo = IoModuleMscp_Api_PowerOnInit(&gIoVbtcConfigDataStru.IoConfigDataStru);
		if (FUNC_SUCESS != FuncNo)
		{
			/*IO数据初始化失败*/
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
* 功能描述:			输出信息处理
* 输入参数:			
* 输入输出参数:     无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
* 修改记录：
* 1.一机控双端，两端都为控制端时增加控制端的开关量，by zy 20210425
* 2.修复YF0069-2262 未对指针防空判断 by zy 20220809
****************************************************************************************/
static UINT8 IoModuleInPutVbtc_Output(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, INOUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	UINT8 FuncRtn = FUNC_SUCESS;
	UINT8 Tc1_CabA_Key = CBTC_FALSE;	/*1端/A端钥匙*/
	UINT8 Tc2_CabB_Key = CBTC_FALSE;	/*2端/B端钥匙*/
	/*迪士尼线路需要用的变量*/
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
	    /*车门关闭且锁闭分开采集时，收到车门关闭有效和锁闭有效，则赋值“车门关闭且锁闭”有效*/
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
	        /*不处理*/
	    }

		/*Line Disney*/
		if (LINE_DISNEY == gIoVbtcConfigDataStru.LineConfig)
		{
		    /*迪士尼新开关量配置(左门开左门关、右门开右门关用同一个点位表示，且高电位表示开门，低点位表示关门)*/
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
			/*一主机控单端的时候，IO的数默认放在Tc1里，此处需做处理,若本端是2端，将开关量存进Tc2结构体中  hqy 2020-05-09*/
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
				/*无需处理*/
			}
#endif

			/*TC1*/
			mNOTSDMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_NONSD_MODE];
			mCMMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_CM_MODE];
			mFORMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_FWD_MODE];
			mREVMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_REV_MODE];
			mSDMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_SD_MODE];
			mSBMode = pIoIn_OutputInfo->Tc1DigtalInputState[IN_STBY_MODE];
			/*获取当前模式开关*/
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
				/*不处理*/
			}

			/*模式开关切换时需持续超过配置周期后才能切换*/
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
				/*方向手柄向前*/
				pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DIR_FORWARD] = CBTC_TRUE;
				pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_DIR_BACKWARD] = CBTC_FALSE;
			}
			else if(IN_REV_MODE == gTc1DsnMode)
			{
				Tc1_CabA_Key = CBTC_TRUE;
				/*方向手柄向后*/
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
			/*获取当前模式开关*/
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
				/*不处理*/
			}

			/*模式开关切换时需持续超过配置周期后才能切换*/
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
				/*方向手柄向前*/
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_FORWARD] = CBTC_TRUE;
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_BACKWARD] = CBTC_FALSE;
			}
			else if(IN_REV_MODE == gTc2DsnMode)
			{
				Tc2_CabB_Key = CBTC_TRUE;
				/*方向手柄向后*/
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_BACKWARD] = CBTC_TRUE;
				pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_DIR_FORWARD] = CBTC_FALSE;
			}
			else
			{
				Tc2_CabB_Key = CBTC_FALSE;
			}

			/*列车完整性 迪士尼没有列车完整性开关量 所以默认不会丢失完整性 db 20220711*/
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
			/*获取1端/A端钥匙*/
			Tc1_CabA_Key = pIoIn_OutputInfo->Tc1DigtalInputState[IN_TRAIN_CAB_ACTIVE];
			/*获取2端/B端钥匙*/
			Tc2_CabB_Key = pIoIn_OutputInfo->Tc2DigtalInputState[IN_TRAIN_CAB_ACTIVE];
		}
		
		if (ONE_CTRL_TWO == gIoVbtcConfigDataStru.IVOCCtrlType)
		{
			/*一主机控两端*/
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
				/*两端都为控制端时，任选一端的开关量作为控制端的开关量(发送给MMI的门状态需要从控制端获取，所以做此处理)*/
			    (void)CommonMemCpy((&pIoIn_OutputInfo->CtrlDigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX, (void*)(&pIoIn_OutputInfo->Tc1DigtalInputState), sizeof(UINT8)*INPUT_DEFINE_MAX);
			}
			else
			{
			    /*不填写控制端、非控制端开关量*/
			}
		}
		else
		{
			/*一主机控单端*/
			if ((CAB_TC1 == gIoVbtcConfigDataStru.IvocTCAttri) && (CBTC_TRUE == Tc1_CabA_Key))
			{
				/*当前端为TC1端且TC1采集到驾驶台钥匙打开；或当前端为TC2端且TC2采集到驾驶台钥匙打开，当前端为控制端*/
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
				/*当前端为TC1端且TC1采集到驾驶台钥匙打开；或当前端为TC2端且TC2采集到驾驶台钥匙打开，当前端为控制端*/
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
				/*当前端为等待端*/
				pIoIn_OutputInfo->LocalVobcAttri = IDLE_IVOC;
				/*这里不清楚为什么非要给个控制端*/
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
				/*对端和本端都是控制端*/
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
* 功能描述:			IO输入的数据处理,将接收到的IO数据解析存放到IO输入数据的结构体中
* 输入参数:			无
* 输入输出参数:     无
* 输出参数:        
* 全局变量:         
* 返回值:           0x00：成功；
					0x01:接收TC1端IO报文有误
					0x02:处理TC1端IO报文有误
					0x04:接收TC2端IO报文有误
					0x08:处理TC2端IO报文有误
* 修改记录：		
****************************************************************************************/
UINT8 IoInPut_Api_PreiodProcess(IN const IO_IN_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo, OUT IO_IN_VBTC_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo)
{
	UINT8 IoMainRtn = FUNC_CODE1;					/*IO公共模块主调函数返回值*/
	UINT8 CallFuncRtn = FUNC_SUCESS;				/* 调用函数的返回值 */
	UINT8 FuncRtn = FUNC_SUCESS;					/* 本函数的返回值 */
	
	if (NULL == pIoIn_OutputInfo)
	{
		FuncRtn |= FUNC_CODE1;
	}
	else
	{
		/*接收IO信息*/
		(void)CommonMemSet(&gIoInOutputStru, sizeof(IO_IN_PERIOD_OUTPUT_STRUCT), (UINT8)0x00, sizeof(IO_IN_PERIOD_OUTPUT_STRUCT));
		IoMainRtn = IoModuleInPut_Api_PreiodProcess(&gIoInOutputStru);

		if (FUNC_CODE2 == (IoMainRtn & FUNC_CODE2))
		{
			/*TC1输入插箱通信中断（当周期未收到TC1输入数据），认为没收到所有开关量*/
			(void)CommonMemSet(&gIoInOutputStru.Tc1DigtalInputState, sizeof(gIoInOutputStru.Tc1DigtalInputState), 0x00U, sizeof(gIoInOutputStru.Tc1DigtalInputState));
			pIoIn_OutputInfo->IoHaveErr = CBTC_TRUE;
		}
		else
		{
			/*nothing*/
		}


		if (FUNC_CODE4 == (IoMainRtn & FUNC_CODE4))
		{
			/*TC2输入插箱通信中断（当周期未收到TC2输入数据），认为没收到所有开关量*/
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
			/*控制端配置不正确*/
			FuncRtn |= FUNC_CODE3;
		}

#if DARKLOG_ENABLE
		/*暗文日志记录*/
		DarkLogPrint(ENUM_LOG_ERR, &pIoIn_OutputInfo->MainCtrSide, DL_1B_FUNC_MAIN_CTRL_SIDE, DL_1B);
		DarkLogPrint(ENUM_LOG_ERR, &pIoIn_OutputInfo->IdleSide, DL_1B_FUNC_MAIN_IDLE_SIDE, DL_1B);
#endif
	}

	return (FuncRtn);
}

/***************************************************************************************
* 功能描述:			获取输入开关量的状态
* 输入参数:			tcType:1、2端属性
					DigtalInputIndex :输入索引，需与IOModuleExtern.h定义的索引一致；
					DigtalInputState:与输入索引对应的输入开关量的状态。
* 输入输出参数:     无
* 输出参数:
* 全局变量:
* 返回值:           0x00：成功；
					FUNC_CODE1:输入指针为空
					FUNC_CODE2:输入索引数据超范围
					FUNC_CODE3:1/2端属性错误
* 修改记录：
****************************************************************************************/
UINT8 IoInPut_Api_GetDigtalInput(UINT8 tcType,UINT8 DigtalInputIndex,UINT8 *DigtalInputState)
{
	UINT8 FuncRtn = FUNC_SUCESS;	/* 本函数的返回值 */

	if (NULL != DigtalInputState)
	{
		/*索引数据合法性校验*/
		if ((DigtalInputIndex < IN_TRAIN_CAB_ACTIVE) || (DigtalInputIndex > IN_IVOC_VALIDITY))
		{
			/*索引数据超范围*/
			FuncRtn |= FUNC_CODE2;
		}
		else
		{
			if (DATA_FROM_TC1 == tcType)
			{/*1端*/
				*DigtalInputState = gIoInOutputStru.Tc1DigtalInputState[DigtalInputIndex];
			}
			else if (DATA_FROM_TC2 == tcType)
			{/*2端*/
				*DigtalInputState = gIoInOutputStru.Tc2DigtalInputState[DigtalInputIndex];
			}
			else
			{/*1/2端属性错误*/
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
* 功能描述:			获取控制等待端
* 输入参数:
* 输入输出参数:
* 输出参数:			控制端、等待端
* 全局变量:         gIoInOutputStru
* 返回值:           0x00：成功；
* 修改记录：		added by Jielu 20230326
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
* 功能描述:     获取跟随数据
* 输入参数:     无
* 输入输出参数:  p_p8Buffer-打包的buf；
* 输出参数:     p_u16Len使用了多少的buf字节数
* 全局变量:     本模块所有全局静态变量
* 返回值:      FUNC_SUCESS 正常
*             FUNC_CODE1  输入参数错误
* 修改记录：
****************************************************************************************/
UINT8 IoInVbtc_Api_GetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len, UINT16 BuffMaxLen)
{
	UINT8 funRtn = FUNC_SUCESS;                                 /*函数返回值*/
	UINT16 Index = 0u;                                           /*循环变量*/
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
* 功能描述:     设置跟随数据
* 输入参数:     p_p8Buffer-传入数的buf
*             p_u16Len-输入模块长度
* 输入输出参数:  无
* 输出参数:     无
* 全局变量:     本模块所有全局静态变量
* 返回值:      FUNC_SUCESS 正常
*             FUNC_CODE1  输入参数错误
*             FUNC_CODE2  输入模块长度和获取长度不一致
* 修改记录：
* 1.比较传入的模块长度和获取的长度, by zwj, 20230330
****************************************************************************************/
UINT8 IoInVbtc_Api_SetFollowData(UINT8* p_p8Buffer, UINT16 *p_u16Len)
{
	UINT8 FuncRtn = FUNC_SUCESS;      /*函数返回值*/
	UINT16 Index = 0u;                /*循环变量*/
	UINT8 *pTmpBuff = NULL;              /*保存入口地址*/

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