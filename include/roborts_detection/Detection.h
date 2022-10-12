#ifndef DETECTION_H
#define DETECTION_H

#include<iostream>
#include<opencv2/opencv.hpp>

#define Y0N2SHOW 1
#define Y0N2PRINT 1

#define RED 1
#define BLUE 0

class Detection
{
public:
    Detection()
    {
        cv::Mat src=cv::Mat::zeros(600,800,CV_8UC3);
        // std::cout<<"Detection class is ok"<<std::endl;
    }
    
    void GetSrc(cv::Mat &pic)
    {
        if(src.empty())
        {
            pic=cv::Mat::zeros(600,800,CV_8UC3);
            if(Y0N2PRINT)
            {
                std::cout<<"src is empty"<<"/t detection"<<std::endl;
            }
            return;
        }
        src.copyTo(pic);
    }//拷贝src

protected:
    cv::Mat src;

    void Imshow(std::string name,cv::Mat& src)
    {   
        if(Y0N2SHOW)
        {
            cv::namedWindow(name, cv::WINDOW_NORMAL);
            cv::imshow(name,src);
            cv::waitKey(1);
            return;
        }
        return;
    }//show a picture

    void showEllipse(cv::RotatedRect* lights,int num)
    {
        if(!Y0N2SHOW) return;
        cv::Mat drawPic(600, 800, CV_8UC3, cv::Scalar(0));
        for (int t = 0; t < num; t++)
        {
            cv::ellipse(drawPic, lights[t], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);//画椭圆
        }
    }//

    void showLight(cv::RotatedRect* lights , int num)
    {
        if(!Y0N2SHOW) return;
        for (int n = 0; n < num; n++)
		{
            cv::Mat drawPic(600, 800, CV_8UC3, cv::Scalar(0));
			cv::Point2f points[4];//
			lights[n].points(points);//
			cv::Point2f cpt = lights[n].center;//

			for (size_t i = 0; i < 4; i++)
			{
				cv::line(drawPic, points[i], points[(i + 1) % 4], cv::Scalar(0, 255, 0), 1, 8, 0);//
			}
			//cout << n << ": " << lights[n].angle << endl;
			//ellipse(drawPic, lights[n], Scalar(0, 0, 255));

			std::string text = std::to_string(n);//
			cv::putText(drawPic, text, lights[n].center, 1, 2, cv::Scalar(0, 0, 255));//
		}
    }//

    void showArmor(std::vector<std::vector<cv::Point2f>> armors)
    {
        if(!Y0N2SHOW) return;
        cv::Mat drawPic(600, 800, CV_8UC3, cv::Scalar(0));
		for (int i = 0; i < armors.size(); i++)
		{
			for (int j = 0; j < 4; j++)
			{
				line(drawPic, armors.at(i).at(j), armors.at(i).at((j + 1) % 4), cv::Scalar(13, 178, 208));//
				std::string text = std::to_string(j);//���
				cv::putText(drawPic, text, armors.at(i).at(j), 1, 2, cv::Scalar(0, 0, 255));//
			}
		}//
    }//

};

#endif