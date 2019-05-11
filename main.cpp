#include <QApplication>
#include <QtQml>
#include <QObject>

#include "MainClass.h"
#include "Settings.h"
#include "func.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // регистрация типа Settings для qml
    // в qml необходимо прописать
    // import Settings 1.0
    // и создавать объект(?) класса:
    //    Settings {
    //               id: settings
    //            }
    qmlRegisterType<Settings>("Settings", 1, 0, "Settings");

    MainClass *mainClass = new MainClass();
    mainClass->initBaseLogic();
    mainClass->registrSignalSlots();
    mainClass->startBaseLogic();

    QObject::connect(&app, SIGNAL(aboutToQuit()), mainClass, SLOT(aboutToQuit()));

    return app.exec();
}
