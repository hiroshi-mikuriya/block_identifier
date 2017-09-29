#include <opencv2/opencv.hpp>

#ifdef _DEBUG
#define LIB_EXTENSION "d.lib"
#else
#define LIB_EXTENSION ".lib"
#endif
#define OPENCV_VERSION CVAUX_STR(CV_VERSION_EPOCH) CVAUX_STR(CV_VERSION_MAJOR) CVAUX_STR(CV_VERSION_MINOR)
#pragma comment( lib, "opencv_core" OPENCV_VERSION  LIB_EXTENSION )
#pragma comment( lib, "opencv_imgproc" OPENCV_VERSION  LIB_EXTENSION )
#pragma comment( lib, "opencv_highgui" OPENCV_VERSION  LIB_EXTENSION )
#pragma comment( lib, "opencv_video" OPENCV_VERSION  LIB_EXTENSION )
#pragma comment( lib, "IlmImf" LIB_EXTENSION )
#pragma comment( lib, "libjasper" LIB_EXTENSION )
#pragma comment( lib, "libjpeg" LIB_EXTENSION )
#pragma comment( lib, "libpng" LIB_EXTENSION )
#pragma comment( lib, "libtiff" LIB_EXTENSION )
#pragma comment( lib, "zlib" LIB_EXTENSION)
#pragma comment( lib, "comctl32.lib" )
#pragma comment( lib, "User32.lib")
#pragma comment( lib, "Gdi32.lib")
#pragma comment( lib, "Advapi32.lib")
#pragma comment( lib, "vfw32.lib")

