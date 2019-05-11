#include "MainClass.h"

MainClass::MainClass() : QObject()
{
    settings = new Settings();
    baselogic = new BaseLogic(settings);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    rootObject = engine.rootObjects().first();
}

MainClass::~MainClass()
{
    baselogicThread.quit();
    baselogicThread.wait();
}

void MainClass::aboutToQuit()
{
    qDebug("aboutToQuit()");
    baselogic->eyetracker->camera->release();
}

void MainClass::startBaseLogic()
{
    // http://habrahabr.ru/post/150274/
    baselogic->moveToThread(&baselogicThread);

    connect(baselogic, SIGNAL(finished()), &baselogicThread, SLOT(quit()));
    connect(baselogic, SIGNAL(finished()), baselogic, SLOT(deleteLater()));
    connect(&baselogicThread, SIGNAL(finished()), &baselogicThread, SLOT(deleteLater()));

    //запуск потока
    baselogicThread.start();

    return;
}

void MainClass::initBaseLogic()
{
    baselogic->eyetracker->mainWindow_height = rootObject->property("height").toInt();
    baselogic->eyetracker->mainWindow_width = rootObject->property("width").toInt();
}
void MainClass::registrSignalSlots()
{
    //получаем доступ к mainWindow
    //http://stackoverflow.com/questions/23177839/how-can-i-access-my-window-object-properties-from-c-while-using-qqmlapplicatio

    //ОБЩЕЕ для всех калибровок
    //КОРРЕКТНЫЕ СИГНЛ-СЛОТЫ запуска КАЛИБРОВКИ
    //1. запуск инициализации
    //2. по завершении инициализации - сигнал "камера проинициализирована"
    //3. обратный отсчет, по завершении - сигнал "начало калибровки"
    //4. конец калибровки

    //сигналы и слоты - инициализация камеры
    QObject::connect(rootObject, SIGNAL(startCameraInitialisationSignal()),
                     baselogic, SLOT(startCameraInitialisationSlot()));
    //камера проинициализирована
    QObject::connect(baselogic, SIGNAL(cameraInitialisationFinishedSignal(QVariant)),
                     rootObject, SLOT(cameraInitialisationFinishedSlot(QVariant)));

    //начало калибровки
    QObject::connect(rootObject, SIGNAL(startCalibrationSignal()),
                     baselogic, SLOT(startCalibrationSlot()));
    //конец калибровки
    QObject::connect(rootObject, SIGNAL(stopCalibrationSignal()),
                     baselogic, SLOT(stopCalibrationSlot()));


    ///
    /// 1 тип калибровки - движение маркера
    /// сигнал - изменение направления движения маркера
    ///
    QObject::connect(rootObject, SIGNAL(directoryChangedSignal(int)),
                     &baselogic->eyetracker->calibrationMotion, SLOT(directoryChangedSlot(int)));


    ///
    ///2 тип калибровки- появление маркера. Число позиций шарика можно указать в qml
    ///начало записи и анализа - для типа калибровки "Появление шарика"
    ///анализ идет во время калибровки и по окончании
    ///во время калибровки, записывается массивы распознанных зрачков для каждой позиции шарика и координаты шарика на экране
    ///после окончании калибровки, массивы обрабатываются - находятся усредненное значение зрачка для каждой позиции
    ///получаем в результате матрицу: массив(СреднийЗрачек, КоординатаЭкрана)[,]
    ///
    QObject::connect(rootObject, SIGNAL(flagEmersionRoundCalibSignal()),
                     baselogic, SLOT(fllagEmersionRoundCalibSlot()));

    QObject::connect(rootObject, SIGNAL(flagStopEmersionRoundCalibSignal()),
                     baselogic, SLOT(fllagStopEmersionRoundCalibSlot()));

    QObject::connect(rootObject, SIGNAL(getMonitorCoordCalibSignal(int, int)),
                     baselogic, SLOT(getMonitorCoordCalibSlot(int, int)));


    QObject::connect(baselogic->eyetracker, SIGNAL(signalCheckBadCalibrovka(QVariant)),
                     rootObject, SLOT(slotCheckBadCalibrovka(QVariant)));

    //сигнал в GUI о том, что кнопка нажата.
    QObject::connect(&baselogic->controlManager, SIGNAL(pressButtonSignal(QVariant)),
                     rootObject, SLOT(pressButtonSlot(QVariant)));

    // изменение ширины и высоты окна
    QObject::connect(rootObject, SIGNAL(widthChngSignal(int)),
                     baselogic, SLOT(mainWindowWidthChangedSlot(int)));
    QObject::connect(rootObject, SIGNAL(heightChngSignal(int)),
                     baselogic, SLOT(mainWindowHeightChangedSlot(int)));

    //отановка работы камеры и анализа
    QObject::connect(rootObject, SIGNAL(fullStopSignal()),
                     baselogic, SLOT(fullStop()));

    //изменена выбранная камера
    QObject::connect(rootObject, SIGNAL(selectedCameraChangedSignal(QString)),
                     baselogic, SLOT(selectedCameraChangedSlot(QString)));

    //рисуем проекцию на мониторе
    QObject::connect(&baselogic->controlManager, SIGNAL(drawPointOnMonitorSignal(QVariant, QVariant)),
                     rootObject, SLOT(drawPointOnMonitorSlot(QVariant, QVariant)));

    //для тестов: запуск WorkProcess без калибровки сразу из гуи при нажатии кнопки
    QObject::connect(rootObject, SIGNAL(workProcessSignal()),
                     baselogic, SLOT(workProcessSlot()));

    //для тестов: запуск теста на базе BioID
    QObject::connect(rootObject, SIGNAL(testBioIDSignal()),
                     baselogic->eyetracker, SLOT(testBioIDSLot()));

    QObject::connect(&baselogic->eyetracker->bioIDTest, SIGNAL(progressChangedTestBioIDSignal(QVariant)),
                     rootObject, SLOT(progressChangedTestBioIDSlot(QVariant)));

    //для настройки детектора моргания
    QObject::connect(rootObject, SIGNAL(startEyeBlinkDetectorSignal()),
                     baselogic->eyetracker, SLOT(startEyeBlinkDetectorSlot()));
    QObject::connect(rootObject, SIGNAL(stopEyeBlinkDetectorSignal()),
                     baselogic, SLOT(stopEyeBlinkDetectorSlot()));

    //главный объект
    //root object in qml должен иметь objectName: "mainWindow"
    //QObject *qmlObject = rootObject->findChild<QObject*>("mainWindow");
    //rootObject->setProperty("visible", true);
    //qDebug() << rootObject->property("visible");

    //объект rectangle1
    QObject *rectObj = rootObject->findChild<QObject*>("rectangle1");
    if (rectObj)
        rectObj->setProperty("color", "#000000" /*"red"*/);

}
