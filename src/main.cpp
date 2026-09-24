#include <QApplication>
#include <QCoreApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Winkler");
    QCoreApplication::setApplicationName("Rezeptverwaltung");

    MainWindow window;
    window.show();
    return app.exec();
}
