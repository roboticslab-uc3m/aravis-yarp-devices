#ifndef __ARAVIS_CAMERA_HPP__
#define __ARAVIS_CAMERA_HPP__

#include <map>
#include <set>

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
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono> & image) override;
#ifdef HAVE_OPENCV
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;
#endif
    int height() const override;
    int width() const override;

    // ---------- IFrameGrabberControls Declarations. Implementation in IFrameGrabberControlsImpl.cpp ---------
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

private:
    void runInteractiveTerminal();
    bool getFeatureLimits(int feature, double * min, double * max);
    bool checkEnabled(cameraFeature_id_t feature, bool * compatible);
    const FeatureInfo * getFeatureInfo(cameraFeature_id_t feature);
    void printFeatureInfo(cameraFeature_id_t featureId, const FeatureInfo & info);
    void listAvailableFeatures();
    bool checkFeatureExistenceAndGetValue(const std::string & featureName, double & value);
    cameraFeature_id_t id_find(const std::string & feature_name);
    bool getAvailablePixelFormats(std::set<std::string> & availablePixelFormats);
    void * getFrameBuffer();

    ArvCamera * camera {nullptr};
    ArvStream * stream {nullptr};

    ArvPixelFormat pixelFormat {0};

    int _width {0};
    int _height {0};

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
};

#endif // __ARAVIS_CAMERA_HPP__
