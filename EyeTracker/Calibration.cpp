#include "Calibration.h"

///
/// CalibrationEmersion
///

CalibrationEmersion::CalibrationEmersion() {

}

void CalibrationEmersion::InitDetectPointsAvg()
{
    Point avgPointL, sumPointL;
    for (int i = 0; i < DetectPointsL.count(); i++)
    {
        foreach (Point point, DetectPointsL[i])
        {
            sumPointL.x += point.x;
            sumPointL.y += point.y;
        }
        avgPointL.x = sumPointL.x/DetectPointsL[i].count();
        avgPointL.y = sumPointL.y/DetectPointsL[i].count();
        DetectPointsAvgL.push_back(avgPointL);
        sumPointL.x = 0; sumPointL.y = 0;
        logWrite(std::string("check. ") + "DetectPointsAvgL[" + func::to_string(i)+ "].x " + func::to_string(DetectPointsAvgL[i].x) + " DetectPointsAvgL["+func::to_string(i)+ "].y " + func::to_string(DetectPointsAvgL[i].y) );
    }

    maxXL = DetectPointsAvgL[0].x;
    minXL = DetectPointsAvgL[0].x;
    maxYL = DetectPointsAvgL[0].y;
    minYL = DetectPointsAvgL[0].y;
    for(int i = 0; i < DetectPointsAvgL.count(); i++)
    {
        if(maxXL < DetectPointsAvgL[i].x) maxXL = DetectPointsAvgL[i].x;
        if(minXL > DetectPointsAvgL[i].x) minXL = DetectPointsAvgL[i].x;
        if(maxYL < DetectPointsAvgL[i].y) maxYL = DetectPointsAvgL[i].y;
        if(minYL > DetectPointsAvgL[i].y) minYL = DetectPointsAvgL[i].y;
    }
    logWrite(string("check. ") + "maxXL is "+func::to_string(maxXL));
    logWrite(string("check. ") + "minXL is "+func::to_string(minXL));
    logWrite(string("check. ") + "maxYL is "+func::to_string(maxYL));
    logWrite(string("check. ") + "minYL is "+func::to_string(minYL));



    Point avgPointR, sumPointR;
    for (int i = 0; i < DetectPointsR.count(); i++)
    {
        foreach (Point point, DetectPointsR[i])
        {
            sumPointR.x += point.x;
            sumPointR.y += point.y;
        }
        avgPointR.x = sumPointR.x/DetectPointsR[i].count();
        avgPointR.y = sumPointR.y/DetectPointsR[i].count();
        DetectPointsAvgR.push_back(avgPointR);
        sumPointR.x = 0; sumPointR.y = 0;
        logWrite(std::string("check. ") + "DetectPointsAvgR[" + func::to_string(i)+ "].x " + func::to_string(DetectPointsAvgR[i].x) + " DetectPointsAvgR["+func::to_string(i)+ "].y " + func::to_string(DetectPointsAvgR[i].y) );
    }

    maxXR = DetectPointsAvgR[0].x;
    minXR = DetectPointsAvgR[0].x;
    maxYR = DetectPointsAvgR[0].y;
    minYR = DetectPointsAvgR[0].y;
    for(int i = 0; i < DetectPointsAvgR.count(); i++)
    {
        if(maxXR < DetectPointsAvgR[i].x) maxXR = DetectPointsAvgR[i].x;
        if(minXR > DetectPointsAvgR[i].x) minXR = DetectPointsAvgR[i].x;
        if(maxYR < DetectPointsAvgR[i].y) maxYR = DetectPointsAvgR[i].y;
        if(minYR > DetectPointsAvgR[i].y) minYR = DetectPointsAvgR[i].y;
    }
    logWrite(string("check. ") + "maxXR is "+func::to_string(maxXR));
    logWrite(string("check. ") + "minXR is "+func::to_string(minXR));
    logWrite(string("check. ") + "maxYR is "+func::to_string(maxYR));
    logWrite(string("check. ") + "minYR is "+func::to_string(minYR));

}

void CalibrationEmersion::setSettings(Settings* sets)
{
    settings = sets;
}

//проверка на распознанные точки
//калибровка будет не корректна если координаты расположены не пол возрастанию.
// надо подкорректировать
bool CalibrationEmersion::checkBadCalibrovka()
{
    if(DetectPointsAvgL.count() == 9)
    {
        for(int i = 0; i < 8; i++)
        {
            if(DetectPointsAvgL[i].x > DetectPointsAvgL[i+1].x)
            {
                logWrite(string("check. bad1 ") + "i="+func::to_string(i));
                logWrite(string("check. bad1 ") + "DetectPointsAvgL["+func::to_string(i)+ "].x="+func::to_string(DetectPointsAvgL[i].x));
                logWrite(string("check. bad1 ") + "DetectPointsAvgL["+func::to_string(i+1)+ "].x="+func::to_string(DetectPointsAvgL[i+1].x));
                return true;
            }
            if(DetectPointsAvgR[i].x > DetectPointsAvgR[i+1].x)
            {
                logWrite(string("check. bad1 ") + "i="+func::to_string(i));
                logWrite(string("check. bad1 ") + "DetectPointsAvgR["+func::to_string(i)+ "].x="+func::to_string(DetectPointsAvgR[i].x));
                logWrite(string("check. bad1 ") + "DetectPointsAvgR["+func::to_string(i+1)+ "].x="+func::to_string(DetectPointsAvgR[i+1].x));
                return true;
            }
            if(i == 1) i++;
            if(i == 4) i++;
        }
        for(int i = 0; i < 8; i = i + 3)
        {
            if(DetectPointsAvgL[i].y > DetectPointsAvgL[i+3].y)
            {
                logWrite(string("check. bad2 ") + "i="+func::to_string(i));
                logWrite(string("check. bad2 ") + "DetectPointsAvgL["+func::to_string(i)+ "].y="+func::to_string(DetectPointsAvgL[i].y));
                logWrite(string("check. bad2 ") + "DetectPointsAvgL["+func::to_string(i+3)+ "].y="+func::to_string(DetectPointsAvgL[i+3].y));
                return true;
            }
            if(DetectPointsAvgR[i].y > DetectPointsAvgR[i+3].y)
            {
                logWrite(string("check. bad2 ") + "i="+func::to_string(i));
                logWrite(string("check. bad2 ") + "DetectPointsAvgR["+func::to_string(i)+ "].y="+func::to_string(DetectPointsAvgR[i].y));
                logWrite(string("check. bad2 ") + "DetectPointsAvgR["+func::to_string(i+3)+ "].y="+func::to_string(DetectPointsAvgR[i+3].y));
                return true;
            }
            if(i == 3) i = -2;
            if(i == 4) i = -1;
        }
    }
    else if(DetectPointsAvgL.count() == 4)
    {
        if (DetectPointsAvgL[0].x > DetectPointsAvgL[1].x ||
            DetectPointsAvgL[2].x > DetectPointsAvgL[3].x ||
            DetectPointsAvgL[0].y > DetectPointsAvgL[2].y ||
            DetectPointsAvgL[1].y > DetectPointsAvgL[3].y )
            return true;
        if (DetectPointsAvgR[0].x > DetectPointsAvgR[1].x ||
            DetectPointsAvgR[2].x > DetectPointsAvgR[3].x ||
            DetectPointsAvgR[0].y > DetectPointsAvgR[2].y ||
            DetectPointsAvgR[1].y > DetectPointsAvgR[3].y )
            return true;
    }
    return false;
}

void CalibrationEmersion::clearVariables()
{
    DetectPointsL.clear();
    DetectPointsR.clear();
    DetectPointsAvgL.clear();
    DetectPointsAvgR.clear();
    PointsMonitor.clear();
    maxXL = 0; minXL = 0; maxYL = 0; minYL = 0;
    maxXR = 0; minXR = 0; maxYR = 0; minYR = 0;
}

void CalibrationEmersion::gotoNextPos()
{
    //QList<Point> list;
    DetectPointsL.push_back(QList<Point>());
    DetectPointsR.push_back(QList<Point>());
}

Point CalibrationEmersion::GetAvgPupil(Point leftPupil, Point rightPupil)
{
    Point eyeAvg;
    eyeAvg.x= static_cast<int>((leftPupil.x + rightPupil.x) / 2 + 0.5);
    eyeAvg.y= static_cast<int>((leftPupil.y + rightPupil.y) / 2 + 0.5);
    return eyeAvg;
}

//получить коэфициенты уравнения прямой по двум точкам.
void CalibrationEmersion::getLineKoeffs_via2points(double& A_01, double& B_01, double& C_01, double& u_01, double& p_01, double& mu_01, double& sinu_01, double& cosu_01, Point p0, Point p1)
{
    ///
    /// уравнение прямой в виде:
    /// Ax + By - C = 0
    /// или
    /// x*cos(u) + y*sin(u) - p = 0;
    ///
    /// чтобы перевести из 1 формы в форму 2, нужно умножить всё на mu = +- 1/sqrt(A*A + B*B)
    /// знак выбрать такой, чтобы mu * C < 0
    ///

    A_01 = p0.y-p1.y;
    B_01 = p1.x-p0.x;
    C_01 = p0.y * (p1.x-p0.x) + p0.x * (p1.y-p0.y);

    mu_01 = 1 / sqrt(A_01*A_01 + B_01*B_01);

    if (mu_01 * C_01 > 0)
        mu_01 = 0 - mu_01;

    u_01 = acos(A_01 * mu_01);
    //todo: проверить u_01 = asin(B_01 * mu_01);

    p_01 = C_01 * mu_01;
    sinu_01 = sin(u_01);
    cosu_01 = cos(u_01);
}

// проверить точку, входит ли она внутрь многоугольника
//http://habrahabr.ru/post/125356/
bool CalibrationEmersion::pt_in_polygon(Point& point,const vector<Point> &polygon)
{
    static const int q_patt[2][2] = { {0,1}, {3,2} };
    if (polygon.size()<3)
        return false;
    std::vector<Point>::const_iterator end=polygon.end();
    Point pred_pt=polygon.back();
    pred_pt.x-=point.x;
    pred_pt.y-=point.y;

    int pred_q=q_patt[pred_pt.y<0][pred_pt.x<0];

    int w=0;
    for(std::vector<Point>::const_iterator iter=polygon.begin();iter!=end;++iter)
    {
        Point cur_pt = *iter;
        cur_pt.x-=point.x;
        cur_pt.y-=point.y;
        int q=q_patt[cur_pt.y<0][cur_pt.x<0];
        switch (q-pred_q)
        {
            case -3:++w;break;
            case 3:--w;break;
            case -2:if(pred_pt.x*cur_pt.y>=pred_pt.y*cur_pt.x) ++w;break;
            case 2:if(!(pred_pt.x*cur_pt.y>=pred_pt.y*cur_pt.x)) --w;break;
        }
        pred_pt = cur_pt;
        pred_q = q;
    }
    return w!=0;
}

Point CalibrationEmersion::calcMP(Point p, vector<Point>& points, vector<Point>& monitorPoints)
{
    Point pointMonitor(0,0);

    ///
    /// уравнение прямой в виде:
    /// Ax + By - C = 0
    /// или
    /// x*cos(u) + y*sin(u) - p = 0;
    ///
    /// чтобы перевести из 1 формы в форму 2, нужно умножить всё на mu = +- 1/sqrt(A*A + B*B)
    /// знак выбрать такой, чтобы mu * C < 0
    ///

    //уравнение прямой 01:
    MyLine line01;
    line01.initKoeffs_via2points(Point2d(points[0].x,points[0].y),Point2d(points[1].x,points[1].y));
    //logWrite(string("check6. 'points[0] to points[1]' is ") +func::to_string(MyLine::distanceBetweenPoints(points[0],points[1])));

    //уравнение прямой 23:
    MyLine line23;
    line23.initKoeffs_via2points(Point2d(points[2].x,points[2].y),Point2d(points[3].x,points[3].y));

    //уравнение прямой 02:
    MyLine line02;
    line02.initKoeffs_via2points(Point2d(points[0].x,points[0].y),Point2d(points[2].x,points[2].y));

    //уравнение прямой 13:
    MyLine line13;
    line13.initKoeffs_via2points(Point2d(points[1].x,points[1].y),Point2d(points[3].x,points[3].y));

    //условие параллельности двух прямых
    if (line01.tgu == line23.tgu)
    {
        //уравнение прямой через точку P - Pparal
        MyLine linePparal;
        //todo проверить
        //linePparal.initKoeffs_viaPointAndU(Point2d(points[0].x,points[0].y),line01.u);
        linePparal.initKoeffs_viaPointAndU(Point2d(p.x,p.y),line01.u);

        //этап 4. точка пересечения Pparal с 02 - А и Pparal с 13 - С
        Point2d A = MyLine::getCrossPoint(linePparal, line02);
        Point2d C = MyLine::getCrossPoint(linePparal, line13);

        //этап 5. отношение AP:PC - искомое. по х
        double AP = MyLine:: distanceBetweenPoints(A,p);
        double PC = MyLine:: distanceBetweenPoints(C,p);

        double AC = MyLine::distanceBetweenPoints(A,C);
        //если точка за пределами
        if (AC < AP + PC)
        {
            if (AP < PC)
                pointMonitor.x = monitorPoints[0].x;
            else
                pointMonitor.x = monitorPoints[1].x;
        }
        else
            pointMonitor.x = static_cast<int>( monitorPoints[0].x + (monitorPoints[1].x - monitorPoints[0].x) * AP / (AP + PC) );

        if (false)
        {
            logWrite(string("check6. x IS parall "));
            logWrite(string("check6. p is ") +func::to_string(p));
            logWrite(string("check6. A is ") +func::to_string(A));
            logWrite(string("check6. C is ") +func::to_string(C));
            logWrite(string("check6. AC is") +func::to_string(AC));
            logWrite(string("check5. AP is ") +func::to_string(static_cast<int>(AP)));
            logWrite(string("check5. PC is ") +func::to_string(static_cast<int>(PC)));
            logWrite(string("check5. pointMonitor.y is ") +func::to_string(pointMonitor.x));
        }
    }
    else
    {
        //этап 2. точка пересечения 01 и 23
        Point2d N = MyLine::getCrossPoint(line01, line23);

        //этап 3. уравнение прямой NP
        MyLine lineNP;
        lineNP.initKoeffs_via2points(N,p);

        //этап 4. точка пересечения NP с 02 - А и NP с 13 - С
        Point2d A = MyLine::getCrossPoint(lineNP, line02);
        Point2d C = MyLine::getCrossPoint(lineNP, line13);

        //этап 5. отношение AP:PC - искомое. по х
        double AP = MyLine:: distanceBetweenPoints(A,p);
        double PC = MyLine:: distanceBetweenPoints(C,p);

        double AC = MyLine::distanceBetweenPoints(A,C);
        //если точка за пределами
        if (AC < AP + PC)
        {
            if (AP < PC)
                pointMonitor.x = monitorPoints[0].x;
            else
                pointMonitor.x = monitorPoints[1].x;
        }
        else
            pointMonitor.x = static_cast<int>( monitorPoints[0].x + (monitorPoints[1].x - monitorPoints[0].x) * AP / (AP + PC) );

        if (false)
        {
            logWrite(string("check6. x ISN'T parall "));
            logWrite(string("check6. N is ") +func::to_string(N));
            logWrite(string("check6. p is ") +func::to_string(p));
            logWrite(string("check6. A is ") +func::to_string(A));
            logWrite(string("check6. C is ") +func::to_string(C));
            logWrite(string("check6. AC is") +func::to_string(AC));
            logWrite(string("check5. AP is ") +func::to_string(static_cast<int>(AP)));
            logWrite(string("check5. PC is ") +func::to_string(static_cast<int>(PC)));
            logWrite(string("check5. pointMonitor.y is ") +func::to_string(pointMonitor.x));
        }
    }


    // для координаты y
    if (line02.tgu == line13.tgu)
    {
        //уравнение прямой через точку P - Pparal
        MyLine linePparal;
        linePparal.initKoeffs_viaPointAndU(Point2d(p.x,p.y),line02.u);

        //этап 4. точка пересечения Pparal с 01 - B и Pparal с 23 - D
        Point2d B = MyLine::getCrossPoint(linePparal, line01);
        Point2d D = MyLine::getCrossPoint(linePparal, line23);

        //этап 5. отношение BP:PD - искомое. по х
        double BP = MyLine:: distanceBetweenPoints(B,p);
        double PD = MyLine:: distanceBetweenPoints(p,D);

        double BD = MyLine::distanceBetweenPoints(B,D);
        //если точка за пределами
        if (BD < BP + PD)
        {
            if (BP < PD)
                pointMonitor.y = monitorPoints[0].y;
            else
                pointMonitor.y = monitorPoints[1].y;
        }
        else
            pointMonitor.y = static_cast<int>( monitorPoints[0].y + (monitorPoints[1].y - monitorPoints[0].y) * BP / (BP + PD) );

        if (false)
        {
            logWrite(string("check6. linePparal u is ") +func::to_string(linePparal.u));
            logWrite(string("check6. linePparal p is ") +func::to_string(linePparal.p));
            logWrite(string("check6. line01 u is ") +func::to_string(line01.u));
            logWrite(string("check6. line01 p is ") +func::to_string(line01.p));
            logWrite(string("check6. y IS parall "));
            logWrite(string("check6. line02.u is ") +func::to_string(line02.u));
            logWrite(string("check6. line02.tgu is ") +func::to_string(line02.tgu));
            logWrite(string("check6. line13.tgu is ") +func::to_string(line13.tgu));
            logWrite(string("check6. B is ") +func::to_string(B));
            logWrite(string("check6. D is ") +func::to_string(D));
            logWrite(string("check6. BD is") +func::to_string(BD));
            logWrite(string("check5. BP is ") +func::to_string(static_cast<int>(BP)));
            logWrite(string("check5. PD is ") +func::to_string(static_cast<int>(PD)));
            logWrite(string("check5. pointMonitor.y is ") +func::to_string(pointMonitor.y));
        }
    }
    else
    {
        //этап 2. точка пересечения 02 и 13
        Point2d M = MyLine::getCrossPoint(line02, line13);

        //этап 3. уравнение прямой MP
        MyLine lineMP;
        lineMP.initKoeffs_via2points(M,p);

        //этап 4. точка пересечения MP с 01 - B и MP с 23 - D
        Point2d B = MyLine::getCrossPoint(lineMP, line01);
        Point2d D = MyLine::getCrossPoint(lineMP, line23);

        //этап 5. отношение BP:PD - искомое. по х
        double BP = MyLine:: distanceBetweenPoints(B,p);
        double PD = MyLine:: distanceBetweenPoints(p,D);

        double BD = MyLine::distanceBetweenPoints(B,D);
        //если точка за пределами
        if (BD < BP + PD)
        {
            if (BP < PD)
                pointMonitor.y = monitorPoints[0].y;
            else
                pointMonitor.y = monitorPoints[1].y;
        }
        else
            pointMonitor.y = static_cast<int>( monitorPoints[0].y + (monitorPoints[1].y - monitorPoints[0].y) * BP / (BP + PD) );


        if (false)
        {
            logWrite(string("check6. y ISN'T parall "));
            logWrite(string("check6. line02.tgu is ") +func::to_string(line02.tgu));
            logWrite(string("check6. line13.tgu is ") +func::to_string(line13.tgu));
            logWrite(string("check6. M is ") +func::to_string(M));
            logWrite(string("check6. B is ") +func::to_string(B));
            logWrite(string("check6. D is ") +func::to_string(D));
            logWrite(string("check6. BD is") +func::to_string(BD));
            logWrite(string("check5. BP is ") +func::to_string(static_cast<int>(BP)));
            logWrite(string("check5. PD is ") +func::to_string(static_cast<int>(PD)));
            logWrite(string("check5. pointMonitor.y is ") +func::to_string(pointMonitor.y));
        }
    }

    return pointMonitor;
}

int CalibrationEmersion::findNearbyArea(Point& pupilAvg)
{
//    if (DetectPointsAvg.count() == 4)
//        return 0;

//    int area = -1;

//    //ищем близлежащую по расстоянию точку
//    int index1 = 0;
//    double dist1 = numeric_limits<double>::max(), tempdist;
//    for(int i = 0 ; i < DetectPointsAvg.count(); i++)
//    {
//        tempdist = MyLine::distanceBetweenPoints(DetectPointsAvg[i], pupilAvg);
//        if(dist1 > tempdist)
//        {
//            dist1 = tempdist;
//            index1 = i;
//        }
//    }

//    if (index1 == 0)
//        area = 0;
//    else if (index1 == 2)
//        area = 1;
//    else if (index1 == 6)
//        area = 2;
//    else if (index1 == 8)
//        area = 3;
//    else // index1 == 1,3,4, 5,7 - искать как минимум еще 1 ближайшую точку
//    {
//        logWrite(string("gegege ") + func::to_string(5));
//        //ищем вторую близлежащую по расстоянию точку
//        int index2 = 0;
//        double dist2 = numeric_limits<double>::max();

//        for(int i = 0 ; i < DetectPointsAvg.count(); i++)
//        {
//            logWrite(string("g4234egege ") + func::to_string(5));
//            tempdist = MyLine::distanceBetweenPoints(DetectPointsAvg[i], pupilAvg);
//            if(index1 != i && dist2 > tempdist)
//            {
//                logWrite(string("gegege ") + func::to_string(44));
//                dist2 = tempdist;
//                index2 = i;
//            }
//        }
//        logWrite(string("gegege ") + func::to_string(533));

//        if (index1 == 1)
//        {
//            if (index2 == 0 || index2 == 3)
//                area = 0;
//            else if (index2 == 2 || index2 == 5)
//                area = 1;
//        }
//        else if (index1 == 3)
//        {
//            if (index2 == 0 || index2 == 1)
//                area = 0;
//            else if (index2 == 6 || index2 == 7)
//                area = 2;
//        }
//        else if (index1 == 5)
//        {
//            if (index2 == 1 || index2 == 2)
//                area = 1;
//            else if (index2 == 7 || index2 == 8)
//                area = 3;
//        }
//        else if (index1 == 7)
//        {
//            if (index2 == 3 || index2 == 6)
//                area = 2;
//            else if (index2 == 5 || index2 == 8)
//                area = 3;
//        }
//        else if (index1 == 4)
//        {
//            if (index2 == 0)
//                area = 0;
//            else if (index2 == 2)
//                area = 1;
//            else if (index2 == 6)
//                area = 2;
//            else if (index2 == 8)
//                area = 3;
//        }
//        if (area == -1) // близлежайшие точки - 4 и 1 либо 4 и 3, 4 и 5, 4 и 7. искать еще 1 ближайшую точку.
//        {
//            logWrite(string("gege342ge ") + func::to_string(5));
//            int index3 = 0;
//            double dist3 = numeric_limits<double>::max();
//            for(int i = 0 ; i < DetectPointsAvg.count(); i++)
//            {
//                logWrite(string("gege234234ge ") + func::to_string(5));
//                tempdist = MyLine::distanceBetweenPoints(DetectPointsAvg[i], pupilAvg);
//                if(index1 != i && index2 != i && dist3 > tempdist)
//                {
//                    logWrite(string("ge2323gege ") + func::to_string(5));
//                    dist3 = tempdist;
//                    index3 = i;
//                }
//            }

//            logWrite(string("gege23423ge ") + func::to_string(5));
//            if (index1 == 1 || index2 == 1)
//            {
//                if (index3 == 0 || index3 == 3)
//                    area = 0;
//                else
//                    area = 1;
//            }
//            else if (index1 == 3 || index2 == 3)
//            {
//                if (index3 == 0 || index3 == 1)
//                    area = 0;
//                else
//                    area = 2;
//            }
//            else if (index1 == 5 || index2 == 5)
//            {
//                if (index3 == 1 || index3 == 2)
//                    area = 1;
//                else
//                    area = 3;
//            }
//            else if (index1 == 7 || index2 == 7)
//            {
//                if (index3 == 3 || index3 == 6)
//                    area = 2;
//                else
//                    area = 3;
//            }
//        }
//    }
//    return area;
    return 0;
}

void CalibrationEmersion::init4points(vector<Point>& pointsL, vector<Point>& pointsR, vector<Point>& monitorPoints,int area)
{
    if (DetectPointsAvgL.count() == 4)
    {
        pointsL.push_back(DetectPointsAvgL[0]);
        pointsL.push_back(DetectPointsAvgL[1]);
        pointsL.push_back(DetectPointsAvgL[2]);
        pointsL.push_back(DetectPointsAvgL[3]);
        pointsR.push_back(DetectPointsAvgR[0]);
        pointsR.push_back(DetectPointsAvgR[1]);
        pointsR.push_back(DetectPointsAvgR[2]);
        pointsR.push_back(DetectPointsAvgR[3]);
        monitorPoints.push_back(PointsMonitor[0]);
        monitorPoints.push_back(PointsMonitor[3]);
        return;
    }
    if (area == 0)
    {
        pointsL.push_back(DetectPointsAvgL[0]);
        pointsL.push_back(DetectPointsAvgL[1]);
        pointsL.push_back(DetectPointsAvgL[3]);
        pointsL.push_back(DetectPointsAvgL[4]);
        pointsR.push_back(DetectPointsAvgR[0]);
        pointsR.push_back(DetectPointsAvgR[1]);
        pointsR.push_back(DetectPointsAvgR[3]);
        pointsR.push_back(DetectPointsAvgR[4]);
        monitorPoints.push_back(PointsMonitor[0]);
        monitorPoints.push_back(PointsMonitor[4]);
    }
    else if (area == 1)
    {
        pointsL.push_back(DetectPointsAvgL[1]);
        pointsL.push_back(DetectPointsAvgL[2]);
        pointsL.push_back(DetectPointsAvgL[4]);
        pointsL.push_back(DetectPointsAvgL[5]);
        pointsR.push_back(DetectPointsAvgR[1]);
        pointsR.push_back(DetectPointsAvgR[2]);
        pointsR.push_back(DetectPointsAvgR[3]);
        pointsR.push_back(DetectPointsAvgR[4]);
        monitorPoints.push_back(PointsMonitor[1]);
        monitorPoints.push_back(PointsMonitor[5]);
    }
    else if (area == 2)
    {
        pointsL.push_back(DetectPointsAvgL[3]);
        pointsL.push_back(DetectPointsAvgL[4]);
        pointsL.push_back(DetectPointsAvgL[6]);
        pointsL.push_back(DetectPointsAvgL[7]);
        pointsR.push_back(DetectPointsAvgR[3]);
        pointsR.push_back(DetectPointsAvgR[4]);
        pointsR.push_back(DetectPointsAvgR[6]);
        pointsR.push_back(DetectPointsAvgR[7]);
        monitorPoints.push_back(PointsMonitor[3]);
        monitorPoints.push_back(PointsMonitor[7]);
    }
    else if (area == 3)
    {
        pointsL.push_back(DetectPointsAvgL[4]);
        pointsL.push_back(DetectPointsAvgL[5]);
        pointsL.push_back(DetectPointsAvgL[7]);
        pointsL.push_back(DetectPointsAvgL[8]);
        pointsR.push_back(DetectPointsAvgR[4]);
        pointsR.push_back(DetectPointsAvgR[5]);
        pointsR.push_back(DetectPointsAvgR[7]);
        pointsR.push_back(DetectPointsAvgR[8]);
        monitorPoints.push_back(PointsMonitor[4]);
        monitorPoints.push_back(PointsMonitor[8]);
    }
}

Point CalibrationEmersion::GetMonitorPoint(Point leftPupil, Point rightPupil)
{
    int area;
    if (settings->pointsCount == 9)
    {
//        //area - область из 4-х точек. всего их 4 при 9 точках калибровки. принимает значения от 0 до 3
//        area = findNearbyArea(pupilAvg);

//        if (area == -1)
//        {
//            logWriteError("area == -1. ошибка в вычислениях findNearbyArea");
//            logWrite(string("pupilaAvg is") + func::to_string(pupilAvg));

//            for(int i = 0 ; i < DetectPointsAvgL.count(); i++)
//            {
//                logWrite(string("DetectPointsAvgL[i] is") + func::to_string(DetectPointsAvgL[i]));
//            }
//        }
//        logWrite(string("area is ") + func::to_string(area));
    }
    else if (settings->pointsCount == 4)
        area = 0;

    vector<Point> pointsL, pointsR; // выбираем 4 точки калибровки
    vector<Point> monitorPoints; // выбираем 4 точки калибровки
    init4points(pointsL, pointsR, monitorPoints, area);

    // monitorPoints - 2 точки ЭКРАНА, получаемые при калибровке (верхняя левая и ниж правая).
    // DetectPointsAvg - 4 или 9 (или другое кол-во) точек НА ИЗОБРАЖЕНИИ, получаемых при калибровке.

    if (false)
    {
        logWrite(string("-----"));
        logWrite(string("check6. leftPupil is ") +func::to_string(leftPupil));
        logWrite(string("check6. pointsL[0] is ") +func::to_string(pointsL[0]));
        logWrite(string("check6. pointsL[1] is ") +func::to_string(pointsL[1]));
        logWrite(string("check6. pointsL[2] is ") +func::to_string(pointsL[2]));
        logWrite(string("check6. pointsL[3] is ") +func::to_string(pointsL[3]));
        logWrite(string("check6. monitorPoints[0] is ") +func::to_string(monitorPoints[0]));
        logWrite(string("check6. monitorPoints[1] is ") +func::to_string(monitorPoints[1]));

        logWrite(string("hhhh"));
        logWrite(string("-----"));
        logWrite(string("check6. maxXL is ") +func::to_string(maxXL));
        logWrite(string("check6. minXL is ") +func::to_string(minXL));
        logWrite(string("check6. maxYL is ") +func::to_string(maxYL));
        logWrite(string("check6. minYL is ") +func::to_string(minYL));
        logWrite(string("-----"));
        logWrite(string("check6. maxXR is ") +func::to_string(maxXR));
        logWrite(string("check6. minXR is ") +func::to_string(minXR));
        logWrite(string("check6. maxYR is ") +func::to_string(maxYR));
        logWrite(string("check6. minYR is ") +func::to_string(minYR));
        logWrite(string("-----"));
    }

    if(settings->pointsCount == 4 || settings->pointsCount == 9)
    {
        if(maxXL < leftPupil.x) leftPupil.x = maxXL;
        if(minXL > leftPupil.x) leftPupil.x = minXL;
        if(maxYL < leftPupil.y) leftPupil.y = maxYL;
        if(minYL > leftPupil.y) leftPupil.y = minYL;
        if(maxXR < rightPupil.x) rightPupil.x = maxXR;
        if(minXR > rightPupil.x) rightPupil.x = minXR;
        if(maxYR < rightPupil.y) rightPupil.y = maxYR;
        if(minYR > rightPupil.y) rightPupil.y = minYR;
    }

    Point pointMonitorL = calcMP(leftPupil, pointsL, monitorPoints);
    Point pointMonitorR = calcMP(rightPupil, pointsR, monitorPoints);

    //logWrite("pointMonitorR x:y - " + to_string(pointMonitorR.x) + ":" + to_string(pointMonitorR.y));

    Point monitorPoint;
    monitorPoint.x = static_cast<int>((pointMonitorL.x +pointMonitorR.x)/2 + 0.5);
    monitorPoint.y = static_cast<int>((pointMonitorL.y +pointMonitorR.y)/2 + 0.5);
    return monitorPoint;
}



///
/// CalibrationMotion
///

//static variables
int CalibrationMotion::calibrationCols;
int CalibrationMotion::calibrationRows;
vector<Point> CalibrationMotion::pupilAvg;
Rect CalibrationMotion::avgEyeRect;
int CalibrationMotion::mainWindow_width;
int CalibrationMotion::mainWindow_height;
Rect CalibrationMotion::leftEyeRect;
Rect CalibrationMotion::rightEyeRect;
Rect CalibrationMotion::CalibrationAVGEyeRect;
Rect CalibrationMotion::CalibrationAVGEyeRectL;
Rect CalibrationMotion::CalibrationAVGEyeRectR;
Rect CalibrationMotion::leftEye_calibrationRect;
Rect CalibrationMotion::rightEye_calibrationRect;
vector< vector<Point> > CalibrationMotion::pupilAvg4;

CalibrationMotion::CalibrationMotion()
{
    CalibrationAVGEyeRectL.width = 1;
    CalibrationAVGEyeRectL.height = 1;
    CalibrationAVGEyeRectR.width = 1;
    CalibrationAVGEyeRectR.height = 1;
}

void CalibrationMotion::setMainWindowSize(int mainWindow_width, int mainWindow_height)
{
    this->mainWindow_width = mainWindow_width;
    this->mainWindow_height = mainWindow_height;
}

Point CalibrationMotion::GetMonitorPoint(vector<Point> *pupilsLeft, vector<Point> *pupilsRight)
{
    if (pupilsLeft->size() == 0 || pupilsRight->size() == 0)
        return Point();

    Point left = (*pupilsLeft)[0];
    Point right = (*pupilsRight)[0];

    left.x -= CalibrationAVGEyeRectL.x;
    left.y -= CalibrationAVGEyeRectL.y;
    right.x -= CalibrationAVGEyeRectR.x;
    right.y -= CalibrationAVGEyeRectR.y;

    //TODO - временно
    //перевод зрачка за грацицей - внутрь
    if (left.x < 0)
        left.x = 0;
    if (left.y < 0)
        left.y = 0;
    if (left.x >= CalibrationAVGEyeRectL.width)
        left.x = CalibrationAVGEyeRectL.width;
    if (left.y >= CalibrationAVGEyeRectL.height)
        left.y = CalibrationAVGEyeRectL.height;

    if (right.x < 0)
        right.x = 0;
    if (right.y < 0)
        right.y = 0;
    if (right.x >= CalibrationAVGEyeRectR.width)
        right.x = CalibrationAVGEyeRectR.width;
    if (right.y >= CalibrationAVGEyeRectR.height)
        right.y = CalibrationAVGEyeRectR.height;

    //правильное округление в static_cast: round = 0.5
    double round = 0.5;

    Point monitorPoint;

    monitorPoint.x = static_cast<int>(
                        ( (mainWindow_width * left.x / CalibrationAVGEyeRectL.width) +
                          (mainWindow_width * right.x / CalibrationAVGEyeRectR.width)   )/2
                        + round);
    monitorPoint.y = static_cast<int>(
                ( (mainWindow_height * left.y / CalibrationAVGEyeRectR.height) +
                  (mainWindow_height * right.y / CalibrationAVGEyeRectR.height)   )/2
                + round);

    return monitorPoint;
}

Point CalibrationMotion::GetMonitorPoint()
{
    if (pupilAvg.size() == 0)
        return Point();

    //правильное округление в static_cast: round = 0.5
    double round = 0.5;

    Point monitorPoint;

    monitorPoint.x = static_cast<int>((mainWindow_width * pupilAvg[0].x / calibrationCols) + round);
    monitorPoint.y = static_cast<int>((mainWindow_height * pupilAvg[0].y / calibrationRows) + round);

    return monitorPoint;
}

bool CalibrationMotion::countAveregePupilCoords(vector<Point> pupilsLeft, vector<Point> pupilsRight)
{
    // используются глобальные
    // pupilsLeft, pupilsRight
    // leftEyeRect, rightEyeRect

    pupilAvg.clear();

    /* //проверка только одного глаза
    avgEyeRect = leftEyeRect;
    vector<Point> pupilsLeft_copy, pupilsRight_copy;
    for (int i = 0; i < pupilsLeft.size() - 1; i++)
    {
    pupilsLeft_copy.push_back(pupilsLeft[i]);
    pupilsLeft_copy[i].x -= leftEyeRect.x;
    pupilsLeft_copy[i].y -= leftEyeRect.y;

    pupilsRight_copy.push_back(pupilsLeft[i]);
    pupilsRight_copy[i].x -= rightEyeRect.x;
    pupilsRight_copy[i].y -= rightEyeRect.y;
    }
    return pupilsLeft_copy;*/

    //правильное округление в static_cast: round = 0.5
    double round = 0.5;

    if (false)
    {
        /////////////////////////////////////////неправильное усренение зрачков
        leftEyeRect = getEyeRect(pupilsLeft);
        rightEyeRect = getEyeRect(pupilsRight);

        avgEyeRect.height = static_cast<int>((leftEyeRect.height + rightEyeRect.height) / 2 + round);
        avgEyeRect.width = static_cast<int>((leftEyeRect.width + rightEyeRect.width) / 2 + round);

        if (pupilsLeft.size() != pupilsRight.size())
        {
            logWriteError("pupilsLeft.size() != pupilsRight.size()");
            pupilAvg.clear();
            return false;
        }

        // используем pupilsLeft_copy, pupilsRight_copy для того чтобы перевести координаты не относительно глаза, а относительно avgEyeRect
        vector<Point> pupilsLeft_copy, pupilsRight_copy;
        for (uint i = 0; i < pupilsLeft.size(); i++)
        {
            pupilsLeft_copy.push_back(pupilsLeft[i]);
            pupilsLeft_copy[i].x -= leftEyeRect.x;
            pupilsLeft_copy[i].y -= leftEyeRect.y;

            pupilsRight_copy.push_back(pupilsRight[i]);
            pupilsRight_copy[i].x -= rightEyeRect.x;
            pupilsRight_copy[i].y -= rightEyeRect.y;
        }

        Point temp;
        for (uint i = 0; i < pupilsLeft_copy.size(); i++)
        {
            if (pupilsLeft_copy[i].x <= 0 && pupilsLeft_copy[i].y <= 0)
            {
                temp.x = static_cast<int>(pupilsRight_copy[i].x * avgEyeRect.width / rightEyeRect.width + round);
                temp.y = static_cast<int>(pupilsRight_copy[i].y * avgEyeRect.height / rightEyeRect.height + round);
            }
            else if (pupilsRight_copy[i].x <= 0 && pupilsRight_copy[i].y <= 0)
            {
                temp.x = static_cast<int>(pupilsLeft_copy[i].x * avgEyeRect.width / leftEyeRect.width + round);
                temp.y = static_cast<int>(pupilsLeft_copy[i].y * avgEyeRect.height / leftEyeRect.height + round);
            }
            else
            {
                temp.x = static_cast<int>(((pupilsLeft_copy[i].x * avgEyeRect.width / leftEyeRect.width) + (pupilsRight_copy[i].x * avgEyeRect.width / rightEyeRect.width)) / 2 + round);
                temp.y = static_cast<int>(((pupilsLeft_copy[i].y * avgEyeRect.height / leftEyeRect.height) + (pupilsRight_copy[i].y * avgEyeRect.height / rightEyeRect.height)) / 2 + round);
            }
            pupilAvg.push_back(temp);
        }
        ////////////////конец
    }
    else
    {
        //если pupilsLeft и pupilsRight разные по длине (а вдруг)
        if (pupilsLeft.size() != pupilsRight.size())
        {
            logWriteError("pupilsLeft.size() != pupilsRight.size()");
            pupilAvg.clear();
            return false;
        }

        Point temp;
        for (uint i = 0; i < pupilsLeft.size(); i++)
        {
            //если левый зрачок был отсеян по ряду причин (занулён) - берём правый
            if (pupilsLeft[i].x <= 0 && pupilsLeft[i].y <= 0)
            {
                temp.x = pupilsRight[i].x;
                temp.y = pupilsRight[i].y;
            }
            //аналогично с правым
            else if (pupilsRight[i].x <= 0 && pupilsRight[i].y <= 0)
            {
                temp.x = pupilsLeft[i].x;
                temp.y = pupilsLeft[i].y;
            }
            //считаем среднее значение
            else
            {
                temp.x = static_cast<int>((pupilsRight[i].x + pupilsLeft[i].x) / 2 + round);
                temp.y = static_cast<int>((pupilsRight[i].y + pupilsLeft[i].y) / 2 + round);
            }
            pupilAvg.push_back(temp);
        }
    }

    //update avgEyeRect
    avgEyeRect = getEyeRect(pupilAvg);
    //переводим в координаты относительно avgEyeRect
    for (uint i = 0; i < pupilsLeft.size(); i++)
    {
        pupilAvg[i].x -= avgEyeRect.x;
        pupilAvg[i].y -= avgEyeRect.y;
    }

    //переводим в координаты относительно CalibrationAVGEyeRect
    pupilAvg[0].x += avgEyeRect.x;
    pupilAvg[0].y += avgEyeRect.y;
    pupilAvg[0].x -= CalibrationAVGEyeRect.x;
    pupilAvg[0].y -= CalibrationAVGEyeRect.y;

    //TODO - временно
    //перевод зрачка за грацицей - внутрь
    if (pupilAvg[0].x < 0)
        pupilAvg[0].x = 0;
    if (pupilAvg[0].y < 0)
        pupilAvg[0].y = 0;
    if (pupilAvg[0].x >= calibrationCols)
        pupilAvg[0].x = calibrationCols;
    if (pupilAvg[0].y >= calibrationRows)
        pupilAvg[0].y = calibrationRows;

    return true;
}

void CalibrationMotion::setSettings(Settings* sets)
{
    settings = sets;
}

bool CalibrationMotion::countAveregePupilCoords(vector< vector<Point> > pupilsLeft, vector< vector<Point> > pupilsRight)
{
    // используются глобальные
    // pupilsLeft, pupilsRight
    // leftEyeRect, rightEyeRect

    pupilAvg.clear();
    pupilAvg4.clear();
    pupilAvg4.push_back(vector<Point>());
    pupilAvg4.push_back(vector<Point>());
    pupilAvg4.push_back(vector<Point>());
    pupilAvg4.push_back(vector<Point>());

    //правильное округление в static_cast: round = 0.5
    double round = 0.5;

    for (uint k=0; k< pupilsLeft.size();k++)
    {
        //если pupilsLeft и pupilsRight разные по длине (а вдруг)
        if (pupilsLeft[k].size() != pupilsRight[k].size())
        {
            logWriteError("pupilsLeft.size() != pupilsRight.size()");
            pupilAvg.clear();
            return false;
        }
    }

    Point temp;
    for (uint k=0; k < pupilsLeft.size();k++)
    {
        for (uint i = 0; i < pupilsLeft[k].size(); i++)
        {
            //если левый зрачок был отсеян по ряду причин (занулён) - берём правый
            if (pupilsLeft[k][i].x <= 0 && pupilsLeft[k][i].y <= 0)
            {
                temp.x = pupilsRight[k][i].x;
                temp.y = pupilsRight[k][i].y;
            }
            //аналогично с правым
            else if (pupilsRight[k][i].x <= 0 && pupilsRight[k][i].y <= 0)
            {
                temp.x = pupilsLeft[k][i].x;
                temp.y = pupilsLeft[k][i].y;
            }
            //считаем среднее значение
            else
            {
                temp.x = static_cast<int>((pupilsRight[k][i].x + pupilsLeft[k][i].x) / 2 + round);
                temp.y = static_cast<int>((pupilsRight[k][i].y + pupilsLeft[k][i].y) / 2 + round);
            }
            pupilAvg.push_back(temp);
            pupilAvg4[k].push_back(temp);
        }
    }
;
    //update avgEyeRect
    avgEyeRect = getEyeRect(pupilAvg4);
    //переводим в координаты относительно avgEyeRect
    for (uint i = 0; i < pupilsLeft.size(); i++)
    {
        pupilAvg[i].x -= avgEyeRect.x;
        pupilAvg[i].y -= avgEyeRect.y;
    }

    //TODO: неправильно! разобраться, почему только 0-й
    //переводим в координаты относительно CalibrationAVGEyeRect
    pupilAvg[0].x += avgEyeRect.x;
    pupilAvg[0].y += avgEyeRect.y;
    pupilAvg[0].x -= CalibrationAVGEyeRect.x;
    pupilAvg[0].y -= CalibrationAVGEyeRect.y;

    //TODO - временно
    //перевод зрачка за грацицей - внутрь
    if (pupilAvg[0].x < 0)
        pupilAvg[0].x = 0;
    if (pupilAvg[0].y < 0)
        pupilAvg[0].y = 0;
    if (pupilAvg[0].x >= calibrationCols)
        pupilAvg[0].x = calibrationCols;
    if (pupilAvg[0].y >= calibrationRows)
        pupilAvg[0].y = calibrationRows;

    return true;
}

Rect CalibrationMotion::getEyeRect(vector<Point> pupils)
{
    int LeftIMG = pupils[0].x, RightIMG = pupils[0].x,
            TopIMG = pupils[0].y, BottomIMG = pupils[0].y;

    for (uint i = 0; i < pupils.size(); i++)
    {
        if (LeftIMG > pupils[i].x)
            LeftIMG = pupils[i].x;
        if (RightIMG < pupils[i].x)
            RightIMG = pupils[i].x;
        if (BottomIMG > pupils[i].y)
            BottomIMG = pupils[i].y;
        if (TopIMG < pupils[i].y)
            TopIMG = pupils[i].y;
    }

    return Rect(Point(LeftIMG, TopIMG), Point(RightIMG, BottomIMG));
}

Rect CalibrationMotion::getEyeRect(vector< vector<Point> > pupils)
{
    int LeftIMG, RightIMG, TopIMG, BottomIMG;

    int sum = 0;
    for (uint i = 0; i < pupils[0].size(); i++)
    {
        sum += pupils[0][i].y;
    }
    TopIMG = static_cast<int>(sum / pupils[0].size() + 0.5);

    sum = 0;
    for (uint i = 0; i < pupils[1].size(); i++)
    {
        sum += pupils[1][i].x;
    }
    RightIMG = static_cast<int>(sum / pupils[1].size() + 0.5);

    sum = 0;
    for (uint i = 0; i < pupils[2].size(); i++)
    {
        sum += pupils[2][i].y;
    }
    BottomIMG = static_cast<int>(sum / pupils[2].size() + 0.5);
    sum = 0;
    for (uint i = 0; i < pupils[3].size(); i++)
    {
        sum += pupils[3][i].x;
    }
    LeftIMG = static_cast<int>(sum / pupils[3].size() + 0.5);

    Rect tt(Point(LeftIMG, TopIMG), Point(RightIMG, BottomIMG));
    return tt;
}

bool CalibrationMotion::finishCalibration(vector< vector<Point> > leftPupils4, vector< vector<Point> > rightPupils4, vector<Rect>& foundEyes, vector<Point>& pupilsLeft, vector<Point>& pupilsRight, Rect& foundFace, Mat& foundfaceImg)
{
    bool returnvalue = true;
    if (pupilsLeft.size() == 0 || pupilsRight.size() == 0)
    {
        logWriteError("Лицо, глаза или зрачки не были найдены 1");
        logWrite("pupilsLeft.size() = " + func::to_string(pupilsLeft.size()));
        logWrite("pupilsRight.size() = " + func::to_string(pupilsRight.size()));
        returnvalue = false;
        return returnvalue;
    }

    //ВИЗУАЛИЗАЦИЯ!!! рисуем траекторию
    if (foundEyes.size() != 2
            || pupilsLeft.size() == 0
            || pupilsRight.size() == 0
            || foundFace.area() <= 0
            )
    {
        logWriteError("Лицо, глаза или зрачки не были найдены 2");
        logWrite("foundEyes.size() = " + func::to_string(foundEyes.size()));
        logWrite("pupilsLeft.size() = " + func::to_string(pupilsLeft.size()));
        logWrite("pupilsRight.size() = " + func::to_string(pupilsRight.size()));
        logWrite("foundFace.area() = " + func::to_string(foundFace.area()));
        returnvalue = false;
        return returnvalue;
    }

    if (foundfaceImg.empty())
    {
        logWriteError("frame is empty");
        //todo:
        //создать закрашеный faceImg или взять изображение из видео
        //для создания необходимы высота и ширина
        returnvalue = false;
        return returnvalue;
    }
    //else
    //	faceImg = frame(foundFace);

    Mat leftImgTrajectory, rightImgTrajectory, rightImgTrajectory2;
    try
    {
        leftImgTrajectory = foundfaceImg(foundEyes[0]);
        rightImgTrajectory = foundfaceImg(foundEyes[1]);
        rightImgTrajectory2 = foundfaceImg(foundEyes[0]).clone();

        //показать движение зрачка по четырем сторонам
        if (false)
        {
            Mat lit1 = leftImgTrajectory.clone();
            Mat lit2 = leftImgTrajectory.clone();
            Mat lit3 = leftImgTrajectory.clone();
            Mat lit4 = leftImgTrajectory.clone();
            for (uint i = 0; i < leftPupils4[0].size(); ++i)
            {
                line(lit1, leftPupils4[0][i], leftPupils4[0][i], Scalar(0, 255, 0));
            }
            for (uint i = 0; i < leftPupils4[1].size(); ++i)
            {
                line(lit2, leftPupils4[1][i], leftPupils4[1][i], Scalar(0, 255, 0));
            }
            for (uint i = 0; i < leftPupils4[2].size(); ++i)
            {
                line(lit3, leftPupils4[2][i], leftPupils4[2][i], Scalar(0, 255, 0));
            }
            for (uint i = 0; i < leftPupils4[3].size(); ++i)
            {
                line(lit4, leftPupils4[3][i], leftPupils4[3][i], Scalar(0, 255, 0));
            }
            imshow("lit1", lit1);
            imshow("lit2", lit2);
            imshow("lit3", lit3);
            imshow("lit4", lit4);


            Mat rit1 = rightImgTrajectory.clone();
            Mat rit2 = rightImgTrajectory.clone();
            Mat rit3 = rightImgTrajectory.clone();
            Mat rit4 = rightImgTrajectory.clone();
            for (uint i = 0; i < rightPupils4[0].size(); ++i)
            {
                line(rit1, rightPupils4[0][i], rightPupils4[0][i], Scalar(0, 255, 0));
            }
            for (uint i = 0; i < rightPupils4[1].size(); ++i)
            {
                line(rit2, rightPupils4[1][i], rightPupils4[1][i], Scalar(0, 255, 0));
            }
            for (uint i = 0; i < rightPupils4[2].size(); ++i)
            {
                line(rit3, rightPupils4[2][i], rightPupils4[2][i], Scalar(0, 255, 0));
            }
            for (uint i = 0; i < rightPupils4[3].size(); ++i)
            {
                line(rit4, rightPupils4[3][i], rightPupils4[3][i], Scalar(0, 255, 0));
            }
            imshow("rit1", rit1);
            imshow("rit2", rit2);
            imshow("rit3", rit3);
            imshow("rit4", rit4);
        }


        //нарисовать прямоугольники
        leftEyeRect = getEyeRect(leftPupils4);
        rightEyeRect = getEyeRect(rightPupils4);

        leftEye_calibrationRect = leftEyeRect;
        rightEye_calibrationRect = rightEyeRect;

        rectangle(leftImgTrajectory, leftEyeRect, CV_RGB(222, 222, 222));
        rectangle(rightImgTrajectory, rightEyeRect, CV_RGB(222, 222, 222));

        //выделить все точки
        Scalar colorLeft(0, 255, 0); //red
        Scalar colorRight(0, 255, 0);
        Scalar colorError(0, 0, 255);
        for (uint i = 0; i < pupilsLeft.size(); ++i)
        {
            line(leftImgTrajectory, pupilsLeft[i], pupilsLeft[i], colorLeft);
            //circle(leftImgTrajectory, pupilsLeft[i], 1, colorLeft);
        }

        for (uint i = 0; i < pupilsRight.size(); ++i)
        {
            line(rightImgTrajectory, pupilsRight[i], pupilsRight[i], colorRight);
            //circle(rightImgTrajectory, pupilsRight[i], 1, colorRight);
        }

        for (uint i = 0; i < pupils_errors_L.size(); ++i)
        {
            circle(leftImgTrajectory, pupils_errors_L[i], 1, colorError);
            //line(leftImgTrajectory, pupils_errors_L[i], pupils_errors_L[i], colorError);
        }

        for (uint i = 0; i < pupils_errors_R.size(); ++i)
        {
            circle(rightImgTrajectory, pupils_errors_R[i], 1, colorError);
            //line(rightImgTrajectory, pupils_errors_R[i], pupils_errors_R[i], colorError);
        }

        countAveregePupilCoords(leftPupils4,rightPupils4);
        CalibrationAVGEyeRect = avgEyeRect;

        CalibrationAVGEyeRectL = getEyeRect(leftPupils4);
        CalibrationAVGEyeRectR = getEyeRect(rightPupils4);

        rectangle(leftImgTrajectory, CalibrationAVGEyeRectL, Scalar(255, 0, 255));
        rectangle(rightImgTrajectory, CalibrationAVGEyeRectR, Scalar(255, 0, 255));

        for (uint i = 0; i < pupilAvg.size(); ++i)
        {
            line(rightImgTrajectory2, pupilAvg[i], pupilAvg[i], colorRight);
            //circle(rightImgTrajectory, pupilsRight[i], 1, colorRight);
        }

        //СОПОСТАВЛЕНИЕ КООРДИНАТ
        //vector<Point>::iterator it = find(pupilsLeft.begin(), pupilsLeft.end(), 5);
        //sort(pupilsLeft.begin(), pupilsLeft.end(), <предикат сортировки>);

        resize(leftImgTrajectory, leftImgTrajectory, Size(leftImgTrajectory.cols * 2, leftImgTrajectory.rows * 2));
        resize(rightImgTrajectory, rightImgTrajectory, Size(rightImgTrajectory.cols * 2, rightImgTrajectory.rows * 2));

        resize(rightImgTrajectory2, rightImgTrajectory2, Size(rightImgTrajectory2.cols * 4, rightImgTrajectory2.rows * 4));

        //destroyWindow("Left Eye");
        //destroyWindow("Right Eye");
        //destroyWindow("AVG глаз");
        if (settings->showImage)
        {
            imshow("Left Eye", leftImgTrajectory);
            imshow("Right Eye", rightImgTrajectory);
        }

        //imshow("AVG глаз", rightImgTrajectory2);
        //waitKey(0);

        //destroyAllWindows();
    }
    catch (exception& ex) {
        QString f(ex.what());
        //MessageBox(f, "Info", MB_OK | MB_ICONSTOP);
        returnvalue = false;
        return returnvalue;
    }

    //TODO точно ли +1?
    calibrationCols = static_cast<int>(avgEyeRect.width);
    calibrationRows = static_cast<int>(avgEyeRect.height);


    if (CalibrationAVGEyeRectL.area() == 0 || CalibrationAVGEyeRectR.area() == 0 || avgEyeRect.area() == 0)
    {
        logWriteError("Ширина или длина прямоугольника, внутри которого движется глаз, равна 0.");
        returnvalue = false;
    }

    return returnvalue;
}



void CalibrationMotion::motionCalibration_EliminateOutlyingBlips(vector< vector<Point> >& _leftPupils4, vector< vector<Point> >& _rightPupils4, vector<Point>& pupilsLeft, vector<Point>& pupilsRight)
{
    //vector<Point> pupils_errors_R, pupils_errors_L;
    leftPupils4 = _leftPupils4;
    rightPupils4 = _rightPupils4;

    //корректировка каждого вектора точек найденных глаз
    eliminateOutlyingBlips(leftPupils4[0], 1, pupils_errors_L);
    eliminateOutlyingBlips(leftPupils4[1], 2, pupils_errors_L);
    eliminateOutlyingBlips(leftPupils4[2], 3, pupils_errors_L);
    eliminateOutlyingBlips(leftPupils4[3], 4, pupils_errors_L);

    eliminateOutlyingBlips(rightPupils4[0], 1, pupils_errors_R);
    eliminateOutlyingBlips(rightPupils4[1], 2, pupils_errors_R);
    eliminateOutlyingBlips(rightPupils4[2], 3, pupils_errors_R);
    eliminateOutlyingBlips(rightPupils4[3], 4, pupils_errors_R);

    size_t pupils_eps = 12;
    for (uint k = 0; k < leftPupils4.size();k++)
    {
        for (size_t i = 0; i < leftPupils4[k].size() - 2; i++)
        {
            if(abs(leftPupils4[k][i].x - leftPupils4[k][i+1].x) > pupils_eps)
            {
                pupils_errors_L.push_back(leftPupils4[k][i+1]);
                leftPupils4[k][i+1] = leftPupils4[k][i];
            }
            if(abs(rightPupils4[k][i].x - rightPupils4[k][i+1].x) > pupils_eps)
            {
                pupils_errors_R.push_back(rightPupils4[k][i+1]);
                rightPupils4[k][i+1] = rightPupils4[k][i];
            }
            if(abs(leftPupils4[k][i].y - leftPupils4[k][i+1].y) > pupils_eps)
            {
                pupils_errors_L.push_back(leftPupils4[k][i+1]);
                leftPupils4[k][i+1] = leftPupils4[k][i];
            }
            if(abs(rightPupils4[k][i].y - rightPupils4[k][i+1].y) > pupils_eps)
            {
                pupils_errors_R.push_back(rightPupils4[k][i+1]);
                rightPupils4[k][i+1] = rightPupils4[k][i];
            }
        }

        if (k < leftPupils4.size() - 1)
        {
            if(abs(leftPupils4[k][leftPupils4[0].size()-1].x - leftPupils4[k+1][0].x) > pupils_eps)
            {
                pupils_errors_L.push_back(leftPupils4[k+1][0]);
                leftPupils4[k+1][0] = leftPupils4[k][leftPupils4[k].size()-1];
            }
            if(abs(rightPupils4[k][rightPupils4[0].size()-1].x - rightPupils4[k+1][0].x) > pupils_eps)
            {
                pupils_errors_R.push_back(rightPupils4[k+1][0]);
                rightPupils4[k+1][0] = rightPupils4[k][rightPupils4[k].size()-1];
            }
            if(abs(leftPupils4[k][leftPupils4[0].size()-1].y - leftPupils4[k+1][0].y) > pupils_eps)
            {
                pupils_errors_L.push_back(leftPupils4[k+1][0]);
                leftPupils4[k+1][0] = leftPupils4[k][leftPupils4[k].size()-1];
            }
            if(abs(rightPupils4[k][rightPupils4[0].size()-1].y - rightPupils4[k+1][0].y) > pupils_eps)
            {
                pupils_errors_R.push_back(rightPupils4[k+1][0]);
                rightPupils4[k+1][0] = rightPupils4[k][rightPupils4[k].size()-1];
            }
        }
    }

//    if(abs(leftPupils4[0][leftPupils4[0].size()].x - leftPupils4[1][0].x) > pupils_eps)
//    {
//        pupils_errors_L.push_back(leftPupils4[1][0].x);
//        leftPupils4[1][0] = leftPupils4[0][leftPupils4[0].size()];
//    }
//    if(abs(rightPupils4[0][rightPupils4[0].size()].x - rightPupils4[1][0].x) > pupils_eps)
//    {
//        pupils_errors_R.push_back(rightPupils4[1][0].x);
//        rightPupils4[1][0] = rightPupils4[0][rightPupils4[0].size()];
//    }
//    if(abs(leftPupils4[0][leftPupils4[0].size()].y - leftPupils4[1][0].y) > pupils_eps)
//    {
//        pupils_errors_L.push_back(leftPupils4[1][0].y);
//        leftPupils4[1][0] = leftPupils4[0][leftPupils4[0].size()];
//    }
//    if(abs(rightPupils4[0][rightPupils4[0].size()].y - rightPupils4[1][0].y) > pupils_eps)
//    {
//        pupils_errors_R.push_back(rightPupils4[1][0].y);
//        rightPupils4[1][0] = rightPupils4[0][rightPupils4[0].size()];
//    }



//    for (size_t i = 0; i < leftPupils4[1].size() - 1; i++)
//    {
//        if(abs(leftPupils4[0][i].x - leftPupils4[0][i+1].x) > pupils_eps)
//        {
//            pupils_errors_L.push_back(leftPupils4[0][i+1]);
//            leftPupils4[0][i+1] = leftPupils4[0][i];
//        }
//        if(abs(rightPupils4[0][i].x - rightPupils4[0][i+1].x) > pupils_eps)
//        {
//            pupils_errors_R.push_back(rightPupils4[0][i+1]);
//            rightPupils4[0][i+1] = rightPupils4[0][i];
//        }
//        if(abs(leftPupils4[0][i].y - leftPupils4[0][i+1].y) > pupils_eps)
//        {
//            pupils_errors_L.push_back(leftPupils4[0][i+1]);
//            leftPupils4[0][i+1] = leftPupils4[0][i];
//        }
//        if(abs(rightPupils4[0][i].y - rightPupils4[0][i+1].y) > pupils_eps)
//        {
//            pupils_errors_R.push_back(rightPupils4[0][i+1]);
//            rightPupils4[0][i+1] = rightPupils4[0][i];
//        }
//    }

//    for (size_t i = 0; i < pupilsLeft.size() - 1; i++)
//    {
//        if(abs(pupilsLeft[i].x - pupilsLeft[i+1].x) > pupils_eps)
//        {
//            pupils_errors_L.push_back(pupilsLeft[i+1]);
//            pupilsLeft[i+1] = pupilsLeft[i];
//        }
//        if(abs(pupilsRight[i].x - pupilsRight[i+1].x) > pupils_eps)
//        {
//            pupils_errors_R.push_back(pupilsRight[i+1]);
//            pupilsRight[i+1] = pupilsRight[i];
//        }
//        if(abs(pupilsLeft[i].y - pupilsLeft[i+1].y) > pupils_eps)
//        {
//            pupils_errors_L.push_back(pupilsLeft[i+1]);
//            pupilsLeft[i+1] = pupilsLeft[i];
//        }
//        if(abs(pupilsRight[i].y - pupilsRight[i+1].y) > pupils_eps)
//        {
//            pupils_errors_R.push_back(pupilsRight[i+1]);
//            pupilsRight[i+1] = pupilsRight[i];
//        }
//    }

    int k=0;
    for (size_t i = 0; i < leftPupils4[0].size(); i++, k++)
    {
        pupilsLeft[k] = leftPupils4[0][i];
        pupilsRight[k] = rightPupils4[0][i];
    }
    for (size_t i = 0; i < leftPupils4[1].size(); i++, k++)
    {
        pupilsLeft[k] = leftPupils4[1][i];
        pupilsRight[k] = rightPupils4[1][i];
    }
    for (size_t i = 0; i < leftPupils4[2].size(); i++, k++)
    {
        pupilsLeft[k] = leftPupils4[2][i];
        pupilsRight[k] = rightPupils4[2][i];
    }
    for (size_t i = 0; i < leftPupils4[3].size(); i++, k++)
    {
        pupilsLeft[k] = leftPupils4[3][i];
        pupilsRight[k] = rightPupils4[3][i];
    }


//    for (size_t i = 0; i < pupilsLeft.size(); i++)
//    {
//        if (leftPupils4[0].size() > 0) {
//            pupilsLeft[i] = leftPupils4[0][];
//            leftPupils4[0].erase(leftPupils4[0].begin());
//            pupilsRight[i] = rightPupils4[0][0];
//            rightPupils4[0].erase(rightPupils4[0].begin());
//        }
//        else if (leftPupils4[1].size() > 0){
//            pupilsLeft[i] = leftPupils4[1][0];
//            leftPupils4[1].erase(leftPupils4[1].begin());
//            pupilsRight[i] = rightPupils4[1][0];
//            rightPupils4[1].erase(rightPupils4[1].begin());
//        }
//        else if (leftPupils4[2].size() > 0){
//            pupilsLeft[i] = leftPupils4[2][0];
//            leftPupils4[2].erase(leftPupils4[2].begin());
//            pupilsRight[i] = rightPupils4[2][0];
//            rightPupils4[2].erase(rightPupils4[2].begin());
//        }
//        else if (leftPupils4[3].size() > 0){
//            pupilsLeft[i] = leftPupils4[3][0];
//            leftPupils4[3].erase(leftPupils4[3].begin());
//            pupilsRight[i] = rightPupils4[3][0];
//            rightPupils4[3].erase(rightPupils4[3].begin());
//        }
//    }

}


void CalibrationMotion::eliminateOutlyingBlips(vector<Point>& storona, int side, vector<Point>& pupils_errors)
{
    /*vector<int> vct ;
    double criticalLevel, averageValue, S, U;

    if (side == 1 || side == 3)
    {
        for (size_t i = 0; i < storona.size(); i++)
        {
            vct.push_back(storona[i].x);// = storona[i].x;
        }
    }
    else
    {
        for (size_t i = 0; i < storona.size(); i++)
        {
            vct.push_back(storona[i].y);//vct[i] = storona[i].y;
        }
    }

    bool haveEjection = true;
    criticalLevel = getCriticalLevel(vct.size(), 1);

    while (haveEjection) {
        haveEjection = false;
        averageValue = GetAverageVal(vct);
        S = GetStandardDeviation(vct, averageValue);
        for (size_t i = 0; i < vct.size(); i++)
        {
            U = (abs(vct[i] - averageValue)) / S;
            if (U > criticalLevel) {
                if (i == 0)
                    vct[i] = vct[i + 1];
                else
                    vct[i] = vct[i - 1];
                haveEjection = true;
                break;
            }
        }
    }*/

    double criticalLevel, averageValue, S, U;

    bool haveEjection = true;
    criticalLevel = getCriticalLevel(storona.size(), 1);

    if (side == 1 || side == 3)
    {
        while (haveEjection) {
            haveEjection = false;
            averageValue = GetAverageVal2(storona, true);
            S = GetStandardDeviation2(storona, averageValue, true);
            for (size_t i = 0; i < storona.size(); i++)
            {
                U = (abs(storona[i].x - averageValue)) / S;
                if (U > criticalLevel)
                {
                    if (i == 0)
                    {
                        pupils_errors.push_back(storona[i]);
                        //storona.erase(storona.begin() + i);
                        storona[i].x = storona[i + 1].x;
                        storona[i].y = storona[i + 1].y;
                    }
                    else
                    {
                        pupils_errors.push_back(storona[i]);
                        storona[i].x = storona[i - 1].x;
                        storona[i].y = storona[i - 1].y;
                    }
                    haveEjection = true;
                    break;
                }
            }
        }
    }
    else
    {
        while (haveEjection) {
            haveEjection = false;
            averageValue = GetAverageVal2(storona, false);
            S = GetStandardDeviation2(storona, averageValue, false);
            for (size_t i = 0; i < storona.size(); i++)
            {
                U = (abs(storona[i].y - averageValue)) / S;
                if (U > criticalLevel)
                {
                    if (i == 0)
                    {
                        pupils_errors.push_back(storona[i]);
                        //storona.erase(storona.begin() + i);
                        storona[i].x = storona[i + 1].x;
                        storona[i].y = storona[i + 1].y;
                    }
                    else
                    {
                        pupils_errors.push_back(storona[i]);
                        storona[i].x = storona[i - 1].x;
                        storona[i].y = storona[i - 1].y;
                    }
                    haveEjection = true;
                    break;
                }
            }
        }
    }
/*

    if (side == 1 || side == 3)
    {
        for (size_t i = 0; i < storona.size(); i++)
        {
            storona[i].x = vct[i];
        }
    }
    else
    {
        for (size_t i = 0; i < storona.size(); i++)
        {
            storona[i].y = vct[i];
        }
    }*/
}

double CalibrationMotion::GetAverageVal(vector<int> vct)
{
    double avg = 0;
    for (size_t i = 0; i < vct.size(); i++)
    {
        avg += vct[i];
    };
    return avg / vct.size();
}

double CalibrationMotion::GetAverageVal2(vector<Point>& vct, bool isTopOrBottom)
{
    double avg = 0;
    if (isTopOrBottom)
    {
        for (size_t i = 0; i < vct.size(); i++)
        {
            avg += vct[i].x;
        };
    }
    else
    {
        for (size_t i = 0; i < vct.size(); i++)
        {
            avg += vct[i].y;
        };
    }
    return avg / vct.size();
}

double CalibrationMotion::GetStandardDeviation2(vector<Point>& vct, double averageValue, bool isTopOrBottom)
{
    double S = 0;
    if (isTopOrBottom)
    {
        for (size_t i = 0; i < vct.size(); i++)
        {
            S += pow((vct[i].x - averageValue), 2);
        }
    }
    else
    {
        for (size_t i = 0; i < vct.size(); i++)
        {
            S += pow((vct[i].y - averageValue), 2);
        }
    }
    S = sqrt(S / (vct.size() - 1));
    return S;
}

double CalibrationMotion::GetStandardDeviation(vector<int> vct, double averageValue)
{
    double S = 0;
    for (size_t i = 0; i < vct.size(); i++)
    {
        S += pow((vct[i] - averageValue), 2);
    }
    S = sqrt(S / (vct.size() - 1));
    return S;
}

// http ://termist.com/laborat/stat/otsew/stepnow_01.htm#smirnow
double CalibrationMotion::getCriticalLevel(int size, float alfa)
{
    double critLevel = 2.5;
    if (size >= 10 && size < 18) {
        if (alfa == 1)  critLevel = 2.35;
        if (alfa == 5)  critLevel = 2.59;
        if (alfa == 10)  critLevel = 3.07;
    }
    if (size >= 18 && size < 20) {
        if (alfa == 1)  critLevel = 2.46;
        if (alfa == 5)  critLevel = 2.69;
        if (alfa == 10)  critLevel = 3.17;
    }
    if (size >= 20 && size < 23) {
        if (alfa == 1)  critLevel = 2.56;
        if (alfa == 5)  critLevel = 2.78;
        if (alfa == 10)  critLevel = 3.26;
    }
    if (size >= 23 && size < 25) {
        if (alfa == 1)  critLevel = 2.59;
        if (alfa == 5)  critLevel = 2.82;
        if (alfa == 10)  critLevel = 3.28;
    }
    if (size >= 25 && size < 30) {
        if (alfa == 1)  critLevel = 2.70;
        if (alfa == 5)  critLevel = 2.93;
        if (alfa == 10)  critLevel = 3.40;
    }
    if (size >= 30 && size < 40) {
        if (alfa == 1)  critLevel = 2.70;
        if (alfa == 5)  critLevel = 2.93;
        if (alfa == 10)  critLevel = 3.40;
    }
    if (size >= 40 && size < 50) {
        if (alfa == 1)  critLevel = 2.79;
        if (alfa == 5)  critLevel = 3.02;
        if (alfa == 10)  critLevel = 3.48;
    }
    if (size >= 50 && size < 100) {
        if (alfa == 1)  critLevel = 3.08;
        if (alfa == 5)  critLevel = 3.29;
        if (alfa == 10)  critLevel = 3.72;
    }
    if (size >= 100 && size < 250) {
        if (alfa == 1)  critLevel = 3.34;
        if (alfa == 5)  critLevel = 3.53;
        if (alfa == 10)  critLevel = 3.95;
    }
    if (size >= 250) {
        if (alfa == 1)  critLevel = 3.53;
        if (alfa == 5)  critLevel = 3.70;
        if (alfa == 10)  critLevel = 4.11;
    }
    return critLevel;
}


void CalibrationMotion::directoryChangedSlot(int directory)
{
    this->directory = directory;
}

void CalibrationMotion::clearVariables()
{
    calibrationCols = 0;
    calibrationRows = 0;
    pupilAvg.clear();
    avgEyeRect = Rect();
    directory = 0;
}

void MyLine::initKoeffs_via2points(Point2d p0, Point2d p1)
{
    ///
    /// уравнение прямой в виде:
    /// Ax + By - C = 0
    /// или
    /// x*cos(u) + y*sin(u) - p = 0;
    ///
    /// чтобы перевести из 1 формы в форму 2, нужно умножить всё на mu = +- 1/sqrt(A*A + B*B)
    /// знак выбрать такой, чтобы mu * C < 0
    ///

    A = p0.y-p1.y;
    B = p1.x-p0.x;
    C = - p0.y * (p1.x-p0.x) + p0.x * (p1.y-p0.y);

    mu = 1 / sqrt(A*A + B*B);

    if (mu * C > 0)
        mu = 0 - mu;

    sinu = B * mu;
    cosu = A * mu;

    u = asin(sinu);

    if (cosu != 0)
    {
        tgu = sinu/cosu;
        u = atan(tgu);
    }
    else
    {
        tgu = numeric_limits<double>::max();
        // pi/2
        u = 1.5707963267;
    }

    // 0 - "C*mu" тк в формуле было + С, а стало - р
    p = 0 - C * mu;

    if (false)
    {
        logWrite(string("!check6. A is ") +func::to_string(A));
        logWrite(string("!check6. B is ") +func::to_string(B));
        logWrite(string("!check6. C is ") +func::to_string(C));
        logWrite(string("!check6. mu is ") +func::to_string(mu));
        logWrite(string("!check6. mu is ") +func::to_string(mu));
        logWrite(string("!check6. u is ") +func::to_string(u));
        logWrite(string("!check6. p is ") +func::to_string(p));
        logWrite(string("!check6. sinu is ") +func::to_string(sinu));
        logWrite(string("!check6. asin(sinu is ") +func::to_string(asin(sinu)));
        logWrite(string("!check6. cosu is ") +func::to_string(cosu));
    }
}


double MyLine::distanceBetweenPoints(Point p1, Point p2)
{
    return sqrt(pow(p1.x - p2.x,2) + pow(p1.y - p2.y,2));
}

void MyLine::initKoeffs_viaPointAndU(Point2d p0, double _u)
{
    u = _u;
    sinu = sin(u);
    cosu = cos(u);

    p = p0.x * cosu + p0.y * sinu;

    A = cosu;
    B = sinu;
    C = p;
}

Point2d MyLine::getCrossPoint(MyLine line1, MyLine line2)
{
//    //точка пересечения line1 и line2
//    double p_y = (line2.p - line1.p*line2.cosu) / ( line2.sinu * line1.cosu - line1.sinu*line2.cosu );
//    double p_x = (line1.p - p_y*line1.sinu) / line1.cosu;

    //logWrite(string("!check6. p_y is ") +func::to_string(p_y));
    //logWrite(string("!check6. p_x is ") +func::to_string(p_x));

    //точка пересечения line1 и line2
    double p_x2 = ((line2.p * line1.sinu) - (line1.p*line2.sinu)) / ( (line2.cosu * line1.sinu) - (line1.cosu *line2.sinu));

    //неправильно. ошибка при sinu = 0
    //double p_y2 = (line1.p - p_x2*line1.cosu) / line1.sinu;
    double p_y2 = ( (line1.cosu*line2.p) - (line1.p*line2.cosu) ) / ( (line2.sinu*line1.cosu) - (line1.sinu * line2.cosu) );

    //logWrite(string("!check6. p_y2 is ") +func::to_string(p_y2));
    //logWrite(string("!check6. p_x2 is ") +func::to_string(p_x2));

    return Point2d(p_x2, p_y2);
}
