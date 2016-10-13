/*
 * DetectPassCode.cpp
 *
 *  Created on: Sep 26, 2016
 *      Author: carlchan
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace cv;
using namespace std;
#define PI 3.14159265
Mat img_mask;

int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

Mat result;
char* source_window = "Source Image";
//char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;

/// Function header
void drawDetect(Mat, Rect, int, void*);
void imageSub(string, string);
Point MatchingMethod(Mat, Mat, int, void*);
vector<Point> phoneKeyPoints(Point, Point);
int guessCurrNum(vector<Point>, vector<Point>);
Point getCenterPoint(vector<Point>, vector<Point>, int);
bool hasPointsBeyond(vector<Point>, vector<Point>, int);
int compareDist(Point, Point, Point);

string detectTempl(string, string);

/** @function main */
int main(int argc, char** argv) {
	string pathPrefix = "src/frame";
//	int previous = -1;
	for (int i = 35; i < 66; i += 10) {
		std::ostringstream imagePath(std::ostringstream::ate);
		imagePath << pathPrefix << i << ".png";
		string tempPath = detectTempl(imagePath.str(), "src/template.jpg");
//		string tempPath = detectTempl("src/frame33.png", "src/template.jpg");
		std::remove(tempPath.c_str()); // delete temp file

		waitKey(0);
	}
//	string pathPrefix = "src/test2/frame";
//	for (int i = 25; i < 66; i += 60) {
//		std::ostringstream imagePath(std::ostringstream::ate);
//		imagePath << pathPrefix << i << ".png";
//		string tempPath = detectTempl(imagePath.str(),
//				"src/test2/template.jpg");
//		//		string tempPath = detectTempl("src/frame33.png", "src/template.jpg");
//		std::remove(tempPath.c_str()); // delete temp file
//
//		waitKey(0);
//	}

	return (0);
}

string detectTempl(string imgPath, string templPath) {
	// Load source image and convert it to gray
	Mat sourceImg = imread(imgPath, 1);

	Mat tmpImg = imread(templPath, 1);

	Point img1_point = MatchingMethod(sourceImg, tmpImg, 0, 0);
	Rect rect = Rect(img1_point.x, img1_point.y, tmpImg.cols, tmpImg.rows);
	Mat roiImg1;
	roiImg1 = sourceImg(rect);
	std::ostringstream pathImageWrite(std::ostringstream::ate);
	pathImageWrite << imgPath << "_roi.png";

	drawDetect(sourceImg, rect, 0, 0);

	imwrite(pathImageWrite.str(), roiImg1);

	return pathImageWrite.str();
}

/** @function drawDetect */
void drawDetect(Mat sourceImg, Rect rect, int, void*) {
	Mat src_gray;
	/// Convert image to gray and blur it
	cvtColor(sourceImg, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(src_gray, canny_output, thresh, thresh * 2, 3);
	/// Find contours
	findContours(canny_output(rect), contours, hierarchy, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Draw contours
	Mat drawing = Mat::zeros(src_gray.size(), CV_8UC3);

	int maxContLen = 0;
	vector<Point> newContour = contours[0];
	for (int i = 1; i < contours.size(); i++) {

		double tmp1 = arcLength(contours[i], true);
		double tmp2 = arcLength(contours[maxContLen], true);

		if (tmp1 > tmp2) {
			maxContLen = i;
		}
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
				rng.uniform(0, 255));
		drawContours(drawing, contours, maxContLen, color, 1, 8, hierarchy, 0,
				Point(rect.x, rect.y));
////		newContour.insert(newContour.end(),contours[i].begin(), contours[i].end());
		for (int j = 0; j < contours[i].size(); j++) {
			newContour.push_back(contours[i][j]);
		}
	}

	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
			rng.uniform(0, 255));

	drawContours(drawing, contours, maxContLen, color, 1, 8, hierarchy, 0,
			Point(rect.x, rect.y));
	int maxX = 0;
	for (int j = 0; j < contours[maxContLen].size(); j++) {

		if (j == 0) {
			cv::circle(drawing, Point(rect.x, rect.y) + contours[maxContLen][j],
					2, color);
		}
		if (contours[maxContLen][j].x > contours[maxContLen][maxX].x)
			maxX = j;

	}
	cv::circle(drawing, Point(rect.x, rect.y) + contours[maxContLen][maxX], 2,
			color);

	vector<Point> phonePoints = phoneKeyPoints(contours[maxContLen][0],
			contours[maxContLen][maxX]);
	color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
			rng.uniform(0, 255));

	int currGuess = guessCurrNum(newContour, phonePoints);
	cout << "Guess:" << currGuess << endl;
//	Moments M = moments(contours[maxContLen]);
//		int cX = int(M.m10 / M.m00);
//		int cY = int(M.m01 / M.m00);
//	cv::circle(drawing, Point(rect.x, rect.y) + Point(cX, cY), 3, color);
//	cv::circle(drawing, Point(rect.x, rect.y) + Point(18,16), 3,
//						color);
	cv::circle(drawing, Point(rect.x, rect.y) + Point(155, 109), 3, color);
	for (int i = 0; i < phonePoints.size(); i++) {
		cv::circle(drawing, Point(rect.x, rect.y) + phonePoints[i], 1, color);
	}

	// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
}

void imageSub(string img1Path, string img2Path) {
	Mat im = imread(img1Path, 1);
	Mat im2 = imread(img2Path, 1);

	Mat dest;

	cv::subtract(im2, im, dest);
	CV_Assert(dest.depth() == CV_8U);

	int channels = dest.channels();

	int nRows = dest.rows;
	int nCols = dest.cols * channels;
	if (dest.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}
	imshow("image diff", dest);
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
Point MatchingMethod(Mat sourceImg, Mat templImg, int, void*) {

	/// Source image to display
	Mat img_display;
	sourceImg.copyTo(img_display);

	/// Create the result matrix
	int result_cols = sourceImg.cols - templImg.cols + 1;
	int result_rows = sourceImg.rows - templImg.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	/// Do the Matching and Normalize
	matchTemplate(sourceImg, templImg, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	double minVal;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED) {
		matchLoc = minLoc;
	} else {
		matchLoc = maxLoc;
	}

	/// Show me what you got
	rectangle(img_display, matchLoc,
			Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows),
			Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc,
			Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows),
			Scalar::all(0), 2, 8, 0);

	imshow(source_window, img_display);

	return matchLoc;
}

vector<Point> phoneKeyPoints(Point upStart, Point upEnd) {
	vector<Point> phonePointsResult;
	float tanAlpha = (upEnd.y - upStart.y) * 1.0 / (upEnd.x - upStart.x);
	float alpha = atan(tanAlpha) * 180.0 / PI;
	alpha += 10;
//	cout << "alpha: " << alpha << endl;
//	cout << "A:" << upStart.x << "," << upStart.y << " B:" << upEnd.x << ","
//			<< upEnd.y << endl;
	float phoneWidth = sqrt(
			pow((upEnd.x - upStart.x), 2) + pow((upEnd.y - upStart.y), 2));
	float phoneLongth = phoneWidth * 138.1 / 67;
	cout << phoneWidth << "," << phoneLongth << endl;

	//calc points for key number 1 , 2, 3
	float tempDist = phoneLongth * 3 / 8;
	Point point38A(upStart.x - tempDist * sin(alpha * PI / 180.0),
			upStart.y + tempDist * cos(alpha * PI / 180.0) - 10);
	Point point38B(upEnd.x - tempDist * sin(alpha * PI / 180.0),
			upEnd.y + tempDist * cos(alpha * PI / 180.0) - 10);
	float tempXsub = point38B.x - point38A.x;
	float tempYsub = point38B.y - point38A.y;
	Point point1(point38A.x + (tempXsub * 2 / 9),
			point38A.y + (tempYsub * 2 / 9));
	Point point2(point38A.x + (tempXsub / 2), point38A.y + (tempYsub / 2));
	Point point3(point38A.x + (tempXsub * 7 / 9),
			point38A.y + (tempYsub * 7 / 9));

	//calc points for key number 4, 5, 6
	tempDist = phoneLongth / 2;
	Point point48A(upStart.x - tempDist * sin(alpha * PI / 180.0),
			upStart.y + tempDist * cos(alpha * PI / 180.0) - 10);
	Point point48B(upEnd.x - tempDist * sin(alpha * PI / 180.0),
			upEnd.y + tempDist * cos(alpha * PI / 180.0) - 10);
	tempXsub = point48B.x - point48A.x;
	tempYsub = point48B.y - point48A.y;
	Point point4(point48A.x + (tempXsub * 2 / 9),
			point48A.y + (tempYsub * 2 / 9));
	Point point5(point48A.x + (tempXsub / 2), point48A.y + (tempYsub / 2));
	Point point6(point48A.x + (tempXsub * 7 / 9),
			point48A.y + (tempYsub * 7 / 9));

	//calc points for key number 7, 8, 9
	tempDist = phoneLongth * 5 / 8;
	Point point58A(upStart.x - tempDist * sin(alpha * PI / 180.0),
			upStart.y + tempDist * cos(alpha * PI / 180.0) - 10);
	Point point58B(upEnd.x - tempDist * sin(alpha * PI / 180.0),
			upEnd.y + tempDist * cos(alpha * PI / 180.0) - 10);
	tempXsub = point58B.x - point58A.x;
	tempYsub = point58B.y - point58A.y;
	Point point7(point58A.x + (tempXsub * 2 / 9),
			point58A.y + (tempYsub * 2 / 9));
	Point point8(point58A.x + (tempXsub / 2), point58A.y + (tempYsub / 2));
	Point point9(point58A.x + (tempXsub * 7 / 9),
			point58A.y + (tempYsub * 7 / 9));

	//calc points for key number 0
	tempDist = phoneLongth * 6 / 8;
	Point point68A(upStart.x - tempDist * sin(alpha * PI / 180.0),
			upStart.y + tempDist * cos(alpha * PI / 180.0) - 10);
	Point point68B(upEnd.x - tempDist * sin(alpha * PI / 180.0),
			upEnd.y + tempDist * cos(alpha * PI / 180.0) - 10);
	tempXsub = point68B.x - point68A.x;
	tempYsub = point68B.y - point68A.y;
	Point point0(point68A.x + (tempXsub / 2), point68A.y + (tempYsub / 2));

	phonePointsResult.push_back(point38A);
	phonePointsResult.push_back(point38B);
	phonePointsResult.push_back(point1);
	phonePointsResult.push_back(point2);
	phonePointsResult.push_back(point3);
	phonePointsResult.push_back(point48A);	//5
	phonePointsResult.push_back(point48B);
	phonePointsResult.push_back(point4);
	phonePointsResult.push_back(point5);
	phonePointsResult.push_back(point6);
	phonePointsResult.push_back(point58A);	//10
	phonePointsResult.push_back(point58B);
	phonePointsResult.push_back(point7);
	phonePointsResult.push_back(point8);
	phonePointsResult.push_back(point9);
	phonePointsResult.push_back(point68A);	//15
	phonePointsResult.push_back(point68B);
	phonePointsResult.push_back(point0);

	return phonePointsResult;
}

int guessCurrNum(vector<Point> contourMax, vector<Point> phonePoints) {

	vector<Point> tempCont;
	// if it is 1, 2, or 3
	for (int i = contourMax.size() - 1; i >= 0; i--) {
		if (contourMax[i].y > phonePoints[2].y
				&& contourMax[i].y < phonePoints[1].y
				&& contourMax[i].x > phonePoints[2].x
				&& contourMax[i].x < phonePoints[1].x) {
//		cout << " point out: "<<contourMax[i].x<< ", "<<contourMax[i].y<<",dist:"<<(contourMax[i].y -phonePoints[0].y )<<",,"<<((contourMax[i].x - phonePoints[0].x)*(phonePoints[1].y-phonePoints[0].y)/(phonePoints[1].x-phonePoints[0].x))<<endl;
			if (((contourMax[i].y - phonePoints[0].y)
					- ((contourMax[i].x - phonePoints[0].x)
							* (phonePoints[1].y - phonePoints[0].y)
							/ (phonePoints[1].x - phonePoints[0].x))) < 2) {
				cout << " -----point123: " << contourMax[i].x << ", "
						<< contourMax[i].y << endl;
				int min = 1;
				if (compareDist(phonePoints[3], phonePoints[2], contourMax[i])
						== 0) {
					min = 2;
				}

				if (compareDist(phonePoints[4], phonePoints[min + 1],
						contourMax[i]) == 0) {
					min = 3;
				}
				return min;
			}
		}
	}

	//if it is 4, 5, 6

	for (int i = contourMax.size() - 1; i >= 0; i--) {

		if (contourMax[i].y > phonePoints[5].y
				&& contourMax[i].y < phonePoints[6].y
				&& contourMax[i].x > phonePoints[5].x
				&& contourMax[i].x < phonePoints[6].x) {
			if ((contourMax[i].y - phonePoints[5].y)
					== (contourMax[i].x - phonePoints[5].x)
							* (phonePoints[6].y - phonePoints[5].y)
							/ (phonePoints[6].x - phonePoints[5].x)) {
				cout << " -----point456: " << contourMax[i].x << ", "
						<< contourMax[i].y << endl;
				int min = 4;
				if (compareDist(phonePoints[8], phonePoints[7], contourMax[i])
						== 0) {
					min = 5;
				}

				if (compareDist(phonePoints[9], phonePoints[min + 3],
						contourMax[i]) == 0) {
					min = 6;
				}

				Point center = getCenterPoint(contourMax, phonePoints, min);
				cout << center.x << ",,,,,," << center.y << endl;

				if (compareDist(phonePoints[13], phonePoints[14], contourMax[i])
						== 0) {
					if (compareDist(phonePoints[13], phonePoints[min + 3],
							center) == 0) {
						min = 8;
					}
				} else {
					if (compareDist(phonePoints[14], phonePoints[min + 3],
							center) == 0) {
						min = 9;
					}
				}
				return min;
			}
		}
	}

	//if it is 7, 8, 9

	for (int i = contourMax.size() - 1; i >= 0; i--) {
		if (contourMax[i].y >= phonePoints[10].y
				&& contourMax[i].y <= phonePoints[11].y
				&& contourMax[i].x > phonePoints[10].x
				&& contourMax[i].x < phonePoints[11].x) {
			if (((contourMax[i].y - phonePoints[10].y)
					- (contourMax[i].x - phonePoints[10].x)
							* (phonePoints[11].y - phonePoints[10].y)
							/ (phonePoints[11].x - phonePoints[10].x)) < 2) {
				int min = 7;
				if (compareDist(phonePoints[13], phonePoints[12], contourMax[i])
						== 0) {
					min = 8;
				}

				if (compareDist(phonePoints[14], phonePoints[min + 5],
						contourMax[i]) == 0) {
					min = 9;
				}

				return min;
			}
		}
	}

	//if it is 0
	for (int i = 1; i < contourMax.size(); i++) {
		if ((contourMax[i].y - phonePoints[15].y)
				== (contourMax[i].x - phonePoints[15].x)
						* (phonePoints[16].y - phonePoints[15].y)
						/ (phonePoints[16].x - phonePoints[15].x)) {
			return 0;
		}
	}

	return 5;
}

Point getCenterPoint(vector<Point> contourMax, vector<Point> phonePoints,
		int min) {

	vector<Point> tempCont;
	for (int i = contourMax.size() - 1; i >= 0; i--) {
		if (min == 6) {
			if ((contourMax[i].x < phonePoints[min + 3].x)
					&& (contourMax[i].x > phonePoints[14].x)) {
				tempCont.push_back(contourMax[i]);
			}
		}
		if (min == 5) {
			if (((contourMax[i].x > phonePoints[10].x)
					&& (contourMax[i].x < phonePoints[14].x))
					&& ((contourMax[i].y > phonePoints[min + 3].y)
							&& (contourMax[i].y < phonePoints[14].y))) {
				tempCont.push_back(contourMax[i]);
			}
		}

		if (min == 1 || min == 2 || min == 3) {
			if (((contourMax[i].x > phonePoints[min + 1].x)
					&& (contourMax[i].x < phonePoints[1].x))
					&& ((contourMax[i].y > phonePoints[min + 1].y)
							&& (contourMax[i].y < phonePoints[1].y))) {
				tempCont.push_back(contourMax[i]);
			}
		}
	}
	Moments M = moments(tempCont);
	int cX = int(M.m10 / M.m00);
	int cY = int(M.m01 / M.m00);
	cout << cX << ",,,,,," << cY << endl;

	return Point(cX, cY);
}

bool hasPointsBeyond(vector<Point> contourMax, vector<Point> phonePoints,
		int min, Point touchPoint) {
	if (min == 1 || min == 2 || min == 3) {
		for (int i = contourMax.size() - 1; i >= 0; i--) {
			//is point between
			phonePoints[min + 1].x > touchPoint.x;
		}
	} else if (min == 4 || min == 5 || min == 6) {

	} else if (min == 7 || min == 8 || min == 9) {

	}
}

//compare dist between p1--p3 and p2---p3
// if dist(p1--p3) < dist(p2--p3) return 0;
//else return -1;
int compareDist(Point p1, Point p2, Point p3) {
	if (sqrt(pow((p1.x - p3.x), 2) + pow((p1.y - p3.y), 2))
			< sqrt(pow((p2.x - p3.x), 2) + pow((p2.y - p3.y), 2))) {
		return 0;
	} else {
		return -1;
	}
}
