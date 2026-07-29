if(NOT Aravis_FOUND AND (NOT DEFINED ENABLE_AravisGigE OR ENABLE_AravisGigE))
    message(WARNING "Aravis package not found, disabling AravisGigE")
endif()

find_package(OpenCV REQUIRED imgproc)

yarp_prepare_plugin(AravisGigE
                    CATEGORY device
                    TYPE AravisGigE
                    INCLUDE AravisGigE.hpp
                    DEFAULT ON
                    DEPENDS Aravis_FOUND
                    EXTRA_CONFIG WRAPPER=frameGrabber_nws_yarp)

if(NOT SKIP_AravisGigE)

    yarp_add_plugin(AravisGigE)

    target_sources(AravisGigE PRIVATE AravisGigE.hpp
                                      AravisGigE.cpp
                                      DeviceDriverImpl.cpp
                                      IFrameGrabberControlsImpl.cpp
                                      IFrameGrabberImageRawImpl.cpp
                                      IFrameGrabberImageImpl.cpp
                                      LogComponent.hpp
                                      LogComponent.cpp
                                      TerminalComponent.cpp)

    target_link_libraries(AravisGigE YARP::YARP_os
                                     YARP::YARP_dev
                                     PkgConfig::Aravis
                                     opencv_imgproc)

    yarp_install(TARGETS AravisGigE
                 LIBRARY DESTINATION ${ARAVIS-YARP-DEVICES_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ARAVIS-YARP-DEVICES_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ARAVIS-YARP-DEVICES_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_AravisGigE OFF CACHE BOOL "Enable/disable AravisGigE device" FORCE)

endif()
