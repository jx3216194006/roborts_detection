// #include<iostream>
// #include<opencv2/opencv.hpp>
// #include<ros/ros.h>

#include<thread>

#include"roborts_detection/Test.h"
#include"roborts_detection/DaHeng.h"
#include"roborts_detection/FindArmor.h"


using namespace std;
using namespace cv;





int main(int argc, char* argv[])
{

    Test t1;
    // t1.ShowPicture("./keqing.jpg");
    // t1.ros_hello();
    thread read_video(&Test::ReadVideo, &t1, "./3.mp4");

    read_video.detach();


    // DaHeng dh;
    
    // dh.AutoExpose();
    // if(Y0N2PRINT)
    // {
    //     cout<<"AutoExpose is ok"<<endl;        
    // }

    // thread get_pic(&DaHeng::GetPic, &dh);
    // get_pic.detach();


    FindArmor fa;

    while (1)
    {
        Mat pic;
        // t1.copyto(pic);
        t1.GetSrc(pic);

        fa.PreProcessImage(RED,pic);
    }
    

    // get_pic.join();
    // read_video.join();
    // pre_rocess_image.join();
    return 0;
}