#ifndef SETWINDOW_H
#define SETWINDOW_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class setwindow;
}

class setwindow : public QDialog
{
    Q_OBJECT

public:
    explicit setwindow(QWidget *parent = nullptr);
    ~setwindow();

private:
    Ui::setwindow *ui;
};

#endif // SETWINDOW_H
