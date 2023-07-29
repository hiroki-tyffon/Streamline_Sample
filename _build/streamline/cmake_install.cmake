# Install script for directory: C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_build/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.common.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.interposer.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.dlss.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/nvngx_dlss.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.imgui.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.reflex.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/NvLowLatencyVk.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.nis.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.dlss_g.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/nvngx_dlssg.dll;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.common.json;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.interposer.json;C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin/sl.reflex.json")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/_package/_bin" TYPE FILE FILES
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/sl.common.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/sl.interposer.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/sl.dlss.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/nvngx_dlss.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/sl.imgui.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/sl.reflex.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/NvLowLatencyVk.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/sl.nis.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/sl.dlss_g.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64/nvngx_dlssg.dll"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64//sl.common.json"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64//sl.interposer.json"
    "C:/Tyffon/Repositories/ExpoC/Streamline_Sample/streamline/bin/x64//sl.reflex.json"
    )
endif()

