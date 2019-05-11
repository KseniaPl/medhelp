#ifndef BASELOGIC_H
#define BASELOGIC_H

#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include <QtMultimedia>

#include <func.h>
#include "Logger.h"
#include "Settings.h"
#include "ControlManager.h"

#include "EyeTracker/Eyetracker.h"

#include "assert.h"
#include "io.h"

using namespace std;
using namespace cv;
using namespace func;

class BaseLogic : public QObject
{
    Q_OBJECT
protected:
    bool InitBaseLogic();

public:
    BaseLogic(Settings*);
    ~BaseLogic();

    EyeTracker* eyetracker;

    //управление нажатием кнопок и др.функций
    ControlManager controlManager;

private:
    Settings *settings;

    bool getNextFrame(Mat &frame);

    // рабочий процесс анализа поступающего видео, начинающийся после завершения калибровки
    void WorkProcess();
    void OnClose();

public slots:
    void stop();

    //отановка работы камеры и анализа
    void fullStop();

    //слоты изменения переменных размеров основного окна
    void mainWindowWidthChangedSlot(int var);
    void mainWindowHeightChangedSlot(int var);

    //калибровка - общее
    void startCameraInitialisationSlot();
    void startCalibrationSlot();
    void startCalibration();

    void stopCalibrationSlot();

    //слоты для 2 типа калибровки - Появление шарика
    void fllagEmersionRoundCalibSlot();
    void fllagStopEmersionRoundCalibSlot();
    void getMonitorCoordCalibSlot(int x, int y);

	//поиск глаз - новый
    void test();

    //изменена выбранная камера
    void selectedCameraChangedSlot(QString id);

    void stopEyeBlinkDetectorSlot();
	
    //запуск WorkProcess без калибровки сразу из гуи при нажатии кнопки
    void workProcessSlot();

signals:
    //сигнал о завершении  работы
    void finished();

    //калибровка
    void cameraInitialisationFinishedSignal(QVariant var);

    //начать работу айтрекера
    void startEyeTracker();

    //инициализировать таймер в своём потоке
    void initEyeTrackerTimer();

};

#endif //BASELOGIC_H
