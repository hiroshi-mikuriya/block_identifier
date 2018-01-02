#pragma once

#ifdef ENABLE_RASPBERRY_PI_CAMERA
#include "RaspiCamCV.h"
#define CvCapture RaspiCamCvCapture
#define cvCreateCameraCapture raspiCamCvCreateCameraCapture
#define cvQueryFrame raspiCamCvQueryFrame
#define cvReleaseCapture raspiCamCvReleaseCapture
#define cvGetCaptureProperty raspiCamCvGetCaptureProperty
#define cvSetCaptureProperty raspiCamCvSetCaptureProperty
#else
#include <opencv/highgui.h>
#endif

class MyCamera
{
    CvCapture * m;
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
        m = cvCreateCameraCapture(device);
        return isOpened();
    }
    void release()
    {
        cvReleaseCapture(&m);
    }
    MyCamera & operator >> (cv::Mat & image)
    {
        image = cv::Mat(cvQueryFrame(m));
        return *this;
    }
    bool set(int propId, double value)
    {
        return !!cvSetCaptureProperty(m, propId, value);
    }
    double get(int propId)
    {
        return cvGetCaptureProperty(m, propId);
    }
};
