#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setApplicationName(APP_NAME);
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setOrganizationName(APP_COMPANY);
    QApplication::setOrganizationDomain(QStringLiteral("https://github.com/lukinalexeyg/WinDeployQtShell"));

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
