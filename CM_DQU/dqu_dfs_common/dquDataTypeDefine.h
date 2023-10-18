/************************************************************************
*
* 文件名   ：  dquDataTypeDefine.h
* 版权说明 ：  北京交控科技有限公司
* 版本号   ：  1.0
* 创建时间 ：  2011.09.22
* 作者     ：  软件部
* 功能描述 ：  文件系统文件预处理过程
* 使用注意 ：
* 修改记录 ：
*
************************************************************************/


#ifndef    DQU_DATA_TYPE_DEFINE_H
#define    DQU_DATA_TYPE_DEFINE_H


#define EMAP_DATA_TYPE						0x10u	/*电子地图ID*/

/*电子地图各数据ID*/
#define EMAP_NOP_0							0u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_LINK_ID						1u		/*link数据ID*/
#define EMAP_POINT_ID						2u		/*道岔数据ID*/
#define EMAP_SIGNAL_ID						3u		/*信号机数据ID*/
#define EMAP_BALISE_ID						4u		/*应答器数据ID*/
#define EMAP_AXLE_ID						5u		/*计轴器数据ID*/
#define EMAP_SCREEN_ID						6u		/*安全屏蔽门数据ID*/
#define EMAP_EMERG_STOP_BUTTON_ID			7u		/*紧急停车按钮数据ID*/
#define EMAP_STATION_ID						8u		/*停车区域数据ID*/
#define EMAP_STOPPOINT_ID					9u		/*停车点数据ID*/
#define EMAP_AXLE_SGMT_ID					10u		/*计轴区段数据ID*/
#define EMAP_LOGIC_SGMT_ID					11u		/*逻辑区段数据ID 42 -> 11*/
#define EMAP_ROUTE_ID						12u		/*进路数据ID*/
#define EMAP_PROTECT_LINK_ID				13u		/*保护区段数据ID*/
#define EMAP_BLOC_ACCESS_LINK_ID			14u		/*点式接近区段数据ID*/
#define EMAP_CBTC_ACCESS_LINK_ID			15u		/*CBTC接近区段数据ID*/
#define EMAP_GRADE_ID						16u		/*坡度数据ID*/
#define EMAP_STATIC_RES_SPEED_ID			17u		/*静态限速数据ID*/
#define EMAP_DEFAULT_ROUTE_ID				18u		/*默认行车序列数据ID*/
#define EMAP_RUN_LEVEL_ID					19u		/*站间运营级别数据ID*/
#define EMAP_EXIT_ROUTE_ID					20u		/*退出CBTC区域数据ID*/
#define EMAP_TRAIN_LINE_ID					21u		/*非CBTC区域数据ID*/
#define EMAP_AR_ID							22u		/*自动折返数据ID*/
#define EMAP_ZC_DIVID_POINT_ID				23u		/*ZC分界点数据ID*/
#define EMAP_ZC_COM_ZONE_ID					24u		/*ZC共管区域数据ID*/
#define EMAP_CI_ID							25u		/*CI数据ID*/
#define EMAP_ATS_ID							26u		/*ATS数据ID*/
#define EMAP_CONRESSPEED_ID					27u		/*统一限速数据ID*/
#define EMAP_CONGRADE_ID					28u		/*统一坡度数据ID*/
#define EMAP_PHYSICAL_SGMT_ID				29u		/*物理区段数据ID*/
#define EMAP_BLOC_TRG_AREA_ID				30u		/*点式触发区段数据ID*/
#define EMAP_CBTC_TRG_AREA_ID				31u		/*CBTC触发区段数据ID*/
#define EMAP_PLAT_FORM_ID					32u		/*站台数据ID*/
#define EMAP_COME_GO_ID						33u		/*往复进路表*/
#define EMAP_FLOOD_GATE_ID					34u		/*防淹门---20140718刘天一添加*/
#define EMAP_SPKS_BUTTON_ID					35u		/*SPKS按钮---20141217刘天一添加*/
#define EMAP_DEPOT_GATE_ID					36u		/*车库门---20141217刘天一添加*/
#define EMAP_TUNNEL_ID						37u		/*隧道--20160405侯国建添加*/
#define EMAP_ZC_ID							38u		/*ZC数据ID*/
#define EMAP_MULTI_POINT_ID					39u		 /*多开道岔数据ID add by qxt 20170621*/
#define EMAP_NOP_40							40u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_POWERLESSZONE_ID				41u		/*分相区表ID，依据互联互通数据结构V12.0.0添加，add by lmy 20180312*/
#define EMAP_NOP_42							42u
#define EMAP_NOP_43							43u		
#define EMAP_NOP_44							44u		
#define EMAP_OC_ID							45u		/*OC表ID，依据车车数据结构V1.0添加，add by sds 2019-6-13*/
#define EMAP_ITS_ID							46u		/*ITS表ID，依据车车数据结构V1.0添加，add by sds 2019-6-13*/
#define EMAP_NOP_47							47u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_48							48u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_49							49u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_50							50u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_CARSTOP_ID						51u		/*车档表ID，依据互联互通数据结构V15.0.0添加，add by sds 20181031*/
#define EMAP_NOP_52							52u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_53							53u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_54							54u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_55							55u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_56							56u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_57							57u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_EVACUATION_ID					58u 	/*疏散侧表ID，依据互联互通数据结构V28.0.0.1添加*/
#define EMAP_NOP_59							59u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define EMAP_NOP_60							60u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/

/*任务行为调度宏定义 wyd 20220214*/
#define TASK_TAB_NOP_0						0u		/*为实现数据表与实际表数值一一对应，所以从0开始 补全数据 ID  sds*/
#define TASK_TAB_SWITCH_STRU				1u		/*道岔数据结构*/
#define TASK_TAB_SIGNAL_STRU				2u		/*信号机数据结构*/
#define TASK_TAB_PSD_STRU					3u		/*屏蔽门数据结构*/
#define TASK_TAB_ESB_STRU					4u		/*紧急停车按钮数据结构*/
#define TASK_TAB_STOPSTATION_STRU			5u		/*停车区域数据结构*/
#define TASK_TAB_AXLE_SGMT_STRU				6u		/*计轴区段数据结构*/
#define TASK_TAB_STOPPOINT_STRU				7u		/*停车点数据结构*/
#define TASK_TAB_AR_IN_STRU					8u		/*无人折返进入数据结构*/
#define TASK_TAB_AR_OUT_STRU				9u		/*无人折返退出数据结构*/
#define TASK_TAB_LOGIC_SGMT_STRU			10u		/*逻辑区段数据结构*/
#define TASK_TAB_ROUTE_STRU					11u		/*进路数据结构*/
#define TASK_TAB_PROTECT_LINK_STRU			12u		/*保护区段数据结构*/
#define TASK_TAB_GRADE_STRU					13u		/*坡度数据结构*/
#define TASK_TAB_DEFAULT_ROUTE_STRU			14u		/*默认行车序列数据结构*/
#define TASK_TAB_DEVICE_INFO_IN_LINK		15u		/*Link和其他设备间的映射关系*/
#define TASK_TAB_GRADE_INFO_CROSS_LINK		16u		/*Link和坡度间的映射关系*/
#define TASK_TAB_SPKS_IN_LINK				17u		/*SPKS包含的Link序列*/
#define TASK_TAB_PHYSGMT_STRU				18u		/*物理区段包含的Link序列*/
#define TASK_TAB_NONCBTC_IN_POINT			19u		/*非CBTC区域包含的道岔*/

/*系统配置数据  宏定义*/
#define  CBTC_CONFIGDATA_TYPE				0x20u	/*系统配置数据*/
#define  CBTC_TRAIN_TYPE_CONFIG_ID			1u		/*列车车型配置数据ID*/
#define  CBTC_TRAIN_INFO_ID					2u		/*车型性能数据ID*/
#define  CBTC_CONFIG_DATA_ID				3u		/*各子系统公用数据ID*/
#define  CBTC_CONFIG_DEPOT_ID				4u		/*车辆段配置信息表ID*/

/*协议配置 宏定义*/
#define DSU_DATA_PROTCL_TYPE				0x30u	/*协议表bookId*/
#define DSU_DATA_PROTCL_RSSP_LNK_ID			1u		/*rssp lnk 信息tableId*/
#define DSU_DATA_PROTCL_CI_PLATFORM_ID		2u		/*平台数据结构体tableId*/
#define DSU_DATA_PROTCL_COMM_INFO_ID		3u		/*设备通信属性表tableId*/
#define DSU_DATA_PROTCL_FSFB_LNK_ID			6u		/*FSFB协议链路信息表tableId*/

/*设备IP配置 宏定义*/
#define DSU_DEVICE_IP_CONFIG_TYPE			0x40u	/*设备ip表bookId*/ 
#define DSU_DATA_IP_IP_ID					1u		/*ip地址表tableId*/
#define DSU_DATA_IP_COMM_PROTCL_ID			2u		/*设备类型与通信协议关系表tableId*/
#define DSU_DATA_IP_CCOV_COMM_ID			3u		/*CCOV通信关系配置表tableId*/
#define DSU_DATA_IP_ZCDSU_COMM_ID			4u		/*ZCDSU通信关系配置表tableId*/
#define DSU_DATA_IP_GATEWAY_ID				5u		/*hxq,设备网关表tableId*/
#define DSU_CI_ATS_DEVICE_IP_ADDR			6u		/*CI_ATS网设备IP地址 add by qxt 20160811 */
#define DSU_NET_PORT						9u		/*端口 add by qxt 20160811 */

/*ATP/ATO公共配置数据 宏定义*/
#define  VOBC_CONFIGDATA_TYPE				0x50u	/*ATP/ATO二级子系统公共配置数据*/
#define  VOBC_RAIL_INFO_ID					1u		/*ATP/ATO公共配置线路数据ID*/
#define  VOBC_TRAIN_INFO_ID					2u		/*ATP/ATO公共配置车辆数据ID*/
#define  VOBC_ACC_TABLE_ID					3u		/*最大牵引加速度ID*/
#define  VOBC_RATIO_ACC_TABLE_ID			4u		/*速度拟合阈值的ID 针对单轨数据结构V3.0.0 add by lmy 20180305*/
#define  VOBC_AWAKEN_CONFIG_DATA			5u		/*唤醒配置数据 针对互联互通数据结构V13.0.0 add by sds 20180625*/
#define  VOBC_IO_TABLE_ID					6u		/*开关量配置输入输出配置表ID add by sds 2019-6-20 车车数据结构V1.0*/

/*TMS公共配置数据 宏定义*/
#define  TMS_CONFIGDATA_TYPE				0x81u
#define  TMS_PORT_INFO_ID					1u		/*端口配置信息表索引*/
#define  TMS_VOBC_SEND_INFO_ID				2u		/*端口对应VOBC->TMS组包信息*/
#define  TMS_VOBC_RECV_INFO_ID				3u		/*端口对应TMS->VOBC组包信息*/

/*TSL配置数据 宏定义*/
#define  VOBC_TSL_CONFIGDATA_TYPE			0xFDu	/*TSL配置数据 类型*/
#define  VOBC_TSL_CONFIGDATA_ID				1u		/*TSL信息表*/

/*ATP配置数据  宏定义*/
#define  VOBC_ATP_CONFIGDATA_TYPE			0x60u	/*ATP配置数据 类型*/
#define  VOBC_ATP_TRAIN_CONFIGDATA_ID		1u		/*ATP车辆参数表ID*/
#define  VOBC_ATP_SECURITY_CONFIGDATA_ID	2u		/*ATP安全防护表ID*/
#define	 VOBC_ATP_KEY_CFG_ID				3u		/*ATP安全防护功能开关配置ID  add by sds 2019-6-20 车车数据结构V1.0*/
#define  VOBC_ATP_MA_CFG_ID					4u		/*ATP产品参数配置表（移动授权计算功能参数配置）add by sds 2019-6-20 车车数据结构V1.0*/
#define  VOBC_ATP_SDU_CFG_ID				5u		/*测速配置数据*/

/*TRDP公共配置数据 宏定义*/
#define TRDP_CONFIGDATA_TYPE				0xF5u	/*TRDP配置数据*/
#define TRDP_CFG_DATAID_PUBLIC				1u		/*TABLE ID: 公共*/
#define TRDP_CFG_DATAID_ATC					2u		/*TABLE ID: ATC*/
#define TRDP_CFG_DATAID_CCU					3u		/*TABLE ID: CCU*/
#define TRDP_CFG_DATAID_ATC2CCU				4u		/*TABLE ID: ATC->CCU*/
#define TRDP_CFG_DATAID_CCU2ATC				5u		/*TABLE ID: CCU->ATC*/
#define TRDP_CFG_DATAID_BCU					6u		/*Table ID: BCU*/
#define TRDP_CFG_DATAID_ATC2BCU				7u		/*TABLE ID: ATC->BCU*/
#define TRDP_CFG_DATAID_BCU2ATC				8u		/*TABLE ID: BCU->ATC*/

/*数据ID宏定义结束*/

/*可变应答器版本号 宏定义*/
#define DSU_BALISE_VER_DATA_TYPE			0x80u
#define DSU_BALISE_VER_DATA_ID				1		/*可变应答器版本ID*/

/*ATO配置数据  宏定义*/
#define  VOBC_ATO_CONFIGDATA_TYPE			0x90u	/*ATO配置数据 类型*/
#define  VOBC_ATO_APP_CONFIGDATA_ID         1u		/*ATO软件功能配置参数*/
#define  VOBC_ATO_TRACTIONSAMPLE_ID         2u		/*车辆牵引性能拟合曲线表*/
#define  VOBC_ATO_BREAKSAMPLE_ID            3u		/*车辆制动性能拟合曲线表*/
#define  VOBC_ATO_ESPECIALTRIPNUM_ID        4u		/*特殊车次号段*/
#define  VOBC_ATO_SPEED_CONFIGDATA_ID       5u		/*ATO速度参数表*/
#define  VOBC_ATO_FUNC_CONFIGDATA_ID        6u		/*ATO功能开关配置表--add by yt 2020年11月17日*/
#define  VOBC_ATO_PRODUCT_CONFIGDATA_ID     7u		/*ATO产品参数配置表--add by yt 2020年11月17日*/
/*ZC子系统配置数据*/
#define  ZC_CONFIGDATA_TYPE					0xA0u
#define  ZC_CONFIGDATA_ID					1u		/*ZC公用配置数据*/
#define  ZC_TESTSEGMENT_ID					2u		/*动态测试区段关系表*/
#define  ZC_LOGIC_SEGMET_LOCAL				3u		/*逻辑区段位置关系表*/
#define  ZC_FunctionConfig					4u		/*功能配置表，依据互联互通数据结构V15.0.0添加，add by sds 20181031*/
#define  ZC_ESBLINKAGESTATION_ID			5u		/*ESB联动车站编号表，依据互联互通数据结构V12.0.0添加，add by lmy 20180319*/

/*DSU子系统配置数据*/
#define  DSU_CONFIGDATA_TYPE				0xC0u        
#define  DSU_CONFIGDATA_ID					1u		/*DSU配置数据*/

/*编号对照表*/
#define  IDMAP_DATA_TYPE					0xF1u	/*编号对照表ID*/

/*编号对照表各数据ID*/
#define IDMAP_ZC_ID							1u		/*ZC编号数据ID*/
#define IDMAP_CI_ID							2u		/*TIOC编号数据ID*/
#define IDMAP_ATS_ID						3u		/*ATS编号数据ID*/
#define IDMAP_SIGNAL_ID						4u		/*信号机编号数据ID*/
#define IDMAP_TRACKSEC_ID					5u		/*轨道区段编号数据ID*/
#define IDMAP_SWITCH_ID						6u		/*道岔编号数据ID*/
#define IDMAP_STATION_ID					7u		/*站台编号数据ID*/
#define IDMAP_EMERGBTN_ID					8u		/*紧急关闭按钮编号数据ID*/
#define IDMAP_BALISE_ID						9u		/*应答器编号数据ID*/
#define IDMAP_PSD_ID						10u		/*屏蔽门编号数据ID*/
#define IDMAP_VOBC_ID						11u		/*VOBC编号数据ID*/
#define IDMAP_FPD_ID						12u		/*屏蔽门编号数据ID*/
#define IDMAP_DSU_ID						13u		/*DSU编号数据ID*/
#define IDMAP_LOGICSEC_ID					14u		/*逻辑区段编号数据ID  add by qxt 20160811*/
#define IDMAP_AOMSEC_ID						15u		/*AOM编号数据ID  add by sds 2018-6-14*/
#define IDMAP_SPKSSEC_ID					16u		/*SPKS编号数据ID  add by sds 2018-6-14*/
#define IDMAP_GODSEC_ID						17u		/*车库门编号数据ID  add by sds 2018-6-14*/
#define IDMAP_STASEC_ID						18u		/*车站编号数据ID  add by slm 2018-6-14*/
#define IDMAP_PHYSEC_ID						19u		/*物理区段编号数据ID  add by slm 2018-6-14*/
#define IDMAP_TSL_ID						20u		/*TSL编号数据ID*/

/*VOBC重叠区配置数据  互联互通需求新增， add by qxt 20160811*/
#define  VOBC_OVERLAP_CONFIGDATA_TYPE		0xF3u	/*VOBC重叠区配置数据表ID*/

/*VOBC重叠区配置数据表各数据ID*/
#define VOBC_OVERLAP_GROUNDDEV_ID			1u		/*车载所属及接管的地面设备信息ID*/
#define VOBC_OVERLAP_LINECONNECT_ID			2u		/*线路连接关系ID*/

/*NDSU子系统配置数据*/
#define NDSU_CONFIGDATA_TYPE				0xF4u
#define NDSU_CONFIGDATA_ID					1u		/*NDSU配置数据*/

#endif
