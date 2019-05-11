#include "Settings.h"

#include <QtXml/QDomDocument>
#include <QFile>

unsigned int Settings::CalibrationType = Settings::ct_emersion;
bool Settings::showImage = false;
unsigned int Settings::pointsCount = 4;
unsigned int Settings::flipImage = Settings::flipImage0;
bool Settings::flip_mirrorReflection = false;

Settings::Settings() : QObject()
{
    setDefaultSets();
    loadSettings("settings.xml");
}

Settings::~Settings()
{
}

bool Settings::setDefaultSets()
{
    faceDetectionMethod = fdm_HaarMethod;
    eyesDetectionMethod = edm_MarkMethod;
    pupilsDetectionMethod = pdm_GradientMethod;
    TrajectoryType = tt_motionTrajectory;

    coordsCorrectionViaTemplates = true;
    coordsCorrectionViaPupilsDistance = false;

    autoCalculateCameraFPS = true;
    ifSearchFaceOneFrame = true;
    videoTrajectory = false;
    videoMotion = false;
    saveCoords = false;
    changeSizeWindow = true;

    koffMotion = 1;
    Step = 2;
    GradientThreshold = 0.75;
    kFastEyeWidth = 50;

    inputDevice = CV_CAP_ANY;
    inputFileName = QString("My video.avi");
    inputImage = QString("photo.jpg");

    NumberHeightForTrajectory = 3;
    NumberLengthForTrajectory = 4;

    test_count = 0;

    //ENGINEERING MODE
    InputType = it_camera;

    eng_save_calibration = false;
    showImage2 = false;

    saveCalibrationVideo = false;
    saveWorkProcessVideo = false;

    projectionType = pt_smooth;

    return true;
}

bool Settings::loadSettings(QString filename)
{
    bool result = false;
    QFile file(filename);
    result = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!result)
        return false;

    QDomDocument doc("settings");
    QString error_str;
    int error_line, error_column;
    if(!doc.setContent(&file, true, &error_str, &error_line, &error_column))
        return false;

    QDomNode node = doc.firstChild();
    if(!node.isElement())
        return false;

    //переводим QDomNode <settings> в Элемент, чтобы обращаться к его элементам
    QDomElement root = node.toElement();
    if(root.tagName() != QString("settings"))
        return false;

    node = node.firstChild();
    while(!node.isNull())
    {
        QDomElement element = node.toElement();
        if(!element.isNull())
        {
            if(element.tagName() == QString("faceDetectionMethod"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    faceDetectionMethod = value2;
            }
            else if(element.tagName() == QString("eyesDetectionMethod"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    eyesDetectionMethod = value2;
            }
            else if(element.tagName() == QString("pupilsDetectionMethod"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    pupilsDetectionMethod = value2;
            }
            else if(element.tagName() == QString("TrajectoryType"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    TrajectoryType = value2;
            }
            else if(element.tagName() == QString("InputType"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    InputType = value2;
            }
            else if(element.tagName() == QString("CalibrationType"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    CalibrationType = value2;
            }
            else if(element.tagName() == QString("showImage"))
            {
                if (element.text() == "0")
                    showImage = false;
                else if (element.text() == "1")
                    showImage = true;
            }
            else if(element.tagName() == QString("autoCalculateCameraFPS"))
            {
                if (element.text() == "0")
                    autoCalculateCameraFPS = false;
                else if (element.text() == "1")
                    autoCalculateCameraFPS = true;
            }
            else if(element.tagName() == QString("coordsCorrectionViaTemplates"))
            {
                if (element.text() == "0")
                    coordsCorrectionViaTemplates = false;
                else if (element.text() == "1")
                    coordsCorrectionViaTemplates = true;
            }
            else if(element.tagName() == QString("coordsCorrectionViaPupilsDistance"))
            {
                if (element.text() == "0")
                    coordsCorrectionViaPupilsDistance = false;
                else if (element.text() == "1")
                    coordsCorrectionViaPupilsDistance = true;
            }
            else if(element.tagName() == QString("ifSearchFaceOneFrame"))
            {
                if (element.text() == "0")
                    ifSearchFaceOneFrame = false;
                else if (element.text() == "1")
                    ifSearchFaceOneFrame = true;
            }
            else if(element.tagName() == QString("videoTrajectory"))
            {
                if (element.text() == "0")
                    videoTrajectory = false;
                else if (element.text() == "1")
                    videoTrajectory = true;
            }
            else if(element.tagName() == QString("videoMotion"))
            {
                if (element.text() == "0")
                    videoMotion = false;
                else if (element.text() == "1")
                    videoMotion = true;
            }
            else if(element.tagName() == QString("saveCoords"))
            {
                if (element.text() == "0")
                    saveCoords = false;
                else if (element.text() == "1")
                    saveCoords = true;
            }
            else if(element.tagName() == QString("changeSizeWindow"))
            {
                if (element.text() == "0")
                    changeSizeWindow = false;
                else if (element.text() == "1")
                    changeSizeWindow = true;
            }
            else if(element.tagName() == QString("koffMotion"))
            {
                bool ok;
                double value2 = element.text().toDouble(&ok);
                if(ok)
                    koffMotion = value2;
            }
            else if(element.tagName() == QString("Step"))
            {
                bool ok;
                double value2 = element.text().toDouble(&ok);
                if(ok)
                    Step = value2;
            }
            else if(element.tagName() == QString("GradientThreshold"))
            {
                bool ok;
                double value2 = element.text().toDouble(&ok);
                if(ok)
                    GradientThreshold = value2;
            }
            else if(element.tagName() == QString("kFastEyeWidth"))
            {
                bool ok;
                double value2 = element.text().toDouble(&ok);
                if(ok)
                    kFastEyeWidth = value2;
            }
            else if(element.tagName() == QString("NumberLengthForTrajectory"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    NumberLengthForTrajectory = value2;
            }
            else if(element.tagName() == QString("NumberHeightForTrajectory"))
            {
                bool ok;
                unsigned int value2 = element.text().toUInt(&ok);
                if(ok)
                    NumberHeightForTrajectory = value2;
            }
            else if(element.tagName() == QString("inputDevice"))
            {
                bool ok;
                int value2 = element.text().toInt(&ok);
                if(ok)
                    inputDevice = value2;
            }
            else if(element.tagName() == QString("inputFileName"))
            {
                inputFileName = element.text();
            }
            else if(element.tagName() == QString("inputImage"))
            {
                inputImage = element.text();
            }
            else if(element.tagName() == QString("test_count"))
            {
                bool ok;
                int value2 = element.text().toInt(&ok);
                if(ok)
                    test_count = value2;
            }
            else if(element.tagName() == QString("pointsCount"))
            {
                bool ok;
                uint value2 = element.text().toUInt(&ok);
                if(ok)
                    pointsCount = value2;
            }
            else if(element.tagName() == QString("flipImage"))
            {
                bool ok;
                uint value2 = element.text().toUInt(&ok);
                if(ok)
                    flipImage = value2;
            }
            else if(element.tagName() == QString("flip_mirrorReflection"))
            {
                if (element.text() == "0")
                    flip_mirrorReflection = false;
                else if (element.text() == "1")
                    flip_mirrorReflection = true;
            }
            else if(element.tagName() == QString("eng_save_calibration"))
            {
                if (element.text() == "0")
                    eng_save_calibration = false;
                else if (element.text() == "1")
                    eng_save_calibration = true;
            }
            else if(element.tagName() == QString("saveCalibrationVideo"))
            {
                if (element.text() == "0")
                    saveCalibrationVideo = false;
                else if (element.text() == "1")
                    saveCalibrationVideo = true;
            }
            else if(element.tagName() == QString("saveWorkProcessVideo"))
            {
                if (element.text() == "0")
                    saveWorkProcessVideo = false;
                else if (element.text() == "1")
                    saveWorkProcessVideo = true;
            }
            else if(element.tagName() == QString("projectionType"))
            {
                bool ok;
                uint value2 = element.text().toUInt(&ok);
                if(ok)
                    projectionType = value2;
            }
            else if(element.tagName() == QString("showImage2"))
            {
                bool ok;
                uint value2 = element.text().toUInt(&ok);
                if(ok)
                    showImage2 = value2;
            }
        }
        node = node.nextSibling();
    }

    file.close();

    result = true;
    return result;
}

bool Settings::saveSettings(QString filename)
{
    QFile xml_file(filename);
    if(xml_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered))
    {
        QDomDocument doc("settings");
        QDomElement tag;
        QDomElement root = doc.createElement("settings");
        doc.appendChild(root);
        tag = doc.createElement("faceDetectionMethod");
        tag.appendChild(doc.createTextNode(QString::number(faceDetectionMethod)));
        root.appendChild(tag);
        tag = doc.createElement("eyesDetectionMethod");
        tag.appendChild(doc.createTextNode(QString::number(eyesDetectionMethod)));
        root.appendChild(tag);
        tag = doc.createElement("pupilsDetectionMethod");
        tag.appendChild(doc.createTextNode(QString::number(pupilsDetectionMethod)));
        root.appendChild(tag);
        tag = doc.createElement("TrajectoryType");
        tag.appendChild(doc.createTextNode(QString::number(TrajectoryType)));
        root.appendChild(tag);
        tag = doc.createElement("InputType");
        tag.appendChild(doc.createTextNode(QString::number(InputType)));
        root.appendChild(tag);
        tag = doc.createElement("CalibrationType");
        tag.appendChild(doc.createTextNode(QString::number(CalibrationType)));
        root.appendChild(tag);
        tag = doc.createElement("showImage");
        tag.appendChild(doc.createTextNode(QString::number(showImage)));
        root.appendChild(tag);
        tag = doc.createElement("autoCalculateCameraFPS");
        tag.appendChild(doc.createTextNode(QString::number(autoCalculateCameraFPS)));
        root.appendChild(tag);
        tag = doc.createElement("coordsCorrectionViaTemplates");
        tag.appendChild(doc.createTextNode(QString::number(coordsCorrectionViaTemplates)));
        root.appendChild(tag);
        tag = doc.createElement("coordsCorrectionViaPupilsDistance");
        tag.appendChild(doc.createTextNode(QString::number(coordsCorrectionViaPupilsDistance)));
        root.appendChild(tag);
        tag = doc.createElement("ifSearchFaceOneFrame");
        tag.appendChild(doc.createTextNode(QString::number(ifSearchFaceOneFrame)));
        root.appendChild(tag);
        tag = doc.createElement("videoTrajectory");
        tag.appendChild(doc.createTextNode(QString::number(videoTrajectory)));
        root.appendChild(tag);
        tag = doc.createElement("videoMotion");
        tag.appendChild(doc.createTextNode(QString::number(videoMotion)));
        root.appendChild(tag);
        tag = doc.createElement("saveCoords");
        tag.appendChild(doc.createTextNode(QString::number(saveCoords)));
        root.appendChild(tag);
        tag = doc.createElement("changeSizeWindow");
        tag.appendChild(doc.createTextNode(QString::number(changeSizeWindow)));
        root.appendChild(tag);
        tag = doc.createElement("koffMotion");
        tag.appendChild(doc.createTextNode(QString::number(koffMotion)));
        root.appendChild(tag);
        tag = doc.createElement("Step");
        tag.appendChild(doc.createTextNode(QString::number(Step)));
        root.appendChild(tag);
        tag = doc.createElement("GradientThreshold");
        tag.appendChild(doc.createTextNode(QString::number(GradientThreshold)));
        root.appendChild(tag);
        tag = doc.createElement("kFastEyeWidth");
        tag.appendChild(doc.createTextNode(QString::number(kFastEyeWidth)));
        root.appendChild(tag);
        tag = doc.createElement("NumberLengthForTrajectory");
        tag.appendChild(doc.createTextNode(QString::number(NumberLengthForTrajectory)));
        root.appendChild(tag);
        tag = doc.createElement("NumberHeightForTrajectory");
        tag.appendChild(doc.createTextNode(QString::number(NumberHeightForTrajectory)));
        root.appendChild(tag);
        tag = doc.createElement("inputDevice");
        tag.appendChild(doc.createTextNode(QString::number(inputDevice)));
        root.appendChild(tag);

        tag = doc.createElement("inputFileName");
        tag.appendChild(doc.createTextNode(inputFileName));
        root.appendChild(tag);

        tag = doc.createElement("inputImage");
        tag.appendChild(doc.createTextNode(inputImage));
        root.appendChild(tag);

        tag = doc.createElement("test_count");
        tag.appendChild(doc.createTextNode(QString::number(test_count)));
        root.appendChild(tag);
        tag = doc.createElement("pointsCount");
        tag.appendChild(doc.createTextNode(QString::number(pointsCount)));
        root.appendChild(tag);
        tag = doc.createElement("flipImage");
        tag.appendChild(doc.createTextNode(QString::number(flipImage)));
        root.appendChild(tag);
        tag = doc.createElement("flip_mirrorReflection");
        tag.appendChild(doc.createTextNode(QString::number(flip_mirrorReflection)));
        root.appendChild(tag);
        tag = doc.createElement("eng_save_calibration");
        tag.appendChild(doc.createTextNode(QString::number(eng_save_calibration)));
        root.appendChild(tag);
        tag = doc.createElement("saveCalibrationVideo");
        tag.appendChild(doc.createTextNode(QString::number(saveCalibrationVideo)));
        root.appendChild(tag);
        tag = doc.createElement("saveWorkProcessVideo");
        tag.appendChild(doc.createTextNode(QString::number(saveWorkProcessVideo)));
        root.appendChild(tag);
        tag = doc.createElement("projectionType");
        tag.appendChild(doc.createTextNode(QString::number(projectionType)));
        root.appendChild(tag);
        tag = doc.createElement("showImage2");
        tag.appendChild(doc.createTextNode(QString::number(showImage2)));
        root.appendChild(tag);

        QString ascii_str = doc.toString().toUtf8();
        xml_file.write(ascii_str.toStdString().c_str());
        xml_file.close();
    }

    return true;
}
