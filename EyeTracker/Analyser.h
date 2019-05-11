#ifndef ANALYSER_H
#define ANALYSER_H

#include "opencv2/core/core.hpp"
#include "opencv2/core/types_c.h"
#include "opencv2/imgproc/imgproc.hpp"

#include <QObject>
#include "QApplication"

#include "Logger.h"
#include "PupilDetector.h"
#include "Settings.h"
#include "FaceFeatures.h"
#include "Blinkdetector.h"

using namespace std;
using namespace cv;
using namespace logger;

///
/// The Analyser class
/// Класс, отвечающий за анализ изображения и получение координат глаз.
///

class Analyser : public QObject
{
    Q_OBJECT
public:
    Analyser(Settings *settings);
    BlinkDetector *blinkDetector;

    //инициализация методов
    bool MethodsInit();
    //анализ изображения
    bool analyseImage(Mat& frame, bool calibrationDone, int directory = 0);
    //очистить переменные
    bool clearVariables();

    Mat MotionDetector(Mat currentFrame, Mat previousFrame);

    //получить foundPupils относительно  кадра
    vector<Point> GetFoundPupilsCoordsFrame();

    //переменные:
    Rect faceRect;
    vector<Rect> eyes;
    Mat foundfaceImg;
    vector<Point> foundPupils;
    static vector<Point> pupilsLeft, pupilsRight;
    //значения, которое вернул PupilDetector - насколько точно определился зрачок.
    //если значение мало, то скорее всего это не зрачок - мб моргание)
    vector<double> pupilsValues;

    //данные для калибровки-движение маркера
    //длительность движения шарика
    static vector< vector<Point> > leftPupils4, rightPupils4;

private:
    Settings *settings;

    // методы (лицо, глаза, зрачки)
    vector<Rect> getEyeCoordsRegionMethod(Mat frame_gray, Rect face);

    //определениее шаблонов
    bool initTemplates(Rect face, Mat frame, bool imageOnly = false);
    //корректирование найденного лица с помощью шаблонов
    bool correctFaceViaTemplates(Rect& face, Mat frame);
    bool correctCoordsViaTemplates(Rect& face2, Mat& frame2, vector<Rect>& _eyes2, vector<Point>& _pupils2);
    bool correctCoordsViaPupilsDistance(Rect& face2, Mat& frame2, vector<Rect>& _eyes2, vector<Point>& _pupils2);

private:
    PupilDetector *pupilDetector;
    FaceFeatures* faceFeatures;

    //первая и предыдущая длинна между зрачками
    double firstLength;
    double lastLength;

    //шаблоны
    Mat templNoseImg;
    Mat templEyebrowsImg;
    Rect templEyebrowsRect;
    Rect templNoseRect;
    Mat templNoseImg_gray;


signals:

};

#endif // ANALYSER_H
