#include "Eyetracker.h"

bool EyeTracker::isFullStop = false;
Camera *EyeTracker::camera;
bool EyeTracker::calibrationDone = false;
unsigned int EyeTracker::StatusCalibration = EyeTracker::None;
//переменные размеров экрана
int EyeTracker::mainWindow_width;
int EyeTracker::mainWindow_height;
//зрачки найдены впервые
 bool EyeTracker::firstFoundPupils = true;
 bool EyeTracker::firstFrame = true;

EyeTracker::EyeTracker(Settings *settings): QObject()
{
    this->settings = settings;
    init();
}

EyeTracker::EyeTracker(Settings *settings, int wighth, int height): QObject()
{
    this->settings = settings;
    init();
    mainWindow_width = wighth;
    mainWindow_height = height;
}

bool EyeTracker::init()
{
    fileManager = new FileManager(settings);
    analyser = new Analyser(settings);
    camera = new Camera(settings);
    //создаем объект класса с калибровкой типа Появление шарика
    calibrationEmersion = new CalibrationEmersion();
    calibrationEmersion->setSettings(settings);

    calibrationMotion.setSettings(settings);

    fllagEmersionRoundCalib = false;
    flagStopEmersionRoundCalibSlot = true;
    currentPoint = Point(0, 0);

    analyser->clearVariables();
    methodIsInit = analyser->MethodsInit();

    if (!methodIsInit)
    {
        logWriteError("EyeTracker::init. analyser->MethodsInit returned false");
        return false;
    }

    thread = new QThread();
    this->moveToThread(thread);

    connect(this, SIGNAL(destroyed()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    connect(thread, SIGNAL(started()), this, SLOT(eventLoop()));

    thread->start();
}

EyeTracker::~EyeTracker()
{
    disconnect(timer, SIGNAL(timeout()), this, SLOT(timeoutHandler()));

    if(timer->isActive())
        timer->stop();

    thread->quit();
    //thread->wait();
}

//инициализировать таймер В своём потоке!
void EyeTracker::initTimer()
{
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutHandler()));
}

void EyeTracker::start()
{
    //TODO: установить timer интервал
    if(!timer->isActive()) {
        framesCount = 0;
        time.start();
        timer->start(camera->delay);
    }
    SetCaptureIsWorking(true);
}

bool EyeTracker::cameraIsOpened()
{
    return camera->isOpened();
}

bool EyeTracker::GetCaptureIsWorking()
{
    return captureIsWorking;
}

void EyeTracker::SetCaptureIsWorking(bool value)
{
    captureIsWorking = value;
    logWrite("captureIsWorking is " + to_string(captureIsWorking));
}

void EyeTracker::clearVariables()
{
    analyser->clearVariables();
    calibrationMotion.clearVariables();
    calibrationEmersion->clearVariables();
    analyser->blinkDetector->clearVariables();
    //TODO добавить calibrationEmersion
    func::safeDelay(1000);
}

bool EyeTracker::CameraInitialisation()
{
    //обнуляем все переменные.
    camera->release();
    clearVariables();

    if (!camera->isOpened())
    {
        camera->Init();
        bool ifinitCamera;
        if(settings->InputType == settings->it_camera)
            ifinitCamera = captureInit(settings->inputDevice);
        else
            ifinitCamera = captureInit(settings->inputFileName);

        if (!ifinitCamera) {
            logWriteError("VideoCapture hasn't been initialised");
            return false;
        }
    }
    logWrite("VideoCapture has been initialised");

    if (settings->InputType == settings->it_camera)
    {
//        camera->calculateFPS();
//        //предварительный тест
//        if (!camera->preliminaryTest(15)) {
//            logWriteError("Камера отключена");
//            fullStop();
//            return false;
//        }

        //проверка, нужна ли запись видео (раб.процесс и калибровка)
        if (calibrationDone)
        {
            if (settings->saveWorkProcessVideo && !fileManager->videoRecordInit(camera, settings->autoCalculateCameraFPS)) {
                logWriteError("VideoWriter hasn't been initialised");
                return false;
            }
        }
        else
        {
            if (settings->saveCalibrationVideo && !fileManager->videoRecordInit(camera, settings->autoCalculateCameraFPS)) {
                logWriteError("VideoWriter hasn't been initialised");
                return false;
            }
        }
    }
    delay = static_cast<int>(1000 / camera->get(CV_CAP_PROP_FPS));
    return true;
}

bool EyeTracker::captureInit(QString fileName)
{
    if (!fileName.isNull() && !fileName.isEmpty())
        camera->open(fileName.toStdString());

    if (!camera->isOpened())
        return false;

    return true;
}

bool EyeTracker::captureInit(int inputDevice)
{
    camera->open(inputDevice);
    if (!camera->isOpened())
        return false;

    int cameraFPS = static_cast<int>(camera->get(CV_CAP_PROP_FPS));
    try
    {
        if (cameraFPS < 30)
        {
            camera->set(CV_CAP_PROP_FPS, 30);
            safeDelay(20);
        }
    }
    catch (Exception ex)
    {
    }

    safeDelay(1000);

    camera->calculateFPS();
    if (!camera->preliminaryTest(15)) {
        return false;
    }

    return true;
}

int EyeTracker::startCalibration()
{
    if(!methodIsInit) return Failed;

    int returnStatus = None;

    clearVariables();

    captureIsWorking = true;

    if (settings->CalibrationType == settings->ct_emersion)
    {
        fllagEmersionRoundCalib = false;
        flagStopEmersionRoundCalibSlot = true;
    }
    else if (settings->CalibrationType == settings->ct_motion)
    { }

    // задержка в цикле
    //int delay = static_cast<int>(1000 / camera->get(CV_CAP_PROP_FPS));

    while (captureIsWorking)
    {
        if (isFullStop)
        {
            fullStop();
            return Failed;
        }
        try {
            if (!camera->read(frame)) {
                //end of media
                logWriteError("Камера отключена");
                fullStop();
                return Failed;
            }

            //поворот фрейма
            flipImage(frame);

            //сохранение в файл
            if (settings->saveCalibrationVideo)
                fileManager->videoWriter.write(frame);

            //анализ
            if (!analyser->analyseImage(frame, calibrationDone, calibrationMotion.directory))
            {
                //logWriteError("Лицо не найдено!");
                //fullStop();
                //return Failed;
                continue;
            }

            //показать движение
            if (settings->videoMotion)
            {
                if (firstFrame) {
                    firstFrame = false;
                    previousFrame = frame.clone();
                }
                else {
                    Mat newImg = analyser->MotionDetector(frame, previousFrame);//frame(faceRect).clone();
                    imshow("Motion", newImg);
                    previousFrame = frame.clone();
                }
            }

            //показать результат
            if (settings->showImage)
                showImage(frame, analyser->faceRect, analyser->eyes, firstFoundPupils);

            if (!settings->ifSearchFaceOneFrame)
                analyser->faceRect = Rect();

            if (settings->CalibrationType == settings->ct_emersion)
            {
                //поступил сигнал о новом положении шарика на мониторе
                if(fllagEmersionRoundCalib)
                {
                    fllagEmersionRoundCalib = false;
                    calibrationEmersion->gotoNextPos();
                }

                if(calibrationEmersion->DetectPointsL.count() != 0 && !flagStopEmersionRoundCalibSlot)
                {
                    calibrationEmersion->DetectPointsL.last().push_back(analyser->foundPupils[0]);
                    calibrationEmersion->DetectPointsR.last().push_back(analyser->foundPupils[1]);
                    logWrite(string("check. ") + "analyser->foundPupils[0] x " + func::to_string(analyser->foundPupils[0].x) + " analyser->foundPupils[0] y " + func::to_string(analyser->foundPupils[0].y ));
                    logWrite(string("check. ") + "analyser->foundPupils[1] x " + func::to_string(analyser->foundPupils[1].x) + " analyser->foundPupils[1] y " + func::to_string(analyser->foundPupils[1].y ));
                    //logWrite(string("check. ") + "eyeAvg x " + func::to_string(eyeAvg.x) + " eyeAvg y " + func::to_string(eyeAvg.y) );
                }
            }

            func::safeDelay(1);

            int key = waitKey(15);
            // нажата ESC
            if (key == 27) {
                captureIsWorking = false;
                logWrite("BTN ESC pressed");
                break;
            }
            // нажата space
            if (key == 32) {
                logWrite("BTN SPACE pressed");
                settings->test_count++;
            }
        }
        catch (Exception ex) {
            break;
        }
        if (isFullStop)
        {
            fullStop();
            return Failed;
        }
    }

    //завершение сохранения видео
    if (settings->saveCalibrationVideo)
        fileManager->videoWriter.release();

    //сохранение калибровки в файл (для быстрой загрузки без проходжения калибровки)
    //для загрузки - метод loadCalibration
    if (settings->eng_save_calibration)
        fileManager->textFileManager.saveCalibration();

    //TODO ...а зачем удалять камеру? пусть дальше работает
    //camera->release();

    //TODO перенести кучу кода в свой класс.
    if (settings->CalibrationType == settings->ct_emersion)
    {
        //вывести все точки
        if(settings->showImage2)
        {
            vector<Mat> myMats;
            for (int k = 0; k < calibrationEmersion->DetectPointsL.count() + 1; ++k)
            {
                myMats.push_back(analyser->foundfaceImg(analyser->eyes[0]).clone());
                if (k < calibrationEmersion->DetectPointsL.count())
                {
                    for (int i = 0; i < calibrationEmersion->DetectPointsL[k].count(); ++i)
                    {
                        line(myMats[k], calibrationEmersion->DetectPointsL[k][i], calibrationEmersion->DetectPointsL[k][i], Scalar(0, 255, 0));
                    }
                }
                else
                {
                    for (int i = 0; i < calibrationEmersion->DetectPointsAvgL.count(); ++i)
                    {
                        if (i == 0)
                        {
                            circle (myMats[k], calibrationEmersion->DetectPointsAvgL[i], 3, Scalar(0, 0, 0));
                        }

                        if (i == 1)
                        {
                            circle (myMats[k], calibrationEmersion->DetectPointsAvgL[i], 3, Scalar(225, 0, 0));
                        }
                        if (i == 2)
                        {
                            circle (myMats[k], calibrationEmersion->DetectPointsAvgL[i], 3, Scalar(0, 225, 0));
                        }
                        if (i == 3)
                        {
                            circle (myMats[k], calibrationEmersion->DetectPointsAvgL[i], 3, Scalar(0, 0, 225));
                        }
                        //line(myMats[k], calibrationEmersion->DetectPointsAvgL[k], DetectPointsAvgL->DetectPointsL[k], Scalar(0, 0, 0));
                    }
                }

                resize(myMats[k], myMats[k], Size(myMats[k].cols * 2, myMats[k].rows * 2));
                imshow(string("Left eye #") + func::to_string(k), myMats[k]);
            }
        }

//        if (settings->showImage)
//            namedWindow("Left eye", CV_WINDOW_NORMAL);

        if (analyser->eyes.size() != 2)
        {
            logWriteError("analyser->eyes.size() != 2");
            //TODO допилить логику выхода
            return Failed;
        }
        if (analyser->foundfaceImg.empty())
        {
            logWriteError("analyser->foundfaceImg.empty()");
            //TODO допилить логику выхода
            return Failed;
        }

        if(calibrationEmersion->DetectPointsL.count() < 4 ||
                calibrationEmersion->DetectPointsR.count() < 4)
        {
            calibrationDone = false;
            returnStatus = NotCorrect;
        }
        else {
            calibrationEmersion->InitDetectPointsAvg();
            if(calibrationEmersion->checkBadCalibrovka())
            {
                logWrite("checkBadCalibrovka returned false");
                calibrationDone = false;
                returnStatus = NotCorrect;
            }
            else
            {
                calibrationDone = true;
                returnStatus = Success;
            }
        }
        emit signalCheckBadCalibrovka(calibrationDone);

        if(!calibrationDone) return returnStatus;

        Mat leftImgTrajectory, rightImgTrajectory, rightImgTrajectory2;

        leftImgTrajectory = analyser->foundfaceImg(analyser->eyes[0]);
        rightImgTrajectory = analyser->foundfaceImg(analyser->eyes[1]);
        rightImgTrajectory2 = analyser->foundfaceImg(analyser->eyes[0]).clone();

        //выделить все точки
        QList<Scalar> colorLeft;
        colorLeft.push_back(Scalar(255, 0, 0));
        colorLeft.push_back(Scalar(255, 0, 238));
        colorLeft.push_back(Scalar(47, 0, 255));
        colorLeft.push_back(Scalar(0, 217, 255));
        colorLeft.push_back(Scalar(0, 255, 0));
        colorLeft.push_back(Scalar(255, 255, 0));
        colorLeft.push_back(Scalar(255, 136, 0));
        colorLeft.push_back(Scalar(176, 112, 255));
        colorLeft.push_back(Scalar(3, 92, 12));
        //Scalar colorLeft(0, 255, 0); //red

        for (int i = 0; i < settings->pointsCount; ++i)
        {
            line(leftImgTrajectory, calibrationEmersion->DetectPointsAvgL[i], calibrationEmersion->DetectPointsAvgL[i], colorLeft[i]);
            line(rightImgTrajectory, calibrationEmersion->DetectPointsAvgR[i], calibrationEmersion->DetectPointsAvgR[i], colorLeft[i]);
        }

        resize(leftImgTrajectory, leftImgTrajectory, Size(leftImgTrajectory.cols * 2, leftImgTrajectory.rows * 2));
        if (true)
            imshow("Left eye", leftImgTrajectory);
    }
    else if(settings->CalibrationType == settings->ct_motion)
    {
        calibrationDone = true;
        //корректировка найденных зрачков по сторонам, отбрасиваем выбросы
        calibrationMotion.motionCalibration_EliminateOutlyingBlips(analyser->leftPupils4, analyser->rightPupils4, analyser->pupilsLeft, analyser->pupilsRight);
        //закончить калибровку (посчитать CalibrationAVGEyeRect, CalibrationCols, CalibrationRows, показать результат калибровки)
        if (calibrationMotion.finishCalibration(analyser->leftPupils4, analyser->rightPupils4, analyser->eyes, analyser->pupilsLeft, analyser->pupilsRight, analyser->faceRect, analyser->foundfaceImg))
            returnStatus = Success;
        else
            returnStatus = NotCorrect;
    }

    //сохранение результата калибровки в файл
    if (settings->saveCoords)
    {
        fileManager->textFileManager.createNewCalibrationFile(mainWindow_width,mainWindow_height);
        //fileManager->textFileManager.createNewCalibrationFile(calibrationMotion.leftEye_calibrationRect, calibrationMotion.rightEye_calibrationRect);//fileManager->saveAnalysisInfoIntoFile();
    }
    destroyWindow("Video Stream");
    destroyWindow("Face");
    destroyWindow("Left eye");

    captureIsWorking = false;

    return returnStatus;
}

void EyeTracker::timeoutHandler()
{
    framesCount++;
    double fps = 1000.0 / (time.elapsed() / framesCount);
    logWrite("timeoutHandler. FPS = " + to_string(QString::number(fps, 'f', 2).toStdString()));
    if (framesCount > 1000)
    {
        framesCount = 0;
        time.restart();
    }

    if (!getNextFrame(frame))
    {
        fullStop();
        return;
    }

    //поворот фрейма
    flipImage(frame);

    //сохранение в файл
    if (settings->saveWorkProcessVideo)
        fileManager->videoWriter.write(frame);

    //анализ
    if (!analyser->analyseImage(frame, calibrationDone, calibrationMotion.directory))
    {
        //TODO: лучше обработать пропуском кадра, ничего не изменив
        //при этом отсчитывая кол-во воступающих кадров либо времени
        //если уже прошло много времени - fullStop()

        logWriteError("analyser->analyseImage returned false");
        //fullStop();

        return;
    }

    //показать движение (false)
    if (settings->videoMotion)
    {
        if (firstFrame) {
            firstFrame = false;
            previousFrame = frame.clone();
        }
        else {
            Mat newImg = analyser->MotionDetector(frame, previousFrame);//frame(faceRect).clone();
            imshow("Motion", newImg);
            previousFrame = frame.clone();
        }
    }

    //показать результат
    if (settings->showImage)
        showImage(frame, analyser->faceRect, analyser->eyes, firstFoundPupils);

    if (settings->InputType == settings->it_camera && calibrationDone)
    {
        if (settings->CalibrationType == settings->ct_motion)
        {
            if (analyser->pupilsLeft.size() != 0 && analyser->pupilsRight.size() != 0)
            {
                currentPoint = calibrationMotion.GetMonitorPoint(&analyser->pupilsLeft, &analyser->pupilsRight);
            }
        }
        else if (settings->CalibrationType == settings->ct_emersion)
        {
            if (analyser->foundPupils.size() != 0)
            {
                //logWrite("analyser->foundPupils0" + to_string(analyser->foundPupils[0]));
                //logWrite("analyser->foundPupils1" + to_string(analyser->foundPupils[1]));
                currentPoint = calibrationEmersion->GetMonitorPoint(analyser->foundPupils[0], analyser->foundPupils[1]);

                if (false)
                {
                    Mat faceImg_emersion, eyeImg_emersionL;
                    faceImg_emersion = frame(analyser->faceRect).clone();
                    eyeImg_emersionL = faceImg_emersion(analyser->eyes[0]).clone();

//                    line(eyeImg_emersionL, eyeAvg, eyeAvg, Scalar(0, 255, 0));
                    //analyser->foundfaceImg(analyser->eyes[0]);
                    for (int i = 0; i < settings->pointsCount; ++i)
                    {
                        line(eyeImg_emersionL, calibrationEmersion->DetectPointsAvgL[i], calibrationEmersion->DetectPointsAvgL[i], Scalar(0, 0, 255));
                    }
                    resize(eyeImg_emersionL, eyeImg_emersionL, Size(eyeImg_emersionL.cols * 2, eyeImg_emersionL.rows * 2));
                    imshow("eyeImg_emersionL", eyeImg_emersionL);
                }
            }
        }
    }

    //сохранение результата в файл
    if (settings->saveCoords)
    {
        fileManager->textFileManager.writeCoordsIntoFile(Point(analyser->foundPupils[0].x - calibrationMotion.leftEye_calibrationRect.x, analyser->foundPupils[0].y - calibrationMotion.leftEye_calibrationRect.y)
            , Point(analyser->foundPupils[1].x - calibrationMotion.rightEye_calibrationRect.x, analyser->foundPupils[1].y - calibrationMotion.rightEye_calibrationRect.y));

        logWrite(string("Coords: x ") + func::to_string(analyser->foundPupils[0].x - calibrationMotion.leftEye_calibrationRect.x)
            + "," + func::to_string(analyser->foundPupils[0].y - calibrationMotion.leftEye_calibrationRect.y)
            + " y " + func::to_string(analyser->foundPupils[1].x - calibrationMotion.rightEye_calibrationRect.x)
            + "," + func::to_string(analyser->foundPupils[1].y - calibrationMotion.rightEye_calibrationRect.y)
            );
        //                fileManager->textfileManager->writeCoordsIntoFile(analyser->foundPupils[0], analyser->foundPupils[1]);
    }

    if (!settings->ifSearchFaceOneFrame)
        analyser->faceRect = Rect();
    analyser->pupilsLeft.clear();
    analyser->pupilsRight.clear();
}

void EyeTracker::eventLoop()
{
    QEventLoop e_l;
    e_l.exec();
}

Point EyeTracker::GetCurrentPoint()
{
    return currentPoint;
}

bool EyeTracker::getNextFrame(Mat &frame)
{
    bool returnValue = true;
    if (settings->InputType == settings->it_image)
    {
        string dir = "BioID-FaceDatabase-V1.2/BioID_";
        if (settings->test_count < 10)
            dir+="000";
        else if (settings->test_count < 100)
            dir+="00";
        else if (settings->test_count < 1000)
            dir+="0";

        dir += to_string(settings->test_count);
        dir += ".pgm";

        settings->test_count++;
        //string dir = string("D:\\BioID-FaceDatabase-V1.2\\BioID_000" + (settings->test_count++)) + ".pgm";
        //string dir = "D:\\BioID-FaceDatabase-V1.2\\BioID_1520.pgm";
        frame = imread(dir);
        //frame = imread(settings->image_dir);
        imshow("input_frame",frame);
        //logWrite("read frame");
    }
    else if (settings->InputType == settings->it_camera)
    {
        if (!camera->read(frame)) {
            //end of media
            logWriteError("Камера отключена");
            returnValue = false;
        }
    }
    else if (settings->InputType == settings->it_videoFile)
    {
        if (!camera->read(frame)) {
            //end of media
            logWriteError("videoFile отсутствует или еще че, я хз");
            returnValue = false;
        }
    }

    return returnValue;
}

void EyeTracker::flipImage(Mat& frame)
{
//    // todo:
//    // попробывать настроить камеру на автоповорот
//    if (settings->flipImage == settings->flipImage90)
//    {
//        //camera->set(CV_CAP_PROP_TILT, 90.0);
//    }

    // переменные для поворота кадра
    Mat rotation_mat;
    Point2f frame_center;

    switch (settings->flipImage)
    {
    case Settings::flipImage180:
        flip(frame, frame, 0);
        break;
    case Settings::flipImage90:
        frame_center = Point2f(camera->get(CV_CAP_PROP_FRAME_WIDTH) / 2.0F, camera->get(CV_CAP_PROP_FRAME_HEIGHT) / 2.0F);
        rotation_mat = getRotationMatrix2D(frame_center, 90, 1.0);
        warpAffine(frame, frame, rotation_mat, frame.size());
        break;
    case Settings::flipImageM90:
        frame_center = Point2f(camera->get(CV_CAP_PROP_FRAME_WIDTH) / 2.0F, camera->get(CV_CAP_PROP_FRAME_HEIGHT) / 2.0F);
        rotation_mat = getRotationMatrix2D(frame_center, -90, 1.0);
        warpAffine(frame, frame, rotation_mat, frame.size());
        break;
    case Settings::flipImage0:
    default:
        break;
    }

    if (settings->flip_mirrorReflection)
        flip(frame, frame, 1);
}

void EyeTracker::fullStop()
{
    logWrite("EyeTracker FullStop");
    isFullStop = true;
    captureIsWorking = false;

    //disconnect(timer, SIGNAL(timeout()), this, SLOT(timeoutHandler()));
    //thread->quit();
    if(timer->isActive())
        timer->stop();

    //camera->release();

    //destroyWindow("Video Stream");
    //destroyWindow("Face");
}

void EyeTracker::showImage(Mat& frame, Rect& faceRect, vector<Rect>& eyes, int i)
{
#if 1
    //показать результаты
    if (faceRect.height > 0)
    {
        Mat faceImg;
        faceImg = frame(faceRect).clone();
        rectangle(faceImg, eyes[0], CV_RGB(0, 0, 255));
        rectangle(faceImg, eyes[1], CV_RGB(0, 0, 255));

        showPupils(faceImg, eyes, analyser->pupilsLeft[analyser->pupilsLeft.size() - 1], analyser->pupilsRight[analyser->pupilsRight.size() - 1]);

       // Mat ppp = faceImg(eyes[0]);
       // vector< vector<Point> > eyeLeftContours;
       // eyeLeftContours.push_back(analyser->faceFeatures->eyeLeftContour);
       // drawContours(ppp, eyeLeftContours, 0, Scalar(0,0,255), 2);

        rectangle(frame, faceRect, CV_RGB(0, 255, 0));
        imshow("Face", faceImg);
    }
    imshow("Video Stream", frame);
#else
    //показать кадр, на котором выделены лицо, глаза, зрачки

    Mat frameToShow = frame.clone();
    //показать результаты
    if (faceRect.height > 0)
    {
        Mat faceImg;
        faceImg = frameToShow(faceRect).clone();
        rectangle(faceImg, eyes[0], CV_RGB(0, 0, 255));
        rectangle(faceImg, eyes[1], CV_RGB(0, 0, 255));
        rectangle(frameToShow, Rect(eyes[0].x + faceRect.x,eyes[0].y + faceRect.y, eyes[0].height, eyes[0].height)  , CV_RGB(0, 0, 255));
        rectangle(frameToShow, Rect(eyes[1].x + faceRect.x,eyes[1].y + faceRect.y, eyes[1].height, eyes[1].height)  , CV_RGB(0, 0, 255));

        showPupils(faceImg, eyes, analyser->pupilsLeft[analyser->pupilsLeft.size() - 1], analyser->pupilsRight[analyser->pupilsRight.size() - 1]);

        rectangle(frameToShow, faceRect, CV_RGB(0, 255, 0));
        imshow("Face", faceImg);
    }
    imshow("Video Stream", frameToShow);
#endif
}

void EyeTracker::showPupils(Mat face, vector<Rect> eyes, Point leftPupil, Point rightPupil)
{
    //Отрисовка прямоугольников

    /*Rect leftRightCornerRegion(eyes[0]);
    leftRightCornerRegion.width -= leftPupil.x;
    leftRightCornerRegion.x += leftPupil.x;
    leftRightCornerRegion.height /= 2;
    leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
    Rect leftLeftCornerRegion(eyes[0]);
    leftLeftCornerRegion.width = leftPupil.x;
    leftLeftCornerRegion.height /= 2;
    leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
    Rect rightLeftCornerRegion(eyes[1]);
    rightLeftCornerRegion.width = rightPupil.x;
    rightLeftCornerRegion.height /= 2;
    rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
    Rect rightRightCornerRegion(eyes[1]);
    rightRightCornerRegion.width -= rightPupil.x;
    rightRightCornerRegion.x += rightPupil.x;
    rightRightCornerRegion.height /= 2;
    rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
    rectangle(face, leftRightCornerRegion, 200);
    rectangle(face, leftLeftCornerRegion, 200);
    rectangle(face, rightLeftCornerRegion, 200);
    rectangle(face, rightRightCornerRegion, 200);*/

    if (leftPupil.x != 0 || leftPupil.y != 0)
    {
        // изменить координаты центра зрачка относительно глаза к координатам относительно лица
        leftPupil.x += eyes[0].x;
        leftPupil.y += eyes[0].y;
        // нарисовать зрачки
        circle(face, leftPupil, 3, CV_RGB(100, 170, 230));
    }

    if (rightPupil.x != 0 || rightPupil.y != 0)
    {
        rightPupil.x += eyes[1].x;
        rightPupil.y += eyes[1].y;
        circle(face, rightPupil, 3, CV_RGB(100, 170, 230));
    }
    //imshow("Pupils", face);

    if (settings->showImage2)
    {
        Mat myMatL = face(eyes[0]).clone();
        Mat myMatR = face(eyes[1]).clone();
        for (int i = 0; i < calibrationEmersion->DetectPointsAvgL.count(); ++i)
        {
            if (i == 0)
            {
                circle (myMatL, calibrationEmersion->DetectPointsAvgL[i], 1, Scalar(0, 0, 0));
                circle (myMatR, calibrationEmersion->DetectPointsAvgR[i], 1, Scalar(0, 0, 0));
            }

            if (i == 1)
            {
                circle (myMatL, calibrationEmersion->DetectPointsAvgL[i], 1, Scalar(225, 0, 0));
                circle (myMatR, calibrationEmersion->DetectPointsAvgR[i], 1, Scalar(225, 0, 0));
            }
            if (i == 2)
            {
                circle (myMatL, calibrationEmersion->DetectPointsAvgL[i], 1, Scalar(0, 225, 0));
                circle (myMatR, calibrationEmersion->DetectPointsAvgR[i], 1, Scalar(0, 225, 0));
            }
            if (i == 3)
            {
                circle (myMatL, calibrationEmersion->DetectPointsAvgL[i], 1, Scalar(0, 0, 225));
                circle (myMatR, calibrationEmersion->DetectPointsAvgR[i], 1, Scalar(0, 0, 225));
            }
        }
        imshow("myMatL",myMatL);
        imshow("myMatR",myMatR);
    }

}

void EyeTracker::testBioIDSLot()
{
    //bioIDTest = new BioIDTest();
    bioIDTest.RunTest();
}

void EyeTracker::startEyeBlinkDetectorSlot()
{
    //clearVariables();

    blinksetIsWorking = true;

    // задержка в цикле
    //int delay = static_cast<int>(1000 / camera->get(CV_CAP_PROP_FPS));

    while (blinksetIsWorking)
    {
        if (isFullStop)
        {
            fullStop();
        }
        try {
            if (!camera->read(frame)) {
                //end of media
                logWriteError("Камера отключена");
                fullStop();
            }

            //поворот фрейма
            flipImage(frame);

            if (!analyser->analyseImage(frame, true))
            {
                logWriteError("startEyeBlinkDetectorSlot! analyser->analyseImage returned false");
            }

            //посчитать дистанцию между веками
            analyser->blinkDetector->calculateMinDistance();
            //посчитать Хэши закрытых глаз
            analyser->blinkDetector->calculateClosedEyesHashes(frame);
            //посчитать макс.значения зрачков на закрытых глазах
            analyser->blinkDetector->calculateValues(analyser->pupilsValues);

            func::safeDelay(1);
            int key = waitKey(15);

        }
        catch (Exception ex) {
            break;
        }
        if (isFullStop)
        {
            fullStop();
        }
    }

    analyser->blinkDetector->setBlinksetDone(true);
    analyser->blinkDetector->calculateValues_finish();

}

void EyeTracker::StopBlinkDetector()
{
    blinksetIsWorking = false;
}
