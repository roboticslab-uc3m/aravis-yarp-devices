#include "AravisCamera.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

bool AravisCamera::getFeatureLimits(int feature, double * min, double * max)
{
    auto f = static_cast<cameraFeature_id_t>(feature);
    const FeatureInfo * info = getFeatureInfo(f);

    if (!info || !info->featureName)
    {
        yCError(ARV) << "Feature" << feature << "not supported";
        return false;
    }

    ArvDevice * device = arv_camera_get_device(camera);

    if (!device)
    {
        yCError(ARV) << "Camera device not available";
        return false;
    }

    if (!arv_device_get_feature(device, info->featureName))
    {
        yCWarning(ARV) << "Feature" << info->featureName << "not available on this device";
        return false;
    }

    GError * error = nullptr;

    // Try integer bounds first
    gint64 min_int = 0;
    gint64 max_int = 0;
    arv_device_get_integer_feature_bounds(device, info->featureName, &min_int, &max_int, &error);

    if (!error && min_int != max_int)
    {
        *min = static_cast<double>(min_int);
        *max = static_cast<double>(max_int);
        return true;
    }

    if (error)
    {
        g_error_free(error);
        error = nullptr;
    }

    // Try float bounds if integer failed
    gdouble min_float = 0.0;
    gdouble max_float = 0.0;
    arv_device_get_float_feature_bounds(device, info->featureName, &min_float, &max_float, &error);

    if (!error && min_float != max_float)
    {
        *min = static_cast<double>(min_float);
        *max = static_cast<double>(max_float);
        return true;
    }

    if (error)
    {
        ArvDevice * device = arv_camera_get_device(camera);
        yCError(ARV, "Error getting bounds for %s: %s", info->featureName, error->message);
        g_error_free(error);
    }

    yCWarning(ARV) << "Could not retrieve valid range for feature" << info->featureName;
    return false;
}

bool AravisCamera::checkEnabled(cameraFeature_id_t feature, bool * compatible)
{
    if (!compatible)
    {
        return false;
    }

    const FeatureInfo * info = getFeatureInfo(feature);

    if (!info)
    {
        *compatible = false;
        return true;
    }

    ArvDevice * device = arv_camera_get_device(camera);
    GError * error = nullptr;

    if (arv_camera_is_feature_available(camera, info->enabledName, &error) == 0 && arv_device_get_feature(device, info->enabledName) != 0)
    {
        *compatible = false;
        return true;
    }

    if (error)
    {
        g_error_free(error);
        *compatible = false;
        return false;
    }

    *compatible = true;
    return true;
}

const FeatureInfo * AravisCamera::getFeatureInfo(cameraFeature_id_t feature)
{
    if (yarp_arv_int_feature_map.count(feature))
    {
        return &yarp_arv_int_feature_map.at(feature);
    }
    else if (yarp_arv_float_feature_map.count(feature))
    {
        return &yarp_arv_float_feature_map.at(feature);
    }

    return nullptr;
}

bool AravisCamera::getAvailablePixelFormats(std::set<std::string> & availablePixelFormats)
{
    guint n_pixel_formats;

    auto ** availableFormatsStrings = arv_camera_dup_available_pixel_formats_as_strings(camera, &n_pixel_formats, nullptr);
    auto ** availableFormatsNames = arv_camera_dup_available_pixel_formats_as_display_names(camera, &n_pixel_formats, nullptr);

    for (int i = 0; i < n_pixel_formats; i++)
    {
        availablePixelFormats.emplace(availableFormatsStrings[i]);
    }

    g_free(availableFormatsStrings);
    g_free(availableFormatsNames);

    return true;
}

void * AravisCamera::getFrameBuffer()
{
    ArvBuffer * arvBuffer = nullptr;
    static constexpr int max_tries = 10;
    int tries = 0;
    bool success = false;

    while (!success && tries++ < max_tries)
    {
        static constexpr guint64 timeout = 200000; // 200 ms

        if (arvBuffer = arv_stream_timeout_pop_buffer(stream, timeout); arvBuffer)
        {
            if (arv_buffer_get_status(arvBuffer) == ARV_BUFFER_STATUS_SUCCESS)
            {
                success = true;
            }
            else
            {
                arv_stream_push_buffer(stream, arvBuffer);
            }
        }
    }

    if (!success || !arvBuffer)
    {
        yCError(ARV) << "Timeout! Could not grab frame...";
        return nullptr;
    }

    auto * framebuffer = const_cast<void *>(arv_buffer_get_data(arvBuffer, nullptr));

    arv_buffer_get_image_region(arvBuffer, nullptr, nullptr, &_width, &_height);
    arv_stream_push_buffer(stream, arvBuffer);

    return framebuffer;
}
