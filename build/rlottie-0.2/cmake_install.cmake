# Install script for directory: D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/rlottie-0.2

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/install")
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
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/App/MinGW/mingw64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/rlottie.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/rlottie-0.2/inc/rlottie.h"
    "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/rlottie-0.2/inc/rlottie_capi.h"
    "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/rlottie-0.2/inc/rlottiecommon.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/rlottie.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/rlottie/rlottieTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/rlottie/rlottieTargets.cmake"
         "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/CMakeFiles/Export/30f7f9dd91d25cb9c34981dcba09a828/rlottieTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/rlottie/rlottieTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/rlottie/rlottieTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/rlottie" TYPE FILE FILES "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/CMakeFiles/Export/30f7f9dd91d25cb9c34981dcba09a828/rlottieTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/rlottie" TYPE FILE FILES "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/CMakeFiles/Export/30f7f9dd91d25cb9c34981dcba09a828/rlottieTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/rlottie" TYPE FILE FILES
    "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/rlottieConfig.cmake"
    "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/rlottieConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/inc/cmake_install.cmake")
  include("D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/src/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/document/code/stm32/H7/h7rec/External/keil_ac6_rlottie/build/rlottie-0.2/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
