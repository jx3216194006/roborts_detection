#ifndef DAHENG_H
#define DAHENG_H

#include<mutex>

#include"roborts_detection/Detection.h"




class DaHeng:public Detection
{
public:
    DaHeng():Detection()
    {
        // std::cout<<"DaHeng class is ok"<<std::endl;

    }


    void GetPic();//相机开始采集图像

    int AutoExpose(double time_min = 100);//自动曝光



private:

    std::mutex mtx_src;


    //三个曝光时间（方便传参）
    typedef struct ExPoseTime
    {
        double time_max=0;
        double time_mid=0;
        double time_min=0;
    }ExTime;



    //Expose中所需变量
    float a1[9] = { 1, 1, 0, 
					-1, 0, 1, 
					0, -1, -1 };//横向滤波矩阵
	cv::Mat m1 = cv::Mat(cv::Size(3, 3), CV_32FC1, a1);//横向滤波矩阵
	//cout << src.rows << endl << src.cols << endl;

    int Expose(ExTime &extime);//曝光核心算法
};


#endif
