/*
 * PLFM_CycleTime_Config.h
 *
 *  Created on: 2023年1月5日
 *      Author: Author
 */

#ifndef SRC_PLFM_NVRAM_PLFM_CYCLETIME_CONFIG_H_
#define SRC_PLFM_NVRAM_PLFM_CYCLETIME_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "PROJECT_OR_BOARD_PTCL.h"
#include "CommonTypes.h"
#include "sfpplatform.h"
#include "CM_Types.h"

#define IPB_DEFAULT_MAINCYCLETIME    (100u)      /** 配置SFP参数时需要使用的输入板默认周期时间*/
#define OPB_DEFAULT_MAINCYCLETIME    (100u)      /** 配置SFP参数时需要使用的输出板默认周期时间*/
#define SPB_DEFAULT_MAINCYCLETIME    (200u)      /** 配置SFP参数时需要使用的信号板默认周期时间*/

#define DMI_SFP_DEVICE_TYPE     (0x71u)     /** SFP配置表中DMI的设备类型*/
#define SPB_SFP_DEVICE_TYPE     (0x40u)     /** SFP配置表中SPB的设备类型*/
#define IPB_SFP_DEVICE_TYPE     (0x90u)     /** SFP配置表中IPB的设备类型*/
#define OPB_SFP_DEVICE_TYPE     (0xB0u)     /** SFP配置表中OPB的设备类型*/


/** 板卡周期时间片配置结构体*/
typedef struct st_CycleTimeConfig
{
    UINT32  MainCycleTime;              /** 一个周期的总时间*/
    UINT32  InputMicroCycleTime;        /** 输入微周期分配的时间*/
    UINT32  AppMicroCycleTime;          /** 应用微周期分配的时间*/
    UINT32  OutputMicroCycleTime;       /** 输出微周期分配的时间*/
    UINT32  MasterSyncSysComCycleTime;  /** 主备同步系间通信阶段分配的时间*/
    UINT32  MasterSyncTime;             /** 主备同步微周期其他阶段分配的时间*/
    UINT32  SendMicroCycleTime;         /** 发送微周期分配的时间*/
    UINT32  IdleMicroCycleTime;         /** 空闲微周期分配的时间*/
}ST_CYCLETIMECONFIG;

/** 主机板上使用的-各执行板卡当前的周期总时间配置结构体*/
typedef struct st_BoardMainCycleTimeConfig
{
    UINT32  IPB_MainCycleTime;          /** 输入板周期总时间*/
    UINT32  OPB_MainCycleTime;          /** 输出板周期总时间*/
    UINT32  SPB_MainCycleTime;          /** 信号板周期总时间*/
}ST_BOARD_MAINCYCLE_TIME_CONFIG;

UINT32 Plat_HardSync_GetCurrWorkCycle();

extern ST_CYCLETIMECONFIG g_PLFM_CycleTime_Config;

extern struc_Unify_Info sfpStru;

extern UINT32 g_TimeCounter; /*用SFP协议 计数*/

extern UINT32 m_worktime;

#ifdef __cplusplus
}
#endif

#endif /* SRC_PLFM_NVRAM_PLFM_CYCLETIME_CONFIG_H_ */
