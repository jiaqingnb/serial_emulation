/****************************************************************************************************
* 文件名   :  IOModuleExtern.h
* 版权说明 :  交控科技股份有限公司
* 版本号   :  1.0
* 创建时间 :  2018.08.16
* 作者     :
* 功能描述 :  IO模块接口（含输入、输出、IO索引及结构）
* 使用注意 :
*
* 修改记录 :
*
**************************************************************************************************/
#ifndef IO_MODULE_EXTERN_H_
#define IO_MODULE_EXTERN_H_

#include "CommDefine.h"
#include "dquVobcConfigData.h"
#include "CommonTypes.h"
#include "App_ComStru.h"
#include "IOConfig.h"


/* 宏定义 */
#define IO_NULL8_MSCP					((UINT8)0xFFU)
#define IO_TRUE_MSCP					((UINT8)0x55U)
#define IO_FALSE_MSCP					((UINT8)0xAAU)

#define INPUT_FLAG_NUM								(UINT8)82U		/*分端Table中安全和非安全输入的总共数目*/
#define INPUT_FLAG_NUM_SINGLE                       (UINT8)126U     /*单节点采集Table中的总共数量*/
#define INPUT_DEFINE_MAX							(UINT16)0xAA	/*整个宏定义IO索引的最大值*/

/*安全输入开关量含义--与配置数据匹配*/
#define IN_TRAIN_CAB_ACTIVE_MSCP_NC					(UINT8)0x01		/*驾驶室激活常闭*/
#define IN_TRAIN_CAB_ACTIVE_MSCP_NO					(UINT8)0x02		/*驾驶室激活常开*/
#define IN_TRAIN_EB_ENABLE_MSCP_NC					(UINT8)0x03		/*列车车辆已实施紧急制动常闭*/
#define IN_TRAIN_EB_ENABLE_MSCP_NO					(UINT8)0x04		/*列车车辆已实施紧急制动常开*/
#define IN_TRAIN_TRACTION_CUT_MSCP_NC				(UINT8)0x05		/*牵引已卸载常闭*/
#define IN_TRAIN_TRACTION_CUT_MSCP_NO				(UINT8)0x06		/*牵引已卸载常开*/
#define IN_TRAIN_DIR_FORWARD_MCSP_NC				(UINT8)0x07		/*方向手柄向前常闭*/
#define IN_TRAIN_DIR_FORWARD_MCSP_NO				(UINT8)0x08		/*方向手柄向前常开*/
#define IN_TRAIN_DIR_BACKWARD_MSCP_NC				(UINT8)0x09		/*方向手柄向后常闭*/
#define IN_TRAIN_DIR_BACKWARD_MSCP_NO				(UINT8)0x0A		/*方向手柄向后常开*/
#define IN_TRAIN_HANDLE_TRACTION_MSCP_NC			(UINT8)0x0B		/*牵引手柄在牵引位常闭*/
#define IN_TRAIN_HANDLE_TRACTION_MSCP_NO			(UINT8)0x0C		/*牵引手柄在牵引位常开*/
#define IN_TRAIN_ATP_CONTROL_MSCP_NC				(UINT8)0x0D		/*自动切换电路导向ATP控车常闭*/
#define IN_TRAIN_ATP_CONTROL_MSCP_NO				(UINT8)0x0E		/*自动切换电路导向ATP控车常开*/
#define IN_BCU_IN_ATP_MSCP_NC						(UINT8)0x0F		/*BCU运行在ATP模式常闭*/
#define IN_BCU_IN_ATP_MSCP_NO						(UINT8)0x10		/*BCU运行在ATP模式常开*/
#define IN_TRAIN_AIR_FILL_MSCP_NC					(UINT8)0x11		/*列车处于充风状态常闭*/
#define IN_TRAIN_AIR_FILL_MSCP_NO					(UINT8)0x12		/*列车处于充风状态常开*/
#define IN_LKJ_BRAKE_STATE_MSCP_NC					(UINT8)0x13		/*LKJ处于制动实施状态常闭*/
#define IN_LKJ_BRAKE_STATE_MSCP_NO					(UINT8)0x14		/*LKJ处于制动实施状态常开*/
#define IN_BCU_IN_LKJ_MSCP_NC						(UINT8)0x15		/*BCU运行在LKJ模式常闭*/
#define IN_BCU_IN_LKJ_MSCP_NO						(UINT8)0x16		/*BCU运行在LKJ模式常开*/
#define IN_TRAIN_OPEN_FSB_VALVE_MSCP_NC				(UINT8)0x17		/*VOBC已开启常用制动阀常闭*/
#define IN_TRAIN_OPEN_FSB_VALVE_MSCP_NO				(UINT8)0x18		/*VOBC已开启常用制动阀常开*/
#define IN_TRAIN_ALLOW_ENTRY_ATO_MSCP_NC			(UINT8)0x19		/*列车允许进入ATO常闭*/
#define IN_TRAIN_ALLOW_ENTRY_ATO_MSCP_NO			(UINT8)0x1A		/*列车允许进入ATO常开*/
#define IN_TRAIN_LKJ_CONTRO_MSCP_NC                 (UINT8)0x1B     /*自动切换电路导向LKJ控车常闭*/
#define IN_TRAIN_LKJ_CONTRO_MSCP_NO                 (UINT8)0x1C     /*自动切换电路导向LKJ控车常开*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL1_MSCP_NC		(UINT8)0x1D		/*常用制动1（电制动）常闭*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL1_MSCP_NO		(UINT8)0x1E		/*常用制动1（电制动）常开*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL2_MSCP_NC		(UINT8)0x1F		/*常用制动2（电制动）常闭*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL2_MSCP_NO		(UINT8)0x20		/*常用制动2（电制动）常开*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL3_MSCP_NC		(UINT8)0x21		/*常用制动3（电制动）常闭*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL3_MSCP_NO		(UINT8)0x22		/*常用制动3（电制动）常开*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL4_MSCP_NC		(UINT8)0x23		/*常用制动4（电制动）常闭*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL4_MSCP_NO		(UINT8)0x24		/*常用制动4（电制动）常开*/
#define IN_TRAIN_CONFIRM_BUTTON_MSCP_NC				(UINT8)0x25     /*列车确认按钮状态 (常开信号)常闭*/
#define IN_TRAIN_CONFIRM_BUTTON_MSCP_NO             (UINT8)0x26     /*列车确认按钮状态 (常开信号)常开*/
#define IN_ATP_TRAIN_INTEG_MSCP_NC					(UINT8)0x27		/*列车完整，常闭*/
#define IN_ATP_TRAIN_INTEG_MSCP_NO					(UINT8)0x28		/*列车完整，常开*/
#define IN_ATP_BCZD_VALID_MSCP_NC					(UINT8)0x29		/*已实施保持制动，常闭*/
#define IN_ATP_BCZD_VALID_MSCP_NO					(UINT8)0x2A		/*已实施保持制动，常开*/
#define IN_ATP_SB_RIGHT_MSCP_NC						(UINT8)0x2B		/*牵引制动手柄在零位且方向手柄在向前位，常闭*/
#define IN_ATP_SB_RIGHT_MSCP_NO						(UINT8)0x2C		/*牵引制动手柄在零位且方向手柄在向前位，常开*/
#define IN_TRAIN_DOORCLOSE_MSCP_NC                  (UINT8)0x2D     /*车门关闭(ATP安全输入)常闭*/
#define IN_TRAIN_DOORCLOSE_MSCP_NO                  (UINT8)0x2E     /*车门关闭(ATP安全输入)常开*/
#define IN_TRAIN_EMERDOORBOARD_MSCP_NC              (UINT8)0x2F     /*逃生门盖板状态监督（逃生门紧急解锁手柄状态,ATP安全输入）常闭*/
#define IN_TRAIN_EMERDOORBOARD_MSCP_NO              (UINT8)0x30     /*逃生门盖板状态监督（逃生门紧急解锁手柄状态,ATP安全输入）常开*/
#define IN_TRAIN_KEY_OPEN_MSCP_NC					(UINT8)0x31		/*司机钥匙打开常闭*/
#define IN_TRAIN_KEY_OPEN_MSCP_NO					(UINT8)0x32		/*司机钥匙打开常开*/
#define IN_ATP_DOOR_CLOSEDANDLOCK_MSCP_NC			(UINT8)0x33		/*车门关闭且锁闭,常闭*/
#define IN_ATP_DOOR_CLOSEDANDLOCK_MSCP_NO           (UINT8)0x34		/*车门关闭且锁闭,常开*/
#define IN_TRAIN_RESECT_SWITCH_MSCP_NC				(UINT8)0x35     /*切除开关在切除位常闭 */    
#define IN_TRAIN_RESECT_SWITCH_MSCP_NO				(UINT8)0x36     /*切除开关在切除位 常开*/  
#define IN_TRAIN_EBHANDLE_ACTIVE_MSCP_NC			(UINT8)0x37		/*车辆紧急手柄激活常闭*/
#define IN_TRAIN_EBHANDLE_ACTIVE_MSCP_NO			(UINT8)0x38		/*车辆紧急手柄激活常开*/
#define IN_TRAIN_OBSTDET_INPUT_MSCP_NC				(UINT8)0x39		/*障碍物脱轨检测常闭*/
#define IN_TRAIN_OBSTDET_INPUT_MSCP_NO				(UINT8)0x3A		/*障碍物脱轨检测常开*/
#define IN_DOUBLE1_DOORCLOSELOCK_MSCP_NC			(UINT8)0x3B		/*迪士尼车门关闭且锁闭（双1有效），常闭*/
#define IN_DOUBLE1_DOORCLOSELOCK_MSCP_NO			(UINT8)0x3C		/*迪士尼车门关闭且锁闭（双1有效），常开*/
#define IN_RESERVE_0x3D_SINGLE						(UINT8)0x3D		/*预留*/
#define IN_RESERVE_0x3E_SINGLE						(UINT8)0x3E		/*预留*/
#define IN_RESERVE_0x3F_SINGLE						(UINT8)0x3F		/*预留*/
#define IN_RESERVE_0x40_SINGLE						(UINT8)0x40		/*预留*/
#define IN_TRAIN_BRAKE_FAULT_MSCP_NC				(UINT8)0x41		/*制动重故障常闭*/
#define IN_TRAIN_BRAKE_FAULT_MSCP_NO				(UINT8)0x42		/*制动重故障常开*/
#define IN_ATP_TRACTION_RIGHT_MSCP_NC				(UINT8)0x43		/*牵引制动手柄在牵引位且方向手柄向前常闭*/
#define IN_ATP_TRACTION_RIGHT_MSCP_NO				(UINT8)0x44		/*牵引制动手柄在牵引位且方向手柄向前常开*/
#define IN_EMERGENCYDOOR_SINGLE_MSCP_NC  			(UINT8)0x45 	/*逃生门状态常闭*/
#define IN_EMERGENCYDOOR_SINGLE_MSCP_NO  			(UINT8)0x46 	/*逃生门状态常开*/

/*Debug todo 朔黄安全*/

#define IN_TRAIN_CONFIRM_BUTTON2_SINGLE             (UINT8)0x47     /* 确认按钮2常闭信号*/
/*非安全输入开关量含义--与配置数据匹配*/
#define IN_ATP_ATO_START1_SINGLE  					(UINT8)0x48	    /*ATO启动按钮1已按下*/
#define IN_ATP_MODE_UP_SINGLE  						(UINT8)0x49		/*模式选择上按钮按下*/
#define IN_ATP_MODE_DOWN_SINGLE  					(UINT8)0x4A		/*模式选择下按钮按下*/
#define IN_ATP_AR_SINGLE  							(UINT8)0x4B		/*AR按钮按下*/
#define IN_ATP_RIGHT_DOOR_OPEN_SINGLE  				(UINT8)0x4C		/*右门开门按钮按下*/
#define IN_ATP_RIGHT_DOOR_CLOSE_SINGLE  			(UINT8)0x4D		/*右门关门按钮按下*/
#define IN_ATP_EUM_SWITCH_SINGLE  					(UINT8)0x4E		/*EUM开关激活*/
#define IN_ATP_LEFT_DOOR_OPEN_SINGLE  				(UINT8)0x4F		/*左门开门按钮按下*/
#define IN_ATP_LEFT_DOOR_CLOSE_SINGLE  				(UINT8)0x50		/*左门关门按钮按下*/
#define IN_ATP_ATO_START2_SINGLE  					(UINT8)0x51 	/*ATO启动按钮2已按下*/

#define IN_ATP_SLEEP_SINGLE  						(UINT8)0x52		/*休眠*/
#define IN_TRAIN_NOBODY_ALERT_SIGNAL_SINGLE  		(UINT8)0x53		/*无人警惕解除相关信号*/
#define IN_TRAIN_WHISTLE_SIGNAL_SINGLE  			(UINT8)0x54		/*鸣笛信号*/
#define IN_ATP_TVS1_Check_SINGLE         			(UINT8)0x55		/*EMC板1的击穿检测（非安全）*/
#define IN_ATP_TVS2_Check_SINGLE         			(UINT8)0x56		/*EMC板2的击穿检测（非安全）*/
#define IN_CHANGE_SWITCH_LKJ_SINGLE                 (UINT8)0x57		/*手动切换开关到LKJ位*/
#define IN_CHANGE_SWITCH_AUTO_SINGLE                (UINT8)0x58		/*手动切换开关到自动位*/
#define IN_TRAIN_FSB_1_SINGLE                       (UINT8)0x59		/*列车已实施常用制动1*/
#define IN_TRAIN_FSB_2_SINGLE                       (UINT8)0x5A		/*列车已实施常用制动2,用常用制动1和常用制动2组合成80kp.100Kp,120Kpa*/
#define IN_TRAIN_FSB_120KPA_SINGLE                  (UINT8)0x5B		/*列车已实施常用制动120KPa */
#define IN_LKJ_EB_ENABLE_SINGLE                     (UINT8)0x5C		/*LKJ处于制动实施状态*/
#define IN_TRAIN_CUT_SWITCH_SINGLE                  (UINT8)0x5D		/*切除开关在切除位【预留，车车和510使用0x4E】*/
#define IN_TRAIN_RELIEVE_SINGLE                     (UINT8)0x5E		/*VOBC允许列车缓解*/
#define IN_ATP_HANDLE_EB_SINGLE                     (UINT8)0x5F		/*牵引制动手柄在电制位*/
#define IN_RESERVE_0x60_SINGLE						(UINT8)0x60		/*预留*/
#define IN_RESERVE_0x61_SINGLE						(UINT8)0x61		/*预留*/
#define IN_RESERVE_0x62_SINGLE						(UINT8)0x62		/*预留*/
#define IN_RESERVE_0x63_SINGLE						(UINT8)0x63		/*预留*/
#define IN_RESERVE_0x64_SINGLE						(UINT8)0x64		/*预留*/
#define IN_RESERVE_0x65_SINGLE						(UINT8)0x65		/*预留*/
#define IN_RESERVE_0x66_SINGLE						(UINT8)0x66		/*预留*/
#define IN_RESERVE_0x67_SINGLE						(UINT8)0x67		/*预留*/
#define IN_RESERVE_0x68_SINGLE						(UINT8)0x68		/*预留*/

/***********************************Disney Unsafe Start***************************************************/
#define IN_IVOC_VALIDITY_SINGLE  					(UINT8)0x69		/*(Unsafe)IVOC控制有效*/
/***********************************Disney End***************************************************/

/**********************************成都四线新增非安全输入 Start***************************************/
#define IN_TRAIN_REDUNDANCY_SINGLE					(UINT8)0x6A		/*冗余状态输入(ATP非安全输入)*/
#define IN_TRAIN_FAN1_SINGLE						(UINT8)0x6B		/*风扇1(ATP非安全输入)*/
#define IN_TRAIN_FAN2_SINGLE						(UINT8)0x6C		/*风扇2(ATP非安全输入)*/
#define IN_TRAIN_DOORLOCK_SINGLE					(UINT8)0x6D		/*车门锁闭(ATP非安全输入)*/
#define IN_TRAIN_ESCAPDOOR_UNLOCK_SINGLE			(UINT8)0x6E		/*逃生门紧急解锁手柄在申请位(ATP非安全输入)*/
#define IN_TRAIN_GUESTDOOR_UNLOCK_SINGLE			(UINT8)0x6F		/*客室门紧急解锁手柄在申请位(ATP非安全输入)*/
/**********************************成都四线新增非安全输入 End***************************************/


/********************************成都实验线增加输入，FAO功能和ATO输入 Start***************************/
#define IN_ATP_SMOKE_ALARM_SINGLE  					(UINT8)0x70		/*烟雾报警*/
#define IN_ATP_DOOR_MM_SINGLE  						(UINT8)0x71		/*人工开人工关*/
#define IN_ATP_DOOR_AM_SINGLE  						(UINT8)0x72		/*自动开人工关*/
#define IN_ATP_DOOR_AA_SINGLE  						(UINT8)0x73		/*自动开自动关*/

/*新机场特有开关量增加*/
#define IN_EBI1_FEEDBACK_SINGLE  					(UINT8)0x74		/*紧急继电器1反馈状态*/
#define IN_EBI2_FEEDBACK_SINGLE  					(UINT8)0x75		/*紧急继电器2反馈状态*/
#define IN_TVS1CHECK_SINGLE  						(UINT8)0x76		/*输入TVS1检测*/
#define IN_TVS2CHECK_SINGLE  						(UINT8)0x77		/*输入TVS2检测*/
#define IN_DOORALLCLOSE_SINGLE  					(UINT8)0x78		/*车门关闭按钮*/
/********************************成都实验线增加输入，FAO功能和ATO输入 End***************************/

/***********************************Disney safe Start***************************************************/

#define IN_CM_MODE_SINGLE_MSCP_NC  					(UINT8)0x79		/*(Safe)CM模式常闭*/
#define IN_CM_MODE_SINGLE_MSCP_NO  					(UINT8)0x7A		/*(Safe)CM模式常开*/
#define IN_NONSD_MODE_SINGLE_MSCP_NC  				(UINT8)0x7B		/*(Safe)NONSD 模式,常闭*/
#define IN_NONSD_MODE_SINGLE_MSCP_NO                (UINT8)0x7C		/*(Safe)NONSD 模式，常开*/
#define IN_FWD_MODE_SINGLE_MSCP_NC  				(UINT8)0x7D		/*(Safe)FWD模式,常闭*/
#define IN_FWD_MODE_SINGLE_MSCP_NO  				(UINT8)0x7E		/*(Safe)FWD模式,常开*/
#define IN_REV_MODE_SINGLE_MSCP_NC  				(UINT8)0x7F		/*(Safe)REV模式,常闭*/
#define IN_REV_MODE_SINGLE_MSCP_NO  				(UINT8)0x80		/*(Safe)REV模式,常开*/
#define IN_STBY_MODE_SINGLE_MSCP_NC  				(UINT8)0x81		/*(Safe)端STBY模式,常闭*/
#define IN_STBY_MODE_SINGLE_MSCP_NO  				(UINT8)0x82		/*(Safe)端STBY模式,常开*/
#define IN_SD_MODE_SINGLE_MSCP_NC  					(UINT8)0x83		/*(Safe)端SD模式,常闭*/
#define IN_SD_MODE_SINGLE_MSCP_NO  					(UINT8)0x84		/*(Safe)端SD模式,常开*/
#define IN_EB_FEEDBACK_SINGLE_MSCP_NC  				(UINT8)0x85		/*(Safe)信号系统紧急制动输出反馈采集,常闭*/
#define IN_EB_FEEDBACK_SINGLE_MSCP_NO  				(UINT8)0x86		/*(Safe)信号系统紧急制动输出反馈采集,常开*/
#define IN_CONDUCT_ZERO_SPEED_SINGLE_MSCP_NC  		(UINT8)0x87		/*(Safe)零速信号已实施,常闭*/
#define IN_CONDUCT_ZERO_SPEED_SINGLE_MSCP_NO  		(UINT8)0x88		/*(Safe)零速信号已实施,常开*/
#define IN_SAFE_INTERLOCK_FEEDBACK_SINGLE_MSCP_NC  	(UINT8)0x89		/*(Safe)安全联锁继电器状态采集,常闭*/
#define IN_SAFE_INTERLOCK_FEEDBACK_SINGLE_MSCP_NO  	(UINT8)0x8A		/*(Safe)安全联锁继电器状态采集，常开*/
#define IN_TRAIN_Reserved                           (UINT8)0xFF		/*预留*/

/***********************************Disney safe End***************************************************/


/*********************************IO模块输出，Start*************************************************/

/*安全输出开关量含义--与配置数据匹配*/
#define OUT_EB										(UINT8)0x01		/*EB输出*/
#define OUT_FORBID_EB_RELIEVE						(UINT8)0x02		/*禁止制动缓解输出*/
#define OUT_TRACTION_CUT							(UINT8)0x03		/*ATP牵引切除 */
#define OUT_BEGIN_FSB                               (UINT8)0x04		/*VOBC开启常用制动阀*/
#define OUT_AUTO_SWITCH_SINGLE1                     (UINT8)0x05		/*自动切换电路控制信号1*/
#define OUT_AUTO_SWITCH_SINGLE2                     (UINT8)0x06		/*自动切换电路控制信号2*/
#define OUT_TRAIN_CONTROL_RIGHT						(UINT8)0x07		/*VOBC获取机车控制权*/
#define OUT_RESERVE_0x08                            (UINT8)0x08		/*预留*/
#define OUT_ATP_CONTRAL_TRAIN                       (UINT8)0x09		/*列车处于控车状态*/	
#define OUT_ELECTRIC_BRAKE_CHANL3					(UINT8)0x0A		/*常用制动3（电制动）*/
#define OUT_ELECTRIC_BRAKE_CHANL4					(UINT8)0x0B		/*常用制动4（电制动）*/
#define OUT_FAULT_EB								(UINT8)0x0C		/*ATP故障制动*/
#define OUT_ATP_LEFTDOOR_EN							(UINT8)0x0D		/*左门使能*/
#define OUT_ATP_RIGHTDOOR_EN						(UINT8)0x0E		/*右门使能*/
#define OUT_ATP_AR_LAMP								(UINT8)0x0F		/*AR指示灯输出*/
#define OUT_RESERVE_0x10							(UINT8)0x10		/*预留*/

#define OUT_ATP_AR_RELAY							(UINT8)0x11		/*AR继电器吸起输出*/
#define OUT_ATP_ZERO_SPEEDSIG						(UINT8)0x12		/*零速信号输出*/
#define OUT_ATP_START_LIGHT							(UINT8)0x13		/*车体外指示灯输出*/
#define OUT_ATP_WAKEUP								(UINT8)0x14		/* OutputValue的Bit10唤醒输出*/
#define OUT_ATP_SLEEP								(UINT8)0x15		/* OutputValue的Bit11休眠输出*/
#define OUT_ATP_REVOKE								(UINT8)0x16		/*OutputValue的Bit12ATP重启输出*/
#define OUT_TRAIN_RELIEVE                     		(UINT8)0x17		/* VOBC允许列车缓解*/
#define OUT_TRAIN_LIGHT1                     		(UINT8)0x18		/*健康指示灯1*/
#define OUT_TRAIN_LIGHT2                     		(UINT8)0x19		/*健康指示灯2*/
#define OUT_RESERVE_0x1A							(UINT8)0x1A		/*预留*/
#define OUT_RESERVE_0x1B							(UINT8)0x1B		/*预留*/
#define OUT_RESERVE_0x1C							(UINT8)0x1C		/*预留*/
#define OUT_RESERVE_0x1D							(UINT8)0x1D		/*预留*/
#define OUT_RESERVE_0x1E							(UINT8)0x1E		/*预留*/
#define OUT_RESERVE_0x1F							(UINT8)0x1F		/*预留*/

/*非安全输出开关量含义--与配置数据匹配*/
#define OUT_PASS_NEUTRAL_FAULT						(UINT8)0x20		/*自动过分相故障 */	
#define OUT_PASS_NEUTRAL_PREVIEW					(UINT8)0x21		/*自动过分相预告 */	
#define OUT_PASS_NEUTRAL_FORCE						(UINT8)0x22		/*自动过分相强迫 */	
#define OUT_PILOT_LAMP1                             (UINT8)0x23		/*预留指示灯1 */
#define OUT_PILOT_LAMP2                             (UINT8)0x24		/*预留指示灯2 */
#define OUT_VOBC_FSB_1                              (UINT8)0x25		/*VOBC常用制动1*/
#define OUT_VOBC_FSB_2                              (UINT8)0x26		/*VOBC常用制动2*/
#define OUT_VOBC_FSB_120KPA                         (UINT8)0x27		/*VOBC常用制动120Kpa*/
#define OUT_ATO_ENABLE_1                            (UINT8)0x28		/*ATO使能1*/
#define OUT_ATO_ENABLE_2                            (UINT8)0x29		/*ATO使能2*/
#define OUT_AIR_BRAKE								(UINT8)0x2A		/*空气制动(制动故障隔离)*/

/*成都实验线增加输出，FAO功能和ATO输出*/
#define IN_TRAIN_OPPOSITE_RESTRAT					(UINT8)0x2B		/*VBTC对端重启指令*/
#define IN_TRAIN_CAB_ACTIVE_OUTPUT					(UINT8)0x2C		/*驾驶室激活输出*/
#define ATP_DIRFORWARD_OUTPUT						(UINT8)0x2D		/*FAO新增:方向向前指令(非安全) */
#define ATP_DIRBACKWARD_OUTPUT						(UINT8)0x2E		/*FAO新增:方向向后指令(非安全)*/
#define ATP_FAMMODE_OUTPUT							(UINT8)0x2F		/*FAO新增:FAM模式输出(非安全) */
#define ATP_CAMMODE_OUTPUT							(UINT8)0x30		/*FAO新增:CAM模式输出(非安全) */
#define ATP_JUMPDRDER_OUTPUT						(UINT8)0x31		/*FAO新增:跳跃指令(非安全)*/
#define AOM_PARKING_BRAKING_IMPLEMENT_OUTPUT_ATP    (UINT8)0x32		/*AOM停放制动施加输出*/
#define AOM_PARKING_BRAKING_FREE_OUTPUT_ATP         (UINT8)0x33		/*AOM停放制动缓解输出*/

#define ATO_ADDROFFSET_OUT_ENABLE					(UINT8)0x34		/*ATO已激活 1*/
#define ATO_ADDROFFSET_OUT_TRACTION					(UINT8)0x35		/*牵引状态 2*/
#define ATO_ADDROFFSET_OUT_BRAKE					(UINT8)0x36		/*制动状态 4*/
#define ATO_ADDROFFSET_OUT_HOLD_BRAKE				(UINT8)0x37		/*保持制动 8*/
#define ATO_ADDROFFSET_OUT_LEFT_DOOR_OPEN			(UINT8)0x38		/*客室左车门开启输出 1*/
#define ATO_ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE		(UINT8)0x39		/*全列车左车门关闭输出 2*/
#define ATO_ADDROFFSET_OUT_RIGHT_DOOR_OPEN			(UINT8)0x3A		/*客室右车门开启输出 4*/
#define ATO_ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE		(UINT8)0x3B		/*全列车右车门关闭输出 8*/
#define ATO_ADDROFFSET_START_LIGHT      			(UINT8)0x3C		/*ATO启动灯*/
#define ATO_ADDROFFSET_BAG_LEFTDOOR_OPEN           	(UINT8)0x3D		/*行李车门左门使能*/
#define ATO_ADDROFFSET_BAG_RIGHTDOOR_OPEN           (UINT8)0x3E		/*行李车门右门使能*/
#define OUT_RESERVE_0x3F							(UINT8)0x3F		/*预留*/
#define OUT_RESERVE_0x40							(UINT8)0x40		/*预留*/
#define OUT_RESERVE_0x41							(UINT8)0x41		/*预留*/
/*成都四线新增*/
#define OUT_TRAIN_EMERDOOR                          (UINT8)0x42		/*逃生门紧急解锁授权(逃生门解锁输出,ATP安全输出)*/
#define OUT_TRAIN_REDUNDANCY                        (UINT8)0x43		/*冗余状态输出(ATP非安全输出)*/
#define OUT_TRAIN_REMOTE_RELIFE_BOGIE               (UINT8)0x44		/*远程缓解转向架(ATP非安全输出)*/
#define OUT_TRAIN_DOOR_UNLOCK_LEFT                  (UINT8)0x45		/*左侧客室门紧急解锁输出(ATP安全输出)*/
#define OUT_TRAIN_DOOR_UNLOCK_RIGHT                 (UINT8)0x46		/*右侧客室门紧急解锁输出(ATP安全输出)*/
#define OUT_TRAIN_EMCY_DOOR_UNLOCK_LEFT             (UINT8)0x47		/*左侧客室门+逃生门紧急解锁输出(ATP安全输出)*/
#define OUT_TRAIN_EMCY_DOOR_UNLOCK_RIGHT            (UINT8)0x48		/*右侧客室门+逃生门紧急解锁输出(ATP安全输出)*/
#define OUT_TRAIN_FORCED_RELEASE                    (UINT8)0x49		/*强迫缓解指令(ATP非安全输出)*/
#define OUT_TRAIN_FAM_START_LIGHT                   (UINT8)0x4A		/*FAM动车指示灯(ATP非安全输出)*/
#define OUT_TRAIN_ALL_DOOR_UNLOCK                   (UINT8)0x4B		/*逃生门和客室门紧急解锁*/
#define OUT_NOT_ALLOW_REBOOT                        (UINT8)0x4C     /*ATP输出不允许重启命令*/
#define OUT_ALLOW_REBOOT_ATO                        (UINT8)0x4D     /*ATP输出重启ATO*/

/*********************************IO模块输出，End*************************************************/


/*********************************以下定义为IO模块给应用的定义，切记应用需要改成这个！！！！！！！************************************************/
/*安全输入开关量含义*/
#define IN_TRAIN_CAB_ACTIVE							(UINT8)0x01		/*驾驶室激活*/
#define IN_TRAIN_EB_ENABLE							(UINT8)0x02		/*列车车辆已实施紧急制动*/
#define IN_TRAIN_TRACTION_CUT						(UINT8)0x03		/*牵引已卸载*/
#define IN_TRAIN_DIR_FORWARD						(UINT8)0x04		/*方向手柄向前*/
#define IN_TRAIN_DIR_BACKWARD						(UINT8)0x05		/*方向手柄向后*/
#define IN_TRAIN_HANDLE_TRACTION					(UINT8)0x06		/*牵引手柄在牵引位*/
#define IN_TRAIN_ATP_CONTROL						(UINT8)0x07		/*自动切换电路导向ATP控车*/
#define IN_BCU_IN_ATP							    (UINT8)0x08		/*BCU运行在ATP模式*/
#define IN_TRAIN_AIR_FILL							(UINT8)0x09		/*列车处于充风状态*/
#define IN_LKJ_BRAKE_STATE							(UINT8)0x0A		/*LKJ处于制动实施状态*/
#define IN_BCU_IN_LKJ								(UINT8)0x0B		/*BCU运行在LKJ模式*/
#define IN_TRAIN_OPEN_FSB_VALVE					    (UINT8)0x0C		/*VOBC已开启常用制动阀*/
#define IN_TRAIN_ALLOW_ENTRY_ATO					(UINT8)0x0D		/*列车允许进入ATO*/
#define IN_TRAIN_LKJ_CONTROL	                    (UINT8)0x0E		/*自动切换电路导向LKJ控车*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL1				(UINT8)0x0F		/*常用制动1（电制动）*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL2				(UINT8)0x10		/*常用制动2（电制动）*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL3				(UINT8)0x11		/*常用制动3（电制动）*/
#define IN_TRAIN_ELECTRIC_BRAKE_CHANL4				(UINT8)0x12		/*常用制动4（电制动）*/
#define IN_TRAIN_CONFIRM_BUTTON						(UINT8)0x13		/*列车确认按钮状态 (常开信号)*/
#define IN_TRAIN_RESECT_SWITCH						(UINT8)0x14		/*切除开关在切除位 */    
#define IN_TRAIN_CONFIRM_BUTTON2	                (UINT8)0x70		/* 确认按钮2常闭信号*/
/*非安全输入开关量含义--与配置数据匹配*/
#define IN_ATP_ATO_START1							(UINT8)0x15		/*ATO启动按钮1已按下*/
#define IN_ATP_MODE_UP								(UINT8)0x16		/*模式选择上按钮按下*/
#define IN_ATP_MODE_DOWN							(UINT8)0x17		/*模式选择下按钮按下*/
#define IN_ATP_AR									(UINT8)0x18		/*AR按钮按下*/
#define IN_ATP_RIGHT_DOOR_OPEN						(UINT8)0x19		/*右门开门按钮按下*/
#define IN_ATP_RIGHT_DOOR_CLOSE						(UINT8)0x1A		/*右门关门按钮按下*/
#define IN_ATP_EUM_SWITCH							(UINT8)0x1B		/*EUM开关激活*/
#define IN_ATP_LEFT_DOOR_OPEN						(UINT8)0x1C		/*左门开门按钮按下*/
#define IN_ATP_LEFT_DOOR_CLOSE						(UINT8)0x1D		/*左门关门按钮按下*/
#define IN_ATP_ATO_START2							(UINT8)0x1E		/*ATO启动按钮2(非安全),从输入板2输入*/
#define IN_ATP_TRAIN_INTEG							(UINT8)0x1F		/*列车完整*/
#define IN_ATP_DOOR_CLOSEDANDLOCK					(UINT8)0x20		/*车门关闭且锁闭*/
#define IN_ATP_BCZD_VALID							(UINT8)0x21		/*已实施保持制动*/
#define IN_ATP_SB_RIGHT								(UINT8)0x22		/*牵引制动手柄在零位且方向手柄在向前位*/
#define IN_ATP_SLEEP								(UINT8)0x23		/*休眠*/
#define IN_TRAIN_NOBODY_ALERT_SIGNAL				(UINT8)0x24		/*无人警惕解除相关信号*/
#define IN_TRAIN_WHISTLE_SIGNAL						(UINT8)0x25		/*鸣笛信号*/
#define IN_ATP_TVS1_Check	       					(UINT8)0x26		/*EMC板1的击穿检测（非安全）*/
#define IN_ATP_TVS2_Check	       					(UINT8)0x27		/*EMC板2的击穿检测（非安全）*/
#define IN_CHANGE_SWITCH_LKJ	                    (UINT8)0x28		/* 手动切换开关到LKJ位*/
#define IN_CHANGE_SWITCH_AUTO	                    (UINT8)0x29		/* 手动切换开关到自动位*/
#define IN_TRAIN_FSB_1	                            (UINT8)0x2A		/* 列车已实施常用制动1 */
#define IN_TRAIN_FSB_2	                            (UINT8)0x2B		/* 列车已实施常用制动2,用常用制动1和常用制动2组合成80kp.100Kp,120Kpa*/
#define IN_TRAIN_FSB_120KPA	                        (UINT8)0x2C		/* 列车已实施常用制动120KPa */
#define IN_LKJ_EB_ENABLE	                        (UINT8)0x2D		/*LKJ处于制动实施状态*/
#define IN_TRAIN_CUT_SWITCH	                        (UINT8)0x2E		/* 切除开关在切除位*/
#define IN_TRAIN_RELIEVE	                     	(UINT8)0x2F		/* VOBC允许列车缓解*/
#define IN_ATP_HANDLE_EB	                     	(UINT8)0x30		/* 牵引制动手柄在电制位*/

/***********************************Disney Start***************************************************/
#define IN_CM_MODE									(UINT8)0x31		/*(Safe)CM模式*/
#define IN_NONSD_MODE								(UINT8)0x32		/*(Safe)NONSD 模式*/
#define IN_FWD_MODE									(UINT8)0x33		/*(Safe)FWD模式*/
#define IN_REV_MODE									(UINT8)0x34		/*(Safe)REV模式*/  
#define IN_STBY_MODE								(UINT8)0x35		/*(Safe)端STBY模式*/
#define IN_SD_MODE									(UINT8)0x36		/*(Safe)端SD模式*/
#define IN_EB_FEEDBACK								(UINT8)0x37		/*(Safe)信号系统紧急制动输出反馈采集*/
#define IN_CONDUCT_ZERO_SPEED						(UINT8)0x38		/*(Safe)零速信号已实施*/
#define IN_SAFE_INTERLOCK_FEEDBACK					(UINT8)0x39		/*(Safe)安全联锁继电器状态采集*/
#define IN_IVOC_VALIDITY							(UINT8)0x3A		/*(Unsafe)IVOC控制有效*/
/***********************************Disney End***************************************************/


/*成都实验线增加输入，FAO功能和ATO输入*/
#define IN_TRAIN_KEY_OPEN							(UINT8)0x97		/*司机钥匙打开*/
#define IN_TRAIN_EBHANDLE_ACTIVE					(UINT8)0x71		/*车辆紧急手柄激活*/
#define IN_TRAIN_OBSTDET_INPUT						(UINT8)0x72		/*障碍物脱轨检测*/
#define IN_TRAIN_BRAKE_FAULT						(UINT8)0x73		/*制动重故障*/
#define IN_ATP_TRACTION_RIGHT						(UINT8)0x74		/*牵引制动手柄在牵引位且方向手柄向前*/
#define IN_ATP_SMOKE_ALARM							(UINT8)0x75		/*烟雾报警*/
#define IN_ATP_DOOR_MM								(UINT8)0x76		/*人工开人工关*/
#define IN_ATP_DOOR_AM								(UINT8)0x78		/*自动开人工关*/
#define IN_ATP_DOOR_AA								(UINT8)0x77		/*自动开自动关*/

/*新机场特有开关量增加*/
#define IN_EBI1_FEEDBACK							(UINT8)0x79		/*紧急继电器1反馈状态*/
#define IN_EBI2_FEEDBACK							(UINT8)0x7A		/*紧急继电器2反馈状态*/
#define IN_EMERGENCYDOOR							(UINT8)0x7B		/*逃生门状态(ATP安全输入)*/
#define IN_TVS1CHECK								(UINT8)0x7C		/*输入TVS1检测*/
#define IN_TVS2CHECK								(UINT8)0x7D		/*输入TVS2检测*/
#define IN_DOORALLCLOSE								(UINT8)0x7E		/*车门关闭按钮*/

/*成都四线新增*/
#define IN_TRAIN_DOORCLOSE							(UINT8)0x98		/*车门关闭(ATP安全输入)*/
#define IN_TRAIN_EMERDOORBOARD						(UINT8)0x99		/*逃生门盖板状态监督（逃生门紧急解锁手柄状态,ATP安全输入）*/
#define IN_TRAIN_REDUNDANCY							(UINT8)0x9A		/*冗余状态输入(ATP非安全输入)*/
#define IN_TRAIN_FAN1								(UINT8)0x9B		/*风扇1(ATP非安全输入)*/
#define IN_TRAIN_FAN2								(UINT8)0x9C		/*风扇2(ATP非安全输入)*/
#define IN_TRAIN_DOORLOCK							(UINT8)0x9D		/*车门锁闭(ATP非安全输入)*/
#define IN_TRAIN_ESCAPDOOR_UNLOCK					(UINT8)0x9E		/*逃生门紧急解锁手柄在申请位(ATP非安全输入)*/
#define IN_TRAIN_GUESTDOOR_UNLOCK					(UINT8)0x9F		/*客室门紧急解锁手柄在申请位(ATP非安全输入)*/

typedef struct
{
	/*------安全输入变量定义-------*/
	UINT8 InTrainCabActive;						/*驾驶室激活*/
	UINT8 InTrainEbEnable;						/*列车车辆已实施紧急制动*/
	UINT8 InTrainTractionCut;					/*牵引已卸载*/
	UINT8 InTrainDirForward;					/*方向手柄向前*/
	UINT8 InTrainDirBackward;					/*方向手柄向后*/
	UINT8 InTrainHandleTraction;				/*牵引手柄在牵引位*/
	UINT8 InTrainAtpControl;					/*自动切换电路导向ATP控车*/
	UINT8 InTrainBcuInAtp;						/*BCU运行在ATP模式*/
	UINT8 InTrainAirFill;						/*列车处于充风状态*/
	UINT8 InLkjBrakeState;						/*LKJ处于制动实施状态*/
	UINT8 InTrainBcuInLkj;						/*BCU运行在LKJ模式*/
	UINT8 InTrainOpenFsbValve;					/*VOBC已开启常用制动阀*/
	UINT8 InTrainAllowEntryAto;					/*列车允许进入ATO*/
	UINT8 InTrainLkjControl;					/* 自动切换电路导向LKJ控车*/
	UINT8 InTrainElectricBrakeChanl1;			/*常用制动1（电制动）*/
	UINT8 InTrainElectricBrakeChanl2;			/*常用制动2（电制动）*/
	UINT8 InTrainElectricBrakeChanl3;			/*常用制动3（电制动）*/
	UINT8 InTrainElectricBrakeChanl4;			/*常用制动4（电制动）*/
	UINT8 InTrainConfirmButton;					/*列车确认按钮状态 */
	UINT8 InTrainResectSwitch;					/*切除开关在切除位 */
	UINT8 InAtpModeUp;							/*模式选择上按钮按下*/
	UINT8 InAtpModeDown;						/*模式选择下按钮按下*/
	UINT8 InAtpArInput;							/*AR按钮按下*/
	UINT8 InAtpRightDoorOpen;					/*右门开门按钮按下*/
	UINT8 InAtpRightDoorClose;					/*右门关门按钮按下*/
	UINT8 InAtpEumSwitch;						/*EUM开关激活*/
	UINT8 InAtpLeftDoorOpen;					/*左门开门按钮按下*/
	UINT8 InAtpLeftDoorClose;					/*左门关门按钮按下*/
	UINT8 InAtpTrainInteg;						/*列车完整*/
	UINT8 InTrainDoorCloseAndLock;				/*车门关闭且锁闭*/
	UINT8 InAtpBczdValid;						/*已实施保持制动*/
	UINT8 InAtpSbRightInput;					/*牵引制动手柄在零位且方向手柄在向前位*/
	UINT8 InAtpSleep;							/*休眠*/
	UINT8 InAtpConfirmButton2;					/* 确认按钮2的状态*/
	UINT8 InTrainDoorClose;						/*车门关闭*/
	UINT8 InTrainEmergencyDoorBoard;			/*逃生门盖板状态监督（逃生门紧急解锁手柄状态）*/

	/*------非安全输入变量定义-------*/
	UINT8 InTrainNobodyAlertSignal;				/*无人警惕解除相关信号*/
	UINT8 InTrainWhistleSignal;					/*鸣笛信号*/
	UINT8 InAtpAtoStart1;						/*ATO启动按钮1已按下*/
	UINT8 InAtpAtoStart2;						/*ATO启动按钮2已按下(非安全)*/
	UINT8 InAtpTvs1Check;						/*EMC板1的击穿检测（非安全）*/
	UINT8 InAtpTvs2Check;						/*EMC板2的击穿检测（非安全）*/
	UINT8 InChangeSwitchLkj;					/* 手动切换开关到LKJ位*/
	UINT8 InChangeSwitchAuto;					/* 手动切换开关到自动位*/
	UINT8 InTrainFsb1;							/* 列车已实施常用制动1 */
	UINT8 InTrainFsb2;							/* 列车已实施常用制动2 */
	UINT8 InTrainFsb120Kpa;						/* 列车已实施常用制动120KPa */
	UINT8 InLkjEbEnable;						/*LKJ处于制动实施状态*/
	UINT8 InTrainCutSwitch;						/* 切除开关在切除位【预留，车车和510使用InAtpEumSwitch】*/
	UINT8 InTrainRelieve;						/* VOBC允许列车缓解*/
	UINT8 InHandleEB;							/* 牵引制动手柄在电制位*/

	/*------迪士尼安全&非安全输入变量定义-------*/
	UINT8 InAtpCMMode;							/*(Safe)CM模式*/
	UINT8 InAtpNotSDMode;						/*(Safe)NOT SD 模式*/
	UINT8 InAtpFORMode;							/*(Safe)FOR模式*/
	UINT8 InAtpREVMode;							/*(Safe)REV模式*/
	UINT8 InAtpSBMode;							/*(Safe)端SB模式*/
	UINT8 InAtpSDMode;							/*(Safe)端SD模式*/
	UINT8 InAtpEBFeedback;						/*(Safe)信号系统紧急制动输出反馈采集*/
	UINT8 InAtpConductZeroSpeed;				/*(Safe)零速信号已实施*/
	UINT8 InAtpSafeInterLockFeedback;			/*(Safe)安全联锁继电器状态采集*/
	UINT8 InAtpIVOCValidity;					/*(Unsafe)IVOC控制有效*/

	/*开关量输入,H-FAO新增*/
	UINT8 TRAIN_KEY_OPEN;						/*司机钥匙激活*/
	UINT8 TRAIN_EBHANDLE_ACTIVE;				/*紧急手柄激活*/
	UINT8 TRAIN_OBSTDET_INPUT;					/*障碍物脱轨有效*/
	UINT8 TRAIN_BRAKE_FAULT;					/*制动重故障*/
	UINT8 ATP_TRACTION_RIGHT;					/*牵引手柄在牵引位且方向手柄在前位*/
	UINT8 ATP_SMOKE_ALARM;						/*烟火报警*/
	UINT8 ATP_DOOR_MM;							/*人工开人工关*/
	UINT8 ATP_DOOR_AM;							/*自动开人工关*/
	UINT8 ATP_DOOR_AA;							/*自动开自动关*/
	/*新机场*/
	UINT8 EBI1_FEEDBACK;						/*紧急继电器1反馈状态*/
	UINT8 EBI2_FEEDBACK;						/*紧急继电器2反馈状态*/
	UINT8 EMERGENCYDOOR;						/*逃生门状态(ATP安全输入)*/
	UINT8 TVS1CHECK;							/*输入TVS1检测*/
	UINT8 TVS2CHECK;							/*输入TVS2检测*/
	UINT8 DOORALLCLOSE;							/*车门关闭按钮*/

	/*成都四线开关量输入*/
	UINT8 InTrainRedundancy;					/*冗余状态输入*/
	UINT8 InTrainFan1;							/*风扇1*/
	UINT8 InTrainFan2;							/*风扇2*/
	UINT8 InTrainDoorLock;						/*车门锁闭*/
	UINT8 InTrainEscapDoorUnlock;				/*逃生门紧急解锁手柄在申请位*/
	UINT8 InTrainGuestDoorUnlock;				/*客室门紧急解锁手柄在申请位*/
}IO_INPUT_STRUCT;

typedef struct
{
	/*------安全输出变量定义-------*/
	UINT8 OutEb;								/*EB输出*/
	UINT8 OutForbidEbRelieve;					/*禁止制动缓解输出*/
	UINT8 OutTractionCut;						/*ATP牵引切除 */
	UINT8 OutBeginFsb;							/*VOBC开启常用制动阀*/
	UINT8 OutAutoSwitchSingle1;					/*自动切换电路控制信号1*/
	UINT8 OutAutoSwitchSingle2;					/*自动切换电路控制信号2*/
	UINT8 OutTrainControlRight;					/*VOBC获取机车控制权*/
	UINT8 OutAtpContralTrain;					/* 列车处于控车状态*/
	UINT8 OutElectricBrakeChanl3;				/*常用制动3（电制动）*/
	UINT8 OutElectricBrakeChanl4;				/*常用制动4（电制动）*/
	UINT8 OutFaultEb;							/*ATP故障制动*/
	UINT8 OutAtpLeftDoorEn;						/*左门使能*/
	UINT8 OutAtpRightDoorEn;					/*右门使能*/
	UINT8 OutAtpArLamp;							/*AR指示灯输出*/

	UINT8 OutAtpArRelay;						/*AR继电器吸起输出*/
	UINT8 OutAtpZeroSpeedsig;					/*零速信号输出*/
	UINT8 OutAtpWakeup;							/* OutputValue的Bit10唤醒输出*/
	UINT8 OutAtpSleep;							/* OutputValue的Bit11休眠输出*/
	UINT8 OutAtpRevoke;							/*OutputValue的Bit12ATP重启输出*/
	UINT8 OutTrainRelieve;						/* VOBC允许列车缓解*/
	UINT8 ATPSafeLigth1;						/*安全指示灯1*/
	UINT8 ATPSafeLigth2;						/*安全指示灯2*/
												/*成都四线新增*/
	UINT8 EmcyDoorUnlock;						/*逃生门紧急解锁授权(逃生门解锁输出)*/
	UINT8 OutDoorUnlockLeft;					/*左侧客室门紧急解锁输出*/
	UINT8 OutDoorUnlockRight;					/*右侧客室门紧急解锁输出*/
	UINT8 OutEmcyUnlockLeft;					/*左侧客室门+逃生门紧急解锁输出*/
	UINT8 OutEmcyUnlockRight;					/*右侧客室门+逃生门紧急解锁输出*/

	/*------非安全输出变量定义-------*/
	UINT8 OutPassNeutralFault;					/*自动过分相故障 */
	UINT8 OutPassNeutralPreview;				/* 自动过分相预告 */
	UINT8 OutPassNeutralForce;					/*自动过分相强迫 */
	UINT8 OutPilotLamp1;						/*预留指示灯1 */
	UINT8 OutPilotLamp2;						/*预留指示灯2 */
	UINT8 OutVobcFsb1;							/*VOBC常用制动1*/
	UINT8 OutVobcFsb2;							/*VOBC常用制动2*/
	/*UINT8 OutVobcFsb120Kpa; VOBC常用制动120Kpa*/
	UINT8 OutForbidTrainAirFill;				/*VOBC禁止列车充风*/

	UINT8 OutStartLight;						/*车体外指示灯输出*/
	UINT8 TRAIN_OPPOSITE_RESTRAT;				/*对端重启命令*/
	UINT8 TRAIN_CAB_ACTIVE_OUTPUT;				/*AR继电器吸起输出(驾驶室激活输出)*/
	UINT8 DIRFORWARD_OUTPUT;					/*方向向前信号*/
	UINT8 DIRBACKWARD_OUTPUT;					/*方向向后信号*/
	UINT8 FAMMODE_OUTPUT;						/*FAM模式输出*/
	UINT8 CAMMODE_OUTPUT;						/*CAM模式输出*/
	UINT8 JUMPDRDER_OUTPUT;						/*跳跃指令*/
	UINT8 PARKING_BRAKING_IMPLEMENT_OUTPUT;		/*停放制动输出*/
	UINT8 PARKING_BRAKING_FREE_OUTPUT;			/*停放制动缓解*/

	UINT8 ADDROFFSET_OUT_ENABLE;				/*ATO已激活*/
	UINT8 ADDROFFSET_OUT_TRACTION;				/*牵引状态*/
	UINT8 ADDROFFSET_OUT_BRAKE;					/*制动状态*/
	UINT8 ADDROFFSET_OUT_HOLD_BRAKE;			/*保持制动*/
	UINT8 ADDROFFSET_OUT_LEFT_DOOR_OPEN;		/*客室左车门开启输出*/
	UINT8 ADDROFFSET_OUT_ALL_LEFT_DOOR_CLOSE;	/*全列车左车门关闭输出*/
	UINT8 ADDROFFSET_OUT_RIGHT_DOOR_OPEN;		/*客室右车门开启输出*/
	UINT8 ADDROFFSET_OUT_ALL_RIGHT_DOOR_CLOSE;	/*全列车右车门关闭输出*/
	UINT8 ADDROFFSET_START_LIGHT;				/*ATO启动灯*/
	UINT8 ADDROFFSET_BAG_LEFTDOOR_OPEN;			/*行李车门左门使能*/
	UINT8 ADDROFFSET_BAG_RIGHTDOOR_OPEN;		/*行李车门右门使能*/
	UINT8 ATO_ENABLE_1;							/*ATO使能1*/
	UINT8 ATO_ENABLE_2;							/*ATO使能2*/
	UINT8 AIR_BRAKE;							/*空气制动隔离输出*/

	UINT8 OutTrainFAMStartLamp;					/*FAM动车指示灯*/
	UINT8 OutEmergencyDoorUnlock;				/*逃生门和客室门解锁*/
	UINT8 Redundancy;							/*冗余状态输出*/
	UINT8 OutRemoteRelifeBogie;					/*远程缓解转向架*/
	UINT8 OutForcedReleaseCmd;					/*强迫缓解指令*/

	UINT8 OutNotAllowReboot;                 /*禁止重启命令*/
	UINT8 OutRebootAto;                      /*ATO重启命令*/
}IO_OUTPUT_STRUCT;

/*IO输入模块的输出信息结构体*/
typedef struct
{
	UINT8 MainCtrSide;				/*控制端*/
	UINT8 IdleSide;					/*等待端*/
	UINT8 Tc1DigtalInputState[INPUT_DEFINE_MAX];	/*TC1端开关量,整个表为所有的索引和*/
	UINT8 Tc2DigtalInputState[INPUT_DEFINE_MAX];	/*TC2端开关量*/
	UINT8 CtrlDigtalInputState[INPUT_DEFINE_MAX];	/*控制端开关量*/
	UINT8 IdleDigtalInputState[INPUT_DEFINE_MAX];	/*等待端开关量*/
}IO_IN_PERIOD_OUTPUT_STRUCT;

/*IO输出模块的输入信息结构体*/
typedef struct
{
	UINT32 CycCount;  							/*周期号*/
	UINT32 OutUnSafeDataMask;					/*非安全数据有效位掩码*/
	UINT32 OutSafeDataMask;						/*安全开关量有效位掩码*/
	UINT8 dwAnaValueValid;						/*输出的模拟量数据有效掩码*/
	UINT8 dwPwmValueValid;						/*PWM数据有效掩码*/
	UINT8 AtoCmdEnable;							/*输出ATO授权标志*/
	UINT8 MainCtrlSide;							/*初始化为无主控端*/
	UINT8 IdleSide;								/*初始化为无主控端*/
	UINT8 LocalVobcAttri;						/*本端是否为控制端*/
	UINT8 WakeCtrlIVOC;							/*唤醒输出激活端*/
	IO_OUTPUT_STRUCT IOOutputStruCtrl;			/*控制端IO信息*/
	IO_OUTPUT_STRUCT IOOutputStruIdle;			/*等待端IO信息*/
	UINT16 wCtrlSideSwValue;					/*控制端的开关量，非控制端输出固定无效值*/
	UINT32 dwCtrlSideAnaValue;					/*控制端的模拟量输出,非控制端输出固定无效值*/
	UINT32 dwCtrlSidePwmValue;					/*控制端的pwm输出,非控制端输出固定无效值*/
	UINT8 FAMReverseTag;						/*FAM折返标志*/
	UINT8 ARReverseTag;							/*AR折返标志*/
	UINT8 ReverseTag;							/*折返输出激活端*/
}IO_OUT_PERIOD_INPUT_STRUCT;

/*配成SID单节点时所有常开常闭节点结构体*/
typedef struct
{
	/*----------------安全输入变量定义---------------------*/
	/*------安全输入变量定义-------*/
	UINT8 InTrainCabActiveMscpNc;				/*驾驶室激活常闭*/
	UINT8 InTrainCabActiveMscpNo;				/*驾驶室激活常开*/
	UINT8 InTrainEbEnableMscpNc;				/*列车车辆已实施紧急制动常闭*/
	UINT8 InTrainEbEnableMscpNo;				/*列车车辆已实施紧急制动常开*/
	UINT8 InTrainTractionCutMscpNc;				/*牵引已卸载常闭*/
	UINT8 InTrainTractionCutMscpNo;				/*牵引已卸载常开*/
	UINT8 InTrainDirForwardMscpNc;				/*方向手柄向前常闭*/
	UINT8 InTrainDirForwardMscpNo;				/*方向手柄向前常开*/
	UINT8 InTrainDirBackwardMscpNc;				/*方向手柄向后常闭*/
	UINT8 InTrainDirBackwardMscpNo;				/*方向手柄向后常开*/
	UINT8 InTrainHandleTractionMscpNc;			/*牵引手柄在牵引位常闭*/
	UINT8 InTrainHandleTractionMscpNo;			/*牵引手柄在牵引位常开*/
	UINT8 InTrainAtpControlMscpNc;				/*自动切换电路导向ATP控车常闭*/
	UINT8 InTrainAtpControlMscpNo;				/*自动切换电路导向ATP控车常开*/
	UINT8 InTrainBcuInAtpMscpNc;				/*BCU运行在ATP模式常闭*/
	UINT8 InTrainBcuInAtpMscpNo;				/*BCU运行在ATP模式常开*/
	UINT8 InTrainAirFillMscpNc;					/*列车处于充风状态常闭*/
	UINT8 InTrainAirFillMscpNo;					/*列车处于充风状态常开*/
	UINT8 InLkjBrakeStateMscpNc;				/*LKJ处于制动实施状态常闭*/
	UINT8 InLkjBrakeStateMscpNo;				/*LKJ处于制动实施状态常开*/
	UINT8 InTrainBcuInLkjMscpNc;				/*BCU运行在LKJ模式常闭*/
	UINT8 InTrainBcuInLkjMscpNo;				/*BCU运行在LKJ模式常开*/
	UINT8 InTrainOpenFsbValveMscpNc;			/*VOBC已开启常用制动阀常闭*/
	UINT8 InTrainOpenFsbValveMscpNo;			/*VOBC已开启常用制动阀常开*/
	UINT8 InTrainAllowEntryAtoMscpNc;			/*列车允许进入ATO常闭*/
	UINT8 InTrainAllowEntryAtoMscpNo;			/*列车允许进入ATO常开*/
	UINT8 InTrainLkjControlMscpNc;				/* 自动切换电路导向LKJ控车常闭*/
	UINT8 InTrainLkjControlMscpNo;				/* 自动切换电路导向LKJ控车常开*/
	UINT8 InTrainElectricBrakeChanl1MscpNc;		/*常用制动1（电制动）常闭*/
	UINT8 InTrainElectricBrakeChanl1MscpNo;		/*常用制动1（电制动）常开*/
	UINT8 InTrainElectricBrakeChanl2MscpNc;		/*常用制动2（电制动）常闭*/
	UINT8 InTrainElectricBrakeChanl2MscpNo;		/*常用制动2（电制动）常开*/
	UINT8 InTrainElectricBrakeChanl3MscpNc;		/*常用制动3（电制动）常闭*/
	UINT8 InTrainElectricBrakeChanl3MscpNo;		/*常用制动3（电制动）常开*/
	UINT8 InTrainElectricBrakeChanl4MscpNc;		/*常用制动4（电制动）常闭*/
	UINT8 InTrainElectricBrakeChanl4MscpNo;		/*常用制动4（电制动）常开*/
	UINT8 InTrainConfirmButtonMscpNc;			/*列车确认按钮状态 常闭*/
	UINT8 InTrainConfirmButtonMscpNo;			/*列车确认按钮状态 常开*/
	UINT8 InTrainResectSwitchMscpNc;			/*切除开关在切除位 常闭*/
	UINT8 InTrainResectSwitchMscpNo;			/*切除开关在切除位 常开*/
	UINT8 InAtpTrainIntegMscpNc;				/*列车完整常闭*/
	UINT8 InAtpTrainIntegMscpNo;				/*列车完整常开*/
	UINT8 InAtpBczdValidMscpNc;					/*已实施保持制动常闭*/
	UINT8 InAtpBczdValidMscpNo;					/*已实施保持制动常开*/
	UINT8 InAtpSbRightInputMscpNc;				/*牵引制动手柄在零位且方向手柄在向前位常闭*/
	UINT8 InAtpSbRightInputMscpNo;				/*牵引制动手柄在零位且方向手柄在向前位常开*/
	UINT8 InTrainEmergencyDoorBoardMscpNc;		/*逃生门盖板状态监督（逃生门紧急解锁手柄状态）常闭*/
	UINT8 InTrainEmergencyDoorBoardMscpNo;		/*逃生门盖板状态监督（逃生门紧急解锁手柄状态）常开*/
	UINT8 InTrainDoorCloseMscpNc;				/*车门关闭常闭*/
	UINT8 InTrainDoorCloseMscpNo;				/*车门关闭常开*/
	UINT8 TRAIN_KEY_OPENMscpNc;					/*司机钥匙激活常闭*/
	UINT8 TRAIN_KEY_OPENMscpNo;					/*司机钥匙激活常开*/
	UINT8 InTrainDoorCloseAndLockMscpNc;		/*车门关闭且锁闭常闭*/
	UINT8 InTrainDoorCloseAndLockMscpNo;		/*车门关闭且锁闭常开*/
	UINT8 TRAIN_EBHANDLE_ACTIVEMscpNc;			/*紧急手柄激活，常闭*/
	UINT8 TRAIN_EBHANDLE_ACTIVEMscpNo;			/*紧急手柄激活，常开*/
	UINT8 TRAIN_OBSTDET_INPUTMscpNc;			/*障碍物脱轨有效，常闭*/
	UINT8 TRAIN_OBSTDET_INPUTMscpNo;			/*障碍物脱轨有效，常开*/
	UINT8 InDouble1DoorCloseLockMscpNc;			/*迪士尼车门关闭且锁闭（双1有效）常闭*/
	UINT8 InDouble1DoorCloseLockMscpNo;			/*迪士尼车门关闭且锁闭（双1有效）常开*/
	UINT8 TRAIN_BRAKE_FAULTMscpNc;				/*制动重故障，常闭*/
	UINT8 TRAIN_BRAKE_FAULTMscpNo;				/*制动重故障，常开*/
	UINT8 ATP_TRACTION_RIGHTMscpNc;				/*牵引制动手柄在牵引位且方向向前,常闭*/
	UINT8 ATP_TRACTION_RIGHTMscpNo;				/*牵引制动手柄在牵引位且方向向前,常开*/
	UINT8 EMERGENCYDOORMscpNc;					/*逃生门状态常闭*/
	UINT8 EMERGENCYDOORMscpNo;					/*逃生门状态常开*/

	UINT8 InAtpCMModeMscpNc;					/*(Safe)CM模式,常闭*/
	UINT8 InAtpCMModeMscpNo;					/*(Safe)CM模式,常开*/
	UINT8 InAtpNotSDModeMscpNc;					/*(Safe)NOT SD 模式,常闭*/
	UINT8 InAtpNotSDModeMscpNo;					/*(Safe)NOT SD 模式,常开*/
	UINT8 InAtpFORModeMscpNc;					/*(Safe)FOR模式,常闭*/
	UINT8 InAtpFORModeMscpNo;					/*(Safe)FOR模式,常开*/
	UINT8 InAtpREVModeMscpNc;					/*(Safe)REV模式,常闭*/
	UINT8 InAtpREVModeMscpNo;					/*(Safe)REV模式,常开*/
	UINT8 InAtpSBModeMscpNc;					/*(Safe)端SB模式，常闭*/
	UINT8 InAtpSBModeMscpNo;					/*(Safe)端SB模式，常开*/
	UINT8 InAtpSDModeMscpNc;					/*(Safe)端SD模式,常闭*/
	UINT8 InAtpSDModeMscpNo;					/*(Safe)端SD模式,常开*/
	UINT8 InAtpEBFeedbackMscpNc;				/*(Safe)信号系统紧急制动输出反馈采集，常闭*/
	UINT8 InAtpEBFeedbackMscpNo;				/*(Safe)信号系统紧急制动输出反馈采集，常开*/
	UINT8 InAtpConductZeroSpeedMscpNc;			/*(Safe)零速信号已实施,常闭*/
	UINT8 InAtpConductZeroSpeedMscpNo;			/*(Safe)零速信号已实施,常开*/
	UINT8 InAtpSafeInterLockFeedbackMscpNc;		/*(Safe)安全联锁继电器状态采集，常闭*/
	UINT8 InAtpSafeInterLockFeedbackMscpNo;		/*(Safe)安全联锁继电器状态采集，常开*/

	/*TODO 以下几个是其他项目的，最后需要确认是安全还是非安全*/
	UINT8 InAtpKeyOpen;
	UINT8 InAtpSleep;

	/*------非安全输入变量定义-------*/
	UINT8 InAtpModeUp;							/*模式选择上按钮按下*/
	UINT8 InAtpModeDown;						/*模式选择下按钮按下*/
	UINT8 InAtpArInput;							/*AR按钮按下*/
	UINT8 InAtpRightDoorOpen;					/*右门开门按钮按下*/
	UINT8 InAtpRightDoorClose;					/*右门关门按钮按下*/
	UINT8 InAtpEumSwitch;						/*EUM开关激活*/
	UINT8 InAtpLeftDoorOpen;					/*左门开门按钮按下*/
	UINT8 InAtpLeftDoorClose;					/*左门关门按钮按下*/
	UINT8 InTrainDoorLock;						/*车门锁闭*/
	UINT8 InAtpConfirmButton2;					/* 确认按钮2的状态*/
	UINT8 InTrainNobodyAlertSignal;				/*无人警惕解除相关信号*/
	UINT8 InTrainWhistleSignal;					/*鸣笛信号*/
	UINT8 InAtpAtoStart1;						/*ATO启动按钮1已按下*/
	UINT8 InAtpAtoStart2;						/*ATO启动按钮2已按下(非安全)*/
	UINT8 InAtpTvs1Check;						/*EMC板1的击穿检测（非安全）*/
	UINT8 InAtpTvs2Check;						/*EMC板2的击穿检测（非安全）*/
	UINT8 InChangeSwitchLkj;					/* 手动切换开关到LKJ位*/
	UINT8 InChangeSwitchAuto;					/* 手动切换开关到自动位*/
	UINT8 InTrainFsb1;							/* 列车已实施常用制动1 */
	UINT8 InTrainFsb2;							/* 列车已实施常用制动2 */
	UINT8 InTrainFsb120Kpa;						/* 列车已实施常用制动120KPa */
	UINT8 InLkjEbEnable;						/*LKJ处于制动实施状态*/
	UINT8 InTrainCutSwitch;						/* 切除开关在切除位【预留，车车和510使用InAtpEumSwitch】*/
	UINT8 InTrainRelieve;						/* VOBC允许列车缓解*/
	UINT8 InHandleEB;							/* 牵引制动手柄在电制位*/

	/*------迪士尼安全&非安全输入变量定义-------*/
	UINT8 InAtpIVOCValidity;					/*(Unsafe)IVOC控制有效*/
	/*开关量输入,H-FAO新增*/
	UINT8 ATP_SMOKE_ALARM;						/*烟火报警*/
	UINT8 ATP_DOOR_MM;							/*人工开人工关*/
	UINT8 ATP_DOOR_AM;							/*自动开人工关*/
	UINT8 ATP_DOOR_AA;							/*自动开自动关*/
	/*新机场*/
	UINT8 EBI1_FEEDBACK;						/*紧急继电器1反馈状态*/
	UINT8 EBI2_FEEDBACK;						/*紧急继电器2反馈状态*/
	UINT8 TVS1CHECK;							/*输入TVS1检测*/
	UINT8 TVS2CHECK;							/*输入TVS2检测*/
	UINT8 DOORALLCLOSE;							/*车门关闭按钮*/
	/*成都四线开关量输入*/
	UINT8 InTrainRedundancy;					/*冗余状态输入*/
	UINT8 InTrainFan1;							/*风扇1*/
	UINT8 InTrainFan2;							/*风扇2*/
	UINT8 InTrainEscapDoorUnlock;				/*逃生门紧急解锁手柄在申请位*/
	UINT8 InTrainGuestDoorUnlock;				/*客室门紧急解锁手柄在申请位*/
	UINT8 reservedInput;						/*预留位*/
}IO_SINGLE_INPUT_STRUCT;

/***************************************************************************************
* 功能描述:			IO输入模块需要初始化的值以及读取IO配置的数据
* 输入参数:			无
* 输入输出参数:		无
* 输出参数:
* 全局变量:
* 返回值:			0x00:初始化成功
*					0x01:读取IO配置数据失败
* 修改记录：
****************************************************************************************/
UINT8 IoModule_Api_PowerOnInit(IN const IO_CFG_DATA_STRU *pIoConfigDataStru);

/***************************************************************************************
* 功能描述:			IO输入的数据处理,将接收到的IO数据解析存放到IO输入数据的结构体中
* 输入参数:			无
* 输入输出参数:		无
* 输出参数:			IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo io输出结构体
* 全局变量:
* 返回值:           0x00：成功；
*					0x01:接收TC1端IO报文有误
*					0x02:处理TC1端IO报文有误
*					0x04:接收TC2端IO报文有误
*					0x08:处理TC2端IO报文有误
* 修改记录：		增加单节点结构体初始化，added by Jielu 20230408
****************************************************************************************/
UINT8 IoModuleInPut_Api_PreiodProcess(OUT IO_IN_PERIOD_OUTPUT_STRUCT *pIoIn_OutputInfo);

/***************************************************************************************
* 功能描述:			IO输出结构体中的数据打包成数据流，分别发送给1端和2端
* 输入参数:			IN IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo IO输出结构体
* 输入输出参数:		无
* 输出参数:			无
* 全局变量:			无
* 返回值:			0x00：成功；
*					FUNC_CODE1:获取控制端失败
*					FUNC_CODE2:有控制端时，主控端输出失败；
*					FUNC_CODE3:有控制端时，非主控端输出失败；
*					FUNC_CODE4:无控制端时，TC1端输出失败；
*					FUNC_CODE5:无控制端时，TC2端输出失败；
* 修改记录：
****************************************************************************************/
UINT32 IoModulOutPut_Api_PreiodProcess(IN IO_OUT_PERIOD_INPUT_STRUCT *pIoOut_InputInfo, IN UCHAR LastActiveState);

/***************************************************************************************
* 功能描述:			IO输出模块需要初始化的值以及读取IO配置的数据
* 输入参数:			无
* 输入输出参数:		无
* 输出参数:
* 全局变量:
* 返回值:			0x00:初始化成功
*					0x01:读取IO配置数据失败
* 修改记录：
****************************************************************************************/
UINT8 IoOutModule_Api_PowerOnInit(IN const UINT8 *IvocCtrlType);

/***************************************************************************************
* 功能描述:     获取静态数据
* 输入参数:     *pGetIoInfoBuff  需要同步的数据流
*				UINT16* pBuffLen  需要同步的数据长度
*				UINT16 BuffMaxLen 该模块同步的最大数据长度
* 输出参数:     无
* 全局变量:     无
* 返回值:       FUNC_CODE1 默认错误值
*				FUNC_CODE2 传入空指针
*				FUNC_CODE3 Get的数据长度过长
*				FUNC_SUCESS 执行成功无
* 修改记录：	lsn 2020/12/15
****************************************************************************************/
UINT8 GetIoInfoStaticDate(UINT8 *pGetIoInfoBuff, UINT16* pBuffLen, UINT16 BuffMaxLen);

/***************************************************************************************
* 功能描述:		设置静态数据
* 输入参数:		*pSetIoInfoBuff 备系需要跟随主系的数据
*				BuffLen 跟随数据的长度
*
* 输出参数:		无
* 全局变量:		无
* 返回值:		FUNC_CODE1 默认错误值
*				FUNC_CODE2 传入空指针
*				FUNC_CODE3 Set的数据长度过长
*				FUNC_SUCESS 执行成功
* 修改记录：	lsn 2020/12/15
****************************************************************************************/
UINT8 SetIoInfoStaticDate(UINT8 *pSetIoInfoBuff, UINT16 *BuffLen);
#if 0
/***************************************************************************************
* 功能描述:     记录本模块2x跟随数据的首地址与长度
* 输入参数:     无
* 输入输出参数:  无
* 输出参数:     无
* 全局变量:     本模块所有全局静态变量
* 返回值:      无
* 修改记录：
****************************************************************************************/
void GetIoInfoPrintf(void);
#endif
/***************************************************************************************
* 功能描述:			分别用于ATP/ATO计算掩码值
* 输入参数:
* 输入输出参数:		无
* 输出参数:			ATP掩码，ATO掩码
* 全局变量:			无
* 返回值:			0x00：成功；
* 修改记录：		added by Jielu 20230326
****************************************************************************************/
void IoModulCalMask_Api_PreiodProcess(OUT UINT32 *atpMask, OUT UINT32 *atoMask, IN IO_CFG_DATA_STRU *IOConfigFunc);
#endif 
