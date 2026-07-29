# Installation from Source Code

First install the dependencies:
- [Install CMake 3.19+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-cmake.md/)
- [Install YCM 0.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-ycm.md/)
- [Install YARP 3.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-yarp.md/)

For unit testing, you'll need the googletest source package. Refer to [Install googletest](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-googletest.md/).

## Installation (Ubuntu)

Once the required dependencies have been installed, the code has to be compiled and installed. Note that you will be prompted for your password upon using `sudo` a couple of times:

```bash
cd  # go home
mkdir -p repos; cd repos  # make $HOME/repos if it doesn't exist; then, enter it
git clone https://github.com/roboticslab-uc3m/aravis-yarp-devices.git  # Download aravis-yarp-devices software from the repository
cd aravis-yarp-devices; mkdir build; cd build; cmake ..  # Configure the aravis-yarp-devices software
make -j$(nproc)  # Compile
sudo make install  # Install :-)
sudo ldconfig  # Just in case
```

Remember to enable the devices you want to compile using `ccmake` instead of `cmake`.

### Useful info to install GrabberControls2Gui

When installing GrabberControls2Gui, take into account the following points:

* GrabberControls2Gui requires Python 3+ with custom yarp Python bindings installed. Currently, installing them is not an easy task (see [comment348230791@roboticslab-uc3m/yarp-devices#145](https://github.com/roboticslab-uc3m/yarp-devices/issues/145#issuecomment-348230791) and [roboticslab-uc3m/installation-guides#26](https://github.com/roboticslab-uc3m/installation-guides/issues/26)) but we expect this to change in the future.

* Setup.py should take care of automatically installing the remaining dependencies for AravisGigEController (`sudo python3 setup.py install`).
