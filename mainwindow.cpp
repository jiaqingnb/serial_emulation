#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shortcut_menu/shortcut_menu.h"

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

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
        pal.setColor(QPalette::Background,Qt::green);
        ui->label_8->setAutoFillBackground(true);
        ui->label_8->setPalette(pal);
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
