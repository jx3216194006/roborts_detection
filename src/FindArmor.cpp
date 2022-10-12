#include"roborts_detection/FindArmor.h"

using namespace cv;
using namespace std;

void FindArmor::PreProcessImage(int LightBarColor,Mat img)
{

    Mat imgs[3];
    split(img, imgs);//三通道分离

    if(LightBarColor == 0)
    {
        img = imgs[0] - imgs[2];//b-r蓝色
    }
    else if(LightBarColor == 1)
    {
        img = imgs[2] - imgs[0];//r-b红色
    }
    else
    {
        if(Y0N2PRINT)
        {
            cout<<"LightBarColor is wrong"<<endl;
        }
        return;
    }
    Imshow("img", img);

    


}