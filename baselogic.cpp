// BaseLogic.cpp : implementation file
//

#include "BaseLogic.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

BaseLogic::BaseLogic(Settings *settings) : QObject()
{
    this->settings = settings;
    InitBaseLogic();
}

BaseLogic::~BaseLogic()
{
}

bool BaseLogic::InitBaseLogic()
{
   eyetracker = new EyeTracker(settings, 800, 800);
   controlManager.setSettings(settings);

   //инициализировать таймер в своём потоке
   connect(this, SIGNAL(initEyeTrackerTimer()), eyetracker, SLOT(initTimer()));
   emit initEyeTrackerTimer();
   return true;
}

void BaseLogic::mainWindowWidthChangedSlot(int var)
{
    eyetracker->mainWindow_width = var;
}

void BaseLogic::mainWindowHeightChangedSlot(int var)
{
    eyetracker->mainWindow_height = var;
}

void BaseLogic::WorkProcess()
{
    logWrite("WorkProcess started");
    if(!eyetracker->cameraIsOpened()) {
        if (settings->InputType != settings->it_image)
        {
            // camera initialization
            if (!eyetracker->CameraInitialisation())
            {
                eyetracker->SetCaptureIsWorking(false);
                logWriteError("Capture is NULL. ");
                return;
            }
        }
    }

    controlManager.setMainWindowSize(eyetracker->mainWindow_width, eyetracker->mainWindow_height);
    controlManager.startCoordHandler();

    if (settings->CalibrationType == settings->ct_motion)
    {
        eyetracker->calibrationMotion.setMainWindowSize(eyetracker->mainWindow_width, eyetracker->mainWindow_height);
    }

    connect(this, SIGNAL(startEyeTracker()), eyetracker, SLOT(start()));
    emit startEyeTracker();

    //подождать отработку запуска потока.
    safeDelay(500);

    while (eyetracker->GetCaptureIsWorking())
    {
        //logWrite("WorkProcess QThread " + to_string((int)QThread::currentThreadId()));
        controlManager.handleCoords(eyetracker->GetCurrentPoint());

        safeDelay(eyetracker->camera->delay /* 33 */ );
    }

    logWrite("WorkProcess finished");
}

void BaseLogic::OnClose()
{
    eyetracker->SetCaptureIsWorking(false);
    func::safeDelay(600);
}

void BaseLogic::workProcessSlot()
{
    logWrite("workProcessSlot called");
    WorkProcess();
}

void BaseLogic::startCameraInitialisationSlot() {
    eyetracker->isFullStop = false;
    logWrite("startCameraInitialisationSlot called :)");
    logWrite("Инициализация камеры...");

    //в случае повторной калибровки
    //завершаем workprocess, если тот работал.
    eyetracker->SetCaptureIsWorking(false);
    func::safeDelay(1000);

    if(!eyetracker->cameraIsOpened())
        if (!eyetracker->CameraInitialisation())
        {
            emit cameraInitialisationFinishedSignal(false);
            return;
        }
    emit cameraInitialisationFinishedSignal(true);
}

void BaseLogic::startCalibrationSlot()
{
    logWrite("startCalibrationSlot called");
    startCalibration();
}

//TODO доделать реакцию интерфейса на неудачную калибровку
void BaseLogic::startCalibration()
{
    controlManager.clearVariables();
    logWrite("startCalibration called");
    int statusCalibration = eyetracker->startCalibration();
    switch (statusCalibration) {
    case 0: {
        logWrite("startCalibrationSlot finished!");
        WorkProcess();
        break;
    }
    case 1:
        logWrite("Калибровка Failed");
        break;
    case 2:
        logWrite("НЕКОРРЕКТНАЯ КАЛИБРОВКА! Пройдите калибровку заново");
        break;
    }
}

void BaseLogic::stopCalibrationSlot() {
    logWrite("stopCalibrationSlot called");
    eyetracker->SetCaptureIsWorking(false);
}

void BaseLogic::stop()
{
    logWrite("stop stop stop");
    eyetracker->SetCaptureIsWorking(false);
    emit finished();
    return;
}

void BaseLogic::getMonitorCoordCalibSlot(int x, int y)
{
    logWrite("getMonitorCoordCalibSlot call  :)");
    eyetracker->calibrationEmersion->PointsMonitor.push_back(Point(x, y));
}

void BaseLogic::fllagEmersionRoundCalibSlot()
{
    logWrite("fllagEmersionRoundCalibSlot call  :)");
    eyetracker->fllagEmersionRoundCalib = true;
    eyetracker->flagStopEmersionRoundCalibSlot = false;
}
void BaseLogic::fllagStopEmersionRoundCalibSlot()
{
    logWrite("fllagStopEmersionRoundCalibSlot call  :)");
    eyetracker->flagStopEmersionRoundCalibSlot = true;
}

void BaseLogic::test()
{
//    logWrite("> Инициализация видео...\r\n");
//    if (!captureInit(trackingParametrs.inputDevice)) {
//        logWrite("> Error: VideoCapture hasn't been initialized");
//        return;
//    }
//    //Thread_MessageCall(safeHwnd, NULL);

//    recorder->eyetracker->eyetracker->eyetracker->eyetracker->captureIsWorking = true;
//    Mat frame;
//    // переменные для поворота фрейма
//    Mat rotation_mat;
//    Point2f frame_center(capture.get(CV_CAP_PROP_FRAME_WIDTH) / 2.0F, capture.get(CV_CAP_PROP_FRAME_HEIGHT) / 2.0F);
//    Sleep(30);


//    while (recorder->eyetracker->eyetracker->eyetracker->eyetracker->captureIsWorking)
//    {
//        try {
//            if (!capture.read(frame))
//            {
//                logWrite("end of media");
//                //BaseLogic::changeButton(IDC_BTN_RECORD, threadData->BaseLogic);
//                break;
//            }

//            //поворот фрейма
//            switch (trackingParametrs.flipImage)
//            {
//            case TrackingParametrs::flipImage180:
//                flip(frame, frame, 0);
//                break;
//            case TrackingParametrs::flipImage90:
//                rotation_mat = getRotationMatrix2D(frame_center, 90, 1.0);
//                warpAffine(frame, frame, rotation_mat, frame.size());
//                break;
//            case TrackingParametrs::flipImageM90:
//                rotation_mat = getRotationMatrix2D(frame_center, -90, 1.0);
//                warpAffine(frame, frame, rotation_mat, frame.size());
//                break;
//            case TrackingParametrs::flipImage0:
//            default:
//                break;
//            }
//            //flip(frame, frame, 1);


//            Mat frame_hsv;
//            cvtColor(frame, frame_hsv, CV_BGR2HSV); //CV_RGB2HSV CV_BGR2HSV
//            vector<Mat> frame_hsv_channels;
//            split(frame_hsv, frame_hsv_channels);

//            frame_hsv_channels[0];
//            Mat dst;//, dst2;
//            threshold(frame_hsv_channels[0], dst, 115, 225, 4);
//            imshow("Video Stream1", dst);
//            //40-100
//            threshold(dst, dst, 70, 225, 0);
//            imshow("Video Stream2", dst);
//            //adaptiveThreshold(dst,dst2,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,11,2);
//            //adaptiveThreshold(dst,dst2,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,11,2);

//            int erosion_type = 0;
//            //#define  CV_SHAPE_RECT      0
//            //#define  CV_SHAPE_CROSS     1
//            //#define  CV_SHAPE_ELLIPSE   2

//            int erosion_size = 1; // до 21
//            Mat element = getStructuringElement(erosion_type,
//                                               Size(2*erosion_size + 1, 2*erosion_size+1),
//                                               Point(erosion_size, erosion_size));
//            erode(dst, dst, element);
//            dilate(dst, dst, element);

//            if (true)
//                imshow("Video Stream3", dst);
//            Sleep(15);
//            if (waitKey(15) == 27) {
//                //BaseLogic::changeButton(IDC_BTN_RECORD, threadData->BaseLogic);
//                //changeButton(IDC_BTN_RECORD, threadData->BaseLogic);
//                break;
//            }
//        }
//        catch (Exception ex) {
//            break;
//        }
//    }
//    destroyWindow("Video Stream");
//    capture.release();
//    return;

}

void BaseLogic::fullStop()
{
   logWrite("fullStop called.");
   eyetracker->fullStop();
}

void BaseLogic::selectedCameraChangedSlot(QString id)
{
    logWrite(id.toStdString());

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    foreach (const QCameraInfo &cameraInfo, cameras)
    {
        if (id == cameraInfo.deviceName())
            settings->inputDevice = cameras.indexOf(cameraInfo);
        logWrite(cameraInfo.deviceName().toStdString());
        logWrite("inputDevice is " + func::to_string(settings->inputDevice));
    }
}

void BaseLogic::stopEyeBlinkDetectorSlot()
{
    eyetracker->StopBlinkDetector();
    //WorkProcess();
}

