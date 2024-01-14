#include "udp_module.h"
#include <QNetworkDatagram>

UDP_module::UDP_module(QObject *parent) : QObject(parent)
{

  socket = new QUdpSocket;
  QString t_ip = "192.168.1.123";



  connect(socket,&QUdpSocket::readyRead,this,&UDP_module::UDP_Rcv);

}

void UDP_module::UDP_Init(QString src_ip,uint16_t src_port,QString des_ip,uint16_t des_port)
{
    linkcfg.src_ip = QHostAddress(src_ip);
    linkcfg.src_port = src_port;
    linkcfg.des_ip = QHostAddress(des_ip);
    linkcfg.des_port = des_port;

    bool ret = socket->bind(linkcfg.src_ip,linkcfg.src_port);
    qDebug()<<"socket创建："<<ret;
}

void UDP_module::UDP_Rcv()
{
    QNetworkDatagram datagram = socket->receiveDatagram();
    *rcvdata = datagram.data();
    qDebug()<<*rcvdata;
    QString send = "0,1,2,3,4,5";
    socket->writeDatagram(send.toUtf8(),linkcfg.des_ip,linkcfg.des_port);

}


