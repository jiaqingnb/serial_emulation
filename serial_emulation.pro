#-------------------------------------------------
#
# Project created by QtCreator 2023-10-11T10:31:20
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serial_emulation
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

INCLUDEPATH += \
                $$PWD/CommonLib/COMMON \
                $$PWD/CommonLib \
                $$PWD/CM_DQU/dqu_cbtc_common \
                $$PWD/CM_DQU/dqu_dfs_common \
                $$PWD/CM_DQU/dqu_emap_common \
                $$PWD/CM_DQU/dqu_idmap_common \
                $$PWD/NET_PRTCL/adaptive/common_dqu_protocol \
                $$PWD/NET_PRTCL/adaptive/common_protocol \
                $$PWD/NET_PRTCL/adaptive/common_protocol_sfp_rp_rsspi_config \
                $$PWD/NET_PRTCL/rp/common_dqu_rp \
                $$PWD/NET_PRTCL/rp/common_redun \
                $$PWD/NET_PRTCL/rp/common_redun_record_config \
                $$PWD/NET_PRTCL/rsspi/common_dqu_rssp \
                $$PWD/NET_PRTCL/rsspi/common_rssp \
                $$PWD/NET_PRTCL/rsspi/common_rssp_record_svc_config \
                $$PWD/NET_PRTCL/sfp/common_dqu_sfp \
                $$PWD/NET_PRTCL/sfp/common_sfp \
                $$PWD/NET_PRTCL/sfp/common_sfp_record_config \
                $$PWD/CM_SFP_CFG \
                $$PWD/MSCP_COMMON \

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    serial_module/serial_module.cpp \
    shortcut_menu/shortcut_menu.cpp \
    setwindow.cpp \
    protocol_module/protocol_module.cpp \
    CommonLib/COMMON/CbtcVersion.c \
    CommonLib/COMMON/CommonBit.c \
    CommonLib/COMMON/CommonCRC.c \
    CommonLib/COMMON/CommonMemory.c \
    CommonLib/COMMON/CommonQueue.c \
    CommonLib/COMMON/CommonRecord.c \
    CommonLib/COMMON/CommonStack.c \
    CommonLib/COMMON/Convert.c \
    CommonLib/COMMON/CRC16.c \
    CommonLib/COMMON/CRC32.c \
    CommonLib/COMMON/DataPackage.c \
    CommonLib/COMMON/FFFE.c \
    CommonLib/COMMON/Translate0203.c \
    CommonLib/COMMON/VobcNetPack.c \
    CM_DQU/dqu_cbtc_common/dquCbtcConfigData.c \
    CM_DQU/dqu_dfs_common/dfsDataRead.c \
    CM_DQU/dqu_dfs_common/dquCbtcType.c \
    CM_DQU/dqu_emap_common/dquEmapInit.c \
    CM_DQU/dqu_emap_common/dquQuery.c \
    CM_DQU/dqu_emap_common/dquQueryExp.c \
    CM_DQU/dqu_emap_common/dquStructInit.c \
    CM_DQU/dqu_emap_common/dsuVar.c \
    CM_DQU/dqu_idmap_common/dquIdmapInit.c \
    CM_DQU/dqu_idmap_common/dquIdmapQuery.c \
    CM_DQU/dqu_idmap_common/dsuIdmapVar.c \
    NET_PRTCL/adaptive/common_dqu_protocol/dsuProtclFun.c \
    NET_PRTCL/adaptive/common_protocol/protclInterFun.c \
    NET_PRTCL/adaptive/common_protocol/ProtclManage.c \
    NET_PRTCL/rp/common_dqu_rp/dsuRpFunc.c \
    NET_PRTCL/rp/common_redun/RpInterface.c \
    NET_PRTCL/rsspi/common_dqu_rssp/dsuRsspFunc.c \
    NET_PRTCL/rsspi/common_rssp/RsspCommonWl.c \
    NET_PRTCL/rsspi/common_rssp/RsspInitWl.c \
    NET_PRTCL/rsspi/common_rssp/RsspLnkNodeMgrWl.c \
    NET_PRTCL/rsspi/common_rssp/RsspOutputWl.c \
    NET_PRTCL/rsspi/common_rssp/RsspReciveWl.c \
    NET_PRTCL/rsspi/common_rssp/RsspTmpMsgLnkCommonWl.c \
    NET_PRTCL/rsspi/common_rssp/RsspWl.c \
    NET_PRTCL/sfp/common_dqu_sfp/dsuRelatedDeal.c \
    NET_PRTCL/sfp/common_sfp/sfpLinkManage.c \
    NET_PRTCL/sfp/common_sfp/sfpPrtcl_Output.c \
    NET_PRTCL/sfp/common_sfp/sfpPrtclCommon.c \
    NET_PRTCL/sfp/common_sfp/sfpPrtclInit.c \
    NET_PRTCL/sfp/common_sfp/sfpPrtclReceive.c \
    NET_PRTCL/sfp/common_sfp/sfpWindowsTimeCounter.c \
    CM_SFP_CFG/sfpInnetDataVar.c \
    MSCP_COMMON/CM_CbtcVersion.c \
    MSCP_COMMON/CM_CommonCRC.c \
    MSCP_COMMON/CM_CommonRecord.c \
    MSCP_COMMON/CM_CommonStack.c \
    MSCP_COMMON/CM_Convert.c \
    MSCP_COMMON/CM_CRC32.c \
    MSCP_COMMON/CM_FFFE.c \
    MSCP_COMMON/CM_Memory.c \
    MSCP_COMMON/CM_Queue.c \
    MSCP_COMMON/CM_Ring.c \
    MSCP_COMMON/CM_VobcNetPack.c

HEADERS += \
        mainwindow.h \
    serial_module/serial_module.h \
    shortcut_menu/shortcut_menu.h \
    setwindow.h \
    protocol_module/protocol_module.h \
    CommonLib/COMMON/CbtcVersion.h \
    CommonLib/COMMON/CommonBit.h \
    CommonLib/COMMON/CommonCRC.h \
    CommonLib/COMMON/CommonMemory.h \
    CommonLib/COMMON/CommonQueue.h \
    CommonLib/COMMON/CommonRecord.h \
    CommonLib/COMMON/CommonStack.h \
    CommonLib/COMMON/CommonTypes.h \
    CommonLib/COMMON/Convert.h \
    CommonLib/COMMON/CRC16.h \
    CommonLib/COMMON/CRC32.h \
    CommonLib/COMMON/DataPackage.h \
    CommonLib/COMMON/FFFE.h \
    CommonLib/COMMON/Translate0203.h \
    CommonLib/COMMON/VobcNetPack.h \
    CommonLib/CommonConfig.h \
    CM_DQU/dqu_cbtc_common/dquCbtcConfigData.h \
    CM_DQU/dqu_dfs_common/dfsDataRead.h \
    CM_DQU/dqu_dfs_common/dquCbtcType.h \
    CM_DQU/dqu_dfs_common/dquDataTypeDefine.h \
    CM_DQU/dqu_emap_common/dquEmapInit.h \
    CM_DQU/dqu_emap_common/dquQuery.h \
    CM_DQU/dqu_emap_common/dquQueryExp.h \
    CM_DQU/dqu_emap_common/dquStructInit.h \
    CM_DQU/dqu_emap_common/dquVobcCommonStru.h \
    CM_DQU/dqu_emap_common/dsuEmapDefine.h \
    CM_DQU/dqu_emap_common/dsuStruct.h \
    CM_DQU/dqu_emap_common/dsuVar.h \
    CM_DQU/dqu_idmap_common/dquIdmapInit.h \
    CM_DQU/dqu_idmap_common/dquIdmapQuery.h \
    CM_DQU/dqu_idmap_common/dsuIdmapStruct.h \
    CM_DQU/dqu_idmap_common/dsuIdmapVar.h \
    NET_PRTCL/adaptive/common_dqu_protocol/dsuProtclFun.h \
    NET_PRTCL/adaptive/common_protocol/protclInterFun.h \
    NET_PRTCL/adaptive/common_protocol/ProtclManage.h \
    NET_PRTCL/adaptive/common_protocol/protclMgeCfg.h \
    NET_PRTCL/adaptive/common_protocol/protclStruct.h \
    NET_PRTCL/adaptive/common_protocol_sfp_rp_rsspi_config/protocolConfig.h \
    NET_PRTCL/rp/common_dqu_rp/dsuRpFunc.h \
    NET_PRTCL/rp/common_redun/RpDeclare.h \
    NET_PRTCL/rp/common_redun/RpInterface.h \
    NET_PRTCL/rp/common_redun/RpPlatform.h \
    NET_PRTCL/rp/common_redun/RpStruct.h \
    NET_PRTCL/rp/common_redun_record_config/RpConfig.h \
    NET_PRTCL/rsspi/common_dqu_rssp/dsuRsspDataStru.h \
    NET_PRTCL/rsspi/common_dqu_rssp/dsuRsspFunc.h \
    NET_PRTCL/rsspi/common_rssp/RsspCommonWl.h \
    NET_PRTCL/rsspi/common_rssp/RsspIFWl.h \
    NET_PRTCL/rsspi/common_rssp/RsspLnkNodeMgrWl.h \
    NET_PRTCL/rsspi/common_rssp/RsspRecordDelareWl.h \
    NET_PRTCL/rsspi/common_rssp/RsspStructWl.h \
    NET_PRTCL/rsspi/common_rssp/RsspTmpMsgLnkCommonWl.h \
    NET_PRTCL/rsspi/common_rssp/RsspWl.h \
    NET_PRTCL/rsspi/common_rssp_record_svc_config/RsspConfigWl.h \
    NET_PRTCL/sfp/common_dqu_sfp/dsuRelatedDeal.h \
    NET_PRTCL/sfp/common_sfp/sfpDeclare.h \
    NET_PRTCL/sfp/common_sfp/sfpLinkManage.h \
    NET_PRTCL/sfp/common_sfp/sfpplatform.h \
    NET_PRTCL/sfp/common_sfp/sfpPrtcl_Output.h \
    NET_PRTCL/sfp/common_sfp/sfpPrtclCommon.h \
    NET_PRTCL/sfp/common_sfp/sfpPrtclReceive.h \
    NET_PRTCL/sfp/common_sfp/sfpPrtclStruct.h \
    NET_PRTCL/sfp/common_sfp/sfpWindowsTimeCounter.h \
    NET_PRTCL/sfp/common_sfp_record_config/sfpConfig.h \
    CM_SFP_CFG/sfpInnetDataVar.h \
    PROJECT_OR_BOARD_PTCL.h \
    MSCP_COMMON/CM_Assert.h \
    MSCP_COMMON/CM_CbtcVersion.h \
    MSCP_COMMON/CM_CommonCRC.h \
    MSCP_COMMON/CM_CommonRecord.h \
    MSCP_COMMON/CM_CommonStack.h \
    MSCP_COMMON/CM_Convert.h \
    MSCP_COMMON/CM_CRC32.h \
    MSCP_COMMON/CM_FFFE.h \
    MSCP_COMMON/CM_Memory.h \
    MSCP_COMMON/CM_Queue.h \
    MSCP_COMMON/CM_Ring.h \
    MSCP_COMMON/CM_Types.h \
    MSCP_COMMON/CM_VobcNetPack.h

FORMS += \
        mainwindow.ui \
    setwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

DISTFILES += \
    NET_PRTCL/sfp/common_sfp/.vs/common_sfp/v17/.suo \
    NET_PRTCL/sfp/common_sfp/.vs/slnx.sqlite \
    NET_PRTCL/sfp/common_sfp/协议c和cpp转换.bat
