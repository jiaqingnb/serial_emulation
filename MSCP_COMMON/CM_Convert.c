/**
@file CM_Convert.c
@brief 通用类型转换函数
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#include "CM_Convert.h"

/*
*  功能描述：  用于将2字节数据变为CM_UINT16， BIG endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT16值
*/
CM_UINT16 CM_ShortFromChar(const CM_UINT8 *pInput)
{ 
    return ShortFromChar(pInput);
}

/*
*  功能描述：  用于将4字节数据变为CM_UINT32， BIG endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT32值
*/
CM_UINT32 CM_LongFromChar(const CM_UINT8 *pInput)
{
    return LongFromChar(pInput);
}

/*
*  功能描述：  将2个字节长的整型变为字节表示  BIG endian
*  参数说明：  Input, 为输入
*              pOutput,为输出
*  返回值：  
*/
void CM_ShortToChar ( CM_UINT16 Input, CM_UINT8 *pOutput)
{
	ShortToChar(Input, pOutput);
}

/*
*  功能描述：  将4个字节长的整型变为字节表示  BIG endian
*  参数说明：  Input, 为输入
*              pOutput,为输出
*  返回值：  
*/
void CM_LongToChar(CM_UINT32 Input, CM_UINT8 *pOutput)
{
	LongToChar(Input, pOutput);
}


/***
* @brief     8个字节长的整型变为字节表示
* @details   无
* @param     CM_UINT64 Input，，输入，输入数据，0-0xffffffffffff
             CM_UINT8 *pOutput，输入，输出数据指针，非空指针
* @return    无
* @author    高晗
* @date      2022.8.20
* @note      无
*/
void CM_LongLongToChar(CM_UINT64 Input, CM_UINT8 *pOutput)
{
    *pOutput =  (UINT8)((Input>>56) & 0xffu);
    *(pOutput + 1) =  (UINT8)((Input>>48) & 0xffu);
    *(pOutput + 2) =  (UINT8)((Input>>40) & 0xffu);
    *(pOutput + 3) =  (UINT8)(Input>>32 & 0xffu);
    *(pOutput + 4) =  (UINT8)((Input>>24) & 0xffu);
    *(pOutput + 5) =  (UINT8)((Input>>16) & 0xffu);
    *(pOutput + 6) =  (UINT8)((Input>>8) & 0xffu);
    *(pOutput + 7) =  (UINT8)(Input & 0xffu);
}

/***
* @brief     将8字节数据变为CM_UINT64
* @details   无
* @param     CM_UINT8 *pInput，输入，输入数据指针，非空指针
* @return    Templong，0-0xffffffffffff
* @author    高晗
* @date      2022.8.20
* @note      无
*/
CM_UINT64 CM_LongLongFromChar(const CM_UINT8 *pInput)
{
    CM_UINT64 Templong;
    Templong = ( *(pInput) );
    Templong = ( Templong<<8 ) + ( *(pInput+1) );
    Templong = ( Templong<<8 ) + ( *(pInput+2) );
    Templong = ( Templong<<8 ) + ( *(pInput+3) );
    Templong = ( Templong<<8 ) + ( *(pInput+4) );
    Templong = ( Templong<<8 ) + ( *(pInput+5) );
    Templong = ( Templong<<8 ) + ( *(pInput+6) );
    Templong = ( Templong<<8 ) + ( *(pInput+7) );

    return Templong;
}

/*
*  功能描述：  用于将CM_UINT8变为CM_UINT16， LITTLE endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT16值
*/
CM_UINT16 CM_ShortFromCharLE(const CM_UINT8 *pInput)
{  
    return ShortFromCharLE(pInput);
}

/*
*  功能描述：  用于将CM_UINT8变为CM_UINT32， LITTLE endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT32值
*/
CM_UINT32 CM_LongFromCharLE(const CM_UINT8 *pInput)
{
    return LongFromCharLE(pInput);
}

/*
*  功能描述： 将2个字节的整型变为字节表示  LITTLE endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值： 
*/
void CM_ShortToCharLE ( CM_UINT16 Input, CM_UINT8 *pOutput)
{
	ShortToCharLE(Input, pOutput);
}

/*
*  功能描述： 将4个字节的整型变为字节表示  LITTLE endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值： 
*/
void CM_LongToCharLE(CM_UINT32 Input, CM_UINT8 *pOutput)
{
	LongToCharLE(Input, pOutput);
}

/*检查大小端*/
/*c.a - 0x01000000*/
/*union不同长度成员均为左对齐（TMS570 Big Ending）*/
CM_UINT16 CheckCpu(void)
{
	union
	{
		CM_UINT16 a;
		CM_UINT8 b;
	}c;
	c.a = 1;
	return (CM_UINT16)(1u == c.b); /*1;BE 0;LE*/
}
/*
*  功能描述： 将4个字节的整型依据宏判断大小端，取高位1字节或低位1字节，
*  			  为4字节转为枚举类型做准备  LITTLE or BIG endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值： 
*/
void CM_LongToOneByte(IN CM_UINT32 Input, OUT CM_UINT8 *pOutput)
{   

	/*默认WINDOWS为BIG endian*/
    #if defined (WIN32_1) || defined (QNX) || defined (MSCP)
		*pOutput = (UINT8)((Input >> 0) & 0xffu);
	#endif
	
}


