if(NOT Python3_FOUND AND (NOT DEFINED ENABLE_armTargetFollower OR ENABLE_armTargetFollower))
    message(WARNING "Python3 interpreter not found or unsupported version (only 3.5+), disabling armTargetFollower program")
endif()

cmake_dependent_option(ENABLE_armTargetFollower "Enable/disable armTargetFollower program" ON
                       Python3_FOUND OFF)

if(NOT ENABLE_armTargetFollower)
    set(ENABLE_armTargetFollower OFF CACHE BOOL "Enable/disable armTargetFollower program" FORCE)
endif()
