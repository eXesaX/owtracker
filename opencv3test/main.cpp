#include "opencv2/opencv.hpp"
#include "opencv2/core/ocl.hpp"
#include <iostream>
#include "time.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

#include <Windows.h>

#include <baseapi.h>
#include <allheaders.h>

using namespace cv;
using namespace std;

const Rect SCOREBOARD_ELIMS = Rect(163, 442, 242, 69);
const Rect SCOREBOARD_OBJ_ELIMS = Rect(433, 442, 242, 69);
const Rect SCOREBOARD_OBJ_TIME = Rect(702, 442, 242, 69);
const Rect SCOREBOARD_DAMAGE = Rect(974, 442, 242, 69);
const Rect SCOREBOARD_HEALING = Rect(1244, 442, 242, 69);
const Rect SCOREBOARD_DEATHS = Rect(1513, 442, 242, 69);
const Rect SCOREBOARD_MATCH_TIME = Rect(425, 68, 66, 26);

const Rect SR = Rect(857, 459, 162, 78);

const Rect LOADING_BLUE_TEAM_SR = Rect(632, 228, 112, 59);
const Rect LOADING_RED_TEAM_SR = Rect(1208, 228, 112, 59);

const Rect MENU_STILL_SR = Rect(1102, 507, 89, 31);

const Rect SR_ROI = Rect(992, 1011, 296, 29);
const Rect SCOREBOARD_ROI = Rect(1570, 284, 131, 131);
const Rect LOADING_ROI = Rect(1735, 896, 83, 83);
const Rect MENU_ROI = Rect(1091, 564, 60, 60);

Mat get_roi(Mat frame, Rect roi, int binThreshold) {
	cvtColor(frame, frame, CV_BGR2GRAY);
	Mat frameRoi = frame(roi);
	Mat binarized;
	threshold(frameRoi, binarized, binThreshold, 255, CV_THRESH_BINARY);
	return binarized;
}

Mat get_sr(Mat frame) {
	Mat bin = get_roi(frame, SR, 128);
	return bin;
}

Mat get_menu_sr(Mat frame) {
	Mat bin = get_roi(frame, MENU_STILL_SR, 225);
	return bin;
}

void get_teams_sr(Mat frame, Mat* output) {
	output[0] = get_roi(frame, LOADING_BLUE_TEAM_SR, 225);
	output[1] = get_roi(frame, LOADING_RED_TEAM_SR, 225);
}

void get_scores(Mat frame, Mat* output) {
	output[0] = get_roi(frame, SCOREBOARD_ELIMS, 128);
	output[1] = get_roi(frame, SCOREBOARD_OBJ_ELIMS, 128);
	output[2] = get_roi(frame, SCOREBOARD_OBJ_TIME, 128);
	output[3] = get_roi(frame, SCOREBOARD_DAMAGE, 128);
	output[4] = get_roi(frame, SCOREBOARD_HEALING, 128);
	output[5] = get_roi(frame, SCOREBOARD_DEATHS, 128);
	output[6] = get_roi(frame, SCOREBOARD_MATCH_TIME, 128);
}


Mat openImage(string name) {
	Mat image;
	image = imread(name.c_str(), IMREAD_COLOR);

	if (image.empty())                     
	{
		cout << "Could not open or find the image" << std::endl;
		return image;
	}

	return image;
}

bool matchWithPattern(Mat frame, Mat pattern, Rect roi, double detectionThreshold) {
	cvtColor(frame, frame, CV_BGR2GRAY);
	Mat frameRoi = frame(roi);
	threshold(frameRoi, frameRoi, 128, 255, CV_THRESH_BINARY);
	cvtColor(pattern, pattern, CV_BGR2GRAY);

	//imshow("frameRoi", frameRoi);
	//imshow("pattern", pattern);

	Mat diff;
	absdiff(frameRoi, pattern, diff);
	//imshow("diff", diff);
	double diffSum = mean(diff)[0];
	cout << diffSum << " ";
	return diffSum < detectionThreshold;
}

void savePattern(Mat frame, Rect roi, int binThreshold, string filename) {
	Mat binarized = get_roi(frame, roi, binThreshold);
	imshow("saved", binarized);
	imwrite(filename, binarized);
}

string OCR(tesseract::TessBaseAPI* api, Mat image) {
	//Pix* tessImg = pixRead("try.png");
	imwrite("C:\\Projects\\owtracker\\x64\\Debug\\temp.bmp", image);
	Pix* tesImg = pixRead("C:\\Projects\\owtracker\\x64\\Debug\\temp.bmp");
	api->SetImage(tesImg);
	//api->SetImage(tessImg);
	api->SetVariable("classify_bln_numeric_mode", "1");
	string outText = string(api->GetUTF8Text());
	//printf("OCR:\n%s", outText);
	return outText;
}

Mat hwnd2mat(HWND hwnd) {

	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom;  //change this to whatever size you want to resize to
	width = windowsize.right;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

																									   // avoid memory leak
	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main(int argc, char** argv) {
	//VideoCapture vid("C:\\Projects\\owtracker\\x64\\Debug\\vid.mkv");
	//bool opened = vid.open();
	//cout << vid.isOpened() << " " << endl;
	//cout << vid.get(CV_CAP_PROP_FOURCC) << endl;

	HWND hDesktopWnd = GetDesktopWindow();

	

	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

	if (api->Init(NULL, "owdig")) {
		fprintf(stderr, "Could not init tesseract\n");
		exit(1);
	}

	Mat currentFrame;
	Mat binPattern = openImage("C:\\Projects\\owtracker\\x64\\Debug\\sr_pattern.png");
	Mat scoreboardPattern = openImage("C:\\Projects\\owtracker\\x64\\Debug\\scoreboard_pattern.png");
	Mat loadingPattern = openImage("C:\\Projects\\owtracker\\x64\\Debug\\loading_pattern.png");
	Mat menuPattern = openImage("C:\\Projects\\owtracker\\x64\\Debug\\menu_still_pattern.png");
	
	while (true) {
		/*bool ok = vid.read(currentFrame);
		if (!ok) break;*/

		currentFrame = hwnd2mat(hDesktopWnd);
		//currentFrame = openImage("C:\\Projects\\owtracker\\x64\\Debug\\currentFrame.png");
		//cout << currentFrame.cols << " " << currentFrame.rows << endl;

/*
		double timestamp = vid.get(CV_CAP_PROP_POS_MSEC);
		double frameCount = vid.get(CV_CAP_PROP_POS_FRAMES);
		double progress = vid.get(CV_CAP_PROP_POS_AVI_RATIO);
		cout << "time: " << timestamp << " frame N: " << frameCount << " progress: " << progress << endl;
*/
		bool isSr = matchWithPattern(currentFrame, binPattern, SR_ROI, 25);
		bool isScoreboard =  matchWithPattern(currentFrame, scoreboardPattern, SCOREBOARD_ROI, 25);
		bool isLoading =  matchWithPattern(currentFrame, loadingPattern, LOADING_ROI, 20);
		bool isMenu =  matchWithPattern(currentFrame, menuPattern, MENU_ROI, 30);

		//bool isSr = false;
		//bool isScoreboard = false;
		//bool isLoading = false;
		//bool isMenu = false;
		cout << endl;
		if (isSr) {
			cout << "sr screen found" << endl;
			
			Mat toOcr = get_sr(currentFrame);
			string outText = OCR(api, toOcr);
			cout << outText << endl;
		}
		if (isScoreboard) {
			cout << "scoreboard screen found" << endl;
			string outText[7];
			Mat scores[7];
			get_scores(currentFrame, scores);
			for (int i = 0; i < 7; i++) {
				outText[i] = OCR(api, scores[i]);
			}

			cout << "ELIM " << outText[0] << endl;
			cout << "OBJ " << outText[1] << endl;
			cout << "OBJTIME " << outText[2] << endl;
			cout << "DMG " << outText[3] << endl;
			cout << "HEAL " << outText[4] << endl;
			cout << "DEATH " << outText[5] << endl;
			cout << "TIME " << outText[6] << endl;
		}
		if (isLoading) {
			cout << "loading screen found" << endl;
			string outText[2];
			Mat teams_sr[2];
			get_teams_sr(currentFrame, teams_sr);
			outText[0] = OCR(api, teams_sr[0]);
			outText[1] = OCR(api, teams_sr[1]);
			cout << outText[0] << endl;
			cout << outText[1] << endl;


		}
		if (isMenu) {
			cout << "menu screen found" << endl;
			Mat toOcr = get_menu_sr(currentFrame);
			string outText = OCR(api, toOcr);
			cout << outText << endl;
		}

		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	cout << "exiting..." << endl;
	/*while (true) {
		
	}*/


}

