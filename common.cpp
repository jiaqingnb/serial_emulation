#include "common.h"

/*
*  功能描述：  用于将2字节数据变为UINT16， BIG endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的UINT16值
*/
UINT16 Conver::ShortFromChar(const UINT8* pInput)
{
    UINT16 Tempshort;
    Tempshort = (*(pInput));
    Tempshort = (Tempshort << 8) + (*(pInput + 1));
    return Tempshort;
}

/*
*  功能描述：  用于将4字节数据变为UINT32， BIG endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的UINT32值
*/
UINT32 Conver::LongFromChar(const UINT8* pInput)
{
    UINT32 Templong;
    Templong = (*(pInput));
    Templong = (Templong << 8) + (*(pInput + 1));
    Templong = (Templong << 8) + (*(pInput + 2));
    Templong = (Templong << 8) + (*(pInput + 3));
    return Templong;
}

/*
*  功能描述：  将2个字节长的整型变为字节表示  BIG endian
*  参数说明：  Input, 为输入
*              pOutput,为输出
*  返回值：
*/
void Conver::ShortToChar(UINT16 Input, UINT8* pOutput)
{
    *pOutput = (Input >> 8) & 0xff;
    *(pOutput + 1) = Input & 0xff;
}

/*
*  功能描述：  将4个字节长的整型变为字节表示  BIG endian
*  参数说明：  Input, 为输入
*              pOutput,为输出
*  返回值：
*/
void Conver::LongToChar(UINT32 Input, UINT8* pOutput)
{
    *pOutput = (UINT8)((Input >> 24) & 0xff);
    *(pOutput + 1) = (UINT8)((Input >> 16) & 0xff);
    *(pOutput + 2) = (UINT8)((Input >> 8) & 0xff);
    *(pOutput + 3) = (UINT8)(Input & 0xff);
}




/*
*  功能描述：  用于将UINT8变为UINT16， LITTLE endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的UINT16值
*/
UINT16 Conver::ShortFromCharLE(const UINT8* pInput)
{
    UINT16 Tempshort;
    Tempshort = (*(pInput + 1));
    Tempshort = (Tempshort << 8) + (*(pInput));
    return Tempshort;
}

/*
*  功能描述：  用于将UINT8变为UINT32， LITTLE endian
*  参数说明：  pInput, 为输入
*  返回值：    变换后的UINT32值
*/
UINT32 Conver::LongFromCharLE(const UINT8* pInput)
{
    UINT32 Templong;
    Templong = (*(pInput + 3));
    Templong = (Templong << 8) + (*(pInput + 2));
    Templong = (Templong << 8) + (*(pInput + 1));
    Templong = (Templong << 8) + (*(pInput));
    return Templong;
}

/*
*  功能描述： 将2个字节的整型变为字节表示  LITTLE endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值：
*/
void Conver::ShortToCharLE(UINT16 Input, UINT8* pOutput)
{
    *(pOutput + 1) = (Input >> 8) & 0xff;
    *pOutput = Input & 0xff;
}

/*
*  功能描述： 将4个字节的整型变为字节表示  LITTLE endian
*  参数说明： Input, 为输入
*             pOutput,为输出
*  返回值：
*/
void Conver::LongToCharLE(UINT32 Input, UINT8* pOutput)
{
    *(pOutput + 3) = (UINT8)((Input >> 24) & 0xff);
    *(pOutput + 2) = (UINT8)((Input >> 16) & 0xff);
    *(pOutput + 1) = (UINT8)((Input >> 8) & 0xff);
    *pOutput = (UINT8)(Input & 0xff);
}