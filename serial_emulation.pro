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
                $$PWD/CommonLib

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
    CommonLib/COMMON/VobcNetPack.c

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

FORMS += \
        mainwindow.ui \
    setwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
