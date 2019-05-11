#ifndef CONTROLMANAGER_H
#define CONTROLMANAGER_H

//
// класс отвечает за управление кнопками
// и другим функционалом, с которым работает пользователь
//
//

#include <QVariant>
#include <QObject>

//TODO:
#include "opencv2/core/core.hpp"

#include <ctime>

#include "Logger.h"
#include "Settings.h"
#include "func.h"
#include "EyeTracker/kalmanfilters.h"

using namespace cv;
using namespace std;
using namespace logger;
class ControlManager : public QObject
{
    Q_OBJECT

public:
    explicit ControlManager();
    void setMainWindowSize(int mainWindow_width, int mainWindow_height);
    void startCoordHandler();
    void handleCoords(Point x);
    void CalculateWidthImages();
    void setSettings(Settings*);
    void clearVariables();

private:
    //переменные размеров основного окна
    static int mainWindow_width;
    static int mainWindow_height;
    int widthImage, mainWindowWidthHalf, mainWindowHeightHalf;
    int timeStart;
    //время задержки взгляда в миллисекундах
    int msec = 3200;
    Settings* settings;

    vector<Point> vectorCoordsForLast2Sec;
    vector<int> vectorMenuBtnsForLast2Sec;
    void updateVectorMenuBtnsForLast2Sec();
    int getBTNnumber(Point point);

signals:
    void pressButtonSignal(QVariant var);
    // сигнал - рисуем проекцию на мониторе
    void drawPointOnMonitorSignal(QVariant x, QVariant y);

public slots:
};

#endif // CONTROLMANAGER_H
