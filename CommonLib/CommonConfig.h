
/************************************************************************
*
* 文件名   ：  CommonConfig.h
* 版权说明 ：  北京瑞安时代科技有限责任公司 
* 版本号   ：  1.0
* 创建时间 ：  2009.11.21
* 作者     ：  车载及协议部
* 功能描述 ：  cbtc公共函数配置文件.  
* 使用注意 ： 
* 修改记录 ：  
*
************************************************************************/


#ifndef COMMONCONFIG_H
#define COMMONCONFIG_H

/*VBTC系统平台定义*/
#define kWINDOWS  0
#define kMOTOROLA 1
#define kVXWORKS  2

/*在这里定义应用平台*/
/***********************************************************/
#define kPLATFORM kMOTOROLA 
/***********************************************************/

/*定义mem操作方式*/
#define LIB_MEM_PROCESS         /*内存操作使用库函数*/

/*定义FS数据读取方式*/
#undef  READ_DATA_FROM_FS_FILEPATH             /*通过传入文件路径的方式进行FS文件读取*/
#undef  READ_DATA_FROM_FS_POINTER_AND_COPY     /*通过传入文件指针的方式进行FS读取并拷贝内存中*/
#define READ_DATA_FROM_FS_POINTER_NO_COPY     /*通过传入文件指针的方式进行FS读取,但不拷贝内存中*/

/*小型化平台使用宏值*/
#define MSCP  


#endif /* _COMMONCONFIG_H_ */
