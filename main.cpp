#include "albumrepository.h"
#include "mainwindow.h"

#include "sqlite34/sqlite3.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CDBaseV1_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();
    w.resize(1200, 600);
    return a.exec();
}
