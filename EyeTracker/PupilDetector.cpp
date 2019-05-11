#include "PupilDetector.h"
#define M_PI 3.14159265358979323846

PupilDetector::PupilDetector(Settings *settings)
{
    this->settings = settings;
}


PupilDetector::~PupilDetector()
{
}


void PupilDetector::removingGlare(Mat& eyerectRou)
{
    int morph_elem = 1, morph_size = 1, avgPoint = 0;
    Mat output;
    double min, max, avg;
    Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );

    //изоляция ярких регионов
    morphologyEx(eyerectRou, output,  CV_MOP_TOPHAT, element);
    minMaxLoc(output, &min, &max);
    //порог для ярких регионов. чем больше делитель, тем больше ярких регионов будут закрашиваться
    avg = (max - min)/4;
    //закрашивание блика средним цветом вокруг каждого яркого пиксела, найднного по функции morphologyEx
    for( int i = 1; i < eyerectRou.rows-1; i++ )
                    for( int j = 1; j < eyerectRou.cols-1; j++ )
                        if (output.at<uchar>(i,j) > avg )
                            {
                            // сумма яркости вокруг пиксела
                                for(int i2 = i-1; i2 < i+2; i2++)
                                    for(int j2 = j-1; j2 < j+2; j2++)
                                        if(i2 != i || j2 != j)
                                            avgPoint += eyerectRou.at<uchar>(i2,j2);
                                // делим на 9 а не на 8 для большей плавности закрашивания
                                eyerectRou.at<uchar>(i,j)= avgPoint/9;
                                avgPoint = 0;
                            }
}

//CDF Analysis Статья с описанием метода тут:
//http://www.iaeng.org/publication/IMECS2010/IMECS2010_pp130-133.pdf
//http://arxiv.org/ftp/arxiv/papers/1202/1202.6517.pdf
Rect PupilDetector::findEyeCDFAnalysis(Mat face, Rect eye)
{
    bool showimg = false;

    Mat eyeROIUnscaled, eyeROI, eyeROIGray;
    eyeROIUnscaled = face(eye);
    if (showimg)
        imshow("eyeROIUnscaled",eyeROIUnscaled);
    resize(eyeROIUnscaled, eyeROI, Size(255, (((float)255) / eyeROIUnscaled.cols) * eyeROIUnscaled.rows));
    if (showimg)
        imshow("eyeROIscaled",eyeROI);

    Mat eyeROI_copy = eyeROI.clone();

    ///HISTOGRAM

    /// Establish the number of bins
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 } ;
    const float* histRange = { range };

    bool uniform = true;
    bool accumulate = false;

    Mat hist;

    /// Compute the histograms:
    calcHist( &eyeROI, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );

    /////////////////////////////////////////////////

    //Draw the histograms for B, G and R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );

    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
              Scalar( 0, 0, 255), 2, 8, 0  );
    }
    /// Display
    namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
    if (showimg)
        imshow("calcHist Demo", histImage );

    /////////////////////////////////////////////////


    ///CDF CALCULATION

    //делим на кол-во пикселей - получаем вероятность, записанную в hist
    float* p;
    p = hist.ptr<float>(0);
    int area = eyeROI.cols * eyeROI.rows;
    for(int i = 0; i < 255; ++i)
    {
        p[i] /= area;
    }

    //меняем eyeROI. если яркость элемента имеет вероятность появления < value, то - белый
    double value = 0.001;
    uchar* p2;
    for(int i = 0; i < eyeROI.rows; ++i)
    {
        p2 = eyeROI.ptr<uchar>(i);
        for (int j = 0; j < eyeROI.cols; ++j)
        {
            if (hist.at<float>(p2[j]) <= value)
                p2[j] = 255;
            else
                p2[j] = 0;

        }
    }

    if (showimg)
        imshow("last_eye",eyeROI);


    ///Eroding and Dilating
    int dilation_size = 2;

    Mat element = getStructuringElement( MORPH_RECT, //MORPH_CROSS MORPH_ELLIPSE
                                         Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                         Point( dilation_size, dilation_size ) );

    dilate(eyeROI, eyeROI, element );
    erode( eyeROI, eyeROI, element );

    dilate(eyeROI, eyeROI, element );
    erode( eyeROI, eyeROI, element );
    erode( eyeROI, eyeROI, element );
    //logWrite("last_eye");
    if (showimg)
        imshow("dilate_last_eye",eyeROI);



    ///find contours

    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Detect edges using canny
    //Canny( eyeROI, canny_output, 1, 2, 3 );
    findContours(eyeROI, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    if (showimg)
        imshow("22222 dilate_last_eye",eyeROI);

    double largest_area=0;
    int largest_contour_index=0;

    vector<Point> largest_contour;
    bool cont = false;
    int max_cols = eyeROI.cols - 2;
    int max_rows = eyeROI.rows - 2;

    for( int i = 0; i< contours.size(); i++ )
    {
        for( int j = 0; j < contours[i].size(); j++ )
        {
            if (1 == contours[i][j].x || 1 == contours[i][j].y ||
                max_cols == contours[i][j].x || max_rows == contours[i][j].y)
            {
                cont = true;
                break;
            }
        }
        if (cont)
        {
            cont = false;
            continue;
        }

        double a=contourArea(contours[i],false);
        if(a>largest_area)
        {
            largest_area=a;
            largest_contour_index=i;
            //largest_contour = contours[i];
        }
    }

    RNG rng(12345);

    /// Draw contours
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    drawContours(eyeROI, contours, largest_contour_index, color, 2, 8, hierarchy, 0, Point() );

    /// Get the moment
    Moments mu = moments( contours[largest_contour_index], false );

    ///  Get the mass center:
    Point2f mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );

    circle(eyeROI, mc, 4, color, -1, 8, 0 );
    circle(eyeROI_copy, mc, 4, color, -1, 8, 0 );


    if (showimg)
        imshow("drawContours_last_eye",eyeROI);
    if (showimg)
        imshow("eyeROI_copy",eyeROI_copy);
}

vector<Point> PupilDetector::getPupilCoords_GradientMethod(vector<Rect> eyes, Mat faceROI, vector< vector<Point> > *contours, vector<double> &max_values)
{
    // переводим лицо в ЧБ-изображение(берем 3-й канал из RGB)
    //GRAY
    //Mat faceROI_gray;
    //cvtColor(faceROI, faceROI_gray, CV_BGR2GRAY);
    vector<Mat> rgbChannels(3);
    split(faceROI, rgbChannels);
    Mat faceROI_test = rgbChannels[2];

	// todo:
	// добавить нахождение уголков глаз
	// вынести вывод картинки из функции
	// добавить проверку вектора eyes, что глаза есть
	// если только 1 глаз, найти только 1 зрачок
    vector<Point> Pupils;
    vector<Point> bestPointsL, bestPointsR, bestPointsLn, bestPointsRn;

    vector< vector<Point> > contours_copy(*contours);

    Point leftPupil = findEyeCenter(faceROI_test, eyes[0], false, bestPointsL, &contours_copy.at(0), max_values[0]);
    Point rightPupil = findEyeCenter(faceROI_test, eyes[1], false, bestPointsR, &contours_copy.at(1), max_values[1]);

    //TEST берем несколько лучших кандидатов и рисуем их на изображении
    if(settings->showImage2)
    {
        Mat m1 = faceROI_test(eyes[0]);
        Mat m2 = faceROI_test(eyes[1]);

        drawContours(m1, *contours, 0, Scalar(0,0,255), 1);
        drawContours(m2, *contours, 1, Scalar(0,0,255), 1);

        for(int i = 0; i < bestPointsL.size(); i ++)
        {
            int curpros = 255/(bestPointsL.size()-1)*i;
            circle(m1, bestPointsL[i], 1, Scalar( 0, curpros, 255 - curpros));
            circle(m2, bestPointsR[i], 1, Scalar( 0, curpros, 255 - curpros));
           // circle(m1, bestPointsLn[i], 1, Scalar( 0, curpros, 255 - curpros));
          //  circle(m2, bestPointsRn[i], 1, Scalar( 0, curpros, 255 - curpros));
        }
        circle(m1, bestPointsL[0], 1, Scalar( 255, 255, 0), CV_FILLED);
        circle(m2, bestPointsR[0], 1, Scalar( 255, 255, 0), CV_FILLED);
        imshow("m1", m1);
        imshow("m2", m2);
    }

//    if(false) {
//            correctPoint_CDFAnalysis(faceROI_test(eyes[0]), leftPupil);
//            correctPoint_CDFAnalysis(faceROI_test(eyes[1]), rightPupil);
//        }

	Pupils.push_back(leftPupil);
	Pupils.push_back(rightPupil);

	return Pupils;
}

void PupilDetector::testPossibleCentersFormula2(int x, int y, const cv::Mat &weight, double gx, double gy, cv::Mat &out, Rect* area)
{
    int endRows = area->y + area->height;
    int endCols = area->x + area->width;

    for (int cy = area->y; cy < endRows; cy++) {
        double *Or = out.ptr<double>(cy);
        const unsigned char *Wr = weight.ptr<unsigned char>(cy);
        for (int cx = area->x; cx < endCols; cx++) {
            if (x == cx && y == cy) {
                continue;
            }
            // create a Vector from the possible center to the gradient origin
            double dx = x - cx;
            double dy = y - cy;
            // normalize d
            double magnitude = sqrt((dx * dx) + (dy * dy));
            dx = dx / magnitude;
            dy = dy / magnitude;
            //скалярное прозведение векторов.
            //double dotProduct = fabs(dx*gx) + fabs(dy*gy);
            double dotProduct = dx*gx + dy*gy;
            dotProduct = std::max(0.0, dotProduct);
            // square and multiply by the weight
            if (kEnableWeight) {
                Or[cx] += dotProduct * dotProduct * (Wr[cx] + 5);
            }
            else {
                Or[cx] += dotProduct * dotProduct;
            }
        }
    }
}

//!!!!!!!!!
void PupilDetector::testPossibleCentersFormula(int x, int y, const cv::Mat &weight, double gx, double gy, cv::Mat &out, bool first, vector<Point> *contour) {
	// for all possible centers
    int step = (int)(settings->Step / 2); int endRows = out.rows; int endCols = out.cols; int fromRows = 0; int fromCols = 0;
	if (first) {
        step = settings->Step;
		endRows = out.rows * 0.9;
		endCols = out.cols * 0.9;
		fromRows = out.rows * 0.1;
		fromCols = out.cols * 0.1;
	}

	for (int cy = fromRows; cy < endRows; cy += step) {
		double *Or = out.ptr<double>(cy);
		const unsigned char *Wr = weight.ptr<unsigned char>(cy);
		for (int cx = fromCols; cx < endCols; cx += step) {
			if (x == cx && y == cy) {
				continue;
			}

//            уберем, т.к. очень замедляет работу.
//            if (pointPolygonTest(*contour, Point2f(cx,cy), false) < 0)
//                continue;

            // create a Vector from the possible center to the gradient origin
            double dx = x - cx;
            double dy = y - cy;
			// normalize d
			double magnitude = sqrt((dx * dx) + (dy * dy));
            dx = dx / magnitude;
            dy = dy / magnitude;
            //скалярное прозведение векторов.
            //double dotProduct = fabs(dx*gx) + fabs(dy*gy);
            double dotProduct = dx*gx + dy*gy;
            dotProduct = std::max(0.0, dotProduct);
			// square and multiply by the weight
			if (kEnableWeight) {
				Or[cx] += dotProduct * dotProduct * (Wr[cx] / kWeightDivisor);
			}
			else {
				Or[cx] += dotProduct * dotProduct;
			}
		}
	}
}

Point PupilDetector::findEyeCenterIPFMethod(Mat face, Rect eye, bool leftEye)
{
	Mat eyeROIUnscaled; Mat eyeROI;
	eyeROIUnscaled = face(eye);
    vector<int> IPFforY, IPFforX, gradientVectorY, gradientVectorX; int Sum;
    resize(eyeROIUnscaled, eyeROI, Size(255, (((float)255) / eyeROIUnscaled.cols) * eyeROIUnscaled.rows));


	//calc IPF Y
	for (int y = 0; y < eyeROI.rows; ++y) {
		Sum = 0;
		for (int x = 0; x < eyeROI.cols; ++x) {
			Sum += eyeROI.at<unsigned char>(y, x);
		}
		IPFforY.push_back(Sum / eyeROI.cols);
	}

	//calc IPF X
	for (int x = 0; x < eyeROI.cols; ++x) {
		Sum = 0;
		for (int y = 0; y < eyeROI.rows; ++y) {
			Sum += eyeROI.at<unsigned char>(y, x);
		}
		IPFforX.push_back(Sum / eyeROI.rows);
	}

	//calc gradient Y
	gradientVectorY.push_back(2*IPFforY[1] - 2*IPFforY[0]);
	for (int x = 1; x < IPFforY.size() - 1; ++x) {
		//if (2*IPFforY[x + 1] - 2*IPFforY[x - 1] > 0)
			gradientVectorY.push_back(IPFforY.size() / 2.0 + pow((2*IPFforY[x + 1] - 2*IPFforY[x - 1]) / 2.0, 2));
		//else gradientVectorY.push_back(IPFforY.size() / 2.0 + pow((IPFforY[x + 1] - IPFforY[x - 1]) / 2.0, 2));
	}
	gradientVectorY.push_back(2 * IPFforY[IPFforY.size() - 1] - 2 * IPFforY[IPFforY.size() - 2]);

	//calc gradient X
	gradientVectorX.push_back(IPFforX[1] - IPFforX[0]);
	for (int x = 1; x < IPFforX.size() - 1; ++x) {
		//if (IPFforX[x + 1] - IPFforX[x - 1] > 0)
			gradientVectorX.push_back(IPFforX.size() / 2.0 + pow((2*IPFforX[x + 1] - 2*IPFforX[x - 1]) / 2.0, 2));
		//else gradientVectorX.push_back(IPFforX.size() / 2.0 - pow((IPFforX[x + 1] - IPFforX[x - 1]) / 2.0, 2));
	}
	gradientVectorX.push_back(2 * IPFforX[IPFforX.size() - 1] - 2 * IPFforX[IPFforX.size() - 2]);

	//visualization
	Mat image1 = eyeROI.clone();
	Mat image2 = eyeROI.clone();
	for (int x = 0; x < gradientVectorY.size() - 1; ++x) {
		line(image1, Point(IPFforY[x], x), Point(IPFforY[x + 1], x + 1), Scalar(0, 0, 0), 2);
		line(image1, Point(gradientVectorY[x], x), Point(gradientVectorY[x + 1], x + 1), Scalar(255, 255, 255), 1);
	}
	for (int x = 0; x < gradientVectorX.size() - 1; ++x) {
		line(image2, Point(x, IPFforX[x]), Point(x + 1, IPFforX[x + 1]), Scalar(0, 0, 0), 2);
		line(image2, Point(x, gradientVectorX[x]), Point(x + 1, gradientVectorX[x + 1]), Scalar(255, 255, 255), 1);
	}
	if (leftEye) {
		imshow("LeftY", image1);
		imshow("LeftX", image2);
	}
	else  {
		imshow("RightY", image1);
		imshow("RightX", image2);
	}
    return Point();
}

Point PupilDetector::findEyeCenter(Mat face, Rect eye, bool first, vector<Point>& bestPoints, vector<Point> *contour, double &value)
{
    Mat eyeROIUnscaled, eyeROI;
    eyeROIUnscaled = face(eye);

    removingGlare(eyeROIUnscaled);

    scaleToFastSize(eyeROIUnscaled, eyeROI, contour);

	//-- Find the gradient
    Mat gradientX = computeMatXGradient(eyeROI);
    Mat gradientY = computeMatXGradient(eyeROI.t()).t();

	//-- Normalize and threshold the gradient
    // compute all the magnitudes
    Mat mags = matrixMagnitude(gradientX, gradientY);

	//compute the threshold
    double gradientThresh = computeDynamicThreshold(mags, settings->GradientThreshold);

	//normalize
	for (int y = 0; y < eyeROI.rows; ++y) {
		double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
		const double *Mr = mags.ptr<double>(y);
		for (int x = 0; x < eyeROI.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			double magnitude = Mr[x];
			if (magnitude > gradientThresh) {
                Xr[x] = gX / magnitude;
                Yr[x] = gY / magnitude;
            }
			else {
				Xr[x] = 0.0;
				Yr[x] = 0.0;
			}
		}
    }

	//-- Create a blurred and inverted image for weighting
	Mat weight;
	GaussianBlur(eyeROI, weight, Size(kWeightBlurSize, kWeightBlurSize), 0, 0);
	for (int y = 0; y < weight.rows; ++y) {
		unsigned char *row = weight.ptr<unsigned char>(y);
		for (int x = 0; x < weight.cols; ++x) {
			row[x] = (255 - row[x]);
		}
    }

	//-- Run the algorithm!
    Mat outSum = Mat::zeros(eyeROI.rows, eyeROI.cols, CV_64F);

    //изменил голосование за центр.
    //первый рабочий, можно вернуть
    //второй (новый) использует знание о контуре глаза - считаются голоса только внутри него
    //вложенность циклов изменена для  ускорения
    if (false)
    {
        // for each possible gradient location
        // Note: these loops are reversed from the way the paper does them
        // it evaluates every possible center for each gradient location instead of
        // every possible gradient location for every center.
        //printf("Eye Size: %ix%i\n", outSum.cols, outSum.rows);
        int step = (int)(settings->Step / 2);
        int endRows = weight.rows;
        int endCols = weight.cols;
        int fromRows = 0;
        int fromCols = 0;

        if (first){
            step = settings->Step;
            endRows = weight.rows * 0.9;
            endCols = weight.cols * 0.9;
            fromRows = weight.rows * 0.1;
            fromCols = weight.cols * 0.1;
        }

        for (int y = fromRows; y < endRows; y += step)
        {
            const unsigned char *Wr = weight.ptr<unsigned char>(y);
            const double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
            for (int x = fromCols; x < endCols; x += step) {
                double gX = Xr[x], gY = Yr[x];
                if (gX == 0.0 && gY == 0.0) {
                    continue;
                }
                testPossibleCentersFormula(x, y, weight, gX, gY, outSum, first, contour);
            }
        }
    }
    else
    {
        //два цикла по точкам, которые будем считать (возможные центры)
        for (int cy = 0; cy < outSum.rows; cy ++)
        {
            double *Or = outSum.ptr<double>(cy);
            const unsigned char *Wr_outSum = weight.ptr<unsigned char>(cy);
            for (int cx = 0; cx < outSum.cols; cx ++)
            {
                //если точка не входит в полигон (область глаза) - гнать её.
                if (pointPolygonTest(*contour, Point2f(cx,cy), false) < 0)
                    continue;

                //два цикла по градиентам
                for (int y = 0; y < weight.rows; y ++)
                {
                    const double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
                    for (int x = 0; x < weight.cols; x ++)
                    {
                        double gX = Xr[x], gY = Yr[x];
                        if (gX == 0.0 && gY == 0.0) {
                            continue;
                        }

                        if (x == cx && y == cy) {
                            continue;
                        }

                        // create a Vector from the possible center to the gradient origin
                        double dx = x - cx;
                        double dy = y - cy;
                        // normalize d
                        double magnitude = sqrt((dx * dx) + (dy * dy));
                        dx = dx / magnitude;
                        dy = dy / magnitude;
                        //скалярное прозведение векторов.
                        //double dotProduct = fabs(dx*gX) + fabs(dy*gY);
                        double dotProduct = dx*gX + dy*gY;
                        dotProduct = std::max(0.0, dotProduct);
                        // square and multiply by the weight
                        if (kEnableWeight) {
                            Or[cx] += dotProduct * dotProduct * (Wr_outSum[cx] / kWeightDivisor);
                        }
                        else {
                            Or[cx] += dotProduct * dotProduct;
                        }
                    }
                }
            }
        }
    }

	// scale all the values down, basically averaging them
	double numGradients = (weight.rows*weight.cols);
	Mat out;
    outSum.convertTo(out, CV_32F, 1.0 / numGradients);

	//-- Find the maximum point
	Point maxP;
	double maxVal;
	minMaxLoc(out, NULL, &maxVal, NULL, &maxP);

    //TEST берем несколько лучших кандидатов и рисуем их на изображении
    Point maxP2 = maxP;
    bestPoints.push_back(unscalePoint(maxP, eye));
    for(int i = 0; i < out.cols; i ++)
    {
        float* row_ptr = out.ptr<float>(maxP2.y);
        row_ptr[maxP2.x] = 0;
        minMaxLoc(out, NULL, &maxVal, NULL, &maxP2);
        bestPoints.push_back(unscalePoint(maxP2, eye));
    }
    value = maxVal;
	return unscalePoint(maxP, eye);
}


void PupilDetector::scaleToFastSize(const Mat& src, Mat& dst, vector<Point> *contour)
{
    if (kScaleToFastSize)
    {
        if (src.cols > settings->kFastEyeWidth)
        {
            int newrows = (int)((((float)(settings->kFastEyeWidth)) / src.cols) * src.rows);
            for( int i = 0; i< contour->size(); i++ )
            {
                contour->at(i).x = (int)((float)contour->at(i).x * settings->kFastEyeWidth / src.cols);
                contour->at(i).y = (int)((float)contour->at(i).y * newrows / src.rows);
            }
            try {
                resize(src, dst, Size(settings->kFastEyeWidth, newrows));
            }
            catch (std::exception & e) {
                logWrite(e.what());
            }
        }
        else
            dst = src.clone();
    }
    else
    {
        dst = src.clone();
    }
}

Mat PupilDetector::computeMatXGradient(const Mat& mat)
{
	Mat out(mat.rows, mat.cols, CV_64F);

	for (int y = 0; y < mat.rows; ++y) {
		const uchar *Mr = mat.ptr<uchar>(y);
		double *Or = out.ptr<double>(y);

		Or[0] = Mr[1] - Mr[0];
		for (int x = 1; x < mat.cols - 1; ++x) {
			Or[x] = (Mr[x + 1] - Mr[x - 1]) / 2.0;
		}
		Or[mat.cols - 1] = Mr[mat.cols - 1] - Mr[mat.cols - 2];
	}
	return out;
}


Mat PupilDetector::matrixMagnitude(const Mat & matX, const Mat & matY)
{
	Mat mags(matX.rows, matX.cols, CV_64F);
	for (int y = 0; y < matX.rows; ++y) {
		const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);
		double *Mr = mags.ptr<double>(y);
		for (int x = 0; x < matX.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			double magnitude = sqrt((gX * gX) + (gY * gY));
			Mr[x] = magnitude;
		}
	}
	return mags;
}


double PupilDetector::computeDynamicThreshold(const Mat & mat, double stdDevFactor)
{
	Scalar stdMagnGrad, meanMagnGrad;
	meanStdDev(mat, meanMagnGrad, stdMagnGrad);
	double stdDev = stdMagnGrad[0] / sqrt(mat.rows*mat.cols);
	return stdDevFactor * stdDev + meanMagnGrad[0];
}


Mat PupilDetector::floodKillEdges(Mat& mat)
{
	rectangle(mat, Rect(0, 0, mat.cols, mat.rows), 255);

	Mat mask(mat.rows, mat.cols, CV_8U, 255);
	std::queue<Point> toDo;
	toDo.push(Point(0, 0));
	while (!toDo.empty()) {
		Point p = toDo.front();
		toDo.pop();
		if (mat.at<float>(p) == 0.0f) {
			continue;
		}
		// add in every direction
		Point np(p.x + 1, p.y); // right
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x - 1; np.y = p.y; // left
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x; np.y = p.y + 1; // down
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x; np.y = p.y - 1; // up
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		// kill it
		mat.at<float>(p) = 0.0f;
		mask.at<uchar>(p) = 0;
	}
	return mask;
}


bool PupilDetector::floodShouldPushPoint(const Point& np, const Mat & mat)
{
	return inMat(np, mat.rows, mat.cols);
}


Point PupilDetector::unscalePoint(Point p, Rect origSize)
{
    if (kScaleToFastSize)
    {
        if (settings->kFastEyeWidth < origSize.width)
        {
            float ratio = (((float)settings->kFastEyeWidth) / origSize.width);
            int x = round(p.x / ratio);
            int y = round(p.y / ratio);
            return Point(x, y);
        }
        else
            return p;
    }
    else
    {
        return Point(p.x, p.y);
    }
}


bool PupilDetector::inMat(Point p, int rows, int cols)
{
	return p.x >= 0 && p.x < cols && p.y >= 0 && p.y < rows;
}

void PupilDetector::correctPoint_CDFAnalysis(cv::Mat roi, cv::Point& pt_pupil)
{
    std::vector<double> cdf(256);
    //Preprocessing
    GaussianBlur(roi, roi, cv::Size(3,3), 0, 0);
    cv::equalizeHist(roi, roi);

    cv::Mat mask;
    roi.copyTo(mask);
    double nf, temp, pos_pmi_i, pos_pmi_j;

    int erosion_size = 1;
    cv::Mat element_erode = cv::getStructuringElement( cv::MORPH_ELLIPSE,
        cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
        cv::Point( erosion_size, erosion_size ) );

    for(int i = 0; i<256; i++) {
        cdf[i] = 0;
    }

    for(int i = 0; i < roi.rows; i++) {
        for(int j = 0; j < roi.cols; j++) {
            ++cdf[roi.at<uchar>(i,j)];
        }
    }

    for(int i=0; i < 256; i++) {
        double value;
        if(i != 0) {
            value = cdf[i];
            cdf[i] += cdf[i-1];
        }
        //std::cout<<"CDF-"<<i<<" = "<<cdf[i]<<std::endl;
    }

    nf = cdf[0];
    for(int i=1; i<256;i++) {
        if(cdf[i] > nf) {
            nf = cdf[i];
        }
    }

    temp = roi.at<uchar>(0,0);
    pos_pmi_i = 0;
    pos_pmi_j = 0;

    cv::erode( mask, mask, element_erode );

    for(int i = 0; i < roi.rows; i++) {
        for(int j = 0; j < roi.cols; j++) {
            if(cdf[roi.at<uchar>(i,j)] >= 0.05 * nf) {
                mask.at<uchar>(i,j) = 0;
                //roi.at<uchar>(i,j) = 255;
            }
            else {
                if(roi.at<uchar>(i,j) <= temp) {
                    pos_pmi_i = i;
                    pos_pmi_j = j;
                    temp = roi.at<uchar>(i,j);
                }
                mask.at<uchar>(i,j) = 255;
                //roi.at<uchar>(i,j) = 255;
            }
        }
    }

    double avg_PI = 0;
    int window_size;
    window_size = roi.cols*roi.rows/175;
    std::cout<<"window_size : "<<window_size<<std::endl;

    for(int i = pos_pmi_i - window_size; i < pos_pmi_i + window_size; i++) {
        for(int j = pos_pmi_j - window_size; j < pos_pmi_j + window_size; j++) {
            if(mask.at<uchar>(i,j)) {
                avg_PI += roi.at<uchar>(i,j);
            }
        }
    }

    for(int i = pos_pmi_i - window_size; i < pos_pmi_i + window_size; i++) {
        for(int j = pos_pmi_j - window_size; j < pos_pmi_j + window_size; j++) {
            if(roi.at<uchar>(i,j) > ((int)avg_PI)) {
                mask.at<uchar>(i,j) = 0;
            }
        }
    }

    cv::erode( mask, mask, element_erode );

    cv::Moments m = cv::moments(mask, 1);
    int pos_i = (int)(m.m10/m.m00), pos_j = (int)(m.m01/m.m00);

    std::cout<<"PMI : "<<pos_pmi_i<<", "<<pos_pmi_j<<std::endl;
    std::cout<<"Point : "<<pos_i<<", "<<pos_j<<std::endl;

    pt_pupil.x = pos_i;
    pt_pupil.y = pos_j;

    cv::circle(roi, cv::Point(pos_i, pos_j), 3, cv::Scalar(255,0,0), -1, 4, 0);
}


