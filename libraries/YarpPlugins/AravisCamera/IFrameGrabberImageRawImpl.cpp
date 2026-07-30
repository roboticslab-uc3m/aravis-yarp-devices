#include "AravisCamera.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

bool AravisCamera::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> & image)
{
    //-- Right now it is implemented as polling (grab + retrieve image)
    //-- I think it could be also implemented with callbacks with ArvStreamCallback

    framebuffer = nullptr;

    if (stream == nullptr)
    {
        yCError(ARV) << "Stream was not initialized";
        return false;
    }

    ArvBuffer * arvBuffer = nullptr;
    int max_tries = 10;
    int tries = 0;
    int success = false;

    while (!success && tries < max_tries)
    {
        arvBuffer = arv_stream_timeout_pop_buffer(stream, 200000);

        if (arvBuffer != nullptr && arv_buffer_get_status(arvBuffer) != ARV_BUFFER_STATUS_SUCCESS)
        {
            arv_stream_push_buffer(stream, arvBuffer);
        }
        else
        {
            success = true;
        }

        tries++;
    }

    if (arvBuffer != nullptr && success)
    {
        size_t buffer_size;
        framebuffer = (void *)arv_buffer_get_data(arvBuffer, &buffer_size);
        arv_buffer_get_image_region(arvBuffer, &xoffset, &yoffset, &_width, &_height);
        frameID = arv_buffer_get_frame_id(arvBuffer);
        arv_stream_push_buffer(stream, arvBuffer);
    }
    else
    {
        yCError(ARV) << "Timeout! Could not grab frame...";
        return false;
    }

    if (framebuffer != nullptr)
    {
        if (pixelFormat != ARV_PIXEL_FORMAT_MONO_8 && pixelFormat != ARV_PIXEL_FORMAT_BAYER_RG_8)
        {
            yCError(ARV) << "Unsupported pixel format";
        }

        image.resize(_width, _height);
        image.setExternal(framebuffer, _width, _height);
    }
    else
    {
        yCError(ARV) << "Framebuffer is empty";
        return false;
    }

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
