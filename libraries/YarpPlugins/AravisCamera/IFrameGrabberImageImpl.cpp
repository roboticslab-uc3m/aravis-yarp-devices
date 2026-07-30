#include "AravisCamera.hpp"

#include <opencv2/imgproc.hpp>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

bool AravisCamera::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image)
{
    if (!stream)
    {
        yCError(ARV) << "Stream was not initialized";
        return false;
    }

    ArvBuffer * arvBuffer = nullptr;
    int max_tries = 10;
    int tries = 0;
    bool success = false;

    while (!success && tries < max_tries)
    {
        arvBuffer = arv_stream_timeout_pop_buffer(stream, 200000);

        if (arvBuffer && arv_buffer_get_status(arvBuffer) == ARV_BUFFER_STATUS_SUCCESS)
        {
            success = true;
        }
        else if (arvBuffer)
        {
            arv_stream_push_buffer(stream, arvBuffer);
        }

        tries++;
    }

    if (!success || !arvBuffer)
    {
        yCError(ARV) << "Timeout! Could not grab frame...";
        return false;
    }

    size_t buffer_size;
    framebuffer = (void *)arv_buffer_get_data(arvBuffer, &buffer_size);
    arv_buffer_get_image_region(arvBuffer, &xoffset, &yoffset, &_width, &_height);
    frameID = arv_buffer_get_frame_id(arvBuffer);
    arv_stream_push_buffer(stream, arvBuffer);

    if (!framebuffer)
    {
        yCError(ARV) << "Framebuffer is empty";
        return false;
    }

    image.resize(_width, _height);

    cv::Mat rgbImg;

    if (pixelFormat == ARV_PIXEL_FORMAT_BAYER_RG_8 ||
        pixelFormat == ARV_PIXEL_FORMAT_BAYER_RG_12P ||
        pixelFormat == ARV_PIXEL_FORMAT_BAYER_RG_16)
    {
        yCInfo(ARV) << "Processing Bayer image...";
        cv::Mat bayerImg(_height, _width, CV_8UC1, framebuffer);
        cv::cvtColor(bayerImg, rgbImg, cv::COLOR_BayerRG2BGR);
    }
    else if (pixelFormat == ARV_PIXEL_FORMAT_YUV_422_PACKED ||
             pixelFormat == ARV_PIXEL_FORMAT_YUV_411_PACKED)
    {
        yCInfo(ARV) << "Processing YUV image...";
        cv::Mat ycbcrImg(_height, _width, CV_8UC2, framebuffer);
        cv::cvtColor(ycbcrImg, rgbImg, cv::COLOR_YUV2RGB_UYVY);
    }
    else if (pixelFormat == ARV_PIXEL_FORMAT_RGB_8_PLANAR)
    {
        yCInfo(ARV) << "Processing RGB8 image...";
        rgbImg = cv::Mat(_height, _width, CV_8UC3, framebuffer);
    }
    else if (pixelFormat == ARV_PIXEL_FORMAT_MONO_8 ||
             pixelFormat == ARV_PIXEL_FORMAT_MONO_12 ||
             pixelFormat == ARV_PIXEL_FORMAT_MONO_16)
    {
        yCInfo(ARV) << "Processing Mono image...";
        cv::Mat monoImg(_height, _width, CV_8UC1, framebuffer);
        cv::cvtColor(monoImg, rgbImg, cv::COLOR_GRAY2RGB);
    }
    else
    {
        yCError(ARV) << "Unsupported pixel format";
        return false;
    }

    image.setExternal(rgbImg.data, _width, _height);

    return true;
}
