#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
//#include<Windows.h>
#include<iostream>
#include<vector>
using namespace std;
using namespace cv;


int main()
{
	Mat img;
	Mat imgt;
	VideoCapture cap(0);

		Mat fore,back;
		Mat cont;
		BackgroundSubtractorMOG2 bg;//works on GMM
		bg.set ("nmixtures", 10);
		vector < vector < Point > >contours;
		
		double t=getTickCount()/getTickFrequency();
	for(;;)
	{
		cap>>img;
		cvtColor(img,img,CV_BGR2HSV);
		flip(img,img,1);
		//Thresholding and extraction part__________________________________________
		static int BL=0;
		static int GL=65;
		static int RL=72;
		static int BH=75;
		static int GH=230;
		static int RH=194;
		namedWindow("Color",WINDOW_NORMAL);
		createTrackbar("BL","Color",&BL,255);
		createTrackbar("GL","Color",&GL,255);
		createTrackbar("RL","Color",&RL,255);
		createTrackbar("BH","Color",&BH,255);
		createTrackbar("GH","Color",&GH,255);
		createTrackbar("RH","Color",&RH,255);
		inRange(img,Scalar(BL,GL,RL),Scalar(BH,GH,RH),imgt);
		static int e=1,d=5;
		int i=0,j=0,k=0;
		createTrackbar("erosion","Color",&e,5);
		createTrackbar("dilation","Color",&d,10);
		for(i=0;i<e;i++)
		{
			erode(imgt,imgt,cv::Mat());
		}
		for(j=0;j<d;j++)
		{
			dilate(imgt,imgt,cv::Mat());
		}
		Mat mask;
		cvtColor(imgt,imgt,CV_GRAY2BGR);
		cvtColor(img,img,CV_HSV2BGR);
		absdiff(img,img,mask);//initialize
		subtract(imgt,img,mask);
		subtract(imgt,mask,mask);
		

		//Contour edges Part_______________________________________________________
		
		Mat add,edge;
		Mat draw;
		absdiff(imgt,imgt,draw);
		static int cannyt1=127,cannyt2=73;
		createTrackbar("CannyT1","Color",&cannyt1,200);
		createTrackbar("CannyT2","Color",&cannyt2,200);
		Canny(mask,edge,cannyt1,cannyt2,3,0);
		dilate(edge,edge,cv::Mat());
		//imshow("edge",edge);
		Scalar color=Scalar(255,0,0);
		findContours (edge, contours, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);//cause of unhandled exceptions
		
		if(contours.size()>0)
		{
		
		for(i=0;i<contours.size();i++)
		{
		 if(contourArea(contours[i])>30)//to eliminate noise contours
			 {
				drawContours (imgt, contours, -1, color, 2);
				drawContours (mask, contours, -1, color, 2);
			 }
		 else if(contours.size()<5)
		 {
			cout<<"no contours\n";
			putText(draw,"No Contours",Point(30,30),FONT_HERSHEY_SIMPLEX,2,Scalar(200,200,0),1.5);
		 
		 }
		 
		}
		//approxPolyDP(contours,contours2,2.0,1);
		
		vector < vector < Point > >contours2(contours.size());
		static int polydp=18;
		createTrackbar("PolyDP","Color",&polydp,50);
		//Rect r;
		for(i=0;i<contours.size();i++)
		{
			approxPolyDP( Mat(contours[i]), contours2[i], polydp, true );
													
		}
		//r=boundingRect(contours2[max]);
		int max=90,maxi=-10;
		vector<Rect> boundRect( contours2.size() );
		for(i=0;i<contours2.size();i++)
		{
			 boundRect[i] = boundingRect( contours2[i] );
		}

		for(i=0;i<boundRect.size();i++)
		{
			if(boundRect[i].width*boundRect[i].height>max)
			 {
				max=boundRect[i].width*boundRect[i].height;
				maxi=i;
			 }
			drawContours (imgt, contours2, -1, Scalar(0,255,0), 2);
			drawContours (draw, contours2, -1, Scalar(0,255,0), 2);
            
			//drawContours (draw, contours2[maxi], -1, Scalar(255,255,255), 4);
			//rectangle( draw, boundRect[maxi].tl(), boundRect[maxi].br(), Scalar(0,255,255),2,0);
			 
		}
			
      		 if(max>90 && maxi>=0)
		{
			cout<<"largest rect is "<<maxi<<"\n";
			rectangle(draw,boundRect[maxi],Scalar(0,255,255),3,8);
			putText(draw,"Tracked",Point(boundRect[maxi].x,boundRect[maxi].y),FONT_HERSHEY_TRIPLEX,1.5,Scalar(0,250,255),1.5);
          		  Point cursor=Point((boundRect[maxi].x)*1366/320,(boundRect[maxi].y)*768/480);
           		 //SetCursorPos(cursor.x,cursor.y);
			
		}

		if(maxi>0)
		{
		//convex Hull part____________________________________________
		
		
		vector<vector<Point> > hull_point(contours2.size());//hull points
		 vector<vector<int> >hull( contours2.size() );//hull ints
		vector<Vec4i> defects;
		//cout<<"largest rect is still"<<maxi<<"\n";
		for(i=0;i<contours2.size();i++)
			{
				convexHull(Mat(contours2[i]),hull_point[i],true);//point based	
				convexHull(Mat(contours2[i]),hull[i],true);//int based
				drawContours (draw, hull_point, -1, Scalar(0,200,250), 2);
				drawContours (mask, hull_point, -1, Scalar(0,200,250), 2);
						
			}

		

		

		
		}
		
		moveWindow("draw",660,0);
			imshow("draw",draw);
			imshow("mask",mask);
		}//end of if condition
		else
		{
			cout<<"still screen\n";
		}
		
		cvtColor(edge,edge,CV_GRAY2BGR);
		moveWindow("HSV",0,0);
		moveWindow("Color",320,0);
		resize(img,img,Size(320,240));
		imshow("HSV",img);
		cout<<"______________________________________\n";
		char c=waitKey(5);
	    if(c=='b' || c=='B')
            {
                break;
            }
	}//end of infinite loop

	return 1;
	

}
