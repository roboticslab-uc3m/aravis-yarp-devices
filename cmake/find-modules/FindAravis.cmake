# Try to find Aravis. Once done, this will define:
#  Aravis_FOUND - System has Aravis
#  Aravis_INCLUDE_DIRS - The Aravis include directories
#  Aravis_LIBRARIES - The libraries needed to use Aravis
#  Aravis_LIBRARY_DIRS - The library directories
#  Aravis_VERSION - The version of Aravis package
# Also, the CMake target PkgConfig::Aravis will be created if found.

find_package(PkgConfig)

pkg_search_module(Aravis IMPORTED_TARGET
                  aravis>=0.8 aravis-0.8 aravis-0.9 aravis-0.10)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Aravis REQUIRED_VARS Aravis_INCLUDE_DIRS
                                                       Aravis_LIBRARIES
                                                       Aravis_LIBRARY_DIRS
                                         VERSION_VAR Aravis_VERSION)
