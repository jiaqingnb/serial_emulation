
/*******************************************************************************
* 文件名    ：  sfpConfig.h
* 版权说明  ：  
* 版本号    ：  1.0  
* 创建时间  ：  2010.05.04
* 作者      ：  李雁
* 功能描述  ：  此文件为SFP协议配置文件 
* 使用注意  ： 
                无
* 修改记录  ：  

*******************************************************************************/

#ifndef SFPCONFIG_H
#define SFPCONFIG_H

/**************************协议记录宏定义开关**********************************/
/*#define PRTCL_RECORD_DATA_ASC*/             /*数据部分字符型记录开关*/
/*#define PRTCL_RECORD_ERR_ASC*/              /*错误部分字符型记录开关*/

#define PRTCL_RECORD_SIMPLIFY_BIN	/*二进制记录开关*/
#define USE_DSU_SFP	/*使用SFP查询函数*/
//#define SWAP_CRC /*交换CRC*/
//#define DUAL_CRC /*双CRC*/
#else
#endif

