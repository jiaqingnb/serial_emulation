/**
@file CM_Memory.c
@brief 实现了内存操作的的封装
@author 小型化安全计算机平台开发小组
@version 1.0.0.0
@date 2017-12-20
*/
#include "CM_Memory.h"
#include "string.h"



void CM_Memset(void * destination, CM_UINT8 value, CM_SIZE_T size)
{
    CM_UINT32 debug_I = 0;
	if ((NULL != destination) && (size != (CM_SIZE_T)0))
	{
	    (void)memset(destination,(int)value,size);
	}
	else
	{
	    ; /**  nothing to do*/
	}

}

void* CM_Malloc(CM_SIZE_T size)
{
	void* rt = NULL;

	/*增加对size=0时候，返回null */
	if (size != (CM_SIZE_T)0)
	{
		rt = malloc(size);
	}

	return rt;
}

void CM_Free(void * p)
{
	if (NULL != p)
	{
		free(p);
	}
}

ENUM_CM_RETURN CM_Memcpy(OUT void * destination,IN CM_SIZE_T destSize,IN const void * source,IN CM_SIZE_T count)
{
	ENUM_CM_RETURN ret = ENUM_CM_FALSE;

	if ((NULL != destination) && (NULL != source) && (count <= destSize) && (count != (CM_SIZE_T)0))
	{
        (void)memcpy(destination,source,count);
        ret = ENUM_CM_TRUE;
	}
	else
	{
	    ret = ENUM_CM_FALSE;
	}
	return ret;
}

CM_INT32 CM_Memcmp(const void * pBufA, const void * pBufB, CM_SIZE_T size )
{
	CM_INT32 ret = 1;
	UINT32 i = 0 ;
	if ((pBufA == NULL) || (pBufB == NULL) )
	{
		ret = (CM_INT32)1;
	}
	else
	{

	    for(i = 0 ; i < size;i++)
	    {
	        if(*((UINT8*)pBufA+i) != *((UINT8*)pBufB+i))
	        {
	            break;
	        }
	    }

	    if(i == size)
	    {
	        ret = 0 ;
	    }
	}
	return ret;
}
CM_INT32 CM_Memcmp_Z(const void * pBufA, const void * pBufB, CM_SIZE_T size , UINT16* p_p16ErrOffSet)
{
    CM_INT32 ret = 1;
    UINT32 i = 0 ;
    if ((pBufA == NULL) || (pBufB == NULL) )
    {
        ret = (CM_INT32)1;
    }
    else
    {

        for(i = 0 ; i < size;i++)
        {
            if(*((UINT8*)pBufA+i) != *((UINT8*)pBufB+i))
            {
                *p_p16ErrOffSet = (UINT16)i;
                break;
            }
        }

        if(i == size)
        {
            ret = 0 ;
        }
    }
    return ret;
}

#if 0
void *
memset (void *dest, int val, unsigned int len)
{
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

int
memcmp (const void *str1, const void *str2, unsigned int count)
{
  const unsigned char *s1 = str1;
  const unsigned char *s2 = str2;

  while (count-- > 0)
    {
      if (*s1++ != *s2++)
	  return s1[-1] < s2[-1] ? -1 : 1;
    }
  return 0;
}

void *
memcpy (void *dest, const void *src, unsigned int len)
{
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

void *
memmove (void *dest, const void *src, unsigned int  len)
{
  char *d = dest;
  const char *s = src;
  if (d < s)
    while (len--)
      *d++ = *s++;
  else
    {
      char *lasts = s + (len-1);
      char *lastd = d + (len-1);
      while (len--)
        *lastd-- = *lasts--;
    }
  return dest;
}
#endif
