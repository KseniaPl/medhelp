#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QTime>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDateTime>

#include <fstream>

#include "EyeTracker/Camera.h"
#include "Logger.h"
#include "Settings.h"
#include "func.h"

using namespace std;
using namespace cv;
using namespace logger;

class TextFileManager : public QObject
{
    Q_OBJECT
public:
    TextFileManager();
    void setSettings(Settings *settings);

    //сохранение анализа калибровки в режиме реального времени
    bool createNewCalibrationFile(Rect leftEye_calibrationRect, Rect rightEye_calibrationRect);
    bool createNewCalibrationFile(int mainWindow_width, int mainWindow_height);
    bool writeCoordsIntoFile(Point p1, Point p2);
    bool writeCoordsIntoFile(Point monitorPoint);
    bool finishSavingIntoFile();

    //не используется
    void loadCalibration();
    void saveCalibration();
    void saveAnalysisInfoIntoFile();

private:
    ofstream out_stream;
    static string textFileName;
    unsigned int linesCounter;
    Settings *settings;

signals:

public slots:
};

class FileManager : public QObject
{
    Q_OBJECT
public:
    FileManager();
    FileManager(Settings *settings);

    //отвечает за работу с текст.файлами.
    TextFileManager textFileManager;

    bool videoRecordInit(Camera *capture, bool autoCalculateCameraFPS);

    static string VideoFileName;
    static VideoWriter videoWriter;

private:
    Settings *settings;

signals:

public slots:
};




#endif // FILEMANAGER_H
