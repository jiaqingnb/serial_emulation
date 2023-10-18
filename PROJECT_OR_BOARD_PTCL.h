#ifndef PROJECT_OR_BOARD_PTCL_H
#define PROJECT_OR_BOARD_PTCL_H

/*BTMRssp-1协议初始化使用的ID和端 */
#define BTM_SELF_TYPE_PTL (0x14)
#define BTM_SELF_ID_PTL (0x00)
#define BTM_SELF_LOGICID_PTL (0x00) /* 用于外网协议哪一端*/
/*关闭超时检测开关*/
//#define CLOSE_MICRO_CYCLE_TIMEOUT_CHECK 0
/*版本发布日期*/
#define CONF_VER_YEAR           (2023u)      /*年*/
#define CONF_VER_MONTH          (8u)        /*月*/
#define CONF_VER_DAY            (30u)        /*日*/
/*2X2oo2平台软件版本号信息*/
/*版本号定义使用10进制*/
#define CONF_VER_PRODUCT_NO     (90u)      /*产品编号*/
#define CONF_VER_SOFTWARE_NO    (19u)      /*软件模块编号*/
#define CONF_VER_MAIN_NO        (8u)       /*软件模块主版本号*/
#define CONF_VER_SEC_NO         (2u)       /*软件模块子版本号*/

/*集成版本号-版本发布日期*/
#define IMP_VER_YEAR           (2023u)      /*年*/
#define IMP_VER_MONTH          (8u)        /*月*/
#define IMP_VER_DAY            (30u)        /*日*/
/*2X2oo2平台软件版本号信息*/
/*版本号定义使用10进制*/
#define IMP_VER_PROJECT_NO     (0u)       /*研发项目公共*/
#define IMP_VER_PRODUCT_NO     (90u)      /*产品编号*/
#define IMP_VER_EXE_NO         (6u)       /*可执行文件编号*/
#define IMP_VER_TEG_NO         (42u)       /*集成编号*/
/*平台2X2功能集开关*/
#define PLFM_2X2FUNC_SWITCH   (0u)

/*平台单端双控开关-单端单控0-单端双控或无头尾贯通标志1*/
#define PLFM_SINGLE_END_DOUBLE_CONTROL_SWITCH   (0u)
/*头尾端1588协议时间同步开关，开：1，关：0*/
#define PLFM_1588SYNTIME_SWITCH        (0u)

/*打印开关*/
/*与应用接口打印*/
#define WINLOGFORMAT_SWITCH            (0u)
#define WINLOGFORMAT_TIME_SWITCH       (0u)
#define PRINTSCREEN_APP_SWITCH         (0u)
#define PRINTSCREENARRAY_APP_SWITCH    (0u)
#define PRINTSCREEN_TEST_SWITCH        (0u)
#define PRINTSCREENARRAY_TEST_SWITCH   (0u)
#define PRINTSCREENQUEUE_APP_SWITCH    (0u)
/*平台打印*/
#define PRINTSCREEN_1_SWITCH           (0u)
#define PRINTSCREEN_2_SWITCH           (0u)
#define PRINTSCREEN_4_SWITCH           (1u)
/*平台数组打印*/
#define PRINTSCREENARRAY_1_SWITCH      (0u)
#define PRINTSCREENARRAY_2_SWITCH      (0u)
#define PRINTSCREENARRAY_4_SWITCH      (1u)

/*
  MASTER==1时，PLFM_PlatformVer.h中会定义 MY_PLATFORM_IS_2X2OO2
  MASTER==0时，PLFM_PlatformVer.h中会定义 MY_PLATFORM_IS_2OO2
*/
#define MASTER (0u)    
/*
	IS_NEED_FUNC_MONITOR==1时，开启监控功能部分代码
	IS_NEED_FUNC_MONITOR==0时，关闭监控功能部分代码
*/
#define IS_NEED_FUNC_MONITOR (0u)
/*
	IS_MONITOR==1时，监控功能为监控机
*/
#define IS_MONITOR        (0u)
/*
	BE_MONITOR==1时，监控功能为被检控方
*/
#define BE_MONITOR        (0u)
/*
	SEND_REQUEST==1时，打开发送请求消息部分代码
*/
#define SEND_REQUEST      (0u)

/*
	SEND_RESPOND==1时，打开接收请求并相应数据部分代码
*/
#define SEND_RESPOND      (1u)

/*
	5728_1G_DDR==1时，表示在使用57281_1G DDR版本，此时无需考虑内存问题
*/
#define IS_5728_1G_DDR        (0u)
/*
	IS_5728_NORMAL_INCLUDE==1时，表示使用小型化——5728主机得inlude路径
*/
#define IS_5728_NORMAL_INCLUDE   (0u)
/*
	IS_570_NORMAL_INLCLUDE==1时，表示使用小型化——570主机得inlude路径
*/
#define IS_570_NORMAL_INLCLUDE   (1u)
/*
	IS_570_Tranparenet_RCV_FUN==1时，570接收5728串口数据具有透传功能
*/
#define IS_570_Tranparenet_RCV_FUN (1u)
/*
                 日志打印开关
*/
#define IS_570_LOG  (0u)
/*
                平台主机板开关
*/
#define IS_5728_MCP (0u)
#if IS_5728_MCP == 1
#define APP_BOARD_NAME "\r\n#### ATP \r\n"
#endif
/*
                平台输入板开关
*/
#define IS_570_IPB  (0u)
#if IS_570_IPB ==1
#define APP_BOARD_NAME "\r\n#### IPB\r\n"
#endif
/*
               平台输出板开关
*/
#define IS_570_OPB  (0u)
#if IS_570_OPB == 1
#define APP_BOARD_NAME "\r\n#### OPB \r\n"
#endif
/*
               平台信号板开关
*/
#define IS_570_SIG  (1u)
#if IS_570_SIG == 1
#define APP_BOARD_NAME "\r\n#### SPB \r\n"
#endif
/*
                平台信号板-B-外置SDRAM开关，如果是-B板卡必须置为1，如果是-A板卡置为0
*/
#define USED_SDRAM  (1u)
/*
                平台信号板时间同步开关
*/
#define IS_570_SYN_TIME (1u)
/*
                平台信号板差分站开关，1表示差分站信号板，0表示非差分站信号板
*/
#define IS_570_DIFF_STATION (0u)
/*
                平台主机板差分站开关，1表示差分站主机板，0表示非差分站主机板
*/
#define IS_5728_DIFF_STATION (0u)

//#define TCT_DEBUG_SELFCHEK /*自检宏*/

#define PLFM_NVRAM_CHECK    (1u)    /*NVRAM自检开关*/

#define IN_SYNC  (0u)   /*输入同步开关*/

#define MULTI_THREAD    (0u)    /*多线程开关*/
/*********************************************************************************************/
/*以下为OC宏，使用OC板卡，请把OC相关宏打卡
*OC主机板，需要打开小型化工程主机板的相关宏
*OC执行板卡，需要打开小型化输入板工程的相关宏
*
*/

//#define SFP_VERSION (1)
#define MiNIATURIZATION_MASTER
//#define OC_MASTER
//#define OC_IPB
//#define OC_OPB
//#define OC_SIG_CTRL
//#define OC_SWITCH_CTRL_5
//#define OC_SWITCH_CTRL_4
//#define OC_PDTC
#define MiNIATURIZATION_ALL
//#define OC_ALL

//#define PSC_MASTER/* 站台门控制器主机板 */
//#define PSC_OPB/* 站台门控制器切系仲裁板 */
#define USEPWL

#if defined(OC_SWITCH_CTRL_5)
    #define BOARD_SWITCH_CTRL_5
    #define APP_BOARD_NAME "\r\n#### SWITCH_5 \r\n"
#endif

#if defined(OC_IPB)
    #define BOARD_IPB
    #define APP_BOARD_NAME "\r\n#### IPB ######"
#endif

#if defined(OC_OPB)
    #define BOARD_OPB
    #define APP_BOARD_NAME "\r\n#### OPB \r\n"
#endif

#if defined(OC_SIG_CTRL)
    #define BOARD_SIG_CTRL
    #define APP_BOARD_NAME "\r\n#### SIG \r\n"
#endif

#if defined(OC_SWITCH_CTRL_4)
    #define BOARD_SWITCH_CTRL_4
    #define APP_BOARD_NAME "\r\n#### SWITCH_4 \r\n"
#endif

#if defined(OC_PDTC)
    #define BOARD_PDTC
    #define APP_BOARD_NAME "\r\n#### PDTC \r\n"
#endif


#if defined(PSC_OPB)
    #define BOARD_OPB
    #define APP_BOARD_NAME "\r\n#### PSC_OPB \r\n"
#endif


/*平台主周期时间宏定义*/
#if IS_5728_MCP == 1
    #if defined(OC_MASTER)
        #define MAINCYCLE     300  /*平台周期最大时间*/
        #define INPUTPROC_TIME_END_DELAY         40                                  /*输入处理微周期：30MS*/
        #define APPLOGICPROC_TIME_END_DELAY      (INPUTPROC_TIME_END_DELAY+90)       /*应用处理微周期：50MS*/
        #define OUTPUTPROC_TIME_END_DELAY        (APPLOGICPROC_TIME_END_DELAY+20)    /*输出处理微周期：20MS*/
        #define MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY  (OUTPUTPROC_TIME_END_DELAY+20)       /*主备同步微周期：70MS*/
        #define MASTERSLAVERSYNC_TIME_END_DELAY  (MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY+30)       /*主备同步微周期：30MS*/
        #define SENDPROC_TIME_END_DELAY          (MASTERSLAVERSYNC_TIME_END_DELAY+60) /*发送处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
        #define IDLEPROC_TIME_END_DELAY          (SENDPROC_TIME_END_DELAY+40) /*空闲处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
        #define SENDPROC_TIME_OVER_DELAY         0
        #define IDLEPROC_TIME_OVER_DELAY         0
    #else
        #define MAINCYCLE     200  /*平台周期最大时间*/
        #define INPUTPROC_TIME_END_DELAY         45                                  /*输入处理微周期：40MS*/
        #define APPLOGICPROC_TIME_END_DELAY      (INPUTPROC_TIME_END_DELAY+45)       /*应用处理微周期：60MS*/
        #define OUTPUTPROC_TIME_END_DELAY        (APPLOGICPROC_TIME_END_DELAY+15)    /*输出处理微周期：15MS*/
        #define MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY        (OUTPUTPROC_TIME_END_DELAY+20)    /*主备同步处理微周期-系间通信阶段：20MS*/
        #define MASTERSLAVERSYNC_TIME_END_DELAY  (MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY+35)       /*主备同步微周期：35MS*/
        #define SENDPROC_TIME_END_DELAY          (MASTERSLAVERSYNC_TIME_END_DELAY+15+5) /*发送处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
        #define IDLEPROC_TIME_END_DELAY          (SENDPROC_TIME_END_DELAY+20) /*空闲处理微周期：20MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
        #define SENDPROC_TIME_OVER_DELAY         0
        #define IDLEPROC_TIME_OVER_DELAY         0
    #endif
#endif

#if IS_570_IPB == 1
#define MAINCYCLE     100  /*平台周期时间*/
#define INPUTPROC_TIME_END_DELAY         20                                  /*输入处理微周期：15MS*/
#define APPLOGICPROC_TIME_END_DELAY      (INPUTPROC_TIME_END_DELAY+20)       /*应用处理微周期：10MS*/
#define OUTPUTPROC_TIME_END_DELAY        (APPLOGICPROC_TIME_END_DELAY+20)    /*输出处理微周期：5MS*/
#define MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY  (OUTPUTPROC_TIME_END_DELAY+0)       /*主备同步微周期：0MS*/
#define MASTERSLAVERSYNC_TIME_END_DELAY  (MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY+0)
#define SENDPROC_TIME_END_DELAY          (MASTERSLAVERSYNC_TIME_END_DELAY+20) /*发送处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
#define IDLEPROC_TIME_END_DELAY          (SENDPROC_TIME_END_DELAY+20) /*空闲处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
#define SENDPROC_TIME_OVER_DELAY         20
#define IDLEPROC_TIME_OVER_DELAY         20
#endif

#if IS_570_OPB == 1
#define MAINCYCLE     100  /*平台周期时间：50ms*/
#define INPUTPROC_TIME_END_DELAY         25                                  /*输入处理微周期15：MS*/
#define APPLOGICPROC_TIME_END_DELAY      (INPUTPROC_TIME_END_DELAY+20)       /*应用处理微周期：10MS*/
#define OUTPUTPROC_TIME_END_DELAY        (APPLOGICPROC_TIME_END_DELAY+25)    /*输出处理微周期：5MS*/
#define MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY  (OUTPUTPROC_TIME_END_DELAY+0)       /*主备同步微周期：0MS*/
#define MASTERSLAVERSYNC_TIME_END_DELAY  (MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY+0)       /*主备同步微周期：0MS*/
#define SENDPROC_TIME_END_DELAY          (MASTERSLAVERSYNC_TIME_END_DELAY+20) /*发送处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
#define IDLEPROC_TIME_END_DELAY          (SENDPROC_TIME_END_DELAY+10) /*空闲处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
#define SENDPROC_TIME_OVER_DELAY         0
#define IDLEPROC_TIME_OVER_DELAY         0

#endif
#if IS_570_SIG == 1
#define MAINCYCLE     200  /*平台周期时间*/
#define INPUTPROC_TIME_END_DELAY         60                                  /*输入处理微周期15：MS*/
#define APPLOGICPROC_TIME_END_DELAY      (INPUTPROC_TIME_END_DELAY+20)       /*应用处理微周期：10MS*/
#define OUTPUTPROC_TIME_END_DELAY        (APPLOGICPROC_TIME_END_DELAY+20)    /*输出处理微周期：5MS*/
#define MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY  (OUTPUTPROC_TIME_END_DELAY+0)       /*主备同步微周期：0MS*/
#define MASTERSLAVERSYNC_TIME_END_DELAY  (MASTERSLAVERSYNC_SYSCOMMSTEP_TIME_END_DELAY+0)       /*主备同步微周期：0MS*/
#define SENDPROC_TIME_END_DELAY          (MASTERSLAVERSYNC_TIME_END_DELAY+40) /*发送处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
#define IDLEPROC_TIME_END_DELAY          (SENDPROC_TIME_END_DELAY+60) /*空闲处理微周期：10MS*//*预留10ms给空闲微周期：保证接收完数据==发送接收数据预留20ms*/
#define SENDPROC_TIME_OVER_DELAY         0
#define IDLEPROC_TIME_OVER_DELAY         0
#endif


/*日志记录类型开关*/
#define ERR_LOG         (1u)
#define WARNING_LOG     (1u)
#define PROMPT_LOG      (1u)
#define DEBUG_LOG       (1u)

/*日志记录应用接口开关*/
#define APP_LOG         (1u)
#define MID_LOG         (1u)
#define DRV_LOG         (1u)
#define PLFM_LOG        (1u)

/*暗文日志记录类型开关*/
#define DARKERR_LOG         (1u)
#define DARKWARNING_LOG     (1u)
#define DARKPROMPT_LOG      (1u)
#define DARKDEBUG_LOG       (1u)

/*明文日志开关*/
#define LOGRECORD_SWITCH (0u)
/*日志转发开关*/
#define LOGTRANS_SWITCH  (0u)
/*日志串口打印开关*/
#define LOGSERIAL_SWITCH (0u)
/**日志SD卡备份开关 */
#define LOGLOCALFILE_SWITCH (0u)
/*暗文日志开关*/
#define DARKLOG_SWITCH (1u)

#define RSSP_VERSION_OLD  (0u)
#define RSSP_VERSION_NEW  (1u)
#if defined(PSC_MASTER)
    #define RSSP_VERSION RSSP_VERSION_OLD
#else
    #define RSSP_VERSION RSSP_VERSION_NEW
#endif


/*TMS570*/
#if (IS_570_IPB == 1) || (IS_570_OPB == 1) || (IS_570_SIG == 1)
#ifndef CCS_570
    #define CCS_570
#endif
#endif /*(IS_570_IPB == 1) || (IS_570_OPB == 1) || (IS_570_SIG == 1)*/

/*AM5728*/
#if IS_5728_MCP == 1
#ifndef CCS_5728
    #define CCS_5728
#endif
#endif /*IS_5728_MCP == 1*/

#endif

