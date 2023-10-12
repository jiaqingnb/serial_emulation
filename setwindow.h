#ifndef SETWINDOW_H
#define SETWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QAbstractButton>
#include <QPushButton>
#include "serial_module/serial_module.h"


namespace Ui {
class setwindow;
}

class setwindow : public QDialog
{
    Q_OBJECT

public:
    explicit setwindow(QWidget *parent = nullptr);
    ~setwindow();

    //QStringList name;
    serial_module ser;

private slots:
    void on_OKbuttonBox_clicked(QAbstractButton *button);

    void on_flushButton_clicked();

private:
    Ui::setwindow *ui;
};

#endif // SETWINDOW_H
