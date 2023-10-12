#ifndef SERIAL_MODULE_H
#define SERIAL_MODULE_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "common.h"

typedef struct
{
  QString input;
  UINT32  output;
}s_uartmap;

typedef struct
{
    QString baud;//波特率
    QString data;//数据位
    QString parity;//校验位
    QString stop;//停止位
}s_uart;


class serial_module
{
public:
    serial_module();

public:
    QSerialPort *serial = new QSerialPort;
    QStringList name;

    QStringList search_serial();
    void serial_init(s_uart uart,QString& name);
private:
    UINT32 search_uartmapping(QString input);
//    QStringList baud;//波特率
//    QStringList data;//数据位
//    QStringList parity;//校验位
//    QStringList stop;//停止位
};

#endif // SERIAL_MODULE_H
