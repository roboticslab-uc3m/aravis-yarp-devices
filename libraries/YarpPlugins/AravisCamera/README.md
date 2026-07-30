# AravisCamera device

To use this YARP device, a USB3/GigE camera is required. Once connected, use the following commands to control the camera and receive images.

## Requirements

Depends on:
- [Aravis 0.8](https://robots.uc3m.es/installation-guides/install-aravis.html)
- [OpenCV 4.6](https://robots.uc3m.es/installation-guides/install-opencv.html) (color frames)

## Launching the device

To run the device and connect to the camera, simply run:

```
yarpdev --device AravisCamera
```

If you want to test the device without an actual camera, you can use a fake Aravis camera:

```
yarpdev --device AravisCamera --fake
```

To use a different pixel format, e.g. 8-bit grayscale or YUV422:

```
yarpdev --device AravisCamera --pixelFormat Mono8 --capabilities RAW
```

```
yarpdev --device AravisCamera --pixelFormat YCbCr422_8_CbYCrY --capabilities COLOR
```

By default, the wrapper assumes you want color frames, therefore in that case you can omit `--capabilities`:

```
yarpdev --device AravisCamera --pixelFormat BayerRG8
```


To check all available Aravis pixel formats (your camera probably supports only a subset of them), run:

```
yarpdev --device AravisCamera --introspection
```

## Obtaining a color image from the device in RAW mode

This YARP device exposes two modes through the [frameGrabber_nws_yarp](https://www.yarp.it/latest/classFrameGrabber__nws__yarp__ParamsParser.html#details) network wrapper: grayscale (`--capabilities RAW`) and RGB (`--capabilities COLOR`, only if compiled with OpenCV support). To obtain a color image in grayscale mode, the stream has to be connected using the [Bayer carrier](https://www.yarp.it/latest/group__carrier__config.html#carrier_config_bayer) to interpret the raw image as an RGB image. Given an `AravisCamera` device named `/grabber` and an input port named `/v` (from a viewer, for instance), the command to run to connect them is:

```
yarp connect /grabber /v udp+recv.bayer+order.bggr
```

Note: this method is probably less efficient than just simply using the COLOR mode directly.

## Camera parameters control

The control of the camera parameters is performed from the image port (`/grabber` by default ), through a [RPC interface](https://www.yarp.it/latest/rpc_ports.html).

```
yarp rpc /grabber
```

Once there, one can send commands to the camera. The most common commands are: `has`, `get` and `set`.

### `has`

With the `has` command one can query the device if it has some property. For instance, to check if the camera has zoom controls:

```
fgc has feat 16
```

### `get`

With the `get` command one can query the *value* of some property. For instance, to check the current gain value:

```
fgc get feat 9
```

### `set`

With the `set` command one can change the *value* of some property. For instance, to set the zoom to the maximum value:

```
fgc set feat 16 100
```

### Available features

These are the features currently available in YARP. To check which ones are supported by the camera, the `has` command can be used:

| Feature | Enum name | Enum value |
| --- | --- | ---|
| Brightness | `YARP_FEATURE_BRIGHTNESS` | 0 |
| Exposure | `YARP_FEATURE_EXPOSURE` | 1 |
| Sharpness | `YARP_FEATURE_SHARPNESS` | 2 |
| White Balance | `YARP_FEATURE_WHITE_BALANCE` | 3 |
| Hue | `YARP_FEATURE_HUE` | 4 |
| Saturation | `YARP_FEATURE_SATURATION` | 5 |
| Gamma | `YARP_FEATURE_GAMMA` | 6 |
| Shutter | `YARP_FEATURE_SHUTTER` | 7 |
| Gain | `YARP_FEATURE_GAIN` | 8 |
| Iris | `YARP_FEATURE_IRIS` | 9 |
| Focus | `YARP_FEATURE_FOCUS` | 10 |
| Temperature | `YARP_FEATURE_TEMPERATURE` | 11 |
| Trigger | `YARP_FEATURE_TRIGGER` | 12 |
| Trigger delay | `YARP_FEATURE_TRIGGER_DELAY` | 13 |
| White Shading | `YARP_FEATURE_WHITE_SHADING` | 14 |
| Frame Rate | `YARP_FEATURE_FRAME_RATE` | 15 |
| Zoom | `YARP_FEATURE_ZOOM` | 16 |
| Pan | `YARP_FEATURE_PAN` | 17 |
| Tilt | `YARP_FEATURE_TILT` | 18 |
| Optical Filter | `YARP_FEATURE_OPTICAL_FILTER` | 19 |
| Capture size | `YARP_FEATURE_CAPTURE_SIZE` | 20 |
| Capture quality | `YARP_FEATURE_CAPTURE_QUALITY` | 21 |
| Mirror | `YARP_FEATURE_MIRROR` | 22 |
| Number of features | `YARP_FEATURE_NUMBER_OF` | 23 |

## FAQ

### I can receive an image, but it is all dark, what can I do?

This is probably due to a bad configuration of the camera parameters. Try to increase the gain or exposure until the image starts looking brighter. For our GigE camera, some values that work great are:

```
Gain: 10
Exposure: 32000
```

### I cannot receive a color image, but I receive a grey image with a regular point pattern on it.

What you are receiving is the raw image of the camera. To obtain a color image from it you need to either launch the device with the appropriate capabilities or decode the image using a Bayer filter. Follow the steps in the section [Obtaining a color image from the device](#obtaining-a-color-image-from-the-device) in this very same guide to fix it.

## Useful links

* [GrabberControls2GuiGUI](/programs/grabberControls2Gui) program
