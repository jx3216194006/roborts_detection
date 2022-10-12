// #include<iostream>
// #include<opencv2/opencv.hpp>
// #include<mutex>

#include"roborts_detection/GxIAPI.h"
#include"roborts_detection/DxImageProc.h"

#include"roborts_detection/DaHeng.h"

using namespace std;
using namespace cv;


void DaHeng::GetPic()
{
	//cv::Mat src;
	GX_STATUS status = GX_STATUS_SUCCESS;//判断状态
	GX_DEV_HANDLE hDevice = NULL;//设备
	uint32_t nDeviceNum = 0;//设备数

	status = GXInitLib();//初始化库
	if (status != GX_STATUS_SUCCESS)
	{
		return;
	}

	status = GXUpdateDeviceList(&nDeviceNum, 1000);//枚举设备列表
	if ((status != GX_STATUS_SUCCESS) || (nDeviceNum <= 0))
	{
		return;
	}

	status = GXOpenDeviceByIndex(1, &hDevice);//打开第一个设备
	if (status == GX_STATUS_SUCCESS)
	{

		PGX_FRAME_BUFFER pFrameBuffer;//初始化帧缓冲区
		
		status = GXStreamOn(hDevice);//打开流传输
		if (status == GX_STATUS_SUCCESS)
		{
			//status = GXSetEnum(hDevice,GX_ENUM_BALANCE_WHITE_AUTO, GX_BALANCE_WHITE_AUTO_ONCE);
			
			//开始循环采集图像
			while(true)
			{
				//cout << "frame: " << frame << endl;
				
				status = GXDQBuf(hDevice, &pFrameBuffer, 200);//获取帧缓冲区
				if (status == GX_STATUS_SUCCESS)
				{
					if (pFrameBuffer->nStatus == GX_FRAME_STATUS_SUCCESS)
					{
						//图像获取成功
						//对图像进行处理...
	                
						void* rowdata = pFrameBuffer->pImgBuf;//获取图像数据
						char *pBGR24Buf = new char[pFrameBuffer->nWidth * pFrameBuffer->nHeight * 3];//创建图像缓冲区
						if (pBGR24Buf == NULL)
 						{
							return;
						}
						else
						{
							memset(pBGR24Buf,0,pFrameBuffer->nWidth * pFrameBuffer->nHeight * 3 * sizeof( char));//缓冲区初始化
						}
	                
						unique_lock<mutex> lock_src(mtx_src);//加锁

						src.create(pFrameBuffer->nHeight, pFrameBuffer->nWidth, CV_8UC3);//创建图像矩阵
	                
						VxInt32 DxStatus = DxRaw8toRGB24Ex(rowdata,pBGR24Buf,pFrameBuffer->nWidth,pFrameBuffer->nHeight,RAW2RGB_NEIGHBOUR,BAYERRG,false,DX_ORDER_BGR);//转换图像格式
						if (DxStatus != DX_OK)
						{
							if (pBGR24Buf != NULL)
							{
								delete []pBGR24Buf;
								pBGR24Buf = NULL;
							}
							return;
						}

						memcpy(src.data,pBGR24Buf,pFrameBuffer->nWidth*pFrameBuffer->nHeight*3);//将图像数据拷贝到矩阵中
						
						Imshow("src", src);
						lock_src.unlock();//解锁

						delete pBGR24Buf;//删除图像缓冲区
					}//if (pFrameBuffer->nStatus == GX_FRAME_STATUS_SUCCESS) is over
						
					status = GXQBuf(hDevice, pFrameBuffer);//释放帧缓冲区

				}//if (status == GX_STATUS_SUCCESS) is over
			}//while(true) is over
		}//{if (status == GX_STATUS_SUCCESS)} is over

		status = GXStreamOff(hDevice);//关闭流

	}//{if (status == GX_STATUS_SUCCESS)} is over

	status = GXCloseDevice(hDevice);//关闭设备
	status = GXCloseLib();//关闭库
	return;
}


int DaHeng::AutoExpose(double time_min)
{	
	//初始化曝光时间
    //time_min最小曝光时间(环境昏暗时对五米外的装甲板灯条进行测试)
	double time_mid,//中间曝光时间
			time_max;//最大曝光时间
	int frame = 0;//帧数

	//初始化
	GX_STATUS status = GX_STATUS_SUCCESS;//判断状态
	GX_DEV_HANDLE hDevice = NULL;//设备句柄
	uint32_t nDeviceNum = 0;//设备数

	//初始化库
	status = GXInitLib();
	if (status != GX_STATUS_SUCCESS)
	{
		return 0;
	}

	//枚举设备列表
	status = GXUpdateDeviceList(&nDeviceNum, 1000);
	if ((status != GX_STATUS_SUCCESS) || (nDeviceNum <= 0))
	{
		return 0;
	}
	
	status = GXOpenDeviceByIndex(1, &hDevice);//打开第一个设备
	if (status == GX_STATUS_SUCCESS)
	{
		
		PGX_FRAME_BUFFER pFrameBuffer;//初始化帧缓冲区
		
		status = GXStreamOn(hDevice);//判断是否开启流

		if (status == GX_STATUS_SUCCESS)
		{	
			int64_t ExState = GX_EXPOSURE_AUTO_ONCE;//判断曝光状态

			status = GXSetEnum(hDevice, GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_ONCE);//设置自动曝光一次
			while(ExState == GX_EXPOSURE_AUTO_ONCE)
			{
				GXGetEnum(hDevice, GX_ENUM_EXPOSURE_AUTO, &ExState);//获取曝光状态
			}//等待本次自动曝光结束

			// do
			// {
			// 	GXGetFloat(hDevice, GX_FLOAT_EXPOSURE_TIME, &time_max);//获取曝光时间
			// 	cout << "time_max: " << time_max << " time_min: " << time_min << endl;
			// } while (time_max < 100);//等待曝光时间获取正确
			
			GXGetFloat(hDevice, GX_FLOAT_EXPOSURE_TIME, &time_max);//获取曝光时间
			
			time_mid = (time_max + time_min) / 2;//time_mid赋初值

			ExTime extime;
			extime.time_min = time_min;
			extime.time_mid = time_mid;
			extime.time_max = time_max;

			if(Y0N2PRINT)
			{
				cout << "time_max: " << time_max << " time_min: " << time_min << endl;
			}

			status = GXSetEnum(hDevice, GX_ENUM_BALANCE_WHITE_AUTO, GX_BALANCE_WHITE_AUTO_ONCE);//自动白平衡一次
			int64_t BwState = GX_BALANCE_WHITE_AUTO_ONCE;//判断白平衡状态
			
			while(BwState == GX_BALANCE_WHITE_AUTO_ONCE)
			{
				GXGetEnum(hDevice, GX_ENUM_BALANCE_WHITE_AUTO, &BwState);//获取白平衡状态
			}//等待本次自动白平衡结束


			while(true)
			{
				frame++;
				status = GXDQBuf(hDevice, &pFrameBuffer, 200);//获取帧缓冲区?会获取失败
				if (status == GX_STATUS_SUCCESS)
				{
					if (pFrameBuffer->nStatus/*获取图像状态*/ == GX_FRAME_STATUS_SUCCESS)
					{
						//图像获取成功

						//对图像进行处理...
						void* rowdata = pFrameBuffer->pImgBuf;//图像数据指针（启用帧信息后，pImgBuf包含图像数据和帧信息数据）
						char *pBGR24Buf = new char[pFrameBuffer->nWidth * pFrameBuffer->nHeight * 3];//将图像数据存储到缓冲区中
						if (pBGR24Buf == NULL)
 						{
							return -1;
						}
						else
						{
							//缓冲区初始化
							memset(pBGR24Buf,0,pFrameBuffer->nWidth * pFrameBuffer->nHeight * 3 * sizeof(char));//？？
						}
	                
						src.create(pFrameBuffer->nHeight, pFrameBuffer->nWidth, CV_8UC3);//创建图像矩阵
	                
						VxInt32 DxStatus = DxRaw8toRGB24Ex(rowdata,pBGR24Buf,pFrameBuffer->nWidth,pFrameBuffer->nHeight,RAW2RGB_NEIGHBOUR,BAYERRG,false,DX_ORDER_BGR);//将图像数据转换为BGR24格式
						if (DxStatus != DX_OK)
						{
							if (pBGR24Buf != NULL)
							{
								delete []pBGR24Buf;//释放缓冲区
								pBGR24Buf = NULL;//缓冲区置空
							}
							return -1;
						}

						memcpy(src.data,pBGR24Buf,pFrameBuffer->nWidth*pFrameBuffer->nHeight*3);//将图像数据复制到矩阵中
						Imshow("src", src);

						if (frame >= 10)//防止前几帧曝光不正常
						{
							int exreturn=0;//曝光时间调整返回值
							exreturn=DaHeng::Expose(extime);//调用曝光时间调整函数
							if( exreturn == 2 )
							{
								if(Y0N2PRINT)
								{
									cout <<"end:"<< "time_max= " << extime.time_max <<"   time_mide="<<extime.time_mid<< "    time_min" << extime.time_min << endl;
								}
								goto part2;
							}
							else if( exreturn == 1 )
							{
								if(Y0N2PRINT)
								{
									cout << "time_max= " << extime.time_max <<"   time_mide="<<extime.time_mid<< "    time_min" << extime.time_min << endl;
								}
								GXSetFloat(hDevice, GX_FLOAT_EXPOSURE_TIME, extime.time_mid);//设置曝光时间
								goto part1;
							}
						}//if (time_target - 5 < time_current && time_current < time_target + 5 && ExAu == 0 && BWAu == 0) is over
						
						delete pBGR24Buf;//
					}//if (pFrameBuffer->nStatus/*获取图像状态*/ == GX_FRAME_STATUS_SUCCESS) is over
					// else
					// {
					// 	cout << "1Get Image Failed" << endl;
					// }

part1:				status = GXQBuf(hDevice, pFrameBuffer);//调用 GXQBuf 将图像 buf 放回库中继续采图
				
				}//if (帧缓冲区 == GX_STATUS_SUCCESS) is over
				// else
				// {
				// 	cout << "2Get Image Failed" << endl;
				// }

			}//while(true) is over

		}//if (开启流 == GX_STATUS_SUCCESS) is over
		// else
		// {
		// 	cout << "3Open Stream Failed" << endl;
		// }

part2:	status = GXStreamOff(hDevice);//停止采集

	}//if (打开设备 == GX_STATUS_SUCCESS) is over

	status = GXCloseDevice(hDevice);//关闭设备

	status = GXCloseLib();//关闭库
	
	// destroyWindow("src");
	return 0;
}


int DaHeng::Expose(ExTime &extime)
{
	int CountWhite = 0;//过亮像素点个数
	float row_a[3];//像素点的bgr值

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			//遍历图像，计算白色像素个数和白色像素的总和
			
			for (int num = 0; num < 3; num++)
			{
				//获取三通道的像素值
				row_a[num] = src.at<Vec3b>(i, j)[num];
			}
			
			Mat row = Mat(Size(3, 1), CV_32FC1, row_a);//将b，g，r三个参数放入矩阵

			//row: (b, g, r); m1: (1, 1, 0, -1, 0, 1, 0, -1, -1)
			//ans_mat: (b-g, b-r, g-r)

			Mat ans_mat = Mat::zeros(Size(3, 1), CV_32FC1);//输出矩阵
			ans_mat = row * m1;//矩阵乘法


			//计算ans_mat的两个较小元素
			float elm[2];//存放ans_mat的两个较小元素			
			float max = abs(ans_mat.at<float>(0));//初始化max为ans_mat的第一个元素
			int count=0;//计数器

			for (int num = 1; num < 3; num++)
			{
				//如果b-g的绝对值小于b-r or g-r，则将b-g存入elm的0号位
				if (max < abs(ans_mat.at<float>(num)))
				{
					
					elm[count] = max;//较小的值存入elm
					max = abs(ans_mat.at<float>(num));//较大的值存入max
					count++;
				}
				else
				{
					elm[count] = abs(ans_mat.at<float>(num));//较小的值存入elm
					count++;
				}
				if (count == 2)
				{
					break;
				}
			}

			//计算row_a的两个较小元素
			float row_elm[2];//存放row_a的两个较小元素
			float row_max = row_a[0];//初始化max为row_a的第一个元素
			count = 0;//计数器归零

			for (int num = 1; num < 3; num++)
			{
				if (row_max < row_a[num])
				{
					row_elm[count] = row_max;//较小的值存入row_elm
					row_max = row_a[num];//较大的值存入row_max
					count++;
				}
				else
				{
					row_elm[count] = row_a[num];//较小的值存入row_elm
					count++;
				}
				if (count == 2)
				{
					break;
				}
			}

			//cout << "[" << i << "," << j << "]: " << row_elm[0] << "  " << row_elm[1] << endl;

			
			if (row_elm[0] >= 200 && row_elm[1] >= 200)
			{
				CountWhite += 1;

				if (extime.time_max <= extime.time_min + 1)
				{
					return 2;
				}
				//二分法减少曝光时间
				extime.time_max = extime.time_mid;
				extime.time_mid = (extime.time_max + extime.time_min) / 2;

				return 1;
			}

			if (row_elm[0] >= 70 && row_elm[1] >= 70 && elm[0] + elm[1] < 30)
			{
				CountWhite += 1;

				if (extime.time_max <= extime.time_min + 1)
				{
					return 2;
				}
				//二分法减少曝光时间
				extime.time_max = extime.time_mid;
				extime.time_mid = (extime.time_max + extime.time_min) / 2;

				return 1;

			}
		}//for (int j = 0; j < src.cols; j++) is over
	}//if (src.rows > 0) is over

	if (extime.time_max <= extime.time_min + 1)
	{

		return 2;
	}

	if( CountWhite == 0 )
	{
		//二分法增加曝光时间
		extime.time_min = extime.time_mid;
		extime.time_mid = (extime.time_max + extime.time_min) / 2;

	}
	return 0;
}


    