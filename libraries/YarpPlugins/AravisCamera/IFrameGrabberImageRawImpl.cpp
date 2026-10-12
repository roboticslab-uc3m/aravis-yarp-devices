#include "AravisCamera.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> & image)
#else
bool AravisCamera::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> & image)
#endif
{
    void * framebuffer = getFrameBuffer();

    if (!framebuffer)
    {
        yCError(ARV) << "Frame buffer is empty";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    switch (pixelFormat)
    {
    case ARV_PIXEL_FORMAT_MONO_8:
    case ARV_PIXEL_FORMAT_BAYER_RG_8:
        break;
    default:
        yCError(ARV) << "Unsupported pixel format";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    image.resize(_width, _height);
    image.setExternal(framebuffer, _width, _height);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

int AravisCamera::height() const
{
    return _height;
}

int AravisCamera::width() const
{
    return _width;
}
