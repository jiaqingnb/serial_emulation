#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "shortcut_menu/shortcut_menu.h"
#include "udp_module.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    shortcut_menu* menu = new shortcut_menu;
    UDP_module* udp;

private slots:
    void on_pushButton_3_clicked();

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
