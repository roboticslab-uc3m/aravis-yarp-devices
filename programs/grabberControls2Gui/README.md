# grabberControls2Gui

## Requirements

Depends on:
- Python 3+
- [setuptools using pip3](https://robots.uc3m.es/installation-guides/install-setuptools.html#install-setuptools-using-pip3)
- [YARP with Python 3 bindings](https://robots.uc3m.es/installation-guides/install-yarp.html#install-python-bindings)

## Usage

To use the GUI app, you will need to be already running an instance of `yarp server` and the [AravisCamera](/libraries/YarpPlugins/AravisCamera) device. Once both are up and running, you can simply call the `grabbercontrols2gui` app:

```bash
grabbercontrols2gui
```

By default, it will try to connect to `/grabber`. If the port for the [AravisCamera](/libraries/YarpPlugins/AravisCamera) device is not `/grabber`, you can specify it when launching `grabbercontrols2gui`:

```bash
grabbercontrols2gui --remote-port /whatever_port_you_want
```
