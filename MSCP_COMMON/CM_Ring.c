#include "CM_Memory.h"
#include "CM_Ring.h"

CM_BOOL CM_RQ_Init(STRU_CM_RQ_Light *pRQ, CM_UINT16 MsgNum, CM_UINT16 MsgSize)
{
	CM_BOOL ret = CM_FALSE;
	if ((NULL != pRQ) && (0u != MsgNum) && (0u != MsgSize))
	{
		if (CM_TRUE == pRQ->Initialized)
		{
			ret = CM_FALSE;
		}
		else
		{
			pRQ->buf = CM_Malloc((CM_SIZE_T)(MsgNum)*MsgSize);
			if (NULL != pRQ->buf)
			{
				CM_Memset(pRQ->buf, 0, (CM_SIZE_T)(MsgNum)*MsgSize);
				pRQ->Initialized = CM_TRUE;
				pRQ->rear = 0;
				pRQ->MsgNum = MsgNum;
				pRQ->MsgSize = MsgSize;
				pRQ->MsgCount = 0;
				ret = CM_TRUE;
			}
		}
	}

	return ret;
}

CM_BOOL CM_RQ_Write(STRU_CM_RQ_Light *pRQ, CM_UINT8 msg[])
{
	CM_BOOL ret = CM_FALSE;
	if ((NULL != pRQ) && (NULL != msg) && (CM_TRUE == pRQ->Initialized))
	{
		pRQ->rear = (pRQ->rear + 1) % pRQ->MsgNum;
		if(pRQ->MsgCount<pRQ->MsgNum)
		{
			pRQ->MsgCount++;
		}
		ret = (CM_BOOL)CM_Memcpy(pRQ->buf + pRQ->rear*((CM_INT32)pRQ->MsgSize), pRQ->MsgSize, msg, pRQ->MsgSize);
	}
	return ret;
}

CM_BOOL CM_RQ_Get(STRU_CM_RQ_Light *pRQ, CM_UINT16 offset, CM_UINT8 msg[])
{
	CM_BOOL ret = CM_FALSE;
	CM_INT32 index = 0;
	if ((NULL != pRQ) && (CM_TRUE == pRQ->Initialized) && (offset < pRQ->MsgCount))
	{
		if(pRQ->rear >= (CM_INT32)offset)
		{
			index = pRQ->rear - (CM_INT32)offset;
		}
		else
		{
			index = pRQ->rear + pRQ->MsgNum - (CM_INT32)offset;
		}
		ret = (CM_BOOL)CM_Memcpy(msg, pRQ->MsgSize, pRQ->buf + index * (CM_INT32)pRQ->MsgSize, pRQ->MsgSize);
	}
	return ret;
}

CM_BOOL CM_RQ_Clear(STRU_CM_RQ_Light *pRQ)
{
	CM_BOOL ret = CM_FALSE;
	if ((NULL != pRQ) && (CM_TRUE == pRQ->Initialized))
	{
		CM_Memset(pRQ->buf, 0, (CM_SIZE_T)(pRQ->MsgNum)*pRQ->MsgSize);
		pRQ->rear = 0;
		pRQ->MsgCount = 0;
	}
	return ret;
}

CM_UINT16 CM_RQ_GetCount(STRU_CM_RQ_Light *pRQ)
{
	return pRQ->MsgCount;
}
