/****************************************************************************************************
* 文件名   :  IOOutputVbtc.c
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

extern UINT8 gARReverStrTag;      /*进入AR模式折返的开始标志*/
extern UINT8 gFamDirBackTag;      /*FAM方向向后标志*/
extern UINT8 gFamCtrlSide;
extern UINT8 gFamReverStrTag;     /*进入FAM模式折返的开始标志*/
/***************************************************************************************
* 功能描述:			IO输出去的数据处理,将接收到的IO数据解析存放到IO输入数据的结构体中
* 输入参数:			IO_OUT_VBTC_PERIOD_INPUT_STRUCT *pIoIn_InputInfo
* 输入输出参数:     无
* 输出参数:        
* 全局变量:         
* 返回值:           
0x00：成功；
0x01:
* 修改记录：		
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
	        /*前10周期向输出板输出全0，防止主机板与AB系输出板建链速度不一致，导致输出板两系输出不一致，单系输出板宕机的情况。*/
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
	            /*迪士尼开关量需要做一个转换*/
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
	                /*影子模式下，IVOC输出导向安全侧，包括持续输出紧急制动(在EB模块处理)、非零速、无左右门使能*/
	                /*零速信号输出*/
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutAtpZeroSpeedsig = CBTC_FALSE;
	                /*左门使能*/
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutAtpLeftDoorEn = CBTC_FALSE;
	                /*右门使能*/
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutAtpRightDoorEn = CBTC_FALSE;
	            }
	            else
	            {
	                /*nothing*/
	            }
	        }
	        else if (LINE_COMMON == gIoVbtcConfigDataStru.LineConfig)
	        {
	            /*北京11输出开关量转换*/
	            /*控制端*/
	            /*紧急制动输出*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutEb = CBTC_ENABLE;
	            }

	            /*牵引切除*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutTractionCut)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutTractionCut = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruCtrl.OutTractionCut = CBTC_ENABLE;
	            }

	            /*等待端*/
	            /*紧急制动输出*/
	            if (CBTC_ENABLE == pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutEb)
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutEb = CBTC_DISABLE;
	            }
	            else
	            {
	                pIoIn_InputInfo->IoOut_InputInfo.IOOutputStruIdle.OutEb = CBTC_ENABLE;
	            }

	            /*牵引切除*/
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

	        /*记住刚进入FAM折返的控制端*/
	        if ((CBTC_TRUE == pIoIn_InputInfo->IoOut_InputInfo.FAMReverseTag)&&(CBTC_FALSE ==gFamReverStrTag))
	        {
	            gFamCtrlSide = pIoIn_InputInfo->IoOut_InputInfo.MainCtrlSide;
	            gFamReverStrTag = CBTC_TRUE;
	        }
	        else
	        {
	            /*nothing*/
	        }

	        /*记住刚进入AR折返的控制端*/
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
* 功能描述:			获取FAM方向向后标志
* 输入参数:			无
* 输入输出参数:     无
* 输出参数:			无
* 全局变量:			gFamDirBackTag
* 返回值:			FAM方向向后标志
* 修改记录：		新增，wyd 20230217
****************************************************************************************/
UINT8 IoVbtcOut_Api_GetFamDirBack(void)
{
	return gFamDirBackTag;
}
