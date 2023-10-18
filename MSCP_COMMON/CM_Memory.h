
/**
@file CM_Memory.h
@brief 实现了内存操作的的封装
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/

#ifndef CM_MEMORY_H  //修改飞检——不得使用保留标识符
#define CM_MEMORY_H

#include "CM_Types.h"
#include "CommonMemory.h"
#include "PROJECT_OR_BOARD_PTCL.h"
#ifdef __cplusplus
extern "C" {
#endif

	void CM_Memset(void* destination, CM_UINT8 value, CM_SIZE_T size);

	void* CM_Malloc(CM_SIZE_T size);

	void CM_Free(void* p);

	ENUM_CM_RETURN CM_Memcpy(OUT void * destination, IN CM_SIZE_T destSize, IN const void * source, IN CM_SIZE_T count);

	CM_INT32 CM_Memcmp(const void* pBufA, const void* pBufB, CM_SIZE_T size);
	CM_INT32 CM_Memcmp_Z(const void * pBufA, const void * pBufB, CM_SIZE_T size , UINT16* p_p16ErrOffSet);
	
#if 0
	void*
	memset (void *dest, int val, unsigned int len);
	int
	memcmp (const void *str1, const void *str2, unsigned int count);
	void *
	memcpy (void *dest, const void *src, unsigned int len);
	void *
	memmove (void *dest, const void *src, unsigned int  len);
#endif 
	

#ifdef __cplusplus
}
#endif

#endif


