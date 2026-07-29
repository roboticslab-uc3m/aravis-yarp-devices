#include "AravisGigE.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

bool AravisGigE::getFeatureLimits(int feature, double * min, double * max)
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

bool AravisGigE::checkEnabled(cameraFeature_id_t feature, bool * compatible)
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

const FeatureInfo * AravisGigE::getFeatureInfo(cameraFeature_id_t feature)
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

void AravisGigE::printFeatureInfo(cameraFeature_id_t featureId, const FeatureInfo & info)
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

            yCInfo(ARV) << "  Mode: " << modeStr;
        }
    }

    if (info.enabledName)
    {
        if (bool isActive; getActive(featureId, &isActive))
        {
            yCInfo(ARV) << "  Status: " << (isActive ? "Enabled" : "Disabled");
        }
    }

    if (double value; getFeature(featureId, &value))
    {
        yCInfo(ARV) << "  Current value: " << value;
    }

    if (double min, max; getFeatureLimits(featureId, &min, &max))
    {
        yCInfo(ARV) << "  Range: " << min << " to " << max;
    }

    if (bool compatible; checkEnabled(featureId, &compatible))
    {
        yCInfo(ARV) << "  Compatible with current format: " << (compatible ? "Yes" : "No");
    }

    if (bool jauto; hasAuto(featureId, &jauto))
    {
        yCInfo(ARV) << "  auto: " << (jauto ? "Yes" : "No");
    }
}

void AravisGigE::listAvailableFeatures()
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

bool AravisGigE::checkFeatureExistenceAndGetValue(const std::string & featureName, double & value)
{
    cameraFeature_id_t id = id_find(featureName);

    if (id == YARP_FEATURE_INVALID)
    {
        yCWarning(ARV) << "Feature not found: " << featureName;
        return false;
    }

    return getFeature(id, &value);
}

cameraFeature_id_t AravisGigE::id_find(const std::string & feature_name)
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
