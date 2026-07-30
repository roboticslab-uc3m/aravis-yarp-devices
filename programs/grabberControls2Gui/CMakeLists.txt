if(NOT Python3_FOUND AND (NOT DEFINED ENABLE_grabbercontrols2gui OR ENABLE_grabbercontrols2gui))
    message(WARNING "Python3 interpreter not found or unsupported version (only 3.5+), disabling grabbercontrols2gui program")
endif()

cmake_dependent_option(ENABLE_grabbercontrols2gui "Enable/disable grabbercontrols2gui program" ON
                       Python3_FOUND OFF)

if(ENABLE_grabbercontrols2gui)

    configure_file("${CMAKE_SOURCE_DIR}/cmake/templates/setup.py.in"
                   "${CMAKE_BINARY_DIR}/setup.py")

    install(CODE "execute_process(COMMAND \"${Python3_EXECUTABLE}\" -m pip install --break-system-packages \"${CMAKE_BINARY_DIR}\")")

else()

    set(ENABLE_grabbercontrols2gui OFF CACHE BOOL "Enable/disable grabbercontrols2gui program" FORCE)

endif()
