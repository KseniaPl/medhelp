#include "Camera.h"

Camera::Camera(Settings *settings)
{
    this->settings = settings;
    fps = 30;
    delay = 33;
}

void Camera::Init()
{
    width = get(CV_CAP_PROP_FRAME_WIDTH);
    height = get(CV_CAP_PROP_FRAME_HEIGHT);
    safeDelay(30);
    frame_center = Point2f(width / 2.0F, height / 2.0F);
}

void Camera::calculateFPS()
{
    fps = 0;
    double dWidth = get(CV_CAP_PROP_FRAME_WIDTH);
    double dHeight = get(CV_CAP_PROP_FRAME_HEIGHT);

    // WARNINIG Усечение!
    // int cameraFPS = static_cast<int>(capture.get(CV_CAP_PROP_FPS));
    // if camera doesn't support capture.get(CV_CAP_PROP_FPS) method:

    // считаем, с какой скоростью работает камера, для того
    // чтобы видео записалось с нормальный fps. (не было ускоренным)

    // переменная, отвечающая за Уменьшение длинны-ширины размера кадра для увеличения скорости.
    // Если cameraFPS < 30, то размер единожды уменьшается, cameraFPS пересчитывается
    bool changeRatioOnce = false;

    if (settings->autoCalculateCameraFPS)
    {
        while (fps < 30)
        {
            unsigned int i = 0;
            Mat frame;
            QTime time;
            while (i < 10)
            {
                read(frame);
                i++;
            }
            time.start();
            while (i < 30) {
                read(frame);
                i++;
                waitKey(10);
            }
            int difference = time.elapsed();
            delay = static_cast<int>(difference / 20);
            fps = static_cast<int>((1000 / delay) + 0.5);
            if (!changeRatioOnce)
                break;
            if (fps < 30)
            {
                dWidth /= 2;
                dHeight /= 2;
                set(CV_CAP_PROP_FRAME_WIDTH, dWidth);
                set(CV_CAP_PROP_FRAME_HEIGHT, dHeight);
                changeRatioOnce = false;
            }
        }
    }
    else
        fps = 30;
    logWrite("fps is " + to_string(fps));
    if (fps <= 0)
        fps = 30;
}

bool Camera::preliminaryTest(int frames = 30)
{
    //разгон видеокамеры
    Mat testFrame;
    while (frames>0)
    {
        if (!read(testFrame))
        {
            return false;
        }
        frames--;
    }
    return true;
}

