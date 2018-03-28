#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("live-share");
    QCoreApplication::setApplicationName("client");
    QCoreApplication::setApplicationVersion("0.1.0");

    MainWindow w;
    w.show();

    return a.exec();
}
