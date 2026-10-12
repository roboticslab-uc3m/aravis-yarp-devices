# FIXME: workaround for YARP 3.12.
set(ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE ON)

option(ALLOW_DEVICE_PARAM_PARSER_GENERATION "Allow YARP to (re)build device param parsers"
       ${ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE})

# YARP devices.
add_subdirectory(AravisCamera)
