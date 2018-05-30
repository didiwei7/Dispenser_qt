#include "mainwindow/mainwindow.h"
#include <QApplication>
#include <time.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qsrand(clock());
    QApplication::setStyle(new FlexStyle());

    MainWindow w;
    w.show();

    return a.exec();
}
