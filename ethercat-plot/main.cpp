#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("plot-test");//设置窗口名称
    //w.setWindowFlags(Qt::FramelessWindowHint); //也是去掉标题栏的语句,设置之后会无法移动
    w.show();

    return a.exec();
}
