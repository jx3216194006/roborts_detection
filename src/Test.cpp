// #include<opencv2/opencv.hpp>
#include<ros/ros.h>

#include"roborts_detection/Test.h"

using namespace std;
using namespace cv;


int Test::ReadVideo(string name)
{
    VideoCapture video(name);
    if (video.isOpened())
    {
        if(Y0N2PRINT)
        {
            cout << "视频中图像的宽度" << video.get(CAP_PROP_FRAME_WIDTH) << endl;
            cout << "视频中图像的高度" << video.get(CAP_PROP_FRAME_HEIGHT) << endl;
            cout << "视频的帧率" << video.get(CAP_PROP_FPS) << endl;
            cout << "视频的总帧数" << video.get(CAP_PROP_FRAME_COUNT) << endl;
        }
    }
    else
    {
        if(Y0N2PRINT)
        {
            cout << "请确认视频是否导入成功" << endl;
        }
        return 1;
    }
    while (1)
    {
        // unique_lock<mutex> lock_img(mtx_img);
        video >> src;
        Imshow("src", src);
        waitKey(1000 / video.get(CAP_PROP_FPS));
        // lock_img.unlock();
    }
    return 0;
}


void Test::ShowPicture(string location)
{
    Mat src = imread(location);
    Mat dst;
    if(src.empty())
    {
        cout << "can not load this image" << endl;
        return;
    }

    cvtColor(src, dst, COLOR_BGR2GRAY);

    namedWindow("src", WINDOW_NORMAL);
    namedWindow("dst", WINDOW_NORMAL);

    imshow("src", src);
    imshow("dst", dst);

    waitKey(0);
}

void Test::RosHello()
{
    ROS_INFO("Hello World!");
}
