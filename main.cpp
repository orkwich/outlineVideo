#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio/videoio_c.h>
using namespace std;
using namespace cv;

typedef struct
{
    double fps;
    int x,y;
    string fourcc="";
    int frames;
    bool isColor;
}videoData;

videoData extract_frames(const string &videoFilePath,vector<Mat>& frames)
{
	videoData vd;
	try{
  	    VideoCapture cap(videoFilePath);
  	    if(!cap.isOpened())
        {
            cout<<"err\n";
            vd.fps=-1;
            return vd;
        }
  	    for(int frameNum = 0; frameNum < cap.get(CV_CAP_PROP_FRAME_COUNT);frameNum++)
  	    {
  		    Mat frame;
  		    cap >> frame;
  		    frames.push_back(frame);
  	    }
        vd.fps=cap.get(CV_CAP_PROP_FPS);
        vd.x=static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
        vd.y=static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        vd.frames=static_cast<int>(cap.get(CV_CAP_PROP_FRAME_COUNT));
        vd.isColor=1;

        int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));
        // Transform from int to char via Bitwise operators
        char c[] = {(char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8), (char)((ex & 0XFF0000) >> 16), (char)((ex & 0XFF000000) >> 24), '\0'};
        for(int i=0;i<4;i++) vd.fourcc+=c[i];

    } catch( cv::Exception& e ){
        cerr << e.msg << endl;
        vd.fps=-1;
        return vd;
    }
	return vd;
}

int main(int, char**)
{
    vector<Mat> frames;
    string path="pathToMP4file";
    videoData vd = extract_frames(path,frames);

    if(vd.fps<0)
    {
        cout<<"sth is not yes\n";
        return 1;
    }
    VideoWriter out("videoOut/out.mp4",VideoWriter::fourcc(vd.fourcc[0],vd.fourcc[1],vd.fourcc[2],vd.fourcc[3]),vd.fps,Size(vd.x,vd.y),vd.isColor);
    VideoWriter kr("videoOut/lines.mp4",VideoWriter::fourcc(vd.fourcc[0],vd.fourcc[1],vd.fourcc[2],vd.fourcc[3]),vd.fps,Size(vd.x,vd.y),0);
    for(int i=0;i<vd.frames;i++)
    {
        Mat tmp;
        Mat blur;
        GaussianBlur(frames[i], blur, Size(13,13), 0, 0);
        Canny(blur, tmp, 200, 300, 5, false);
        kr.write(tmp);
        cvtColor(tmp, tmp, COLOR_GRAY2RGB);
        Mat output = frames[i]-tmp;
        out.write(output);
    }
    return 0;
}
