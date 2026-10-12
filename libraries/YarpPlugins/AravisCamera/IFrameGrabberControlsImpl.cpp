#include "AravisCamera.hpp"

#include <iostream>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::getCameraDescription(yarp::dev::CameraDescriptor & camera)
#else
bool AravisCamera::getCameraDescription(CameraDescriptor * camera)
#endif
{
    if (arv_camera_is_uv_device(this->camera))
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        camera.busType = yarp::dev::BusType::BUS_USB;
#else
        camera->busType = BUS_USB;
#endif
    }
    else
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        camera.busType = yarp::dev::BusType::BUS_UNKNOWN;
#else
        camera->busType = BUS_UNKNOWN;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    camera.deviceDescription = std::string(arv_camera_get_device_id(this->camera, nullptr)) + ": " +
                               arv_camera_get_model_name(this->camera, nullptr);
    return yarp::dev::ReturnValue_ok;
#else
    camera->deviceDescription = std::string(arv_camera_get_device_id(this->camera, nullptr)) + ": " +
                                arv_camera_get_model_name(this->camera, nullptr);
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::hasFeature(yarp::dev::cameraFeature_id_t feature, bool & hasFeature)
#else
bool AravisCamera::hasFeature(int feature, bool * hasFeature)
#endif
{
    yCDebug(ARV) << "Checking feature availability:" << static_cast<int>(feature);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (auto f = static_cast<int>(feature);
        f < static_cast<int>(yarp::dev::cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS) || f > static_cast<int>(yarp::dev::cameraFeature_id_t::YARP_FEATURE_NUMBER_OF) - 1)
#else
    if (auto f = static_cast<int>(feature); f < YARP_FEATURE_BRIGHTNESS || f > YARP_FEATURE_NUMBER_OF - 1)
#endif
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "not supported by YARP";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info || !info->featureName)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        hasFeature = false;
        return yarp::dev::ReturnValue_ok;
#else
        *hasFeature = false;
        return true;
#endif
    }

    auto * device = arv_camera_get_device(camera);

    if (!device)
    {
        yCError(ARV) << "Camera device not available";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    hasFeature = (arv_device_get_feature(device, info->featureName) != nullptr);
#else
    *hasFeature = (arv_device_get_feature(device, info->featureName) != nullptr);
#endif

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (!hasFeature)
#else
    if (!*hasFeature)
#endif
    {
        yCWarning(ARV) << "Feature" << info->featureName << "not found in camera";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_ok;
#else
        return true;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (bool compatible; !checkEnabled(feature, compatible) || !compatible)
#else
    if (bool compatible; !checkEnabled(feature, &compatible) || !compatible)
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        hasFeature = false;
#else
        *hasFeature = false;
#endif
        yCWarning(ARV) << "Feature" << info->featureName << "not in camera but not compatible for pixelFormat";
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::setFeature(yarp::dev::cameraFeature_id_t feature, double value)
#else
bool AravisCamera::setFeature(int feature, double value)
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

    yCDebug(ARV) << "Setting feature" << info->featureName << "to value" << value;

    // 1. Check feature compatibility with current pixel format
    bool compatible;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (!checkEnabled(feature, compatible))
#else
    if (!checkEnabled(f, &compatible))
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    if (!compatible)
    {
        yCError(ARV) << "Feature" << info->featureName << "not compatible with current pixel format";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    // 2. Check if feature is in auto mode
    if (info->autoName)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        if (yarp::dev::FeatureMode currentMode; getMode(feature, currentMode) && currentMode == yarp::dev::FeatureMode::MODE_AUTO)
#else
        if (FeatureMode currentMode; getMode(f, &currentMode) && currentMode == MODE_AUTO)
#endif
        {
            yCWarning(ARV) << "Feature" << info->featureName << "is in auto mode, manual change ignored";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
        }
    }

    // 3. Activate feature if disabled
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (bool hasauto = false; hasAuto(feature, hasauto) && info->enabledName && !hasauto)
#else
    if (bool hasauto = false; hasAuto(f, &hasauto) && info->enabledName && !hasauto)
#endif
    {
        bool isActive;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        if (!getActive(feature, isActive))
#else
        if (!getActive(f, &isActive))
#endif
        {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
        }

        if (!isActive)
        {
            yCInfo(ARV) << "Auto-enabling feature" << info->featureName;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
            if (!setActive(feature, true))
#else
            if (!setActive(f, true))
#endif
            {
                yCError(ARV) << "Failed to enable feature" << info->featureName;
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
            }
        }
    }

    // 4. Set the value
    ArvDevice * device = arv_camera_get_device(camera);
    GError * error = nullptr;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (yarp_arv_int_feature_map.count(feature))
#else
    if (yarp_arv_int_feature_map.count(f))
#endif
    {
        arv_device_set_integer_feature_value(device, info->featureName, static_cast<gint64>(value), &error);
    }
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    else if (yarp_arv_float_feature_map.count(feature))
#else
    else if (yarp_arv_float_feature_map.count(f))
#endif
    {
        arv_device_set_float_feature_value(device, info->featureName, static_cast<gdouble>(value), &error);
    }
    else
    {
        yCError(ARV) << "Feature type not recognized";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    if (error)
    {
        yCError(ARV, "Error setting feature %s: %s", info->featureName, error->message);
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::getFeature(yarp::dev::cameraFeature_id_t feature, double & value)
#else
bool AravisCamera::getFeature(int feature, double * value)
#endif
{
    yCDebug(ARV) << "Getting value for feature" << static_cast<int>(feature);

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

    GError * error = nullptr;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (yarp_arv_int_feature_map.count(feature))
#else
    if (yarp_arv_int_feature_map.count(f))
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        value = arv_device_get_integer_feature_value(device, info->featureName, &error);
#else
        *value = arv_device_get_integer_feature_value(device, info->featureName, &error);
#endif
    }
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    else if (yarp_arv_float_feature_map.count(feature))
#else
    else if (yarp_arv_float_feature_map.count(f))
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        value = arv_device_get_float_feature_value(device, info->featureName, &error);
#else
        *value = arv_device_get_float_feature_value(device, info->featureName, &error);
#endif
    }
    else
    {
        yCError(ARV) << "Feature type not recognized";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    if (error)
    {
        yCError(ARV, "Error getting feature %s: %s", info->featureName, error->message);
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yCDebug(ARV) << "Feature" << info->featureName << "value:" << value;
    return yarp::dev::ReturnValue_ok;
#else
    yCDebug(ARV) << "Feature" << info->featureName << "value:" << *value;
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::setFeature(yarp::dev::cameraFeature_id_t feature, double value1, double value2)
#else
bool AravisCamera::setFeature(int feature, double value1, double value2)
#endif
{
    yCError(ARV) << "No features with 2 values supported!";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_method_failed;
#else
    return false;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::getFeature(yarp::dev::cameraFeature_id_t feature, double & value1, double & value2)
#else
bool AravisCamera::getFeature(int feature, double * value1, double * value2)
#endif
{
    return getFeatureLimits(feature, value1, value2);
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::hasOnOff(yarp::dev::cameraFeature_id_t feature, bool & hasOnOff)
#else
bool AravisCamera::hasOnOff(int feature, bool * hasOnOff)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info)
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "not supported";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    hasOnOff = (info->enabledName != nullptr);
    return yarp::dev::ReturnValue_ok;
#else
    *hasOnOff = (info->enabledName != nullptr);
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::setActive(yarp::dev::cameraFeature_id_t feature, bool onoff)
#else
bool AravisCamera::setActive(int feature, bool onoff)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info || !info->enabledName)
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "does not support on/off";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    yCDebug(ARV) << "Setting feature" << static_cast<int>(feature) << info->featureName << (onoff ? "ON" : "OFF");

    ArvDevice * device = arv_camera_get_device(camera);

    if (!device) {
        yCError(ARV) << "Camera device not available";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    GError * error = nullptr;
    arv_device_set_boolean_feature_value(device, info->enabledName, onoff, &error);

    if (error)
    {
        yCError(ARV, "Error setting on/off for %s: %s", info->featureName, error->message);
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::getActive(yarp::dev::cameraFeature_id_t feature, bool & isActive)
#else
bool AravisCamera::getActive(int feature, bool * isActive)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info || !info->enabledName)
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "does not support on/off";
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

    GError * error = nullptr;
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    isActive = arv_device_get_boolean_feature_value(device, info->enabledName, &error);
#else
    *isActive = arv_device_get_boolean_feature_value(device, info->enabledName, &error);
#endif

    if (error)
    {
        yCError(ARV, "Error getting on/off for %s: %s", info->featureName, error->message);
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yCDebug(ARV) << "Feature" << static_cast<int>(feature) << info->featureName << "is" << (isActive ? "ON" : "OFF");
    return yarp::dev::ReturnValue_ok;
#else
    yCDebug(ARV) << "Feature" << info->featureName << "is" << (*isActive ? "ON" : "OFF");
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::hasAuto(yarp::dev::cameraFeature_id_t feature, bool & hasAuto)
#else
bool AravisCamera::hasAuto(int feature, bool * hasAuto)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info)
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

    GError * error = nullptr;

    if (arv_camera_is_feature_available(camera, info->autoName, &error)==1 && arv_device_get_feature(device, info->autoName)!=0)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        hasAuto = true;
        return yarp::dev::ReturnValue_ok;
#else
        *hasAuto = true;
        return true;
#endif
    }

    if (error)
    {
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        hasAuto = false;
        return yarp::dev::ReturnValue_error_method_failed;
#else
        *hasAuto = false;
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    hasAuto = false;
    return yarp::dev::ReturnValue_ok;
#else
    *hasAuto = false;
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::hasManual(yarp::dev::cameraFeature_id_t feature, bool & hasManual)
#else
bool AravisCamera::hasManual(int feature, bool * hasManual)
#endif
{
    // All numeric features support manual mode by default
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    hasManual = (yarp_arv_int_feature_map.count(feature) || yarp_arv_float_feature_map.count(feature));
    return yarp::dev::ReturnValue_ok;
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    *hasManual = (yarp_arv_int_feature_map.count(f) || yarp_arv_float_feature_map.count(f));
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::hasOnePush(yarp::dev::cameraFeature_id_t feature, bool & hasOnePush)
#else
bool AravisCamera::hasOnePush(int feature, bool * hasOnePush)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const FeatureInfo * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const FeatureInfo * info = getFeatureInfo(f);
#endif

    if (!info)
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "not supported";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    hasOnePush = info->supportsOnePush;
    return yarp::dev::ReturnValue_ok;
#else
    *hasOnePush = info->supportsOnePush;
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::setMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode mode)
#else
bool AravisCamera::setMode(int feature, FeatureMode mode)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info || !info->autoName)
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "does not support auto modes";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    yCDebug(ARV) << "Setting mode" << static_cast<int>(mode) << "for feature" << info->featureName;

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

    const char * mode_str = nullptr;

    switch (mode)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        case yarp::dev::FeatureMode::MODE_AUTO:     mode_str = "Continuous"; break;
        case yarp::dev::FeatureMode::MODE_MANUAL:   mode_str = "Off"; break;
#else
        case MODE_AUTO:     mode_str = "Continuous"; break;
        case MODE_MANUAL:   mode_str = "Off"; break;
#endif
        default:
            yCError(ARV) << "Invalid mode specified";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    GError * error = nullptr;
    arv_device_set_string_feature_value(device, info->autoName, mode_str, &error);

    if (error)
    {
        yCError(ARV, "Failed to set mode for %s: %s", info->featureName, error->message);
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::getMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode & mode)
#else
bool AravisCamera::getMode(int feature, FeatureMode * mode)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const auto * info = getFeatureInfo(feature);
#else
    auto f = static_cast<cameraFeature_id_t>(feature);
    const auto * info = getFeatureInfo(f);
#endif

    if (!info)
    {
        yCError(ARV) << "Feature" << static_cast<int>(feature) << "not supported";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    // If no auto mode, it's manual by default
    if (!info->autoName)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        mode = yarp::dev::FeatureMode::MODE_MANUAL;
        return yarp::dev::ReturnValue_ok;
#else
        *mode = MODE_MANUAL;
        return true;
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

    GError * error = nullptr;
    const char * current_mode = arv_device_get_string_feature_value(device, info->autoName, &error);

    if (error)
    {
        yCError(ARV, "Failed to get mode for %s: %s", info->featureName, error->message);
        g_error_free(error);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    if (!current_mode)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        mode = yarp::dev::FeatureMode::MODE_MANUAL;
        return yarp::dev::ReturnValue_ok;
#else
        *mode = MODE_MANUAL;
        return true;
#endif
    }

    if (strcmp(current_mode, "Continuous") == 0)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        mode = yarp::dev::FeatureMode::MODE_AUTO;
#else
        *mode = MODE_AUTO;
#endif
    }
    else if (strcmp(current_mode, "Once") == 0)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        mode = yarp::dev::FeatureMode::MODE_AUTO;
#else
        *mode = MODE_AUTO;
#endif
    }
    else if (strcmp(current_mode, "Off") == 0)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        mode = yarp::dev::FeatureMode::MODE_MANUAL;
#else
        *mode = MODE_MANUAL;
#endif
    }
    else
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        mode = yarp::dev::FeatureMode::MODE_UNKNOWN;
#else
        *mode = MODE_UNKNOWN;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue AravisCamera::setOnePush(yarp::dev::cameraFeature_id_t feature)
{
    if (!setMode(feature, yarp::dev::FeatureMode::MODE_AUTO))
    {
        return yarp::dev::ReturnValue_error_method_failed;
    }

    return setMode(feature, yarp::dev::FeatureMode::MODE_MANUAL);
}
#else
bool AravisCamera::setOnePush(int feature)
{
    if (!setMode(feature, MODE_AUTO))
    {
        return false;
    }

    return setMode(feature, MODE_MANUAL);
}
#endif
