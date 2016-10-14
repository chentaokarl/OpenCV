/*
 * DisplayImage.cpp
 *
 *  Created on: Sep 13, 2016
 *      Author: carlchan
 */

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

#include <stdio.h>

using namespace cv;
using namespace std;

Mat globalMatSrc, globalMatDis;
vector <Mat> globalMatSrcVector;


void videoInformation(string videoPath)
{
	VideoCapture cap(videoPath);
	if(!cap.isOpened())
		cout<< "the video is not found!" << endl;
	cout << "CV_CAP_PROP_FRAME_COUNT : " << cap.get(CV_CAP_PROP_FRAME_COUNT) << endl;
	cout << "CV_CAP_PROP_FRAME_WIDTH : " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
	cout << "CV_CAP_PROP_FRAME_HEIGHT : " << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
	cout << "CV_CAP_PROP_FPS : " << cap.get(CV_CAP_PROP_FPS) << endl;

}

void videoFrameReader(string videoPath)
{
	VideoCapture cap(videoPath);
	if(!cap.isOpened())
		cout<< "the video is not found!" << endl;
	int frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
	for(int i = 0; i < frames; i++)
	{
		Mat frame;
		cap >> frame;
		imshow("video ", frame);
		//cvtColor(frame, frame, COLOR_BGR2GRAY);
		cout<<i<<endl;
		cvWaitKey(100);
	}
}

void saveImageFromVideo(string videoPath)
{
	std::ostringstream pathImageWrite(std::ostringstream::ate);
	VideoCapture cap(videoPath);
	int frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
		for(int i = 0; i < frames; i++)
		{
			Mat frame;
			cap >> frame;
			pathImageWrite.str("");

			pathImageWrite << "src/test2/frame"<<i<<".png";
//			if(i/10 >2 &&i/10 <10){
				imwrite(pathImageWrite.str(), frame);
//			}
		}
}

int maindd( int argc, char** argv )
{
	string videoPath = "/Users/carlchan/Desktop/Test2.mp4";

//	videoInformation(videoPath);
//	videoFrameReader(videoPath);
	saveImageFromVideo(videoPath);

//  Mat image;
//  image = imread( argv[1], 1 );
//
//  if( argc != 2 || !image.data )
//    {
//      printf( "No image data \n" );
//      return -1;
//    }
//
//  namedWindow( "Display Image", WINDOW_NORMAL );
//  imshow( "Display Image", image );
//
//
//    VideoCapture cap(0);
//
//    while(true){
//        Mat Webcam;
//        cap.read(Webcam);
//        imshow("Webcam", Webcam);
//    }
//
//
//
//  waitKey(0);



  return 0;
}

