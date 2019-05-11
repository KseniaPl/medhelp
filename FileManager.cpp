#include "FileManager.h"

//static variables
string FileManager::VideoFileName;
VideoWriter FileManager::videoWriter;

FileManager::FileManager(): QObject()
{
}

FileManager::FileManager(Settings *settings) : QObject()
{
    this->settings = settings;
    textFileManager.setSettings(settings);
}

bool FileManager::videoRecordInit(Camera* capture, bool autoCalculateCameraFPS = true)
{
    double dWidth = capture->get(CV_CAP_PROP_FRAME_WIDTH);
    double dHeight = capture->get(CV_CAP_PROP_FRAME_HEIGHT);

    Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

    //VideoWriter
    VideoFileName = "My video";
    QString fileExt = ".txt";
    QString path = QString::fromStdString(VideoFileName) + fileExt;
    for (int i = 1;;++i)
    {
        QFileInfo checkFile(path);
        if (!(checkFile.exists() && checkFile.isFile()))
            break;

        path = QString::fromStdString(VideoFileName)  + QString::number(i) + fileExt;
    }
    VideoFileName = path.toStdString();

    videoWriter.open(VideoFileName, CV_FOURCC('D', 'I', 'V', 'X'), capture->fps, frameSize, true);
    if (!videoWriter.isOpened())
        return false;

    logWrite("Запись производится в видео " + VideoFileName);
    return true;
}

string TextFileManager::textFileName;

TextFileManager::TextFileManager()
{
}

void TextFileManager::setSettings(Settings *settings)
{
    this->settings = settings;
}

bool TextFileManager::createNewCalibrationFile(int mainWindow_width, int mainWindow_height)
{
    logWrite("createNewCalibrationFile started");

    linesCounter = 0;

    textFileName = "Coords_w_" +func::to_string(mainWindow_width) + "_h_" + func::to_string(mainWindow_height);

    QString fileExt = ".txt";
    QString path = QString::fromStdString(textFileName) + fileExt;

    for (int i = 1;;++i)
    {
        QFileInfo checkFile(path);
        if (!(checkFile.exists() && checkFile.isFile()))
            break;

        path = QString::fromStdString(textFileName)  + QString::number(i) + fileExt;
    }
    textFileName = path.toStdString();
    logWrite("textFileName is " + textFileName);
    out_stream.open(textFileName.c_str());
    return true;
}

bool TextFileManager::createNewCalibrationFile(Rect leftEye_calibrationRect, Rect rightEye_calibrationRect)
{
    logWrite("createNewCalibrationFile started");

    linesCounter = 0;

    textFileName = "left_x_[0," + func::to_string(leftEye_calibrationRect.width) + "]_y_[0,"+ func::to_string(leftEye_calibrationRect.height) +
            "]_right_x_[0," +func::to_string(rightEye_calibrationRect.width) + "]_y_[0," + func::to_string(rightEye_calibrationRect.height) + "]";

    QString fileExt = ".txt";
    QString path = QString::fromStdString(textFileName)  + fileExt;

    for (int i = 1;;++i)
    {
        QFileInfo checkFile(path);
        if (!(checkFile.exists() && checkFile.isFile()))
            break;

        path = QString::fromStdString(textFileName) + QString::number(i) + fileExt;
    }
    textFileName = path.toStdString();
    logWrite("textFileName is " + textFileName);
    out_stream.open(textFileName.c_str());

    return true;
}
bool TextFileManager::finishSavingIntoFile()
{
    out_stream.close();
    return true;
}

bool TextFileManager::writeCoordsIntoFile(Point monitorPoint)
{
    linesCounter++;
    out_stream << func::to_string(linesCounter) << ","
               << QDateTime::currentDateTime().toString("HH:mm:ss").toStdString()
               << func::to_string(monitorPoint.x) << ","
               << func::to_string(monitorPoint.y) << ","
               << endl;
    return true;
}

bool TextFileManager::writeCoordsIntoFile(Point p1, Point p2)
{
    linesCounter++;

    //    fileOut.open(filename, ios::app);// ios::app - открывает файл для записи в конец файла

    out_stream << func::to_string(linesCounter) << ","
               << func::to_string(p1.x) << ","
               << func::to_string(p1.y) << ","
               << func::to_string(p2.x) << ","
               << func::to_string(p2.y) << endl;
    //out_stream.close();

    return true;
}


void TextFileManager::saveAnalysisInfoIntoFile()
{
//    if (settings->saveCoords)
//    {
//        vector<vector<vector<int>>> myarray(CalibrationCols, vector<vector<int>>(CalibrationRows));

//        for (int i = 0; i < pupilAvg.size(); i++)
//            myarray[pupilAvg[i].x][pupilAvg[i].y].push_back(i);

//        //проверяем имя файла. итог - filename
//        QString str = "Result 1", filename = "Result 1.txt";
//        int counter = 1, cnt = 1;

//        while (CFileFind().FindFile(filename)) {
//            QString temp;
//            counter++;
//            temp.Format("%d", counter);
//            str.Delete(str.GetLength() - cnt, cnt);
//            if (temp[temp.GetLength() - 1] == '0' || cnt == 0)
//            {
//                if (cnt == 0 || cnt == 1)
//                    cnt++;
//            }
//            str += temp;
//            filename = str;
//            filename += ".txt";
//        }

//        ofstream out_stream(filename);

//        time_t seconds = time(NULL);
//        tm* timeinfo = localtime(&seconds);
//        out_stream << asctime(timeinfo) << endl;

//        //time_t t;
//        //t = time(0);
//        //out_stream << puts(ctime(&t));

//        out_stream << "Информация о траектории записана в следующем формате:\n"
//            << "№квадрата[строка][столбец] номера кадров, на которых взгляд был направлен на данный квадрат.\n"
//            << "\nВсего строк:" << CalibrationRows << ", столбцов:" << CalibrationCols
//            << "\nКадров в секунду = " << capture->get(CV_CAP_PROP_FPS)
//            <<"\nВремя на 1 кадр (мс) = " << static_cast<int>(1000 / capture->get(CV_CAP_PROP_FPS)) << endl;

//        /*for (int i = 0; i < myarray.size(); i++)
//        {
//        for (int j = 0; j < myarray[i].size(); j++)
//        {*/

//        for (int i = 0; i < CalibrationRows; i++)
//        {
//            for (int j = 0; j < CalibrationCols; j++)
//            {
//                out_stream << "\nКв.[" << i << "][" << j << "] ";

//                for (int k = 0; k < myarray[j][i].size(); k++)
//                {
//                    if (k != 0)
//                        out_stream << ", ";

//                    out_stream << myarray[j][i][k];
//                }
//            }
//        }

//        logWrite("> Информация об анализе схранена в файл ");
//        filename += "\r\n";
//        logWrite(filename);
//        out_stream.close();
//    }
}



void TextFileManager::saveCalibration()
{
//    //проверяем имя файла. итог - filename
//    QString str = "ENG_Calibration 1", filename = "ENG_Calibration 1.txt";
//    int counter = 1, cnt = 1;

//    while (CFileFind().FindFile(filename)) {
//        QString temp;
//        counter++;
//        temp.Format("%d", counter);
//        str.Delete(str.GetLength() - cnt, cnt);
//        if (temp[temp.GetLength() - 1] == '0' || cnt == 0)
//        {
//            if (cnt == 0 || cnt == 1)
//                cnt++;
//        }
//        str += temp;
//        filename = str;
//        filename += ".txt";
//    }
//    ofstream out_stream(filename);
//    out_stream << pupilsLeft.size() << endl;
//    out_stream << pupilsRight.size() << endl;

//    for (int i = 0; i < pupilsLeft.size(); i++)
//    {
//        out_stream << pupilsLeft[i].x << endl;
//        out_stream << pupilsLeft[i].y << endl;
//    }
//    for (int i = 0; i < pupilsRight.size(); i++)
//    {
//        out_stream << pupilsRight[i].x << endl;
//        out_stream << pupilsRight[i].y << endl;
//    }

//    logWrite("> ENG Информация о калибровке схранена в файл ");
//    filename += "\r\n";
//    logWrite(filename);
//    out_stream.close();
}


void TextFileManager::loadCalibration()
{
//    QFile file("ENG_Calibration.txt");
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        logWrite("loadCalibration. Не могу загрузить Калибровку");
//        return;
//    }

//    QTextStream in(&file);
//    QString str1;
//    QString str2;
//    if (!in.atEnd()) {
//        str1 = in.readLine();
//        str2 = in.readLine();
//    }
//    else {
//        logWrite("loadCalibration. End of file");
//        return;
//    }

//    int countL = str1.toInt();// std::stoi(str1);
//    int countR = str2.toInt();//std::stoi(str2);

//    pupilsLeft.clear();
//    pupilsRight.clear();

//    while (!in.atEnd() && countL > 0)
//    {
//        str1 = in.readLine();
//        str2 = in.readLine();

//        Point pupil(str1.toInt(), str2.toInt());
//        pupilsLeft.push_back(pupil);
//        countL--;
//    }
//    while (!in.atEnd() && countR > 0)
//    {
//        str1 = in.readLine();
//        str2 = in.readLine();

//        Point pupil(str1.toInt(), str2.toInt());
//        pupilsRight.push_back(pupil);
//        countL--;
//    }


//    FirstCalibration = false;
//    leftEyeRect = getEyeRect(pupilsLeft);
//    rightEyeRect = getEyeRect(pupilsRight);
//    pupilAvg = countAveregePupilCoords();
//    CalibrationAVGEyeRect = avgEyeRect;
//    CalibrationCols = static_cast<int>(avgEyeRect.width + 1);
//    CalibrationRows = static_cast<int>(avgEyeRect.height + 1);
//    OnBnClickedBtnTrajectory();

//    logWrite("> ENG Калибровка загружена");
}
