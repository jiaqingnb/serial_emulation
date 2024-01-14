#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shortcut_menu/shortcut_menu.h"
#include "thread/my_thread.h"


#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    udp = new UDP_module;
    my_thread* b = new my_thread;
    b->start();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    QPalette pal;

    /*打开串口*/
    if(menu->s->ser->serial->open(QIODevice::ReadWrite))
    {
        ui->label_8->setStyleSheet("background-color:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0 rgba(0,238,0,255),stop:1 rgba(255,255,255,255));border-radius:12px;");
        qDebug()<<"串口打开成功";
    }
    else
    {
        qDebug()<<"串口打开失败";
    }
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{

   qDebug()<<"右键菜单";
   QTreeWidgetItem* choiceitem;

   choiceitem = ui->treeWidget->currentItem();
   ui->treeWidget->columnCount();

   menu->Creat_menu(pos);
   choiceitem->setText(0,menu->s->treename);


}
//SFP协议的UDP发送按钮
void MainWindow::on_pushButton_4_clicked()
{
  QString scr_ip = ui->Initiator_IP->text();
  uint16_t scr_port = static_cast<uint16_t>(ui->Initiator_Port->text().toUInt());
  QString des_ip = ui->Follower_IP->text();
  uint16_t des_port = static_cast<uint16_t>(ui->Follower_Port->text().toUInt());

  qDebug()<<scr_ip<<scr_port;
  udp->UDP_Init(scr_ip,scr_port,des_ip,des_port);
}
