#pragma once

#ifdef ENABLE_RASPBERRY_PI_CAMERA
#include "RaspiCamCV.h"
class MyCamera
{
    RaspiCamCvCapture * m;
public:
    MyCamera()
    {
    }
    MyCamera(int device)
    {
        open(device);
    }
    ~MyCamera()
    {
        release();
    }
    bool isOpened()const
    {
        return !!m;
    }
    bool open(int device)
    {
        m = raspiCamCvCreateCameraCapture(device);
        return isOpened();
    }
    void release()
    {
        raspiCamCvReleaseCapture(&m);
    }
    MyCamera & operator >> (cv::Mat & image)
    {
        image = cv::Mat(raspiCamCvQueryFrame(m));
        return *this;
    }
    bool set(int propId, double value)
    {
        return !!raspiCamCvSetCaptureProperty(m, propId, value);
    }
    double get(int propId)
    {
        return raspiCamCvGetCaptureProperty(m, propId);
    }
};
#else
#include <opencv/highgui.h>
typedef cv::VideoCapture MyCamera;
#endif
