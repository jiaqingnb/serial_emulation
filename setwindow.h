#ifndef SETWINDOW_H
#define SETWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QAbstractButton>
#include <QPushButton>
#include "serial_module/serial_module.h"
#include "protocol_module/protocol_module.h"



namespace Ui {
class setwindow;
}

class setwindow : public QDialog
{
    Q_OBJECT

public:
    explicit setwindow(QWidget *parent = nullptr);
    ~setwindow();

    QString treename;
    serial_module* ser = new serial_module;
    protocol_module* pro = new protocol_module;

private slots:
    void on_OKbuttonBox_clicked(QAbstractButton *button);

    void on_flushButton_clicked();

private:
    Ui::setwindow *ui;
};

#endif // SETWINDOW_H
