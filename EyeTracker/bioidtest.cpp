#include "bioidtest.h"

#include <QDir>
#include <QStringList>
#include <QTextStream>


void BioIDTest::RunTest()
{
    Settings* settings = new Settings();
    Analyser* analyser = new Analyser(settings);
    if (!analyser->MethodsInit())
    {
        logWriteError("BioIDTest::RunTest() analyser->MethodsInit returned false");
        return;
    }

    vector<ResultPupils> resulFactPupils;
    vector<ResultPupils> resulTheorPupils;
    Mat frame;
    QStringList filters;
    QFileInfoList list;

    settings->InputType = settings->it_image;
    settings->ifSearchFaceOneFrame = false;
    settings->coordsCorrectionViaTemplates = false;

    filters << "*.pgm";
    QDir dir(QDir::currentPath() + "/BioID-FaceDatabase-V1.2");
    dir.setNameFilters( filters );
    list = dir.entryInfoList();

    //распознаем зрачки на базе и запис. получаем фактические результаты
    for(int i = 0; i < list.size(); i++)
    {
        std::string fileName = "BioID-FaceDatabase-V1.2/" + list.at(i).fileName().toStdString();
        frame = imread(fileName);
        if (!analyser->analyseImage(frame, false, 0))
        {
            continue;
        }

        vector<Point> foundPoints = analyser->GetFoundPupilsCoordsFrame();
        resulFactPupils.push_back(ResultPupils(foundPoints[0], foundPoints[1], i));

        emit progressChangedTestBioIDSignal(i);
    }

    //тянем теоретические координаты из базы
    filters.clear();
    filters << "*.eye";
    dir.setNameFilters(filters);
    list = dir.entryInfoList();
    for(int i = 0; i < list.size(); i++)
    {
        QString fileName = "BioID-FaceDatabase-V1.2/" + list.at(i).fileName();
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            logWriteError("Error opening file!");
                continue;
        }
        file.readLine();
        QString line = file.readLine();
        QStringList listStrings = line.split(QRegExp("\t|\n"), QString::SkipEmptyParts); //\\t
        resulTheorPupils.push_back(ResultPupils(Point(listStrings[0].toInt(), listStrings[1].toInt()),
                Point(listStrings[2].toInt(), listStrings[3].toInt()), i));
        file.close();

        emit progressChangedTestBioIDSignal(i);
    }

    //запись результатов в один файл
    QFile res("result.txt");
    res.open((QIODevice::WriteOnly)| QIODevice::Text);
    res.write("LXT\tLYT\tRXT\tRYT\t\tLXF\tLYF\tRXF\tRYF\n");

    int counter = 0;
    for(int i = 0; i < list.size(); i++)
    {
        if(resulFactPupils[counter].id != resulTheorPupils[i].id)
            continue;

        QTextStream out(&res);
        out << resulTheorPupils[i].leftPoint.x << "\t"
                               << resulTheorPupils[i].leftPoint.y << "\t"
                               << resulTheorPupils[i].rightPoint.x << "\t"
                               << resulTheorPupils[i].rightPoint.y << "\t\t"
                               << resulFactPupils[counter].leftPoint.x << "\t"
                               << resulFactPupils[counter].leftPoint.y << "\t"
                               << resulFactPupils[counter].rightPoint.x << "\t"
                               << resulFactPupils[counter].rightPoint.y << "\n";
        counter++;
        emit progressChangedTestBioIDSignal(i);
    }
    emit progressChangedTestBioIDSignal(list.size());
    res.close();
}
