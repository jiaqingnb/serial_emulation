/************************************************************************
* 文件名	：  sfpInnetDataVar.c
* 版权说明	：   
* 版本号	：  v1.0  
* 创建时间	：  2009.11.12
* 作者		：	刘宏杰、李雁
* 功能描述	：  VOBC内网各子系统间通信使用SFP协议时的数据库文件
* 使用注意	：  本文件属于VOBC公共文件,暂由刘宏杰维护(感谢李雁帮忙提供初稿)
				在C文件中定义变量,头文件中提供extern供其他人调用
* 修改记录	：	无
************************************************************************/
#include "CommonTypes.h"
#include "PROJECT_OR_BOARD_PTCL.h"
#include "sfpInnetDataVar.h"
/*ATP、ATO、MMI间通信时,SFP协议使用的数据库数组:
*两端ATP分别当作ZC1和ZC2,ZC1为发起方;
*ATO为CI3,MMI为CI4,CCOV为CI5(CCOV仅为预留,暂无通信的需求).ZC与CI通信时ZC为发起方
*总结各子系统ID为:两端ATP分别为0x5501,0x5502;ATO为0x7103;MMI为0x7104;CCOV为0x7105*/
#if defined(MiNIATURIZATION_ALL)
UINT8 vSfpInnetDataBuf[] =
{
	   
 0,7,/*不同设备通信关系表记录个数*/
 /*记录的格式如下:*/
 /*                   发起方  发起方发送数据  发起方报文   发起方报文  发起方接收Ack          跟随方  跟随方发送数据  跟随方运行  跟随方报文  此通信关系中成功发送           */
 /*                   设备类型    通信类型    发送周期长度   最大延时   报文最大延时差       设备类型    通信类型      周期长度    最大延时   一包数据所需最小周期数                               */
 /*VOBC->DMI*/           0x55,       0x55,       0x00,0xC8,   0x01,0x90,   0x01,0x90,            0x71,     0x55,           0x00,0xC8,  0x01,0x90,  0x02,
 /*VOBC->SIG*/           0x55,       0x55,       0x00,0xC8,   0x00,0xC8,   0x00,0xC8,            0x40,     0x55,           0x00,0xC8,  0x01,0x90,  0x02,
 /*VOBC->IPB*/           0x55,       0x55,       0x00,0xC8,   0x00,0xC8,   0x00,0xC8,            0x90,     0x55,           0x00,0x64,  0x00,0xC8,  0x02,
 /*VOBC->OPB*/           0x55,       0x55,       0x00,0xC8,   0x00,0xC8,   0x00,0xC8,            0xB0,     0x55,           0x00,0x64,  0x00,0xC8,  0x02,
 /*VOBC->ATO*/           0x55,       0x55,       0x00,0xC8,   0x00,0xC8,   0x00,0xC8,            0xF0,     0x55,           0x00,0xC8,  0x01,0x90,  0x02,
 /*ATO-IPB*/             0xf0,       0x55,       0x00,0xC8,   0x00,0xC8,   0x00,0xC8,            0x90,     0x55,           0x00,0x64,  0x00,0x64,  0x02,
 /*ATO-OPB*/             0xf0,       0x55,       0x00,0xC8,   0x00,0xC8,   0x00,0xC8,            0xB0,     0x55,           0x00,0x64,  0x00,0x64,  0x02,
 0,3,/*相同设备通信关系表记录个数*/
 /*记录的格式如下:*/
 /*                   发起方  发起方      发起方发送数据  发起方报文  发起方报文 发起方接收Ack    跟随方  跟随方      跟随方发送数据  跟随方运行  跟随方报文  此通信关系中成功发送           */
 /*                   设备类型 设备ID         通信类型    发送周期长度  最大延时  报文最大延时差  设备类型 设备ID         通信类型      周期长度    最大延时   一包数据所需最小周期数     */
 /*AOM1->AOM2*/       0x56,   1,               0x55,      0x00,0xC8,  0x00,0xC8,   0x00,0xC8,       0x56,   2,             0x55,      0x00,0xC8,  0x00,0xC8,  0x02,
 /*ZC1->ZC2*/         0x55,   1,               0x55,      0x00,0xC8,  0x00,0xC8,   0x00,0xC8,       0x55,   2,             0x55,      0x00,0xC8,  0x00,0xC8,  0x02,
 /*VOBC1-VOBC2*/      0X55,   1,               0x55,      0x00,0xC8,  0x00,0xC8,   0x00,0xC8,       0x55,   2,             0x55,      0x00,0xC8,  0x00,0xC8,  0x02

};
#elif defined(OC_ALL)

const UINT8 vSfpInnetDataBuf[] = 
{
	   
 0,8,/*不同设备通信关系表记录个数*/
 /*记录的格式如下:*/
 /*                           发起方                                                        发起方发送数据                                 发起方报文                                         发起方报文                                          发起方接收Ack                  跟随方                                                                        跟随方发送数据                                            跟随方运行                                          跟随方报文                                   此通信关系中成功发送           */
 /*                          设备类型                                                      通信类型                                              发送周期长度                                      最大延时                                               报文最大延时差                                                                设备类型                                                                        通信类型                                                     周期长度                                              最大延时                                        一包数据所需最小周期数    */
 /*MASTER->IPB*/             0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_IPB,             0x55,              0x00,0x64,        0x01,0x2C,        0x02,
 /*MASTER->OPB*/             0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_OPB,             0x55,              0x00,0x64,        0x01,0x2C,        0x02,
 /*MASTER->TDPC*/            0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_OC_SIG,          0x55,              0x00,0x96,        0x01,0x2C,        0x02,
 /*MASTER->SIG*/             0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_PDTC,            0x55,              0x00,0x64,        0x01,0x2C,        0x02,
 /*MASTER->SWITCH_4*/        0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_OCSWITCH_4,      0x55,              0x00,0x96,        0x01,0x2C,        0x02,
 /*MASTER->SWITCH_5*/        0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_OCSWITCH_5,      0x55,              0x00,0x96,        0x01,0x2C,        0x02,
 /*MASTER->Ipb1    */        0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_SCATTERIPB,      0x55,              0x00,0x64,        0x01,0x2C,        0x02,
 /*MASTER->opb1    */        0x55,              0x55,             0x01,0x2C,         0x01,0x2C,         0x01,0x2C,          MSCP_BOARD_MASK_SCATTEROPB,      0x55,              0x00,0x64,        0x01,0x2C,        0x02,
 0,2,/*相同设备通信关系表记录个数*/
 /*记录的格式如下:*/
 /*                   发起方  发起方              发起方发送数据        发起方报文              发起方报文 发起方接收Ack    跟随方  跟随方                       跟随方发送数据  跟随方运行  跟随方报文  此通信关系中成功发送           */
 /*                   设备类型 设备ID   通信类型                   发送周期长度           最大延时  报文最大延时差              设备类型 设备ID       通信类型      周期长度    最大延时   一包数据所需最小周期数     */
 /*AOM1->AOM2*/       0x56,   1,    0x55,      0x00,0xC8,  0x00,0xC8,          0x00,0xC8,       0x56,   2,             0x55,      0x00,0xC8,  0x00,0xC8,  0x02,
 /*ZC1->ZC2*/         0x55,   1,    0x55,      0x00,0xC8,  0x00,0xC8,          0x00,0xC8,       0x55,   2,             0x55,      0x00,0xC8,  0x00,0xC8,  0x02

};
#else
const UINT8 vSfpInnetDataBuf[] = {0};

#endif

