#include "AravisCamera.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::getFeatureLimits(yarp::dev::cameraFeature_id_t feature, double & min, double & max)
#else
bool AravisCamera::getFeatureLimits(int feature, double * min, double * max)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info || !info->featureName)
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "not supported";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    ArvDevice * device = arv_camera_get_device(camera);

    if (!device)
    {
        yCError(ARV) << "Camera device not available";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    if (!arv_device_get_feature(device, info->featureName))
    {
        yCWarning(ARV) << "Feature" << info->featureName << "not available on this device";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    GError * error = nullptr;

    // Try integer bounds first
    gint64 min_int = 0;
    gint64 max_int = 0;
    arv_device_get_integer_feature_bounds(device, info->featureName, &min_int, &max_int, &error);

    if (!error && min_int != max_int)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        min = static_cast<double>(min_int);
        max = static_cast<double>(max_int);
        return yarp::dev::ReturnValue_ok;
#else
        *min = static_cast<double>(min_int);
        *max = static_cast<double>(max_int);
        return true;
#endif
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
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        min = static_cast<double>(min_float);
        max = static_cast<double>(max_float);
        return yarp::dev::ReturnValue_ok;
#else
        *min = static_cast<double>(min_float);
        *max = static_cast<double>(max_float);
        return true;
#endif
    }

    if (error)
    {
        ArvDevice * device = arv_camera_get_device(camera);
        yCError(ARV, "Error getting bounds for %s: %s", info->featureName, error->message);
        g_error_free(error);
    }

    yCWarning(ARV) << "Could not retrieve valid range for feature" << info->featureName;
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_method_failed;
#else
    return false;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::checkEnabled(yarp::dev::cameraFeature_id_t feature, bool & compatible)
#else
bool AravisCamera::checkEnabled(int feature, bool * compatible)
#endif
{
#if YARP_VERSION_COMPARE(<, 4, 0, 0)
    if (!compatible)
    {
        return false;
    }
#endif

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        compatible = false;
        return yarp::dev::ReturnValue_ok;
#else
        *compatible = false;
        return true;
#endif
    }

    ArvDevice * device = arv_camera_get_device(camera);
    GError * error = nullptr;

    if (arv_camera_is_feature_available(camera, info->enabledName, &error) == 0 && arv_device_get_feature(device, info->enabledName) != 0)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        compatible = false;
        return yarp::dev::ReturnValue_ok;
#else
        *compatible = false;
        return true;
#endif
    }

    if (error)
    {
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        compatible = false;
        return yarp::dev::ReturnValue_error_method_failed;
#else
        *compatible = false;
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        compatible = true;
        return yarp::dev::ReturnValue_ok;
#else
        *compatible = true;
        return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
const FeatureInfo * AravisCamera::getFeatureInfo(yarp::dev::cameraFeature_id_t feature)
#else
const FeatureInfo * AravisCamera::getFeatureInfo(cameraFeature_id_t feature)
#endif
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
