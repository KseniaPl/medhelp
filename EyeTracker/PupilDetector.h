#pragma once

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>

#include "Logger.h"
#include "Settings.h"
#include "func.h"

using namespace cv;
using namespace std;
using namespace logger;
using namespace func;

class PupilDetector
{
public:
    PupilDetector(Settings *settings);
	~PupilDetector();
    vector<Point> getPupilCoords_GradientMethod(vector<Rect> eyes, Mat faceROI, vector< vector<Point> > *contours, vector<double> &max_values);
    void removingGlare(Mat& eyerectRou);
private:

	//----константы
	const bool kPlotVectorField = false;

	// переменная настроек
    Settings *settings;

	// Size constants
	const int kEyePercentTop = 25;
	const int kEyePercentSide = 13;
	const int kEyePercentHeight = 30;
	const int kEyePercentWidth = 35;

	// Preprocessing
	const bool kSmoothFaceImage = false;
	const float kSmoothFaceFactor = 0.005;

	// Algorithm Parameters
	const int kFastEyeWidth = 50;
	const int kWeightBlurSize = 5;
	const bool kEnableWeight = true;
	//const float kWeightDivisor = 150.0;
	//BUGFIX
	const float kWeightDivisor = 1.0;
	// в оригинальной статье указано такое значение
	//const double kGradientThreshold = 0.3;
	const double kGradientThreshold = 0.75;

	// Postprocessing
	const bool kEnablePostProcess = false;
	const float kPostProcessThreshold = 0.97;

	// Eye Corner
	const bool kEnableEyeCorner = false;

    const bool kScaleToFastSize = true;

	//----константы
	
    Point findEyeCenter(Mat face, Rect eye, bool first, vector<Point>& bestPoints, vector<Point> *contour, double &value);
    void scaleToFastSize(const Mat& src, Mat& dst, vector<Point> *contour);
	Mat computeMatXGradient(const Mat& mat);
	Mat matrixMagnitude(const Mat & matX, const Mat & matY);
	double computeDynamicThreshold(const Mat & mat, double stdDevFactor);
	Mat floodKillEdges(Mat& mat);
	bool floodShouldPushPoint(const Point& np, const Mat & mat);
    Point unscalePoint(Point p, Rect origSize);
	bool inMat(Point p, int rows, int cols);
    void testPossibleCentersFormula(int x, int y, const cv::Mat &weight, double gx, double gy, cv::Mat &out, bool first, vector<Point> *contour);
    void testPossibleCentersFormula2(int x, int y, const cv::Mat &weight, double gx, double gy, cv::Mat &out, Rect* area);
    Point findEyeCenterIPFMethod(Mat face, Rect eye, bool leftEye);
    Rect findEyeCDFAnalysis(Mat face, Rect eye);
    void correctPoint_CDFAnalysis(cv::Mat roi, cv::Point& pt_pupil);
};

