#ifndef SERIAL_MODULE_H
#define SERIAL_MODULE_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "CommonTypes.h"

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
    /*变量区*/
    QSerialPort *serial = new QSerialPort;
    QStringList name;
    UINT8 ser_senddata[1024] = {'\0'};

    /*函数区*/
    QStringList search_serial();

    void serial_init(s_uart uart,QString& name);

    UINT8 StringToU8(QString input);//一个将字符串转换为U8数组的指针函数

private:
    UINT32 search_uartmapping(QString input);


};

#endif // SERIAL_MODULE_H
