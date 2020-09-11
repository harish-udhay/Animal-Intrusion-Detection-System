// OpenCV visual capture
// openCamera() is Code snippet to turn on web camera
// readFrames() is the code snippet to read visuals through the camera
// detectMotion() is the code snippet to detect motion

int openCamera(int Camera)
{
	//setting camera properties
	//source.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	//source_set(CV_CAP_PROP_FRAME_HEIGHT, 720);

	//Opening a capturing device or video to open video input the filename to open()

	Source.open(Camera);
	if(!Source.isOpened())
	{
		cout<<"ERROR ACQUIRING VIDEO FEED\n";
		return -1;
	}
}

void readFrames(Mat &f1, Mat &f2, Mat &f3, Mat &f4)
{
	Mat temp1, temp2, temp3, temp4;

	openCamera(CameraNumber);
	source.read(temp1);
	source.release();
	usleep(FrameSkipTime);
	f1 = temp1;

	openCamera(CameraNumber);
	Source.read(temp2);
	source.release();
	usleep(FrameSkipTime);
	f2 = temp2;

	openCamera(CameraNumber);
	Source.read(temp3);
	source.release();
	usleep(FrameSkipTime);
	f3 = temp3;

	openCamera(CameraNumber);
	Source.read(temp4);
	source.release();
	usleep(FrameSkipTime);
	f4 = temp4;
}

Mat detectMotion(Mat &f1, Mat &f2, Mat &f3)
{
	Mat prev_frame, curr_frame, next_frame;
	prev_frame = f1;
	curr_frame = f2;
	next_frame = f3;

	Mat gray1, gray2, gray3;	
}

//Convert all the frames to gray
Mat gray1, gray2, gray3;
cvtColor(prev_frame, gray1, COLOR_BGR2GRAY);
cvtColor(curr_frame, gray2, COLOR_BGR2GRAY);
cvtColor(next_frame, gray3, COLOR_BGR2GRAY);

//difference between frames
Mat diff1, diff2;
absdiff(gray1, gray3, diff1);
absdiff(gray2, gray3, diff2);

//compute the result frame

Mat result;
bitwise_and(diff1, diff2, result);

//post-process the frames

threshhold(result, result, ThreshholdValue, 255. THRESH_BINARY);
blur(result, result, Size(BlurValue, BlurValue));
threshhold(result, result, ThreshholdValue, 255. THRESH_BINARY);
Mat element = getStructuringElement( morph_rect, sIZE( 2*MorphSize + 1, 2*MorphSize+1), Point(MorphSize));
morphologyEx( result, result, MORPH_CLOSE, element);