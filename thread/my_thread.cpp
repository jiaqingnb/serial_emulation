#include "my_thread.h"
#include <QDebug>


my_thread::my_thread()
{

}

void my_thread::run()
{

    QTimer* p_time = new QTimer;

    p_time->setInterval(200);
    connect(p_time,SIGNAL(timeout()),this,SLOT(timeclock()));
    p_time->start();
    this->exec();
}


void my_thread::timeclock()
{
     m_worktime++;
}
