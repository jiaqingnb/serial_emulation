#include "mainwindow.h"
#include "setwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setWindowTitle("串口仿真工具 V1.0.0");
//    setwindow s;
//    s.show();
//    s.setWindowTitle("设置");

    return a.exec();
}
