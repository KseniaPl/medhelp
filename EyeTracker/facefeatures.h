#ifndef FACEFEATURES_H
#define FACEFEATURES_H

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <dest/dest.h>
#include <dest/face/face_detector.h>
#include <dest/util/draw.h>
#include <dest/util/convert.h>

#include "Logger.h"
#include "Settings.h"
#include <func.h>

#include "EyeTracker/kalmanfilters.h"

using namespace cv;
using namespace std;
using namespace logger;
using namespace dest;


class FaceFeatures
{
public:
    FaceFeatures();
    bool init();
    bool initFaceFeatures(Mat img);
    void assignPoints(vector<Point> points);
    double calcDistanse(int point1, int point2);

public:
    Rect eyeLeft, eyeRight;
    //прямоугольник лица текущего кадра
    Rect faceRect;
    //точки глаз на текущем кадре (6 точек)
    vector< vector<Point> > eyeContours;

private:
    Point leftpupil;
    Point rightpupil;
    Point face_centre;
    Point left_eye;
    Point right_eye;
    Point mid_eye;
    Point nose_base;
    Point nose_tip;
    Point mouth;

    //прямоугольники глаз, лица на первом кадре и середина лица 1-го кадра.
    Rect firstEyeLeft, firstEyeRight;
    Rect firstFaceRect;
    Point2f firstMeanPoint;

    //прямоугольник лица, найденный методом Виол-Дж, и середина лица текущего кадра
    Rect faceRectViol;
    Point2f meanPoint;
    vector<Point> meanPoints;

    double left_eye_nose_distance;
    double right_eye_nose_distance;
    double left_eye_right_eye_distance;
    double nose_mouth_distance;

    double mid_eye_mouth_distance;
    double nose_base_nose_tip_distance;
    double theta, tau;
    double sigma, symm_x;

    double normal[3];

    double yaw, pitch;

    double kalman_pitch, kalman_yaw;
    double kalman_pitch_pre, kalman_yaw_pre;


    Mat currentFrame;
    core::Tracker t;
    core::Shape s;
    core::ShapeTransform shapeToImage;
    core::Rect r;
    int frameCount, detectRate;
    Settings* settings;

    // The OpenCV detector rectangles are significantly different from tight bounds.
    // The values below attempt to match tight rects from landmarks and OpenCV rects.
    const float scaleToCV = 1.25f; // Scale between rectangles
    const  float txToCV = -0.01f; // Translation in x normalized by image width
    const float tyToCV = -0.05f;

    Mat foundfaceImg;
    bool detectSuccess = false;
    CascadeClassifier face_cascade, eyes_cascade;

private:
    bool getFaceCoords_Haar(Mat frame);
    void initFacePose();
    vector<Rect> getEyeCoordsRegionMethod(Mat frame_gray, Rect face);
    bool checkFaceEyes(Mat img);
    void showResults(Mat grayCV);
    bool calcFaceAndEyeRects(Point2f&);
    Point2f calcMeanPoint();
    void initEyeRects();
    // установка переменных eyeLeftPoints и eyeRightPoints (6 точек глаза)
    void calcEyeShapes();
};

#endif // FACEFEATURES_H
