/**
@file CM_CommonRecord.c
@brief 记录函数的源文件，可调用该文件中的函数将字符型或二进制型的数据写入记录数组
       记录数组前两位为当前写入数组的字节数，从第三个字节开始为需要写入记录数组的数据
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2018-11-13
*/

#include "CM_CommonRecord.h"

/******************************************************************************************
* 功能描述      : 以字符形式向文件记录数组中写入一个字符，该种形式的记录可直接查看，
记录数组中存储的是需要记录的数据的ASCII码。
如{'a'}，存储为{97,即0x61}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT8 c                        需要写入的字符
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintFilec(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT8 c)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintFilec(RecArray, RecArraySize, c);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}


/******************************************************************************************
* 功能描述      : 以字符形式向文件记录数组中写入N个Hex，该种形式的记录可直接查看，
记录数组中存储的是需要记录的数据的ASCII码。
如{0x12,0x34}，存储为{0x31，0x32，0x33，0x34}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT8 *HexSrc                  需要写入Hex数据的头指针
CM_UINT16 Length                  需要写入Hex数据的长度
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintFileHex(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT8 *HexSrc, IN CM_UINT16 Length)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintFileHex(RecArray, RecArraySize, HexSrc, Length);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}


/******************************************************************************************
* 功能描述      : 以字符形式向文件记录数组中写入一个字符串，该种形式的记录可直接查看，
记录数组中存储的是需要记录的数据的ASCII码。
如"1234"，存储为{0x31，0x32，0x33，0x34}={"1234"}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_CHAR *StringSrc                需要写入字符串的头指针
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintFiles(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_CHAR *StringSrc)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintFiles(RecArray, RecArraySize, StringSrc);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以字符形式向文件记录数组中写入一个Hex型的short(16bit)，该种形式的记录可直接查看，
记录数组中存储的是需要记录的数据的ASCII码。
如{0x1234}，存储为{0x31，0x32，0x33，0x34}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT16 HexSrc                  需要写入的Short型数据
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintFileU16Hex(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT16 HexSrc)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintFileU16Hex(RecArray, RecArraySize, HexSrc);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}


/******************************************************************************************
* 功能描述      : 以字符形式向文件记录数组中写入一个Hex型的long(32bit)，该种形式的记录可直接查看，
记录数组中存储的是需要记录的数据的ASCII码。
如{0x12345678}，存储为{0x31，0x32，0x33，0x34,0x35,0x36,0x37,0x38}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT32 HexSrc                  需要写入的long型数据
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintFileU32Hex(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT32 HexSrc)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintFileU32Hex(RecArray, RecArraySize, HexSrc);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以字符形式向文件记录数组中写入一个Int型的long(16bit)，该种形式的记录可直接查看，
记录数组中存储的是需要记录的数据的ASCII码。
如{305397760，十六进制表示为0x12340000}，存储为{0x31，0x32，0x33，0x34,0x30,0x30,0x30,0x30}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT32 IntSrc                  需要写入的int型数据
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintFileU32Int(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT32 IntSrc)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintFileU32Int(RecArray, RecArraySize, IntSrc);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以二进制形式向文件记录数组中写入一个字节
二进制形式举例：{0x12}，存储为{0x12}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT8 ByteData                 需要写入的字节
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintRecArrayByte(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT8 ByteData)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintRecArrayByte(RecArray, RecArraySize, ByteData);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以二进制形式小端方式向文件记录数组中写入两个字节
二进制形式举例：{0x1234}，存储为{0x34，0x12}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT16 BytesData               需要写入的字节
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintRecArray2ByteL(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT16 BytesData)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintRecArray2ByteL(RecArray, RecArraySize, BytesData);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以二进制形式大端方式向文件记录数组中写入两个字节
二进制形式举例：{0x1234}，存储为{0x12，0x34}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT16 BytesData               需要写入的字节
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintRecArray2ByteB(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT16 BytesData)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintRecArray2ByteB(RecArray, RecArraySize, BytesData);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以二进制形式小端方式向文件记录数组中写入四个字节
二进制形式举例：{0x12345678}，存储为{0x78，0x56，0x34，0x12}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT32 BytesData               需要写入的字节
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintRecArray4ByteL(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT32 BytesData)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintRecArray4ByteL(RecArray, RecArraySize, BytesData);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以二进制形式大端方式向文件记录数组中写入四个字节
二进制形式举例：{0x12345678}，存储为{0x12，0x34,0x56,0x78}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT32 BytesData               需要写入的字节
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintRecArray4ByteB(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT32 BytesData)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintRecArray4ByteB(RecArray, RecArraySize, BytesData);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}



/******************************************************************************************
* 功能描述      : 以二进制形式向文件记录数组中写入N个字节
二进制形式举例：{0x12,0x34}，存储为{0x12，0x34}
* 输入参数      : CM_UINT16 RecArraySize            记录数组大小
CM_UINT8 *pBytesData              需要写入的数据的头指针
CM_UINT16 Length                  需要写入的字节数
* 输入输出参数  : CM_UINT8 *RecArray                记录数组的头指针
* 输出参数      :
* 全局变量      :
* 返回值  ： 0x55，成功
*            0xAA，失败
*******************************************************************************************/
ENUM_CM_RETURN CM_PrintRecArrayNByte(INOUT CM_UINT8 *RecArray, IN CM_UINT16 RecArraySize, IN CM_UINT8 *pBytesData, IN CM_UINT16 Length)/* %RELAX<MISRA_8_10> */
{
	ENUM_CM_RETURN  rtnValue = ENUM_CM_FALSE;
	CM_UINT8 tmpRtn = 0;
	tmpRtn = PrintRecArrayNByte(RecArray, RecArraySize, pBytesData, Length);
	if (1U == tmpRtn)
	{
		rtnValue = ENUM_CM_TRUE;
	}
	else
	{
		rtnValue = ENUM_CM_FALSE;
	}
	return rtnValue;
}

