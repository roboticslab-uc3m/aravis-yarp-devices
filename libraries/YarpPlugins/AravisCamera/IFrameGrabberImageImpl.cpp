#include "AravisCamera.hpp"

#include <opencv2/imgproc.hpp>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

bool AravisCamera::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image)
{
    void * framebuffer = getFrameBuffer();

    if (!framebuffer)
    {
        yCError(ARV) << "Frame buffer is empty";
        return false;
    }

    cv::Mat sourceImg, rgbImg;

    switch (pixelFormat)
    {
    case ARV_PIXEL_FORMAT_BAYER_RG_8:
    case ARV_PIXEL_FORMAT_BAYER_RG_12P:
    case ARV_PIXEL_FORMAT_BAYER_RG_16:
        sourceImg = cv::Mat(_height, _width, CV_8UC1, framebuffer);
        cv::cvtColor(sourceImg, rgbImg, cv::COLOR_BayerRG2BGR);
        break;
    case ARV_PIXEL_FORMAT_YUV_422_PACKED:
    case ARV_PIXEL_FORMAT_YUV_411_PACKED:
        sourceImg = cv::Mat(_height, _width, CV_8UC2, framebuffer);
        cv::cvtColor(sourceImg, rgbImg, cv::COLOR_YUV2RGB_UYVY);
        break;
    case ARV_PIXEL_FORMAT_RGB_8_PLANAR:
        rgbImg = cv::Mat(_height, _width, CV_8UC3, framebuffer);
        break;
    case ARV_PIXEL_FORMAT_MONO_8:
    case ARV_PIXEL_FORMAT_MONO_12:
    case ARV_PIXEL_FORMAT_MONO_16:
        sourceImg = cv::Mat(_height, _width, CV_8UC1, framebuffer);
        cv::cvtColor(sourceImg, rgbImg, cv::COLOR_GRAY2RGB);
        break;
    default:
        yCError(ARV) << "Unsupported pixel format";
        return false;
    }

    image.resize(_width, _height);
    image.setExternal(rgbImg.data, _width, _height);

    return true;
}
