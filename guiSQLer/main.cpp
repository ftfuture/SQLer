#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Sqler Application");
    QCoreApplication::setOrganizationName("Sqler Organization");

    MainWindow w;
    w.show();

    return a.exec();
}
