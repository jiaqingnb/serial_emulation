#ifndef UDP_MODULE_H
#define UDP_MODULE_H

#include <QObject>
#include <QUdpSocket>

typedef struct
{
    QHostAddress src_ip;
    uint16_t     src_port;
    QHostAddress des_ip;
    uint16_t     des_port;
}s_udpconfig;

class UDP_module : public QObject
{
    Q_OBJECT
public:
    explicit UDP_module(QObject *parent = nullptr);

    void UDP_Init(QString src_ip,uint16_t src_port,QString des_ip,uint16_t des_port);

private:
    s_udpconfig linkcfg;
    QUdpSocket* socket;
    QByteArray* rcvdata = new QByteArray;
signals:

public slots:
    void UDP_Rcv();
};

#endif // UDP_MODULE_H
