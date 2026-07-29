if(NOT Aravis_FOUND AND (NOT DEFINED ENABLE_AravisGigE OR ENABLE_AravisGigE))
    message(WARNING "Aravis package not found, disabling AravisGigE")
endif()

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
                                      LogComponent.hpp
                                      LogComponent.cpp
                                      TerminalComponent.cpp)

    target_link_libraries(AravisGigE YARP::YARP_os
                                     YARP::YARP_dev
                                     PkgConfig::Aravis)

    if(TARGET opencv_imgproc)
        message(STATUS "OpenCV imgproc module found, enabling color support in AravisGigE")
        target_link_libraries(AravisGigE opencv_imgproc)
        target_compile_definitions(AravisGigE PRIVATE HAVE_OPENCV)
        target_sources(AravisGigE PRIVATE IFrameGrabberImageImpl.cpp)
    else()
        message(STATUS "OpenCV imgproc module not found, disabling color support in AravisGigE")
    endif()

    yarp_install(TARGETS AravisGigE
                 LIBRARY DESTINATION ${ARAVIS-YARP-DEVICES_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ARAVIS-YARP-DEVICES_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ARAVIS-YARP-DEVICES_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_AravisGigE OFF CACHE BOOL "Enable/disable AravisGigE device" FORCE)

endif()
