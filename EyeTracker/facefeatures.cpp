#include "facefeatures.h"

//сколько последних кадров усреднять, для нахождения средней точки
#define MEAN_POINT_COUNT 3

bool check_point_in_mat(cv::Point p,int rows,int cols);
void make_unit_vector(std::vector<double> vec, std::vector<double>& unit_vector);
bool is_point_in_mat(const cv::Point &np, const cv::Mat &mat);
void draw_eye_gaze(cv::Point pt, std::vector<double> vec_gaze, cv::Rect roi_eye, cv::Mat& img, cv::Rect& faceRect, int scale);
void project_facial_pose(cv::Mat img, double normal[3], double sigma, double theta);
void draw_facial_normal(cv::Mat& img, core::Shape shape, std::vector<double> normal, double mag);
void blow_up_rect(cv::Rect& rect, double f);
cv::Point2f get_mid_point(cv::Point2f p1, cv::Point2f p2);
double get_distance(cv::Point p1, cv::Point p2);
double find_sigma(int ln, int lf, double Rn, double theta);
double get_angle_between(cv::Point pt1, cv::Point pt2);
double get_conversion_factor (double magnitude_normal, int mode, core::Shape s, double* normal);

std::vector<double> vec_ce_pos_l(3), vec_ce_vel_l(3), vec_ce_pos_l_old(3), vec_ce_vel_l_old(3), vec_ce_kalman_l(3);
std::vector<double> vec_ep_pos_l(3), vec_ep_vel_l(3), vec_ep_pos_l_old(3), vec_ep_vel_l_old(3), vec_ep_kalman_l(3);
//std::vector<double> vec_cp_pos_l(3), vec_cp_vel_l(3), vec_cp_pos_l_old(3), vec_cp_vel_l_old(3), vec_cp_kalman_l(3);

std::vector<double> vec_ce_pos_r(3), vec_ce_vel_r(3), vec_ce_pos_r_old(3), vec_ce_vel_r_old(3), vec_ce_kalman_r(3);
std::vector<double> vec_ep_pos_r(3), vec_ep_vel_r(3), vec_ep_pos_r_old(3), vec_ep_vel_r_old(3), vec_ep_kalman_r(3);
//std::vector<double> vec_cp_pos_r(3), vec_cp_vel_r(3), vec_cp_pos_r_old(3), vec_cp_vel_r_old(3), vec_cp_kalman_r(3);

std::vector<double> center_eye_proj(3);
std::vector<double> vec_cp_kalman_avg(3);

std::vector<std::vector<double> > vec_kmeans_centers_l;
std::vector<float> vec_kmeans_data_l;

cv::Point pt_p_pos_l(0,0), pt_p_vel_l(0,0), pt_p_pos_l_old(0,0), pt_p_kalman_l(0,0), pt_p_vel_l_old(0,0);
cv::Point pt_e_pos_l(0,0), pt_e_vel_l(0,0), pt_e_pos_l_old(0,0), pt_e_kalman_l(0,0);

cv::Point pt_p_pos_r(0,0), pt_p_vel_r(0,0), pt_p_pos_r_old(0,0), pt_p_kalman_r(0,0), pt_p_vel_r_old(0,0);
cv::Point pt_e_pos_r(0,0), pt_e_vel_r(0,0), pt_e_pos_r_old(0,0), pt_e_kalman_r(0,0);

int k_pt_e_l = 0, k_pt_p_l = 0, k_vec_ce_l = 0, k_vec_cp_l = 0, k_vec_ep_l = 0;
int k_pt_e_r = 0, k_pt_p_r = 0, k_vec_ce_r = 0, k_vec_cp_r = 0, k_vec_ep_r = 0;

//double Cf_left, Cf_right, mag_nor = 12.0, alpha = 30.0;
double mag_nor = 12.0;

FaceFeatures::FaceFeatures()
{
    settings = new Settings();

    //TODO : Initialize all vectors to [0, 0, 0];
    vec_ce_pos_l[0] = 0;vec_ce_pos_l[1] = 0;vec_ce_pos_l[2] = 0;
    vec_ce_pos_l_old[0] = 0;vec_ce_pos_l_old[1] = 0;vec_ce_pos_l_old[2] = 0;

    vec_ce_pos_r[0] = 0;vec_ce_pos_r[1] = 0;vec_ce_pos_r[2] = 0;
    vec_ce_pos_r_old[0] = 0;vec_ce_pos_r_old[1] = 0;vec_ce_pos_r_old[2] = 0;


    vec_ep_pos_l[0] = 0;vec_ep_pos_l[1] = 0;vec_ep_pos_l[2] = 0;
    vec_ep_pos_l_old[0] = 0;vec_ep_pos_l_old[1] = 0;vec_ep_pos_l_old[2] = 0;

    vec_ep_pos_r[0] = 0;vec_ep_pos_r[1] = 0;vec_ep_pos_r[2] = 0;
    vec_ep_pos_r_old[0] = 0;vec_ep_pos_r_old[1] = 0;vec_ep_pos_r_old[2] = 0;


    //vec_cp_pos_l[0] = 0;vec_cp_pos_l[1] = 0;vec_cp_pos_l[2] = 0;
    //vec_cp_pos_l_old[0] = 0;vec_cp_pos_l_old[1] = 0;vec_cp_pos_l_old[2] = 0;

    //vec_cp_pos_r[0] = 0;vec_cp_pos_r[1] = 0;vec_cp_pos_r[2] = 0;
   // vec_cp_pos_r_old[0] = 0;vec_cp_pos_r_old[1] = 0;vec_cp_pos_r_old[2] = 0;

    eyeContours.push_back(vector<Point>());
    eyeContours.push_back(vector<Point>());
    for(int i=0; i<6;i++)
    {
        eyeContours[0].push_back(Point());
        eyeContours[1].push_back(Point());
    }
}

bool FaceFeatures::init()
{
    frameCount = 0;
    detectRate = 1;

    //Dest Tracker
    if(!t.load("res/dest_tracker_VJ_ibug.bin"))
    //if(!t.load("res/dest_tracker_VJ_HELEN.bin"))
        return false;

    //face init

     face_cascade.load("res/haarcascade_frontalface_alt2.xml");
     if (face_cascade.empty())
         return false;
     return true;
}

bool FaceFeatures::getFaceCoords_Haar(Mat frame)
{
    //Mat gray;
    //cvtColor(frame, gray, COLOR_BGR2GRAY);
    equalizeHist(frame, frame);

    try {
        vector<Rect> faces;
        face_cascade.detectMultiScale(frame, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));

        if (faces.size() > 0)
        {
            //todo: получить лучший результат
            faceRectViol = faces[0];

            return true;
        }
    }
    catch (Exception ex) {
    }

    return false;
}

bool FaceFeatures::initFaceFeatures(Mat img)
{
    currentFrame = img.clone();
    cv::Mat grayCV;
    dest::core::Image imgDest;

    cv::cvtColor(img, grayCV, CV_BGR2GRAY);
    dest::util::toDest(grayCV, imgDest);

    const bool isDetectFrame = (frameCount % detectRate == 0);
    if (isDetectFrame || (settings->InputType == settings->it_image))
    {
        if (getFaceCoords_Haar(grayCV.clone()))
        {
            dest::util::toDest(faceRectViol, r);
            shapeToImage = dest::core::estimateSimilarityTransform(dest::core::unitRectangle(), r);
            s = t.predict(imgDest, shapeToImage);

            if (firstFaceRect.height == 0 || (settings->InputType == settings->it_image))
            {
                 firstFaceRect = faceRectViol;
                 faceRect = firstFaceRect;
                 firstMeanPoint = calcMeanPoint();
                 initEyeRects();
            }

            meanPoint = calcMeanPoint();
            meanPoints.push_back(meanPoint);

            if(meanPoints.size() == MEAN_POINT_COUNT)
            {
                Point mean_point;
                for(vector<Point>::iterator it = meanPoints.begin(); it != meanPoints.end(); ++it)
                    mean_point += (*it);

                mean_point.x /= meanPoints.size();
                mean_point.y /= meanPoints.size();
                meanPoint = mean_point;
                meanPoints.erase(meanPoints.begin());
            }

            calcFaceAndEyeRects(meanPoint);
            //initFacePose();

            //найти точки глаз и записать их в переменные
            calcEyeShapes();

            if(!checkFaceEyes(grayCV))
            {
                logWrite("checkFaceEyes returned false");
                detectSuccess = false;
                return false;
            }

            detectSuccess = true;
        }
        else
        {
            logWriteError("getFaceCoords_Haar returned false");
            detectSuccess = false;
            return false;
        }
    }
    if (!isDetectFrame && detectSuccess)
    {
        // Mimick detector behaviour. Only works for OpenCV face detectors.
        r = dest::core::shapeBounds(s);
        shapeToImage = dest::core::estimateSimilarityTransform(dest::core::unitRectangle(), r);
        Eigen::AffineCompact2f tr;
        tr.setIdentity();
        tr = Eigen::Translation2f(txToCV * imgDest.cols(), tyToCV * imgDest.rows()) *
                Eigen::Translation2f(shapeToImage.translation()) *
                Eigen::Scaling(scaleToCV) *
                Eigen::Translation2f(-shapeToImage.translation());
        r = tr * r.colwise().homogeneous();

        shapeToImage = dest::core::estimateSimilarityTransform(dest::core::unitRectangle(), r);
        s = t.predict(imgDest, shapeToImage);

        initFacePose();
    }

    // showResults вызывается после определения центров зрачков в assignPoints(vector<Point> points)
    if (settings->showImage)
        showResults(currentFrame);

    if(frameCount == detectRate)
        frameCount = 0;
    else
        frameCount++;

    return true;
}

void FaceFeatures::showResults(Mat grayCV)
{
    //drawContours(grayCV, eyeContours, 0, Scalar(0,0,255), 1);
    //drawContours(grayCV, eyeContours, 1, Scalar(0,0,255), 1);

    dest::util::drawShape(grayCV, s, 2);
    //dest::util::drawShapeText(grayCV, s, 2);

    //точка-центр (рассчитывается в методе calcMeanPoint)
    circle(grayCV, meanPoint,3,Scalar(0,0,255),2);
    //лицо, найденное в первый раз (сохранены размеры лица)
    rectangle(grayCV, faceRect, Scalar( 0, 180, 0), 2);
    //лицо, найденное методом Виолы-Дж
    rectangle(grayCV, faceRectViol, Scalar( 0, 0, 150), 1);

    circle(grayCV, left_eye, 2, Scalar( 222, 222, 0), -1, 8, 0 );
    circle(grayCV, right_eye, 2, Scalar( 222, 222, 0), -1, 8, 0 );
    circle(grayCV, nose_tip, 2, Scalar( 222, 222, 0), -1, 8, 0 );
    circle(grayCV, mouth, 2, Scalar( 222, 222, 0), -1, 8, 0 );
    circle(grayCV, mid_eye, 2, Scalar( 222, 222, 0), -1, 8, 0 );
    circle(grayCV, nose_base, 2, Scalar( 222, 222, 0), -1, 8, 0 );

    imshow("DEST Tracking", grayCV);

}

Point2f FaceFeatures::calcMeanPoint()
{
    cv::Point2f sum;
    Point2f mean_point;
    if (false)
    {
        for (core::Shape::Index i = 0; i < s.cols(); ++i) {
            sum += cv::Point2f(s(0, i), s(1 ,i));
        }
        mean_point = Point2f(sum.x / s.cols(), sum.y / s.cols());
    }
    else
    {
        core::Shape::Index i;
        for (i = 0; i < 6; ++i) {
            sum += cv::Point2f(s(0, i), s(1 ,i));
        }
        for (i = 11; i < 48; ++i) {
            sum += cv::Point2f(s(0, i), s(1 ,i));
        }
        mean_point = Point2f(sum.x / 42, sum.y / 42);
    }
    return mean_point;
}

//скорректировать центр и заменить найденный faceRect на firstFaceRect, чтобы сохранить размеры лица
bool FaceFeatures::calcFaceAndEyeRects(Point2f &meanp)
{
    faceRect.x = 0.5 + (firstFaceRect.x + meanp.x - firstMeanPoint.x);
    faceRect.y = 0.5 + (firstFaceRect.y + meanp.y - firstMeanPoint.y);

    eyeLeft.x = 0.5 + (firstEyeLeft.x + meanp.x - firstMeanPoint.x);
    eyeLeft.y = 0.5 + (firstEyeLeft.y + meanp.y - firstMeanPoint.y);
    eyeRight.x = 0.5 + (firstEyeRight.x + meanp.x - firstMeanPoint.x);
    eyeRight.y = 0.5 + (firstEyeRight.y + meanp.y - firstMeanPoint.y);
    return true;
}

bool FaceFeatures::checkFaceEyes(Mat img)
{
    //TODO:: вернуить false если eyeLeft и eyeRight выходят за пределы rects
    if(faceRect.x < 0) faceRect.x = 0;
    if(faceRect.y < 0) faceRect.y = 0;
    vector<Rect> rects = getEyeCoordsRegionMethod(img, faceRect);
//    Mat face_mat = img(faceRect);
//    rectangle(face_mat, rects[0], Scalar( 255, 0, 0), 1);
//    rectangle(face_mat, rects[1], Scalar( 255, 0, 0), 1);

    if(rects[0].x > eyeLeft.x || rects[1].x > eyeRight.x ||
       rects[0].y > eyeLeft.y || rects[1].y > eyeRight.y ||
       rects[0].width < eyeLeft.width || rects[1].width < eyeRight.width ||
       rects[0].height < eyeLeft.height || rects[1].height < eyeRight.height)
        return false;
    return true;
}

void FaceFeatures::calcEyeShapes()
{
    int i = 0;
    for(core::Shape::Index j=42; j<=47;j++, i++)
    {
        eyeContours[0][i].x = (int)s(0, j) - eyeLeft.x;
        eyeContours[0][i].y = (int)s(1 ,j) - eyeLeft.y;
    }
    i = 0;
    for(core::Shape::Index j=36;j<=41;j++, i++)
    {
        eyeContours[1][i].x = (int)s(0, j) - eyeRight.x;
        eyeContours[1][i].y = (int)s(1 ,j) - eyeRight.y;
    }
}

void FaceFeatures::initEyeRects()
{
    vector<Point2f> leftContour;
    vector<Point2f> rightContour;

    for(core::Shape::Index j=42;j<=47;j++)
        leftContour.push_back(Point2f(s(0, j), s(1 ,j)));

    for(core::Shape::Index j=36;j<=41;j++)
        rightContour.push_back(Point2f(s(0, j), s(1 ,j)));

    eyeLeft = cv::boundingRect(leftContour);
    eyeRight = cv::boundingRect(rightContour);

    blow_up_rect(eyeLeft, 2);
    blow_up_rect(eyeRight, 2);
    firstEyeLeft = eyeLeft;
    firstEyeRight = eyeRight;
}

void FaceFeatures::initFacePose()
{
    //init base points
    //face_centre = cv::Point2f(0, 0);
    left_eye = get_mid_point(cv::Point2f(s(0, 42), s(1 ,42)), cv::Point2f(s(0, 45), s(1 ,45)));
    right_eye = get_mid_point(cv::Point2f(s(0, 36), s(1 ,36)), cv::Point2f(s(0, 39), s(1 ,39)));
    nose_tip = cv::Point2f(s(0, 30), s(1 ,30));
    mouth = get_mid_point(cv::Point2f(s(0, 48), s(1 ,48)), cv::Point2f(s(0, 54), s(1 ,54)));
    
    mid_eye.x = (left_eye.x + right_eye.x)/2.0;
    mid_eye.y = (left_eye.y + right_eye.y)/2.0;
    
    //Find the nose base along the symmetry axis
    nose_base.x = mouth.x + (mid_eye.x - mouth.x)*0.5;
    nose_base.y = mouth.y - (mouth.y - mid_eye.y)*0.5;

    left_eye_nose_distance = get_distance(left_eye, nose_base);
    right_eye_nose_distance = get_distance(right_eye, nose_base);
    left_eye_right_eye_distance = get_distance(left_eye, right_eye);
    nose_mouth_distance = get_distance(nose_base, mouth);

    mid_eye_mouth_distance = get_distance(mid_eye, mouth);
    nose_base_nose_tip_distance = get_distance(nose_tip, nose_base);

    symm_x = get_angle_between(nose_base, mid_eye);
                //symm angle - angle between the symmetry axis and the 'x' axis
    tau = get_angle_between(nose_base, nose_tip);
                //tilt angle - angle between normal in image and 'x' axis
    theta = (abs(tau - symm_x)) * (3.141592653589/180.0);
                //theta angle - angle between the symmetry axis and the image normal
    sigma = find_sigma(nose_base_nose_tip_distance, mid_eye_mouth_distance, 0.5, theta);
            //std::cout<<"symm : "<<symm_x<<" tau : "<<tau<<" theta : "<<theta<<" sigma : "<<sigma<<" ";

    normal[0] = (sin(sigma))*(cos((360 - tau)*(3.141592653589/180.0)));
    normal[1] = (sin(sigma))*(sin((360 - tau)*(3.141592653589/180.0)));
    normal[2] = -cos(sigma);

//    kalman_pitch_pre = pitch;
//    pitch = acos(sqrt((normal[0]*normal[0] + normal[2]*normal[2])/(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2])));
//    if((nose_tip.y - nose_base.y) < 0) {
//        pitch = -pitch;
//    }

//    kalman_yaw_pre = yaw;
//    yaw = acos((abs(normal[2]))/(sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2])));
//    if((nose_tip.x - nose_base.x) < 0) {
//        yaw = -yaw;
//    }

    project_facial_pose(currentFrame, normal, sigma, theta);
}

void FaceFeatures::assignPoints(vector<Point> points)
{
    leftpupil = points[0];
    rightpupil = points[1];

    Mat roi1 = currentFrame(Rect(eyeLeft.x + faceRect.x,eyeLeft.y + faceRect.y, eyeLeft.width, eyeLeft.height));
    Mat roi2 = currentFrame(Rect(eyeRight.x + faceRect.x,eyeRight.y + faceRect.y, eyeRight.width, eyeRight.height));

    if(s.size() == 0)
    {
        //std::cout<<"zero faces"<<std::endl;
        k_pt_p_l=0;
        k_pt_e_l=0;
        k_vec_ce_l=0;
        k_vec_ep_l=0;
        k_pt_p_r=0;
        k_pt_e_r=0;
        k_vec_ce_r=0;
        k_vec_ep_r=0;
    }
    else
    {
        //TODO : Initialize the variables used in the Kalman filter
        // pt_e - центр глаза
        // pt_p - центр зрачка
        pt_p_pos_l_old = pt_p_pos_l;
        pt_p_vel_l_old = pt_p_vel_l;
        pt_e_pos_l_old = pt_e_pos_l;

        pt_p_pos_r_old = pt_p_pos_r;
        pt_p_vel_r_old = pt_p_vel_r;
        pt_e_pos_r_old = pt_e_pos_r;

        vec_ce_pos_l_old = vec_ce_pos_l;
        vec_ep_pos_l_old = vec_ep_pos_l;
        //vec_cp_pos_l_old = vec_cp_pos_l;

        vec_ce_pos_r_old = vec_ce_pos_r;
        vec_ep_pos_r_old = vec_ep_pos_r;
        //vec_cp_pos_r_old = vec_cp_pos_r;

        //Cf_left = get_conversion_factor(alpha, 1, s, normal);
        //Cf_right = get_conversion_factor(alpha, 2, s, normal);

        pt_e_pos_l.x -= eyeLeft.x;
        pt_e_pos_l.y -= eyeLeft.y;
        pt_e_vel_l.x = pt_e_pos_l.x - pt_e_pos_l_old.x;
        pt_e_vel_l.y = pt_e_pos_l.y - pt_e_pos_l_old.y;

        pt_e_pos_r.x -= eyeRight.x;
        pt_e_pos_r.y -= eyeRight.y;
        pt_e_vel_r.x = pt_e_pos_r.x - pt_e_pos_r_old.x;
        pt_e_vel_r.y = pt_e_pos_r.y - pt_e_pos_r_old.y;

        if(k_pt_e_l == 0) {
            pt_e_pos_l_old.x = 0;
            pt_e_pos_l_old.y = 0;
            init_kalman_point_e_l(pt_e_pos_l);
            ++k_pt_e_l;
        }

        if(k_pt_e_r == 0) {
            pt_e_pos_r_old.x = 0;
            pt_e_pos_r_old.y = 0;
            init_kalman_point_e_r(pt_e_pos_r);
            ++k_pt_e_r;
        }

        pt_e_kalman_l = kalman_correct_point_e_l(pt_e_pos_l, pt_e_pos_l_old);
        pt_e_kalman_r = kalman_correct_point_e_r(pt_e_pos_r, pt_e_pos_r_old);

        pt_p_pos_l = leftpupil;
        pt_p_vel_l.x = pt_p_pos_l.x - pt_p_pos_l_old.x;
        pt_p_vel_l.y = pt_p_pos_l.y - pt_p_pos_l_old.y;

        pt_p_pos_r = rightpupil;
        pt_p_vel_r.x = pt_p_pos_r.x - pt_p_pos_r_old.x;
        pt_p_vel_r.y = pt_p_pos_r.y - pt_p_pos_r_old.y;

        if(k_pt_p_l == 0) {
            pt_p_pos_l_old.x = 0;
            pt_p_pos_l_old.y = 0;
            init_kalman_point_p_l(pt_p_pos_l);
            ++k_pt_p_l;
        }
        if(k_pt_p_r == 0) {
            pt_p_pos_r_old.x = 0;
            pt_p_pos_r_old.y = 0;
            init_kalman_point_p_r(pt_p_pos_r);
            ++k_pt_p_r;
        }

        pt_p_kalman_l = kalman_correct_point_p_l(pt_p_pos_l, pt_p_pos_l_old, pt_p_vel_l);
        pt_p_kalman_r = kalman_correct_point_p_r(pt_p_pos_r, pt_p_pos_r_old, pt_p_vel_r);

        if(!is_point_in_mat(pt_p_kalman_l, roi1)) {
            k_pt_p_l=0;
            k_pt_e_l=0;
            k_vec_ce_l=0;
            k_vec_ep_l=0;
        }
        if(!is_point_in_mat(pt_p_kalman_r, roi2)) {
            k_pt_p_r=0;
            k_pt_e_r=0;
            k_vec_ce_r=0;
            k_vec_ep_r=0;
        }

        circle(roi1, pt_p_kalman_l, 0, Scalar( 222, 222, 222), -1, 8, 0 );
        circle(roi2, pt_p_kalman_r, 0, Scalar( 222, 222, 222), -1, 8, 0 );
        circle(roi1, leftpupil, 0, Scalar( 0, 0, 222), -1, 8, 0 );
        circle(roi2, rightpupil, 0, Scalar( 0, 0, 222), -1, 8, 0 );
        imshow("roi1", roi1);
        imshow("roi2", roi2);

        //позиция лица на кадре
        vec_ce_pos_l[0] = normal[0];
        vec_ce_pos_l[1] = normal[1];
        vec_ce_pos_l[2] = normal[2];

        vec_ce_pos_r[0] = normal[0];
        vec_ce_pos_r[1] = normal[1];
        vec_ce_pos_r[2] = normal[2];

        vec_ce_vel_l[0] = vec_ce_pos_l[0] - vec_ce_pos_l_old[0];
        vec_ce_vel_l[1] = vec_ce_pos_l[1] - vec_ce_pos_l_old[1];
        vec_ce_vel_l[2] = vec_ce_pos_l[2] - vec_ce_pos_l_old[2];

        vec_ce_vel_r[0] = vec_ce_pos_r[0] - vec_ce_pos_r_old[0];
        vec_ce_vel_r[1] = vec_ce_pos_r[1] - vec_ce_pos_r_old[1];
        vec_ce_vel_r[2] = vec_ce_pos_r[2] - vec_ce_pos_r_old[2];

        if(k_vec_ce_l == 0) {
            vec_ce_pos_l_old[0] = 0;vec_ce_pos_l_old[1] = 0;vec_ce_pos_l_old[2] = 0;
            init_kalman_ce_l(vec_ce_pos_l);
            ++k_vec_ce_l;
        }

        if(k_vec_ce_r == 0) {
            vec_ce_pos_r_old[0] = 0;vec_ce_pos_r_old[1] = 0;vec_ce_pos_r_old[2] = 0;
            init_kalman_ce_r(vec_ce_pos_r);
            ++k_vec_ce_r;
        }

        kalman_predict_correct_ce_l(vec_ce_pos_l, vec_ce_pos_l_old, vec_ce_kalman_l);
        kalman_predict_correct_ce_r(vec_ce_pos_r, vec_ce_pos_r_old, vec_ce_kalman_r);

        make_unit_vector(vec_ce_pos_l, vec_ce_pos_l);
        make_unit_vector(vec_ce_kalman_l, vec_ce_kalman_l);
        make_unit_vector(vec_ce_pos_r, vec_ce_pos_r);
        make_unit_vector(vec_ce_kalman_r, vec_ce_kalman_r);

        vec_ep_pos_l[0] = pt_p_kalman_l.x - pt_e_kalman_l.x;
        vec_ep_pos_l[1] = pt_p_kalman_l.y - pt_e_kalman_l.y;
        vec_ep_pos_l[2] = 0.0;

        vec_ep_pos_r[0] = pt_p_kalman_r.x - pt_e_kalman_r.x;
        vec_ep_pos_r[1] = pt_p_kalman_r.y - pt_e_kalman_r.y;
        vec_ep_pos_r[2] = 0.0;

        vec_ep_pos_l[0] = pt_p_pos_l.x - pt_e_pos_l.x;
        vec_ep_pos_l[1] = pt_p_pos_l.y - pt_e_pos_l.y;
        vec_ep_pos_l[2] = 0.0;

        vec_ep_pos_r[0] = pt_p_pos_r.x - pt_e_pos_r.x;
        vec_ep_pos_r[1] = pt_p_pos_r.y - pt_e_pos_r.y;
        vec_ep_pos_r[2] = 0.0;

        if(k_vec_ep_l == 0) {
            vec_ep_pos_l_old[0] = 0;
            vec_ep_pos_l_old[1] = 0;
            vec_ep_pos_l_old[2] = 0;
            init_kalman_ep_l(vec_ep_pos_l);++k_vec_ep_l;
        }
        if(k_vec_ep_r == 0) {
            vec_ep_pos_r_old[0] = 0;
            vec_ep_pos_r_old[1] = 0;
            vec_ep_pos_r_old[2] = 0;
            init_kalman_ep_r(vec_ep_pos_r);
            ++k_vec_ep_r;
        }
        kalman_predict_correct_ep_l(vec_ep_pos_l, vec_ep_pos_l_old, vec_ep_kalman_l);
        kalman_predict_correct_ep_r(vec_ep_pos_r, vec_ep_pos_r_old, vec_ep_kalman_r);

        draw_facial_normal(currentFrame, s, vec_ce_kalman_l, 5*mag_nor);
    }
    //showResults(currentFrame);
}

void draw_eye_gaze(cv::Point pt, std::vector<double> vec_gaze, cv::Rect roi_eye, cv::Mat& img, cv::Rect& faceRect, int scale)
{
    double del_x = scale*vec_gaze[0];
    double del_y = scale*vec_gaze[1];

    cv::line(img, cv::Point(pt.x + roi_eye.x + faceRect.x, pt.y + roi_eye.y + faceRect.y), cv::Point(pt.x + del_x + roi_eye.x + faceRect.x, pt.y + del_y + roi_eye.y + faceRect.y), cv::Scalar(255, 255, 255), 1);
}

void project_facial_pose(cv::Mat img, double normal[3], double sigma, double theta) {

    cv::Point origin = cv::Point(50,50);
    cv::Scalar colour = cv::Scalar(255);

    cv::Point projection_2d;
    projection_2d.x = origin.x + cvRound(60*(normal[0]));
    projection_2d.y = origin.y + cvRound(60*(normal[1]));

    if (normal[0] > 0 && normal[1] < 0)
    {
        cv::ellipse(img, origin, cv::Size(25,std::abs(cvRound(25-sigma*(180/(2*3.141592653589))))) ,
                    std::abs(180-(theta*(180/3.141592653589))), 0, 360, colour, 2,4,0);
    }
    else
    {
        cv::ellipse(img, origin, cv::Size(25,std::abs(cvRound(25-sigma*(180/(2*3.141592653589))))) ,
                    std::abs(theta*(180/3.141592653589)), 0, 360, colour, 2,4,0);
    }

    cv::line(img, origin, projection_2d, colour, 2, 4, 0);
}

void draw_facial_normal(cv::Mat& img, core::Shape shape, std::vector<double> normal, double mag) {

    double del_x = mag*normal[0];
    double del_y = mag*normal[1];

    cv::line(img, cv::Point2f(shape(0, 30), shape(1, 30)),
    cv::Point2f(shape(0, 30) + del_x, shape(1, 30) + del_y), cv::Scalar(0), 3);
    //std::cout<<"magnitude : "<<vectorMagnitude(f->normal, 3)<<" ";
    //std::cout<<f->normal[0]<<", "<<f->normal[1]<<", "<<f->normal[2];
    //std::cout<<"  pitch "<<f->pitch*180.0/PI<<" , yaw  "<<f->yaw*180.0/PI<<std::endl;

}

void make_unit_vector(std::vector<double> vec, std::vector<double>& unit_vector) {

    double magnitude = 0;

    for(uint i = 0; i < vec.size(); i++) {
        magnitude += vec[i]*vec[i];
    }
    magnitude = sqrt(magnitude);

    for(uint i = 0; i < vec.size(); i++) {
        unit_vector[i] = (((double)(vec[i])/magnitude));
    }
}

bool is_point_in_mat(const cv::Point &np, const cv::Mat &mat) {
    return check_point_in_mat(np, mat.rows, mat.cols);
}

bool check_point_in_mat(cv::Point p,int rows,int cols) {
    return p.x >= 0 && p.x < cols && p.y >= 0 && p.y < rows;
}

double find_sigma(int ln, int lf, double Rn, double theta) {
    double dz=0;
    double sigma;
    double m1 = ((double)ln*ln)/((double)lf*lf);
    double m2 = (cos(theta))*(cos(theta));

    if (m2 == 1)
    {
        dz = sqrt(	(Rn*Rn)/(m1 + (Rn*Rn))	);
    }
    if (m2>=0 && m2<1)
    {
        dz = sqrt(	((Rn*Rn) - m1 - 2*m2*(Rn*Rn) + sqrt(	((m1-(Rn*Rn))*(m1-(Rn*Rn))) + 4*m1*m2*(Rn*Rn)	))/ (2*(1-m2)*(Rn*Rn))	);
    }
    sigma = acos(dz);
    return sigma;
}


double get_angle_between(cv::Point pt1, cv::Point pt2) {
    return 360 - cvFastArctan(pt2.y - pt1.y, pt2.x - pt1.x);
}

double get_distance(cv::Point p1, cv::Point p2) {
    double x = p1.x - p2.x;
    double y = p1.y - p2.y;

    return sqrt(x*x + y*y);
}

void blow_up_rect(cv::Rect& rect, double f) {
    rect.x = rect.x - (rect.width*(f - 1))/4.0;
    rect.y = rect.y - (rect.height*(f - 1))/2.0;
    rect.width = ((double)rect.width)*1.5;
    rect.height = ((double)rect.height)*f;
}

cv::Point2f get_mid_point(cv::Point2f p1, cv::Point2f p2) {
	return cv::Point2f((p1.x + p2.x)/2.0, (p1.y + p2.y)/2.0);
}

vector<Rect> FaceFeatures::getEyeCoordsRegionMethod(Mat frame_gray, Rect face)
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

void compute_eye_gaze () {
}

double get_conversion_factor (double magnitude_normal, int mode, core::Shape s, double* normal) {
    cv::Point2f p1, p2;
    //mode : 1 for left eye, 2 for right eye
    if(mode == 1) {
        p1 = cv::Point2f(s(0, 42), s(1 ,42));
        p2 = cv::Point2f(s(0, 45), s(1 ,45));
    }
    else if(mode == 2) {
        p1 = cv::Point2f(s(0, 36), s(1 ,36));
        p2 = cv::Point2f(s(0, 39), s(1 ,39));
    }

    double dx = p1.x - p2.x, dy = p1.y - p2.y;
    double temp1, temp2, beta;
    double n1 = normal[0], n2 = normal[1], n3 = normal[2];
    double beta_old = sqrt(dx*dx + dy*dy)/magnitude_normal;

    temp1 = dx*dx*(1.0 - n2*n2);
    temp2 = dy*dy*(1.0 - n1*n1);

    beta = sqrt(temp1 + temp2)/((double)(magnitude_normal*fabs(n3)));
    beta = 1.0/((double) beta);

    //std::cout<<"Beta : "<<beta<<std::endl;
    return beta;
}

double FaceFeatures::calcDistanse(int point1, int point2)
{
    return cv::sqrt( pow( s(0, point1) - s(0, point2) , 2) + pow( s(1, point1) - s(1, point2), 2) );
}
