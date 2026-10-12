#ifndef __ARAVIS_CAMERA_HPP__
#define __ARAVIS_CAMERA_HPP__

#include <map>
#include <set>

#include <yarp/conf/version.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberImage.h>

#include <arv.h>

struct FeatureInfo
{
    const char * featureName;
    const char * enabledName;
    const char * autoName;
    bool supportsOnePush;
};

/**
 * @ingroup YarpPlugins
 * @defgroup AravisCamera
 * @brief Contains AravisCamera.
 */

 /**
  * @ingroup AravisCamera
  * @brief Implementation for USB3/GigE cameras using Aravis as driver.
  */
class AravisCamera : public yarp::dev::DeviceDriver,
#ifdef HAVE_OPENCV
                     public yarp::dev::IFrameGrabberImage,
#endif
                     public yarp::dev::IFrameGrabberImageRaw,
                     public yarp::dev::IFrameGrabberControls
{
public:
    //  --------- DeviceDriver Declarations. Implementation in DeviceDriverImpl.cpp ---------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

    //  --------- IFrameGrabberImageOf<> Declarations. Implementation in IFrameGrabberImage(Raw)Impl.cpp ---------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> & image) override;
#else
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> & image) override;
#endif
#ifdef HAVE_OPENCV
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;
#else
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;
#endif
#endif // HAVE_OPENCV
    int height() const override;
    int width() const override;

    // ---------- IFrameGrabberControls Declarations. Implementation in IFrameGrabberControlsImpl.cpp ---------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getCameraDescription(yarp::dev::CameraDescriptor & camera) override;
    yarp::dev::ReturnValue hasFeature(yarp::dev::cameraFeature_id_t feature, bool & hasFeature) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double & value) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value1, double value2) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double & value1, double & value2) override;
    yarp::dev::ReturnValue hasOnOff(yarp::dev::cameraFeature_id_t feature, bool & hasOnOff) override;
    yarp::dev::ReturnValue setActive(yarp::dev::cameraFeature_id_t feature, bool onoff) override;
    yarp::dev::ReturnValue getActive(yarp::dev::cameraFeature_id_t feature, bool & isActive) override;
    yarp::dev::ReturnValue hasAuto(yarp::dev::cameraFeature_id_t feature, bool & hasAuto) override;
    yarp::dev::ReturnValue hasManual(yarp::dev::cameraFeature_id_t feature, bool & hasManual) override;
    yarp::dev::ReturnValue hasOnePush(yarp::dev::cameraFeature_id_t feature, bool & hasOnePush) override;
    yarp::dev::ReturnValue setMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode mode) override;
    yarp::dev::ReturnValue getMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode & mode) override;
    yarp::dev::ReturnValue setOnePush(yarp::dev::cameraFeature_id_t feature) override;
#else
    bool getCameraDescription(CameraDescriptor * camera) override;
    bool hasFeature(int feature, bool * hasFeature) override;
    bool setFeature(int feature, double value) override;
    bool getFeature(int feature, double * value) override;
    bool setFeature(int feature, double value1, double value2) override;
    bool getFeature(int feature, double * value1, double * value2) override;
    bool hasOnOff(int feature, bool * hasOnOff) override;
    bool setActive(int feature, bool onoff) override;
    bool getActive(int feature, bool * isActive) override;
    bool hasAuto(int feature, bool * hasAuto) override;
    bool hasManual(int feature, bool * hasManual) override;
    bool hasOnePush(int feature, bool * hasOnePush) override;
    bool setMode(int feature, FeatureMode mode) override;
    bool getMode(int feature, FeatureMode * mode) override;
    bool setOnePush(int feature) override;
#endif

private:
    // ---------- Generic helpers. Implementation in IFrameGrabberControlsImpl.cpp ---------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getFeatureLimits(yarp::dev::cameraFeature_id_t feature, double & min, double & max);
    yarp::dev::ReturnValue checkEnabled(yarp::dev::cameraFeature_id_t feature, bool & compatible);
    const FeatureInfo * getFeatureInfo(yarp::dev::cameraFeature_id_t feature);
#else
    bool getFeatureLimits(int feature, double * min, double * max);
    bool checkEnabled(int feature, bool * compatible);
    const FeatureInfo * getFeatureInfo(cameraFeature_id_t feature);
#endif
    bool getAvailablePixelFormats(std::set<std::string> & availablePixelFormats);
    void * getFrameBuffer();

    // ---------- Terminal helpers. Implementation in TerminalComponent.cpp ---------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    void printFeatureInfo(yarp::dev::cameraFeature_id_t featureId, const FeatureInfo & info);
#else
    void printFeatureInfo(cameraFeature_id_t featureId, const FeatureInfo & info);
#endif
    void listAvailableFeatures();
    bool checkFeatureExistenceAndGetValue(const std::string & featureName, double & value);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::cameraFeature_id_t id_find(const std::string & feature_name);
#else
    cameraFeature_id_t id_find(const std::string & feature_name);
#endif
    void runInteractiveTerminal();

    ArvCamera * camera {nullptr};
    ArvStream * stream {nullptr};

    ArvPixelFormat pixelFormat {0};

    int _width {0};
    int _height {0};

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const std::map<yarp::dev::cameraFeature_id_t, FeatureInfo> yarp_arv_int_feature_map = {
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, {"Brightness", "BrightnessEnabled", "BrightnessAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_SHUTTER, {"Shutter", "ShutterEnabled", "ShutterAuto", true}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_IRIS, {"Iris", "IrisEnabled", "IrisAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_FOCUS, {"Focus", "FocusEnabled", "FocusAuto", true}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_TEMPERATURE, {"Temperature", "TemperatureEnabled", nullptr, false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_TRIGGER, {"Trigger", "TriggerEnabled", "TriggerAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_WHITE_SHADING, {"WhiteShading", "WhiteShadingEnabled", nullptr, false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_ZOOM, {"Zoom", "ZoomEnabled", "ZoomAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_PAN, {"Pan", "PanEnabled", "PanAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_TILT, {"Tilt", "TiltEnabled", "TiltAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_SHARPNESS, {"Sharpness", "SharpnessEnabled", "SharpnessAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_OPTICAL_FILTER, {"OpticalFilter", "OpticalFilterEnabled", "OpticalFilter", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_CAPTURE_SIZE, {"CaptureSize", "CaptureSizeEnabled", "CaptureSizeAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_CAPTURE_QUALITY, {"CaptureQuality", "CaptureQualityEnabled", "CaptureQualityAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_MIRROR, {"Mirror", "MirrorEnabled", "MirrorAuto", false}}
    };
#else
    const std::map<cameraFeature_id_t, FeatureInfo> yarp_arv_int_feature_map = {
        {YARP_FEATURE_BRIGHTNESS, {"Brightness", "BrightnessEnabled", "BrightnessAuto", false}},
        {YARP_FEATURE_SHUTTER, {"Shutter", "ShutterEnabled", "ShutterAuto", true}},
        {YARP_FEATURE_IRIS, {"Iris", "IrisEnabled", "IrisAuto", false}},
        {YARP_FEATURE_FOCUS, {"Focus", "FocusEnabled", "FocusAuto", true}},
        {YARP_FEATURE_TEMPERATURE, {"Temperature", "TemperatureEnabled", nullptr, false}},
        {YARP_FEATURE_TRIGGER, {"Trigger", "TriggerEnabled", "TriggerAuto", false}},
        {YARP_FEATURE_WHITE_SHADING, {"WhiteShading", "WhiteShadingEnabled", nullptr, false}},
        {YARP_FEATURE_ZOOM, {"Zoom", "ZoomEnabled", "ZoomAuto", false}},
        {YARP_FEATURE_PAN, {"Pan", "PanEnabled", "PanAuto", false}},
        {YARP_FEATURE_TILT, {"Tilt", "TiltEnabled", "TiltAuto", false}},
        {YARP_FEATURE_SHARPNESS, {"Sharpness", "SharpnessEnabled", "SharpnessAuto", false}},
        {YARP_FEATURE_OPTICAL_FILTER, {"OpticalFilter", "OpticalFilterEnabled", "OpticalFilter", false}},
        {YARP_FEATURE_CAPTURE_SIZE, {"CaptureSize", "CaptureSizeEnabled", "CaptureSizeAuto", false}},
        {YARP_FEATURE_CAPTURE_QUALITY, {"CaptureQuality", "CaptureQualityEnabled", "CaptureQualityAuto", false}},
        {YARP_FEATURE_MIRROR, {"Mirror", "MirrorEnabled", "MirrorAuto", false}}
    };
#endif

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    const std::map<yarp::dev::cameraFeature_id_t, FeatureInfo> yarp_arv_float_feature_map = {
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_EXPOSURE, {"ExposureTime", "ExposureEnabled", "ExposureAuto", true}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_TRIGGER_DELAY, {"TriggerDelay", "TriggerDelayEnabled", "TriggerDelayAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_GAIN, {"Gain", "GainEnabled", "GainAuto", true}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_FRAME_RATE, {"AcquisitionFrameRate", "AcquisitionFrameRateEnabled", "AcquisitionFrameRateAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE, {"BalanceWhite", "BalanceWhiteEnabled", "BalanceWhiteAuto", true}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_HUE, {"Hue", "HueEnabled", "HueAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_SATURATION, {"Saturation", "SaturationEnabled", "SaturationAuto", false}},
        {yarp::dev::cameraFeature_id_t::YARP_FEATURE_GAMMA, {"Gamma", "GammaEnabled", "GammaAuto", false}}
    };
#else
    const std::map<cameraFeature_id_t, FeatureInfo> yarp_arv_float_feature_map = {
        {YARP_FEATURE_EXPOSURE, {"ExposureTime", "ExposureEnabled", "ExposureAuto", true}},
        {YARP_FEATURE_TRIGGER_DELAY, {"TriggerDelay", "TriggerDelayEnabled", "TriggerDelayAuto", false}},
        {YARP_FEATURE_GAIN, {"Gain", "GainEnabled", "GainAuto", true}},
        {YARP_FEATURE_FRAME_RATE, {"AcquisitionFrameRate", "AcquisitionFrameRateEnabled", "AcquisitionFrameRateAuto", false}},
        {YARP_FEATURE_WHITE_BALANCE, {"BalanceWhite", "BalanceWhiteEnabled", "BalanceWhiteAuto", true}},
        {YARP_FEATURE_HUE, {"Hue", "HueEnabled", "HueAuto", false}},
        {YARP_FEATURE_SATURATION, {"Saturation", "SaturationEnabled", "SaturationAuto", false}},
        {YARP_FEATURE_GAMMA, {"Gamma", "GammaEnabled", "GammaAuto", false}}
    };
#endif
};

#endif // __ARAVIS_CAMERA_HPP__
