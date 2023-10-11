#include "setwindow.h"
#include "ui_setwindow.h"

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
