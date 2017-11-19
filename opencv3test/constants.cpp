#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

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

Mat get_roi(Mat frame, Rect roi, int binThreshold) {
	cvtColor(frame, frame, CV_BGR2GRAY);
	Mat frameRoi = frame(roi);
	Mat binarized;
	threshold(frameRoi, binarized, binThreshold, 255, CV_THRESH_BINARY);
	return binarized;
}




