#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

const int kvalue = 15;//双边滤波邻域大小

int main()
{
	string filename = "../yuan_mu.png";
	Mat img = imread(filename, 0);
	Mat ori = imread(filename);
	if(img.empty()) {
		cout << "can not open " << filename << endl;
		return -1;
	}
	Mat cimg;
	medianBlur(img, img, 5);
	cvtColor(img, cimg, COLOR_GRAY2BGR);
	vector<Vec3f> circles;
	HoughCircles(img, circles, HOUGH_GRADIENT, 1, 30, 150, 32, 10, 60 );
	int cnt = 0;
	int row = cimg.rows;
	int col = cimg.cols;

	int limit = 10;
	for( size_t i = 0; i < circles.size(); i++ ) {
		Vec3i c = circles[i];
		// circle( cimg, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, LINE_AA);
		// circle( cimg, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, LINE_AA);
		int x0 = c[0] - c[2];
		int y0 = c[1] + c[2];
		int x1 = c[0] + c[2];
		int y1 = c[1] - c[2];
		if ( x0 + limit> 0 && y0-limit < row  && x1 - limit< col && y1+limit > 0) {
			cnt ++;
			rectangle(ori,Point(c[0]-c[2],c[1]+c[2]),Point(c[0]+c[2],c[1]-c[2]),Scalar(255,255,0));
		}
	}
	cout << "total size: " << cnt << endl;
	// imshow("detected circles", cimg);
	imwrite("result.png", ori);
	waitKey();

	return 0;
}

