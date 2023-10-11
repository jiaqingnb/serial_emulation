#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shortcut_menu/shortcut_menu.h"
#include "serial_module/serial_module.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    serial_module ser;
    ui->setupUi(this);
    ui->com1_serialBox->addItems(ser.search_serial());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    /*串口进行初始化*/
    serial_module ser;
    s_uart uart;
    uart.baud = ui->com1_baudBox->itemText(ui->com1_baudBox->currentIndex());
    uart.data = ui->com1_dataBox->itemText(ui->com1_dataBox->currentIndex());
    uart.parity = ui->com1_parityBox->itemText(ui->com1_parityBox->currentIndex());
    uart.stop = ui->com1_stopBox->itemText(ui->com1_stopBox->currentIndex());
    qDebug()<<ui->com1_baudBox->itemText(ui->com1_baudBox->currentIndex());
    ser.serial_init(uart);
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
   shortcut_menu menu;
   qDebug()<<"右键菜单";

   menu.Creat_menu(pos);

}
