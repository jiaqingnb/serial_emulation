/*
 * PLFM_CycleTime_Config.c
 *
 *  Created on: 2023年1月5日
 *      Author: Author
 */
#include "PROJECT_OR_BOARD_PTCL.h"
#include "PLFM_CycleTime_Config.h"
#include "CommonTypes.h"

UINT32 m_worktime = 0;

UINT32 Plat_HardSync_GetCurrWorkCycle()
{
    return m_worktime;
}

extern UINT8 vSfpInnetDataBuf[];

ST_CYCLETIMECONFIG g_PLFM_CycleTime_Config = {(UINT32)MAINCYCLE,(UINT32)INPUTPROC_TIME_END_DELAY,(UINT32)APPLOGICPROC_TIME_END_DELAY,(UINT32)OUTPUTPROC_TIME_END_DELAY,(UINT32)MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY,(UINT32)MASTERSLAVERSYNC_TIME_END_DELAY,(UINT32)SENDPROC_TIME_END_DELAY,(UINT32)IDLEPROC_TIME_END_DELAY};


/** 计算板卡配置的周期时间和默认的周期时间之间比值的函数*/
UINT8 CalcScale(UINT32 pConfigCycleTime, UINT32 pDefaultCycleTime)
{
    UINT8 Scale = 0u;
    /** 如果配置时间大于缺省时间*/
    if(pConfigCycleTime > pDefaultCycleTime)
    {
        Scale = (UINT8)(pConfigCycleTime/pDefaultCycleTime);
    }
    /** 如果配置时间小于等于缺省时间*/
    else
    {
        Scale = (UINT8)(pDefaultCycleTime/pConfigCycleTime);
    }
    return Scale;
}

/** 根据板卡周期比值计算SFP配置表参数函数*/
void CalcSFPConfigData(UINT32 ConfigMainCycleTime, UINT32 DefaultMainCycleTime, UINT16 OffsetOfCycleTime, UINT16 OffsetOfOnePackMinCycleNum)
{
    UINT8 Scale = 0u;
    /** 如果配置的周期时间小于默认的周期时间*/
    if(ConfigMainCycleTime < DefaultMainCycleTime)
    {
        Scale = (UINT8)(DefaultMainCycleTime / ConfigMainCycleTime);
        vSfpInnetDataBuf[OffsetOfCycleTime] = vSfpInnetDataBuf[OffsetOfCycleTime] / Scale;
        vSfpInnetDataBuf[OffsetOfOnePackMinCycleNum] = vSfpInnetDataBuf[OffsetOfOnePackMinCycleNum] * Scale;
    }
    /** 如果配置的周期时间大于默认的周期时间*/
    else
    {
        Scale = (UINT8)(ConfigMainCycleTime / DefaultMainCycleTime);
        vSfpInnetDataBuf[OffsetOfCycleTime] = vSfpInnetDataBuf[OffsetOfCycleTime] * Scale;
        vSfpInnetDataBuf[OffsetOfOnePackMinCycleNum] = vSfpInnetDataBuf[OffsetOfOnePackMinCycleNum] / Scale;
    }
}

/** 根据板卡周期时间修改SFP协议配置参数的函数*/
void SetSFPConfigData(void)
{
    UINT8   deviceNum = 0u;
    UINT8   deviceType = 0u;
    UINT16   i = 0u;
    UINT16  offsetOfDeviceType = 0u;
    UINT16  offsetOfCycleTime = 0u;
    UINT16  offsetOfOnePackMinCycleNum = 0u;
    /** 从SFP配置表中获取设备数量*/
    deviceNum = vSfpInnetDataBuf[1u];
    /** 根据设备数量遍历SFP配置表*/
    for(i = 0u;i < deviceNum;i++)
    {
        offsetOfDeviceType  = 1u + (15u * i) + 9u;
        offsetOfCycleTime   = 1u + (15u * i) + 12u;
        offsetOfOnePackMinCycleNum  = 1u + (15u * i) + 15u;
        /** 获取SFP配置表中的设备类型*/
        deviceType = vSfpInnetDataBuf[offsetOfDeviceType];
        switch(deviceType)
        {
        case    IPB_SFP_DEVICE_TYPE:    /** 如果设备类型是输入板*/
            /** 获取配置的输入板周期时间*/
#if (1u == IS_5728_MCP)
            CalcSFPConfigData(g_Board_MainCycle_Time_Config.IPB_MainCycleTime, IPB_DEFAULT_MAINCYCLETIME, offsetOfCycleTime, offsetOfOnePackMinCycleNum);
#endif
#if (1u == IS_570_IPB)
            CalcSFPConfigData(g_PLFM_CycleTime_Config.MainCycleTime, IPB_DEFAULT_MAINCYCLETIME, offsetOfCycleTime, offsetOfOnePackMinCycleNum);
#endif
            break;
        case    OPB_SFP_DEVICE_TYPE:    /** 如果设备类型是输出板*/
            /** 获取配置的输出板周期时间*/
#if (1u == IS_5728_MCP)
            CalcSFPConfigData(g_Board_MainCycle_Time_Config.OPB_MainCycleTime, OPB_DEFAULT_MAINCYCLETIME, offsetOfCycleTime, offsetOfOnePackMinCycleNum);
#endif
#if (1u == IS_570_OPB)
            CalcSFPConfigData(g_PLFM_CycleTime_Config.MainCycleTime, OPB_DEFAULT_MAINCYCLETIME, offsetOfCycleTime, offsetOfOnePackMinCycleNum);
#endif
            break;
        case    DMI_SFP_DEVICE_TYPE:    /** 如果设备类型是MMI*/
            /** 获取配置的信号板周期时间*/
#if (1u == IS_5728_MCP)
            CalcSFPConfigData(g_Board_MainCycle_Time_Config.SPB_MainCycleTime, SPB_DEFAULT_MAINCYCLETIME, offsetOfCycleTime, offsetOfOnePackMinCycleNum);
#endif
#if (1u == IS_570_SIG)
            CalcSFPConfigData(g_PLFM_CycleTime_Config.MainCycleTime, SPB_DEFAULT_MAINCYCLETIME, offsetOfCycleTime, offsetOfOnePackMinCycleNum);
#endif
            break;
        default:
            /** 均不符合 */
            break;
        }
    }
}



