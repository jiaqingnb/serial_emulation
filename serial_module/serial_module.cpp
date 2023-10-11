#include "serial_module.h"
#include <QDebug>

static s_uartmap uart_mapping[20] = {
    {"9600",9600},{"19200",19200},{"38400",38400},{"56000",56000},{"115200",115200},{"128000",128000},{"256000",256000},{"512000",512000},
    {"8",8},{"7",7},{"6",6},{"5",5},
    {"0x00",0x00},{"0x01",0x01},{"0x02",0x02}
};

serial_module::serial_module()
{

}

QStringList serial_module::search_serial()
{
    QStringList list;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        qDebug()<<info.portName()<<info.description();

        /*如果串口空闲*/
        if(!info.isBusy())
        {
           list<<info.portName()+info.description();
        }
        else
        {
            qDebug()<<"串口打开失败";
        }
    }
    return list;
}

void serial_module::serial_init(s_uart uart)
{
    static QSerialPort *serial;
    serial = new QSerialPort;
    UINT32 baud;
    UINT32 data;
    UINT32 parity;
    UINT32 stop;

    baud = search_uartmapping(uart.baud);
    serial->setBaudRate(static_cast<INT32>(baud));

    data = search_uartmapping(uart.data);
    serial->setDataBits(static_cast<QSerialPort::DataBits>(data));

    parity = search_uartmapping(uart.parity);
    serial->setParity(static_cast<QSerialPort::Parity>(parity));

    stop = search_uartmapping(uart.stop);
    serial->setStopBits(static_cast<QSerialPort::StopBits>(stop));

    qDebug()<<"baud"<<baud<<"data"<<data<<"parity"<<parity<<"stop"<<stop;
}

UINT32 serial_module::search_uartmapping(QString input)
{
    UINT8 i = 0;
    UINT32 output = 0xffffffff;
    for (i = 0;i<20;i++)
    {
        if(uart_mapping[i].input == input)
        {
            output = uart_mapping[i].output;
            break;
        }
    }
    return output;
}
