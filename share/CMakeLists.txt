yarp_install(FILES applications/armTargetFollower.xml
             DESTINATION ${ARAVIS-YARP-DEVICES_APPLICATIONS_INSTALL_DIR})

yarp_install(DIRECTORY contexts/
             DESTINATION ${ARAVIS-YARP-DEVICES_CONTEXTS_INSTALL_DIR})

yarp_install(DIRECTORY gazebo/
             DESTINATION share/gazebo)
