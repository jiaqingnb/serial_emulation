#ifndef SHORTCUT_MENU_H
#define SHORTCUT_MENU_H

#include <QMainWindow>
#include <QMenu>
#include <qaction.h>
#include "setwindow.h"



class shortcut_menu : public QMainWindow
{
    Q_OBJECT
public:
    shortcut_menu();

public:

    setwindow s;
    //函数1：创建菜单
    void Creat_menu(const QPoint &pos);

private:


public slots:
    void on_menu_clicked();
};

#endif // SHORTCUT_MENU_H
