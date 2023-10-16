#ifndef SHORTCUT_MENU_H
#define SHORTCUT_MENU_H

#include <QMainWindow>
#include <QMenu>
#include <qaction.h>
#include "setwindow.h"

typedef struct
{
    const char *name;
    UINT8 index;
}menu_name;

class shortcut_menu : public QMainWindow
{
    Q_OBJECT
public:
    shortcut_menu();
    ~shortcut_menu();

public:

    setwindow* s = new setwindow;
    //函数1：创建菜单
    void Creat_menu(const QPoint &pos);

private:


public slots:
    void on_menu_clicked();
};

#endif // SHORTCUT_MENU_H
