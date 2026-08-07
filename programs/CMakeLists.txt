add_subdirectory(armTargetFollower)
add_subdirectory(grabberControls2Gui)

if(ENABLE_armTargetFollower OR ENABLE_grabberControls2Gui)
    configure_file("${CMAKE_SOURCE_DIR}/cmake/templates/setup.py.in"
                   "${CMAKE_BINARY_DIR}/setup.py")

    install(CODE "execute_process(COMMAND \"${Python3_EXECUTABLE}\" -m pip install --break-system-packages \"${CMAKE_BINARY_DIR}\")")
endif()
