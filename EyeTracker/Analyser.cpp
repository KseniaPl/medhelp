#include "Analyser.h"

//static variables
vector<Point> Analyser::pupilsLeft, Analyser::pupilsRight;
vector< vector<Point> > Analyser::leftPupils4, Analyser::rightPupils4;

Analyser::Analyser(Settings *settings) : QObject()
{
    this->settings = settings;
    pupilDetector = new PupilDetector(settings);
    faceFeatures = new FaceFeatures();
    blinkDetector = new BlinkDetector(this);
    blinkDetector->setSettings(settings);
    blinkDetector->setFaceFeatures(faceFeatures);
}

bool Analyser::analyseImage(Mat& frame, bool calibrationDone, int directory /*переменная для 1 типа калибровки*/)
{
    // поиск координат лица и глаз
    if (faceFeatures->initFaceFeatures(frame))
    {
        faceRect = faceFeatures->faceRect;
        if (foundfaceImg.cols == 0)
            foundfaceImg = frame(faceRect).clone();

        //перевод в координаты относительно лица.
        faceFeatures->eyeLeft.x -= faceFeatures->faceRect.x;
        faceFeatures->eyeLeft.y -= faceFeatures->faceRect.y;
        faceFeatures->eyeRight.x -= faceFeatures->faceRect.x;
        faceFeatures->eyeRight.y -= faceFeatures->faceRect.y;

        eyes.clear();
        eyes.push_back(faceFeatures->eyeLeft);
        eyes.push_back(faceFeatures->eyeRight);

        vector<Point> pupils;
        if (settings->pupilsDetectionMethod == settings->pdm_GradientMethod)
        {
            pupils = pupilDetector->getPupilCoords_GradientMethod(eyes, frame(faceFeatures->faceRect), &faceFeatures->eyeContours, pupilsValues);

            //faceFeatures->assignPoints(pupils);

            if ((pupils[0].x != 0 && pupils[0].y != 0) && (pupils[1].x != 0 && pupils[1].y != 0))
            {
                foundPupils = pupils;
            }

            // добавляем найднные зрачки в глобальную переменную
            // для дальнейшего анализа траектории движения
            if (pupils.size() > 1) {
                pupilsLeft.push_back(pupils[0]);
                pupilsRight.push_back(pupils[1]);
                if (settings->CalibrationType == settings->ct_motion && !calibrationDone)
                {
                    //directory изменяется от 0 до 3 - 4 стороны
                    leftPupils4[directory].push_back(pupils[0]);
                    rightPupils4[directory].push_back(pupils[1]);
                }
            }
        }

        //определение моргания
        if(blinkDetector->blinksetDone && blinkDetector->checkForBlink(frame, pupilsValues))
        {
            logWrite("BLINK!!!!!!!!!!!");
            QApplication::beep();
        }

    }
    else
        return false;
    return true;
}


vector<Rect> Analyser::getEyeCoordsRegionMethod(Mat frame_gray, Rect face)
{
    vector<Rect> eyes;
    Mat faceROI = frame_gray(face);

    // Size constants
    int kEyePercentTop = 25;
    int kEyePercentSide = 15;
    int kEyePercentHeight = 26;
    int kEyePercentWidth = 30;

    //-- Find eye regions and draw them
    int eye_region_width = static_cast<int>(face.width * (kEyePercentWidth / 100.0));
    int eye_region_height = static_cast<int>(face.width * (kEyePercentHeight / 100.0));
    int eye_region_top = static_cast<int>(face.height * (kEyePercentTop / 100.0));
    Rect leftEyeRegion(static_cast<int>(face.width*(kEyePercentSide / 100.0)),
        eye_region_top, eye_region_width, eye_region_height);
    Rect rightEyeRegion(static_cast<int>(face.width - eye_region_width - face.width*(kEyePercentSide / 100.0)),
        eye_region_top, eye_region_width, eye_region_height);
    eyes.push_back(leftEyeRegion);
    eyes.push_back(rightEyeRegion);

    return eyes;
}


bool Analyser::MethodsInit()
{
    if(faceFeatures->init())
        return true;
    else
        return false;
}


bool Analyser::correctFaceViaTemplates(Rect& face, Mat frame)
{
    Mat faceImg = frame(face).clone();
    Mat resultNose, resultEyebrows;
    Point bestNoseMatch, bestEyebrowsMatch, minLoc, maxLoc, matchLoc;
    double minVal, maxVal;

    // Ищем шаблон только в области старого шаблона для ускорения процесса! Для этого создаём newFaceImg

    /*
    //найти брови-лоб, лучшй результат записать в bestEyebrowsMatch
    Mat newFaceImg = faceImg(Rect(templEyebrowsRect.x - 20, templEyebrowsRect.y - 20, templEyebrowsRect.width + 40, templEyebrowsRect.height + 40));
    matchTemplate(newFaceImg, templEyebrowsImg, resultEyebrows, CV_TM_SQDIFF_NORMED);
    normalize(resultEyebrows, resultEyebrows, 0, 1, NORM_MINMAX, -1, Mat());
    minMaxLoc(resultEyebrows, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    bestEyebrowsMatch = minLoc;
    */

    //найти нос, лучшй результат записать в bestEyebrowsMatch

    //Mat newFaceImg = faceImg(Rect(templNoseRect.x - templNoseRect.width*0.5, templNoseRect.y - templNoseRect.height*0.5, templNoseRect.width * 2, templNoseRect.height * 2));
    Mat newFaceImg = faceImg(Rect(templNoseRect.x - 20, templNoseRect.y - 20, templNoseRect.width + 40, templNoseRect.height + 40));
    matchTemplate(newFaceImg, templNoseImg, resultNose, CV_TM_SQDIFF_NORMED);
    normalize(resultNose, resultNose, 0, 1, NORM_MINMAX, -1, Mat());
    minMaxLoc(resultNose, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    bestNoseMatch = minLoc;

    //переводим в координаты лица
    bestNoseMatch.x = templNoseRect.x - 20 + bestNoseMatch.x;
    bestNoseMatch.y = templNoseRect.y - 20 + bestNoseMatch.y;
    //bestEyebrowsMatch.x = templEyebrowsRect.x - 20 + bestEyebrowsMatch.x;
    //bestEyebrowsMatch.y = templEyebrowsRect.y - 20 + bestEyebrowsMatch.y;

    //Визуальная проверка рерзультатов:

    //брови:
    //Mat bestEyebrowsMatchImg = faceImg(Rect(bestEyebrowsMatch.x, bestEyebrowsMatch.y, templEyebrowsImg.cols, templEyebrowsImg.rows)).clone();
    //imshow("Eyebrows Template", templEyebrowsImg);
    //imshow("Best Eyebrows Match", bestEyebrowsMatchImg);

    //нос:
    Mat bestNoseMatchImg = faceImg(Rect(bestNoseMatch.x, bestNoseMatch.y, templNoseImg.cols, templNoseImg.rows)).clone();
    imshow("Nose Template", templNoseImg);
    imshow("Best Nose Match", bestNoseMatchImg);

    //рисуем на изображении найденный шаблон и расположение шаблона на изначальном кадре, с которого записывался шаблон
    rectangle(faceImg, bestNoseMatch, Point(bestNoseMatch.x + templNoseImg.cols, bestNoseMatch.y + templNoseImg.rows), CV_RGB(220, 100, 60), 3.5);
    rectangle(faceImg, Point(templNoseRect.x, templNoseRect.y), Point(templNoseRect.x + templNoseImg.cols, templNoseRect.y + templNoseImg.rows), CV_RGB(255, 0, 0), 3.5);
    //rectangle(faceImg, bestEyebrowsMatch, Point(bestEyebrowsMatch.x + templEyebrowsImg.cols, bestEyebrowsMatch.y + templEyebrowsImg.rows), CV_RGB(220, 100, 60), 3.5);
    //rectangle(faceImg, Point(templEyebrowsRect.x, templEyebrowsRect.y), Point(templEyebrowsRect.x + templEyebrowsImg.cols, templEyebrowsRect.y + templEyebrowsImg.rows), CV_RGB(255, 0, 0), 3.5);
    imshow("Face Before Corretion", faceImg);

    //нос:
    //корректируем координаты лица (проверяем, чтобы новые координаты были в доступных пределах изображения)
    if (bestNoseMatch.x - templNoseRect.x < 30 &&
        bestNoseMatch.y - templNoseRect.y < 30
        )
    {
        if (face.x + bestNoseMatch.x - templNoseRect.x + face.width < frame.cols &&
            face.y + bestNoseMatch.y - templNoseRect.y + face.height < frame.rows &&
            face.x + bestNoseMatch.x - templNoseRect.x > 0 &&
            face.y + bestNoseMatch.y - templNoseRect.y > 0
            )
        {
            face.x += (bestNoseMatch.x - templNoseRect.x);
            face.y += (bestNoseMatch.y - templNoseRect.y);
        }
    }

    /*
    //брови:
    //корректируем координаты лица (проверяем, чтобы новые координаты были в доступных пределах изображения)
    if (bestEyebrowsMatch.x - templEyebrowsRect.x < 30 &&
        bestEyebrowsMatch.y - templEyebrowsRect.y < 30
        )
    {
        if (face.x + bestEyebrowsMatch.x - templEyebrowsRect.x + face.width < frame.cols &&
            face.y + bestEyebrowsMatch.y - templEyebrowsRect.y + face.height < frame.rows &&
            face.x + bestEyebrowsMatch.x - templEyebrowsRect.x > 0 &&
            face.y + bestEyebrowsMatch.y - templEyebrowsRect.y > 0
            )
        {
            face.x += (bestEyebrowsMatch.x - templEyebrowsRect.x);
            face.y += (bestEyebrowsMatch.y - templEyebrowsRect.y);
        }
    }
    */

    return true;
}



bool Analyser::initTemplates(Rect face, Mat frame, bool imageOnly)
{
    //bool imageOnly - обновить только каринку, но не координаты

    //брови:
    //координаты задаются в процентах от координат лица:
    /*if (!imageOnly)
    {
        int eyebrowsPercentTop = 15;
        int eyebrowsPercentSide = 20;
        int eyebrowsPercentHeight = 12;
        int eyebrowsPercentWidth = 60;

        //templEyebrowsRect.x = static_cast<int>(face.width*(eyebrowsPercentSide / 100.0));
        //templEyebrowsRect.y = static_cast<int>(face.height * (eyebrowsPercentTop / 100.0));
        //templEyebrowsRect.width = static_cast<int>(face.width * (eyebrowsPercentWidth / 100.0));
        //templEyebrowsRect.height = static_cast<int>(face.height * (eyebrowsPercentHeight / 100.0));

        //rectangle(frame, Rect(face.x + templEyebrowsRect.x, face.y + templEyebrowsRect.y, templEyebrowsRect.width, templEyebrowsRect.height), CV_RGB(0, 200, 200));
    }*/

    ////нос:
    if (!imageOnly)
    {
        int nosePercentTop = 48;
        int nosePercentSide = 25;
        int nosePercentHeight = 21;
        int nosePercentWidth = 50;

        templNoseRect.x = static_cast<int>(face.width*(nosePercentSide / 100.0));
        templNoseRect.y = static_cast<int>(face.height * (nosePercentTop / 100.0));
        templNoseRect.width = static_cast<int>(face.width * (nosePercentWidth / 100.0));
        templNoseRect.height = static_cast<int>(face.height * (nosePercentHeight / 100.0));
    }

    ////нос + губы:
//    if (!imageOnly)
//    {
//        int nosePercentTop = 60;
//        int nosePercentSide = 25;
//        int nosePercentHeight = 28;
//        int nosePercentWidth = 50;

//        templNoseRect.x = static_cast<int>(face.width*(nosePercentSide / 100.0));
//        templNoseRect.y = static_cast<int>(face.height * (nosePercentTop / 100.0));
//        templNoseRect.width = static_cast<int>(face.width * (nosePercentWidth / 100.0));
//        templNoseRect.height = static_cast<int>(face.height * (nosePercentHeight / 100.0));
//    }

    //rectangle(frame, Rect(face.x + templNoseRect.x, face.y + templNoseRect.y, templNoseRect.width, templNoseRect.height), CV_RGB(0, 200, 200));

    Mat templateNoseImg3ch = frame(Rect(face.x + templNoseRect.x, face.y + templNoseRect.y, templNoseRect.width, templNoseRect.height)).clone();
    //cvtColor(templateNoseImg3ch, templateNoseImg3ch, CV_RGB2HSV);
    vector<Mat> rgbChannels(3);
    split(templateNoseImg3ch, rgbChannels);
    templNoseImg_gray = rgbChannels[2];
    cvtColor(templNoseImg_gray, templNoseImg, CV_GRAY2RGB);

    //templEyebrowsImg = frame(Rect(face.x + templEyebrowsRect.x, face.y + templEyebrowsRect.y, templEyebrowsRect.width, templEyebrowsRect.height)).clone();

    //imwrite("res\\templateNose.jpg", imgNose);
    //imwrite("res\\templateEyebrows.jpg", imgEyebrows);
    //сбросить ROI
    //frame(Rect(0, 0, frame.cols, frame.rows));

    return true;
}

bool Analyser::correctCoordsViaTemplates(Rect& face2, Mat& frame2, vector<Rect>& _eyes2, vector<Point>& _pupils2)
{
    bool showimg = false;

    //переменная возврата ф-ции
    bool returnbool = true;

    //изменяем координаты не напрямую. копируем, в конце функции измененные координаты передаются обратно.
    vector<Point> _pupils = _pupils2;
    Rect face = face2;
    Mat frame = frame2.clone();
    vector<Rect> _eyes = _eyes2;

    //// проверка
    //Mat frame242 = frame.clone();
    //rectangle(frame242, face, CV_RGB(0, 0, 255));
    //rectangle(frame242, Rect(_eyes[0].x + face.x, face.y + _eyes[0].y, _eyes[0].width, _eyes[0].height), CV_RGB(0, 0, 255));
    //rectangle(frame242, Rect(_eyes[1].x + face.x, face.y + _eyes[1].y, _eyes[1].width, _eyes[1].height), CV_RGB(0, 0, 255));
    //circle(frame242, Point(_pupils[0].x + _eyes[0].x + face.x, _pupils[0].y + _eyes[0].y + face.y), 3, CV_RGB(0, 0, 255));
    //circle(frame242, Point(_pupils[1].x + _eyes[1].x + face.x, _pupils[1].y + _eyes[1].y + face.y), 3, CV_RGB(0, 0, 255));
    //circle(frame242, Point(foundPupils[0].x + _eyes[0].x + face.x, foundPupils[0].y + _eyes[0].y + face.y), 3, CV_RGB(0, 0, 255));
    //circle(frame242, Point(foundPupils[1].x + _eyes[1].x + face.x, foundPupils[1].y + _eyes[1].y + face.y), 3, CV_RGB(0, 0, 255));
    //imshow("tttttttt", frame242);

    _pupils = _pupils2;

    //переводим в координаты ВСЕГО изображения. (в конце метода приводим ображно в координаты (нового) лица)
    _pupils[0].x += _eyes[0].x + face.x;
    _pupils[0].y += _eyes[0].y + face.y;
    _pupils[1].x += _eyes[1].x + face.x;
    _pupils[1].y += _eyes[1].y + face.y;
    _eyes[0].x += face.x;
    _eyes[0].y += face.y;
    _eyes[1].x += face.x;
    _eyes[1].y += face.y;

    //поиск шаблона...
    Mat faceImg = frame(face).clone();
    Mat resultNose;//, resultEyebrows;
    Point bestNoseMatch, minLoc, maxLoc;//, bestEyebrowsMatch, matchLoc;
    double minVal, maxVal;

    // Ищем шаблон только в области старого шаблона для ускорения процесса! Для этого создаём newFaceImg
    /*
    //найти брови-лоб, лучшй результат записать в bestEyebrowsMatch
    Mat newFaceImg = faceImg(Rect(templEyebrowsRect.x - 20, templEyebrowsRect.y - 20, templEyebrowsRect.width + 40, templEyebrowsRect.height + 40));
    matchTemplate(newFaceImg, templEyebrowsImg, resultEyebrows, CV_TM_SQDIFF_NORMED);
    normalize(resultEyebrows, resultEyebrows, 0, 1, NORM_MINMAX, -1, Mat());
    minMaxLoc(resultEyebrows, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    bestEyebrowsMatch = minLoc;
    */

    //найти нос, лучшй результат записать в bestEyebrowsMatch

    //сколько добавить пикселей со всех сторон для поиска носа относительно точки приблизительного его расположения
    unsigned int addArea = std::min((faceImg.cols - templNoseRect.width) / 2, (faceImg.rows - templNoseRect.height) / 2);

    if (addArea > 16)
        addArea = 15;
    else
        addArea--;

    Mat noseArea = faceImg;

    noseArea = faceImg(Rect(templNoseRect.x - addArea, templNoseRect.y - addArea, templNoseRect.width + 2 * addArea, templNoseRect.height + 2 * addArea));

    // Mat frameCopy;// = frame.clone();
    // cvtColor(frame, frameCopy, CV_BGR2GRAY);
    // размытие
    // cvPyrDown(img, temp, CV_GAUSSIAN_5x5);
    // выравнивание гистограммы
    // equalizeHist(frame_gray, frame_gray);

    //берем также один канал (но переводим одноканальное изображение в 3х канальное для работы matchTemplate)
    vector<Mat> rgbChannels(3);
    //можно перевести в HSV и также использовать 3 канал. протестировать. (todo:)
    //cvtColor(noseArea, noseArea, CV_RGB2HSV);
    split(noseArea, rgbChannels);
    Mat noseArea2ch = rgbChannels[2];

    if (showimg)
    {
        imshow("noseArea2ch", noseArea2ch);
        imshow("templNoseImg_gray", templNoseImg_gray);
    }

    matchTemplate(noseArea2ch, templNoseImg_gray, resultNose, CV_TM_SQDIFF_NORMED);
    normalize(resultNose, resultNose, 0, 1, NORM_MINMAX, -1, Mat());
    minMaxLoc(resultNose, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    bestNoseMatch = minLoc;

    //переводим в координаты изображения! больше face не нужен
    bestNoseMatch.x = templNoseRect.x - addArea + bestNoseMatch.x + face.x;
    bestNoseMatch.y = templNoseRect.y - addArea + bestNoseMatch.y + face.y;

    ////Визуальная проверка рерзультатов:
    if (showimg)
    {
        Mat temp = frame.clone();
        rectangle(temp, Rect(bestNoseMatch.x, bestNoseMatch.y, templNoseImg.cols, templNoseImg.rows), CV_RGB(0, 0, 255));
        imshow("faceImg223", temp);
        temp.release();
    }

    if (showimg)
    {
        //нос:
        Mat bestNoseMatchImg = frame(Rect(bestNoseMatch.x, bestNoseMatch.y, templNoseImg.cols, templNoseImg.rows)).clone();
        imshow("Best Nose Match", bestNoseMatchImg);
    }

    //нос:
    //корректируем координаты лица (проверяем, чтобы новые координаты были в доступных пределах изображения)
    if ((bestNoseMatch.x - templNoseRect.x) + faceRect.width < frame.cols &&
        (bestNoseMatch.y - templNoseRect.y) + faceRect.height < frame.rows &&
        (bestNoseMatch.x - templNoseRect.x > 0) &&
        (bestNoseMatch.y - templNoseRect.y > 0)
        )
    {
        face.x = bestNoseMatch.x - templNoseRect.x;
        face.y = bestNoseMatch.y - templNoseRect.y;
        face.width = faceRect.width;
        face.height = faceRect.height;
    }
    else
    {
        logWriteError("Координаты найденного шаблона выходят за границы!");
    }

    // глаза размечаем на новом лице
    _eyes = getEyeCoordsRegionMethod(frame, face);
    // координаты зрачков - не относителньо ферйма, а относительно глаз
    _pupils[0].x -= (face.x + _eyes[0].x);
    _pupils[0].y -= (face.y + _eyes[0].y);
    _pupils[1].x -= (face.x + _eyes[1].x);
    _pupils[1].y -= (face.y + _eyes[1].y);

    if (showimg)
    {
        Mat frame242 = frame.clone();
        rectangle(frame242, face, CV_RGB(0, 0, 255));
        rectangle(frame242, Rect(_eyes[0].x + face.x, face.y + _eyes[0].y, _eyes[0].width, _eyes[0].height), CV_RGB(0, 0, 255));
        rectangle(frame242, Rect(_eyes[1].x + face.x, face.y + _eyes[1].y, _eyes[1].width, _eyes[1].height), CV_RGB(0, 0, 255));
        circle(frame242, Point(_pupils[0].x + _eyes[0].x + face.x, _pupils[0].y + _eyes[0].y + face.y), 3, CV_RGB(0, 0, 255));
        circle(frame242, Point(_pupils[1].x + _eyes[1].x + face.x, _pupils[1].y + _eyes[1].y + face.y), 3, CV_RGB(0, 0, 255));
        imshow("tttttttt", frame242);
    }

    _pupils2 = _pupils;
    face2 = face;
    frame2 = frame.clone();
    _eyes2 = _eyes;

    return returnbool;
}


bool Analyser::correctCoordsViaPupilsDistance(Rect& face2, Mat& frame2, vector<Rect>& _eyes2, vector<Point>& _pupils2)
{
    if (firstLength == 0)
        return false;

    //переменная возврата ф-ции
    bool returnbool = true;

    //задать нулевые координаты неправильно найденному зрачку перед выходом функции
    bool setZeroTo_Pupil0 = false;
    bool setZeroTo_Pupil1 = false;
    //изменяем координаты не напрямую. копируем, в конце функции измененные координаты передаются обратно.
    vector<Point> _pupils = _pupils2;
    Rect face = face2;
    Mat frame = frame2.clone();
    vector<Rect> _eyes = _eyes2;

    double factor = 0;
    //
    // factor - множитель, который будет изменять изображение (увеличивать или уменьшать)
    // координаты eyes изменяются под новые координаты face
    // координаты pupils изменяются под новые координаты eyes
    //

    // проверка
    /*Mat frame242 = frame.clone();
    rectangle(frame242, face, CV_RGB(0, 0, 255));
    rectangle(frame242, Rect(_eyes[0].x + face.x, face.y + _eyes[0].y, _eyes[0].width, _eyes[0].height), CV_RGB(0, 0, 255));
    rectangle(frame242, Rect(_eyes[1].x + face.x, face.y + _eyes[1].y, _eyes[1].width, _eyes[1].height), CV_RGB(0, 0, 255));
    circle(frame242, Point(_pupils[0].x + _eyes[0].x + face.x, _pupils[0].y + _eyes[0].y + face.y), 3, CV_RGB(0, 0, 255));
    circle(frame242, Point(_pupils[1].x + _eyes[1].x + face.x, _pupils[1].y + _eyes[1].y + face.y), 3, CV_RGB(0, 0, 255));
    circle(frame242, Point(foundPupils[0].x + _eyes[0].x + face.x, foundPupils[0].y + _eyes[0].y + face.y), 3, CV_RGB(0, 0, 255));
    circle(frame242, Point(foundPupils[1].x + _eyes[1].x + face.x, foundPupils[1].y + _eyes[1].y + face.y), 3, CV_RGB(0, 0, 255));
    imshow("tttttttt", frame242);*/

    ///
    /// отбрасываем худшие результаты найденных зарчков.
    /// если есть отклонение на _eyes[0].width / 4, _eyes[0].height / 4 - записываем 0 в конце метода.
    /// setZeroTo_Pupil0, setZeroTo_Pupil1 проверяются в конце метода
    ///
    if (foundPupils[0].x > _pupils[0].x + _eyes[0].width / 4 ||
        foundPupils[0].x < _pupils[0].x - _eyes[0].width / 4 ||
        foundPupils[0].y > _pupils[0].y + _eyes[0].height / 4 ||
        foundPupils[0].y < _pupils[0].y - _eyes[0].height / 4
        )
    {
        factor = firstLength / lastLength;
        returnbool = false;
        setZeroTo_Pupil0 = true;
    }
    if (foundPupils[1].x > _pupils[1].x + _eyes[1].width / 4 ||
        foundPupils[1].x < _pupils[1].x - _eyes[1].width / 4 ||
        foundPupils[1].y > _pupils[1].y + _eyes[1].height / 4 ||
        foundPupils[1].y < _pupils[1].y - _eyes[1].height / 4
        )
    {
        factor = firstLength / lastLength;
        returnbool = false;
        setZeroTo_Pupil1 = true;
    }

    _pupils = _pupils2;

    //переводим в координаты ВСЕГО изображения. (в конце метода приводим ображно в координаты (нового) лица)
    _pupils[0].x += _eyes[0].x + face.x;
    _pupils[0].y += _eyes[0].y + face.y;
    _pupils[1].x += _eyes[1].x + face.x;
    _pupils[1].y += _eyes[1].y + face.y;
    _eyes[0].x += face.x;
    _eyes[0].y += face.y;
    _eyes[1].x += face.x;
    _eyes[1].y += face.y;

    if (factor == 0)
    {
        //расстояние между зрачками
        double length = norm(_pupils[0] - _pupils[1]);

        /*	if (length > lastLength + foundEyes[0].width/5 || length < lastLength*0.8)
        factor = firstLength / lastLength;
        else*/

        factor = firstLength / length;
        lastLength = length;
    }

    //изменение всего, начиная с frame.
    Size newFrameSize(static_cast<int>(frame.cols*factor), static_cast<int>(frame.rows*factor));
    resize(frame, frame, newFrameSize);

    _eyes[0].x = static_cast<int>(_eyes[0].x * factor);
    _eyes[0].y = static_cast<int>(factor * _eyes[0].y);
    _eyes[0].height = static_cast<int>(factor * _eyes[0].height);
    _eyes[0].width = static_cast<int>(factor * _eyes[0].width);
    _eyes[1].x = static_cast<int>(factor * _eyes[1].x);
    _eyes[1].y = static_cast<int>(factor * _eyes[1].y);
    _eyes[1].height = static_cast<int>(factor * _eyes[1].height);
    _eyes[1].width = static_cast<int>(factor * _eyes[1].width);

    _pupils[0].x = static_cast<int>(factor * _pupils[0].x);
    _pupils[0].y = static_cast<int>(factor * _pupils[0].y);
    _pupils[1].x = static_cast<int>(factor * _pupils[1].x);
    _pupils[1].y = static_cast<int>(factor * _pupils[1].y);

    face.x = static_cast<int>(factor * face.x);
    face.y = static_cast<int>(factor * face.y);
    face.height = static_cast<int>(factor * face.height);
    face.width = static_cast<int>(factor * face.width);

    // глаза размечаем на новом лице
    _eyes = getEyeCoordsRegionMethod(frame, face);
    // координаты зрачков - не относителньо ферйма, а относительно глаз
    _pupils[0].x -= (face.x + _eyes[0].x);
    _pupils[0].y -= (face.y + _eyes[0].y);
    _pupils[1].x -= (face.x + _eyes[1].x);
    _pupils[1].y -= (face.y + _eyes[1].y);

    if (setZeroTo_Pupil0)
    {
        _pupils[0].x = 0;
        _pupils[0].y = 0;
    }
    if (setZeroTo_Pupil1)
    {
        _pupils[1].x = 0;
        _pupils[1].y = 0;
    }

    _pupils2 = _pupils;
    face2 = face;
    frame2 = frame.clone();
    _eyes2 = _eyes;

    return returnbool;
}



Mat Analyser::MotionDetector(Mat currentFrame, Mat previousFrame)
{
    if (currentFrame.rows > 1000)
    {
        resize(currentFrame, currentFrame, Size(currentFrame.cols*0.5, currentFrame.rows*0.5));
        resize(previousFrame, previousFrame, Size(previousFrame.cols*0.5, previousFrame.rows*0.5));
    }
    double koff = settings->koffMotion;
    Mat frameDiff = currentFrame.clone();
    for (int y = 0; y < currentFrame.rows; ++y)
    {
        for (int x = 0; x < currentFrame.cols; ++x) {
            frameDiff.at<Vec3b>(y, x)[0] = ((255 - koff * abs(currentFrame.at<Vec3b>(y, x)[0] - previousFrame.at<Vec3b>(y, x)[0]) >= 0) ?
                koff * abs(currentFrame.at<Vec3b>(y, x)[0] - previousFrame.at<Vec3b>(y, x)[0]) :
                abs(currentFrame.at<Vec3b>(y, x)[0] - previousFrame.at<Vec3b>(y, x)[0]));
            frameDiff.at<Vec3b>(y, x)[1] = ((255 - koff * abs(currentFrame.at<Vec3b>(y, x)[1] - previousFrame.at<Vec3b>(y, x)[1]) >= 0) ?
                koff * abs(currentFrame.at<Vec3b>(y, x)[1] - previousFrame.at<Vec3b>(y, x)[1]) :
                abs(currentFrame.at<Vec3b>(y, x)[1] - previousFrame.at<Vec3b>(y, x)[1]));
            frameDiff.at<Vec3b>(y, x)[2] = ((255 - koff * abs(currentFrame.at<Vec3b>(y, x)[2] - previousFrame.at<Vec3b>(y, x)[2]) >= 0) ?
                koff * abs(currentFrame.at<Vec3b>(y, x)[2] - previousFrame.at<Vec3b>(y, x)[2]) :
                abs(currentFrame.at<Vec3b>(y, x)[2] - previousFrame.at<Vec3b>(y, x)[2]));
        }
    }

    //Mat rgbframeDiff[3], rgbCurrentFrame[3], rgbPreviousFrame[3];
    //split(currentFrame, rgbCurrentFrame);
    //split(previousFrame, rgbPreviousFrame);
    //absdiff(rgbCurrentFrame[0], rgbPreviousFrame[0], rgbframeDiff[0]);
    //absdiff(rgbCurrentFrame[1], rgbPreviousFrame[1], rgbframeDiff[1]);
    //absdiff(rgbCurrentFrame[2], rgbPreviousFrame[2], rgbframeDiff[2]);
    //merge(rgbframeDiff, 3, frameDiff);
    return frameDiff;
}

bool Analyser::clearVariables()
{
    logWrite("clearVariables called");
    faceRect = Rect();
    foundfaceImg = Mat();
    foundPupils.clear();

    pupilsLeft.clear();
    pupilsRight.clear();
    leftPupils4.clear();
    rightPupils4.clear();
    pupilsValues.clear();

    leftPupils4.push_back(vector<Point>());
    rightPupils4.push_back(vector<Point>());
    leftPupils4.push_back(vector<Point>());
    rightPupils4.push_back(vector<Point>());
    leftPupils4.push_back(vector<Point>());
    rightPupils4.push_back(vector<Point>());
    leftPupils4.push_back(vector<Point>());
    rightPupils4.push_back(vector<Point>());

    pupilsValues.push_back(0.0);
    pupilsValues.push_back(0.0);

    //TODO проверить
    eyes.clear();
    templNoseImg = Mat();
    templEyebrowsImg = Mat();
    templEyebrowsRect = Rect();
    templNoseRect = Rect();
    firstLength = 0;
    lastLength = 0;
    return true;
}

vector<Point> Analyser::GetFoundPupilsCoordsFrame()
{
    vector<Point> returnvalue;
    Point left = Point(foundPupils[0].x + eyes[0].x + faceFeatures->faceRect.x,
            foundPupils[0].y + eyes[0].y + faceFeatures->faceRect.y);
    Point right = Point(foundPupils[1].x + eyes[1].x + faceFeatures->faceRect.x,
            foundPupils[1].y + eyes[1].y + faceFeatures->faceRect.y);
    returnvalue.push_back(left);
    returnvalue.push_back(right);
    return returnvalue;
}
