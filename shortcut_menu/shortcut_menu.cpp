#include "shortcut_menu.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

shortcut_menu::shortcut_menu()
{

}
void shortcut_menu::on_menu_clicked()
{

    s.exec();
    qDebug()<<"odsfdasffafdafdasfsk";

}

void shortcut_menu::Creat_menu(const QPoint &pos)
{
   Q_UNUSED(pos);
   QMenu* menuList = new QMenu;

   qDebug()<<"159";
   menuList->addAction("设置",this,&shortcut_menu::on_menu_clicked);


   menuList->exec(QCursor::pos());
   delete menuList;

}
