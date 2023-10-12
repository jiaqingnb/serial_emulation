#include "setwindow.h"
#include "ui_setwindow.h"
#include <QDebug>


setwindow::setwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setwindow)
{

    ui->setupUi(this);

}

setwindow::~setwindow()
{
    delete ui;
}

void setwindow::on_OKbuttonBox_clicked(QAbstractButton *button)
{
    if(ui->OKbuttonBox->button(QDialogButtonBox::Ok) == button)
    {

        /*串口进行初始化*/       
        s_uart uart;
        INT32 index;

        uart.baud = ui->com1_baudBox->itemText(ui->com1_baudBox->currentIndex());
        uart.data = ui->com1_dataBox->itemText(ui->com1_dataBox->currentIndex());
        uart.parity = ui->com1_parityBox->itemText(ui->com1_parityBox->currentIndex());
        uart.stop = ui->com1_stopBox->itemText(ui->com1_stopBox->currentIndex());
        index = ui->com1_serialBox->currentIndex();

        qDebug()<<ser.name[index];

        ser.serial_init(uart,ser.name[index]);

        this->close();

    }
    else
    {
        this->close();
    }
}

void setwindow::on_flushButton_clicked()
{
    QStringList Itemname;
    /*清空Box项*/
    ui->com1_serialBox->clear();
    ser.name.clear();
    /*识别系统所有串口并显示*/
    Itemname = ser.search_serial();
    ui->com1_serialBox->addItems(Itemname);

}
