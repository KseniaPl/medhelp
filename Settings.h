#pragma once

#include "opencv2/highgui/highgui.hpp"
#include <QObject>
#include "Logger.h"

using namespace cv;
using namespace std;
using namespace logger;

class Settings : public QObject
{
    Q_OBJECT
public:
    Settings();
    ~Settings();

	enum faceDetectionMethod{ fdm_NoMethod = 0, fdm_HaarMethod = 1 };
	enum eyesDetectionMethod{ edm_NoMethod = 0, edm_MarkMethod = 1, edm_HaarMethod = 2 };
	enum pupilsDetectionMethod{ pdm_NoMethod = 0, pdm_GradientMethod = 1 };
	enum flipImage{ flipImage0 = 0, flipImage90 = 1, flipImage180 = 2, flipImageM90 = 3 };
	enum TrajectoryType{ tt_motionTrajectory = 0, tt_blinkTrajectory = 1};
    enum CalibrationType {ct_motion = 0, ct_emersion = 1};
    enum InputType {it_camera = 0, it_videoFile = 1, it_image = 2};
    enum projectionType {pt_none = 0, pt_exact = 1, pt_smooth = 2};

    unsigned int faceDetectionMethod;
    unsigned int eyesDetectionMethod;
    unsigned int pupilsDetectionMethod;
    unsigned int TrajectoryType;
    unsigned int InputType;

    static unsigned int CalibrationType;
    Q_INVOKABLE unsigned int getCalibrationType()
    { return CalibrationType; }
    Q_INVOKABLE void setCalibrationType(unsigned int type)
    { CalibrationType = type; }

    static bool showImage;
    Q_INVOKABLE bool getShowImage()
    { return showImage; }
    Q_INVOKABLE void setShowImage(bool value)
    { showImage = value; }

    bool autoCalculateCameraFPS;
    bool coordsCorrectionViaTemplates;
    bool coordsCorrectionViaPupilsDistance;
    bool ifSearchFaceOneFrame;
    bool videoTrajectory;
    bool videoMotion;
    bool saveCoords;
    bool changeSizeWindow;
    double koffMotion;
    double Step;
    double GradientThreshold;
    double kFastEyeWidth;
	
    unsigned int NumberLengthForTrajectory;
    unsigned int NumberHeightForTrajectory;
    int inputDevice;
    QString inputFileName;
    QString inputImage;
    int test_count;

    //количество точек при калибровке
    static unsigned int pointsCount;
    Q_INVOKABLE unsigned int getPointsCount()
    { return pointsCount; }

    static unsigned int flipImage;
    Q_INVOKABLE unsigned int getFlipImage()
    { return flipImage; }
    Q_INVOKABLE void setFlipImage(unsigned int type)
    { flipImage = type; }

    static bool flip_mirrorReflection;
    Q_INVOKABLE bool getFlip_mirrorReflection()
    { return flip_mirrorReflection; }
    Q_INVOKABLE void setFlip_mirrorReflection(bool value)
    { flip_mirrorReflection = value; }

	//ENGINEERING MODE
    bool eng_save_calibration;

    //сохранение в видеофайл
    bool saveCalibrationVideo;
    bool saveWorkProcessVideo;

    unsigned int projectionType;

    bool showImage2;

public:
    bool loadSettings(QString filename);

    Q_INVOKABLE bool saveSettings(QString filename);

private:
    bool setDefaultSets();

};

