# Installation from Source Code

First install the mandatory dependencies:
- [Install CMake 3.19+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-cmake.md/)
- [Install YCM 0.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-ycm.md/)
- [Install YARP 3.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-yarp.md/)

### Components with known additional/specific dependencies

- [../libraries/YarpPlugins/AravisCamera](../libraries/YarpPlugins/AravisCamera#requirements)
- [../programs/grabberControls2Gui](../programs/grabberControls2Gui#requirements)

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
