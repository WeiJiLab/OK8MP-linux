
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/videoio/legacy/constants_c.h>

using namespace std;
using namespace cv;
       
Mat frame, grad, src_gray;
Mat grad_x, grad_y;
Mat abs_grad_x, abs_grad_y;

static void help()
{
    std::cout << "\n\nHot keys: \n"
            "\tESC - quit the program\n";
}
int close = 0;
void OnMouseAction(int event,int x,int y,int flags,void *ustc){
	if(event==CV_EVENT_LBUTTONDOWN)
		close = 1;
}
namespace{

    int process(VideoCapture& capture) {
	namedWindow("Sobel", WINDOW_NORMAL);
	moveWindow("Sobel", 0, 40);  
	//resizeWindow("Sobel", 1024, 600);
	setMouseCallback("Sobel",OnMouseAction);
//	setWindowProperty("Sobel", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	while(!close) {
		capture >> frame;
		if (frame.empty())
			break;
		cvtColor(frame, src_gray, COLOR_RGB2GRAY);

		Sobel(src_gray, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
		convertScaleAbs(grad_x, abs_grad_x);
		Sobel(src_gray, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);
		convertScaleAbs(grad_y, abs_grad_y);
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

		imshow("Sobel", grad);
		char key = (char)waitKey(30);
		switch (key) {
			case 27: //escape key
				return 0;
			default:
				break;
		}
	}
	return 0;
	}
}

int main(int argc, const char *argv[])
{
	int device_id = 0;
	help();

	char *pEnv = getenv("OPENCV_VIDEO_DEVICE");
	if ((pEnv != NULL) && (strstr(pEnv, "/dev/video") != NULL)) {
		device_id = atoi(&pEnv[strlen("/dev/video")]);
	}

	VideoCapture capture(device_id, cv::CAP_GSTREAMER);

//	capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);  
//	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
//	capture.set(CV_CAP_PROP_FPS, 30.0);
	if(!capture.isOpened()){
		cerr << "open fail !\n" << endl;
        return 1;
    }
        
    return process(capture);
}






