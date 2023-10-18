/**
@file CM_CbtcVersion.c
@brief 版本号格式转换软件
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#include "CM_CbtcVersion.h"


/*
* 功能说明 : 软件模块版本号转换。
* 参数说明 : CM_UINT16 productNo,产品编号
*            CM_UINT16 moduleNo,软件模块编号
*            CM_UINT16 mainVer,主版本号
*            CM_UINT16 subVer，子版本号
* 返回值:    U32表示的版本号
*/
CM_UINT32 CM_VerToU32SoftwareModule(CM_UINT16 productNo,CM_UINT16 moduleNo,CM_UINT16 mainVer,CM_UINT16 subVer)
{
	return VerToU32SoftwareModule(productNo, moduleNo, mainVer, subVer);
}

/*
* 功能说明 : 软件模块版本号转换。
* 参数说明 : CM_UINT16* pProductNo,产品编号
*            CM_UINT16* pModuleNo,软件模块编号
*            CM_UINT16* pMainVer,主版本号
*            CM_UINT16* pSubVer，子版本号
*            CM_UINT32 u32Ver,U32表示的版本号
*/
void CM_VerFromU32SoftwareModule(CM_UINT16* pProductNo,CM_UINT16* pModuleNo,CM_UINT16* pMainVer,CM_UINT16* pSubVer,CM_UINT32 u32Ver)
{
	VerFromU32SoftwareModule(pProductNo, pModuleNo, pMainVer, pSubVer, u32Ver);
}

/*
* 功能说明 : 软件集成版本号转换。
* 参数说明 : CM_UINT16 projectNo,项目编号
*            CM_UINT16 productNo,产品编号
*            CM_UINT16 ocNo,可执行文件编号
*            CM_UINT16 integrationNo,集成编号
* 返回值:    U32表示的版本号
*/
CM_UINT32 CM_VerToU32SoftwareInt(CM_UINT16 projectNo,CM_UINT16 productNo,CM_UINT16 ocNo,CM_UINT16 integrationNo)
{
	return VerToU32SoftwareInt(projectNo, productNo, ocNo, integrationNo);
}

/*
* 功能说明 : 软件集成版本号转换。
* 参数说明 : CM_UINT16* pProjectNo,项目编号
*            CM_UINT16* pProductNo,产品编号
*            CM_UINT16* pOcNo,可执行文件编号
*            CM_UINT16* pIntegrationNo，集成编号
*            CM_UINT32 u32Ver,U32表示的版本号
*/
void CM_VerFromU32SoftwareInt(CM_UINT16* pProjectNo,CM_UINT16* pProductNo,CM_UINT16* pOcNo,CM_UINT16* pIntegrationNo,CM_UINT32 u32Ver)
{
	VerFromU32SoftwareInt(pProjectNo, pProductNo, pOcNo, pIntegrationNo, u32Ver);
}

/*
* 功能说明 : 数据模块版本号转换。
* 参数说明 : CM_UINT16 projectNo,项目编号
*            CM_UINT16 dataTypeNo,数据类型编号
*            CM_UINT16 dataNo,数据版本号
*            CM_UINT16 reserved,预留信息
* 返回值:    U32表示的版本号
*/
CM_UINT32 CM_VerToU32DataModule(CM_UINT16 projectNo,CM_UINT16 dataTypeNo,CM_UINT16 dataNo,CM_UINT16 reserved)
{
	return VerToU32DataModule( projectNo, dataTypeNo, dataNo, reserved);
}

/*
* 功能说明 : 数据模块版本号转换。
* 参数说明 : CM_UINT16* pProjectNo,项目编号
*            CM_UINT16* pDataTypeNo,数据类型编号
*            CM_UINT16* pDataNo,数据版本号
*            CM_UINT16* pReserved，预留信息
*            CM_UINT32 u32Ver,U32表示的版本号
*/
void CM_VerFromU32DataModule(CM_UINT16* pProjectNo,CM_UINT16* pDataTypeNo,CM_UINT16* pDataNo,CM_UINT16* pReserved,CM_UINT32 u32Ver)
{
	VerFromU32DataModule(pProjectNo, pDataTypeNo, pDataNo, pReserved, u32Ver);
}

/*
* 功能说明 : 数据集成版本号转换。
* 参数说明 : CM_UINT16 projectNo,项目编号
*            CM_UINT16 dataFileNo,数据文件编号
*            CM_UINT16 dataIntNo,数据集成版本号
*            CM_UINT16 reserved,预留信息
* 返回值:    U32表示的版本号
*/
CM_UINT32 CM_VerToU32DataInt(CM_UINT16 projectNo,CM_UINT16 dataFileNo,CM_UINT16 dataIntNo,CM_UINT16 reserved)
{
	return VerToU32DataInt(projectNo, dataFileNo, dataIntNo, reserved);
}

/*
* 功能说明 : 数据模块版本号转换。
* 参数说明 : CM_UINT16* pProjectNo,项目编号
*            CM_UINT16* pDataFileNo,数据文件编号
*            CM_UINT16* pDataIntNo,数据集成版本号
*            CM_UINT16* pReserved，预留信息
*            CM_UINT32 u32Ver,U32表示的版本号
*/
void CM_VerFromU32DataInt(CM_UINT16* pProjectNo,CM_UINT16* pDataFileNo,CM_UINT16* pDataIntNo,CM_UINT16* pReserved,CM_UINT32 u32Ver)
{
	VerFromU32DataInt(pProjectNo, pDataFileNo, pDataIntNo, pReserved, u32Ver);
}

