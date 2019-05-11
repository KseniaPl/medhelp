#ifndef BIOIDTEST
#define BIOIDTEST

#include <QObject>
#include <QVariant>
#include "EyeTracker/Analyser.h"
#include "Logger.h"
#include "Settings.h"

class BioIDTest : public QObject
{
    Q_OBJECT
public:
    void RunTest();
signals:
    void progressChangedTestBioIDSignal(QVariant pos);
};

struct ResultPupils
{
    Point leftPoint;
    Point rightPoint;
    int id;
    ResultPupils (Point left, Point right, int index)
    {
        id = index;
        leftPoint = left;
        rightPoint = right;
    }
};

#endif // BIOIDTEST

