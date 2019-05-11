#ifndef EYETRACKER_H
#define EYETRACKER_H

#include "EyeTracker/Analyser.h"
#include "EyeTracker/Camera.h"
#include "FileManager.h"
#include "Logger.h"
#include "Settings.h"
#include "EyeTracker/Calibration.h"
#include "EyeTracker/bioidtest.h"

#include <QThread>
#include <QTimer>
#include <QObject>

class EyeTracker : public QObject
{
    Q_OBJECT
public:
    EyeTracker(Settings *settings);
    EyeTracker(Settings *settings, int wighth, int height);
    ~EyeTracker();
    bool cameraIsOpened();
    bool CameraInitialisation();

    //вовзращяет статус StatusCalibration
    int startCalibration();

    //отановка работы камеры и анализа
    void fullStop();

    Point GetCurrentPoint();
    bool GetCaptureIsWorking();
    void SetCaptureIsWorking(bool value);
    void StopBlinkDetector();

public:
    static bool isFullStop;
    static Camera *camera;

    //переменные размеров основного окна
    static int mainWindow_width;
    static int mainWindow_height;

    bool fllagEmersionRoundCalib;
    bool flagStopEmersionRoundCalibSlot;

    //класс типа калибровки - "Появление шарика"
    CalibrationEmersion* calibrationEmersion;
    CalibrationMotion calibrationMotion;
    BioIDTest bioIDTest;
    Analyser *analyser;

private:
    enum StatusCalibration { Success, Failed, NotCorrect, None };
    static unsigned int StatusCalibration;

    Settings *settings;
    //управление записью видео, записью/загрузкой калибровки
    FileManager *fileManager;

    bool captureIsWorking;
    bool methodIsInit;

    //калибровка была пройдена
    static bool calibrationDone;
    bool blinksetIsWorking;
    //зрачки найдены впервые
    static bool firstFoundPupils;
    static bool firstFrame;

    //текущий кадр
    Mat frame;

    //выводим разницу между кадрами для детектирования движения
    Mat previousFrame;

    // задержка в цикле
    int delay;

    QTimer *timer;//таймер
    QThread *thread;//поток

    Point currentPoint;

    //рассчет fps
    QTime time;
    int framesCount;

private:
    bool init();
    // инициализация камеры, загрузка видео
    bool captureInit(int inputDevice);
    bool captureInit(QString fileName = QString(""));
    //переворот кадра
    void flipImage(Mat& frame);
    //основные методы анализа и показа изображения (одного фрейма)
    //analyseimage - в классе analyser
    void showImage(Mat& frame, Rect& faceRect, vector<Rect>& eyes, int i);
    //отображение зрачков
    void showPupils(Mat face, vector<Rect> eyes, Point leftPupil, Point rightPupil);
    bool getNextFrame(Mat &frame);
    void clearVariables();

private slots:
    //обработчик срабатывания таймера
    void timeoutHandler();
    //старт цикла событий потока
    void eventLoop();

public slots:
    //начать обработывать кадры и находить current Point
    void start();
    void initTimer();
    //запуск теста на базе BioID
    void testBioIDSLot();

    void startEyeBlinkDetectorSlot();

signals:
    void signalCheckBadCalibrovka(QVariant var);
};

#endif // EYETRACKER_H
