#pragma once

#include <opencv2/opencv.hpp>

class MyCamera
{
    CvCapture * m;
public:
    /*
     CvCapture -> RaspiCamCvCapture
     cvCreateCameraCapture -> raspiCamCvCreateCameraCapture
     cvQueryFrame -> raspiCamCvQueryFrame
     cvReleaseCapture -> raspiCamCvReleaseCapture
     cvGetCaptureProperty -> raspiCamCvGetCaptureProperty
     */
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
        IplImage *frame = cvQueryFrame(m);
        image = cv::Mat(frame);
        cvReleaseImage(&frame);
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
