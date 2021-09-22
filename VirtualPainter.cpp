#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

///////////// Virtual Painter ////////////////

Mat img;
vector<vector<int>> newPoints;

//choose the object(the marker cap for instance) color want to be detected
vector<vector<int>> myColors{ {81,131,0,123,255,255}, //blue
    {155,152,156,179,255,255} }; //red

//the colors of tracks used to be drawn on canvas
vector<Scalar> myColorValues{ {0,0,255}, //blue
    {255,0,0} }; //red

Point getContours(Mat imgDil) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0,0);

	for (int i = 0;i < contours.size();i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		vector<vector<Point>> conPoly(contours.size());
		vector<Rect> boundRect(contours.size());
		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);

			int objCor = (int)conPoly[i].size();
			
			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
		}
		return myPoint;
	}

}

vector<vector<int>> findColor(Mat img) {
	Mat imgHSV;	
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0;i < myColors.size();i++) 
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		Point myPoint = getContours(mask);

		if (myPoint.x != 0 && myPoint.y != 0)
		{
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}		
	}
	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{
	for (int i = 0;i < newPoints.size();i++)
	{
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
	}
}

void main() {

	VideoCapture cap(0);

	while (true) {

		cap.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, myColorValues);

		imshow("Image", img);
		waitKey(1);
	}
}
