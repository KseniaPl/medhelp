#ifndef BLINKDETECTOR_H
#define BLINKDETECTOR_H

#include "FaceFeatures.h"
#include <Settings.h>
#include "Logger.h"

#include <QObject>

using namespace std;
using namespace cv;
using namespace logger;

class BlinkDetector : public QObject
{
    Q_OBJECT
public:
    explicit BlinkDetector(QObject *parent = 0);
    void setSettings(Settings* sets);
    void setFaceFeatures(FaceFeatures*);

    bool checkForBlink(Mat frame, vector<double> pupilValues);
    bool calculateMinDistance();
    void calculateValues(vector<double> pupilValues);
    void calculateValues_finish();
    bool checkPupilValues(vector<double> pupilValues);
    void setBlinksetDone(bool);
    void clearVariables();
    //инициализация closedEyeL, closedEyeR
    void calculateClosedEyesHashes(Mat& frame);

public:
    //было пройдено определение закрытого глаза
    bool blinksetDone;

private:
    Settings* settings;
    FaceFeatures* faceFeatures;

    //изображения закрытых глаз.
    Mat closedEyeL, closedEyeR;
    vector< vector<uint64> > closedEyesHashes;

    double distanceL = 0;
    double distanceR = 0;
    double lastDistanceL = 0;
    double lastDistanceR = 0;
    double minDistanceR, minDistanceL;

private:
    uint64 calcHammingDistance(uint64 x, uint64 y);
    uint64 calcImageHash(Mat& src);

    bool checkEyeDistance();

    //сравнение текущих глаз с closedEyeL, closedEyeR
    bool checkBlinkHist(Mat& frame);
    bool checkBlinkHash(Mat& frame);

    double avg_pupilValueL, avg_pupilValueR;
    double max_pupilValueL, max_pupilValueR;
    double sum_pupilValuesL, sum_pupilValuesR;
    int count_pupilValuesL, count_pupilValuesR;

signals:

public slots:
};

#endif // BLINKDETECTOR_H
