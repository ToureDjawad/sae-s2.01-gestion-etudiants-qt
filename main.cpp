#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTranslator translator;
    QString locale = QLocale::system().name();
    if (translator.load(":/i18n/GestionEtudiants_" + locale)) {
        app.installTranslator(&translator);
    }

    MainWindow fenetre;
    fenetre.show();

    return app.exec();
}