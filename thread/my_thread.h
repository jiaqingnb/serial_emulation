#ifndef MY_THREAD_H
#define MY_THREAD_H

#include <QThread>
#include <QTimer>
#include "CommonTypes.h"
#include "PLFM_CycleTime_Config.h"

extern UINT32 m_worktime;

class my_thread : public QThread
{
    Q_OBJECT
public:
    my_thread();

    void run();

private:


public slots:
    void timeclock();
};

#endif // MY_THREAD_H
