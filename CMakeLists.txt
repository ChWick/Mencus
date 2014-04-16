cmake_minimum_required(VERSION 2.8)

project(Mencus)

# set version
set (MENCUS_VERSION_CODE 9)
set (MENCUS_VERSION_NAME 0.0.9)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set (MENCUS_TEMPLATES_DIR "${CMAKE_MODULE_PATH}/templates")
#include(cotire)

set (MENCUS_STATIC_DEPENDENCIES FALSE)
if (ANDROID)
  message("Using static dependencies. Android needs them.")
  set (MENCUS_STATIC_DEPENDENCIES TRUE)
endif()

find_package(Ogre)
find_package(OIS)
find_package(CEGUI)
find_package(tinyxml2)

if(ANDROID)
  include(toolchain/AndroidGame)
else()
  add_subdirectory(Game)
  add_subdirectory(FileConverter)
  #add_subdirectory(MapEditor)
endif()

