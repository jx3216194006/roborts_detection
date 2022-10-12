#ifndef FINDARMOR_H
#define FINDARMOR_H


#include<mutex>
#include "roborts_detection/Detection.h"

using namespace cv;
using namespace std;

class FindArmor:public Detection
{
public:
    FindArmor():Detection()
    {
        // std::cout<<"FindArmor class is ok"<<std::endl;
    }

    void PreProcessImage(int LightBarColor,Mat img);



};



#endif