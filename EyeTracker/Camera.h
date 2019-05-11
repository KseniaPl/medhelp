#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/highgui/highgui.hpp"

#include <QCoreApplication>
#include <QTime>

#include "Logger.h"
#include "func.h"
#include "Settings.h"

using namespace std;
using namespace cv;
using namespace logger;
using namespace func;

class Camera : public VideoCapture
{
public:
    Camera(Settings *settings);
    void Init();
    int width;
    int height;
    Point2f frame_center;
    int fps;
    int delay;
    void calculateFPS();

    //разгон видеокамеры
    //прогон нескольких кадров перед началом работы
    bool preliminaryTest(int frames);
private:
    Settings *settings;
};

#endif // CAMERA_H
