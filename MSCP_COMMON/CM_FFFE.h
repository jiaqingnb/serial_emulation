/**
@file CM_FFFE.h
@brief FFFE协议转义及接收文件
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#ifndef CM_FFFE_H  //修改飞检——不得使用保留标识符
#define CM_FFFE_H

#include "CM_Types.h"
#include "FFFE.h"
#ifdef __cplusplus
extern "C" {
#endif
#define STRU_CM_FFFE_STATE FFFE_STATE_STRU



#define LEN_INPUTCMP_DATA       (512*6u)

/*
* 功能描述： 将数据进行FFFE转义处理（不加头FFFE及尾FFFD）。
* 输入参数： const CM_UINT8 *pSrc    数据(无FFFE转义)
*            CM_UINT16 SrcLen        数据(无FFFE转义)的长度
* 输出参数： CM_UINT8 *pTgt          数据(经过FFFE转义)
* 返回值：   转义后的长度   
*/
CM_UINT16 CM_TranslateToFFFE(const CM_UINT8 *pSrc, CM_UINT8 *pTgt, CM_UINT16 SrcLen);


/*
* 功能描述：  将经过FFFE转义处理后的数据（已经去掉了头FFFE及尾FFFD）还原为原始数据
* 输入参数：  const CM_UINT8 *pSrc,    数据(FFFE转义)
*             CM_UINT16 SrcLen,        数据(FFFE转义)的长度
* 输出参数：  CM_UINT8 *pTgt,          数据(去掉FFFE转义)
* 返回值：    大于0,  转义成功,返回转义后的数据长度
*             0,   有不能转义的字符，转义失败。或者转义后的数据长度为0   
*/
CM_UINT16 CM_TranslateFromFFFE(const CM_UINT8 *pSrc, CM_UINT8 *pTgt, CM_UINT16 SrcLen);

/*
* 功能描述：  FFFE结构体初始化函数
* 输入参数：  CM_UINT16 FFFEBufferSize, 用于接收一帧数据的缓冲区长度   
*             PORT_STATE_STRU *pFFFEStatStru, FFFE状态结构体
* 返回值：    0,函数执行失败
*             1,函数执行成功
*/
ENUM_CM_RETURN CM_FFFEInit(STRU_CM_FFFE_STATE *pFFFEStatStru, CM_UINT16 FFFEBufferSize);

/*
* 功能描述：   按字符接收待解转义的数据，获得整帧FFFE报文（不含FFFE和FFFD）。
* 输入参数：   CM_UINT8 RcvChar,      当前接收的字符
*              PORT_STATE_STRU *pFFFEStatStru,
* 输出参数：   CM_UINT8 *pRawDataRcv,  数据接收缓冲区指针
* 返回值：     大于0,  表示收到完整帧的数据长度，并将收到的完整帧拷贝到接收缓冲区；
*              0,   未收到整帧数据； 
*/

CM_UINT16 CM_FFFERcv(STRU_CM_FFFE_STATE *pFFFEStatStru, CM_UINT8 RcvChar, CM_UINT8 *pRawDataRcv);

/*
* 功能描述：  FFFE状态清理函数
* 输入参数：  PORT_STATE_STRU *pFFFEStatStru, FFFE状态结构体
* 返回值：     
*/
void CM_FFFEFree(STRU_CM_FFFE_STATE *pFFFEStatStru);

/*
* 功能描述： 将数据进行FFFE转义处理,加头FFFE及尾FFFD
* 输入参数： const CM_UINT8 *pSrc    数据(无FFFE转义)
*            CM_UINT16 SrcLen        数据(无FFFE转义)的长度
* 输出参数： CM_UINT8 *pTgt          数据(经过FFFE转义),并添加FFFE头及FFFD尾
* 返回值：   转义后的长度   
*/
CM_UINT16 CM_FFFEPack(const CM_UINT8 *pSrc, CM_UINT8 *pTgt, CM_UINT16 SrcLen);


/*编码FFFE*/
ENUM_CM_RETURN CM_FFFECode(IN const CM_UINT8 *pInBuf, CM_UINT16 inLen, OUT CM_UINT8 *pOutBuf, OUT CM_UINT16 *pOutLen);

/*解码FFFE*/
ENUM_CM_RETURN CM_FFFEDecode(FFFE_STATE_STRU *pFFFE_Struct, IN CM_UINT8 tempUint8, OUT CM_UINT8 *pOutBuf, OUT CM_UINT32 *pOutLen);

#ifdef __cplusplus
}
#endif

#endif
