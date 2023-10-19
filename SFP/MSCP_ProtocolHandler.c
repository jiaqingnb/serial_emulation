/****************************************************************************************************
* 文件名   :  MSCP_ProtocolHandler.c
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2020.02.17
* 作者     :  lsn
* 功能描述 : 平台对输入数据输出数据进行协议处理,包含输入数据的去重处理
* 使用注意 :
* 修改记录 :
**************************************************************************************************/

#include "PROJECT_OR_BOARD_PTCL.h"
#include "CM_Types.h"
#include "CM_Memory.h"
#include "CommonMemory.h"
#include "CommonQueue.h"
#include "Convert.h"
#include "CM_FFFE.h"
#include "sfpplatform.h"
#include "sfpInnetDataVar.h"
#include "PLFM_CycleTime_Config.h"

/* 对于5728和570各个板卡通用的协议处理函数部分*/
/***************************************************************************************
* 功能描述: 将数据按照协议形式封装成过SFP协议前的数据
* 输入参数:  UINT8 * pSendbuff 发送的数据流
*         UINT16 SendBuffLen 发送数据长度
*         UINT16 DataTypeId 发送数据的类型和ID
* 输入输出参数:     无
* 输出参数:   QueueStruct * pBefor_Sfp_Q 过SFP协议前的队列指针
* 全局变量:         ...
* 返回值:           返回值为0x55，打包成功
*                 返回值为0xAA，打包失败
* 修改记录：  lsn新增 2020-03-11
****************************************************************************************/
ENUM_CM_RETURN SFP_Protocol_Pack(UINT8 * pSendbuff,UINT16 SendBuffLen,UINT16 DataTypeId,QueueStruct * pBefor_Sfp_Q)
{

    ENUM_CM_RETURN FuncRtn = ENUM_CM_FALSE;
    UINT8 CallRtn = 0;
    UINT8 writeHead[6] = { 0, }; /* 用于存放SFP的协议头定义的数组*/

    if((NULL != pSendbuff)&&(NULL != pBefor_Sfp_Q))
    {
        /* 将数据长度存放到队列中*/
       ShortToChar(SendBuffLen + 4u, &writeHead[0]);
       writeHead[2] = 0;
       writeHead[3] = 0;
       /* 将typeID以小端形似存放*/
       writeHead[4] = (UINT8)((DataTypeId) & 0xffu);
       writeHead[5] = (UINT8)((DataTypeId >> 8) & 0xffu);
       /*将封装的SFP的头将 */
       CallRtn = QueueWrite(6, writeHead, pBefor_Sfp_Q);
       if(CallRtn == 1u)
       {
           /* 将数据放入过协议前队列*/
           CallRtn = QueueWrite(SendBuffLen, pSendbuff, pBefor_Sfp_Q);
           if(CallRtn == 1u)
           {
               /* 将数据均写到队列中，将返回值赋值为正常*/
               FuncRtn = ENUM_CM_TRUE;
           }
           else
           {
               /* do nothing */
           }

       }
       else
       {
           /* nothing */
       }

    }
    else
    {
        /* nothing */
    }

    return FuncRtn;
}

UINT8 QueueCopyAdd(QueueStruct* p_stpQueueDest , QueueStruct* p_stpQueueSrc)
{
    UINT8 t_u8Ret = 0 ;
       UINT32 t_u8I = 0 ;
       UINT8 t_u8Tmp = 0 ;
       UINT8 t_u8TmpRet1 = 0 ;
       UINT8 t_u8TmpRet2 = 0 ;
       UINT32 t_u32QueueSize = 0 ;
       t_u32QueueSize = QueueStatus(p_stpQueueSrc);
       if((NULL != p_stpQueueDest)&&(NULL != p_stpQueueSrc))
       {
           for(t_u8I = 0 ; t_u8I < t_u32QueueSize; t_u8I++)
           {
               t_u8TmpRet1 = QueueRead(1 , &t_u8Tmp , p_stpQueueSrc);
               t_u8TmpRet2 = QueueWrite(1,&t_u8Tmp ,p_stpQueueDest);
               if((1u != t_u8TmpRet1) || (1u != t_u8TmpRet2))
               {
                   t_u8Ret = 0 ;
                   break;
               }
               else
               {
                   /*  nothing to do  */
               }

           }
           if(t_u8I == t_u32QueueSize)
           {
               t_u8Ret = 1;
           }
           else
           {
               /*  nothing to do  */
           }
       }
       else
       {
           /*入参防空*/
       }

       return t_u8Ret;
}

UINT32 g_TimeCounter = 1; /*用SFP协议 计数*/
struc_Unify_Info sfpStru;/* SFP协议用到的结构体*/

/* 对SFP协议进行初始化*/
ENUM_CM_RETURN SFP_Protocol_INIT(void)
{
      ENUM_CM_RETURN t_u8Ret = ENUM_CM_FALSE;
      UINT16 SFPProId = 0;

      UINT8 tempRtn = 0;

      g_TimeCounter =Plat_HardSync_GetCurrWorkCycle();


      /* 将CPUID转换为SFP协议类型ID*/
      SFPProId =  0x7105;


      if(SFPProId != 0xFFFF)
      {
          /* DMI的SFP协议上电初始化 */
          sfpStru.LocalID = (UINT8)SFPProId;
          sfpStru.LocalType = (UINT8)(SFPProId>>8);
          sfpStru.TimeCounter = &g_TimeCounter;

        sfpStru.RedundTimeout = 0xFFFF;
        sfpStru.OutputTimeout = 0xFFFF;
        sfpStru.DataBase = vSfpInnetDataBuf;

        sfpStru.MaxNumLink = 2;
        sfpStru.MaxNumPerCycle = 2;
        sfpStru.InputSize = 500;
        sfpStru.OutputSize = 500;
        sfpStru.OutnetQueueSize = 500;
        sfpStru.RecordArraySize = 500;
        sfpStru.CycleNum = &g_TimeCounter;


        tempRtn = ProtclInit(NULL,0,NULL,&sfpStru);
        if(1 != tempRtn)
         {
            t_u8Ret = ENUM_CM_FALSE;
         }
        else
        {
            t_u8Ret = ENUM_CM_TRUE;
        }
      }




    return t_u8Ret;
}

/***************************************************************************************
* 功能描述:   将输出发送需要封装SFP协议的数据，从原队列中提取，放到SFP封装协议队列中
* 输入参数:  struc_Unify_Info * pBefor_Sfp_Stru  SFP协议结构体
* 输入输出参数:     无
* 输出参数:
* 全局变量:         ...
* 返回值:
* 修改记录：
****************************************************************************************/
void Send_Sfp_Befor_Hander(struc_Unify_Info * pBefor_Sfp_Stru)
{
     UINT16 u16SendSize = 6;
     UINT8 arr8SendBuff[1024] = {0,1,2,3,4,5};

    g_TimeCounter = Plat_HardSync_GetCurrWorkCycle(); /* 将SFP协议的计数加1*/

    if(NULL != pBefor_Sfp_Stru)
    {
        (void)QueueClear(&(pBefor_Sfp_Stru->OutputDataQueue));

        SFP_Protocol_Pack(arr8SendBuff,u16SendSize,MSCP_BOARD_MASK_MAIN_SFP_TC1,&(pBefor_Sfp_Stru->OutputDataQueue)); /*20220905 zsq add*/

    }
    else
    {
        ;
    }
}

/***************************************************************************************
* 功能描述:   从处理后的SFP协议队列中将不同类型数据提取到相关队列中并将DMI数据封装FFFE
* 输入参数:  struc_Unify_Info * pAfter_Sfp_Stru  SFP协议处理后的结构体
* 输入输出参数:     无
* 输出参数:
* 全局变量:         ...
* 返回值:
* 修改记录：
****************************************************************************************/
//void Send_Sfp_After_Hander(struc_Unify_Info * pAfter_Sfp_Stru)
//{
//    UINT32 AllAfterSFPLen = 0; /* 用于存放SFP协议数据的总长度*/
//    UINT32 AfterSfpDataLen = 0;/* 用于存放过完SFP协议后非链路信息部分的有效数据长度*/
//    UINT16 SfpNodeDataLen = 0; /* 存放Sfp链路节点数据长度*/
//    UINT16 sfpDataTypeID  = 0; /* 数据类型和ID*/
//    UINT32 index = 0u;
//    QueueStruct* p_stpSendQueue = NULL;/* 用于存放DMI不及时透传队列指针*/
//    QueueStruct* p_stpCylceSendQueue = NULL;/* 20220905 add*/
//    UINT16 addDoubleCrcStruIndex = 0;
//    STRU_ADD_DOUBLECRC stru_add_double_crc[STRUDOUBLECRCLEN] = {0};
//    ENUM_CM_RETURN FuncRtn = ENUM_CM_FALSE;
//    static UINT8 SendSfpAnswerBuff[100] = {0,}; /* 用于存放各个执行板卡向主机板发送的应答数据*/
//    static UINT8 SendSfpAnswerNum  = 0;/*由于暂时执行板卡的CPU2收不到主机板的数据请求数据，所以默认应答消息保留两个周期*/
//    /** v7.0.1：Sfp应答消息最多存储的周期数，缺省值为2*/
//    UINT8 SendSfpAnswerMaxCycleNum = 2u;

//    CM_ASSERT((NULL == pAfter_Sfp_Stru), __FILE__, __FUNCTION__, __LINE__);
//    /** 如果板卡当前周期时间小于缺省值*/
//    if(g_PLFM_CycleTime_Config.MainCycleTime < MAINCYCLE)
//    {
//        /** SendSfpAnswerMaxCycleNum需要乘以周期之间的比值*/
//        SendSfpAnswerMaxCycleNum = SendSfpAnswerMaxCycleNum * (MAINCYCLE / g_PLFM_CycleTime_Config.MainCycleTime);
//    }
//    /** 如果板卡当前周期时间大于等于缺省值*/
//    else
//    {
//        /** SendSfpAnswerMaxCycleNum需要除以周期之间的比值*/
//        SendSfpAnswerMaxCycleNum = SendSfpAnswerMaxCycleNum / (g_PLFM_CycleTime_Config.MainCycleTime / MAINCYCLE);
//    }
//    SendSfpAnswerMaxCycleNum += 1;
//    p_stpSendQueue = GetOutPutCmpDataBufferQueue();//比较前缓冲队列
//    p_stpCylceSendQueue = GetcycleOutPutCmpDataBufferQueue();

//    if(SendSfpAnswerNum >= SendSfpAnswerMaxCycleNum) /*当已将应答消息在发送队列中存放了两个周期后，不再发送*/
//    {
//        SendSfpAnswerNum = 0;                       /* 将发送的计数清0*/
//        Sfp_570_Answer_Flag = NOT_SEND_SFP_ANSWER;  /* 将发送标志记为不向外发送 */
//    }
//    else
//    {
//        ;/* do nothing */
//    }

//    /* 过完协议后的队列形式如下*/
//    /*4字节总长度 + {&4字节长度 +|&2字节长度+1字节协议类型 + 1字节目的类型 + 1字节目的ID + 2字节保留+网络上发送数据&| +|&n个&|&} +{链路数据} */

//    /* 读取有效数据中的4字节总长度*/
//    AllAfterSFPLen = LongFromChar(&(pAfter_Sfp_Stru->PrtclSndBuff[index]));
//    index += 4u;
//    /* 读取有效数据中的4字节sfp数据长度*/
//    AfterSfpDataLen = LongFromChar(&(pAfter_Sfp_Stru->PrtclSndBuff[index]));
//    index += 4u;

//    while((index - 8 ) < AfterSfpDataLen)
//    {
//        /* 解析每个sfp链路节点数据的长度*/
//        SfpNodeDataLen = ShortFromChar(&(pAfter_Sfp_Stru->PrtclSndBuff[index]));
//        index += 2;

//        index += 1 ; /* 协议类型字节，用不到，将此字节跳过*/

//        sfpDataTypeID = ShortFromChar(&(pAfter_Sfp_Stru->PrtclSndBuff[index]));
//        index += 2; /* 类型和ID所占字节长度*/

//        index += 2;  /* 两字节预留数据所占长度*/

//        if((MSCP_BOARD_MASK_MAIN_SFP_TC1 == sfpDataTypeID) || (MSCP_BOARD_MASK_MAIN_SFP_TC2 == sfpDataTypeID))
//        {
//            if((SEND_SFP_ANSWER == Sfp_570_Answer_Flag)&&(0 == SendSfpAnswerNum))
//            {
//                //Sfp_570_Answer_Flag = SEND_SFP_ANSWER;
//                /* 将应答数据放到暂存的buff中*/
//                CM_Memcpy(SendSfpAnswerBuff, SfpNodeDataLen + 2,
//                          &(pAfter_Sfp_Stru->PrtclSndBuff[index-7]), SfpNodeDataLen + 2);

//            }
//            else
//            {
//                ;/* do nothing */
//            }

//            if(SEND_SFP_ANSWER == Sfp_570_Answer_Flag)
//            {
//                index += SfpNodeDataLen-5;
//            }
//            else
//            {
//                stru_add_double_crc[addDoubleCrcStruIndex].sfpBufLen = SfpNodeDataLen - 5;
//                stru_add_double_crc[addDoubleCrcStruIndex].sfpTypeId = sfpDataTypeID;
//                stru_add_double_crc[addDoubleCrcStruIndex].fffeFlag = NOT_USE_FFFE;/*570 默认不适用fffe*/

//                CM_Memcpy(&stru_add_double_crc[addDoubleCrcStruIndex].sfpSendBuf[0], SfpNodeDataLen - 5, &(pAfter_Sfp_Stru->PrtclSndBuff[index]), SfpNodeDataLen - 5);
//                addDoubleCrcStruIndex++;

//                index += SfpNodeDataLen-5;
//            }
//        }
//        else
//        {
//            stru_add_double_crc[addDoubleCrcStruIndex].sfpBufLen = SfpNodeDataLen - 5;
//            stru_add_double_crc[addDoubleCrcStruIndex].sfpTypeId = sfpDataTypeID;
//            stru_add_double_crc[addDoubleCrcStruIndex].fffeFlag = NOT_USE_FFFE;/*570 默认不适用fffe*/

//            CM_Memcpy(&stru_add_double_crc[addDoubleCrcStruIndex].sfpSendBuf[0], SfpNodeDataLen - 5, &(pAfter_Sfp_Stru->PrtclSndBuff[index]), SfpNodeDataLen - 5);
//            addDoubleCrcStruIndex++;

//            index += SfpNodeDataLen-5;
//        }
//    }


//    if(SEND_SFP_ANSWER == Sfp_570_Answer_Flag)
//    {
//        SendSfpAnswerNum = SendSfpAnswerNum +1;

//        SfpNodeDataLen = ShortFromChar(&SendSfpAnswerBuff[0]);

//        sfpDataTypeID = ShortFromChar(&SendSfpAnswerBuff[3]);

//        stru_add_double_crc[addDoubleCrcStruIndex].sfpBufLen = SfpNodeDataLen - 5;
//        stru_add_double_crc[addDoubleCrcStruIndex].sfpTypeId = sfpDataTypeID;
//        stru_add_double_crc[addDoubleCrcStruIndex].fffeFlag = NOT_USE_FFFE;/*570 默认不适用fffe*/

//        CM_Memcpy(&stru_add_double_crc[addDoubleCrcStruIndex].sfpSendBuf[0], SfpNodeDataLen - 5, &SendSfpAnswerBuff[7], SfpNodeDataLen - 5);
//        addDoubleCrcStruIndex++;

//    }
//    else
//    {
//        ;
//    }

//    if(0 != addDoubleCrcStruIndex )
//    {
//        FuncRtn = PLFM_AddDoubleCrc(&stru_add_double_crc[0],addDoubleCrcStruIndex);

//        CM_ASSERT((ENUM_CM_TRUE != FuncRtn), __FILE__, __FUNCTION__, __LINE__);

//        FuncRtn = StrucDoubleCrcToQueue(&stru_add_double_crc[0], addDoubleCrcStruIndex,p_stpSendQueue, p_stpCylceSendQueue);

//        CM_ASSERT((ENUM_CM_TRUE != FuncRtn), __FILE__, __FUNCTION__, __LINE__);

//    }
//    else
//    {
//        /*无操作*/
//    }
//}
//#endif
//#if IS_5728_MCP ==1
//void Send_Sfp_After_Hander(struc_Unify_Info * pAfter_Sfp_Stru)
//{

//    UINT8 CallFuncRtn = 0;
//    UINT32 AllAfterSFPLen = 0; /* 用于存放SFP协议数据的总长度*/
//    UINT32 AfterSfpDataLen = 0;/* 用于存放过完SFP协议后非链路信息部分的有效数据长度*/
//    UINT16 SfpLinkLen = 0;/*存放Sfp链路信息长度*/
//    QueueStruct* p_stpSendQueue = NULL;/* 用于存放DMI不及时透传队列指针*/
//    static UINT8 SendSfpAnswerBuff[100] = {0,}; /* 用于存放各个执行板卡向主机板发送的应答数据*/
//    static UINT8 SendSfpAnswerNum  = 0;/*由于暂时执行板卡的CPU2收不到主机板的数据请求数据，所以默认应答消息保留两个周期*/
//    if(NULL != pAfter_Sfp_Stru)
//    {
//#if IS_5728_MCP ==1
//           p_stpSendQueue = Get_SEND_After_SFP_Queue();

//#elif ((IS_570_SIG ==1)||(IS_570_IPB ==1)||(IS_570_OPB ==1))
//          p_stpSendQueue = GetOutPutCmpDataBufferQueue();//比较前缓冲队列

//#endif
//          CM_Memset(SFPFollowData,0,(UINT32)(1024*3)); /*将跟随数据流清零*/
//          /* 过完协议后的队列形式如下*/
//         /*4字节总长度 + {&4字节长度 +|&2字节长度+1字节协议类型 + 1字节目的类型 + 1字节目的ID + 2字节保留+网络上发送数据&| +|&n个&|&} +{链路数据} */
//         /* 读取有效数据中的4字节总长度*/
//         AllAfterSFPLen = LongFromChar(&(pAfter_Sfp_Stru->PrtclSndBuff[0]));
//         DarkLogPrint(ENUM_LOG_PROMPT,&AllAfterSFPLen,LOGTYPE_OUTAFTSFPLEN,4);


//#if 0/*协议调试日志打印，调试是时可以打开，不要删除  zfr*/
//CM_UINT16 sfpLogLen = 0;
//    sfpLogLen = CM_ShortFromChar(pAfter_Sfp_Stru->RecordArray);
//    PlfmLogPrintStr(ENUM_LOG_PROMPT,"sfpLogLen = %d(d)", sfpLogLen);
//    PrintScreenArray_4(&pAfter_Sfp_Stru->RecordArray[0], sfpLogLen + 2);
//#endif
//         if((SEND_SFP_ANSWER == Sfp_570_Answer_Flag)&&(0u == SendSfpAnswerNum))
//         {

//             /* 将应答数据放到暂存的buff中*/
//             (void)CM_Memcpy(SendSfpAnswerBuff, AllAfterSFPLen + 4u,
//                       &(pAfter_Sfp_Stru->PrtclSndBuff[0]), AllAfterSFPLen + 4u);

//         }
//         else
//         {
//             /* code */
//         }
         
//         if (0u != AllAfterSFPLen)
//         {
//             AfterSfpDataLen = LongFromChar(&(pAfter_Sfp_Stru->PrtclSndBuff[4])); /* 将不包含链路信息的数据长度接析出*/
//             SfpLinkLen = (UINT16)(AllAfterSFPLen - AfterSfpDataLen - 4u);/* 将数据总长度减去SFP的长度，数据总长度减去数据长度，以及4个字节的数据长度的长度*/
//             /* 将2个字节的数据长度放到链路信息数据流中*/
//             ShortToChar(SfpLinkLen+4u,&SFPFollowData[0]);
//             LongToChar(g_TimeCounter,&SFPFollowData[2]);
//             /* 将SFP的链路数据长度数据存放到静态变量中,数据流格式为2个字节长度+链路信息*/
//             (void)CM_Memcpy(&SFPFollowData[6],SfpLinkLen,&(pAfter_Sfp_Stru->PrtclSndBuff[4u+4u+AfterSfpDataLen]),SfpLinkLen);
//              if(SendSfpAnswerNum >= 2u) /*当已将应答消息在发送队列中存放了两个周期后，不再发送*/
//             {
//                 SendSfpAnswerNum = 0;/* 将发送的计数清0*/
//                 Sfp_570_Answer_Flag = NOT_SEND_SFP_ANSWER;/*将发送标志记为不向外发送 */

//             }
//             else
//             {
//                 /* do nothing */
//             }
             
//            if(SEND_SFP_ANSWER == Sfp_570_Answer_Flag)
//             {
//                SendSfpAnswerNum = SendSfpAnswerNum +1u;
//                 /* 将封装完SFP协议的数据存放到过完SFP协议后的队列中*/
//               /* 跟据数据总长度将数据存放到封装SFP协议后队列*/
//               CallFuncRtn = QueueWrite((UINT32)(SEND_SFP_ANSWER_LEN + 4), SendSfpAnswerBuff, p_stpSendQueue);

//             }
//             else
//             {
//                 /* 将封装完SFP协议的数据存放到过完SFP协议后的队列中*/
//                  /* 跟据数据总长度将数据存放到封装SFP协议后队列*/
//                  CallFuncRtn = QueueWrite(AllAfterSFPLen + 4u, &(pAfter_Sfp_Stru->PrtclSndBuff[0]), p_stpSendQueue);
//             }
//             if (1u == CallFuncRtn)
//             {
//                 /*将数据写入到过SFP协议后的队列中 成功*/
//             }
//             else
//             {
//                 /* 数据写入不正确*/
//             }
//         }
//         else
//         {
//             /* do nothing */
//         }
         
//    }
//}
//#endif
///***************************************************************************************
//* 功能描述:      将解析SFP协议队列的数据，放到SFP协议队列中
//* 输入参数:      无
//* 输入输出参数:    struc_Unify_Info * pBefore_Sfp_Stru sfp协议数据结构体
//* 输出参数:      无
//* 全局变量:
//* 返回值:
//* 修改记录：
//****************************************************************************************/
//void Recv_SFP_Before_Handler(struc_Unify_Info * pBefore_Sfp_Stru)
//{
//    QueueStruct* t_stpTmpQueue= NULL;
//    UINT32 dmiQueueLen = 0;
//    UINT8 t_u8TmpRet1 = 0;
//#if defined(CCS_5728)
//     UINT8 InInentDataBuff[1024*14] = {0};/*2020.12.14--sgz--输入数据同步，数据量增大*//*注意，此处570需要修改源来为6*/
//#else
//     UINT8 InInentDataBuff[1024*2] = {0};
//#endif
//    UINT16 InInentDataLen = 0;

//    if(NULL != pBefore_Sfp_Stru)
//    {
//        pBefore_Sfp_Stru->RadamSN = Plat_HardSync_GetCurrWorkCycle();
//        /* 内网数据格式如下*/
//        /*|2字节长度|2字节BoardId|n字节数据（*n*）
//         * |2字节长度|2字节BoardId|n字节数据（*n*）
//         * |2字节长度|2字节BoardId|n字节数据（*n*）
//         * |2字节长度|2字节BoardId|n字节数据（*n*）*/

//         (void)QueueClear(&pBefore_Sfp_Stru->OutnetQueueA);

//#if IS_5728_MCP==1
//         /* 对于主机板将DMI需要解SFP的数据放到解SFP协议之前队列中*/
//         t_stpTmpQueue = (QueueStruct*)(GetPwlPreTransSfpQueue());
//         dmiQueueLen = CM_QueueStatus(t_stpTmpQueue);

//         if(0u != dmiQueueLen)
//         {
//             /*将预处理后的DMI的SFP数据放到协议处理前队列中*/
//             (void)QueueCopyAdd(&(pBefore_Sfp_Stru->OutnetQueueA),t_stpTmpQueue);
//         }
//         else
//         {
//             /* do nothing*/
//         }
//#endif
//         /*从内网接收数据队列中，将需要过SFP的数据提取出数据流形式*/
//        t_u8TmpRet1 = RecvInInnet_pre_Treated_Handler(&InInentDataBuff[0],&InInentDataLen);

//        if(1u == t_u8TmpRet1)
//        {
//            /* 将内网数据存放到需要过SFP协议的队列中*/
//            (void)QueueWrite(InInentDataLen,&InInentDataBuff[0],&(pBefore_Sfp_Stru->OutnetQueueA));

//        }
//        else
//        {
//            /* code */
//        }

//        /* 清空需要存放解协议后的队列*/
//        (void)QueueClear(&pBefore_Sfp_Stru->DataToApp);
//    }
//    else
//    {
//        /*do nothing*/
//    }
//}
///***************************************************************************************
//* 功能描述:      将解析完SFP协议的数据，根据数据类型不同，将数据放到不同队列中
//* 输入参数:      无
//* 输入输出参数:    struc_Unify_Info * pAfter_Sfp_Stru sfp协议结构体
//* 输出参数:      无
//* 全局变量:
//* 返回值:
//* 修改记录：
//****************************************************************************************/
//void Recv_SFP_After_Handler(struc_Unify_Info * pAfter_Sfp_Stru)
//{
//   /* 将解完SFP协议的透传数据放到此队列中*/
//    QueueStruct* t_PwlMscpToApp_Queue = NULL;
//#if ((IS_570_SIG ==1)||(IS_570_IPB ==1)||(IS_570_OPB ==1))
//    QueueStruct* t_PwlAfterSFPInnet_Queue = NULL;
//#endif
//    QueueStruct* t_u8FreeDate = NULL;
//    UINT16 t_u16Len = 0; /* 用于存放队列中存放数据的第一个长度*/
//    UINT16 DataLen = 0; /* 向队列中存放数据的数据长度*/
//    UINT16 DataTypeID= 0; /* 用于存放协议中提取数据的类型和ID*/
//    UINT16 SideType = 0; /* 用于存放端类型的字节*/
//    UINT8 InfoHead[7] = {0,};/* 用于存储解完SFP协议后的前7个字节头*/
//    UINT8 SFPDataBuf[1000] ={0,};/* 用于暂存SFP协议解析后的数据流*/
//    UINT8 t_arr8Buff_2Byte[2] = {0,};
//    UINT32 AfterSFPDatalen = 0;/*用于存放解完Sfp协议后，数组总长度*/
//    UINT32 AfterSfpOffset = 0;
//    UINT16 InfoLength = 0;
//    UINT8 SfpLinkState = 0u;
//#ifdef CCS_570
//    UINT8 SfpLinkStateTC1 = 0u;
//    UINT8 SfpLinkStateTC2 = 0u;
//    UINT8 SfpLinkStateATO1 = 0u;
//    UINT8 SfpLinkStateATO2 = 0u;
//#endif
//    UINT8 t_readrtn = 0;

//    if(NULL != pAfter_Sfp_Stru)
//    {
//#if 0/*协议调试日志打印，调试是时可以打开，不要删除 zfr*/
//        CM_UINT16 sfpLogLen = 0;
//        sfpLogLen = CM_ShortFromChar(pAfter_Sfp_Stru->RecordArray);
//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"Rec_sfpLogLen = %d(d)\r\n", sfpLogLen);
//        PlfmLogPrintArray(ENUM_LOG_PROMPT, &pAfter_Sfp_Stru->RecordArray[0], sfpLogLen + 2);
//#endif
//#if IS_5728_MCP ==1

//#else
//        SfpLinkStateTC1 = ProtclCheckLnkStatus((MSCP_BOARD_MASK_MAIN_SFP_TC1>>8)&0xFF, MSCP_BOARD_MASK_MAIN_SFP_TC1&0xFF, pAfter_Sfp_Stru);
//        SfpLinkStateTC2 = ProtclCheckLnkStatus((MSCP_BOARD_MASK_MAIN_SFP_TC2>>8)&0xFF, MSCP_BOARD_MASK_MAIN_SFP_TC2&0xFF, pAfter_Sfp_Stru);
//        SfpLinkStateATO1 = ProtclCheckLnkStatus((MSCP_BOARD_MASK_ATO_TC1_A>>8)&0xFF, MSCP_BOARD_MASK_ATO_TC1_A&0xFF, pAfter_Sfp_Stru);
//        SfpLinkStateATO2 = ProtclCheckLnkStatus((MSCP_BOARD_MASK_ATO_TC2_A>>8)&0xFF, MSCP_BOARD_MASK_ATO_TC2_A&0xFF, pAfter_Sfp_Stru);

//        DarkLogPrint(ENUM_LOG_PROMPT,&SfpLinkStateTC1,LOGTYPE_SFPLINKSTATETC1,1);
//        DarkLogPrint(ENUM_LOG_PROMPT,&SfpLinkStateTC2,LOGTYPE_SFPLINKSTATETC2,1);
//        DarkLogPrint(ENUM_LOG_PROMPT,&SfpLinkStateATO1,LOGTYPE_SfpLINKSTATEATO1,1);
//        DarkLogPrint(ENUM_LOG_PROMPT,&SfpLinkStateATO2,LOGTYPE_SfpLINKSTATEATO2,1);

//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"prtclStatus = 0x%x 2 = 0x%x  ATO1 = 0x%x ATO2 = 0x%x\r\n", SfpLinkStateTC1,SfpLinkStateTC2,SfpLinkStateATO1,SfpLinkStateATO2);
//        if((0x33 == SfpLinkStateTC1) || (0x33 == SfpLinkStateTC2))
//        {
//            Sfp_570_Answer_Flag = SEND_SFP_ANSWER;
//        }
//        else
//        {
//            /* code */
//        }
//#endif

//        AfterSFPDatalen = QueueStatus(&pAfter_Sfp_Stru->DataToApp);
//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"After  Recv SFPDatalen %d\n", AfterSFPDatalen);
//        DarkLogPrint(ENUM_LOG_PROMPT,&AfterSFPDatalen, LOGTYPE_AFTSFPLEN,4);

//        /*|&+2字节长度+4字节源类型ID(小端)+1字节lifetime+应用数据&|+|&?n个?&|*/
//        if(0u !=AfterSFPDatalen)
//        {
//          /* 一直从队列中读取数据，直至将数据读空为止*/
//           while(AfterSfpOffset < AfterSFPDatalen)
//           {
//                /*首先读出头7个字节字符的数据，这7个字节表示该帧数据的一些头信息*/
//                t_readrtn = QueueRead(7, InfoHead, &pAfter_Sfp_Stru->DataToApp);
//                if(1u == t_readrtn)
//                {
//                    AfterSfpOffset = AfterSfpOffset + 7u;//2字节长度+4字节源类型ID(小端)+1字节lifetime = 7
//                    InfoLength = ShortFromChar(&InfoHead[0]);/*该数据帧的长度*/

//                    /* 将数据的类型和ID解析*/
//                    DataTypeID = ShortFromCharLE(&InfoHead[4]);
//                    SfpLinkState = ProtclCheckLnkStatus((UINT8)((DataTypeID>>8)&0xFFu),(UINT8)(DataTypeID&0xFFu),pAfter_Sfp_Stru);

//                    /*根据数据的头信息识别DMI是1端还是2端数据将数据存放到解析后的透传队列*/
//                    t_readrtn = QueueRead((UINT32)(InfoLength-5u), (UINT8*)SFPDataBuf, &pAfter_Sfp_Stru->DataToApp);
//                    if(1u == t_readrtn)
//                    {
//                        DataLen = InfoLength-5u;
//                        AfterSfpOffset = AfterSfpOffset + DataLen;
//#if IS_5728_MCP ==1

//                        t_PwlMscpToApp_Queue = (QueueStruct*)(GetMscpToAppBuff_Queue());
//                        /* 将数据按照 2个字节长S度、2个字节类型ID+2个字节端类型+1个字节预留写入到平台与应用的队列中*/
//                         t_u16Len= (UINT16)(DataLen + DATA_TYPE_SIZE + SIDE_TYPE_SIZE +FREE_DATA_SIZE); /* DMI应用数据长度+2字节类型 + 2个字节端类型+1字节预留 */
//                        /* 两字节长度写入队列*/
//                        ShortToChar(t_u16Len,t_arr8Buff_2Byte);
//                        (void)QueueWrite(2 ,t_arr8Buff_2Byte ,t_PwlMscpToApp_Queue);
//                        /* 两字节类型ID写入队列*/
//                        ShortToChar(DataTypeID,t_arr8Buff_2Byte);
//                        (void)QueueWrite(2 ,t_arr8Buff_2Byte ,t_PwlMscpToApp_Queue);
//                        /* 两个字节端类型写入对列，由于内网和DMI用不到此字节，因此写入0*/
//                        ShortToChar(SideType,t_arr8Buff_2Byte);
//                        (void)QueueWrite(2 ,t_arr8Buff_2Byte ,t_PwlMscpToApp_Queue);
//                        /* 1个字节预留*/
//                        (void)QueueWrite(1 ,(QueueElement *)(&t_u8FreeDate),t_PwlMscpToApp_Queue);
//                        /* 将buff写入队列*/
//                        (void)QueueWrite(DataLen ,&SFPDataBuf[0],t_PwlMscpToApp_Queue);
//                        /*gh-v6.0.1-test*/


//#elif ((IS_570_SIG ==1)||(IS_570_IPB ==1)||(IS_570_OPB ==1))

//                           /***************20220905 zsq add ato通信节点数据解析转储**************/
//                           /* 对于570的板子，如果数据来源为主机板*/
//                           if((MSCP_BOARD_MASK_MAIN_SFP_TC1 == DataTypeID) || (MSCP_BOARD_MASK_MAIN_SFP_TC2 == DataTypeID))
//                           {
//                               /* 将协议类型转换为板卡用的协议类型*//*atp*/
//                               DataTypeID = MSCP_CPU_ID_MAIN_TC1_A1;
//                           }
//                           else if((MSCP_BOARD_MASK_ATO_TC1_A == DataTypeID) || (MSCP_BOARD_MASK_ATO_TC2_A == DataTypeID))
//                           {
//                               /* 将协议类型转换为板卡用的协议类型*//*ato*/
//                               DataTypeID = MSCP_BOARD_ATO_AOM_TC1_A1;
//                           }
//                           else
//                           {
//                               DataTypeID = (CM_UINT16)0u;
//                           }
//                           if((CM_UINT16)0u != DataTypeID)
//                           {
//                               t_PwlAfterSFPInnet_Queue = GetRcvInnetDataQueue();
//                               /* 根据以下格式将数据存放到内网数据队列中*/
//                               /*|2字节长度|2字节BoardId|n字节数据）*/
//                                t_u16Len = DataLen +2; /* Len2+ BoardIdLen*/
//                                /* 两字节长度写入队列*/
//                               ShortToChar(t_u16Len,t_arr8Buff_2Byte);
//                               QueueWrite(2, t_arr8Buff_2Byte, t_PwlAfterSFPInnet_Queue);
//                               /* 两字节板卡ID写入队列,此板卡ID为SFP协议用到的板卡ID*/
//                               ShortToChar(DataTypeID, t_arr8Buff_2Byte);
//                               QueueWrite(2, t_arr8Buff_2Byte, t_PwlAfterSFPInnet_Queue);
//                               /* 将buff写入队列*/
//                               QueueWrite(DataLen ,&SFPDataBuf[0], t_PwlAfterSFPInnet_Queue);
//                           }
//                           /***************20220905 zsq add ato通信节点数据解析转储**************/
//#if 0
//                           /***************20220905 zsq del**************/
//                           if((MSCP_BOARD_MASK_MAIN_SFP_TC1 == DataTypeID) || (MSCP_BOARD_MASK_MAIN_SFP_TC2 == DataTypeID))
//                           {
//                               /* 将协议类型转换为板卡用的协议类型*/
//                               DataTypeID = MSCP_CPU_ID_MAIN_TC1_A1;
//                               t_PwlAfterSFPInnet_Queue = GetRcvInnetDataQueue();
//                               /* 根据以下格式将数据存放到内网数据队列中*/
//                               /*|2字节长度|2字节BoardId|n字节数据）*/
//                                t_u16Len = DataLen +2; /* Len2+ BoardIdLen*/
//                                /* 两字节长度写入队列*/
//                               ShortToChar(t_u16Len,t_arr8Buff_2Byte);
//                               QueueWrite(2 ,t_arr8Buff_2Byte ,t_PwlAfterSFPInnet_Queue);
//                               /* 两字节板卡ID写入队列,此板卡ID为SFP协议用到的板卡ID*/
//                               ShortToChar(DataTypeID,t_arr8Buff_2Byte);
//                               QueueWrite(2 ,t_arr8Buff_2Byte ,t_PwlAfterSFPInnet_Queue);
//                               /* 将buff写入队列*/
//                               QueueWrite(DataLen ,&SFPDataBuf[0],
//                                          t_PwlAfterSFPInnet_Queue);
//                           }
//#endif
//                           else
//                           {
//                               ;/*do nothing*/
//                           }
//#endif
//                   }
//                   else
//                   {
//                       ;/*do nothing*/
//                   }
//                }
//                else
//                {
//                    ;/*do nothing */
//                }
//           }
//        }
//        else
//        {
//            ;/*do nothing*/
//        }
//    }
//    else
//    {
//        ;/* do_nothing */
//    }
//}

//#if IS_5728_MCP==1
///***************************************************************************************
//* 功能描述:   将发送的数据需要封装RSSP-1协议的数据写入到输出的过协议前的队列中
//*          根据是否需要封装RSSP-1进行不同处理
//* 输入参数:         无
//* 输入输出参数:     QueueStruct* p_stpQueueToPctlAdpter RSSP-1封装协议前的队列
//* 输出参数:         无
//* 全局变量:
//* 返回值:
//****************************************************************************************/
//UINT8 Send_Rssp_Before_Handler(QueueStruct* p_stpQueueToPctlAdpter,QueueStruct* p_stpRsspQueue)
//{
//    UINT8 t_u8Ret = 0 ;
//    UINT32 QueueLen = 0;
//    QueueStruct* t_stpTmpQueue= NULL;

//    if((NULL != p_stpQueueToPctlAdpter)&&(NULL != p_stpRsspQueue))
//    {
//        /*清空入参队列*/
//        (void)QueueClear(p_stpQueueToPctlAdpter);
//        /*加协议前标准数据格式    |&+2字节长度+1字节目的类型+1字节目的ID+2字节预留+应用数据&|+|&?n个?&|*/
//        /*将过RSSP_1协议前队列数据放到适配层解协议前*/
//        t_stpTmpQueue = p_stpRsspQueue;
//        /*gh-test*/
//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"--Before p_stpQueueToPctlAdpter len =%d\r\n",CM_QueueStatus(p_stpQueueToPctlAdpter));
//        (void)QueueCopyAdd(p_stpQueueToPctlAdpter,t_stpTmpQueue);
//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"--After p_stpQueueToPctlAdpter len =%d\r\n",CM_QueueStatus(p_stpQueueToPctlAdpter));
//        QueueLen = CM_QueueStatus(p_stpQueueToPctlAdpter);
//        DarkLogPrint(ENUM_LOG_PROMPT,&QueueLen,LOGTYPE_OUTBEFINRSSP1LEN,4);
//        (void)QueueClear(t_stpTmpQueue);

//        t_u8Ret = 1;
//    }
//    else
//    {
//        /* do nothing */
//    }

//    return t_u8Ret;
//}


///**
//* @brief   应用输出的给后备通道的外网数据封装RSSP-1写入到 过协议前的数据队列
//* @details   应用输出的给后备通道的外网数据封装RSSP-1写入到 过协议前的数据队列
//* @param  p_stpQueueToPctlAdpter RSSP-1封装协议前的队列 取值范围：非空
//* @param  p_stpRsspQueue  应用数据队列 指针  取值范围 ：非空
//* @return  0：失败  1：成功
//*/
//UINT8 Send_RsspBack_Before_Handler(QueueStruct* p_stpQueueToPctlAdpter,QueueStruct* p_stpRsspQueue)
//{
//    UINT8 t_u8Ret = 0 ;
//    UINT32 QueueLen = 0;
//    QueueStruct* t_stpTmpQueue= NULL;

//    if((NULL != p_stpQueueToPctlAdpter)&&(NULL != p_stpRsspQueue))
//    {
//        /*清空入参队列*/
//        (void)QueueClear(p_stpQueueToPctlAdpter);
//        /*加协议前标准数据格式    |&+2字节长度+1字节目的类型+1字节目的ID+2字节预留+应用数据&|+|&?n个?&|*/
//        /*将过RSSP_1协议前队列数据放到适配层解协议前*/
//        t_stpTmpQueue = p_stpRsspQueue;
//        /*gh-test*/
//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"--Before Back p_stpQueueToPctlAdpter len =%d\r\n",CM_QueueStatus(p_stpQueueToPctlAdpter));
//        (void)QueueCopyAdd(p_stpQueueToPctlAdpter,t_stpTmpQueue);
//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"--After Back p_stpQueueToPctlAdpter len =%d\r\n",CM_QueueStatus(p_stpQueueToPctlAdpter));
//        QueueLen = CM_QueueStatus(p_stpQueueToPctlAdpter);

//        (void)QueueClear(t_stpTmpQueue);

//        t_u8Ret = 1;
//    }
//    else
//    {
//        /* do nothing */
//    }

//    return t_u8Ret;
//}

///***************************************************************************************
//* 功能描述:   将封装RSSP-1协议后的数据按照发送数据格式存放到发送队列中
//* 输入参数:       UINT8* p_p8ArrFromPctlAdpter 封装完协议后的数据
//* 输入输出参数:     无
//* 输出参数:         无
//* 全局变量:
//* 返回值:
//****************************************************************************************/
//UINT8 Send_Rssp_After_Handler(UINT8* p_p8ArrFromPctlAdpter,QueueStruct* p_stpRsspQueue)
//{
//    /*4字节总长度+|&2字节长度+1字节协议类型+1字节目的类型+1字节目的ID+2字节预留+网络上发送数据&|+|&?n个?&|*/
//    UINT8 t_u8Ret = 0 ;
//    UINT8 CallFuncRtn = 0;
//    UINT32 t_u32TotalLen = 0 ;
//    QueueStruct* t_stpSendqueue = NULL;

//    if((NULL != p_p8ArrFromPctlAdpter)&&(NULL != p_stpRsspQueue))
//    {
//        t_stpSendqueue = p_stpRsspQueue;
//        /* 4个字节总长度*/
//        t_u32TotalLen = LongFromChar(p_p8ArrFromPctlAdpter);
//        if (0u != t_u32TotalLen)
//        {
//            /*gh-test*/
//            DarkLogPrint(ENUM_LOG_PROMPT,&t_u32TotalLen,LOGTYPE_OUTAFTINRSSP1LEN,4);
//            PlfmLogPrintStr(ENUM_LOG_PROMPT, "--t_u32TotalLen = %d\r\n",t_u32TotalLen);
//            /* 跟据数据总长度将数据存放到封装RSSP-1协议后队列*/
//            CallFuncRtn = QueueWrite(t_u32TotalLen + 4u, p_p8ArrFromPctlAdpter, t_stpSendqueue);
//            if (1u == CallFuncRtn)
//            {
//                t_u8Ret = 1; /*将数据写入到过RSSP协议后的队列中 */
//            }
//            else
//            {
//                /* do nothing */
//            }
            

//        }
//        else
//        {
//            /* do nothing */
//        }
        
//    }
//    else
//    {
//        /* do nothing */
//    }
    
//    return t_u8Ret ;
//}


///**
//* @brief   后备通道过完RSSP-1协议的数据写到协议后数据队列
//* @details   后备通道过完RSSP-1协议的数据写到协议后数据队列
//* @param  p_stpQueueToPctlAdpter 过完协议的数据   取值范围：非空
//* @param  p_stpRsspQueue  存放过完协议的数据队列  取值范围 ：非空
//* @return  0：失败  1：成功
//*/
//UINT8 Send_RsspBack_After_Handler(UINT8* p_p8ArrFromPctlAdpter,QueueStruct* p_stpRsspQueue)
//{
//    /*4字节总长度+|&2字节长度+1字节协议类型+1字节目的类型+1字节目的ID+2字节预留+网络上发送数据&|+|&?n个?&|*/
//    UINT8 t_u8Ret = 0 ;
//    UINT8 CallFuncRtn = 0;
//    UINT32 t_u32TotalLen = 0 ;
//    QueueStruct* t_stpSendqueue = NULL;

//    if((NULL != p_p8ArrFromPctlAdpter)&&(NULL != p_stpRsspQueue))
//    {
//        t_stpSendqueue = p_stpRsspQueue;
//        /* 4个字节总长度*/
//        t_u32TotalLen = LongFromChar(p_p8ArrFromPctlAdpter);
//        if (0u != t_u32TotalLen)
//        {
//            PlfmLogPrintStr(ENUM_LOG_PROMPT, "--Back t_u32TotalLen = %d\r\n",t_u32TotalLen);
//            /* 跟据数据总长度将数据存放到封装RSSP-1协议后队列*/
//            CallFuncRtn = QueueWrite(t_u32TotalLen + 4u, p_p8ArrFromPctlAdpter, t_stpSendqueue);
//            if (1u == CallFuncRtn)
//            {
//                t_u8Ret = 1; /*将数据写入到过RSSP协议后的队列中 */
//            }
//            else
//            {
//                /* do nothing */
//            }


//        }
//        else
//        {
//            /* do nothing */
//        }

//    }
//    else
//    {
//        /* do nothing */
//    }

//    return t_u8Ret ;
//}
///***************************************************************************************
//* 功能描述:      外网的协议处理前准备解Rssp-1协议的对列
//* 输入参数:      无
//* 输入输出参数:    QueueStruct* p_stpQueueToPctlAdpter 过Rssp-1协议的的队列指针
//* 输出参数:      无
//* 全局变量:
//* 返回值:
//* 修改记录：
//****************************************************************************************/
//UINT8 Recv_Outnet_Before_Handler(QueueStruct* p_stpQueueToPctlAdpter)
//{

//    UINT8 t_u8Ret = 0 ;
//    UINT32 QueueLen = 0;
//    QueueStruct* t_stpTmpQueue= NULL;

//    /* 以下为新增队列，用于将不需要解RSSP-1协议的数据存放到平台与应用的接口队列里*/

//    if(NULL != p_stpQueueToPctlAdpter)
//    {
//        (void)QueueClear(p_stpQueueToPctlAdpter);

//        t_stpTmpQueue = (QueueStruct*)(GetPwlOutnetInQueue());
//        (void)QueueCopyAdd(p_stpQueueToPctlAdpter,t_stpTmpQueue);
//        QueueLen = CM_QueueStatus( p_stpQueueToPctlAdpter);
//        DarkLogPrint(ENUM_LOG_PROMPT,&QueueLen,LOGTYPE_BEFRSSP1LEN,4);
//        PlfmLogPrintStr(ENUM_LOG_PROMPT," outnet RSSP_1 before len = %d\n",CM_QueueStatus( p_stpQueueToPctlAdpter));
//        (void)QueueClear(t_stpTmpQueue);
//    }
//    else
//    {
//        /* do nothing */
//    }

//    return t_u8Ret;
//}

///**
//* @brief  后备数据通道数据主备解RSSP-1协议前处理
//* @details   后备数据通道数据主备解RSSP-1协议前处理
//* @param  p_stpQueueToPctlAdpter 过协议数据队列
//* @return 0失败 1成功
//*/
//UINT8 Recv_OutnetBack_Before_Handler(QueueStruct* p_stpQueueToPctlAdpter)
//{

//    UINT8 t_u8Ret = 0 ;
//    UINT32 QueueLen = 0;
//    QueueStruct* t_stpTmpQueue= NULL;

//    /* 以下为新增队列，用于将不需要解RSSP-1协议的数据存放到平台与应用的接口队列里*/

//    if(NULL != p_stpQueueToPctlAdpter)
//    {
//        (void)QueueClear(p_stpQueueToPctlAdpter);

//        t_stpTmpQueue = (QueueStruct*)(GetPwlOutnetBackInQueue());
//        (void)QueueCopyAdd(p_stpQueueToPctlAdpter,t_stpTmpQueue);
//        QueueLen = CM_QueueStatus(p_stpQueueToPctlAdpter);

//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"Back outnet RSSP_1 before len = %d\n",QueueLen);
//        (void)QueueClear(t_stpTmpQueue);
//    }
//    else
//    {
//        /* do nothing */
//    }

//    return t_u8Ret;
//}

///***************************************************************************************
//* 功能描述:      BTM的协议处理前准备解Rssp-1协议的队列
//* 输入参数:      无
//* 输入输出参数:    QueueStruct* p_stpQueueToPctlAdpter 过Rssp-1协议的的队列指针
//* 输出参数:      无
//* 全局变量:
//* 返回值:
//* 修改记录：
//****************************************************************************************/
//UINT8 Recv_IntnetRssp_Before_Handler(QueueStruct* p_stpQueueToPctlAdpter)
//{

//    UINT8 t_u8Ret = 0 ;
//    UINT32 QueueLen = 0;
//    QueueStruct* t_stpTmpQueue= NULL;


//    if(NULL != p_stpQueueToPctlAdpter)
//    {
//        (void)QueueClear(p_stpQueueToPctlAdpter);

//        /*gh-test:将内网RSSP队列数据存入新增的GetUdpInnetRsspQueue队列中*/
//        /*gh-ethernet-add:以太网通信时，内网RSSP协议数据走GetUdpInnetRsspQueue队列；PWL通信时，内网RSSP协议数据走GetPwlPreTransRsspQueue队列*/
//        t_stpTmpQueue = (QueueStruct*)(GetPwlPreTransRsspQueue());
//        (void)QueueCopyAdd(p_stpQueueToPctlAdpter,t_stpTmpQueue);
//        PlfmLogPrintStr(ENUM_LOG_PROMPT, "--length of GetPwlPreTransRsspQueue = %d\r\n",QueueStatus(p_stpQueueToPctlAdpter));
//        if(DRV_MSCP_ATP_ETHERNET == drvGetBoardType())
//        {
//            t_stpTmpQueue = (QueueStruct*)(GetUdpInnetRsspQueue());
//            PlfmLogPrintStr(ENUM_LOG_PROMPT, "--length of GetUdpInnetRsspQueue = %d\r\n",QueueStatus(t_stpTmpQueue));
//            (void)QueueCopyAdd(p_stpQueueToPctlAdpter,t_stpTmpQueue);
//            PlfmLogPrintStr(ENUM_LOG_PROMPT, "--length of GetUdpInnetRsspQueue and GetPwlPreTransRsspQueue = %d\r\n",QueueStatus(p_stpQueueToPctlAdpter));
//        }
//        else
//        {
//            ;
//        }
//        QueueLen = CM_QueueStatus(p_stpQueueToPctlAdpter);
//        DarkLogPrint(ENUM_LOG_PROMPT,&QueueLen,LOGTYPE_BEFINRSSP1LEN,4);
//        PlfmLogPrintStr(ENUM_LOG_PROMPT,"Intent RSSP_1 Before Handler len = %d\r\n",CM_QueueStatus(p_stpQueueToPctlAdpter));
//        (void)QueueClear(t_stpTmpQueue);
//    }
//    else
//    {
//        /* do nothing */
//    }
    
//    return t_u8Ret;
//}

///* 将解析完的外网RSSP数据按照数据长度、数据类型、端类型、2个字节预留放到平台给应用的数据流中*/
//UINT8 Recv_Outnet_After_Handler(UINT8* p_p8ArrFromPctlAdpter)
//{
//    /* 适配层解析完之后的数据结构*/
//    /*|&+2字节长度+1字节源类型+1字节源ID+2字节logid+1字节lifetime+应用数据&|+|&?n个?&|*/
//    UINT8 t_u8Ret  = 0 ;
//    UINT16 t_u16OffSet1 = 0 ;
//    UINT16 t_u16OffSet2 = 0 ;
//    UINT16 t_u16TmpLen1 = 0 ;
//    UINT16 t_u16TmpLen2 = 0 ;
//    UINT16 t_u16TmpDevTypeID = 0 ;

//    QueueStruct* t_stpMscpToAppQueue= NULL;

//    if(NULL != p_p8ArrFromPctlAdpter)
//    {
//        /* 获取平台跟应用接口的数据队列*/
//        /*|&+2字节长度+2个字节类型ID+2字节端类型+1字节预留+应用数据&|+|&?n个?&|*/
//        t_stpMscpToAppQueue = (QueueStruct*)GetMscpToAppBuff_Queue();

//        t_u16OffSet1 = 0 ;
//        while(1)
//        {
//            t_u16OffSet2 = 0;
//            t_u16TmpDevTypeID = 0;
//            t_u16OffSet2 = t_u16OffSet1;//截取此时偏移以后用
//            t_u16TmpLen1=ShortFromChar(&p_p8ArrFromPctlAdpter[t_u16OffSet1]);
//            t_u16OffSet1 += 2u;
//            if(0u != t_u16TmpLen1)
//            {
//               /* 将从外网接收的外网数据进行解析，由于外网出来的数据结构和平台给应用的结构一样，因此直接copy,|&+2字节长度+2个字节类型ID+2字节端类型+1字节预留+应用数据&|+|&?n个?&|格式写入到平台给应用的队列中*/
//                t_u16TmpDevTypeID = ShortFromChar(&p_p8ArrFromPctlAdpter[t_u16OffSet1]);
//                t_u16TmpLen2 = t_u16TmpLen1 +2u;/* 队列中的总数据长度需要包含该长度本身的长度*/
//                /* 将数据按照格式写入队列中*/
//                (void)QueueWrite(t_u16TmpLen2,&p_p8ArrFromPctlAdpter[t_u16OffSet2],t_stpMscpToAppQueue);


//                /*得到当前从数据队列中读取数据的偏移量，用于下次继续读取数据*/
//                t_u16OffSet1 += t_u16TmpLen1;
//            }
//            else
//            {
//                break;
//            }
//        }
//        DarkLogPrint(ENUM_LOG_PROMPT,&t_u16TmpLen2,LOGTYPE_AFTRSSP1LEN,2);
//        t_u8Ret = 1;
//    }
//    else
//    {
//        ;/*do nothing*/
//    }
//    return t_u8Ret;
//}


///* 将解析完的外网后备通道RSSP数据按照数据长度、数据类型、端类型、2个字节预留放到平台给应用的数据流中*/
//UINT8 Recv_OutnetBack_After_Handler(UINT8* p_p8ArrFromPctlAdpter)
//{
//    /* 适配层解析完之后的数据结构*/
//    /*|&+2字节长度+1字节源类型+1字节源ID+2字节logid+1字节lifetime+应用数据&|+|&?n个?&|*/
//    UINT8 t_u8Ret  = 0 ;
//    UINT16 t_u16OffSet1 = 0 ;
//    UINT16 t_u16OffSet2 = 0 ;
//    UINT16 t_u16TmpLen1 = 0 ;
//    UINT16 t_u16TmpLen2 = 0 ;
//    UINT16 t_u16TmpDevTypeID = 0 ;

//    QueueStruct* t_stpMscpToAppQueue= NULL;

//    if(NULL != p_p8ArrFromPctlAdpter)
//    {
//        /* 获取平台跟应用接口的数据队列*/
//        /*|&+2字节长度+2个字节类型ID+2字节端类型+1字节预留+应用数据&|+|&?n个?&|*/
//        t_stpMscpToAppQueue = (QueueStruct*)GetMscpToBackAppBuff_Queue();

//        t_u16OffSet1 = 0 ;
//        while(1)
//        {
//            t_u16OffSet2 = 0;
//            t_u16TmpDevTypeID = 0;
//            t_u16OffSet2 = t_u16OffSet1;//截取此时偏移以后用
//            t_u16TmpLen1=ShortFromChar(&p_p8ArrFromPctlAdpter[t_u16OffSet1]);
//            t_u16OffSet1 += 2u;
//            if(0u != t_u16TmpLen1)
//            {
//               /* 将从外网接收的外网数据进行解析，由于外网出来的数据结构和平台给应用的结构一样，因此直接copy,|&+2字节长度+2个字节类型ID+2字节端类型+1字节预留+应用数据&|+|&?n个?&|格式写入到平台给应用的队列中*/
//                t_u16TmpDevTypeID = ShortFromChar(&p_p8ArrFromPctlAdpter[t_u16OffSet1]);
//                t_u16TmpLen2 = t_u16TmpLen1 +2u;/* 队列中的总数据长度需要包含该长度本身的长度*/
//                /* 将数据按照格式写入队列中*/
//                (void)QueueWrite(t_u16TmpLen2,&p_p8ArrFromPctlAdpter[t_u16OffSet2],t_stpMscpToAppQueue);


//                /*得到当前从数据队列中读取数据的偏移量，用于下次继续读取数据*/
//                t_u16OffSet1 += t_u16TmpLen1;
//            }
//            else
//            {
//                break;
//            }
//        }
//        DarkLogPrint(ENUM_LOG_PROMPT,&t_u16TmpLen2,LOGTYPE_AFTRSSP1LEN,2);
//        t_u8Ret = 1;
//    }
//    else
//    {
//        ;/*do nothing*/
//    }
//    return t_u8Ret;
//}

//UINT8 Recv_Intnet_RSSP_After_Handler(UINT8* p_p8ArrFromPctlAdpter)
//{
//    /* 内网解完RSSP协议格式*/
//    /*|&+2字节长度+2字节源类型+2字节logid+应用数据&|+|&?n个?&|*/
//    UINT8 t_u8Ret  = 0 ;
//    UINT16 t_u16OffSet1 = 0 ;
//    UINT16 t_u16TmpLen1 = 0 ;
//    UINT16 t_u16TmpDevTypeID = 0 ;
//    UINT16 t_u16TmpLogId = 0 ;
//    UINT16 t_u16BTMDateLen = 0 ;
//    UINT8 t_arr8Buff_2Byte[2] ={ 0 };
//    UINT8 t_u8BTMFreeDate[1] = {0,};
//    QueueStruct* t_stpMscpToAppQueue= NULL;

//    if(NULL != p_p8ArrFromPctlAdpter)
//    {
//        /* 将内网解完RSSP协议的数据放到平台到App的队列中*/
//        /*|&+2字节长度+2个字节类型ID+2字节端类型+1字节预留+应用数据&|+|&?n个?&|*/
//        t_stpMscpToAppQueue = (QueueStruct*)GetMscpToAppBuff_Queue();
//        t_u16OffSet1 = 0 ;
//        while(1)
//        {
//            t_u16TmpDevTypeID = 0;
//            t_u16TmpLogId = 0 ;

//            t_u16TmpLen1=ShortFromChar(&p_p8ArrFromPctlAdpter[t_u16OffSet1]);
//            t_u16OffSet1 += 2u;
//            /*gh-test*/
//            PlfmLogPrintStr(ENUM_LOG_PROMPT, "--t_u16TmpLen1 = %d\r\n",t_u16TmpLen1);
//            if(0u != t_u16TmpLen1)
//            {
//                /* 两字节的typeid*/
//                t_u16TmpDevTypeID = ShortFromChar(&p_p8ArrFromPctlAdpter[t_u16OffSet1]);
//                t_u16OffSet1 += 2u;
//                /* 两字节的逻辑ID*/
//                t_u16TmpLogId = ShortFromChar(&p_p8ArrFromPctlAdpter[t_u16OffSet1]);
//                t_u16OffSet1 += 2u;

//                /* 将数据解析后按照平台和应用的数据流格式存放*/
//                t_u16BTMDateLen = t_u16TmpLen1 + 1u;/*平台和应用的接口比，内网RSSP解析完之后多了一个字节的预留数据*/
//                /* 将2个字节长度写入队列*/
//                ShortToChar(t_u16BTMDateLen,t_arr8Buff_2Byte);
//                (void)QueueWrite(2 ,t_arr8Buff_2Byte ,t_stpMscpToAppQueue);
//                /* 将2个字节的类型ID写入到队列*/
//                ShortToChar(t_u16TmpDevTypeID,t_arr8Buff_2Byte);
//                (void)QueueWrite(2 ,t_arr8Buff_2Byte ,t_stpMscpToAppQueue);
//                /* 将两个字节的逻辑ID写入队列*/
//                ShortToChar(t_u16TmpLogId,t_arr8Buff_2Byte);
//                (void)QueueWrite(2 ,t_arr8Buff_2Byte ,t_stpMscpToAppQueue);
//                /* 写入一个字节的预留字节*/
//                (void)QueueWrite(1 ,&t_u8BTMFreeDate[0] ,t_stpMscpToAppQueue);
//                /* 将应用数据写入队列(数据长度-2字节的typeid和2字节的逻辑id)*/
//                (void)QueueWrite((UINT32)(t_u16TmpLen1 - 2u - 2u) ,&p_p8ArrFromPctlAdpter[t_u16OffSet1] ,t_stpMscpToAppQueue);
//                PlfmLogPrintStr(ENUM_LOG_PROMPT,"Intnet RSSP_1 after len = %d typeid %x\n",t_u16TmpLen1,t_u16TmpDevTypeID);
//                t_u16OffSet1 += (t_u16TmpLen1 -(2u + 2u)); /* 将偏移量增加数据流的长度*/
//            }
//            else
//            {
//                break;
//            }
//        }
//        DarkLogPrint(ENUM_LOG_PROMPT,&t_u16BTMDateLen,LOGTYPE_AFTINRSSP1LEN,2);
//        t_u8Ret = 1;
//    }
//    else
//    {
//        ;/* do nothing */
//    }
//    return t_u8Ret;
//}

///***************************************************************************************
//* 功能描述:   对接收数据进行解FFFE协议后进行去重处理，比如只保留BTM输入数据的1端和2端的一份数据，
//* 输入参数:         无
//* 输入输出参数:
//* 输出参数:         无
//* 全局变量:
//* 返回值:
//****************************************************************************************/
//void RecvData_pre_Treated_Handler(void)
//{
//    /* 1.将接收的透传数据根据类型不同进行解FFFE处理*/
//    /* 2.从解析后的FFFE队列中将单端的DMI和BTM数据分别取1包数据放到预处理后的队列，将BTM数据放到透传RSSP-1队列，将DMI数据放到透传SFP队列，供下一步2取和解协议处理 */
//    /* 为了避免BTM数据存在丢包现象，将去重功能删除 lsn 2010年03月30日*/
//    /*BTM 解FFFE前和解FFFE后的结构体*/
//    ST_TRANSPARENT_DATE t_stBTMDate_BeforeFFFE = {0,};
//    ST_TRANSPARENT_DATE t_stBTMDate_AfterFFFE = {0,};
//    /*DMI 解FFFE前和解FFFE后的结构体*/
//    ST_TRANSPARENT_DATE t_stDMIDate_BeforeFFFE = {0,};
//    ST_TRANSPARENT_DATE t_stDMIDate_AfterFFFE = {0,};

//    /*BTM 解FFFE并且去完重之后的队列后的结构体*/
//   // ST_TRANSPARENT_DATE t_stBTMDate_AfterPreTreat = {0,};
//    /*DMI 解FFFE并且去完重之后的队列后的结构体*/
//   // ST_TRANSPARENT_DATE t_stDMIDate_AfterPreTreat = {0,};
//    UINT8 t_u8TmpRet1= 0 ; /* 从透传队列中挑选不同类型型数据的返回值*/
//    UINT8 t_u8I = 0;
///*如果BTM需要解RSSP需要用到的变量*/
//    UINT16 t_u16BTMAorB = 0 ;
//    UINT16 t_WhichEnd = 0;  //用于透传类型的数据，填入供应用识别端位
//    UINT8 t_arr8Buff[1000] = {0 };
//    UINT16 t_u16TmpLen1 = 0;
//    UINT8 t_arr8Buff_2Byte[2] = {0}; /* 定义两个字节长度，用于存储临时变量*/
//    UINT8 RrotocolTypeRtn = APP_USE_ERR_PROTOCOL;

//#if PLFM_TRANS_DATA_TEST == 1
//	 //|2字节长度|2字节BoardId|（2字节长度|2字节类型|1字节预留|n字节数据）（*n*）
//	 UINT8 Emulate_Trans_Data1[500] = { 0x00,0x37,0x00,0x40,/*信号板TCIA CPU1Data*/
//		 /*19 btm*/    0x00,0x11,0x00,0xa3,0x00,0xff,0xfe,0x03,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
//		 /*19 btm*/      0x00,0x11,0x00,0xa4,0x00,0xff,0xfe,0x03,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0xff,0xfd,
//	 };
//	 //|2字节长度|2字节BoardId|（2字节长度|2字节类型|1字节预留|n字节数据）（*n*）
//	 UINT8 Emulate_Trans_Data2[500] = { 0x00,0x37,0x00,0x40,/*信号板TCIA CPU1Data*/
//	         /*19 btm*/    0x00,0x11,0x00,0xa3,0x00,0x0d,0x0e,0x03,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0xff,0xfd,
//	         /*19 btm*/      0x00,0x11,0x00,0xa4,0x00,0xff,0xfe,0x03,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0xff,0xfd,
//	     };
//#endif
//    UINT16 DMIDataLen = 0;
//    QueueStruct* RecvTransParenetQueue= NULL; /* 接收原始透传队列指针*/
//    QueueStruct* TransPreTreatRsspQueue= NULL; /* 经过去重后透传过RSSP-1的队列*/
//    QueueStruct* TransPreTreatSfpQueue= NULL; /* 经过去重后透传过SFP的队列*/

//    RecvTransParenetQueue = (QueueStruct*)(GetPwlInnetInQueue_TransParenet());
//    TransPreTreatRsspQueue = (QueueStruct*)(GetPwlPreTransRsspQueue());
//    TransPreTreatSfpQueue = (QueueStruct*)(GetPwlPreTransSfpQueue());

//#if PLFM_TRANS_DATA_TEST == 1
//    if(0 == (Plat_HardSync_GetCurrWorkCycle()%2))
//    {
//     QueueWrite((0x37 + 2) * 1, Emulate_Trans_Data1, RecvTransParenetQueue);
//    }
//    else
//    {
//     QueueWrite((0x37 + 2) * 1, Emulate_Trans_Data2, RecvTransParenetQueue);
//    }
//#endif

//    CM_Memset(BtmFFFEFollowDada,0,(UINT32)(1024*2));
//    CM_Memset(DMIFFFEFollowDada,0,1024);
//    /* 从透传数据队列将数据读出，根据数据类型不同，将数据放到不同结构体中去*/
//    t_u8TmpRet1 = GetTypeDateFromRcvQueue(RecvTransParenetQueue,&t_stBTMDate_BeforeFFFE);
//    if(1u == t_u8TmpRet1)
//    {
//        /* 将提取出的BTM原始数据解FFFE协议*/
//        DarkLogPrint(ENUM_LOG_PROMPT,&t_stBTMDate_BeforeFFFE.m_u8TranFrameCount,LOGTYPE_BEFORBTMCNT,1);
//        DarkLogPrint(ENUM_LOG_PROMPT,&t_stDMIDate_BeforeFFFE.m_u8TranFrameCount,LOGTYPE_BEFORDMICNT,1);
//        PlfmLogPrintStr(ENUM_LOG_DEBUG,"Recv BTM before FFFE count %d,DMI FFFE Count %d\n", t_stBTMDate_BeforeFFFE.m_u8TranFrameCount,t_stDMIDate_BeforeFFFE.m_u8TranFrameCount);
//        /** 数据解FFFE协议*/
//        (void)DATE_DECODE_FFFE(&t_stBTMDate_BeforeFFFE,&t_stBTMDate_AfterFFFE);
//        /*将需要主备同步的数据放到相应数据流中*/
//        MainPrepareFfeFollowBuff(&t_stBTMDate_BeforeFFFE,UART_BTM_1,BtmFFFEFollowDada,(UINT16)(1024u*2u));//前两个入参没有-待定删除
//        /* 将过完FFFE协议的数据进行BTM数据去冗余*/

//        PlfmLogPrintStr(ENUM_LOG_DEBUG,"Recv BTM After FFFE count %d\n",t_stBTMDate_AfterFFFE.m_u8TranFrameCount);
//        DarkLogPrint(ENUM_LOG_PROMPT,&t_stBTMDate_AfterFFFE.m_u8TranFrameCount,LOGTYPE_AFTERBTMCNT,1);
//       /* 将BTM去冗余后的数据放到透传RSSP-1的队列中*/
//        /*根据是否需要解RSSP协议队列，将数据以不同的格式放到预处理后的透传RSSP-1队列中*/
//        for(t_u8I = 0 ; t_u8I < t_stBTMDate_AfterFFFE.m_u8TranFrameCount ; t_u8I++)
//        {
//            /* 根据BTM是否需要解RSSP-1，如果需要解RSSP协议，将数据从透传队列中取出，将数据放到解协议前队列*/
//            CM_Memset(t_arr8Buff,0,1000);

//            PlfmLogPrintStr(ENUM_LOG_DEBUG,"DateType[%x]  m_u16BoardId[%x] m_u16SysId[%x] \n",t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateType,
//                                                                                                t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16BoardId,
//                                                                                                t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16SysId);
//            /* 根据板卡ID和类型寻找到相应的RSSP-1中用到的类型 */
//            t_u8TmpRet1 =  (UINT8)FindPctlDateTypeBoardIdAndType(
//                    t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateType,
//                    t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16BoardId,
//                    t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16SysId,
//                    &t_u16BTMAorB,&t_WhichEnd);

//            PlfmLogPrintStr(ENUM_LOG_DEBUG,"Device[%x] t_u8TmpRet = %d \n",t_u16BTMAorB,t_u8TmpRet1);

//            if(1u == t_u8TmpRet1)
//            {
//                RrotocolTypeRtn = GetDataProtocolType(t_u16BTMAorB);
//                PlfmLogPrintStr(ENUM_LOG_DEBUG,"RrotocolTypeRtn =  %x \n",RrotocolTypeRtn);
//                t_u16TmpLen1 = 0 ;
//                DarkLogPrint(ENUM_LOG_PROMPT,&gBtmInfType,LOGTYPE_BTMINFOTYPE,1);
//                DarkLogPrint(ENUM_LOG_PROMPT,&t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateSize,LOGTYPE_BTMINFOSIZE,2);
//                /** 判断是否是走RSSP数据*/
//                if(APP_USE_RSSP1 == RrotocolTypeRtn)
//                {
//                    if (BTM_OLD_INF == gBtmInfType)
//                    {
//                        (void)NoProtocolDataToApp(t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16SysId,t_u16BTMAorB, t_WhichEnd, t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_arr8Date, t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateSize);
//                        PlfmLogPrintStr(ENUM_LOG_DEBUG, "--BTMA RcvAfterFFFE = %d\r\n", t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateSize);
//                    }
//                    else if (BTM_NEW_INF == gBtmInfType)
//                    {
//                        /* 准备需要往解Rssp-1协议队列中存放的数据*/
//                        (void)IntnetBeforeRsspDataPrepare(0,
//                                               t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_arr8Date,
//                                               t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateSize,
//                                               t_arr8Buff,&t_u16TmpLen1,1000);
//                        /*t_arr8BuffTmp2写入p_stpQueueToPctlAdpter*/
//                        (void)QueueWrite(t_u16TmpLen1,t_arr8Buff, TransPreTreatRsspQueue );
//                    }
//                    else
//                    {
//                        ; /**  nothing to do*/
//                    }
//                }
//                /** 判断是否是走SFP数据*/
//                else if(APP_USE_SFP == RrotocolTypeRtn)
//                {
//                      t_u16TmpLen1 = t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateSize;
//                      DarkLogPrint(ENUM_LOG_PROMPT,&DMIDataLen,LOGTYPE_DMIINFOSIZE,2);
//                      ShortToChar((t_u16TmpLen1),&t_arr8Buff_2Byte[0]);
//                      /*将数据长度写入队列 */
//                      (void)QueueWrite(2, &t_arr8Buff_2Byte[0],TransPreTreatSfpQueue);
//                      /* 将接收到的数据存放到解协议前数据流中*/
//                      (void)QueueWrite(t_u16TmpLen1,&t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_arr8Date[0],TransPreTreatSfpQueue);
//                }
//                /** 判断是否是走无协议数据*/
//                else if(APP_USE_NOT_PROTOCOL == RrotocolTypeRtn)
//                {

//                    (void)NoProtocolDataToApp(t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16SysId,t_u16BTMAorB, t_WhichEnd, t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_arr8Date, t_stBTMDate_AfterFFFE.m_arrStTranDateUnion[t_u8I].m_u16DateSize);
//                }
//                else
//                {
//                    PlfmLogPrintStr(ENUM_LOG_PROMPT,"NO_FindPROTOCOLType !\n");
//                }
//            }
//            else
//            {
//                PlfmLogPrintStr(ENUM_LOG_PROMPT,"NO_FindAorB !\n");
//            }
//        }//endof for(t_u8I = 0 ; t_u8I < t_stBTMDate.m_u8BTMFrameCount ; t_u8I++)
//    }
//    else
//    {
//         /*无相关类型数据*/
//    }

//    /* 将去重后的数据放回透传队列中,先将透传队列清掉后，再向里面放数据*/
//    (void)QueueClear(RecvTransParenetQueue);
//}

///*gh-1031:获取板卡温度，并放到队列里，然后传给应用*/
//UINT8 Get_Board_Temprature_To_App(QueueStruct* pAppGetTempQueue)
//{
//    UINT8 rtnValue = 0;
//    CM_INT32 temperature = 0x7FFFFFFF;
//    UINT8 tempBuff[32] = {0};
//    UINT16 tempDataLen = 0;
//    UINT8 temp_2Byte[2] = {0};
//    UINT8 SYSID = 0xFFu;
//    /*入参防空*/
//    if(NULL != pAppGetTempQueue)
//    {
//        PlfmLogPrintStr(ENUM_LOG_DEBUG, "--Before Get Temperature To App\r\n");


//        /*获取SYSID*/
//        SYSID = PLFM_PlatformGetLocalName();
//        /*获取CPUID*/

//        /*获取板卡温度*/
//        temperature = MSCP_GetTemperature();
//        /*往buff里写入SYSID*/
//        tempBuff[0] = SYSID;
//        tempDataLen = tempDataLen + 1u;
//        /*往buff里写入温度*/
//        tempBuff[tempDataLen    ] = ((UINT8)((UINT32)temperature>>24) & 0xffu);
//        tempBuff[tempDataLen + 1u] = ((UINT8)((UINT32)temperature>>16) & 0xffu);
//        tempBuff[tempDataLen + 2u] = ((UINT8)((UINT32)temperature>>8) & 0xffu);
//        tempBuff[tempDataLen + 3u] = ((UINT8)((UINT32)temperature& 0xffu));
//        tempDataLen = tempDataLen + 4u;
//        PlfmLogPrintArray(ENUM_LOG_PROMPT, tempBuff, tempDataLen);
//        /*往队列里写入2字节长度*/
//        CM_ShortToChar(tempDataLen, temp_2Byte);
//        (void)QueueWrite(2,temp_2Byte,pAppGetTempQueue);
//        /*往队列里写入buff的数据： 1字节SYSID + 4字节温度值*/
//        (void)QueueWrite(tempDataLen,tempBuff,pAppGetTempQueue);
//        rtnValue = 1;
//        PlfmLogPrintStr(ENUM_LOG_DEBUG, "--APP Get Temeprature Success\r\n");
//    }
//    else
//    {
//        rtnValue = 0;
//    }

//    return rtnValue;
//}

//#endif


