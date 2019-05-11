#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <QObject>
#include <QMap>
#include <QCoreApplication>
#include <QTime>

#include <func.h>
#include "Logger.h"
#include "Settings.h"

using namespace std;
using namespace cv;
using namespace logger;

class CalibrationEmersion
{
public:
    CalibrationEmersion();
    QList< QList<Point> > DetectPointsL;
    QList< QList<Point> > DetectPointsR;
    QList<Point> DetectPointsAvgL;
    QList<Point> DetectPointsAvgR;
    QList<Point> PointsMonitor;

    void InitDetectPointsAvg();
    bool checkBadCalibrovka();
    Point GetAvgPupil(Point leftPupil, Point rightPupil);
    Point GetMonitorPoint(Point leftPupil, Point rightPupil);
    void gotoNextPos();
    void setSettings(Settings*);
    void clearVariables();

private:
    Settings *settings;
    int maxXL, minXL, maxYL, minYL;
    int maxXR, minXR, maxYR, minYR;

    void getLineKoeffs_via2points(double& A_01, double& B_01, double& C_01, double& u_01, double& p_01, double& mu_01, double& sinu_01, double& cosu_01, Point p0, Point p1);
    bool pt_in_polygon(Point& point,const vector<Point> &polygon);
    int findNearbyArea(Point& pupilAvg);
    void init4points(vector<Point>& pointsL, vector<Point>& pointsR, vector<Point>& monitorPoints, int area);
    Point calcMP(Point p, vector<Point>& points, vector<Point>& monitorPoints);

};



class CalibrationMotion : public QObject
{
    Q_OBJECT
public:
    CalibrationMotion();
    Point GetMonitorPoint();
    Point GetMonitorPoint(vector<Point> *pupilsLeft, vector<Point> *pupilsRight);
    bool countAveregePupilCoords(vector<Point> pupilsLeft, vector<Point> pupilsRight);
    bool countAveregePupilCoords(vector< vector<Point> > pupilsLeft, vector< vector<Point> > pupilsRight);
    bool finishCalibration(vector< vector<Point> > leftPupils4, vector< vector<Point> > rightPupils4, vector<Rect>& foundEyes, vector<Point>& pupilsLeft, vector<Point>& pupilsRight, Rect& foundFace, Mat& foundfaceImg);
    void setSettings(Settings*);
    void clearVariables();

public:
    //отсеить выбросы, полученные при калибровке
    void motionCalibration_EliminateOutlyingBlips(vector< vector<Point> >& _leftPupils4, vector< vector<Point> >& _rightPupils4, vector<Point>& pupilsLeft, vector<Point>& pupilsRight);

    //данные полученные, после калибровки
    //TODO на самом деле calibrationCols, calibrationRows можно всегда получить из CalibrationAVGEyeRect width & height, так что это лишние переменные
    static int calibrationCols, calibrationRows;
    //после калибровки прямоугльник, внутри когорого движется глаз левый и правый
    static Rect leftEye_calibrationRect, rightEye_calibrationRect;

    //зрачки текущего кадра
    static Point pupilLeft, pupilRight;

    //усредненный зрачек в одном массиве
    static vector<Point> pupilAvg;
    //усредненный зрачок, раздеренный на 4 части - каждая часть по направлению движения зрачка
    static vector< vector<Point> > pupilAvg4;

    //для текущего кадра
    static Rect avgEyeRect;

    void setMainWindowSize(int mainWindow_width, int mainWindow_height);

    //переменная - направление движения маркера
    //0-3
    int directory = 0;

private:
    Settings *settings;

    //для текущего кадра
    static Rect leftEyeRect, rightEyeRect;

    //после калибровки прямоугльник, внутри когорого движется глаз
    static Rect CalibrationAVGEyeRect;
    //после калибровки прямоугльник, внутри когорого движется Левый и Правый глаз
    static Rect CalibrationAVGEyeRectL, CalibrationAVGEyeRectR;

    //левый и правый зрачки, раздеренные на 4 части - каждая часть по направлению движения зрачка
    vector< vector<Point> > leftPupils4, rightPupils4;

    //отсеить выбросы, полученные при калибровке
    void eliminateOutlyingBlips(vector<Point>& storona, int side, vector<Point>& pupils_errors);
    vector<Point> pupils_errors_R, pupils_errors_L;
    double GetAverageVal(vector<int> vct);
    double GetAverageVal2(vector<Point>& vct, bool isTopOrBottom);
    double GetStandardDeviation2(vector<Point>& vct, double averageValue, bool isTopOrBottom);
    double GetStandardDeviation(vector<int> vct, double averageValue);
    double getCriticalLevel(int size, float alfa);

    Rect getEyeRect(vector<Point> pupils);
    Rect getEyeRect(vector< vector<Point> > pupils);

    //переменные размеров основного окна
    static int mainWindow_width;
    static int mainWindow_height;

public slots:
    void directoryChangedSlot(int directory);
};

///класс обозначения линии. испольуется для задания линии через нормальное представление
///
/// уравнение прямой в виде:
/// Ax + By - C = 0
/// или
/// x*cos(u) + y*sin(u) - p = 0;
///
/// чтобы перевести из 1 формы в форму 2, нужно умножить всё на mu = +- 1/sqrt(A*A + B*B)
/// знак выбрать такой, чтобы mu * C < 0
///
class MyLine
{
public:
    double A, B, C, u, p, mu, sinu, cosu, tgu;
    void initKoeffs_via2points(Point2d p0, Point2d p1);
    static Point2d getCrossPoint(MyLine line1, MyLine line2);
    void initKoeffs_viaPointAndU(Point2d p0, double _u);
    static double distanceBetweenPoints(Point p1, Point p2);
    //static double lengthOf2Points(Point2d p1, Point2d p2);
private:

};

#endif // CALIBRATION_H
