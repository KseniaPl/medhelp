#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include <BaseLogic.h>
#include <Logger.h>
#include <QQmlApplicationEngine>
#include <QCoreApplication>

#include <func.h>
#include <Settings.h>

class MainClass: public QObject
{
    Q_OBJECT
public:
    BaseLogic *baselogic;
    MainClass();
    ~MainClass();
    void initBaseLogic();
    void startBaseLogic();
    void registrSignalSlots();

private:
    QQmlApplicationEngine engine;
    QObject *rootObject;
    QThread baselogicThread;
    Settings *settings;

public slots:
    void aboutToQuit();

signals:

};

#endif // MAINCLASS_H
