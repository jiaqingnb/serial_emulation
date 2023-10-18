/**
@file CM_FFFE.c
@brief FFFE协议转义及接收文件
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#include "CM_FFFE.h"
/*FFFE解转义状态*/
#define CM_WAIT_FF_C1 WAIT_FF_C1
#define CM_WAIT_FF_C2 WAIT_FF_C2
#define CM_WAIT_NO_FF WAIT_NO_FF
#define CM_WAIT_FF_POS WAIT_FF_POS
#define CM_ABNORMAL_STATE ABNORMAL_STATE

/*FFFE数据接收状态*/
#define CM_COM_WAIT_START_FF COM_WAIT_START_FF
#define CM_COM_WAIT_START_FE COM_WAIT_START_FE
#define CM_COM_WAIT_STOP_FF COM_WAIT_STOP_FF
#define CM_COM_WAIT_STOP_FD COM_WAIT_STOP_FD

/*
* 功能描述： 将数据进行FFFE转义处理（不加头FFFE及尾FFFD）。
* 输入参数： const CM_UINT8 *pSrc    数据(无FFFE转义)
*            CM_UINT16 SrcLen        数据(无FFFE转义)的长度
* 输出参数： CM_UINT8 *pTgt          数据(经过FFFE转义)
* 返回值：   转义后的长度   
*/
CM_UINT16 CM_TranslateToFFFE(const CM_UINT8 *pSrc, CM_UINT8 *pTgt,CM_UINT16 SrcLen)
{
	return TranslateToFFFE(pSrc, pTgt, SrcLen);
}


/*
* 功能描述：  将经过FFFE转义处理后的数据（已经去掉了头FFFE及尾FFFD）还原为原始数据
* 输入参数：  const CM_UINT8 *pSrc,    数据(FFFE转义)
*             CM_UINT16 SrcLen,        数据(FFFE转义)的长度
* 输出参数：  CM_UINT8 *pTgt,          数据(去掉FFFE转义)
* 返回值：    大于0,  转义成功,返回转义后的数据长度
*             0,   有不能转义的字符，转义失败。或者转义后的数据长度为0   
*/
CM_UINT16 CM_TranslateFromFFFE(const CM_UINT8 *pSrc,CM_UINT8 *pTgt,CM_UINT16 SrcLen)
{
    return TranslateFromFFFE(pSrc,pTgt,SrcLen);
}

/*
* 功能描述：  FFFE结构体初始化函数
* 输入参数：  CM_UINT16 FFFEBufferSize, 用于接收一帧数据的缓冲区长度   
*             STRU_CM_FFFE_STATE *pFFFEStatStru, FFFE状态结构体
* 返回值：       ENUM_CM_TRUE, 函数执行失败
*             ENUM_CM_FALSE, 函数执行成功
*/
ENUM_CM_RETURN CM_FFFEInit(STRU_CM_FFFE_STATE *pFFFEStatStru,CM_UINT16 FFFEBufferSize)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	if (1u == FFFEInit(pFFFEStatStru,FFFEBufferSize))
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}

/*
* 功能描述：   按字符接收待解转义的数据，获得整帧FFFE报文（不含FFFE和FFFD）。
* 输入参数：   CM_UINT8 RcvChar,      当前接收的字符 -> character
*            PORT_STATE_STRU *pFFFEStatStru,
* 输出参数：   CM_UINT8 *pRawDataRcv,  数据接收缓冲区指针
* 返回值：     大于0,  表示收到完整帧的数据长度，并将收到的完整帧拷贝到接收缓冲区；
*              0,   未收到整帧数据； 
*/

CM_UINT16 CM_FFFERcv(STRU_CM_FFFE_STATE *pFFFEStatStru ,CM_UINT8 RcvChar, CM_UINT8 *pRawDataRcv)
{
    return FFFERcv(pFFFEStatStru,RcvChar,pRawDataRcv);
}

/*
* 功能描述：  FFFE状态清理函数
* 输入参数：  PORT_STATE_STRU *pFFFEStatStru, FFFE状态结构体
* 返回值：     
*/
void CM_FFFEFree(STRU_CM_FFFE_STATE *pFFFEStatStru)
{
	FFFEFree(pFFFEStatStru);

}


/*
* 功能描述： 将数据进行FFFE转义处理,加头FFFE及尾FFFD
* 输入参数： const CM_UINT8 *pSrc    数据(无FFFE转义)
*            CM_UINT16 SrcLen        数据(无FFFE转义)的长度
* 输出参数： CM_UINT8 *pTgt          数据(经过FFFE转义),并添加FFFE头及FFFD尾
* 返回值：   转义后的长度   
*/
CM_UINT16 CM_FFFEPack(const CM_UINT8 *pSrc, CM_UINT8 *pTgt,CM_UINT16 SrcLen)
{
	return FFFEPack(pSrc, pTgt, SrcLen);

}


/*编码FFFE*/
ENUM_CM_RETURN CM_FFFECode(IN const CM_UINT8 *pInBuf, CM_UINT16 inLen, OUT CM_UINT8 *pOutBuf, OUT CM_UINT16 *pOutLen)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	CM_UINT16 tempLen = 0;

	if ((NULL != pInBuf) && (NULL != pOutBuf) && (NULL != pOutLen))
	{
		tempLen = CM_TranslateToFFFE(&pInBuf[0], &pOutBuf[2], inLen);
		pOutBuf[0] = 0xff;
		pOutBuf[1] = 0xfe;
		pOutBuf[tempLen + 2u] = 0xff;
		pOutBuf[tempLen + 3u] = 0xfd;
		*pOutLen = tempLen + 4u;
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;/*指针防控*/
	}
	return rtnValue;
}

/*解码FFFE*/
ENUM_CM_RETURN CM_FFFEDecode(FFFE_STATE_STRU *pFFFE_Struct, IN CM_UINT8 tempUint8, OUT CM_UINT8 *pOutBuf, OUT CM_UINT32 *pOutLen)
{
	ENUM_CM_RETURN rtnValue = ENUM_CM_FALSE;
	CM_UINT8 RcvWholeFrmBuf[LEN_INPUTCMP_DATA]; /*由于初始化时间较长，该初始化不影响安全*/
	CM_UINT16 RcvAftFffeLen = 0;
	CM_UINT16 FffeRcvLen = 0;
	if ((NULL != pFFFE_Struct) && (NULL != pOutBuf) && (NULL != pOutLen))
	{
		FffeRcvLen = CM_FFFERcv(pFFFE_Struct, tempUint8, RcvWholeFrmBuf);/*通过FFFE协议接收一帧数据*/
		if (0u != FffeRcvLen)/*如果接受到完整帧的数据*/
		{
			RcvAftFffeLen = CM_TranslateFromFFFE(RcvWholeFrmBuf, pOutBuf, FffeRcvLen);/*从FFFE数据帧进行转义；源，目标，长度*/
			if (RcvAftFffeLen > 0u)/*转义成功*/
			{
				*pOutLen = (CM_UINT32)RcvAftFffeLen;
				rtnValue = ENUM_CM_TRUE;
			}
			else
			{
                ;
			}
		}
		else
		{
			rtnValue = ENUM_CM_FALSE;
		}
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}
