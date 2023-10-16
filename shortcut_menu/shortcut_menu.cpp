#include "shortcut_menu.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

static menu_name cutname[5] = {
    {"设置",0},{"新建",1}
};

shortcut_menu::shortcut_menu()
{

}

shortcut_menu::~shortcut_menu()
{
    delete s;
}

void shortcut_menu::on_menu_clicked()
{
    static uint8_t mjq = 0;

    if(mjq == 1)
    {
       mjq++;
    }


   s->setWindowTitle("设置");
   qDebug()<<"设置窗口开启";
   s->exec();
   mjq++;

}

void shortcut_menu::Creat_menu(const QPoint &pos)
{
   Q_UNUSED(pos);
   QMenu menuList;
   QAction* a = new QAction;
   static uint8_t mjq = 0;

   if(mjq == 1)
   {
      mjq++;
   }

   menuList.addAction("设置",this,&shortcut_menu::on_menu_clicked);
   //menuList->addAction(cutname[1].name,this,&shortcut_menu::on_menu_clicked);

   menuList.exec(QCursor::pos());

mjq++;
}

