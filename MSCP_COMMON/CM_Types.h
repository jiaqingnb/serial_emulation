/**
@file CM_Types.h
@brief 本文件定义公共模块的基本数据类型定义
公共模块由平台或应用调用
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#ifndef CM_TYPES_H  //修改飞检——不得使用保留标识符
#define CM_TYPES_H

#include "CommonConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(MSCP) || defined(LINUX)
#include "CommonTypes.h"
#endif

#ifdef WIN32_1

    #include <winsock2.h>
#endif

#ifdef VXWORKS
#include <vxworks.h>
#ifdef VXWORKS6_6
#include <ioLib.h>
#include <sioLibCommon.h>
#endif

#ifdef VXWORKS5_5
#endif
#endif
#ifdef QNX
#include <errno.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */
    /**
    * @BasicType
    * @brief 基本类型定义
    *
    * Detailed description.
    * @{
    */

#ifdef WIN32_1
#define CM_Get_Error_No() GetLastError()
#endif

#ifdef VXWORKS
#define CM_Get_Error_No() errno
#endif

#ifdef QNX
#define CM_Get_Error_No() errno
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif



#define CM_CHAR  INT8
#define CM_INT8 INT8
#define CM_UINT8 UINT8
#define CM_UCHAR UCHAR
#define	CM_INT16 INT16
#define	CM_UINT16 UINT16
#define	CM_INT32 INT32
#define	CM_UINT32 UINT32
/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
#define	CM_FLOAT32 FLOAT32
#define	CM_FLOAT64 FLOAT64
/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
typedef int                CM_BOOL;
typedef unsigned int       CM_UINT;
typedef size_t             CM_SIZE_T;


#ifndef CM_TRUE
#define CM_TRUE   ((CM_BOOL)(1))
#endif

#ifndef CM_FALSE
#define CM_FALSE    ((CM_BOOL)(0))
#endif

#ifndef CM_ERROR
#define CM_ERROR   ((CM_INT32)(-1))
#endif

#ifndef CM_OK
#define CM_OK   ((CM_UINT8)(0))
#endif

#define CM_NULL   ((void*)(0))

/*定义枚举型函数返回值*/
typedef enum
{
	ENUM_CM_TRUE    =   (0x55u),
	ENUM_CM_FALSE   =   (0xAAu),
	ENUM_CM_NULL    =   (0x00u)
}ENUM_CM_RETURN;


/*定义与枚举型函数返回值捆绑操作函数结构体*/
#define MAX_RTNVALUE_NUM 64u

/**
@brief 连续与数据与方法信息结构体
*/
typedef struct s_RtnOperation   //修改飞检——使用了保留名称 '_RtnOperation'
{
	void(*AddRtn)(ENUM_CM_RETURN ret, struct s_RtnOperation *pThis);
	ENUM_CM_RETURN(*rtnAllAboveResult)(struct s_RtnOperation *pThis);
	ENUM_CM_RETURN *pRtnArray;
	CM_UINT32 countRtnNum;
}STRU_CM_RtnOperation;

#ifdef WIN32_1
#pragma pack(1)
#define CM_PACKED
#endif

#define CBTC_TRUE   ((UINT8)0x55u)
#define CBTC_FALSE  ((UINT8)0xAAu)
#define CBTC_NULL   ((UINT8)0xFFu)
#ifdef WIN32_1
    typedef unsigned long long int CM_UINT64;
    typedef long long int          CM_INT64;
#else/*其它操作系统*/
    typedef unsigned long long CM_UINT64;
    typedef long long          CM_INT64;
#endif

#ifdef WIN32_1
    typedef unsigned        CM_THREAD;
    typedef unsigned        CM_THREAD_FUNC_RETURN;
    typedef CM_THREAD_FUNC_RETURN(__stdcall* CM_THREAD_FUNC)(void* args);
#define CM_THREAD_CALL __stdcall
#endif
#ifdef QNX
    typedef pthread_t       CM_THREAD;
    typedef void*           CM_THREAD_FUNC_RETURN;
    typedef CM_THREAD_FUNC_RETURN(*CM_THREAD_FUNC)(void* args);
#define CM_THREAD_CALL
#endif
#ifdef VXWORKS
    typedef int             CM_THREAD;
    typedef int             CM_THREAD_FUNC_RETURN;
    typedef FUNCPTR         CM_THREAD_FUNC;
#define CM_THREAD_CALL
#endif


#define CM_UINT32_MAX (4294967295U) /**< CM_UINT32最大值 */
#define CM_UINT16_MAX (65535U)        /**< CM_UINT16最大值 */
#define CM_UINT8_MAX  (255)          /**< CM_UINT8最大值 */
#define CM_INT16_MIN  (-32768)
#define CM_INT16_MAX  (32767)
#define CM_INT32_MIN   (-2147483647L - 1)
#define CM_INT32_MAX   (2147483647L)
#define CM_FLOAT32_MAX     (3.402823466e+38F)
#define CM_FLOAT64_MAX     (1.7976931348623158e+308)

    /**
    * @main
    * @brief 根据不同的操作系统定义相应的入口文件
    *
    * 支持Windowx、Linux、VxWorks.
    * @{
    */
#ifdef WIN32_1
#undef QNX
#undef VXWORKS
#define CM_MAIN_ENTRY void main
#endif



#ifdef QNX
#undef WIN32_1
#undef VXWORKS
#define CM_MAIN_ENTRY void main
#endif

#ifdef VXWORKS
#undef QNX
#undef WIN32_1
#define CM_MAIN_ENTRY void usrAppInit
#endif

#ifdef QNX
    extern CM_BOOL CM_Timer_Initialized;
#endif

#define CM_Log_Msg(...) NULL

#ifdef __cplusplus
}

#endif /**< __cplusplus */

#endif /**< _CM_TYPES_H */

