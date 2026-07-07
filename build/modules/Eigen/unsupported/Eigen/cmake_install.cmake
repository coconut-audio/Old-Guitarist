# Install script for directory: /home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/AdolcForward"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/AlignedVector3"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/ArpackSupport"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/AutoDiff"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/BVH"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/EulerAngles"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/FFT"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/GPU"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/IterativeSolvers"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/KroneckerProduct"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/LevenbergMarquardt"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/MatrixFunctions"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/MPRealSupport"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/NNLS"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/NonLinearOptimization"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/NumericalDiff"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/OpenGLSupport"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/Polynomials"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/SparseExtra"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/SpecialFunctions"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/Splines"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/StructuredMatrices"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/Tensor"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/TensorSymmetry"
    "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/ThreadPool"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/home/vivekv/Desktop/Old-Guitarist/modules/Eigen/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/vivekv/Desktop/Old-Guitarist/build/modules/Eigen/unsupported/Eigen/CXX11/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/vivekv/Desktop/Old-Guitarist/build/modules/Eigen/unsupported/Eigen/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
