#include "Blinkdetector.h"

BlinkDetector::BlinkDetector(QObject *parent) : QObject(parent)
{
    clearVariables();
}

void BlinkDetector::setSettings(Settings* sets)
{
    settings = sets;
}

void BlinkDetector::setFaceFeatures(FaceFeatures* faceFeatures)
{
    this->faceFeatures = faceFeatures;
}

uint64 BlinkDetector::calcHammingDistance(uint64 x, uint64 y)
{
    uint64 dist = 0, val = x ^ y;

    // Count the number of set bits
    while(val)
    {
        ++dist;
        val &= val - 1;
    }

    return dist;
}

// рассчитать хеш картинки
uint64 BlinkDetector::calcImageHash(Mat& src)
{
    if(src.cols == 0)
        return 0;

    Mat res, gray, bin;

    // уменьшаем картинку
    resize(src, res, Size(8,8));
    // переводим в градации серого
    cvtColor(res, gray, CV_BGR2GRAY);

    // вычисляем среднее
    cv::Scalar average = cv::mean( gray );

    //   printf("[i] average: %.2f \n", average.val[0]);

    // получим бинарное изображение относительно среднего
    // для этого воспользуемся пороговым преобразованием
    threshold(gray, bin, average.val[0], 255, CV_THRESH_BINARY);

    // построим хэш
    uint64 hash = 0;

    int i=0;
    // пробегаемся по всем пикселям изображения
    for (int y = 0; y < bin.rows; ++y) {
        for (int x = 0; x < bin.cols; ++x) {
            if(bin.at<unsigned char>(y, x))
            {
                // hash |= 1<<i;  // warning C4334: '<<' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
                hash |= (uint64)1<<i;
            }
            i++;
        }
    }

    // printf("[i] hash: %I64X \n", hash);

    //        if(show_results)
    //        {
    //            // увеличенные картинки для отображения результатов
    //            //Mat dst3 =  Mat(Size(128, 128), IPL_DEPTH_8U, 3);
    //            //IplImage* dst1 = cvCreateImage( cvSize(128, 128), IPL_DEPTH_8U, 1);

    //            Mat dst3, dst1;
    //            // показываем картинки
    //            resize(res, dst3, Size(128, 128));
    //            imshow( "64", dst3 );
    //            resize(gray, dst1, Size(128, 128));
    //            imshow( "gray", dst1 );
    //            resize(bin, dst1, Size(128, 128));
    //            imshow( "bin", dst1 );
    //        }

    return hash;
}

bool BlinkDetector::checkBlinkHash(Mat& frame)
{
    bool returnValue = false;

    Mat imgL_test = frame(faceFeatures->eyeLeft).clone();
    Mat imgR_test = frame(faceFeatures->eyeRight).clone();

    if (settings->showImage2)
        imshow("imgL_test", imgL_test);
    if (settings->showImage2)
        imshow("imgR_test", imgR_test);

    uint64 hashL_test = calcImageHash(imgL_test);
    uint64 hashR_test = calcImageHash(imgR_test);

    bool blink_L = false, blink_R = false;
    for (uint i = 0; i < closedEyesHashes[0].size(); i++)
    {
        uint64 distL = calcHammingDistance(closedEyesHashes[0][i], hashL_test);

        //logWrite("distL" + to_string(distL));
        if (distL < 15)
        {
            blink_L = true;
            break;
        }
    }

    for (uint i = 0; i < closedEyesHashes[1].size(); i++)
    {
        uint64 distR = calcHammingDistance(closedEyesHashes[1][i], hashR_test);

        //logWrite("distR" + to_string(distR));
        if (distR < 15)
        {
            blink_R = true;
            break;
        }
    }
    returnValue = blink_L && blink_R;

    return returnValue;
}

// http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_comparison/histogram_comparison.html
bool BlinkDetector::checkBlinkHist(Mat& frame)
{
    //    // СРАВНИТЬ гистограммы eyeLeft и eyeRight с closedEyeL и closedEyeR

    ////    closedEyeL;
    ////    closedEyeR;

    ////    double base_base = compareHist( hist_base, hist_base, compare_method );

    //    Mat src_base, hsv_base;
    //        Mat src_test1, hsv_test1;
    //        Mat src_test2, hsv_test2;
    //        Mat hsv_half_down;

    //        src_base = closedEyeL.clone();
    //        src_test1 = eyeLeft.clone();

    //        imshow("src_base", src_base);
    //        imshow("src_test1", src_test1);

    //        Mat src_test_gray, src_base_gray;
    //        cvtColor( src_base, src_test_gray, COLOR_BGR2GRAY );
    //        /// Convert to HSV
    //        cvtColor( src_base, hsv_base, COLOR_BGR2HSV );
    //        cvtColor( src_test1, hsv_test1, COLOR_BGR2HSV );

    //        vector<Mat> rgbChannels(3);
    //        split(hsv_base, rgbChannels);
    //        vector<Mat> rgbChannels3(3);
    //        split(src_base, rgbChannels3);

    //        vector<Mat> rgbChannels2(3);
    //        split(hsv_test1, rgbChannels2);
    //        Mat mmm2 = rgbChannels2[2];

    //       // imshow("mmm10", rgbChannels[0]);
    //       // imshow("mmm11", rgbChannels[1]);
    //        imshow("mmm12", rgbChannels[2]);
    //       // imshow("mmm330", rgbChannels3[0]);
    //        //imshow("mmm331", rgbChannels3[1]);
    //        //imshow("mmm332", rgbChannels3[2]);
    //        //imshow("src_test_gray", src_test_gray);

    //        imshow("mmm2", mmm2);

    //        hsv_half_down = hsv_base( Range( hsv_base.rows/2, hsv_base.rows - 1 ), Range( 0, hsv_base.cols - 1 ) );


    //        /// Using 50 bins for hue and 60 for saturation
    //        int h_bins = 50; int s_bins = 60; int v_bins = 256;
    //        int histSize[] = { v_bins };

    //        // hue varies from 0 to 179, saturation from 0 to 255
    //        float h_ranges[] = { 0, 180 };
    //        float s_ranges[] = { 0, 256 };
    //        float v_ranges[] = { 0, 256 };

    //        const float* ranges[] = { v_ranges };

    //        // Use the o-th and 1-st channels
    //        int channels[] = { 2};


    //        /// Histograms
    //        MatND hist_base;
    //        MatND hist_half_down;
    //        MatND hist_test1;
    //        MatND hist_test2;

    //        /// Calculate the histograms for the HSV images
    //        calcHist( &hsv_base, 1, channels, Mat(), hist_base, 1, histSize, ranges, true, false );
    //        normalize( hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat() );

    //        calcHist( &hsv_test1, 1, channels, Mat(), hist_test1, 1, histSize, ranges, true, false );
    //        normalize( hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat() );

    //        /// Apply the histogram comparison methods
    //        for( int i = 0; i < 4; i++ )
    //        {
    //            int compare_method = i;
    //            double base_base = compareHist( hist_base, hist_base, compare_method );
    //            double base_test1 = compareHist( hist_base, hist_test1, compare_method );

    //            //printf
    //            QString str = QString("Method [%1] Perfect, Base-Test(1) : %2, %3").arg(QString::number(i), QString::number(base_base), QString::number(base_test1));
    //            //logWrite( QString(" Method [%d] Perfect, Base-Half, Base-Test(1) : %f, %f, %f \n", i, base_base, base_half , base_test1).toStdString());
    //            logWrite(str.toStdString());
    //        }

    //   logWrite("------- checkBlinkHist END");
    return false;
}

void BlinkDetector::setBlinksetDone(bool b)
{
    blinksetDone = b;
}

void BlinkDetector::calculateClosedEyesHashes(Mat& frame)
{
    Mat face = frame(faceFeatures->faceRect).clone();
    if (closedEyeL.cols <= 0)
    {
        closedEyeL = face(faceFeatures->eyeLeft).clone();
        closedEyeR = face(faceFeatures->eyeRight).clone();
        if (settings->showImage2)
        {
            imshow("closedEyeL1",closedEyeL);
            imshow("closedEyeR1",closedEyeR);
        }
        closedEyesHashes.push_back(vector <uint64>());
        closedEyesHashes.push_back(vector <uint64>());
        closedEyesHashes[0].push_back(calcImageHash(closedEyeL));
        closedEyesHashes[1].push_back(calcImageHash(closedEyeR));
        return;
    }

    Mat closedEyeL1 = face(faceFeatures->eyeLeft).clone();
    Mat closedEyeR1 = face(faceFeatures->eyeRight).clone();

    uint64 hashL = calcImageHash(closedEyeL1);
    uint64 hashR = calcImageHash(closedEyeR1);

    bool flag = true;
    for (uint i = 0; i < closedEyesHashes[0].size(); i++)
    {
        uint64 distL = calcHammingDistance(closedEyesHashes[0][i], hashL);

        if (distL < 8)
        {
            flag = false;
            break;
        }
    }

    if (flag)
        closedEyesHashes[0].push_back(hashL);

    if (flag && settings->showImage2)
        imshow(to_string(hashL),closedEyeL1);

    flag = true;
    for (uint i = 0; i < closedEyesHashes[1].size(); i++)
    {
        uint64 distR = calcHammingDistance(closedEyesHashes[1][i], hashR);
        if (distR < 8)
        {
            flag = false;
            break;
        }
    }

    if (flag)
        closedEyesHashes[1].push_back(hashR);

    if (flag && settings->showImage2)
        imshow(to_string(hashR),closedEyeR1);
}


bool BlinkDetector::checkForBlink(Mat frame, vector<double> pupilValues)
{
    bool returnvalue;
    bool bool1, bool2, bool3;

    Mat face = frame(faceFeatures->faceRect).clone();

    //3 способа определения мограния. наилучшие 1 и 3 - используем их в совокупности
    bool1 = checkEyeDistance();
    bool2 = checkBlinkHash(face);
    bool3 = checkPupilValues(pupilValues);

    if (false)
    {
        logWrite("------ bool1 is " + to_string(bool1));
        logWrite("------ bool2 is " + to_string(bool2));
        logWrite("------ bool3 is " + to_string(bool3));
    }

    returnvalue = bool1 &&/* bool2 && */bool3;
    return returnvalue;
}

bool BlinkDetector::checkPupilValues(vector<double> pupilValues)
{
    if (pupilValues[0] < (max_pupilValueL) && pupilValues[1] < (max_pupilValueR))
        return true;
    return false;
}

bool BlinkDetector::checkEyeDistance()
{
    bool returnvalue = false;

    double dist1 = faceFeatures->calcDistanse(37,41);
    double dist2 = faceFeatures->calcDistanse(38,40);
    distanceL = (dist1 + dist2)/2;

    dist1 = faceFeatures->calcDistanse(43,47);
    dist2 = faceFeatures->calcDistanse(44,46);
    distanceR = (dist1 + dist2)/2;

    //logWrite("- minDistanceL is " + to_string(minDistanceL));
    //logWrite("- distanceL is " + to_string(distanceL));

    bool blinkL = false, blinkR = false;
    if(minDistanceL == INT_MAX)
    {
        double ratio = lastDistanceL/distanceL ;
        if(ratio > 1.5)
            blinkL = true;
        lastDistanceL  =  distanceL;
    }
    else if(distanceL < (minDistanceL * 1.10))
        blinkL = true;

    if(minDistanceR == INT_MAX)
    {
        double ratio1 = lastDistanceR/distanceR ;
        if(ratio1 > 1.5)
            blinkR = true;
        lastDistanceR  =  distanceR;
    }
    else if(distanceR < (minDistanceR * 1.10))
        blinkR = true;

    returnvalue = blinkL && blinkR;
    return returnvalue;
}

bool BlinkDetector::calculateMinDistance()
{
    double dist1 = faceFeatures->calcDistanse(37,41);
    double dist2 = faceFeatures->calcDistanse(38,40);
    double distL = (dist1 + dist2)/2;

    dist1 = faceFeatures->calcDistanse(43,47);
    dist2 = faceFeatures->calcDistanse(44,46);
    double distR = (dist1 + dist2)/2;

    if(distL < minDistanceL)
        minDistanceL = distL;

    if(distR < minDistanceR)
        minDistanceR = distR;

    return true;
}

void BlinkDetector::clearVariables()
{
    blinksetDone = false;
    minDistanceL = INT_MAX;
    minDistanceR = INT_MAX;

    //изображения закрытых глаз.
    closedEyeL = Mat();
    closedEyeR = Mat();
    closedEyesHashes.clear();

    sum_pupilValuesL = 0.0;
    sum_pupilValuesR = 0.0;
    count_pupilValuesL = 0;
    count_pupilValuesR = 0;
    avg_pupilValueL = 0.0;
    avg_pupilValueR = 0.0;

    max_pupilValueL = 0.0;
    max_pupilValueR = 0.0;
}

void BlinkDetector::calculateValues(vector<double> pupilValues)
{
    sum_pupilValuesL += pupilValues[0];
    sum_pupilValuesR += pupilValues[1];
    count_pupilValuesL++;
    count_pupilValuesR++;

    if (pupilValues[0] > max_pupilValueL)
        max_pupilValueL = pupilValues[0];
    if (pupilValues[1] > max_pupilValueR)
        max_pupilValueR = pupilValues[1];
}

void BlinkDetector::calculateValues_finish()
{
    avg_pupilValueL = sum_pupilValuesL / count_pupilValuesL;
    avg_pupilValueR = sum_pupilValuesR / count_pupilValuesR;
}
