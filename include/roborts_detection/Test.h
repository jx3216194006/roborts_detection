#ifndef TEST_H
#define TEST_H

// #include<iostream>
#include<mutex>

#include"roborts_detection/Detection.h"

class Test:public Detection
{
public:
    Test()
    {
        // std::cout<<"Test class is ok"<<std::endl;
    }

    void ShowPicture(std::string location);//show a picture
    void RosHello();//ros hello world
    int ReadVideo(std::string name);//read a video

    
private:
    std::mutex mtx_img;

};


#endif