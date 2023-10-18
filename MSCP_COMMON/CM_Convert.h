/**
@file CM_Convert.h
@brief 通用类型转换函数
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#ifndef CM_CONVERT_H  //修改飞检——不得使用保留标识符
#define CM_CONVERT_H

#include "CM_Types.h"
#include "Convert.h"
#ifdef __cplusplus
extern "C" {
#endif


/*
*  功能描述：  用于将2字节数据变为CM_UINT16， BIG endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT16值
*/
CM_UINT16 CM_ShortFromChar(const CM_UINT8 *pInput);

/*
*  功能描述：  用于将4字节数据变为CM_UINT32， BIG endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT32值
*/
CM_UINT32 CM_LongFromChar(const CM_UINT8 *pInput);


/*
*  功能描述：  将2个字节长的整型变为字节表示  BIG endian
*  参数说明：  Input, 为输入
*              pOutput,为输出
*  返回值：  
*/
void CM_ShortToChar ( CM_UINT16 Input, CM_UINT8 *pOutput);

/*
*  功能描述：  将4个字节长的整型变为字节表示  BIG endian
*  参数说明：  Input, 为输入
*              pOutput,为输出
*  返回值：  
*/
void CM_LongToChar(CM_UINT32 Input, CM_UINT8 *pOutput);

/*
*  功能描述：  将8个字节长的整型变为字节表示  BIG endian
*  参数说明：  Input, 为输入
*              pOutput,为输出
*  返回值：
*/
void CM_LongLongToChar(CM_UINT64 Input, CM_UINT8 *pOutput);

/*
*  功能描述：  用于将8字节数据变为CM_UINT64， BIG endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT64值
*/
CM_UINT64 CM_LongLongFromChar(const CM_UINT8 *pInput);

/*
*  功能描述：  用于将CM_UINT8变为CM_UINT16， LITTLE endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT16值
*/
CM_UINT16 CM_ShortFromCharLE(const CM_UINT8 *pInput);


/*
*  功能描述：  用于将CM_UINT8变为CM_UINT32， LITTLE endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的CM_UINT32值
*/
CM_UINT32 CM_LongFromCharLE(const CM_UINT8 *pInput);

/*
*  功能描述： 将2个字节的整型变为字节表示  LITTLE endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值： 
*/
void CM_ShortToCharLE ( CM_UINT16 Input, CM_UINT8 *pOutput);

/*
*  功能描述： 将4个字节的整型变为字节表示  LITTLE endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值： 
*/
void CM_LongToCharLE(CM_UINT32 Input, CM_UINT8 *pOutput);

/*检查大小端*/
/*c.a - 0x01000000*/
CM_UINT16 CheckCpu(void);
/*
*  功能描述： 将4个字节的整型依据宏判断大小端，取高位1字节或低位1字节，
*  			  为4字节转为枚举类型做准备  LITTLE or BIG endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值： 
*/
void CM_LongToOneByte(IN CM_UINT32 Input, OUT CM_UINT8 *pOutput);

#ifdef __cplusplus
}
#endif

#endif
