#include "AravisCamera.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

bool AravisCamera::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> & image)
{
    //-- Right now it is implemented as polling (grab + retrieve image)
    //-- I think it could be also implemented with callbacks with ArvStreamCallback

    void * framebuffer = getFrameBuffer();

    if (!framebuffer)
    {
        yCError(ARV) << "Frame buffer is empty";
        return false;
    }

    switch (pixelFormat)
    {
    case ARV_PIXEL_FORMAT_MONO_8:
    case ARV_PIXEL_FORMAT_BAYER_RG_8:
        break;
    default:
        yCError(ARV) << "Unsupported pixel format";
        return false;
    }

    image.resize(_width, _height);
    image.setExternal(framebuffer, _width, _height);

    return true;
}

int AravisCamera::height() const
{
    return _height;
}

int AravisCamera::width() const
{
    return _width;
}
