#include "setwindow.h"
#include "ui_setwindow.h"
#include <QDebug>


setwindow::setwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setwindow)
{

    ui->setupUi(this);
    on_flushButton_clicked();
}

setwindow::~setwindow()
{
    delete ui;
    delete ser;
    delete pro;
}

void setwindow::on_OKbuttonBox_clicked(QAbstractButton *button)
{
    if(ui->OKbuttonBox->button(QDialogButtonBox::Ok) == button)
    {

        /*串口进行初始化*/       
        s_uart uart;
        s_protocol proto;
        INT32 index;

        uart.baud = ui->com1_baudBox->itemText(ui->com1_baudBox->currentIndex());
        uart.data = ui->com1_dataBox->itemText(ui->com1_dataBox->currentIndex());
        uart.parity = ui->com1_parityBox->itemText(ui->com1_parityBox->currentIndex());
        uart.stop = ui->com1_stopBox->itemText(ui->com1_stopBox->currentIndex());
        index = ui->com1_serialBox->currentIndex();

        proto.device = ui->device_Box_6->itemText(ui->device_Box_6->currentIndex());
        proto.protocol = ui->protocol_Box_8->itemText(ui->protocol_Box_8->currentIndex());
        proto.cycle = ui->cycle_Box_7->itemText(ui->cycle_Box_7->currentIndex());

        pro->protocol_init(proto);

        qDebug()<<ser->name[index];

        treename = ser->name[index];//给树形窗口命名赋值
        ser->serial_init(uart,ser->name[index]);


        /*获取设置文本框的内容*/
        QString str = ui->sendtextEdit->toPlainText();

        ser->StringToU8(str);
        qDebug()<<ser->ser_senddata[0]<<ser->ser_senddata[1];

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
    ser->name.clear();
    /*识别系统所有串口并显示*/
    Itemname = ser->search_serial();
    ui->com1_serialBox->addItems(Itemname);

}
