#include <math.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/video/background_segm.hpp"

#include "Pipe.h"
#include "Data.h"

using namespace std;
using namespace cv;

struct MessagePacket C;

VideoCapture Source;
double CameraViewWidth,CameraViewHeight;    //the lenght and width of the frame
float CenteroidX1=0,CenteroidY1=0;
float CenteroidX2=0,CenteroidY2=0;
int CameraX,CameraY;     					//for camera and intrusion distance esitimation

bool ShowTrackbar = false;					//a boolean switch
int CameraNumber = 1;						//Specify the camera to be used
float FrameSkipTime = 1000;					//time in seconds - dealy between consequeive frames

int ThresholdValue = 30;					//our sensitivity value to be used for the thresolding of the motion mask
int BlurValue = 20;                         //blur value for reducing inconsistency
int MorphSize = 10;							//Used for Image closing and removing incosistency
int MinAreaFilter = 500; 					//area of the minimun accepted blob
int MaxAreaFilter = 75000;					//area of the maximum accepted blob
int CameraHeight = 10;						//height at which camera is placed at

double RotationConstantX = 5;				//linear rotation constant
double RotationConstantY = 1;				//linear rotation constant

void pipeInitialize()
{
	N = 1;
	strcpy(PipeNames[0],"CAMPIPE");
	strcpy(PipePermission[0],"W");

	openPipes();
}

void sendCameraPostion(int pan,int tilt)
{
	char temp[50];
	
	setMessagePacket(&C,"","","","");
	sprintf(temp,"%d",tilt);
	setMessagePacket(&C,"CameraSensor",temp,"Camera-1","Tilt");
	sendMessagePacket("CAMPIPE",&C);

	setMessagePacket(&C,"","","","");
	sprintf(temp,"%d",pan);
	setMessagePacket(&C,"CameraSensor",temp,"Camera-1","Pan");
	sendMessagePacket("CAMPIPE",&C);
}

int openCamera(int Camera)
{
	//setting camera properties
	//Source.set(CV_CAP_PROP_FRAME_WIDTH,1280);
	//Source.set(CV_CAP_PROP_FRAME_HEIGHT,720);	

	//Opeing a capturing device or video .to open video input the filename to open() function
	Source.open(Camera);
	if(!Source.isOpened())
	{
		cout<<"ERROR ACQUIRING VIDEO FEED\n";
		return -1;
	}
}

void readFrames(Mat &f1,Mat &f2,Mat &f3,Mat &f4)
{

	Mat temp1,temp2,temp3,temp4;
	
	openCamera(CameraNumber);
	Source.read(temp1);
	Source.release();
	usleep(FrameSkipTime);
	f1 = temp1;
	
	openCamera(CameraNumber);
	Source.read(temp2);
	Source.release();
	usleep(FrameSkipTime);
	f2 = temp2;
	
	openCamera(CameraNumber);
	Source.read(temp3);
	Source.release();
	usleep(FrameSkipTime);
	f3 = temp3;
	
	openCamera(CameraNumber);
	Source.read(temp4);
	Source.release();
	usleep(FrameSkipTime);
	f4 = temp4;
		
}

Mat detectMotion(Mat &f1,Mat &f2,Mat &f3)
{
	Mat prev_frame,curr_frame,next_frame;
	prev_frame = f1;
	curr_frame = f2;
	next_frame = f3;

	//convert all the frames to gray 	
	Mat gray1,gray2,gray3;	
	cvtColor(prev_frame,gray1,COLOR_BGR2GRAY);
	cvtColor(curr_frame,gray2,COLOR_BGR2GRAY);
	cvtColor(next_frame,gray3,COLOR_BGR2GRAY);
	
	//difference between frames 
	Mat diff1,diff2;
	absdiff(gray1,gray3,diff1);
	absdiff(gray2,gray3,diff2);
	
	//compute the result frame
	Mat result;
	bitwise_and(diff1, diff2, result);

	//post-process the frames
    threshold(result,result,ThresholdValue,255,THRESH_BINARY);
    blur(result,result,Size(BlurValue,BlurValue));
    threshold(result,result,ThresholdValue,255,THRESH_BINARY);
    Mat element = getStructuringElement( MORPH_RECT, Size( 2*MorphSize + 1, 2*MorphSize+1 ), Point( MorphSize, MorphSize ) );
    morphologyEx( result, result, MORPH_CLOSE, element );
        
    return result;
}

Rect blobAnalysis(Mat &f)
{
	double a,largestArea=0;
	int i,largestContour=0;
	Rect box = Rect(0,0,0,0);
	
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	//find contours of filtered image using openCV findContours function
	findContours(f,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );		// retrieves all contours
	
	if(contours.size() < 1)
		return box;
		
	else
	{	
		//find the largest contour 	
		for(i=0;i<contours.size();i++)
		{
			a = contourArea(contours[i],false);
			if(a > largestArea)
			{	
				largestArea = a;
				largestContour = i;
			}
		}
		
		//area filter
		if(contourArea(contours[largestContour],false) < MinAreaFilter || contourArea(contours[largestContour],false) > MaxAreaFilter )
			return box;
		
		//cout << contourArea(contours[largestContour],false) << endl;
		
		//get the rectangle 
		box = boundingRect( (Mat) contours[largestContour]);
	}
				
	return box;			
}

float histcheck(Mat &f1,Mat &f2)
{
	float x;
	Mat t1,t2;
	MatND tp1,tp2;
	int channels[] = {0,1};
	int bins[] = {50,60};
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	const float* ranges[] = { h_ranges, s_ranges };
		
	cvtColor( f1, t1, COLOR_BGR2HSV );
   	cvtColor( f2, t2, COLOR_BGR2HSV );
   	
    calcHist( &t1, 1, channels, Mat(), tp1, 2, bins, ranges, true, false );
   	calcHist( &t2, 1, channels, Mat(), tp2, 2, bins, ranges, true, false );
   	
   	normalize( tp1, tp1, 0, 1, NORM_MINMAX, -1, Mat() );
   	normalize( tp2, tp2, 0, 1, NORM_MINMAX, -1, Mat() );
   	
   	x = compareHist( tp1, tp2, CV_COMP_CORREL);
   			
	return x;
}

int featurecheck(Mat &f1,Mat &f2,Mat &f3)
{
	vector< KeyPoint > key1,key2;
	vector< DMatch > matches;
	Mat des1,des2;
	Mat a,b;
	int matchnumber;
	
	cvtColor(f1,a,COLOR_BGR2GRAY);
	cvtColor(f2,b,COLOR_BGR2GRAY);
		
	Ptr<FeatureDetector> detector = ORB::create();
	BFMatcher matcher(NORM_L1,true);
	
	detector -> detect(a,key1);
	detector -> detect(b,key2);
	
	detector -> compute(a,key1,des1);
	detector -> compute(b,key2,des2);
	
	if(des1.rows > 0 && des1.cols > 0 && des2.rows > 0 && des2.cols > 0)
		matcher.match(des1,des2,matches);
		
	matchnumber = matches.size();
	
	Mat img_matches;
    drawMatches( a, key1, b, key2, matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    f3 = img_matches;
	 	
	return matchnumber;
}

void computeParameter(Rect box1,Rect box2)
{
	double degreePan,degreeTilt;
	double temp1,temp2;
	double r = 25;
	
	
	if(box1.width > 0 && box1.height > 0 && box2.width > 0 && box2.height > 0)
	{
		//compute the centeroids
		CenteroidX1 = box1.x + (box1.width  / 2);
		CenteroidY1 = CameraViewHeight - (box1.y + (box1.height / 2));
		CenteroidX2 = box2.x + (box2.width  / 2);
		CenteroidY2 = CameraViewHeight - (box2.y + (box2.height / 2));
			
		//compute the pan degree		
		temp1 = abs(CenteroidX1 - CenteroidX2) * 0.0147;
		temp2 = ((2*r*r) - (temp1*temp1)) / (2 * r * r);
		temp2 = acos(temp2) * (180 / M_PI) * RotationConstantX;
		degreePan = ceil(temp2);

		//update pan degree 
		if((CenteroidX1 - CenteroidX2) < 0)
			CameraX = CameraX - degreePan;
		else if((CenteroidX1 - CenteroidX2) > 0)
			CameraX = CameraX + degreePan;
		else
			CameraX = CameraX;
				
		//normalize pan
		if(CameraX >= 360)
			CameraX = CameraX - 360;
		else if(CameraX <= 0)
			CameraX = 360 - abs(CameraX);
			
		//compute the tilt angle
		temp1 = abs(CenteroidY1 - CenteroidY2);
		temp1 = temp1 / 70;
		temp2 = temp1 * 5;
		degreeTilt = ceil(temp2);
		
		//update tilt degree
		if((CenteroidY1 - CenteroidY2) > 0)
			CameraY = CameraY + degreeTilt;
		else if((CenteroidX1 - CenteroidX2) < 0)
			CameraY = CameraY - degreeTilt;
		else
			CameraY = CameraY;
	
		//normalze tilt	
		if(CameraY > 180)
			CameraY = 0;
		else if(CameraY < 0)
			CameraY = 60;
		
		//print values	
		cout << "X1: " << CenteroidX1 << endl;
		cout << "X2: " << CenteroidX2 << endl;
		cout << "Pan angle :" << CameraX << endl;
		cout << "Tilt angle :" << CameraY << endl;
	}
	else
		cout << "Intrusion Not Detected" << endl;
}


int main()
{
	//local Variables
	Mat frame1,frame2,frame3,frame4,frame5;
	Mat fgmask1,fgmask2;
	Mat Cropped1,Cropped2;
	Rect box1,box2;	
	Mat matched;
	char a[30],b[30];		
	int i = 0,c;
	bool d;
	
	//start pipes
	pipeInitialize();
	
	//initial point
	CameraX = 20;
	CameraY = 80;
	sendCameraPostion(CameraX,CameraY);
	usleep(100000);
	cout << endl;
	
	// Create a window
  	namedWindow( "Window1", CV_WINDOW_AUTOSIZE );
  	
  	for(;;)
  	{
  	
  		c = waitKey(100);
  			
		//to quit the program
		if( c != 255 && c == (int)'e' )
			break;
			
		//to read the frames and process	
		if( c != 255 && c == (int)'r' )	
  		{
  			cout << "READING FRAMES" << endl;
  			
		  	//read frames
		  	readFrames(frame1,frame2,frame3,frame4);
		  	
		  	CameraViewWidth = frame1.cols;
			CameraViewHeight = frame1.rows;
		  	
		  	
		  	//Perform trackbar requirements if required
		  	if(ShowTrackbar == true)
		  	{
		  		char Name[100];
		  		
		  		//Create a window
	  			namedWindow( "TrackBar", CV_WINDOW_AUTOSIZE );
		  		
		  		strcpy(Name,"Threshold");
		  		createTrackbar( Name, "TrackBar", &ThresholdValue, 50, NULL );
		  		
		  		strcpy(Name,"Blur");
		  		createTrackbar( Name, "TrackBar", &BlurValue, 30, NULL );
		  		
		  		strcpy(Name,"Morph");
		  		createTrackbar( Name, "TrackBar", &MorphSize, 20, NULL );	
		  		
		  		strcpy(Name,"Area filter Minimum");
		  		createTrackbar( Name, "TrackBar", &MinAreaFilter, 1000, NULL );
		  		
		  		strcpy(Name,"Area filter Maximum");
		  		createTrackbar( Name, "TrackBar", &MaxAreaFilter, 300000, NULL );
		  	}
		  	
		  	//initialize 
		  	box1.width = 0;
		  	box1.height = 0;
		  	box2.width = 0;
		  	box2.height = 0;
		  	
		  	// for the first three frames and iteration one
		  	fgmask1 = detectMotion(frame1,frame2,frame3);							//detect motion in the camera
		  	box1 = blobAnalysis(fgmask1);											//blob analysis
		  	Cropped1 = frame3(box1);
		  	
		  	// for the first three frames and iteration one
		  	fgmask2 = detectMotion(frame2,frame3,frame4);							//detect motion in the camera
		  	box2 = blobAnalysis(fgmask2);											//blob analysis
		  	Cropped2 = frame4(box2);
		  	
		  	//feature matching
		  	//cout << endl << "Hist-Check " << histcheck(Cropped1,Cropped2) << endl;
    		//cout << "Feature-Check " << featurecheck(Cropped1,Cropped2,matched) << endl << endl;

		  	rectangle( frame3, box1.tl(), box1.br(),Scalar(0,0,255) , 2, 8, 0 ); 		//Draw rectangle
		  	rectangle( frame4, box2.tl(), box2.br(),Scalar(0,0,255) , 2, 8, 0 ); 		//Draw rectangle
		  	computeParameter(box1,box2);
		  	sendCameraPostion(CameraX,CameraY);		  									//move motor
		  	
		  	usleep(1000000);
		  	openCamera(CameraNumber);
			Source.read(frame5);
			Source.release();
		  				  		  		
			//display the result			
			strcpy(a,"../Photos/");
			strcat(a,"MotionFrame-1");
			sprintf(b,"%d",i);
			strcat(a,b);
			strcat(a,".jpg");
			imwrite(a,frame3);
			
			strcpy(a,"../Photos/");
			strcat(a,"MotionFrame-2");
			sprintf(b,"%d",i);
			strcat(a,b);
			strcat(a,".jpg");
			imwrite(a,frame4);
			
			strcpy(a,"../Photos/");
			strcat(a,"ReferenceFrame");
			sprintf(b,"%d",i);
			strcat(a,b);
			strcat(a,".jpg");
			imwrite(a,frame5);
								
			i = i + 1;						
			imshow("Window1", frame3);
		}
	}
	
	Source.release();
	closePipe();
	for(;;);
	return 0;
}

