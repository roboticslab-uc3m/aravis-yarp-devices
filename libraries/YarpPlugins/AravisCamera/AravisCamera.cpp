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

void AravisCamera::printFeatureInfo(cameraFeature_id_t featureId, const FeatureInfo & info)
{
    if (bool available = false; !hasFeature(featureId, &available) || !available)
    {
        return;
    }

    yCInfo(ARV, "- %s (ID %d):", info.featureName, featureId);

    if (info.autoName)
    {
        if (FeatureMode mode; getMode(featureId, &mode))
        {
            std::string modeStr;

            switch (mode)
            {
                case MODE_AUTO: modeStr = "Auto"; break;
                case MODE_MANUAL: modeStr = "Manual"; break;
                default: modeStr = "Unknown";
            }

            yCInfo(ARV) << "  Mode:" << modeStr;
        }
    }

    if (info.enabledName)
    {
        if (bool isActive; getActive(featureId, &isActive))
        {
            yCInfo(ARV) << "  Status:" << (isActive ? "Enabled" : "Disabled");
        }
    }

    if (double value; getFeature(featureId, &value))
    {
        yCInfo(ARV) << "  Current value:" << value;
    }

    if (double min, max; getFeatureLimits(featureId, &min, &max))
    {
        yCInfo(ARV) << "  Range:" << min << "to" << max;
    }

    if (bool compatible; checkEnabled(featureId, &compatible))
    {
        yCInfo(ARV) << "  Compatible with current format:" << (compatible ? "Yes" : "No");
    }

    if (bool jauto; hasAuto(featureId, &jauto))
    {
        yCInfo(ARV) << "  auto:" << (jauto ? "Yes" : "No");
    }
}

void AravisCamera::listAvailableFeatures()
{
    yCInfo(ARV) << "Listing available features:";

    for (const auto & [id, info] : yarp_arv_int_feature_map)
    {
        printFeatureInfo(id, info);
    }

    for (const auto & [id, info] : yarp_arv_float_feature_map)
    {
        printFeatureInfo(id, info);
    }
}

bool AravisCamera::checkFeatureExistenceAndGetValue(const std::string & featureName, double & value)
{
    cameraFeature_id_t id = id_find(featureName);

    if (id == YARP_FEATURE_INVALID)
    {
        yCWarning(ARV) << "Feature not found:" << featureName;
        return false;
    }

    return getFeature(id, &value);
}

cameraFeature_id_t AravisCamera::id_find(const std::string & feature_name)
{
    for (const auto [id, info] : yarp_arv_int_feature_map)
    {
        if (std::string(info.featureName) == feature_name)
        {
            return id;
        }
    }

    for (const auto [id, info] : yarp_arv_float_feature_map)
    {
        if (std::string(info.featureName) == feature_name)
        {
            return id;
        }
    }

    return YARP_FEATURE_INVALID;
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
    if (!stream)
    {
        yCError(ARV) << "Stream was not initialized";
        return nullptr;
    }

    ArvBuffer * arvBuffer = nullptr;
    static constexpr int max_tries = 10;
    int tries = 0;
    bool success = false;

    while (!success && tries++ < max_tries)
    {
        static constexpr guint64 timeout = 200000; // 200 ms
        arvBuffer = arv_stream_timeout_pop_buffer(stream, timeout);

        if (arvBuffer && arv_buffer_get_status(arvBuffer) == ARV_BUFFER_STATUS_SUCCESS)
        {
            success = true;
        }
        else if (arvBuffer)
        {
            arv_stream_push_buffer(stream, arvBuffer);
        }
    }

    if (!success || !arvBuffer)
    {
        yCError(ARV) << "Timeout! Could not grab frame...";
        return nullptr;
    }

    size_t buffer_size;
    auto * framebuffer = const_cast<void *>(arv_buffer_get_data(arvBuffer, &buffer_size));
    arv_buffer_get_image_region(arvBuffer, &xoffset, &yoffset, &_width, &_height);
    frameID = arv_buffer_get_frame_id(arvBuffer);
    arv_stream_push_buffer(stream, arvBuffer);

    return framebuffer;
}
