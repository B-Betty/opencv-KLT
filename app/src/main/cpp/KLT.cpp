
//
// Created by Betty on 2017/1/17.
//
#include<jni.h>
#include <opencv2/video.hpp>
#include <android/log.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include "opencv/cv.h"
#include <iostream>

using namespace cv;
using namespace std;
#define LOG_TAG "KLT/klt"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__))
extern "C"
{
JNIEXPORT void JNICALL
Java_com_betty_klt_MainActivity_detection(JNIEnv *env, jobject instance, jlong matAddInRGBA) {
//    bool addNewPoints();
//    bool acceptTrackedPoint(int i);
//  描述：声明全局变量
    string window_name = "optical flow tracking";
    Mat gray;   // 当前图片
    Mat gray_prev;  // 预测图片
    vector<Point2f> points[2];  // point0为特征点的原来位置，point1为特征点的新位置
    vector<Point2f> initial;    // 初始化跟踪点的位置
    vector<Point2f> features;   // 检测的特征
    int maxCount = 500; // 检测的最大特征数
    double qLevel = 0.01;   // 特征检测的等级
    double minDist = 10.0;  // 两特征点之间的最小距离
    vector<uchar> status;   // 跟踪特征的状态，特征的流发现为1，否则为0
    vector<float> err;
    Mat *frame = (Mat*)matAddInRGBA;
    Mat resut;
    LOGI("enter opencv success, receiver address from java");
    cvtColor(*frame,gray,COLOR_RGBA2GRAY);
    frame->copyTo(resut);
    //添加特征点
    if (points[0].size() <= 10)
    {
        goodFeaturesToTrack(gray,features,maxCount,qLevel,minDist);//查询特征点
        points[0].insert(points[0].end(),features.begin(),features.end());
        initial.insert(initial.end(),features.begin(),features.end());
    }
    if (gray_prev.empty())
    {
        gray.copyTo(gray_prev);
    }
    //L-K光流法运动跟踪
    calcOpticalFlowPyrLK(gray_prev, gray,points[0], points[1],status, err);
    //去掉一些不好的特征点
    int k = 0;
    for (size_t i = 0; i < points[1].size(); ++i) {
        if (status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2))
        {
            initial[k] = initial[i];
            points[1][k++] = points[1][i];
        }
    }
    points[1].resize(k);
    initial.resize(k);
    //显示特征点运动轨迹
    for (size_t i = 0; i < points[1].size(); ++i)
    {
        line(resut,initial[i],points[i][i],Scalar(0,0,255));
        circle(resut,points[1][i],3,Scalar(0,255,0));
    }
    //把当前跟踪结果作为下一帧参考
    swap(points[1],points[0]);
    swap(gray_prev,gray);
}
}

