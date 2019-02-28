#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>

/***********************************************************************
 * The MIT License (MIT)
 * 
 * @copyright (c) 2014 Jochen Peters, Krefeld, Germany
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

using namespace std;
using namespace cv;
Mat workImg;

// mark the "readyness" to stop cropping and start storeing
bool ready;
// first click = start cropping
// second click = stop cropping
// fourth click = start new cropping
int clickCount;
// the 4 points of the crop-region
Point2f selection[4];
// cropping size
int width;
int height;

// Colors vor the crop region
Scalar borderColor(255,255,255);
Scalar fillColor(255,255,0);

/**
 * Button Callback to close fltk window-thread and store image
 */
void doneCallback(Fl_Widget *, void* win) {
	ready = true;
	((Fl_Window *) win )->hide();
}

/**
 * Button Callback to close fltk window-thread and do not store image
 */
void cancelCallback(Fl_Widget *, void* win) {
	ready = false;
	((Fl_Window *) win )->hide();
}
/**
 * Button Callback to close fltk window-thread and store image as jpeg
 */
void asJpegCallback(Fl_Widget *, void* win) {
	ready = true;
	clickCount = -10; // indicator for jpeg Button
	((Fl_Window *) win )->hide();
}

/**
 * start coping an image src by the left upper point (poi) into
 * a des(tination) image and mix the images via 0..1 = alpha.
 * An alpha = 1.0 -> full src Image!
 */
void copyAlpha(Mat & src, Mat & des, Point2f poi, float alpha) {
	for(int y=poi.y; y<(poi.y + src.rows); y++) {
		for(int x=poi.x; x<(poi.x + src.cols); x++) {
			des.at<Vec3b>(y,x) = (1-alpha) * des.at<Vec3b>(y,x) + alpha * src.at<Vec3b>(y-poi.y,x-poi.x);
		}
	}
}

/**
 * swaps two integers
 */
void swap(int &a, int &b) {
	int swap;
	swap = a;
	a = b;
	b = swap;
}

/**
 * Draws a nice cropping frame into an image
 * @uses copyAlpha()
 */
void MarkRegion(Mat & viewImg, Point2f p1, Point2f p2) {
	int mwidth  = p2.x - p1.x;
	int mheight = p2.y - p1.y;

	/// make mirroring in y-axis possible
	if(mwidth<0) {
		mwidth  *= -1;
		swap(p1.x, p2.x);
	}
	/// make mirroring in x-axis possible
	if(mheight<0) {
		mheight *= -1;
		swap(p1.y, p2.y);
	}

	Mat tempImg(mheight,mwidth,CV_8UC3);
	
	rectangle(tempImg, Point2f(0, 0), Point2f(mwidth, mheight), fillColor, -1);
	copyAlpha(tempImg, viewImg, p1, 0.2);
	
	rectangle(viewImg, p1, p2, borderColor);
}

/**
 * callback on every mouse event
 * make selection and (right-click) store-dialog possible
 */
void navigate(int event, int xw, int yw, int flags, void * param) {
	static Point sel;
	sel.x = xw;
	sel.y = yw;

	switch (event) {
		/// start, stop, restart cropping-function
		case CV_EVENT_LBUTTONDOWN:
			if(clickCount>2) clickCount=0;
			if(clickCount==0) {
				selection[0].x = xw;
				selection[0].y = yw;
			}
			clickCount++;
			break;
		
		/// stop cropping
		case CV_EVENT_RBUTTONDOWN:
			ready = true;
			break;
			
		/// set positions to redraw selection (cropping region)
		case CV_EVENT_MOUSEMOVE:
			if(clickCount==1) {
				selection[1].x = xw;
				selection[1].y = selection[0].y;
				
				selection[2].x = xw;
				selection[2].y = yw;
				
				selection[3].x = selection[0].x;
				selection[3].y = yw;
			}
			break;
	}
	return;
}

/**
 * makes new image from cropping region
 */
void calcResult(Mat &resultImg) {
	width  = selection[1].x - selection[0].x;
	height = selection[3].y - selection[0].y;
	Point2f sourceTri[4];

	if(width<0)  width  *= -1;
	if(height<0) height *= -1;
	
	sourceTri[0] = Point2f(0, 0);
	sourceTri[1] = Point2f(width, 0);
	sourceTri[2] = Point2f(width, height);
	sourceTri[3] = Point2f(0, height);

	Mat warp = getPerspectiveTransform(selection, sourceTri);
	workImg.convertTo(workImg, CV_8UC3);
	warpPerspective(
		workImg,
		resultImg,
		warp,
		Size(width, height)
	);
}

int main(int argc, char** argv) {
	Mat resultImg, viewImg;
	ready = false;
	clickCount = 0;
	char sizeLable[15];

	if(argc < 2) {
		cout<<"=> The image-file-path parameter is missing !"<<endl;
		exit(1);
	}
	
	workImg = imread(argv[1]);
	if(workImg.rows == 0) {
		cout<<"=> The image-file-path or name is wrong, file does not exist or is corrupt!"<<endl;
		return 1;
	}

	/// Automatic resizing result window
	namedWindow((string) "+ RESULT +", WINDOW_AUTOSIZE);
	/// scalable window to crop/select a region
	namedWindow((string) "see and crop", WINDOW_NORMAL);
	
	setMouseCallback((string) "see and crop", navigate);
	/// some initial images
	resultImg = workImg.clone();
	viewImg = workImg.clone();
	imshow((string) "+ RESULT +", resultImg);
	imshow((string) "see and crop", viewImg);

	resizeWindow(
		(string) "see and crop",
		800,
		800.0 * viewImg.rows / viewImg.cols
	);
	
	while(true) {
		/// endless loop to make a live refresh while drawing the selection
		viewImg = workImg.clone();
		if(clickCount>0) {
			/// draw/recalculate new region into result
			calcResult(resultImg);
			imshow((string) "+ RESULT +", resultImg);
			MarkRegion(viewImg, selection[0], selection[2]);
			sprintf(sizeLable, "%d x %d", width, height);
			putText(viewImg, sizeLable, Point2f(2,13), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar::all(0), 2, CV_AA);
			putText(viewImg, sizeLable, Point2f(1,12), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar::all(255), 1, CV_AA);
		}
		/// redraw cropping-Tool image/window
		imshow((string) "see and crop", viewImg);

		/// leave loop on keypress
		if(waitKey(30) >= 0) break;
		/// or leave it, if some mouse interaction says it
		if(ready == true) break;
	}

	/// now we need 'ready' for GUI interaction
	ready = false;

	/// we only need the fltk framework for this small dialog :-/
	Fl_Window win(420, 45, "Save to ...");
	Fl_Input input(10, 10, 190, 25, "");
	input.value(argv[1]);
	Fl_Button *done = new Fl_Button(210, 10, 60, 25, "save");
	Fl_Button *asJpeg = new Fl_Button(275, 10, 60, 25, ".jpg");
	Fl_Button *cancel = new Fl_Button(350, 10, 60, 25, "cancel");
	/// we set ready =true/false in the callbacks!
	done->callback(doneCallback, (void*) &win); // save image
	asJpeg->callback(asJpegCallback, (void*) &win); // save image as jpg
	cancel->callback(cancelCallback, (void*) &win); // not save image
	win.show();
	Fl::run();

	/// we store it?
	if(ready == true) {
		if(clickCount == -10) {
			/// store as jpeg with 90% quality
			vector<int> params;
			params.push_back(CV_IMWRITE_JPEG_QUALITY);
			params.push_back(90);
			imwrite((string) input.value()+".jpg", resultImg, params);
		} else {
			/// store it in the same format as we opened
			imwrite(input.value(), resultImg);
		}
	}

	return 0;
}