#include "ControlManager.h"

//static variables
int ControlManager::mainWindow_width;
int ControlManager::mainWindow_height;
cv::Point pt_projection(0,0), pt_projection_kalman(0,0);
int k_projection = 0;



void ControlManager::CalculateWidthImages()
{
    int countImages = 4;
    int widthImg = (mainWindow_width - mainWindow_width/16*1.5)/countImages - 3*(countImages-1);
    int heightImg = mainWindow_height/(countImages - 1) - 3*(countImages - 2);
    widthImage = (widthImg > heightImg) ? heightImg : widthImg;
    mainWindowWidthHalf = mainWindow_width/2;
    mainWindowHeightHalf = mainWindow_height/2;
}

ControlManager::ControlManager()
{
    CalculateWidthImages();
}

void ControlManager::clearVariables()
{
    timeStart = clock();
    vectorCoordsForLast2Sec.clear();
    vectorMenuBtnsForLast2Sec.clear();
}

void ControlManager::setSettings(Settings* sets)
{
    settings = sets;
}

void ControlManager::setMainWindowSize(int mainWindow_width, int mainWindow_height)
{
    this->mainWindow_width = mainWindow_width;
    this->mainWindow_height = mainWindow_height;
    logWrite("!!! mainWindow_width is" + func::to_string(mainWindow_width));
    CalculateWidthImages();
}

void ControlManager::startCoordHandler()
{
    timeStart = clock();
}

void ControlManager::handleCoords(Point point)
{
    //находим среднюю координату из всех точек, которые набрались за последние 2 сек.
    //выбирарется та кнопка, на которую смотрел чаще
    if (true)
    {
        //kalman filter
//        pt_projection = point;
//        if(k_projection == 0) {
//            init_kalman_projectionOnMonitor(pt_projection);
//            ++k_projection;
//        }
//        pt_projection_kalman = kalman_correct_projectionOnMonitor(pt_projection);
//        if(pt_projection_kalman.x == 0 && pt_projection_kalman.y == 0) {
//            k_projection=0;
//        }

        //point = pt_projection_kalman;
        vectorCoordsForLast2Sec.push_back(point);

        //рисуем найденную точку
        if (settings->projectionType == settings->pt_exact)
            drawPointOnMonitorSignal(point.x, point.y);

        if (clock() - timeStart > msec)
        {
            Point mean_point;
            for(vector<Point>::iterator it = vectorCoordsForLast2Sec.begin(); it != vectorCoordsForLast2Sec.end(); ++it)
                mean_point += (*it);

            mean_point.x /= vectorCoordsForLast2Sec.size();
            mean_point.y /= vectorCoordsForLast2Sec.size();

            //рисуем найденную точку
            if (settings->projectionType == settings->pt_smooth)
                drawPointOnMonitorSignal(mean_point.x, mean_point.y);

            vectorCoordsForLast2Sec.erase(vectorCoordsForLast2Sec.begin());

            int sq_num = getBTNnumber(mean_point);
            pressButtonSignal(QVariant(sq_num));
        }
    }
    //находим на какую кнопку посмотрел в каждом кадре
    //выбирарется та кнопка, на которую смотрел чаще
    else
    {
        vectorCoordsForLast2Sec.push_back(point);
        //обновление
        updateVectorMenuBtnsForLast2Sec();

        //рисуем найденную точку
        if(true)
            drawPointOnMonitorSignal(point.x, point.y);

        if (clock() - timeStart > msec)
        {
            vector<int> k(10, 0);
            for(vector<int>::iterator it = vectorMenuBtnsForLast2Sec.begin(); it != vectorMenuBtnsForLast2Sec.end(); ++it)
            {
                k[*it]++;
            }
            int sq_num = (max_element(k.begin(), k.end()) - k.begin());

            vectorCoordsForLast2Sec.erase(vectorCoordsForLast2Sec.begin());
            vectorMenuBtnsForLast2Sec.erase(vectorMenuBtnsForLast2Sec.begin());

            pressButtonSignal(QVariant(sq_num));

            //        if (sq_num != 0)
            //            logWrite("Нажата кнопка " + func::to_string(sq_num));

            //обнуление. через каждые 2 секунды данные удаляются и собираются заново.
            //новая иконка будет также выбрана через 2 секунды
            if (false)
            {
                timeStart = clock();
                vectorCoordsForLast2Sec.clear();
                vectorMenuBtnsForLast2Sec.clear();
            }
        }
    }

}

int ControlManager::getBTNnumber(Point point)
{
    int returnvalue = 0;

    //новый UI
    if(true) {
        //4 - btn_doctor
        if (point.x > (mainWindowWidthHalf - widthImage) &&
                point.x < (mainWindowWidthHalf + widthImage) &&
                point.y > (mainWindowHeightHalf - widthImage*1.5) &&
                point.y < (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 4;
        //1 - btn_phone
        else if (//point.x > (mainWindowWidthHalf - 2*widthImage) &&
                 point.x < (mainWindowWidthHalf - widthImage) &&
                 //point.y > (mainWindowHeightHalf - widthImage*1.5) &&
                 point.y < (mainWindowHeightHalf - widthImage*0.5))
            returnvalue = 1;
        //2 - btn_lamp
        else if (//point.x > (mainWindowWidthHalf - 2*widthImage) &&
                 point.x < (mainWindowWidthHalf - widthImage) &&
                 point.y > (mainWindowHeightHalf - widthImage*0.5) &&
                 point.y < (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 2;
        //3 - btn_speaker
        else if (//point.x > (mainWindowWidthHalf - 2*widthImage) &&
                 point.x < (mainWindowWidthHalf - widthImage) &&
                 point.y > (mainWindowHeightHalf + widthImage*0.5) //&&
                 //point.y > (mainWindowHeightHalf + widthImage*1.5)
                 )
            returnvalue = 3;
        //5 - btn_soup
        else if (point.x > (mainWindowWidthHalf - widthImage) &&
                 point.x < mainWindowWidthHalf &&
                 point.y > (mainWindowHeightHalf + widthImage*0.5) //&&
                 //point.y > (mainWindowHeightHalf + widthImage*1.5)
                 )
            returnvalue = 5;
        //6 - btn_drink
        else if (point.x > mainWindowWidthHalf &&
                 point.x < (mainWindowWidthHalf + widthImage) &&
                 point.y > (mainWindowHeightHalf + widthImage*0.5) //&&
                 //point.y > (mainWindowHeightHalf + widthImage*1.5)
                 )
            returnvalue = 6;
        //7 - rectangle_clock
    //    else if  (point.x > (mainWindowHeightHalf + widthImage) &&
    //              point.x < (mainWindowHeightHalf + 2*widthImage) &&
    //              point.y > (mainWindowHeightHalf - widthImage*1.5) &&
    //              point.y > (mainWindowHeightHalf - widthImage*0.5))
    //        vectorMenuBtnsForLast2Sec.push_back(7);
        //8 - btn_music
        else if (point.x > (mainWindowWidthHalf + widthImage) &&
                 //point.x < (mainWindowHeightHalf + 2*widthImage) &&
                 point.y > (mainWindowHeightHalf - widthImage*0.5) &&
                 point.y < (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 8;
         //9 - btn_monitor
        else if (point.x > (mainWindowWidthHalf + widthImage) &&
                 //point.x < (mainWindowHeightHalf + 2*widthImage) &&
                 point.y > (mainWindowHeightHalf + widthImage*0.5) //&&
                 //point.y > (mainWindowHeightHalf + widthImage*1.5)
                 )
            returnvalue = 9;
        else returnvalue = 0;
    }
    //старый UI
    else {

        if (point.x <= (mainWindowWidthHalf - widthImage*0.5) &&
                point.y <= (mainWindowHeightHalf - widthImage*0.5))
            returnvalue = 1;

//        else if (point.x > (mainWindowWidthHalf - widthImage*0.5) &&
//                 point.x < (mainWindowWidthHalf + widthImage*0.5) &&
//                 point.y < (mainWindowHeightHalf - widthImage*0.5))
//            vectorMenuBtnsForLast2Sec.push_back(2);

        else if (point.x >= (mainWindowWidthHalf + widthImage*0.5) &&
                 point.y <= (mainWindowHeightHalf - widthImage*0.5))
            returnvalue = 3;

        else if (point.x < (mainWindowWidthHalf - widthImage*0.5) &&
                 point.y > (mainWindowHeightHalf - widthImage*0.5)&&
                 point.y < (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 4;

        else if (point.x > (mainWindowWidthHalf - widthImage*0.5) &&
                 point.x < (mainWindowWidthHalf + widthImage*0.5) &&
                 point.y > (mainWindowHeightHalf - widthImage*0.5)&&
                 point.y < (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 5;

        else if (point.x > (mainWindowWidthHalf + widthImage*0.5) &&
                 point.y > (mainWindowHeightHalf - widthImage*0.5)&&
                 point.y < (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 6;


        else if (point.x < (mainWindowWidthHalf - widthImage*0.5) &&
                 point.y > (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 7;

        else if (point.x > (mainWindowWidthHalf - widthImage*0.5) &&
                 point.x < (mainWindowWidthHalf + widthImage*0.5) &&
                 point.y > (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 8;

        else if (point.x > (mainWindowWidthHalf + widthImage*0.5) &&
                 point.y > (mainWindowHeightHalf + widthImage*0.5))
            returnvalue = 9;
        else returnvalue = 0;
    }
    return returnvalue;
}

void ControlManager::updateVectorMenuBtnsForLast2Sec()
{
    int btn = getBTNnumber(vectorCoordsForLast2Sec.back());
    vectorMenuBtnsForLast2Sec.push_back(btn);
}
