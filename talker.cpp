#include <iostream>
#include "std_msgs/String.h"
#include <std_msgs/UInt16.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"
#include <vector>
#include <sstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cv.h>
#include "ros/ros.h"
#include <stdlib.h>

#define XMID 320
#define A_XMAX 55 	//right angle
#define A_XMIN 180	//left angle

#define YMID 240	//mid
#define A_YMAX 30 	//top angle
#define A_YMIN 140	//bottom angle
using namespace cv;
using namespace std;

 int main( int argc, char** argv )
 {


	ros::init(argc, argv, "talker");
	ros::NodeHandle n;
	ros::Publisher chatter_pub = n.advertise<std_msgs::Float32MultiArray>("servo", 1000);
	ros::Rate loop_rate(10);
	std_msgs::Float32MultiArray msg;

    VideoCapture cap(1); //capture the video from webcam

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the web cam" << endl;
         return -1;
    }

 //create a window called "Control"
    namedWindow("Control", CV_WINDOW_AUTOSIZE);

		  int iLowH = 0;
		  int iHighH = 179;

		  int iLowS = 100; 
		  int iHighS = 255;

		  int iLowV = 60;
		  int iHighV = 255;

  //Create trackbars in "Control" window
		  createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
		  createTrackbar("HighH", "Control", &iHighH, 179);

		  createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
		  createTrackbar("HighS", "Control", &iHighS, 255);

		  createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
		  createTrackbar("HighV", "Control", &iHighV, 255);

  //Capture a temporary image from the camera
		  Mat imgTmp;
		  cap.read(imgTmp); 

  //Create a black image with the size as the camera output
		 //Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
		 Mat imgCircles = Mat::zeros( imgTmp.size(), CV_8UC3 );;
 
	float Y_angle = 55;
	float X_angle = 115;
  	int a = 0;
    while (true)
    {
        Mat imgOriginal;
	imgCircles = Mat::zeros(imgTmp.size(), CV_8UC3);

        bool bSuccess = cap.read(imgOriginal); // read a new frame from video



         if (!bSuccess) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }

    	Mat imgHSV;

   	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
 
  	Mat imgThresholded;

   	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
      
	  //morphological opening (removes small objects from the foreground)
	  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

	   //morphological closing (removes small holes from the foreground)
	  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
	  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	   //Calculate the moments of the thresholded image
	  Moments oMoments = moments(imgThresholded);

	  double dM01 = oMoments.m01;
	  double dM10 = oMoments.m10;
	  double dArea = oMoments.m00;

   // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
	  if(dArea < 1000000){
		printf("NO OBJECT CAN BE DETECTED!\n");
	  }

	  if (dArea > 100000)
	  {
		   //calculate the position of the ball
		   int posX = dM10 / dArea;
		   int posY = dM01 / dArea;        
		  
		   Point CircleCenter;
		   CircleCenter=Point(posX,posY);
			   
			int Radius;
			Scalar Color;
			int Thickness;
			int Shift;

		   Radius=45;
		   Color=CV_RGB(0,0,255);
		   Thickness=3;
		   Shift = 0;

		/*-----------------------------------------------------------------*/
		std_msgs::Float32MultiArray msg;	//Create a "String" type object named "msg".

			//FUCK
		msg.data.push_back(Y_angle);
		msg.data.push_back(X_angle);
		//msg.data[0] = Y_angle;
		//msg.data[1] = X_angle;
			//FUCK

		//msg.data[2] = X_angle;
		//ROS_INFO("%d", msg.data);	//Print the data of the "msg.data" to the screen
		chatter_pub.publish(msg);	//Publish the object "msg" which contains some data through the topic "chatter". 
		ros::spinOnce();	//For callbacks

		    loop_rate.sleep();	//wait till next loop
		    
			float dis = 0;

			//UP and DOWN
			if( (posY-240) > 70 ){
				dis = (posY-240)*0.012;
				//printf("%f\tRIGHT\n", dis);
				Y_angle += 1;
				printf("%f\tDOWN\n", Y_angle);
			}

			if( (posY-240) < (-70) ){
				dis = (posY-240)*(-0.012);
				//printf("%f\tDOWN\n", Y_angle);
				Y_angle -= 1;
				printf("%f\tUP\n", Y_angle);
			} 	 	

			if( Y_angle <= 50){
				printf("TOP!!!\n");
				Y_angle = 50;
			}

			if( 140 == Y_angle){
				Y_angle = A_YMIN;
			}


			dis = 0;
			//LEFT and RIGHT

			if( (posX-320) < (-70) ){
				dis = (posX-XMID)*0.015;
				//printf("%f\tLEFT\n", dis);
				X_angle += 1;
			}

			if( (posX-320) > 70 ){
				dis = (posX-XMID)*(0.015);
				//dis = (posY-YMID);
				//if(dis < 0)	dis = 0;

				//printf("%f\tRIGHT\n", dis);
				X_angle -= 1;
			} 	 	
	
					if( X_angle ==55){
						X_angle = A_XMAX;
					}

					if( X_angle == 180){
						X_angle = A_XMIN;
					}

		


		/*-----------------------------------------------------------------*/
			circle(imgCircles,CircleCenter,Radius,Color,Thickness,CV_AA,Shift); 
			printf("X_pos = %d\tY_pos = %d\n",posX,posY);
	
	  }


			   imshow("Thresholded Image", imgThresholded); //show the thresholded image
			  // imshow("Circle Image", imgCircles);

			   imgOriginal = imgOriginal + imgCircles;
			  imshow("Original", imgOriginal); //show the original image

			if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		       {
			    cout << "esc key is pressed by user" << endl;
			    break; 
		       }
    }

   return 0;
}
