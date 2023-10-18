/************************************************************************
* 文件名	：  protclMgeCfg.h
* 版权说明	：   
* 版本号	：  1.0  
* 创建时间	：  2013.09.22
* 作者		：	软件部
* 功能描述	：	协议管理配置 
                将各协议集中管理，对外提供统一接口。
* 使用注意	： 

* 修改记录	：	2013.09.22  1 新建 
                
************************************************************************/

#ifndef PROTCLMGECFG_H
#define PROTCLMGECFG_H

#include "CommonTypes.h"
#include "CommonQueue.h"
#include "protclStruct.h"

/****#宏定义#*************************************************************/
/* 协议类型 */


#ifdef __cplusplus
extern "C" {
#endif

/****#类型定义#*************************************************************/
/* 函数指针 */
typedef UINT8  (*F_INIT)(UINT8*, UINT32, UINT8*, UINT8, ProtclConfigInfoStru*);       /* 封装之INIT类型 */
typedef UINT8  (*F_SND)(ProtclConfigInfoStru*, UINT8, UINT8*, UINT32*, UINT8*, UINT16*, UINT8*, UINT16*);  /* 封装之SND类型 */
typedef UINT8  (* F_RCV)(QueueStruct*, ProtclConfigInfoStru*);  /* 封装之RCV类型 */
typedef UINT8  (* F_LNK)(UINT8,UINT8,UINT16, ProtclConfigInfoStru*);   /* 封装之GETLNK类型 */
typedef void   (*F_NETPKGNUM)(ProtclConfigInfoStru*, const UINT8, const UINT8, const UINT16, UINT16*, UINT16*);   /* 封装之NetsRecvPkgNum类型 */
typedef UINT8  (* F_DELLNK)(UINT8,UINT8,UINT16, ProtclConfigInfoStru*);/* 封装之DELLNK类型 */
typedef UINT8  (* F_REFRESHLNK)(UINT16,UINT8*, ProtclConfigInfoStru*);   /* 封装之REFRESHLNK类型 */
typedef UINT8  (* F_END)(ProtclConfigInfoStru*);    /* 封装之END类型 */

/****#结构体定义#********************************************************/
typedef struct  {
	F_INIT fInit;
	F_SND  fSnd;
	F_RCV  fRcv;
	F_LNK fLnk;
	F_NETPKGNUM fNetsRecvPkgNum;
	F_DELLNK fDelLnk;
	F_REFRESHLNK fReFreshLnk;
	F_END fEnd;
}PROTCL_MGECFG_STRU;


#ifdef __cplusplus
}
#endif

#endif
