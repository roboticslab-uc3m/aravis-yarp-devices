if(NOT Aravis_FOUND AND (NOT DEFINED ENABLE_AravisCamera OR ENABLE_AravisCamera))
    message(WARNING "Aravis package not found, disabling AravisCamera")
endif()

yarp_prepare_plugin(AravisCamera
                    CATEGORY device
                    TYPE AravisCamera
                    INCLUDE AravisCamera.hpp
                    DEFAULT ON
                    DEPENDS Aravis_FOUND
                    EXTRA_CONFIG WRAPPER=frameGrabber_nws_yarp)

if(NOT SKIP_AravisCamera)

    yarp_add_plugin(AravisCamera)

    target_sources(AravisCamera PRIVATE AravisCamera.hpp
                                        AravisCamera.cpp
                                        DeviceDriverImpl.cpp
                                        IFrameGrabberControlsImpl.cpp
                                        IFrameGrabberImageRawImpl.cpp
                                        LogComponent.hpp
                                        LogComponent.cpp
                                        TerminalComponent.cpp)

    target_link_libraries(AravisCamera YARP::YARP_os
                                       YARP::YARP_dev
                                       PkgConfig::Aravis)

    if(TARGET opencv_imgproc)
        message(STATUS "OpenCV imgproc module found, enabling color support in AravisCamera")
        target_link_libraries(AravisCamera opencv_imgproc)
        target_compile_definitions(AravisCamera PRIVATE HAVE_OPENCV)
        target_sources(AravisCamera PRIVATE IFrameGrabberImageImpl.cpp)
    else()
        message(STATUS "OpenCV imgproc module not found, disabling color support in AravisCamera")
    endif()

    yarp_install(TARGETS AravisCamera
                 LIBRARY DESTINATION ${ARAVIS-YARP-DEVICES_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ARAVIS-YARP-DEVICES_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ARAVIS-YARP-DEVICES_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_AravisCamera OFF CACHE BOOL "Enable/disable AravisCamera device" FORCE)

endif()
